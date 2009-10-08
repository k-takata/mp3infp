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
	COggExt(char *name,char *value)
	{
		m_strName = name;
		m_strValue = value;
	};
	virtual ~COggExt()
	{
	};

	CString GetName(){return m_strName;};
	void SetName(char *name){m_strName = name;};
	CString GetValue(){return m_strValue;};
	void SetValue(char *value){m_strValue = value;};
	CString Get1LineDisp()
	{
		//改行があるときは(...)に置き換え
		CString strValue;
		int crlf = m_strValue.FindOneOf("\r\n");
		if(crlf != -1)
		{
			strValue = CString(m_strValue,crlf) + "(...)";
		}
		else
		{
			strValue = m_strValue;
		}
		return m_strName + "=" + strValue;
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
		strAvg.Format(" %ld(avg)",lpcs->m_Ogg.GetBitrateAvg());
	CString strUpper;
	if(lpcs->m_Ogg.GetBitrateUpper() != -1)
		strUpper.Format(" %ld(upper)",lpcs->m_Ogg.GetBitrateUpper());
	CString strNominal;
	if(lpcs->m_Ogg.GetBitrateNominal() != -1)
		strNominal.Format(" %ld(nominal)",lpcs->m_Ogg.GetBitrateNominal());
	CString strLower;
	if(lpcs->m_Ogg.GetBitrateLower() != -1)
		strLower.Format(" %ld(lower)",lpcs->m_Ogg.GetBitrateLower());
	CString strWindow;
	if(lpcs->m_Ogg.GetBitrateWindow() != 0)
		strWindow.Format(" %ld(window)",lpcs->m_Ogg.GetBitrateWindow());

	CString tmp;
	tmp.Format( "Version = %ld ,Serial=%ld\n"
				"Bitrate = %s%s%s%s%s\n"
				"Vendor = %s\n",
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

	lpcs->m_Ogg.GetComment("TITLE",0,tmp);
	SetDlgItemText(hDlg,IDC_EDIT_SBJ,tmp);

	lpcs->m_Ogg.GetComment("TRACKNUMBER",0,tmp);
	SetDlgItemText(hDlg,IDC_EDIT_TRK,tmp);
	
	lpcs->m_Ogg.GetComment("ARTIST",0,tmp);
	SetDlgItemText(hDlg,IDC_EDIT_ART,tmp);
	
	lpcs->m_Ogg.GetComment("ALBUM",0,tmp);
	SetDlgItemText(hDlg,IDC_EDIT_PRD,tmp);
	
	lpcs->m_Ogg.GetComment("DATE",0,tmp);
	SetDlgItemText(hDlg,IDC_EDIT_CRD,tmp);
	
	lpcs->m_Ogg.GetComment("GENRE",0,tmp);
	SetDlgItemText(hDlg,IDC_EDIT_GNR,tmp);
	
	lpcs->m_Ogg.GetComment("COMMENT",0,tmp);
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
	long items = SendMessage(GetDlgItem(hDlg,IDC_LIST_VALUE),LB_GETCOUNT,0,0);
	if(items != LB_ERR)
	{
		for(int i=0; i<items; i++)
		{
			COggExt *oggExt = (COggExt *)SendMessage(GetDlgItem(hDlg,IDC_LIST_VALUE),LB_GETITEMDATA,0,0);
			if(oggExt)
			{
				delete oggExt;
			}
			SendMessage(GetDlgItem(hDlg,IDC_LIST_VALUE),LB_DELETESTRING,0,0);
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
				!strName.Compare("TITLE") ||
				!strName.Compare("TRACKNUMBER") ||
				!strName.Compare("ARTIST") ||
				!strName.Compare("ALBUM") ||
				!strName.Compare("DATE") ||
				!strName.Compare("GENRE") ||
				!strName.Compare("COMMENT"))
				)
			{
				continue;
			}
//			TRACE("OGG %s %s\n",strName,strValue);
			COggExt *oggExt = new COggExt((char *)(LPCSTR )strName,(char *)(LPCSTR )strValue);
			SendMessage(GetDlgItem(hDlg,IDC_LIST_VALUE),LB_ADDSTRING,0,(LPARAM )(LPCTSTR )oggExt->Get1LineDisp());
			SendMessage(GetDlgItem(hDlg,IDC_LIST_VALUE),LB_SETITEMDATA,items,(LPARAM )oggExt);
			items++;
		}
	}
}

BOOL CALLBACK CShellExt::PageDlgProc_ogg(HWND hDlg,UINT uMessage,WPARAM wParam,LPARAM lParam)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	LPCSHELLEXT	lpcs = (CShellExt *)GetWindowLong(hDlg,DWL_USER);
	CString strTmp;
	HANDLE hFile;

	switch(uMessage){
	case WM_INITDIALOG:
		{
			lpcs = (LPCSHELLEXT )((LPPROPSHEETPAGE )lParam)->lParam;
			lpcs->m_hwndPage = hDlg;
			SetWindowLong(hDlg, DWL_USER, (DWORD )lpcs);
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
					(LPARAM )(LPCSTR )""	//空白
				);
			for(int i=0; i<256; i++)
			{
				if(lpcs->m_Id3tagv1.GenreNum2String(i).GetLength())
					SendMessage(
							GetDlgItem(hDlg,IDC_EDIT_GNR),
							CB_ADDSTRING,
							0,
							(LPARAM )(LPCSTR )lpcs->m_Id3tagv1.GenreNum2String(i)
						);
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
					COggExt *oggExt = new COggExt((char *)(LPCSTR )dlg.m_strName,(char *)(LPCSTR )dlg.m_strValue);
					long index = SendMessage(GetDlgItem(hDlg,IDC_LIST_VALUE),LB_ADDSTRING,0,(LPARAM )(LPCTSTR )oggExt->Get1LineDisp());
					if(index == LB_ERR)
					{
						delete oggExt;
					}
					else
					{
						SendMessage(GetDlgItem(hDlg,IDC_LIST_VALUE),LB_SETITEMDATA,index,(LPARAM )oggExt);
					}
				
					PropSheet_Changed(GetParent(hDlg),hDlg);
					lpcs->m_bApply = TRUE;
				}
			}
			break;
		case IDC_BUTTON_DEL_VALUE:
			//値削除
			{
				long item = SendMessage(GetDlgItem(hDlg,IDC_LIST_VALUE),LB_GETCURSEL,0,0);
				if(item == LB_ERR)
					break;
				
				COggExt *oggExt = (COggExt *)SendMessage(GetDlgItem(hDlg,IDC_LIST_VALUE),LB_GETITEMDATA,item,0);
				if(oggExt)
				{
					delete oggExt;
				}
				SendMessage(GetDlgItem(hDlg,IDC_LIST_VALUE),LB_DELETESTRING,item,0);
				
				PropSheet_Changed(GetParent(hDlg),hDlg);
				lpcs->m_bApply = TRUE;
			}
			break;
		case IDC_LIST_VALUE:
			if(HIWORD(wParam) == LBN_DBLCLK)
			{
				//ダブルクリック - 編集
				long item = SendMessage(GetDlgItem(hDlg,IDC_LIST_VALUE),LB_GETCURSEL,0,0);
				if(item == LB_ERR)
					break;
				
				COggExt *oggExt = (COggExt *)SendMessage(GetDlgItem(hDlg,IDC_LIST_VALUE),LB_GETITEMDATA,item,0);
				if(oggExt == NULL)
					break;
				
				CDlg_ogg_exttag dlg;

				dlg.m_strName = oggExt->GetName();
				dlg.m_strValue = oggExt->GetValue();
				if(dlg.DoModal() == IDOK)
				{
					oggExt->SetName((char *)(LPCSTR )dlg.m_strName);
					oggExt->SetValue((char *)(LPCSTR )dlg.m_strValue);
					SendMessage(GetDlgItem(hDlg,IDC_LIST_VALUE),LB_DELETESTRING,item,0);
					SendMessage(GetDlgItem(hDlg,IDC_LIST_VALUE),LB_ADDSTRING,item,(LPARAM )(LPCTSTR )oggExt->Get1LineDisp());
					SendMessage(GetDlgItem(hDlg,IDC_LIST_VALUE),LB_SETITEMDATA,item,(LPARAM )oggExt);
					
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
				PlayWinamp(hDlg,(char *)(LPCSTR )lpcs->m_strSelectFile);
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
			regSetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"PropAOT",(DWORD )lpcs->m_bPropAOT);
			break;
		case IDC_SETUP:
			ShellExecute(hDlg,"open","rundll32.exe","shell32.dll,Control_RunDLL mp3infp.cpl,,8",NULL,SW_SHOW);
			break;
		case IDC_HELPVIEW:
			lpcs->OpenHtmlHelp(hDlg,"extension.htm");
			break;
		DLG_CLIPBORD_MACRO(lpcs->m_strSelectFile);
		}
		break;
	case WM_DESTROY:
		{
			//後始末
			long items = SendMessage(GetDlgItem(hDlg,IDC_LIST_VALUE),LB_GETCOUNT,0,0);
			if(items != LB_ERR)
			{
				for(int i=0; i<items; i++)
				{
					COggExt *oggExt = (COggExt *)SendMessage(GetDlgItem(hDlg,IDC_LIST_VALUE),LB_GETITEMDATA,i,0);
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
		char szTmp[256];
		strcpy(szTmp,APP_NAME);
		strcat(szTmp," ");
		strcat(szTmp,COPY_RIGHT);
		MessageBox(hDlg,szTmp,"About",MB_APPLMODAL);
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
				TRACE("WM_NOTIFY(PSN_APPLY) - 保存\n");
				//ファイルが書き込み可能か調べる
				if(GetFileAttributes(lpcs->m_strSelectFile) & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY))
				{
					//「このファイルは「読み込み専用」です」
					AfxMessageBox(IDS_PAGE_THIS_READONLY,MB_ICONINFORMATION);
					//適用ボタンは引き続き有効
					SetWindowLong(hDlg,DWL_MSGRESULT,PSNRET_INVALID);
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
				lpcs->m_Ogg.AddComment("TITLE",strTmp);

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_TRK));
				wnd.GetWindowText(strTmp);
				wnd.Detach();
				lpcs->m_Ogg.AddComment("TRACKNUMBER",strTmp);

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_ART));
				wnd.GetWindowText(strTmp);
				wnd.Detach();
				lpcs->m_Ogg.AddComment("ARTIST",strTmp);

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_PRD));
				wnd.GetWindowText(strTmp);
				wnd.Detach();
				lpcs->m_Ogg.AddComment("ALBUM",strTmp);

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_CRD));
				wnd.GetWindowText(strTmp);
				wnd.Detach();
				lpcs->m_Ogg.AddComment("DATE",strTmp);

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_GNR));
				wnd.GetWindowText(strTmp);
				wnd.Detach();
				lpcs->m_Ogg.AddComment("GENRE",strTmp);

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_CMT));
				wnd.GetWindowText(strTmp);
				wnd.Detach();
				lpcs->m_Ogg.AddComment("COMMENT",strTmp);

				//その他分を保存
				long items = SendMessage(GetDlgItem(hDlg,IDC_LIST_VALUE),LB_GETCOUNT,0,0);
				if(items != LB_ERR)
				{
					for(int i=0; i<items; i++)
					{
						COggExt *oggExt = (COggExt *)SendMessage(GetDlgItem(hDlg,IDC_LIST_VALUE),LB_GETITEMDATA,i,0);
						if(oggExt)
						{
							lpcs->m_Ogg.AddComment(oggExt->GetName(),oggExt->GetValue());
						}
					}
				}

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
					SetWindowLong(hDlg,DWL_MSGRESULT,PSNRET_INVALID);
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
				lpcs->m_Ogg.Load(lpcs->m_strSelectFile);
				DispInfo(hDlg,lpcs);
				DispInfoExt(hDlg,lpcs);

				SetWindowLong(hDlg,DWL_MSGRESULT,PSNRET_NOERROR);

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