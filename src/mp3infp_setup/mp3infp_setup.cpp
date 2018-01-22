// mp3infp_setup.cpp : アプリケーション用の初期化処理の定義を行います。
//

#include "stdafx.h"
#include "mp3infp_setup.h"

#include "SetupProperty.h"

#pragma comment(lib, "htmlhelp.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CMp3infpSetupApp

BEGIN_MESSAGE_MAP(CMp3infpSetupApp, CWinApp)
	//{{AFX_MSG_MAP(CMp3infpSetupApp)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
		//        この位置に生成されるコードを編集しないでください。
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// 唯一の CMp3infpSetupApp オブジェクト

CMp3infpSetupApp theApp;

CMp3infpSetupApp* CMp3infpSetupApp::m_pThis = NULL;
//DWORD g_dwCookie;

/////////////////////////////////////////////////////////////////////////////
// CMp3infpSetupApp の構築

CMp3infpSetupApp::CMp3infpSetupApp()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	//comctl32.dllのロードと初期化
	InitCommonControls();
#ifdef _DEBUG
	OutputDebugString("CMp3infpSetupApp::CMp3infpSetupApp()\n");
#endif
	m_pThis = this;
//	HtmlHelp(
//			NULL,
//			NULL,
//			HH_INITIALIZE,
//			(DWORD )&g_dwCookie);//Hhctrl.ocx により返される cookie

}

CMp3infpSetupApp::~CMp3infpSetupApp()
{
}

BOOL CMp3infpSetupApp::InitInstance()
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

	TCHAR tmp[100];
	//「mp3infp設定」
	::LoadString(m_pThis->m_hResource,IDS_CPL_NAME,tmp,sizeof_array(tmp));
	CSetupProperty propDlg(tmp,NULL,0);
	m_pMainWnd = &propDlg;
	propDlg.DoModal();

	// ダイアログが閉じられてからアプリケーションのメッセージ ポンプを開始するよりは、
	// アプリケーションを終了するために FALSE を返してください。
	return FALSE;
}

LONG CMp3infpSetupApp::LoadLanguage()
{
#ifdef _DEBUG
	OutputDebugString("CMp3infpSetupApp::LoadLanguage()\n");
#endif
	CString strLangPath;
	strLangPath = regGetStringEx(HKEY_LOCAL_MACHINE,MP3INFP_REG_ENTRY,"path","c:\\progra~1\\mp3infp");
	AddTailYenSign(strLangPath);
	strLangPath += "language\\";
	CString strLanguage = regGetStringEx(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"Language",(char *)(LPCSTR )DEF_SETUP_MAIN_LANGUAGE);
	strLanguage += ".lng";
	strLangPath += strLanguage;
#ifdef _DEBUG
	OutputDebugString(strLangPath);
	OutputDebugString("\n");
#endif
	//m_pThis->m_hResource = LoadLibraryEx(strLangPath,0,LOAD_LIBRARY_AS_DATAFILE);
	m_pThis->m_hResource = LoadLibrary(strLangPath);
	if(m_pThis->m_hResource)
	{
		AfxSetResourceHandle(theApp.m_hResource);
		char tmp[20];
		if((LoadString(AfxGetResourceHandle(),IDS_LANGUAGE_VER,tmp,sizeof(tmp)) == 0) ||
			(atoi(tmp) < LANG_VER) )
		{
			FreeLibrary(m_pThis->m_hResource);
			m_pThis->m_hResource = NULL;
		}
	}
	if(!m_pThis->m_hResource)
	{
		m_pThis->m_hResource = AfxGetInstanceHandle();
		AfxSetResourceHandle(theApp.m_hResource);
	}
	return 1; // OK
}
