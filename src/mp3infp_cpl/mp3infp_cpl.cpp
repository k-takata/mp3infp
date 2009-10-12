// mp3infp_cpl.cpp : DLL �p�̏����������̒�`���s���܂��B
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
//	����!
//
//		���� DLL �� MFC DLL �ɑ΂��ē��I�Ƀ����N�����ꍇ�A
//		MFC ���ŌĂяo����邱�� DLL ����G�N�X�|�[�g���ꂽ
//		�ǂ̊֐����֐��̍ŏ��ɒǉ������ AFX_MANAGE_STATE 
//		�}�N�����܂�ł��Ȃ���΂Ȃ�܂���B
//
//		��:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �ʏ�֐��̖{�̂͂��̈ʒu�ɂ���܂�
//		}
//
//		���̃}�N�����e�֐��Ɋ܂܂�Ă��邱�ƁAMFC ����
//		�ǂ̌Ăяo�����D�悷�邱�Ƃ͔��ɏd�v�ł��B
//		����͊֐����̍ŏ��̃X�e�[�g�����g�łȂ���΂�
//		��Ȃ����Ƃ��Ӗ����܂��A�R���X�g���N�^�� MFC 
//		DLL ���ւ̌Ăяo�����s���\��������̂ŁA�I�u
//		�W�F�N�g�ϐ��̐錾�����O�łȂ���΂Ȃ�܂���B
//
//		�ڍׂɂ��Ă� MFC �e�N�j�J�� �m�[�g 33 �����
//		58 ���Q�Ƃ��Ă��������B
//

/////////////////////////////////////////////////////////////////////////////
// CMp3infp_cplApp

BEGIN_MESSAGE_MAP(CMp3infp_cplApp, CWinApp)
	//{{AFX_MSG_MAP(CMp3infp_cplApp)
		// ���� - ClassWizard �͂��̈ʒu�Ƀ}�b�s���O�p�̃}�N����ǉ��܂��͍폜���܂��B
		//        ���̈ʒu�ɐ��������R�[�h��ҏW���Ȃ��ł��������B
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// �B��� CMp3infp_cplApp �I�u�W�F�N�g

CMp3infp_cplApp theApp;

CMp3infp_cplApp* CMp3infp_cplApp::m_pThis = NULL;
DWORD g_dwCookie;

/////////////////////////////////////////////////////////////////////////////
// CMp3infp_cplApp �̍\�z

CMp3infp_cplApp::CMp3infp_cplApp()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	//comctl32.dll�̃��[�h�Ə�����
	InitCommonControls();
#ifdef _DEBUG
	OutputDebugString("CMp3infp_cplApp::CMp3infp_cplApp()\n");
#endif
	m_pThis = this;
//	HtmlHelp(
//			NULL,
//			NULL,
//			HH_INITIALIZE,
//			(DWORD )&g_dwCookie);//Hhctrl.ocx �ɂ��Ԃ���� cookie

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
	//�ump3infp�̓���ݒ���s���܂��B�v
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
	//�ump3infp�ݒ�v
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
