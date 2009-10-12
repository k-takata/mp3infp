#include "StdAfx.h"
#include "mp3infp.h"

static int ids[] =
{
	-1,
	-1,
	-1,
	CLP_NAM,
	CLP_CMT,
	CLP_ART,
	CLP_CRD,
	CLP_GNR,
	CLP_COP,
	CLP_ENG,
	CLP_SRC,
	CLP_SFT,
	-1,
	0
};

static int staticWnd[] =
{
	IDC_STATIC_FORMAT_A_,
	IDC_STATIC_FORMAT_V_,
	IDC_STATIC_TIME_,
	IDC_STATIC_NAM,
	IDC_STATIC_CMT,
	IDC_STATIC_ART,
	IDC_STATIC_CRD,
	IDC_STATIC_GNR,
	IDC_STATIC_COP,
	IDC_STATIC_ENG,
	IDC_STATIC_SRC,
	IDC_STATIC_SFT,
	IDC_STATIC_AVI_VER_,
	0
};

static int editWnd[] =
{
	IDC_STATIC_FORMAT_A,
	IDC_STATIC_FORMAT_V,
	IDC_STATIC_TIME,
	IDC_EDIT_SBJ,
	IDC_EDIT_CMT,
	IDC_EDIT_ART,
	IDC_EDIT_CRD,
	IDC_EDIT_GNR,
	IDC_EDIT_COP,
	IDC_EDIT_ENG,
	IDC_EDIT_SRC,
	IDC_EDIT_SFT,
	IDC_STATIC_AVI_VER,
	0
};

UINT CALLBACK CShellExt::PageCallback_avi(HWND hWnd,UINT uMessage,LPPROPSHEETPAGE ppsp)
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
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_FORMAT_A_),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_FORMAT_A),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_STATIC_FORMAT_A),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_FORMAT_V_),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_FORMAT_V),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_STATIC_FORMAT_V),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_TIME_),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_TIME),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_STATIC_TIME),_T(""));

	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_NAM),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_SBJ),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_SBJ),bEnable);

	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_CMT),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_CMT),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_CMT),bEnable);

	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_ART),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_ART),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_ART),bEnable);

	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_CRD),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_CRD),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_CRD),bEnable);

	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_GNR),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_GNR),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_GNR),bEnable);

	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_COP),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_COP),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_COP),bEnable);

	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_ENG),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_ENG),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_ENG),bEnable);

	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_SRC),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_SRC),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_SRC),bEnable);

	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_SFT),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_SFT),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_SFT),bEnable);
	
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_AVI_VER),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_STATIC_AVI_VER),_T(""));
}

static void DispInfo(HWND hDlg,CShellExt *lpcs)
{
	if(lpcs->m_OpenDML.IsEnable())
	{
		EnableEdit(hDlg,lpcs,TRUE);

		//avi
		CString strAudioFormat;
		CString strVideoFormat;
		CString strStreamFormat;
		CString strTime;
		BOOL bAvi2;

		GetAviFormat((LPCTSTR )lpcs->m_strSelectFile,
						strAudioFormat,
						strVideoFormat,
						strStreamFormat,
						strTime,
						bAvi2,
						lpcs->m_iAviCodecFind);
		strVideoFormat += _T("\r\n");
		strVideoFormat += strStreamFormat;
		SetDlgItemText(hDlg,IDC_STATIC_FORMAT_A,strAudioFormat);
		SetDlgItemText(hDlg,IDC_STATIC_FORMAT_V,strVideoFormat);
		SetDlgItemText(hDlg,IDC_STATIC_TIME,strTime);

		SetDlgItemText(hDlg,IDC_STATIC_AVI_VER,bAvi2?_T("AVI 2.0 (OpenDML)"):_T("AVI 1.0 (VfW)"));
		//ISBJ songname
//2002-03-17
//INAMを優先、無ければISBJを表示
//		SetDlgItemText(hDlg,IDC_EDIT_SBJ,lpcs->m_OpenDML.GetField('I','S','B','J'));
		CString tmp;
		tmp = lpcs->m_OpenDML.GetField('I','N','A','M');
		if(tmp.GetLength() == 0)
		{
			tmp = lpcs->m_OpenDML.GetField('I','S','B','J');
		}
		SetDlgItemText(hDlg,IDC_EDIT_SBJ,tmp);
		//IART アーティスト名
		SetDlgItemText(hDlg,IDC_EDIT_ART,lpcs->m_OpenDML.GetField('I','A','R','T'));
//		//IPRD アルバム名
//		SetDlgItemText(hDlg,IDC_EDIT_PRD,lpcs->m_OpenDML.GetField('I','P','R','D'));
		//ICMT コメント
		SetDlgItemText(hDlg,IDC_EDIT_CMT,lpcs->m_OpenDML.GetField('I','C','M','T'));
		//ICRD 日付
		SetDlgItemText(hDlg,IDC_EDIT_CRD,lpcs->m_OpenDML.GetField('I','C','R','D'));
		//IGNR ジャンル
		SetDlgItemText(hDlg,IDC_EDIT_GNR,lpcs->m_OpenDML.GetField('I','G','N','R'));
		//ICOP 著作権
		SetDlgItemText(hDlg,IDC_EDIT_COP,lpcs->m_OpenDML.GetField('I','C','O','P'));
		//IENG エンジニア	
		SetDlgItemText(hDlg,IDC_EDIT_ENG,lpcs->m_OpenDML.GetField('I','E','N','G'));
		//ISRC ソース	
		SetDlgItemText(hDlg,IDC_EDIT_SRC,lpcs->m_OpenDML.GetField('I','S','R','C'));
		//ISFT ソフトウェア
		SetDlgItemText(hDlg,IDC_EDIT_SFT,lpcs->m_OpenDML.GetField('I','S','F','T'));
	
		lpcs->m_bApply = FALSE;
		OutputDebugString(_T("ENABLE"));
	}
	else
	{
		EnableEdit(hDlg,lpcs,FALSE);
		OutputDebugString(_T("Disable"));
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

BOOL CALLBACK CShellExt::PageDlgProc_avi(HWND hDlg,UINT uMessage,WPARAM wParam,LPARAM lParam)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	TRACE(_T("PageDlgProc_avi(mes=%08x,wParam=%08x,lParam=%08x\r\n"),uMessage,wParam,lParam);
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
				SendMessage(GetDlgItem(hDlg,IDC_ICON1),
					STM_SETIMAGE,IMAGE_ICON,
					(LPARAM )sfi.hIcon);
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
*/			//情報を表示
			DispInfo(hDlg,lpcs);
			lpcs->m_bApply = FALSE;
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
		case IDC_EDIT_CRD:
		case IDC_EDIT_SBJ:
		case IDC_EDIT_ART:
//		case IDC_EDIT_PRD:
		case IDC_EDIT_CMT:
		case IDC_EDIT_COP:
		case IDC_EDIT_ENG:
		case IDC_EDIT_SRC:
		case IDC_EDIT_SFT:
		case IDC_EDIT_GNR:
			if((HIWORD(wParam) == EN_CHANGE) &&
				(IsWindowEnabled((HWND )lParam)) )
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
			ShellExecute(hDlg,_T("open"),_T("rundll32.exe"),_T("shell32.dll,Control_RunDLL mp3infp.cpl,,4"),NULL,SW_SHOW);
			break;
		case IDC_HELPVIEW:
			lpcs->OpenHtmlHelp(hDlg,_T("extension.htm"));
			break;
		DLG_CLIPBORD_MACRO(lpcs->m_strSelectFile);
		}
		break;
	// コンテキストメニュー
	case WM_CONTEXTMENU:
		DlgContextMenu(hDlg,lParam,lpcs->m_OpenDML.IsEnable());
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
			if(lpcs->m_bApply && lpcs->m_OpenDML.IsEnable())
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

				lpcs->m_bApply = FALSE;

				CString strTmp;
				CWnd wnd;
				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_SBJ));
				wnd.GetWindowText(strTmp);
//2002-03-17
//INAMを優先、ISBJは撤去
				lpcs->m_OpenDML.SetField('I','S','B','J',_T(""));
				lpcs->m_OpenDML.SetField('I','N','A','M',strTmp);
//				lpcs->m_OpenDML.SetField('I','S','B','J',strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_ART));
				wnd.GetWindowText(strTmp);
				lpcs->m_OpenDML.SetField('I','A','R','T',strTmp);
				wnd.Detach();

//				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_PRD));
//				wnd.GetWindowText(strTmp);
//				lpcs->m_OpenDML.SetField('I','P','R','D',strTmp);
//				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_CRD));
				wnd.GetWindowText(strTmp);
				lpcs->m_OpenDML.SetField('I','C','R','D',strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_GNR));
				wnd.GetWindowText(strTmp);
				lpcs->m_OpenDML.SetField('I','G','N','R',strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_CMT));
				wnd.GetWindowText(strTmp);
				lpcs->m_OpenDML.SetField('I','C','M','T',strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_COP));
				wnd.GetWindowText(strTmp);
				lpcs->m_OpenDML.SetField('I','C','O','P',strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_ENG));
				wnd.GetWindowText(strTmp);
				lpcs->m_OpenDML.SetField('I','E','N','G',strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_SRC));
				wnd.GetWindowText(strTmp);
				lpcs->m_OpenDML.SetField('I','S','R','C',strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_SFT));
				wnd.GetWindowText(strTmp);
				lpcs->m_OpenDML.SetField('I','S','F','T',strTmp);
				wnd.Detach();

				//タイムスタンプを保存
				hFile = CreateFile(
							lpcs->m_strSelectFile,
							GENERIC_READ,
							FILE_SHARE_READ|FILE_SHARE_WRITE,
							NULL,
							OPEN_EXISTING,	//ファイルをオープンします。指定ファイルが存在していない場合、関数は失敗します。
							FILE_ATTRIBUTE_NORMAL,
							NULL);
				if(hFile != INVALID_HANDLE_VALUE)
				{
					GetFileTime(hFile,&lpcs->m_createTime,NULL,&lpcs->m_fileTime);
					CloseHandle(hFile);
				}

				DWORD dwRet = lpcs->m_OpenDML.Save(hDlg,lpcs->m_strSelectFile);
				if(dwRet != ERROR_SUCCESS)
				{
					lpcs->m_bApply = TRUE;
					if(dwRet == -1)
						//「ファイルを正しく更新できませんでした。」
						AfxMessageBox(IDS_PAGE_BROKEN,MB_ICONINFORMATION);
					else if(dwRet == -2)
						//「AVIX-RIFFチャンクを含むAVI2(OpenDML)の書き換えには対応していません。」
						AfxMessageBox(IDS_AVI_UNCODE,MB_ICONINFORMATION);
					else
						//システムエラーを表示
						errMessageBox(hDlg,dwRet);
					//適用ボタンは引き続き有効
					SetWindowLongPtr(hDlg,DWLP_MSGRESULT,PSNRET_INVALID);
					break;
				}

				if(lpcs->m_bSaveTimeStamp)
				{
					//タイムスタンプを復元
					hFile = CreateFile(
								lpcs->m_strSelectFile,
								GENERIC_READ|GENERIC_WRITE,
								FILE_SHARE_READ|FILE_SHARE_WRITE,
								NULL,
								OPEN_EXISTING,	//ファイルをオープンします。指定ファイルが存在していない場合、関数は失敗します。
								FILE_ATTRIBUTE_NORMAL,
								NULL);
					if(hFile != INVALID_HANDLE_VALUE)
					{
						SetFileTime(hFile,&lpcs->m_createTime,NULL,&lpcs->m_fileTime);
						CloseHandle(hFile);
					}
				}

				//情報を表示
				DispInfo(hDlg,lpcs);
				lpcs->m_bApply = FALSE;

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