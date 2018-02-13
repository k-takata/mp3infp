// mp3infp_config.cpp : アプリケーション用の初期化処理の定義を行います。
//

#include "stdafx.h"
#include "mp3infp_config.h"

#include "SetupProperty.h"

#pragma comment(lib, "htmlhelp.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CMp3infpConfigApp

BEGIN_MESSAGE_MAP(CMp3infpConfigApp, CWinApp)
	//{{AFX_MSG_MAP(CMp3infpConfigApp)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
		//        この位置に生成されるコードを編集しないでください。
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// 唯一の CMp3infpConfigApp オブジェクト

CMp3infpConfigApp theApp;

//DWORD g_dwCookie;

/////////////////////////////////////////////////////////////////////////////
// CMp3infpConfigApp の構築

CMp3infpConfigApp::CMp3infpConfigApp()
{
	//comctl32.dllのロードと初期化
	InitCommonControls();
#ifdef _DEBUG
	OutputDebugString("CMp3infpConfigApp::CMp3infpConfigApp()\n");
#endif
//	HtmlHelp(
//			NULL,
//			NULL,
//			HH_INITIALIZE,
//			(DWORD )&g_dwCookie);//Hhctrl.ocx により返される cookie

}

CMp3infpConfigApp::~CMp3infpConfigApp()
{
}

BOOL CMp3infpConfigApp::InitInstance()
{
	// 標準的な初期化処理
	// もしこれらの機能を使用せず、実行ファイルのサイズを小さくしたけ
	//  れば以下の特定の初期化ルーチンの中から不必要なものを削除して
	//  ください。

#ifdef _AFXDLL
	Enable3dControls();			// 共有 DLL 内で MFC を使う場合はここをコールしてください。
#else
	Enable3dControlsStatic();	// MFC と静的にリンクする場合はここをコールしてください。
#endif

	LoadLanguage();

	// Parse command line
	int argc;
	LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	int page = 0;
	if(argc > 1)
	{
		if((argv[1][0] == L'-' || argv[1][0] == L'/')
			&& (argv[1][1] == L'h' || argv[1][1] == L'H' || argv[1][1] == L'?'))
		{
			// TODO: Translate this message.
			AfxMessageBox(_T("Usage: mp3infp_config [page_num]\n\n  page_num: 1...10"));
			return FALSE;
		}
		page = _wtoi(argv[1]) - 1;
	}

	TCHAR tmp[100];
	//「mp3infp設定」
	::LoadString(m_hResource,IDS_CPL_NAME,tmp,sizeof_array(tmp));
	CSetupProperty propDlg(tmp,NULL,page);
	m_pMainWnd = &propDlg;
	propDlg.DoModal();

	// ダイアログが閉じられてからアプリケーションのメッセージ ポンプを開始するよりは、
	// アプリケーションを終了するために FALSE を返してください。
	return FALSE;
}

LONG CMp3infpConfigApp::LoadLanguage()
{
#ifdef _DEBUG
	OutputDebugString("CMp3infpConfigApp::LoadLanguage()\n");
#endif
	CString strLangPath;
	strLangPath = regGetStringEx(HKEY_LOCAL_MACHINE,MP3INFP_REG_ENTRY,"path","c:\\progra~1\\mp3infp");
	AddTailYenSign(strLangPath);
	strLangPath += "language\\";
	CString strLanguage = regGetStringEx(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"Language",DEF_SETUP_MAIN_LANGUAGE);
	strLanguage += ".lng";
	strLangPath += strLanguage;
#ifdef _DEBUG
	OutputDebugString(strLangPath);
	OutputDebugString("\n");
#endif
	//m_hResource = LoadLibraryEx(strLangPath,0,LOAD_LIBRARY_AS_DATAFILE);
	m_hResource = LoadLibrary(strLangPath);
	if(m_hResource)
	{
		AfxSetResourceHandle(theApp.m_hResource);
		char tmp[20];
		if((LoadString(AfxGetResourceHandle(),IDS_LANGUAGE_VER,tmp,sizeof(tmp)) == 0) ||
			(atoi(tmp) < LANG_VER) )
		{
			FreeLibrary(m_hResource);
			m_hResource = NULL;
		}
	}
	if(!m_hResource)
	{
		m_hResource = AfxGetInstanceHandle();
		AfxSetResourceHandle(theApp.m_hResource);
	}
	return 1; // OK
}
