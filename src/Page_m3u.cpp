#include "StdAfx.h"
#include "mp3infp.h"

UINT CALLBACK CShellExt::PageCallback_m3u(HWND hWnd,UINT uMessage,LPPROPSHEETPAGE ppsp)
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
	TRACE(_T("DispInfo()\n"));

	TRACE(_T("DispInfo() - Load\n"));
	int i = 0;
	CString strTmp;
	CString strEdit;
	while(lpcs->m_M3u.GetLine(i++,strTmp))
	{
		strEdit += strTmp;
		strEdit += _T("\r\n");
	}
	SetDlgItemText(hDlg,IDC_EDIT_LIST,strEdit);

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

BOOL CALLBACK CShellExt::PageDlgProc_m3u(HWND hDlg,UINT uMessage,WPARAM wParam,LPARAM lParam)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	LPCSHELLEXT	lpcs = (CShellExt *)GetWindowLongPtr(hDlg,DWLP_USER);

	switch(uMessage){
	case WM_INITDIALOG:
		{
			lpcs = (LPCSHELLEXT )((LPPROPSHEETPAGE )lParam)->lParam;
			lpcs->m_hwndPage = hDlg;
			SetWindowLongPtr(hDlg, DWLP_USER, (LONG_PTR)lpcs);

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

			//情報を表示
			DispInfo(hDlg,lpcs);
			lpcs->m_bApply = FALSE;

			//スイッチを初期化
			switch(lpcs->m_bM3uProp_Fullpath){
			case 0:
				CheckRadioButton(hDlg,IDC_RADIO_PATH1,IDC_RADIO_PATH3,IDC_RADIO_PATH1);
				break;
			case 1:
				CheckRadioButton(hDlg,IDC_RADIO_PATH1,IDC_RADIO_PATH3,IDC_RADIO_PATH2);
				break;
			case 2:
			default:
				CheckRadioButton(hDlg,IDC_RADIO_PATH1,IDC_RADIO_PATH3,IDC_RADIO_PATH3);
				break;
			}
			DragAcceptFiles(hDlg,TRUE);//Drag&Drop on
		}
		break;

	//ファイルがドロップされたときの処理
	case WM_DROPFILES:
	{
		CString strTmp;
		CWnd wnd;
		wnd.Attach(hDlg);
		wnd.GetDlgItemText(IDC_EDIT_LIST,strTmp);
		wnd.Detach();

		TCHAR szFile[MAX_PATH];
		//ドロップされたファイル数
		UINT iFileNum=DragQueryFile((HDROP )wParam,-1,NULL,0);
		for(UINT i=0; i<iFileNum; i++)
		{
			DragQueryFile((HDROP )wParam,i,szFile,MAX_PATH);
			switch(lpcs->m_bM3uProp_Fullpath){
			case 0:	// 相対パスで追加
				if(_tcsncmp(szFile,lpcs->m_strSelectFile,3) == 0)
				{
					// 相対パスを作る
					int offset = 0;
					int m3uLen = lstrlen(lpcs->m_strSelectFile);
					int mp3Len = lstrlen(szFile);
					for(int j=0; j<((m3uLen<mp3Len)?m3uLen:mp3Len); j++)
					{
						if(IS_LEAD_TBYTE(lpcs->m_strSelectFile[j]) && IS_LEAD_TBYTE(szFile[j]))
						{
							j++;
							continue;
						}
						if(IS_LEAD_TBYTE(lpcs->m_strSelectFile[j]) || IS_LEAD_TBYTE(szFile[j]))
						{
							break;
						}
						if(lpcs->m_strSelectFile[j] != szFile[j])
						{
							break;
						}
						if(lpcs->m_strSelectFile[j] == '\\')
						{
							offset = j+1;
						}
					}
					int yenCount = 0;
					LPCTSTR yenPtr = &(((LPCTSTR )lpcs->m_strSelectFile)[offset]);
					while(1)
					{
						yenPtr = _tcschr(yenPtr+1,'\\');
						if(yenPtr == NULL)
						{
							break;
						}
						yenCount++;
						strTmp += _T("..\\");
					}
					strTmp += &szFile[offset];
				}
				else
				{
					// 相対パスにできない(ドライブが違う)
					strTmp += szFile;
				}
				break;
			case 1:	// フルパスで追加
				strTmp += szFile;
				break;
			case 2:	// ファイル名で追加
			default:
				strTmp += getFileNameExtName(szFile);
				break;
			}
			strTmp += _T("\r\n");
		}
		DragFinish((HDROP )wParam);

		//変更をエディットコントロールに反映
		SetWindowText(GetDlgItem(hDlg,IDC_EDIT_LIST),strTmp);
		PropSheet_Changed(GetParent(hDlg),hDlg);
		
		Edit_SetModify(GetDlgItem(hDlg,IDC_EDIT_LIST),TRUE);
		// カーソル位置を終端にセット
		Edit_SetSel(GetDlgItem(hDlg,IDC_EDIT_LIST),strTmp.GetLength(),strTmp.GetLength());
		break;
	}

	case WM_COMMAND:
	switch(LOWORD(wParam)){
		case IDC_EDIT_LIST:
			if((HIWORD(wParam) == EN_CHANGE) &&
				(IsWindowEnabled((HWND )lParam)) )
			{
				PropSheet_Changed(GetParent(hDlg),hDlg);
			}
			break;
		case IDC_RADIO_PATH1:
		case IDC_RADIO_PATH2:
		case IDC_RADIO_PATH3:
			if(IsDlgButtonChecked(hDlg,IDC_RADIO_PATH1) == BST_CHECKED)
			{
				lpcs->m_bM3uProp_Fullpath = 0;
			}
			else if(IsDlgButtonChecked(hDlg,IDC_RADIO_PATH2) == BST_CHECKED)
			{
				lpcs->m_bM3uProp_Fullpath = 1;
			}
			else if(IsDlgButtonChecked(hDlg,IDC_RADIO_PATH3) == BST_CHECKED)
			{
				lpcs->m_bM3uProp_Fullpath = 2;
			}
			regSetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("m3u_Prop_Fullpath"),(DWORD )lpcs->m_bM3uProp_Fullpath);
			break;
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
			OpenConfigPage(hDlg,7);
			break;
		case IDC_HELPVIEW:
			lpcs->OpenHtmlHelp(hDlg,_T("extension.htm"));
			break;
		}
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
			TRACE(_T("WM_NOTIFY(PSN_APPLY)\n"));
			if(Edit_GetModify(GetDlgItem(hDlg,IDC_EDIT_LIST)))
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

				CString strTmp,strLine;
				CWnd wnd;
				wnd.Attach(hDlg);
				wnd.GetDlgItemText(IDC_EDIT_LIST,strTmp);
				wnd.Detach();

				lpcs->m_M3u.Release();
				//終端に改行をつける
				if( (strTmp.GetLength() > 1) &&
					(strTmp.Right(2) != _T("\r\n")) )
				{
					strTmp += _T("\r\n");
				}
				int start = 0;
				int end = 0;
				while((end = strTmp.Find(_T("\r\n"),start)) != -1)
				{
					strLine = strTmp.Mid(start,end-start);
					lpcs->m_M3u.Add(strLine);
					TRACE(_T("lpcs->m_M3u.Add(%s)\n"),strLine);
					
					start = end + 2;
				}

				//タイムスタンプを保存
				lpcs->PushTimeStamp(lpcs->m_strSelectFile);

				DWORD dwRet = lpcs->m_M3u.Save(lpcs->m_strSelectFile);
				if(dwRet != ERROR_SUCCESS)
				{
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
				DispInfo(hDlg,lpcs);
				lpcs->m_bApply = FALSE;

				SetWindowLongPtr(hDlg,DWLP_MSGRESULT,PSNRET_NOERROR);
				Edit_SetModify(GetDlgItem(hDlg,IDC_EDIT_LIST),FALSE);

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
