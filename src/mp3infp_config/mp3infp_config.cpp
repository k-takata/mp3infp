// mp3infp_config.cpp : �A�v���P�[�V�����p�̏����������̒�`���s���܂��B
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
		// ���� - ClassWizard �͂��̈ʒu�Ƀ}�b�s���O�p�̃}�N����ǉ��܂��͍폜���܂��B
		//        ���̈ʒu�ɐ��������R�[�h��ҏW���Ȃ��ł��������B
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// �B��� CMp3infpConfigApp �I�u�W�F�N�g

CMp3infpConfigApp theApp;

//DWORD g_dwCookie;

/////////////////////////////////////////////////////////////////////////////
// CMp3infpConfigApp �̍\�z

CMp3infpConfigApp::CMp3infpConfigApp()
{
	//comctl32.dll�̃��[�h�Ə�����
	InitCommonControls();
#ifdef _DEBUG
	OutputDebugString("CMp3infpConfigApp::CMp3infpConfigApp()\n");
#endif
//	HtmlHelp(
//			NULL,
//			NULL,
//			HH_INITIALIZE,
//			(DWORD )&g_dwCookie);//Hhctrl.ocx �ɂ��Ԃ���� cookie

}

CMp3infpConfigApp::~CMp3infpConfigApp()
{
}

BOOL CMp3infpConfigApp::InitInstance()
{
	// �W���I�ȏ���������
	// ���������̋@�\���g�p�����A���s�t�@�C���̃T�C�Y��������������
	//  ��Έȉ��̓���̏��������[�`���̒�����s�K�v�Ȃ��̂��폜����
	//  ���������B

#ifdef _AFXDLL
	Enable3dControls();			// ���L DLL ���� MFC ���g���ꍇ�͂������R�[�����Ă��������B
#else
	Enable3dControlsStatic();	// MFC �ƐÓI�Ƀ����N����ꍇ�͂������R�[�����Ă��������B
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
	//�ump3infp�ݒ�v
	::LoadString(m_hResource,IDS_CPL_NAME,tmp,sizeof_array(tmp));
	CSetupProperty propDlg(tmp,NULL,page);
	m_pMainWnd = &propDlg;
	propDlg.DoModal();

	// �_�C�A���O�������Ă���A�v���P�[�V�����̃��b�Z�[�W �|���v���J�n������́A
	// �A�v���P�[�V�������I�����邽�߂� FALSE ��Ԃ��Ă��������B
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
