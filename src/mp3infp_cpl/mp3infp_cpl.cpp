// mp3infp_cpl.cpp : DLL 用の初期化処理の定義を行います。
//

#include "stdafx.h"
#include "mp3infp_cpl.h"

#include "SetupProperty.h"

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
// CMp3infp_cplApp

BEGIN_MESSAGE_MAP(CMp3infp_cplApp, CWinApp)
	//{{AFX_MSG_MAP(CMp3infp_cplApp)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
		//        この位置に生成されるコードを編集しないでください。
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// 唯一の CMp3infp_cplApp オブジェクト

CMp3infp_cplApp theApp;

CMp3infp_cplApp* CMp3infp_cplApp::m_pThis = NULL;
DWORD g_dwCookie;

/////////////////////////////////////////////////////////////////////////////
// CMp3infp_cplApp の構築

CMp3infp_cplApp::CMp3infp_cplApp()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	//comctl32.dllのロードと初期化
	InitCommonControls();
#ifdef _DEBUG
	OutputDebugString("CMp3infp_cplApp::CMp3infp_cplApp()\n");
#endif
	m_pThis = this;
//	HtmlHelp(
//			NULL,
//			NULL,
//			HH_INITIALIZE,
//			(DWORD )&g_dwCookie);//Hhctrl.ocx により返される cookie

}

CMp3infp_cplApp::~CMp3infp_cplApp()
{
}

extern "C" LONG __stdcall CMp3infp_cplApp::CPlApplet(HWND hWnd,UINT uMsg,LONG lParam1,LONG lParam2)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 

	CMp3infp_cplApp* pApplet = m_pThis;
	ASSERT(m_pThis);

#ifdef _DEBUG
	char tmp[1000];
	sprintf(tmp,"pApplet->m_hResource=%08x AfxGetInstanceHandle()=%08x\n",pApplet->m_hResource,AfxGetInstanceHandle());
	OutputDebugString(tmp);
#endif
	if(pApplet->m_hResource && (pApplet->m_hResource != AfxGetInstanceHandle()))
	{
#ifdef _DEBUG
		OutputDebugString("AfxSetResourceHandle()\n");
#endif
		AfxSetResourceHandle(pApplet->m_hResource);
	}

	switch(uMsg){
	case CPL_DBLCLK:
		return OnDblclk(hWnd,lParam1,lParam2);

	case CPL_EXIT:
		return OnExit();

	case CPL_GETCOUNT:
		return OnGetCount();

	case CPL_INIT:
		return OnInit();

	case CPL_INQUIRE:
		return OnInquire(lParam1,(CPLINFO *)lParam2);

	case CPL_NEWINQUIRE:
		return OnNewInquire(lParam1,(NEWCPLINFO *)lParam2);

	case CPL_STOP:
		return OnStop(lParam1,lParam2);

	case CPL_STARTWPARMS:
		OnDblclk(hWnd, lParam1, lParam2);
		return TRUE;

	default:
		break;
	}

	return FALSE;
}

LONG CMp3infp_cplApp::OnNewInquire(UINT uAppNum,NEWCPLINFO* pInfo)
{    
#ifdef _DEBUG
	OutputDebugString("CMp3infp_cplApp::OnNewInquire()\n");
#endif
	pInfo->dwSize			= (DWORD )sizeof(NEWCPLINFO);
	pInfo->dwFlags			= 0;
	pInfo->dwHelpContext	= 0;
	pInfo->lData			= 0;
	pInfo->hIcon			= ::LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON1));
	strcpy(pInfo->szHelpFile,"");
	strcpy(pInfo->szName,"mp3infp");
	//「mp3infpの動作設定を行います。」
	::LoadString(m_pThis->m_hResource,IDS_CPL_INFO,pInfo->szInfo,64);
	
	return 0;
}

LONG CMp3infp_cplApp::OnInquire(UINT uAppNum,CPLINFO* pInfo)
{
#ifdef _DEBUG
	OutputDebugString("CMp3infp_cplApp::OnInquire()\n");
#endif
	pInfo->idIcon	= CPL_DYNAMIC_RES;
	pInfo->idName	= CPL_DYNAMIC_RES;
	pInfo->idInfo	= CPL_DYNAMIC_RES;
	pInfo->lData	= 0;

	return 0;
}

LONG CMp3infp_cplApp::OnDblclk(HWND hWnd,UINT uAppNum,LONG lData)
{
#ifdef _DEBUG
	OutputDebugString("CMp3infp_cplApp::OnDblclk()\n");
#endif
	CWnd wnd;
	wnd.Attach(hWnd);
	ASSERT(wnd);

	int iPageNumber = 0;
	if(lData)
	{
		iPageNumber = _ttoi((const char *)lData)-1;
	}

	char tmp[100];
	//「mp3infp設定」
	::LoadString(m_pThis->m_hResource,IDS_CPL_NAME,tmp,sizeof(tmp));
	CSetupProperty propDlg(tmp,&wnd,iPageNumber);
	propDlg.DoModal();
	wnd.Detach();
	
	return 0; // OK
}

LONG CMp3infp_cplApp::OnExit()
{
#ifdef _DEBUG
	OutputDebugString("CMp3infp_cplApp::OnExit()\n");
#endif
	if(m_pThis->m_hResource && (m_pThis->m_hResource != AfxGetInstanceHandle()))
	{
		FreeLibrary(m_pThis->m_hResource);
	}
	return 0; // OK
}

LONG CMp3infp_cplApp::OnGetCount()
{
#ifdef _DEBUG
	OutputDebugString("CMp3infp_cplApp::OnGetCount()\n");
#endif
	return 1; // only one applet
}

LONG CMp3infp_cplApp::OnInit()
{
#ifdef _DEBUG
	OutputDebugString("CMp3infp_cplApp::OnInit()\n");
#endif
	LoadLanguage();

#ifdef _DEBUG
	char tmp[1000];
	sprintf(tmp,"CMp3infp_cplApp::OnInit()> m_hResource=%08x AfxGetInstanceHandle()=%08x\n",m_pThis->m_hResource,AfxGetInstanceHandle());
	OutputDebugString(tmp);
#endif
	return 1; // OK
}

LONG CMp3infp_cplApp::OnStop(UINT uAppNum,LONG lData) 
{
#ifdef _DEBUG
	OutputDebugString("CMp3infp_cplApp::OnStop()\n");
#endif
	return 1; // not handled
}

LONG CMp3infp_cplApp::LoadLanguage()
{
#ifdef _DEBUG
	OutputDebugString("CMp3infp_cplApp::LoadLanguage()\n");
#endif
	CString strLangPath;
	strLangPath = regGetStringEx(HKEY_LOCAL_MACHINE,MP3INFP_REG_ENTRY,"path","c:\\progra~1\\mp3infp");
	AddTAilYenSigne(strLangPath);
	strLangPath += "language\\";
	CString strLanguage = regGetStringEx(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"Language",(char *)(LPCSTR )DEF_SETUP_MAIN_LANGUAGE);
	strLanguage += ".lng";
	strLangPath += strLanguage;
#ifdef _DEBUG
	OutputDebugString(strLangPath);
	OutputDebugString("\n");
#endif
	m_pThis->m_hResource = LoadLibraryEx(strLangPath,0,LOAD_LIBRARY_AS_DATAFILE);
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
