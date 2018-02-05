// mp3infp.cpp : DLL 用の初期化処理の定義を行います。
//

#include "stdafx.h"
#include "mp3infp.h"
#include "GlobalCommand.h"

#ifdef _DEBUG
#include "psapi.h"
#pragma comment(lib,"psapi.lib")
#endif

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "htmlhelp.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//
//	メモ!
//
//		この DLL が MFC DLL に対して動的にリンクされる場合、
//		MFC 内で呼び出されるこの DLL からエクスポートされた
//		どの関数も関数の最初に追加される AFX_MANAGE_STATE 
//		マクロを含んでいなければなりません。
//
//		例:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 通常関数の本体はこの位置にあります
//		}
//
//		このマクロが各関数に含まれていること、MFC 内の
//		どの呼び出しより優先することは非常に重要です。
//		これは関数内の最初のステートメントでなければな
//		らないことを意味します、コンストラクタが MFC 
//		DLL 内への呼び出しを行う可能性があるので、オブ
//		ジェクト変数の宣言よりも前でなければなりません。
//
//		詳細については MFC テクニカル ノート 33 および
//		58 を参照してください。
//

/////////////////////////////////////////////////////////////////////////////
// CMp3infpApp

BEGIN_MESSAGE_MAP(CMp3infpApp, CWinApp)
	//{{AFX_MSG_MAP(CMp3infpApp)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
		//        この位置に生成されるコードを編集しないでください。
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMp3infpApp の構築

CMp3infpApp::CMp3infpApp()
{
	// TODO: この位置に構築用のコードを追加してください。
	// ここに InitInstance の中の重要な初期化処理をすべて記述してください。

	//comctl32.dllのロードと初期化
	InitCommonControls();
	//comの初期化
//	CoInitialize(NULL);
	m_iWaveCodecFind = 0;
	m_iAviCodecFind = 0;
}

CMp3infpApp::~CMp3infpApp()
{
//	CoUninitialize();
}

BOOL CMp3infpApp::InitInstance() 
{
	TRACE(_T("CMp3infpApp::InitInstance(0)\n"));
	
	theApp.m_hResource = NULL;

	CString strLangPath;
	strLangPath = regGetStringEx(HKEY_LOCAL_MACHINE,MP3INFP_REG_ENTRY,_T("path"),_T("c:\\progra~1\\mp3infp"));
	AddTailYenSign(strLangPath);
	strLangPath += _T("language\\");
	
	CString strLanguage = regGetStringEx(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("Language"),DEF_SETUP_MAIN_LANGUAGE);
	strLanguage += _T(".lng");
	
	strLangPath += strLanguage;
	//theApp.m_hResource = LoadLibraryEx(strLangPath,0,LOAD_LIBRARY_AS_DATAFILE);
	theApp.m_hResource = LoadLibrary(strLangPath);
	if(theApp.m_hResource)
	{
		AfxSetResourceHandle(theApp.m_hResource);
		TCHAR tmp[20];
		if((LoadString(AfxGetResourceHandle(),IDS_LANGUAGE_VER,tmp,sizeof_array(tmp)) == 0) ||
			(_ttoi(tmp) < LANG_VER) )
		{
			FreeLibrary(theApp.m_hResource);
			theApp.m_hResource = NULL;
		}
	}
	if(theApp.m_hResource == NULL)
	{
		// 言語パックがロードできないときは内蔵のリソースを使う
		theApp.m_hResource = theApp.m_hInstance;
		AfxSetResourceHandle(theApp.m_hResource);
	}

	return CWinApp::InitInstance();
}

int CMp3infpApp::ExitInstance() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TRACE(_T("CMp3infpApp::ExitInstance()\n"));
	if(theApp.m_hResource && (theApp.m_hResource != theApp.m_hInstance))
	{
		FreeLibrary(theApp.m_hResource);
	}


	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// 唯一の CMp3infpApp オブジェクト

CMp3infpApp theApp;

//AFX_MANAGE_STATE(AfxGetStaticModuleState());
//
// Global variables
//
UINT	g_cRefThisDll = 0;	// Reference count of this DLL.
DWORD	g_dwCookie;

/////////////////////////////////////////////////////////////////////////////
///エクスポート関数

STDAPI DllCanUnloadNow(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

#ifdef _DEBUG
	char szProcessName[MAX_PATH];
	HANDLE hProcess = GetCurrentProcess();
	GetModuleBaseName(hProcess,NULL,szProcessName,sizeof(szProcessName));
	TRACE(_T("[%s]In DLLCanUnloadNow(Ref=%d) process=%s\r\n"),APP_NAME,g_cRefThisDll,szProcessName);
#endif

	return (g_cRefThisDll == 0 ? S_OK : S_FALSE);
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppvOut)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	TRACE(_T("[%s]In DllGetClassObject\r\n"),APP_NAME);

	*ppvOut = NULL;

	if(IsEqualIID(rclsid,CLSID_ShellExt))
	{
        CShellExtClassFactory *pcf = new CShellExtClassFactory;
		if(pcf == NULL)
		{
			TRACE(_T("[%s]  +--return FALSE\r\n"),APP_NAME);
			return S_FALSE;
		}
        HRESULT hr = pcf->QueryInterface(riid, ppvOut);
		pcf->Release();
		TRACE(_T("[%s]  +--return(%08x)\r\n"),hr);
		return hr;
	}

	TRACE(_T("[%s]  +--return(CLASS_E_CLASSNOTAVAILABLE)\r\n"));
	return CLASS_E_CLASSNOTAVAILABLE;
}

static BOOL regExSet(LPCTSTR szExName,LPCTSTR szData)
{
	HKEY	hKeyResult = 0;
	DWORD	dwDisposition = 0;
	DWORD	dwSize;
	LONG	lResult;
	TCHAR	szRetVal[255];
	DWORD	dwType;

	if(RegOpenKeyEx(HKEY_CLASSES_ROOT,szExName,0,KEY_WRITE|KEY_READ,&hKeyResult) == ERROR_SUCCESS)
	{
		dwSize=sizeof(szRetVal);
		if(RegQueryValueEx(hKeyResult,_T(""),NULL,&dwType,(LPBYTE)szRetVal,&dwSize) == ERROR_SUCCESS)
		{
			if(dwSize > 1)
			{		
				RegCloseKey(hKeyResult);
				return FALSE;	//すでに値がある
			}
		}
		RegSetValueEx(hKeyResult,
				NULL,
				0,
				REG_SZ,
				(const unsigned char *)szData,
				(lstrlen(szData)+1)*sizeof(TCHAR));
		RegCloseKey(hKeyResult);
		return TRUE;		//値がないようなので設定した
	}else
	{
		//キーがないときは作成する
		if(RegCreateKeyEx(HKEY_CLASSES_ROOT,
			szExName,
			0,
			_T(""),
			REG_OPTION_NON_VOLATILE,
			KEY_WRITE|KEY_READ,
			NULL,
			&hKeyResult,
			&dwDisposition) == ERROR_SUCCESS)
		{
			lResult=RegSetValueEx(hKeyResult,
						NULL,
						0,
						REG_SZ,
						(const unsigned char *)szData,
						(lstrlen(szData)+1)*sizeof(TCHAR));
			RegCloseKey(hKeyResult);
			return TRUE;
		}
	}
	return FALSE;
}

static const LPCTSTR	extlist[]={_T(".mp3"),_T(".mp2"),_T(".mp1"),_T(".rmp"),_T(".wav"),_T(".avi"),_T(".vqf"),_T(".wma"),_T(".wmv"),_T(".asf"),_T(".m3u"),_T(".m3u8"),_T(".ogg"),_T(".ape"),_T(".mp4"),_T(".m4v"),_T(".m4a"),_T(".3gp"),_T(".3g2"),_T(".cda")};

STDAPI DllRegisterServer()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	TCHAR szDllPath[MAX_PATH];
	GetModuleFileName(AfxGetInstanceHandle(),szDllPath,MAX_PATH);

	//コンポーネントの登録
	regSetString(HKEY_CLASSES_ROOT,_T("CLSID\\") CLSID_STR_ShellExt,_T(""),APP_NAME);
	regSetString(HKEY_CLASSES_ROOT,_T("CLSID\\") CLSID_STR_ShellExt _T("\\InProcServer32"),_T(""),szDllPath);
	regSetString(HKEY_CLASSES_ROOT,_T("CLSID\\") CLSID_STR_ShellExt _T("\\InProcServer32"),_T("ThreadingModel"),_T("Apartment"));

	//Approved
	regSetString(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"),CLSID_STR_ShellExt,APP_NAME);

	//関連付けの設定を維持する設定
	//2003-08-10 インストーラに移動
//	char szRegExePath[MAX_PATH];
//	regGetString(HKEY_LOCAL_MACHINE,MP3INFP_REG_ENTRY,_T("path"),szRegExePath,_T(""));
//	AddTailYenSign(szRegExePath);
//	strcat(szRegExePath,_T("mp3infp_regist.exe"));
//	regSetString(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"),APP_NAME,szRegExePath);

	//関連付け
	TCHAR szKey[256];
	//(ContextMenu)
	regSetString(HKEY_CLASSES_ROOT,_T("*\\shellex\\ContextMenuHandlers\\mp3infp"),_T(""),CLSID_STR_ShellExt);
	//(Property)
	regSetString(HKEY_CLASSES_ROOT,_T("*\\shellex\\PropertySheetHandlers\\mp3infp"),_T(""),CLSID_STR_ShellExt);
	for(int i=0; i<sizeof_array(extlist); i++)
	{
		//(InfoTip)
		wsprintf(szKey,_T("%s\\shellex\\{00021500-0000-0000-C000-000000000046}"),extlist[i]);
		regExSet(szKey,CLSID_STR_ShellExt);
	}
	//ドライブ
//	regSetString(HKEY_CLASSES_ROOT,_T("Drive\\shellex\\PropertySheetHandlers\\") CLSID_STR_ShellExt,_T(""),_T("mp3infp"));
	regSetString(HKEY_CLASSES_ROOT,_T("Folder\\shellex\\ColumnHandlers\\") CLSID_STR_ShellExt,_T(""),_T("mp3infp"));

	return S_OK;
}

STDAPI DllUnregisterServer()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	//関連付け解除
	TCHAR szKey[256];
	//(ContextMenu)
	wsprintf(szKey,_T("*\\shellex\\ContextMenuHandlers\\mp3infp"));
	RegDeleteKey(HKEY_CLASSES_ROOT,szKey);
	//(Property)
	wsprintf(szKey,_T("*\\shellex\\PropertySheetHandlers\\mp3infp"));
	RegDeleteKey(HKEY_CLASSES_ROOT,szKey);
	for(int i=0; i<sizeof_array(extlist); i++)
	{
		regGetString(HKEY_CLASSES_ROOT,extlist[i],_T(""),szKey,_T(""));
		if(lstrlen(szKey))
		{
			//(Property) ～Ver.2.00Beta2
			wsprintf(szKey,_T("%s\\shellex\\PropertySheetHandlers\\mp3infp"),szKey);
			RegDeleteKey(HKEY_CLASSES_ROOT,szKey);
		}
		//(InfoTip)
		wsprintf(szKey,_T("%s\\shellex\\{00021500-0000-0000-C000-000000000046}"),extlist[i]);
		RegDeleteKey(HKEY_CLASSES_ROOT,szKey);
	}
	//ドライブ
	RegDeleteKey(HKEY_CLASSES_ROOT,_T("Drive\\shellex\\PropertySheetHandlers\\") CLSID_STR_ShellExt);
	RegDeleteKey(HKEY_CLASSES_ROOT,_T("Folder\\shellex\\ColumnHandlers\\") CLSID_STR_ShellExt);

	//Approved登録解除
	regDelVal(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"),CLSID_STR_ShellExt);

	//関連付けの設定を維持する設定を解除
//	regDelVal(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"),APP_NAME);

	//コンポーネントの登録解除
	RegDeleteKey(HKEY_CLASSES_ROOT,_T("CLSID\\") CLSID_STR_ShellExt _T("\\InProcServer32"));
	RegDeleteKey(HKEY_CLASSES_ROOT,_T("CLSID\\") CLSID_STR_ShellExt);
	
	//mp3infpの設定を削除
//	RegDeleteKey(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\win32lab.com\\mp3infp"));
//	RegDeleteKey(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\win32lab.com"));
//	RegDeleteKey(HKEY_CURRENT_USER,_T("Software\\win32lab.com\\mp3infp"));
//	RegDeleteKey(HKEY_CURRENT_USER,_T("Software\\win32lab.com"));
	
	return S_OK;
}
