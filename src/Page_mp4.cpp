#include "StdAfx.h"
#include "mp3infp.h"
#include "Resource.h"

static int ids[] =
{
	-1,
	CLP_NAM,
	CLP_ART,
	CLP_PRD,
	CLP_GROUP,
	CLP_COMPOSE,
	CLP_GNR,
	CLP_TRACK,
	CLP_TRACK2,
	CLP_DISC,
	CLP_DISC2,
	CLP_BPM,
	CLP_CRD,
	CLP_COMPILATION,
	CLP_CMT,
	CLP_SFT,
	0
};

static int staticWnd[] =
{
	-1,
	IDC_STATIC_NAM,
	IDC_STATIC_ART,
	IDC_STATIC_PRD,
	IDC_STATIC_GRP,
	IDC_STATIC_COMPOSE,
	IDC_STATIC_GNR,
	IDC_STATIC_TRK,
	-1,
	IDC_STATIC_DISC,
	-1,
	IDC_STATIC_BPM,
	IDC_STATIC_CRD,
	IDC_COMPILATION,
	IDC_STATIC_CMT,
	IDC_STATIC_TOOL,
	0
};

static int editWnd[] =
{
	IDC_EDIT_FORMAT,
	IDC_EDIT_SBJ,
	IDC_EDIT_ART,
	IDC_EDIT_PRD,
	IDC_EDIT_GRP,
	IDC_EDIT_COMPOSE,
	IDC_EDIT_GNR,
	IDC_EDIT_TRK,
	IDC_EDIT_TRK2,
	IDC_EDIT_DISC,
	IDC_EDIT_DISC2,
	IDC_EDIT_BPM,
	IDC_EDIT_CRD,
	IDC_COMPILATION,
	IDC_EDIT_CMT,
	IDC_EDIT_TOOL,
	0
};

UINT CALLBACK CShellExt::PageCallback_mp4(HWND hWnd,UINT uMessage,LPPROPSHEETPAGE ppsp)
{
	LPCSHELLEXT	lpcs = (CShellExt *)ppsp->lParam;
	switch(uMessage){
	case PSPCB_CREATE:	//プロパティページが作成されつつある
		return TRUE;
	case PSPCB_RELEASE:	//プロパティページが破壊されつつある
		if(lpcs) 
		{
			lpcs->m_hwndPage = NULL;
			lpcs->Release();
		}
		return TRUE;
	}

	return TRUE;
}

static void EnableEdit(HWND hDlg,CShellExt *lpcs,BOOL bEnable)
{
	SetDlgItemText(hDlg,IDC_EDIT_FORMAT,_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_FORMAT),bEnable);
	SetDlgItemText(hDlg,IDC_EDIT_SBJ,_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_SBJ),bEnable);
	SetDlgItemText(hDlg,IDC_EDIT_ART,_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_ART),bEnable);
	SetDlgItemText(hDlg,IDC_EDIT_PRD,_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_PRD),bEnable);
	SetDlgItemText(hDlg,IDC_EDIT_GNR,_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_GRP),bEnable);
	SetDlgItemText(hDlg,IDC_EDIT_GRP,_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_COMPOSE),bEnable);
	SetDlgItemText(hDlg,IDC_EDIT_COMPOSE,_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_GNR),bEnable);
	SetDlgItemText(hDlg,IDC_EDIT_TRK,_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_TRK),bEnable);
	SetDlgItemText(hDlg,IDC_EDIT_TRK2,_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_TRK2),bEnable);
	SetDlgItemText(hDlg,IDC_EDIT_DISC,_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_DISC),bEnable);
	SetDlgItemText(hDlg,IDC_EDIT_DISC2,_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_DISC2),bEnable);
	SetDlgItemText(hDlg,IDC_EDIT_BPM,_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_BPM),bEnable);
	SetDlgItemText(hDlg,IDC_EDIT_CRD,_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_CRD),bEnable);
	CheckDlgButton(hDlg,IDC_COMPILATION,0);
	EnableWindow(GetDlgItem(hDlg,IDC_COMPILATION),bEnable);
	SetDlgItemText(hDlg,IDC_EDIT_CMT,_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_CMT),bEnable);
	SetDlgItemText(hDlg,IDC_EDIT_TOOL,_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_TOOL),bEnable);
}

static void DispInfo(HWND hDlg,CShellExt *lpcs)
{
	CString tmp;

	EnableEdit(hDlg,lpcs,TRUE);

	SetDlgItemText(hDlg,IDC_EDIT_FORMAT,lpcs->m_Mp4.GetTrackInfoString());

	SetDlgItemText(hDlg,IDC_EDIT_SBJ,lpcs->m_Mp4.GetMetadata_Name());
	SetDlgItemText(hDlg,IDC_EDIT_ART,lpcs->m_Mp4.GetMetadata_Artist());
	SetDlgItemText(hDlg,IDC_EDIT_PRD,lpcs->m_Mp4.GetMetadata_Album());
	SetDlgItemText(hDlg,IDC_EDIT_GRP,lpcs->m_Mp4.GetMetadata_Group());
	SetDlgItemText(hDlg,IDC_EDIT_COMPOSE,lpcs->m_Mp4.GetMetadata_Composer());
	SetDlgItemText(hDlg,IDC_EDIT_GNR,lpcs->m_Mp4.GetMetadata_Genre());

	int val = lpcs->m_Mp4.GetMetadata_Track1();
	if((val != -1) && val)
	{
		tmp.Format(_T("%d"),val);
		SetDlgItemText(hDlg,IDC_EDIT_TRK,tmp);
	}

	val = lpcs->m_Mp4.GetMetadata_Track2();
	if((val != -1) && val)
	{
		tmp.Format(_T("%d"),val);
		SetDlgItemText(hDlg,IDC_EDIT_TRK2,tmp);
	}

	val = lpcs->m_Mp4.GetMetadata_Disc1();
	if((val != -1) && val)
	{
		tmp.Format(_T("%d"),val);
		SetDlgItemText(hDlg,IDC_EDIT_DISC,tmp);
	}
	val = lpcs->m_Mp4.GetMetadata_Disc2();
	if((val != -1) && val)
	{
		tmp.Format(_T("%d"),val);
		SetDlgItemText(hDlg,IDC_EDIT_DISC2,tmp);
	}

	val = lpcs->m_Mp4.GetMetadata_Tempo();
	if((val != -1) && val)
	{
		tmp.Format(_T("%d"),val);
		SetDlgItemText(hDlg,IDC_EDIT_BPM,tmp);
	}

	SetDlgItemText(hDlg,IDC_EDIT_CRD,lpcs->m_Mp4.GetMetadata_Year());
	val = lpcs->m_Mp4.GetMetadata_Compilation();
	if(val != -1)
	{
		if(val)
		{
			CheckDlgButton(hDlg,IDC_COMPILATION,1);
		}
	}
	SetDlgItemText(hDlg,IDC_EDIT_CMT,lpcs->m_Mp4.GetMetadata_Comment());
	SetDlgItemText(hDlg,IDC_EDIT_TOOL,lpcs->m_Mp4.GetMetadata_Tool());

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

	lpcs->m_bApply = FALSE;
}

BOOL CALLBACK CShellExt::PageDlgProc_mp4(HWND hDlg,UINT uMessage,WPARAM wParam,LPARAM lParam)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	LPCSHELLEXT	lpcs = (CShellExt *)GetWindowLongPtr(hDlg,DWLP_USER);
	CString strTmp;

	switch(uMessage){
	case WM_INITDIALOG:
		{
			lpcs = (LPCSHELLEXT )((LPPROPSHEETPAGE )lParam)->lParam;
			lpcs->m_hwndPage = hDlg;
			SetWindowLongPtr(hDlg, DWLP_USER, (LONG_PTR)lpcs);
			lpcs->m_bApply = FALSE;

			//Ver情報
			SetDlgItemText(hDlg,IDC_STATIC_VER_INFO,SOFT_NAME);
			//タイトル
			SetDlgItemText(hDlg,IDC_EDIT_FILENAME,getFileName(lpcs->m_strSelectFile));
			//アイコンを表示
			SHFILEINFO sfi;
			if(SHGetFileInfo(lpcs->m_strSelectFile,0,&sfi,sizeof(sfi),SHGFI_ICON))
			{
				SendMessage(GetDlgItem(hDlg,IDC_ICON1),
					STM_SETIMAGE,IMAGE_ICON,
					(LPARAM )sfi.hIcon);
			}

			//コンボボックスの初期化
			SendMessage(
					GetDlgItem(hDlg,IDC_EDIT_GNR),
					CB_ADDSTRING,
					0,
					(LPARAM )(LPCTSTR )_T("")	//空白
				);
			for(int i=0; i<256; i++)
			{
				if(lpcs->m_Id3tagv1.GenreNum2String(i).GetLength())
					SendMessage(
							GetDlgItem(hDlg,IDC_EDIT_GNR),
							CB_ADDSTRING,
							0,
							(LPARAM )(LPCTSTR )lpcs->m_Id3tagv1.GenreNum2String(i)
						);
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
			lpcs->m_bApply = FALSE;

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
		case IDC_COMPILATION:
			if(HIWORD(wParam) == BN_CLICKED)
			{
				PropSheet_Changed(GetParent(hDlg),hDlg);
				lpcs->m_bApply = TRUE;
			}
			break;
		case IDC_EDIT_SBJ:
		case IDC_EDIT_ART:
		case IDC_EDIT_PRD:
		case IDC_EDIT_GRP:
		case IDC_EDIT_COMPOSE:
		case IDC_EDIT_TRK:
		case IDC_EDIT_TRK2:
		case IDC_EDIT_DISC:
		case IDC_EDIT_DISC2:
		case IDC_EDIT_BPM:
		case IDC_EDIT_CRD:
		case IDC_EDIT_CMT:
		case IDC_EDIT_TOOL:
			if(HIWORD(wParam) == EN_CHANGE)
			{
				PropSheet_Changed(GetParent(hDlg),hDlg);
				lpcs->m_bApply = TRUE;
			}
			break;
		case IDC_EDIT_GNR:
			if( ((HIWORD(wParam) == CBN_SELENDOK) || (HIWORD(wParam) == CBN_EDITCHANGE)) )
			{
				PropSheet_Changed(GetParent(hDlg),hDlg);
				lpcs->m_bApply = TRUE;
			}
			break;
		case IDC_BUTTON_PLAY:
			{
				PlayWinamp(hDlg,(LPCTSTR )lpcs->m_strSelectFile);
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
			regSetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("PropAOT"),(DWORD )lpcs->m_bPropAOT);
			break;
		case IDC_SETUP:
			OpenSetupPage(hDlg,10);
			break;
		case IDC_HELPVIEW:
			lpcs->OpenHtmlHelp(hDlg,_T("extension.htm"));
			break;
		DLG_CLIPBORD_MACRO(lpcs->m_strSelectFile);
		}
		break;
	
	// コンテキストメニュー
	case WM_CONTEXTMENU:
		DlgContextMenu(hDlg,lParam,lpcs->m_Mp4.IsEnable());
		break;
	
	//状況依存ヘルプ
	case WM_HELP:
	{
		TCHAR szTmp[256];
		lstrcpy(szTmp,APP_NAME);
		lstrcat(szTmp,_T(" "));
		lstrcat(szTmp,COPY_RIGHT);
		MessageBox(GetParent(hDlg),szTmp,_T("About"),MB_APPLMODAL);
		break;
	}
	case WM_NOTIFY:
		switch(((NMHDR FAR *)lParam)->code){
//		case PSN_SETACTIVE:
//			break;
		case PSN_APPLY:
			//保存
			if(lpcs->m_bApply)
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
				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_SBJ));
				wnd.GetWindowText(strTmp);
				wnd.Detach();
				lpcs->m_Mp4.SetMetadata_Name(strTmp);

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_ART));
				wnd.GetWindowText(strTmp);
				wnd.Detach();
				lpcs->m_Mp4.SetMetadata_Artist(strTmp);

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_PRD));
				wnd.GetWindowText(strTmp);
				wnd.Detach();
				lpcs->m_Mp4.SetMetadata_Album(strTmp);

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_GRP));
				wnd.GetWindowText(strTmp);
				wnd.Detach();
				lpcs->m_Mp4.SetMetadata_Group(strTmp);

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_COMPOSE));
				wnd.GetWindowText(strTmp);
				wnd.Detach();
				lpcs->m_Mp4.SetMetadata_Composer(strTmp);

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_GNR));
				wnd.GetWindowText(strTmp);
				wnd.Detach();
				lpcs->m_Mp4.SetMetadata_Genre(strTmp);

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_TRK));
				wnd.GetWindowText(strTmp);
				wnd.Detach();
				if(strTmp.GetLength())
				{
					lpcs->m_Mp4.SetMetadata_Track1(_ttoi(strTmp));
				}
				else
				{
					lpcs->m_Mp4.SetMetadata_Track1(-1);
				}

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_TRK2));
				wnd.GetWindowText(strTmp);
				wnd.Detach();
				if(strTmp.GetLength())
				{
					lpcs->m_Mp4.SetMetadata_Track2(_ttoi(strTmp));
				}
				else
				{
					lpcs->m_Mp4.SetMetadata_Track2(-1);
				}

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_DISC));
				wnd.GetWindowText(strTmp);
				wnd.Detach();
				if(strTmp.GetLength())
				{
					lpcs->m_Mp4.SetMetadata_Disc1(_ttoi(strTmp));
				}
				else
				{
					lpcs->m_Mp4.SetMetadata_Disc1(-1);
				}

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_DISC2));
				wnd.GetWindowText(strTmp);
				wnd.Detach();
				if(strTmp.GetLength())
				{
					lpcs->m_Mp4.SetMetadata_Disc2(_ttoi(strTmp));
				}
				else
				{
					lpcs->m_Mp4.SetMetadata_Disc2(-1);
				}

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_BPM));
				wnd.GetWindowText(strTmp);
				wnd.Detach();
				if(strTmp.GetLength())
				{
					lpcs->m_Mp4.SetMetadata_Tempo(_ttoi(strTmp));
				}
				else
				{
					lpcs->m_Mp4.SetMetadata_Tempo(-1);
				}

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_CRD));
				wnd.GetWindowText(strTmp);
				wnd.Detach();
				lpcs->m_Mp4.SetMetadata_Year(strTmp);

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_CMT));
				wnd.GetWindowText(strTmp);
				wnd.Detach();
				lpcs->m_Mp4.SetMetadata_Comment(strTmp);

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_TOOL));
				wnd.GetWindowText(strTmp);
				wnd.Detach();
				lpcs->m_Mp4.SetMetadata_Tool(strTmp);

				if(IsDlgButtonChecked(hDlg,IDC_COMPILATION))
				{
					lpcs->m_Mp4.SetMetadata_Compilation(1);
				}
				else
				{
					lpcs->m_Mp4.SetMetadata_Compilation(-1);
				}

				//タイムスタンプを保存
				lpcs->PushTimeStamp(lpcs->m_strSelectFile);
				
				DWORD dwRet = lpcs->m_Mp4.Save(lpcs->m_strSelectFile);
				if(dwRet != ERROR_SUCCESS)
				{
					lpcs->m_bApply = TRUE;
					//「ファイルを正しく更新できませんでした。」
					AfxMessageBox(IDS_PAGE_BROKEN,MB_ICONINFORMATION);
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
				lpcs->m_Mp4.Load(lpcs->m_strSelectFile);
				DispInfo(hDlg,lpcs);
				lpcs->m_bApply = FALSE;
				
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