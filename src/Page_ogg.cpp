#include "StdAfx.h"
#include "mp3infp.h"

#include "Dlg_ogg_exttag.h"

static int ids[] =
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

static int staticWnd[] =
{
	IDC_STATIC_FORMAT_,
	IDC_STATIC_TIME_,
	IDC_STATIC_NAM,
	IDC_STATIC_TRK,
	IDC_STATIC_ART,
	IDC_STATIC_PRD,
	IDC_STATIC_CRD,
	IDC_STATIC_GNR,
	IDC_STATIC_CMT,
	0
};

static int editWnd[] =
{
	IDC_STATIC_FORMAT,
	IDC_STATIC_TIME,
	IDC_EDIT_SBJ,
	IDC_EDIT_TRK,
	IDC_EDIT_ART,
	IDC_EDIT_PRD,
	IDC_EDIT_CRD,
	IDC_EDIT_GNR,
	IDC_EDIT_CMT,
	0
};

class COggExt
{
public:
	COggExt(LPCTSTR name,LPCTSTR value)
	{
		m_strName = name;
		m_strValue = value;
	};
	virtual ~COggExt()
	{
	};

	CString GetName(){return m_strName;};
	void SetName(LPCTSTR name){m_strName = name;};
	CString GetValue(){return m_strValue;};
	void SetValue(LPCTSTR value){m_strValue = value;};
	CString Get1LineDisp()
	{
		//改行があるときは(...)に置き換え
		CString strValue;
		int crlf = m_strValue.FindOneOf(_T("\r\n"));
		if(crlf != -1)
		{
			strValue = CString(m_strValue,crlf) + _T("(...)");
		}
		else
		{
			strValue = m_strValue;
		}
		return m_strName + _T("=") + strValue;
	}

private:
	CString m_strName;
	CString m_strValue;
};

UINT CALLBACK CShellExt::PageCallback_ogg(HWND hWnd,UINT uMessage,LPPROPSHEETPAGE ppsp)
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

static void DispInfo(HWND hDlg,CShellExt *lpcs)
{
	SetDlgItemText(hDlg,IDC_STATIC_FORMAT,lpcs->m_Ogg.GetAudioFormatString());
	SetDlgItemText(hDlg,IDC_STATIC_TIME,lpcs->m_Ogg.GetTimeString());
	
	//その他の情報
	CString strAvg;
	if(lpcs->m_Ogg.GetBitrateAvg() != -1)
		strAvg.Format(_T(" %ld(avg)"),lpcs->m_Ogg.GetBitrateAvg());
	CString strUpper;
	if(lpcs->m_Ogg.GetBitrateUpper() != -1)
		strUpper.Format(_T(" %ld(upper)"),lpcs->m_Ogg.GetBitrateUpper());
	CString strNominal;
	if(lpcs->m_Ogg.GetBitrateNominal() != -1)
		strNominal.Format(_T(" %ld(nominal)"),lpcs->m_Ogg.GetBitrateNominal());
	CString strLower;
	if(lpcs->m_Ogg.GetBitrateLower() != -1)
		strLower.Format(_T(" %ld(lower)"),lpcs->m_Ogg.GetBitrateLower());
	CString strWindow;
	if(lpcs->m_Ogg.GetBitrateWindow() != 0)
		strWindow.Format(_T(" %ld(window)"),lpcs->m_Ogg.GetBitrateWindow());

	CString tmp;
	tmp.Format( _T("Version = %ld ,Serial=%ld\n")
				_T("Bitrate = %s%s%s%s%s\n")
				_T("Vendor = %s\n"),
				lpcs->m_Ogg.GetVersion(),
				lpcs->m_Ogg.GetSerial(),
				strAvg,
				strUpper,
				strNominal,
				strLower,
				strWindow,
				lpcs->m_Ogg.GetVendor()
				);
	SetDlgItemText(hDlg,IDC_STATIC_ETC_INFO,tmp);

	lpcs->m_Ogg.GetComment(_T("TITLE"),0,tmp);
	SetDlgItemText(hDlg,IDC_EDIT_SBJ,tmp);

	lpcs->m_Ogg.GetComment(_T("TRACKNUMBER"),0,tmp);
	SetDlgItemText(hDlg,IDC_EDIT_TRK,tmp);
	
	lpcs->m_Ogg.GetComment(_T("ARTIST"),0,tmp);
	SetDlgItemText(hDlg,IDC_EDIT_ART,tmp);
	
	lpcs->m_Ogg.GetComment(_T("ALBUM"),0,tmp);
	SetDlgItemText(hDlg,IDC_EDIT_PRD,tmp);
	
	lpcs->m_Ogg.GetComment(_T("DATE"),0,tmp);
	SetDlgItemText(hDlg,IDC_EDIT_CRD,tmp);
	
	lpcs->m_Ogg.GetComment(_T("GENRE"),0,tmp);
	SetDlgItemText(hDlg,IDC_EDIT_GNR,tmp);
	
	lpcs->m_Ogg.GetComment(_T("COMMENT"),0,tmp);
	SetDlgItemText(hDlg,IDC_EDIT_CMT,tmp);

	lpcs->m_bApply = FALSE;
	
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

static void DispInfoExt(HWND hDlg,CShellExt *lpcs)
{
	//クリア
	long items = ListBox_GetCount(GetDlgItem(hDlg,IDC_LIST_VALUE));
	if(items != LB_ERR)
	{
		for(int i=0; i<items; i++)
		{
			COggExt *oggExt = (COggExt *)ListBox_GetItemData(GetDlgItem(hDlg,IDC_LIST_VALUE),0);
			if(oggExt)
			{
				delete oggExt;
			}
			ListBox_DeleteString(GetDlgItem(hDlg,IDC_LIST_VALUE),0);
		}
	}

	CString strDisp;
	CStringArray strArray;
	lpcs->m_Ogg.GetCommentNames(strArray);
	items = 0;
	for(int i=0; i<strArray.GetSize(); i++)
	{
		CString strName = strArray.GetAt(i);
		CString strValue;
		CString _strValue;
		int j = 0;
		while(1)
		{
			if(!lpcs->m_Ogg.GetComment(strName,j++,strValue))
			{
				break;
			}

			//標準名は除外
			if((j == 1) &&
				(
				!strName.Compare(_T("TITLE")) ||
				!strName.Compare(_T("TRACKNUMBER")) ||
				!strName.Compare(_T("ARTIST")) ||
				!strName.Compare(_T("ALBUM")) ||
				!strName.Compare(_T("DATE")) ||
				!strName.Compare(_T("GENRE")) ||
				!strName.Compare(_T("COMMENT")))
				)
			{
				continue;
			}
//			TRACE(_T("OGG %s %s\n"),strName,strValue);
			COggExt *oggExt = new COggExt(strName,strValue);
			ListBox_AddString(GetDlgItem(hDlg,IDC_LIST_VALUE),oggExt->Get1LineDisp());
			ListBox_SetItemData(GetDlgItem(hDlg,IDC_LIST_VALUE),items,oggExt);
			items++;
		}
	}
}

BOOL CALLBACK CShellExt::PageDlgProc_ogg(HWND hDlg,UINT uMessage,WPARAM wParam,LPARAM lParam)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	LPCSHELLEXT	lpcs = (CShellExt *)GetWindowLongPtr(hDlg,DWLP_USER);
	CString strTmp;
	HANDLE hFile;

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
/*			RECT rect;
			SetRect(&rect,145,208,145,75);//ボタン位置の基準
			MapDialogRect(hDlg,&rect);
			SetWindowPos(GetDlgItem(hDlg,IDC_BUTTON_REW),NULL,rect.left+25*0,rect.top,23,23,SWP_NOZORDER | SWP_SHOWWINDOW);
			SetWindowPos(GetDlgItem(hDlg,IDC_BUTTON_PLAY),NULL,rect.left+25*1,rect.top,23,23,SWP_NOZORDER | SWP_SHOWWINDOW);
			SetWindowPos(GetDlgItem(hDlg,IDC_BUTTON_PAUSE),NULL,rect.left+25*2,rect.top,23,23,SWP_NOZORDER | SWP_SHOWWINDOW);
			SetWindowPos(GetDlgItem(hDlg,IDC_BUTTON_STOP),NULL,rect.left+25*3,rect.top,23,23,SWP_NOZORDER | SWP_SHOWWINDOW);
			SetWindowPos(GetDlgItem(hDlg,IDC_BUTTON_FFW),NULL,rect.left+25*4,rect.top,23,23,SWP_NOZORDER | SWP_SHOWWINDOW);
*/			
			//情報を表示
			DispInfo(hDlg,lpcs);
			DispInfoExt(hDlg,lpcs);
			
			return FALSE;
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
		case IDC_BUTTON_ADD_VALUE:
			//値追加
			{
				CDlg_ogg_exttag dlg;
				if(dlg.DoModal() == IDOK)
				{
					COggExt *oggExt = new COggExt(dlg.m_strName,dlg.m_strValue);
					long index = ListBox_AddString(GetDlgItem(hDlg,IDC_LIST_VALUE),oggExt->Get1LineDisp());
					if(index == LB_ERR)
					{
						delete oggExt;
					}
					else
					{
						ListBox_SetItemData(GetDlgItem(hDlg,IDC_LIST_VALUE),index,oggExt);
					}
				
					PropSheet_Changed(GetParent(hDlg),hDlg);
					lpcs->m_bApply = TRUE;
				}
			}
			break;
		case IDC_BUTTON_DEL_VALUE:
			//値削除
			{
				long item = ListBox_GetCurSel(GetDlgItem(hDlg,IDC_LIST_VALUE));
				if(item == LB_ERR)
					break;
				
				COggExt *oggExt = (COggExt *)ListBox_GetItemData(GetDlgItem(hDlg,IDC_LIST_VALUE),item);
				if(oggExt)
				{
					delete oggExt;
				}
				ListBox_DeleteString(GetDlgItem(hDlg,IDC_LIST_VALUE),item);
				
				PropSheet_Changed(GetParent(hDlg),hDlg);
				lpcs->m_bApply = TRUE;
			}
			break;
		case IDC_LIST_VALUE:
			if(HIWORD(wParam) == LBN_DBLCLK)
			{
				//ダブルクリック - 編集
				long item = ListBox_GetCurSel(GetDlgItem(hDlg,IDC_LIST_VALUE));
				if(item == LB_ERR)
					break;
				
				COggExt *oggExt = (COggExt *)ListBox_GetItemData(GetDlgItem(hDlg,IDC_LIST_VALUE),item);
				if(oggExt == NULL)
					break;
				
				CDlg_ogg_exttag dlg;

				dlg.m_strName = oggExt->GetName();
				dlg.m_strValue = oggExt->GetValue();
				if(dlg.DoModal() == IDOK)
				{
					oggExt->SetName(dlg.m_strName);
					oggExt->SetValue(dlg.m_strValue);
					ListBox_DeleteString(GetDlgItem(hDlg,IDC_LIST_VALUE),item);
					ListBox_InsertString(GetDlgItem(hDlg,IDC_LIST_VALUE),item,oggExt->Get1LineDisp());
					ListBox_SetItemData(GetDlgItem(hDlg,IDC_LIST_VALUE),item,oggExt);
					
					PropSheet_Changed(GetParent(hDlg),hDlg);
					lpcs->m_bApply = TRUE;
				}
			}
			break;
		case IDC_EDIT_SBJ:
		case IDC_EDIT_TRK:
		case IDC_EDIT_ART:
		case IDC_EDIT_PRD:
		case IDC_EDIT_CRD:
		case IDC_EDIT_CMT:
			if((HIWORD(wParam) == EN_CHANGE) &&
				(IsWindowEnabled((HWND )lParam)) )
			{
				PropSheet_Changed(GetParent(hDlg),hDlg);
				lpcs->m_bApply = TRUE;
			}
			break;
		case IDC_EDIT_GNR:
			if( ((HIWORD(wParam) == CBN_SELENDOK) || (HIWORD(wParam) == CBN_EDITCHANGE)) &&
				(IsWindowEnabled((HWND )lParam)) )
			{
				PropSheet_Changed(GetParent(hDlg),hDlg);
				lpcs->m_bApply = TRUE;
			}
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
			regSetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("PropAOT"),(DWORD )lpcs->m_bPropAOT);
			break;
		case IDC_SETUP:
			OpenConfigPage(hDlg,8);
			break;
		case IDC_HELPVIEW:
			lpcs->OpenHtmlHelp(hDlg,_T("extension.htm"));
			break;
		DLG_CLIPBORD_MACRO(lpcs->m_strSelectFile);
		}
		break;
	case WM_DESTROY:
		{
			//後始末
			long items = ListBox_GetCount(GetDlgItem(hDlg,IDC_LIST_VALUE));
			if(items != LB_ERR)
			{
				for(int i=0; i<items; i++)
				{
					COggExt *oggExt = (COggExt *)ListBox_GetItemData(GetDlgItem(hDlg,IDC_LIST_VALUE),i);
					if(oggExt)
					{
						delete oggExt;
					}
				}
			}
		}
		return TRUE;
	// コンテキストメニュー
	case WM_CONTEXTMENU:
		DlgContextMenu(hDlg,lParam,lpcs->m_Ogg.IsEnable());
		break;
	//状況依存ヘルプ
	case WM_HELP:
	{
		TCHAR szTmp[256];
		lstrcpy(szTmp,APP_NAME);
		lstrcat(szTmp,_T(" "));
		lstrcat(szTmp,COPY_RIGHT);
		MessageBox(hDlg,szTmp,_T("About"),MB_APPLMODAL);
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
				TRACE(_T("WM_NOTIFY(PSN_APPLY) - 保存\n"));
				//ファイルが書き込み可能か調べる
				if(GetFileAttributes(lpcs->m_strSelectFile) & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY))
				{
					//「このファイルは「読み込み専用」です」
					AfxMessageBox(IDS_PAGE_THIS_READONLY,MB_ICONINFORMATION);
					//適用ボタンは引き続き有効
					SetWindowLongPtr(hDlg,DWLP_MSGRESULT,PSNRET_INVALID);
					break;
				}
				lpcs->m_bApply = FALSE;

				CString strTmp,strLine;
				CWnd wnd;
				//いったんリセット
				lpcs->m_Ogg.Release();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_SBJ));
				wnd.GetWindowText(strTmp);
				wnd.Detach();
				lpcs->m_Ogg.AddComment(_T("TITLE"),strTmp);

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_TRK));
				wnd.GetWindowText(strTmp);
				wnd.Detach();
				lpcs->m_Ogg.AddComment(_T("TRACKNUMBER"),strTmp);

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_ART));
				wnd.GetWindowText(strTmp);
				wnd.Detach();
				lpcs->m_Ogg.AddComment(_T("ARTIST"),strTmp);

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_PRD));
				wnd.GetWindowText(strTmp);
				wnd.Detach();
				lpcs->m_Ogg.AddComment(_T("ALBUM"),strTmp);

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_CRD));
				wnd.GetWindowText(strTmp);
				wnd.Detach();
				lpcs->m_Ogg.AddComment(_T("DATE"),strTmp);

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_GNR));
				wnd.GetWindowText(strTmp);
				wnd.Detach();
				lpcs->m_Ogg.AddComment(_T("GENRE"),strTmp);

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_CMT));
				wnd.GetWindowText(strTmp);
				wnd.Detach();
				lpcs->m_Ogg.AddComment(_T("COMMENT"),strTmp);

				//その他分を保存
				long items = ListBox_GetCount(GetDlgItem(hDlg,IDC_LIST_VALUE));
				if(items != LB_ERR)
				{
					for(int i=0; i<items; i++)
					{
						COggExt *oggExt = (COggExt *)ListBox_GetItemData(GetDlgItem(hDlg,IDC_LIST_VALUE),i);
						if(oggExt)
						{
							lpcs->m_Ogg.AddComment(oggExt->GetName(),oggExt->GetValue());
						}
					}
				}

				//タイムスタンプを保存
				lpcs->PushTimeStamp(lpcs->m_strSelectFile);

				DWORD dwRet = lpcs->m_Ogg.Save(lpcs->m_strSelectFile);
				if(dwRet != ERROR_SUCCESS)
				{
					lpcs->m_bApply = TRUE;
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
				lpcs->m_Ogg.Load(lpcs->m_strSelectFile);
				DispInfo(hDlg,lpcs);
				DispInfoExt(hDlg,lpcs);

				SetWindowLongPtr(hDlg,DWLP_MSGRESULT,PSNRET_NOERROR);

				//シェルに変更を通知
				SHChangeNotify(SHCNE_UPDATEITEM,SHCNF_PATH,lpcs->m_strSelectFile,NULL);
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
