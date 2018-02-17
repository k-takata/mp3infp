#include "StdAfx.h"
#include "mp3infp.h"

#include "GetFloppyType.h"

#ifdef _DEBUG
#include "psapi.h"
#pragma comment(lib,"psapi.lib")
#endif

void DlgContextMenu(HWND hDlg,LPARAM lParam,BOOL isEnable)
{
	HMENU hMenu = LoadMenu(AfxGetResourceHandle(),MAKEINTRESOURCE(IDR_MENU_PROPERTY_CONTEXT));
	HMENU hPopup = GetSubMenu(hMenu,0);
	EnableMenuItem(hPopup,ID_MENU_PASTE,MF_GRAYED);
	EnableMenuItem(hPopup,ID_MENU_COPY,MF_GRAYED);
	if(isEnable)
	{
		EnableMenuItem(hPopup,ID_MENU_COPY,MF_ENABLED);
		if(OpenClipboard(hDlg))
		{
			UINT cfMp3nfp = RegisterClipboardFormat(CF_MP3INFP);
			HANDLE hText = GetClipboardData(cfMp3nfp);
			if(hText)
			{
				EnableMenuItem(hPopup,ID_MENU_PASTE,MF_ENABLED);
				GlobalUnlock(hText);
			}
			CloseClipboard();
		}
	}
	TrackPopupMenu(hPopup,
					TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,
					GET_X_LPARAM(lParam),
					GET_Y_LPARAM(lParam),
					0,
					hDlg,
					0);
	DestroyMenu(hMenu);
}

static bool IsButton(HWND hwnd)
{
	TCHAR szClassName[100];
	GetClassName(hwnd, szClassName, sizeof_array(szClassName));
	return lstrcmp(szClassName, _T("Button")) == 0;
}

void SetDlgOutlineTextSp(HWND hDlg,const int *idArray,const int *editWndArray)
{
	if(!OpenClipboard(hDlg))
	{
		AfxMessageBox(_T("clipboard fail!"));
		return;
	}

	UINT cfMp3nfp = RegisterClipboardFormat(CF_MP3INFP);
	HANDLE hText = GetClipboardData(cfMp3nfp);
	if(!hText)
	{
		return;
	}
	
	char *txtData = (char *)GlobalLock(hText);
	if(!txtData)
	{
		return;
	}

	int readOffset = 0;
	while(*(int *)(&(txtData[readOffset])))
	{
		int id = *(int *)(&(txtData[readOffset]));
		readOffset += sizeof(int);
		int len = *(int *)(&(txtData[readOffset]));
		readOffset += sizeof(int);
		if(id != -1)
		{
			for(int i=0; idArray[i]!=0; i++)
			{
				if(idArray[i] == id)
				{
					if(!IsButton(GetDlgItem(hDlg,editWndArray[i])))
					{
						// Edit
						SetWindowText(GetDlgItem(hDlg,editWndArray[i]),(LPCTSTR)&(txtData[readOffset]));
					}
					else
					{
						// Checkbox
						int val = _ttoi((LPCTSTR)&(txtData[readOffset]));
						if(val)
						{
							CheckDlgButton(hDlg,editWndArray[i],1);
						}
						else
						{
							CheckDlgButton(hDlg,editWndArray[i],0);
						}
					}
					break;
				}
			}
		}
		readOffset += (len + 1) * sizeof(TCHAR);
	}
		
	GlobalUnlock(hText);
	CloseClipboard();
}

HGLOBAL GetDlgOutlineTextSp(HWND hDlg,const int *idArray,const int *editWndArray)
{
	CString strTmp;
	CWnd wnd;
	int totalLen = 0;
	int i=0;
	for(; idArray[i]!=0; i++)
	{
		totalLen += 4/*ID*/ + 4/*Size*/;
		// The following size is not accurate for a checkbox, but it would be enough.
		totalLen += (GetWindowTextLength(GetDlgItem(hDlg,editWndArray[i]))+1)*sizeof(TCHAR);
	}
	totalLen += 4;	// end mark
	
	HGLOBAL hg = GlobalAlloc(GHND,totalLen);
	char *txtData = (char *)GlobalLock(hg);
	int writeOffset = 0;
				
	for(i=0; idArray[i]!=0; i++)
	{
		*(int *)(&(txtData[writeOffset])) = idArray[i];
		writeOffset += sizeof(int);

		if(!IsButton(GetDlgItem(hDlg,editWndArray[i])))
		{
			// editbox
			wnd.Attach(GetDlgItem(hDlg,editWndArray[i]));
			wnd.GetWindowText(strTmp);
			wnd.Detach();
		}
		else
		{
			// checkbox
			if(IsDlgButtonChecked(hDlg,editWndArray[i]))
			{
				strTmp = _T("1");
			}
			else
			{
				strTmp = _T("0");
			}
		}
		*(int *)(&(txtData[writeOffset])) = strTmp.GetLength();
		writeOffset += sizeof(int);
		lstrcpy((LPTSTR)&(txtData[writeOffset]),strTmp);
		writeOffset += (strTmp.GetLength() + 1) * sizeof(TCHAR);
	}
	*(int *)(&(txtData[writeOffset])) = 0;	// end mark
	writeOffset += sizeof(int);
	ASSERT(totalLen >= writeOffset);

	GlobalUnlock(hg);

	return hg;
}

HGLOBAL GetDlgOutlineText(HWND hDlg,const int *staticWndArray,const int *editWndArray,CString strFileName)
{
	CString strTmp;
	CWnd wnd;

	CString strData = strFileName;
	strData += _T("\r\n");

	for(int i=0; editWndArray[i]!=0; i++)
	{
		if(staticWndArray[i] != -1)
		{
			wnd.Attach(GetDlgItem(hDlg,staticWndArray[i]));
			wnd.GetWindowText(strTmp);
			wnd.Detach();
			strData += strTmp + _T("\t");
		}

		if(!IsButton(GetDlgItem(hDlg,editWndArray[i])))
		{
			// editbox
			if(editWndArray[i] != -1)
			{
				wnd.Attach(GetDlgItem(hDlg,editWndArray[i]));
				wnd.GetWindowText(strTmp);
				wnd.Detach();
				strData += strTmp;
				if(staticWndArray[i+1] != -1)	// トラック番号など第二値を / 区切りする
				{
					strData += _T("\r\n");
				}
				else
				{
					strData += _T(" / ");
				}
			}
		}
		else
		{
			// checkbox
			if(IsDlgButtonChecked(hDlg,editWndArray[i]))
			{
				strData += _T("Yes");
			}
			else
			{
				strData += _T("No");
			}
			strData += _T("\r\n");
		}
	}

	HGLOBAL hg = GlobalAlloc(GHND,(strData.GetLength()+1)*sizeof(TCHAR));
	TCHAR *txtData = (TCHAR *)GlobalLock(hg);
	lstrcpy(txtData,strData);
	GlobalUnlock(hg);

	return hg;
}

void OpenConfigPage(HWND hwnd, int pagenum)
{
	// install path of mp3infp
	TCHAR szMp3infpPath[MAX_PATH];
	regGetString(HKEY_LOCAL_MACHINE,MP3INFP_REG_ENTRY,_T("path"),szMp3infpPath,_T(""));
	AddTailYenSign(szMp3infpPath);
	CString strConfigPath = szMp3infpPath;
	strConfigPath += _T("mp3infp_config.exe");
	CString strParam;
	strParam.Format(_T("%d"),pagenum);

	ShellExecute(hwnd,_T("open"),strConfigPath,strParam,NULL,SW_SHOW);
}

void OpenAboutDlg(HWND hwnd)
{
	TCHAR szTmp[256];
	lstrcpy(szTmp,APP_NAME);
	lstrcat(szTmp,_T(" "));
	lstrcat(szTmp,COPY_RIGHT);
	MessageBox(hwnd,szTmp,_T("About"),MB_APPLMODAL);
}

CShellExt::CShellExt()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#ifdef _DEBUG
	TCHAR szProcessName[MAX_PATH];
	HANDLE hProcess = GetCurrentProcess();
	GetModuleBaseName(hProcess,NULL,szProcessName,sizeof_array(szProcessName));
	TRACE(_T("[%s]CShellExt::CShellExt(%d->%d) process=%s\n"),APP_NAME,g_cRefThisDll,g_cRefThisDll+1,szProcessName);
#endif

	m_cRef = 1L;	//Inside DCOM参照
	m_pDataObj = NULL;
	g_cRefThisDll++;

	//オプションを取得
	ConfigLoad();
	
	m_fileType = UNKNOWN;
	m_bMultiSelect = FALSE;
	m_timestamp.Clear();
}

CShellExt::~CShellExt()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#ifdef _DEBUG
	TCHAR szProcessName[MAX_PATH];
	HANDLE hProcess = GetCurrentProcess();
	GetModuleBaseName(hProcess,NULL,szProcessName,sizeof_array(szProcessName));
	TRACE(_T("[%s]CShellExt::~CShellExt(%d->%d) process=%s\n"),APP_NAME,g_cRefThisDll,g_cRefThisDll-1,szProcessName);
#endif

	if(m_pDataObj)
		m_pDataObj->Release();
	g_cRefThisDll--;
}

STDMETHODIMP CShellExt::QueryInterface(REFIID riid, LPVOID FAR *ppv)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TRACE(_T("[%s]CShellExt::QueryInterface() ref=%d\n"),APP_NAME,m_cRef+1);

	*ppv = NULL;

	if(!theApp.m_hResource)
	{
		TRACE(_T("[%s]リソースDLLをロードできなかった\n"),APP_NAME);
		return E_NOINTERFACE;
	}

	if(IsEqualIID(riid,IID_IShellExtInit) || IsEqualIID(riid,IID_IUnknown))
	{
		*ppv = (LPSHELLEXTINIT )this;
		TRACE(_T("[%s]   +============> IID_IShellExtInit or IID_IUnknown\n"),APP_NAME);
	}
	else if(IsEqualIID(riid,IID_IContextMenu))
	{
		*ppv = (LPCONTEXTMENU )this;
		TRACE(_T("[%s]   +============> IID_IContextMenu\n"),APP_NAME);
	}
	else if(IsEqualIID(riid,IID_IShellPropSheetExt))
	{
		*ppv = (LPSHELLPROPSHEETEXT )this;
		TRACE(_T("[%s]   +============> IID_IShellPropSheetExt\n"),APP_NAME);
	}
	else if(IsEqualIID(riid,IID_IQueryInfo))
	{
		*ppv = (IQueryInfo *)this;
		TRACE(_T("[%s]   +============> IID_IQueryInfo\n"),APP_NAME);
	}
	else if(IsEqualIID(riid,IID_IPersist))
	{
		*ppv = (IPersist *)this;
		TRACE(_T("[%s]   +============> IID_IPersist\n"),APP_NAME);
	}
	else if(IsEqualIID(riid,IID_IPersistFile))
	{
		*ppv = (IPersistFile *)this;
		TRACE(_T("[%s]   +============> IID_IPersistFile\n"),APP_NAME);
	}
	else if(IsEqualIID(riid,IID_IColumnProvider))
	{
		*ppv = (IColumnProvider *)this;
		TRACE(_T("[%s]   +============> IID_IColumnProvider\n"),APP_NAME);
	}
	if(*ppv)
	{
		AddRef();
		return NOERROR;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CShellExt::AddRef()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TRACE(_T("[%s]CShellExt::AddRef(%d->%d)\n"),APP_NAME,m_cRef,m_cRef+1);

	return ++m_cRef;
}

STDMETHODIMP_(ULONG) CShellExt::Release()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TRACE(_T("[%s]CShellExt::Release(%d->%d)\n"),APP_NAME,m_cRef,m_cRef-1);

	if(--m_cRef)
		return m_cRef;

	delete this;

	return 0L;
}

CShellExt::EN_FILETYPE CShellExt::GetFileType(LPCTSTR szFileName)
{
	LPCTSTR szExt = getExtName(szFileName);
	if((lstrcmpi(szExt,_T(".mp3")) == 0) ||
		(lstrcmpi(szExt,_T(".mp2")) == 0) ||
		(lstrcmpi(szExt,_T(".mp1")) == 0) ||
		(lstrcmpi(szExt,_T(".rmp")) == 0) )
	{
		DWORD dwRet;
		HANDLE hFile = CreateFile(
					szFileName,
					GENERIC_READ,
					FILE_SHARE_READ,
					NULL,
					OPEN_EXISTING,			//指定したファイルが存在していない場合、この関数は失敗します。 
					FILE_ATTRIBUTE_NORMAL,
					NULL);
		if(hFile != INVALID_HANDLE_VALUE)
		{
			//RIFF/WAVEヘッダがある場合はWAVEとみなす
			char buf[16];
			if(ReadFile(hFile,&buf,16,&dwRet,NULL) && (dwRet == 16))
			{
				buf[4] = '_';
				buf[5] = '_';
				buf[6] = '_';
				buf[7] = '_';
				if((dwRet == 16) && (memcmp(buf,"RIFF____WAVEfmt ",16) == 0))
				{
					CloseHandle(hFile);
					return WAVE;
				}
			}
			CloseHandle(hFile);
		}
		return MP3;
	}
	/*else*/ if(lstrcmpi(szExt,_T(".wav")) == 0)
	{
		return WAVE;
	}
	/*else*/ if(lstrcmpi(szExt,_T(".avi")) == 0)
	{
		return AVI;
	}
	/*else*/ if(lstrcmpi(szExt,_T(".vqf")) == 0)
	{
		return VQF;
	}
	/*else*/ if((lstrcmpi(szExt,_T(".wma")) == 0) ||
			(lstrcmpi(szExt,_T(".wmv")) == 0) ||
			(lstrcmpi(szExt,_T(".asf")) == 0) )
	{
		return WMA;
	}
	/*else*/ if((lstrcmpi(szExt,_T(".m3u")) == 0) ||
			(lstrcmpi(szExt,_T(".m3u8")) == 0) )
	{
		return M3U;
	}
	/*else*/ if(lstrcmpi(szExt,_T(".ogg")) == 0)
	{
		return OGG;
	}
	/*else*/ if(lstrcmpi(szExt,_T(".ape")) == 0)
	{
		return APE;
	}
	/*else*/ if((lstrcmpi(szExt,_T(".mp4")) == 0) ||
			(lstrcmpi(szExt,_T(".m4v")) == 0) ||
			(lstrcmpi(szExt,_T(".m4a")) == 0) ||
			(lstrcmpi(szExt,_T(".3gp")) == 0) ||
			(lstrcmpi(szExt,_T(".3g2")) == 0) )
	{
		return MP4;
	}

	DWORD dwUseExtra=0;
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("UseExperimentalSupport"),&dwUseExtra,FALSE);
	if(dwUseExtra){
		if(lstrcmpi(szExt,_T(".tta")) == 0)return MP3;
		if(lstrcmpi(szExt,_T(".tak")) == 0)return APE;
	}

	return UNKNOWN;
}

BOOL CShellExt::Load()
{
	TRACE(_T("[%s]   Load(%s)\n"),APP_NAME,m_strSelectFile);

	m_fileType = UNKNOWN;
	m_bMultiSelect = FALSE;

	//mp3infpを使用しないドライブの種類を指定
	if(m_bSelectDrive)
	{
		TCHAR szDrive[10];
		lstrcpyn(szDrive,m_strSelectFile,4);
		szDrive[3] = '\0';
		if(szDrive[0] == '\\')
		{
			//UNC
			if(m_bDisable_NETWORK)
				return FALSE;
		}
		int iFddType;
		switch(GetDriveType(szDrive)){
		case DRIVE_REMOVABLE:
			iFddType = GetDriveFormFactor(_totupper(szDrive[0])-'A'+1);
			if((iFddType == 525) || (iFddType == 350))
			{
				if(m_bDisable_FDD)//フロッピーディスクドライブ
					return FALSE;
			}
			if(m_bDisable_REMOVABLE)	//リムーバブルドライブ
				return FALSE;
			break;
		case DRIVE_CDROM:		//CDROMドライブ
			if(m_bDisable_CDROM)
				return FALSE;
			break;
		case DRIVE_REMOTE:	//ネットワークドライブ
			if(m_bDisable_NETWORK)
				return FALSE;
			break;
		}
	}

	switch(GetFileType(m_strSelectFile))
	{
	case MP3:
		if(!m_bMp3PropEnable && !m_bMp3InfotipEnable && !m_bMp3ColumnEnable)
		{
			return FALSE;
		}
		m_Id3tagv1.SetScmpxGenre(m_bId3v1ScmpxGenre);
		m_Id3tagv1.Load(m_strSelectFile);
		m_Id3tagv2.SetEncDefault(m_strRmpSoft);
		m_Id3tagv2.Load(m_strSelectFile);
		m_Rmp3.SetSftDefault(m_strRmpSoft);
		m_Rmp3.SetScmpxGenre(m_bId3v1ScmpxGenre);
		m_Rmp3.Load(m_strSelectFile);
		m_mp3info.Load(m_strSelectFile);
		m_Ape.SetDonotsaveId3v1(FALSE);
		m_Ape.Load(m_strSelectFile);
		if(m_Rmp3.IsEnable())
		{
			m_Ape.Release();
		}
		m_fileType = MP3;
		break;
	case WAVE:
		if(!m_bWavePropEnable && !m_bWaveInfotipEnable && !m_bWaveColumnEnable)
		{
			return FALSE;
		}
		if(m_RiffSIF.Load(m_strSelectFile,'W','A','V','E') == ERROR_SUCCESS)
		{
			m_fileType = WAVE;
			m_Id3tagv1.SetScmpxGenre(m_bId3v1ScmpxGenre);
		}
		break;
	case AVI:
		if(!m_bAviPropEnable && !m_bAviInfotipEnable && !m_bAviColumnEnable)
		{
			return FALSE;
		}
		if(m_OpenDML.Load(m_strSelectFile,'A','V','I',' ') == ERROR_SUCCESS)
		{
			m_fileType = AVI;
		}
		m_OpenDML.SetJunkHeader(_T("This file was made by ") SOFT_NAME);
		break;
	case VQF:
		if(!m_bVqfPropEnable &&	!m_bVqfInfotipEnable && !m_bVqfColumnEnable)
		{
			return FALSE;
		}
		if(m_Vqf.Load(m_strSelectFile) == ERROR_SUCCESS)
		{
			m_fileType = VQF;
		}
		break;
	case WMA:
		if(!m_bWmaPropEnable && !m_bWmaInfotipEnable && !m_bWmaColumnEnable)
		{
			return FALSE;
		}
		if(m_Wma.Load(m_strSelectFile) == ERROR_SUCCESS)
		{
			m_fileType = WMA;
		}
		break;
	case M3U:
		if(!m_bM3uPropEnable && !m_bM3uInfotipEnable)
		{
			return FALSE;
		}
		if(m_M3u.Load(m_strSelectFile) == ERROR_SUCCESS)
		{
			m_fileType = M3U;
		}
		break;
	case OGG:
		if(!m_bOggPropEnable && !m_bOggInfotipEnable && !m_bOggColumnEnable)
		{
			return FALSE;
		}
		if(m_Ogg.Load(m_strSelectFile) == ERROR_SUCCESS)
		{
			m_fileType = OGG;
		}
		break;
	case APE:
		if(!m_bApePropEnable && !m_bApeInfotipEnable && !m_bApeColumnEnable)
		{
			return FALSE;
		}
		if(m_Monkeys.Load(m_strSelectFile) == ERROR_SUCCESS)
		{
			m_Ape.SetDonotsaveId3v1(TRUE);
			m_Ape.Load(m_strSelectFile);
			m_fileType = APE;
		}
		break;
	case MP4:
		if(!m_bMp4PropEnable && !m_bMp4InfotipEnable && !m_bMp4ColumnEnable)
		{
			return FALSE;
		}
		if(m_Mp4.Load(m_strSelectFile) == ERROR_SUCCESS)
		{
			m_fileType = MP4;
		}
		break;
	case UNKNOWN:
		break;
	}

	// ファイルサイズの取得
	m_i64FileSize = 0;
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(m_strSelectFile,&fd);
	if(hFind != INVALID_HANDLE_VALUE)
	{
		FindClose(hFind);
		m_i64FileSize = (((__int64 )fd.nFileSizeHigh)<<32)|(__int64 )fd.nFileSizeLow;
	}

	return TRUE;
}

void CShellExt::OpenHtmlHelp(HWND hWnd,LPCTSTR szViewFile)
{
	CString strHelpPath;
	// mp3infpのインストールパス
	TCHAR szMp3infpPath[MAX_PATH];
	regGetString(HKEY_LOCAL_MACHINE,MP3INFP_REG_ENTRY,_T("path"),szMp3infpPath,_T(""));
	AddTailYenSign(szMp3infpPath);
	// 選択言語
	CString strLanguage = regGetStringEx(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("Language"),DEF_SETUP_MAIN_LANGUAGE);
	if(strLanguage.Compare(DEF_SETUP_MAIN_LANGUAGE) == 0)
	{
		// デフォルト選択
		strHelpPath = szMp3infpPath;
		strHelpPath += _T("\\mp3infp_eng.txt");
		ShellExecute(NULL,_T("open"),strHelpPath,NULL,NULL,SW_SHOW);
		return;
	}
	else
	{
		strHelpPath = szMp3infpPath;
		strHelpPath += _T("language\\");
		strHelpPath += strLanguage;
		strHelpPath += _T(".chm");

		CFileStatus status;
		if(CFile::GetStatus(strHelpPath,status) == FALSE)
		{
			// chmが無ければtxtを開く
			strHelpPath = szMp3infpPath;
			strHelpPath += _T("language\\");
			strHelpPath += strLanguage;
			strHelpPath += _T(".txt");
			ShellExecute(NULL,_T("open"),strHelpPath,NULL,NULL,SW_SHOW);
			return;
		}
	}
TRACE(_T("strHelPath=%s\n"),strHelpPath);
//	DWORD dwCookie;
//	HtmlHelp(
//			NULL,
//			NULL,
//			HH_INITIALIZE,
//			(DWORD )&dwCookie);//Hhctrl.ocx により返される cookie
	
	HWND hwndHelp = HtmlHelp(hWnd,strHelpPath,HH_DISPLAY_INDEX,(DWORD_PTR )szViewFile);
/*	if(hwndHelp)
	{
		HtmlHelp(
				NULL,
				NULL,
				HH_UNINITIALIZE,
				(DWORD )dwCookie);//Hhctrl.ocx により返される cookie
		::SetForegroundWindow(hwndHelp);
	}
	else
	{
//		strcpy(szHelpFile,szHelpPath);
//		lstrcat(szHelpFile,README_FILE);
//		ShellExecute(hWnd,_T("open"),szHelpFile,NULL,NULL,SW_SHOWNORMAL);
	}*/

}

void CShellExt::ConfigLoad()
{
	TRACE(_T("ConfigLoad()\n"));
	//オプションを取得
	CString strDefault;
	//(共通)
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("SaveTimeStamp"),(DWORD *)&m_bSaveTimeStamp,TRUE);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("ContextMenu"),(DWORD *)&m_bContextMenu,TRUE);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("SelectDrive"),(DWORD *)&m_bSelectDrive,DEF_SETUP_MAIN_SELECT_DRIVE);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("Disable_FDD"),(DWORD *)&m_bDisable_FDD,DEF_SETUP_MAIN_DISABLE_FDD);

	// Workaround for the old wrong value name.
	DWORD dwTemp;
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("Disable_RAMOVABLE"),&dwTemp,DEF_SETUP_MAIN_DISABLE_REMOVABLE);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("Disable_REMOVABLE"),(DWORD *)&m_bDisable_REMOVABLE,dwTemp);

	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("Disable_CDROM"),(DWORD *)&m_bDisable_CDROM,DEF_SETUP_MAIN_DISABLE_CDROM);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("Disable_NETWORK"),(DWORD *)&m_bDisable_NETWORK,DEF_SETUP_MAIN_DISABLE_NETWORK);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("Column_8_3"),(DWORD *)&m_bColumn_8_3,DEF_SETUP_MAIN_COLUMN_8_3);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("PropAOT"),(DWORD *)&m_bPropAOT,DEF_SETUP_PROP_AOT);
	//(mp3)
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("mp3_PropEnable"),(DWORD *)&m_bMp3PropEnable,TRUE);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("mp3_Id3v1ScmpxGenre"),(DWORD *)&m_bId3v1ScmpxGenre,FALSE);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("mp3_RiffSifDisable"),(DWORD *)&m_bRiffSifDisable,TRUE);
	m_strRmpSoft = regGetStringEx(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("mp3_RmpSoft"),SOFT_NAME);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("mp3_InfotipEnable"),(DWORD *)&m_bMp3InfotipEnable,TRUE);
	strDefault.LoadString(IDS_DEF_INFOTIP_MP3_FORMAT);
	m_strMp3InfoTipFormat = regGetStringEx(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("mp3_InfotipFormat"),strDefault);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("mp3_ColumnEnable"),(DWORD *)&m_bMp3ColumnEnable,TRUE);
	m_bMp3ApeTagDisable = FALSE;

	//(wave)
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("wave_PropEnable"),(DWORD *)&m_bWavePropEnable,TRUE);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("wave_InfotipEnable"),(DWORD *)&m_bWaveInfotipEnable,TRUE);
	strDefault.LoadString(IDS_DEF_INFOTIP_WAVE_FORMAT);
	m_strWaveInfoTipFormat = regGetStringEx(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("wave_InfotipFormat"),strDefault);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("wave_ColumnEnable"),(DWORD *)&m_bWaveColumnEnable,TRUE);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("wave_CodecFind"),(DWORD *)&m_iWaveCodecFind,0);
	m_iWaveCodecFind = 2;	// 2004-01-21 コーデック取得方法は内蔵辞書固定

	//(avi)
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("avi_PropEnable"),(DWORD *)&m_bAviPropEnable,TRUE);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("avi_InfotipEnable"),(DWORD *)&m_bAviInfotipEnable,TRUE);
	strDefault.LoadString(IDS_DEF_INFOTIP_AVI_FORMAT);
	m_strAviInfoTipFormat = regGetStringEx(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("avi_InfotipFormat"),strDefault);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("avi_ColumnEnable"),(DWORD *)&m_bAviColumnEnable,TRUE);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("avi_CodecFind"),(DWORD *)&m_iAviCodecFind,0);
	m_iAviCodecFind = 2;	// 2004-01-21 コーデック取得方法は内蔵辞書固定

	//(vqf)
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("vqf_PropEnable"),(DWORD *)&m_bVqfPropEnable,TRUE);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("vqf_InfotipEnable"),(DWORD *)&m_bVqfInfotipEnable,TRUE);
	strDefault.LoadString(IDS_DEF_INFOTIP_VQF_FORMAT);
	m_strVqfInfoTipFormat = regGetStringEx(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("vqf_InfotipFormat"),strDefault);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("vqf_ColumnEnable"),(DWORD *)&m_bVqfColumnEnable,TRUE);
	
	//(wma)
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("wma_PropEnable"),(DWORD *)&m_bWmaPropEnable,TRUE);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("wma_InfotipEnable"),(DWORD *)&m_bWmaInfotipEnable,TRUE);
	strDefault.LoadString(IDS_DEF_INFOTIP_WMA_FORMAT);
	m_strWmaInfoTipFormat = regGetStringEx(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("wma_InfotipFormat"),strDefault);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("wma_ColumnEnable"),(DWORD *)&m_bWmaColumnEnable,TRUE);
	
	//(m3u)
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("m3u_PropEnable"),(DWORD *)&m_bM3uPropEnable,DEF_M3U_PROP_ENABLE);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("m3u_InfotipEnable"),(DWORD *)&m_bM3uInfotipEnable,DEF_M3U_INFOTIP_ENABLE);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("m3u_Prop_Fullpath"),(DWORD *)&m_bM3uProp_Fullpath,DEF_M3U_PROP_FULLPATH);

	//(ogg)
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("ogg_PropEnable"),(DWORD *)&m_bOggPropEnable,DEF_OGG_PROP_ENABLE);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("ogg_InfotipEnable"),(DWORD *)&m_bOggInfotipEnable,DEF_OGG_INFOTIP_ENABLE);
	strDefault.LoadString(IDS_DEF_INFOTIP_OGG_FORMAT);
	m_strOggInfoTipFormat = regGetStringEx(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("ogg_InfotipFormat"),strDefault);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("ogg_ColumnEnable"),(DWORD *)&m_bOggColumnEnable,DEF_OGG_COLUMN_ENABLE);

	//(ape)
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("ape_PropEnable"),(DWORD *)&m_bApePropEnable,DEF_APE_PROP_ENABLE);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("ape_InfotipEnable"),(DWORD *)&m_bApeInfotipEnable,DEF_APE_INFOTIP_ENABLE);
	strDefault.LoadString(IDS_DEF_INFOTIP_APE_FORMAT);
	m_strApeInfoTipFormat = regGetStringEx(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("ape_InfotipFormat"),strDefault);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("ape_ColumnEnable"),(DWORD *)&m_bApeColumnEnable,DEF_APE_COLUMN_ENABLE);
	
	//(mp4)
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("mp4_PropEnable"),(DWORD *)&m_bMp4PropEnable,DEF_MP4_PROP_ENABLE);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("mp4_InfotipEnable"),(DWORD *)&m_bMp4InfotipEnable,DEF_MP4_INFOTIP_ENABLE);
	strDefault.LoadString(IDS_DEF_INFOTIP_MP4_FORMAT);
	m_strMp4InfoTipFormat = regGetStringEx(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("mp4_InfotipFormat"),strDefault);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("mp4_ColumnEnable"),(DWORD *)&m_bMp4ColumnEnable,DEF_MP4_COLUMN_ENABLE);
	
}
