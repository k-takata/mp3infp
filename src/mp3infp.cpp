// mp3infp.cpp : DLL �p�̏����������̒�`���s���܂��B
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
// CMp3infpApp

BEGIN_MESSAGE_MAP(CMp3infpApp, CWinApp)
	//{{AFX_MSG_MAP(CMp3infpApp)
		// ���� - ClassWizard �͂��̈ʒu�Ƀ}�b�s���O�p�̃}�N����ǉ��܂��͍폜���܂��B
		//        ���̈ʒu�ɐ��������R�[�h��ҏW���Ȃ��ł��������B
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMp3infpApp �̍\�z

CMp3infpApp::CMp3infpApp()
{
	// TODO: ���̈ʒu�ɍ\�z�p�̃R�[�h��ǉ����Ă��������B
	// ������ InitInstance �̒��̏d�v�ȏ��������������ׂċL�q���Ă��������B

	//comctl32.dll�̃��[�h�Ə�����
	InitCommonControls();
	//com�̏�����
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
		// ����p�b�N�����[�h�ł��Ȃ��Ƃ��͓����̃��\�[�X���g��
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
// �B��� CMp3infpApp �I�u�W�F�N�g

CMp3infpApp theApp;

//AFX_MANAGE_STATE(AfxGetStaticModuleState());
//
// Global variables
//
UINT	g_cRefThisDll = 0;	// Reference count of this DLL.
DWORD	g_dwCookie;

/////////////////////////////////////////////////////////////////////////////
///�G�N�X�|�[�g�֐�

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
				return FALSE;	//���łɒl������
			}
		}
		RegSetValueEx(hKeyResult,
				NULL,
				0,
				REG_SZ,
				(const unsigned char *)szData,
				(lstrlen(szData)+1)*sizeof(TCHAR));
		RegCloseKey(hKeyResult);
		return TRUE;		//�l���Ȃ��悤�Ȃ̂Őݒ肵��
	}else
	{
		//�L�[���Ȃ��Ƃ��͍쐬����
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

	//�R���|�[�l���g�̓o�^
	regSetString(HKEY_CLASSES_ROOT,_T("CLSID\\") CLSID_STR_ShellExt,_T(""),APP_NAME);
	regSetString(HKEY_CLASSES_ROOT,_T("CLSID\\") CLSID_STR_ShellExt _T("\\InProcServer32"),_T(""),szDllPath);
	regSetString(HKEY_CLASSES_ROOT,_T("CLSID\\") CLSID_STR_ShellExt _T("\\InProcServer32"),_T("ThreadingModel"),_T("Apartment"));

	//Approved
	regSetString(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"),CLSID_STR_ShellExt,APP_NAME);

	//�֘A�t���̐ݒ���ێ�����ݒ�
	//2003-08-10 �C���X�g�[���Ɉړ�
//	char szRegExePath[MAX_PATH];
//	regGetString(HKEY_LOCAL_MACHINE,MP3INFP_REG_ENTRY,_T("path"),szRegExePath,_T(""));
//	AddTailYenSign(szRegExePath);
//	strcat(szRegExePath,_T("mp3infp_regist.exe"));
//	regSetString(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"),APP_NAME,szRegExePath);

	//�֘A�t��
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
	//�h���C�u
//	regSetString(HKEY_CLASSES_ROOT,_T("Drive\\shellex\\PropertySheetHandlers\\") CLSID_STR_ShellExt,_T(""),_T("mp3infp"));
	regSetString(HKEY_CLASSES_ROOT,_T("Folder\\shellex\\ColumnHandlers\\") CLSID_STR_ShellExt,_T(""),_T("mp3infp"));

	return S_OK;
}

STDAPI DllUnregisterServer()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	//�֘A�t������
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
			//(Property) �`Ver.2.00Beta2
			wsprintf(szKey,_T("%s\\shellex\\PropertySheetHandlers\\mp3infp"),szKey);
			RegDeleteKey(HKEY_CLASSES_ROOT,szKey);
		}
		//(InfoTip)
		wsprintf(szKey,_T("%s\\shellex\\{00021500-0000-0000-C000-000000000046}"),extlist[i]);
		RegDeleteKey(HKEY_CLASSES_ROOT,szKey);
	}
	//�h���C�u
	RegDeleteKey(HKEY_CLASSES_ROOT,_T("Drive\\shellex\\PropertySheetHandlers\\") CLSID_STR_ShellExt);
	RegDeleteKey(HKEY_CLASSES_ROOT,_T("Folder\\shellex\\ColumnHandlers\\") CLSID_STR_ShellExt);

	//Approved�o�^����
	regDelVal(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"),CLSID_STR_ShellExt);

	//�֘A�t���̐ݒ���ێ�����ݒ������
//	regDelVal(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"),APP_NAME);

	//�R���|�[�l���g�̓o�^����
	RegDeleteKey(HKEY_CLASSES_ROOT,_T("CLSID\\") CLSID_STR_ShellExt _T("\\InProcServer32"));
	RegDeleteKey(HKEY_CLASSES_ROOT,_T("CLSID\\") CLSID_STR_ShellExt);
	
	//mp3infp�̐ݒ���폜
//	RegDeleteKey(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\win32lab.com\\mp3infp"));
//	RegDeleteKey(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\win32lab.com"));
//	RegDeleteKey(HKEY_CURRENT_USER,_T("Software\\win32lab.com\\mp3infp"));
//	RegDeleteKey(HKEY_CURRENT_USER,_T("Software\\win32lab.com"));
	
	return S_OK;
}
