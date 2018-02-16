#include "StdAfx.h"
#include "mp3infp.h"
#include "Resource.h"

static const int ids[] =
{
	-1,
	-1,
	CLP_NAM,
	CLP_TRACK,
	CLP_ART,
	CLP_PRD,
	CLP_CRD,
	CLP_GNR,
	CLP_CMT,
	0
};

static const int staticWnd[] =
{
	IDC_STATIC_FORMAT_,
	IDC_STATIC_TIME_,
	IDC_STATIC_NAM,
	IDC_STATIC_TRACK,
	IDC_STATIC_ART,
	IDC_STATIC_PRD,
	IDC_STATIC_CRD,
	IDC_STATIC_GNR,
	IDC_STATIC_CMT,
	0
};

static const int editWnd[] =
{
	IDC_STATIC_FORMAT,
	IDC_STATIC_TIME,
	IDC_EDIT_NAM,
	IDC_EDIT_TRACK,
	IDC_EDIT_ART,
	IDC_EDIT_PRD,
	IDC_EDIT_CRD,
	IDC_EDIT_GNR,
	IDC_EDIT_CMT,
	0
};

UINT CALLBACK CShellExt::PageCallback_mp3_APE(HWND hWnd,UINT uMessage,LPPROPSHEETPAGE ppsp)
{
	LPCSHELLEXT	lpcs = (CShellExt *)ppsp->lParam;
	switch(uMessage){
	case PSPCB_CREATE:	//プロパティページが作成されつつある
		return TRUE;
	case PSPCB_RELEASE:	//プロパティページが破壊されつつある
		if(lpcs) 
		{
			lpcs->m_hwndMp3Ape = NULL;
			lpcs->Release();
		}
		return TRUE;
	}

	return TRUE;
}

static void EnableButton(HWND hDlg,CShellExt *lpcs)
{
	if(lpcs->m_Id3tagv1.IsEnable() && lpcs->m_Ape.IsEnable())
	{
		EnableWindow(GetDlgItem(hDlg,IDC_COPY_FROM_ID3V1),TRUE);	//Copy from Id3v1
	}
	else
	{
		EnableWindow(GetDlgItem(hDlg,IDC_COPY_FROM_ID3V1),FALSE);	//Copy from Id3v1
	}
	if(lpcs->m_Id3tagv2.IsEnable() && lpcs->m_Ape.IsEnable())
	{
		EnableWindow(GetDlgItem(hDlg,IDC_COPY_FROM_ID3V2),TRUE);	//Copy from Id3v2
	}
	else
	{
		EnableWindow(GetDlgItem(hDlg,IDC_COPY_FROM_ID3V2),FALSE);	//Copy from Id3v2
	}
}

static void EnableEdit(HWND hDlg,CShellExt *lpcs,BOOL bEnable)
{
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_NAM),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_NAM),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_NAM),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_TRACK),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_TRACK),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_TRACK),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_ART),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_ART),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_ART),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_PRD),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_PRD),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_PRD),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_CRD),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_CRD),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_CRD),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_GNR),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_GNR),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_GNR),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_CMT),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_CMT),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_CMT),bEnable);
	if(bEnable)
	{
		EnableWindow(GetDlgItem(hDlg,IDC_BUTTON_DEL_TAG),TRUE);	//Make Id3Tag
		EnableWindow(GetDlgItem(hDlg,IDC_BUTTON_MAKE_TAG),FALSE);	//Make Id3Tag
		HWND hWndTab = PropSheet_GetTabControl(GetParent(hDlg));
		TC_ITEM tcItem;
		tcItem.mask = TCIF_TEXT;
		tcItem.pszText = _T("APE");
		TabCtrl_SetItem(hWndTab,PropSheet_PageToIndex(GetParent(hDlg),lpcs->m_hpageMp3Ape),&tcItem);
		PropSheet_RecalcPageSizes(GetParent(hDlg));
	}
	else
	{
		EnableWindow(GetDlgItem(hDlg,IDC_BUTTON_DEL_TAG),FALSE);	//Del Id3Tag
		EnableWindow(GetDlgItem(hDlg,IDC_BUTTON_MAKE_TAG),TRUE);	//Make Id3Tag
		HWND hWndTab = PropSheet_GetTabControl(GetParent(hDlg));
		TC_ITEM tcItem;
		tcItem.mask = TCIF_TEXT;
		tcItem.pszText = _T("APE(*)");
		TabCtrl_SetItem(hWndTab,PropSheet_PageToIndex(GetParent(hDlg),lpcs->m_hpageMp3Ape),&tcItem);
		PropSheet_RecalcPageSizes(GetParent(hDlg));
	}

	EnableButton(hDlg,lpcs);
}

static void DispInfo(HWND hDlg,CShellExt *lpcs)
{
	//mpeg情報
	SetDlgItemText(hDlg,IDC_STATIC_FORMAT,lpcs->m_mp3info.GetFormatString());
	SetDlgItemText(hDlg,IDC_STATIC_TIME,lpcs->m_mp3info.GetTimeString());

	if(lpcs->m_Ape.IsEnable())
	{
		EnableEdit(hDlg,lpcs,TRUE);

		CString tmp;
		lpcs->m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_TITLE,tmp);
		SetDlgItemText(hDlg,IDC_EDIT_NAM,tmp);

		lpcs->m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_TRACK,tmp);
		SetDlgItemText(hDlg,IDC_EDIT_TRACK,tmp);
		
		lpcs->m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_ARTIST,tmp);
		SetDlgItemText(hDlg,IDC_EDIT_ART,tmp);
		
		lpcs->m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_ALBUM,tmp);
		SetDlgItemText(hDlg,IDC_EDIT_PRD,tmp);
		
		lpcs->m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_YEAR,tmp);
		SetDlgItemText(hDlg,IDC_EDIT_CRD,tmp);
		
		lpcs->m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_GENRE,tmp);
		SetDlgItemText(hDlg,IDC_EDIT_GNR,tmp);
		
		lpcs->m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_COMMENT,tmp);
		SetDlgItemText(hDlg,IDC_EDIT_CMT,tmp);

		lpcs->m_bMp3ApeApply = FALSE;
	}
	else
	{
		EnableEdit(hDlg,lpcs,FALSE);
	}
	if(lpcs->m_bPropAOT)
	{
		CheckDlgButton(hDlg,IDC_CHECK_AOT,BST_CHECKED);
		SetWindowPos(GetParent(hDlg),HWND_TOPMOST,0,0,0,0,
			SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOOWNERZORDER|SWP_NOSIZE);
	}
	else
	{
		CheckDlgButton(hDlg,IDC_CHECK_AOT,BST_UNCHECKED);
		SetWindowPos(GetParent(hDlg),HWND_NOTOPMOST,0,0,0,0,
			SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOOWNERZORDER|SWP_NOSIZE);
	}
}

BOOL CALLBACK CShellExt::PageDlgProc_mp3_APE(HWND hDlg,UINT uMessage,WPARAM wParam,LPARAM lParam)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	LPCSHELLEXT	lpcs = (CShellExt *)GetWindowLongPtr(hDlg,DWLP_USER);
	CString strTmp;

	switch(uMessage){
	case WM_USER+1:	//表示のリフレッシュ
		EnableButton(hDlg,lpcs);
		DispInfo(hDlg,lpcs);
		lpcs->m_bMp3ApeApply = FALSE;
		break;
	case WM_INITDIALOG:
		{
			lpcs = (LPCSHELLEXT )((LPPROPSHEETPAGE )lParam)->lParam;
			lpcs->m_hwndMp3Ape = hDlg;
			SetWindowLongPtr(hDlg, DWLP_USER, (LONG_PTR)lpcs);
			lpcs->m_bMp3ApeApply = FALSE;

			//Ver情報
			SetDlgItemText(hDlg,IDC_STATIC_VER_INFO,SOFT_NAME);
			//タイトル
			SetDlgItemText(hDlg,IDC_EDIT_FILENAME,getFileName(lpcs->m_strSelectFile));
			//アイコンを表示
			SHFILEINFO sfi;
			if(SHGetFileInfo(lpcs->m_strSelectFile,0,&sfi,sizeof(sfi),SHGFI_ICON))
			{
				Static_SetImage_Icon(GetDlgItem(hDlg,IDC_ICON1),sfi.hIcon);
			}

			//コンボボックスの初期化
			ComboBox_AddString(GetDlgItem(hDlg,IDC_EDIT_GNR), _T(""));
			for(int i=0; i<256; i++)
			{
				if(lpcs->m_Id3tagv1.GenreNum2String(i).GetLength())
					ComboBox_AddString(GetDlgItem(hDlg,IDC_EDIT_GNR),
							lpcs->m_Id3tagv1.GenreNum2String(i));
			}
			//オーナードローボタンの初期化
/*			RECT rect,rect2;
			SetRect(&rect,145,208,145,75);//ボタン位置の基準
			MapDialogRect(hDlg,&rect);
			SetWindowPos(GetDlgItem(hDlg,IDC_BUTTON_REW),NULL,rect.left+25*0,rect.top,23,23,SWP_NOZORDER | SWP_SHOWWINDOW);
			GetWindowRect(GetDlgItem(hDlg,IDC_STATIC_WINAMP),&rect2);
			TRACE(_T("GetDlgItem(rect.left,rect.top,rect.bottom,rect.right)=%d,%d,%d,%d\n"),rect2.left,rect2.top,rect2.bottom,rect2.right);
			SetWindowPos(GetDlgItem(hDlg,IDC_BUTTON_PLAY),NULL,rect.left+25*1,rect.top,23,23,SWP_NOZORDER | SWP_SHOWWINDOW);
			SetWindowPos(GetDlgItem(hDlg,IDC_BUTTON_PAUSE),NULL,rect.left+25*2,rect.top,23,23,SWP_NOZORDER | SWP_SHOWWINDOW);
			SetWindowPos(GetDlgItem(hDlg,IDC_BUTTON_STOP),NULL,rect.left+25*3,rect.top,23,23,SWP_NOZORDER | SWP_SHOWWINDOW);
			SetWindowPos(GetDlgItem(hDlg,IDC_BUTTON_FFW),NULL,rect.left+25*4,rect.top,23,23,SWP_NOZORDER | SWP_SHOWWINDOW);
*/			//情報を表示
			DispInfo(hDlg,lpcs);
			lpcs->m_bMp3ApeApply = FALSE;

			break;
		}
		break;

	case WM_TIMER:
		switch(wParam){
		case REW_TIMER:
			SendMessage(hDlg,WM_COMMAND,
				MAKEWPARAM((UINT )IDC_BUTTON_REW,(UINT )BN_CLICKED),
				0);
			break;
		case FFW_TIMER:
			SendMessage(hDlg,WM_COMMAND,
				MAKEWPARAM((UINT )IDC_BUTTON_FFW,(UINT )BN_CLICKED),
				0);
			break;
		}
		break;

	// オーナー描画コントロール
	case WM_DRAWITEM:
		DrawControl(hDlg,(LPDRAWITEMSTRUCT )lParam);
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case IDC_STATIC_VER_INFO:
			TRACE(_T("IDC_STATIC_VER_INFO(%08x)\n"),wParam);
			break;
		case IDC_EDIT_CRD:
		case IDC_EDIT_NAM:
		case IDC_EDIT_ART:
		case IDC_EDIT_PRD:
		case IDC_EDIT_CMT:
		case IDC_EDIT_TRACK:
			if( (HIWORD(wParam) == EN_CHANGE) &&
				(IsWindowEnabled((HWND )lParam)) )
			{
				PropSheet_Changed(GetParent(hDlg),hDlg);
				lpcs->m_bMp3ApeApply = TRUE;
			}
			break;
		case IDC_EDIT_GNR:
			if( ((HIWORD(wParam) == CBN_SELENDOK) || (HIWORD(wParam) == CBN_EDITCHANGE)) &&
				(IsWindowEnabled((HWND )lParam)) )
			{
				PropSheet_Changed(GetParent(hDlg),hDlg);
				lpcs->m_bMp3ApeApply = TRUE;
			}
			break;
		case IDC_BUTTON_DEL_TAG:
			//「APE Tagを削除します。よろしいですか？」
			strTmp.Format(IDS_PAGE_MP3APE_DEL_APE);
			if(MessageBox(hDlg,strTmp,APP_NAME,MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES)
			{
				//ファイルが書き込み可能か調べる
				if(GetFileAttributes(lpcs->m_strSelectFile) & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY))
				{
					//「このファイルは「読み込み専用」です」
					AfxMessageBox(IDS_PAGE_THIS_READONLY,MB_ICONINFORMATION);
					break;
				}
				//タイムスタンプを保存
				lpcs->PushTimeStamp(lpcs->m_strSelectFile);
				// APE Tagを消去
				{
					DWORD dwRet = lpcs->m_Ape.DelTag(lpcs->m_strSelectFile);
					if(dwRet == -1)
					{
						//「ファイルを正しく更新できませんでした。」
						AfxMessageBox(IDS_PAGE_BROKEN,MB_ICONINFORMATION);
						break;
					}
					else if(dwRet != ERROR_SUCCESS)
					{
						//システムエラーを表示
						errMessageBox(hDlg,dwRet);
						break;
					}
				}
				//タイムスタンプを復元
				if(lpcs->m_bSaveTimeStamp)
				{
					lpcs->PopTimeStamp(lpcs->m_strSelectFile);
				}
				//情報を表示
				lpcs->m_Ape.Load(lpcs->m_strSelectFile);
				DispInfo(hDlg,lpcs);
				lpcs->m_bMp3ApeApply = FALSE;
				
				//シェルに変更を通知
				SHChangeNotify(SHCNE_UPDATEITEM,SHCNF_FLUSH|SHCNF_PATH,lpcs->m_strSelectFile,NULL);
				//他の形式タブを更新
				if(lpcs->m_hwndId3v1)
					PostMessage(lpcs->m_hwndId3v1,WM_USER+1,0,0);
				if(lpcs->m_hwndId3v2)
					PostMessage(lpcs->m_hwndId3v2,WM_USER+1,0,0);
				if(lpcs->m_hwndRiffSIF)
					PostMessage(lpcs->m_hwndRiffSIF,WM_USER+1,0,0);
			}
			break;
		case IDC_BUTTON_MAKE_TAG:
			if(lpcs->m_Rmp3.IsEnable())
			{
				//「現在RMP形式のため、APE Tagを作成できません。Riff SIFを削除してください。」
				strTmp.Format(IDS_PAGE_MP3ID3V2_CAN_NOT_MAKE_APE);
				MessageBox(hDlg,strTmp,APP_NAME,MB_ICONINFORMATION);
			}
			else
			{
				//「APE Tagを作成します。よろしいですか？」
				strTmp.Format(IDS_PAGE_MP3APE_MAKE_APE);
				if(MessageBox(hDlg,strTmp,APP_NAME,MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES)
				{
					//ファイルが書き込み可能か調べる
					if(GetFileAttributes(lpcs->m_strSelectFile) & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY))
					{
						//「このファイルは「読み込み専用」です」
						AfxMessageBox(IDS_PAGE_THIS_READONLY,MB_ICONINFORMATION);
						break;
					}
					//タイムスタンプを保存
					lpcs->PushTimeStamp(lpcs->m_strSelectFile);
					// APE Tagを作成
					{
						DWORD dwRet = lpcs->m_Ape.MakeTag(lpcs->m_strSelectFile);
						if(dwRet == -1)
						{
							//「ファイルを正しく更新できませんでした。」
							AfxMessageBox(IDS_PAGE_BROKEN,MB_ICONINFORMATION);
							break;
						}
						else if(dwRet != ERROR_SUCCESS)
						{
							//システムエラーを表示
							errMessageBox(hDlg,dwRet);
							break;
						}
					}
					//タイムスタンプを復元
					if(lpcs->m_bSaveTimeStamp)
					{
						lpcs->PopTimeStamp(lpcs->m_strSelectFile);
					}
					//情報を表示
					lpcs->m_Ape.Load(lpcs->m_strSelectFile);
					DispInfo(hDlg,lpcs);
					lpcs->m_bMp3ApeApply = FALSE;
					
					//シェルに変更を通知
					SHChangeNotify(SHCNE_UPDATEITEM,SHCNF_FLUSH|SHCNF_PATH,lpcs->m_strSelectFile,NULL);
					//他の形式タブを更新
					if(lpcs->m_hwndId3v1)
						PostMessage(lpcs->m_hwndId3v1,WM_USER+1,0,0);
					if(lpcs->m_hwndId3v2)
						PostMessage(lpcs->m_hwndId3v2,WM_USER+1,0,0);
					if(lpcs->m_hwndRiffSIF)
						PostMessage(lpcs->m_hwndRiffSIF,WM_USER+1,0,0);
				}
			}
			break;
		case IDC_COPY_FROM_ID3V2:
			SetWindowText(GetDlgItem(hDlg,IDC_EDIT_NAM),lpcs->m_Id3tagv2.GetTitle());
			SetWindowText(GetDlgItem(hDlg,IDC_EDIT_TRACK),lpcs->m_Id3tagv2.GetTrackNo());
			SetWindowText(GetDlgItem(hDlg,IDC_EDIT_ART),lpcs->m_Id3tagv2.GetArtist());
			SetWindowText(GetDlgItem(hDlg,IDC_EDIT_PRD),lpcs->m_Id3tagv2.GetAlbum());
			SetWindowText(GetDlgItem(hDlg,IDC_EDIT_CRD),lpcs->m_Id3tagv2.GetYear());
			SetWindowText(GetDlgItem(hDlg,IDC_EDIT_GNR),lpcs->m_Id3tagv2.GetGenre());
			SetWindowText(GetDlgItem(hDlg,IDC_EDIT_CMT),lpcs->m_Id3tagv2.GetComment());
			break;
		case IDC_COPY_FROM_ID3V1:
			SetWindowText(GetDlgItem(hDlg,IDC_EDIT_NAM),lpcs->m_Id3tagv1.GetTitle());
			SetWindowText(GetDlgItem(hDlg,IDC_EDIT_TRACK),lpcs->m_Id3tagv1.GetTrackNo());
			SetWindowText(GetDlgItem(hDlg,IDC_EDIT_ART),lpcs->m_Id3tagv1.GetArtist());
			SetWindowText(GetDlgItem(hDlg,IDC_EDIT_PRD),lpcs->m_Id3tagv1.GetAlbum());
			SetWindowText(GetDlgItem(hDlg,IDC_EDIT_CRD),lpcs->m_Id3tagv1.GetYear());
			SetWindowText(GetDlgItem(hDlg,IDC_EDIT_GNR),lpcs->m_Id3tagv1.GetGenre());
			SetWindowText(GetDlgItem(hDlg,IDC_EDIT_CMT),lpcs->m_Id3tagv1.GetComment());
			break;
		case IDC_BUTTON_PLAY:
			{
				PlayWinamp(hDlg,lpcs->m_strSelectFile);
				break;
			}
		case IDC_BUTTON_PAUSE:
			{
				PauseWinamp();
				break;
			}
		case IDC_BUTTON_STOP:
			{
				StopWinamp();
				break;
			}
		case IDC_BUTTON_REW:
			{
				Rew5Winamp();
				break;
			}
		case IDC_BUTTON_FFW:
			{
				Ffw5Winamp();
				break;
			}
		case IDC_CHECK_AOT:
			if(IsDlgButtonChecked(hDlg,IDC_CHECK_AOT) == BST_CHECKED)
			{
				lpcs->m_bPropAOT = TRUE;
				SetWindowPos(GetParent(hDlg),HWND_TOPMOST,0,0,0,0,
					SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOOWNERZORDER|SWP_NOSIZE);
			}
			else
			{
				lpcs->m_bPropAOT = FALSE;
				SetWindowPos(GetParent(hDlg),HWND_NOTOPMOST,0,0,0,0,
					SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOOWNERZORDER|SWP_NOSIZE);
			}
			if(lpcs->m_hwndId3v2)
				PostMessage(lpcs->m_hwndId3v2,WM_USER+1,0,0);
			if(lpcs->m_hwndId3v1)
				PostMessage(lpcs->m_hwndId3v1,WM_USER+1,0,0);
			if(lpcs->m_hwndRiffSIF)
				PostMessage(lpcs->m_hwndRiffSIF,WM_USER+1,0,0);
			regSetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("PropAOT"),(DWORD )lpcs->m_bPropAOT);
			break;
		case IDC_SETUP:
			OpenConfigPage(hDlg,2);
			break;
		case IDC_HELPVIEW:
			lpcs->OpenHtmlHelp(hDlg,_T("extension.htm"));
			break;
		DLG_CLIPBORD_MACRO(lpcs->m_strSelectFile);
		}
		break;
	
	// コンテキストメニュー
	case WM_CONTEXTMENU:
		DlgContextMenu(hDlg,lParam,lpcs->m_Ape.IsEnable());
		break;
	
	//状況依存ヘルプ
	case WM_HELP:
		OpenAboutDlg(hDlg);
		break;

	case WM_NOTIFY:
		switch(((NMHDR FAR *)lParam)->code){
//		case PSN_SETACTIVE:
//			break;
		case PSN_APPLY:
			//保存
			if(lpcs->m_bMp3ApeApply && lpcs->m_Ape.IsEnable())
			{
				//ファイルが書き込み可能か調べる
				if(GetFileAttributes(lpcs->m_strSelectFile) & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY))
				{
					//「このファイルは「読み込み専用」です」
					AfxMessageBox(IDS_PAGE_THIS_READONLY,MB_ICONINFORMATION);
					//適用ボタンは引き続き有効
					SetWindowLongPtr(hDlg,DWLP_MSGRESULT,PSNRET_INVALID);
					break;
				}

				CString strTmp;
				CWnd wnd;
				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_NAM));
				wnd.GetWindowText(strTmp);
				wnd.Detach();
				lpcs->m_Ape.SetComment(CTag_Ape::APE_TAG_FIELD_TITLE,strTmp);

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_TRACK));
				wnd.GetWindowText(strTmp);
				wnd.Detach();
				lpcs->m_Ape.SetComment(CTag_Ape::APE_TAG_FIELD_TRACK,strTmp);

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_ART));
				wnd.GetWindowText(strTmp);
				wnd.Detach();
				lpcs->m_Ape.SetComment(CTag_Ape::APE_TAG_FIELD_ARTIST,strTmp);

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_PRD));
				wnd.GetWindowText(strTmp);
				wnd.Detach();
				lpcs->m_Ape.SetComment(CTag_Ape::APE_TAG_FIELD_ALBUM,strTmp);

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_CRD));
				wnd.GetWindowText(strTmp);
				wnd.Detach();
				lpcs->m_Ape.SetComment(CTag_Ape::APE_TAG_FIELD_YEAR,strTmp);

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_GNR));
				wnd.GetWindowText(strTmp);
				wnd.Detach();
				lpcs->m_Ape.SetComment(CTag_Ape::APE_TAG_FIELD_GENRE,strTmp);

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_CMT));
				wnd.GetWindowText(strTmp);
				wnd.Detach();
				lpcs->m_Ape.SetComment(CTag_Ape::APE_TAG_FIELD_COMMENT,strTmp);

				//タイムスタンプを保存
				lpcs->PushTimeStamp(lpcs->m_strSelectFile);
				
				DWORD dwRet = lpcs->m_Ape.Save(lpcs->m_strSelectFile);
				if(dwRet != ERROR_SUCCESS)
				{
					lpcs->m_bMp3ApeApply = TRUE;
					if(dwRet == -1)
						//「ファイルを正しく更新できませんでした。」
						AfxMessageBox(IDS_PAGE_BROKEN,MB_ICONINFORMATION);
					else
						//システムエラーを表示
						errMessageBox(hDlg,dwRet);
					//適用ボタンは引き続き有効
					SetWindowLongPtr(hDlg,DWLP_MSGRESULT,PSNRET_INVALID);
					break;
				}

				//タイムスタンプを復元
				if(lpcs->m_bSaveTimeStamp)
				{
					lpcs->PopTimeStamp(lpcs->m_strSelectFile);
				}

				//情報を表示
				lpcs->m_Ape.Load(lpcs->m_strSelectFile);
				DispInfo(hDlg,lpcs);
				lpcs->m_bMp3ApeApply = FALSE;
				
				SetWindowLongPtr(hDlg,DWLP_MSGRESULT,PSNRET_NOERROR);
				
				//シェルに変更を通知
				SHChangeNotify(SHCNE_UPDATEITEM,SHCNF_FLUSH|SHCNF_PATH,lpcs->m_strSelectFile,NULL);
			}
			break;
		default:
			break;
		}
		break;

	default:
		return FALSE;
	
	}

	return TRUE;
}
