// SetupProperty.cpp : �C���v�������e�[�V���� �t�@�C��
//

#include "stdafx.h"
#include "mp3infp_setup.h"

#include "SetupProperty.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupProperty

IMPLEMENT_DYNAMIC(CSetupProperty, CPropertySheet)

CSetupProperty::CSetupProperty(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	AddPages();
}

CSetupProperty::CSetupProperty(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPages();
}

CSetupProperty::~CSetupProperty()
{
}


BEGIN_MESSAGE_MAP(CSetupProperty, CPropertySheet)
	//{{AFX_MSG_MAP(CSetupProperty)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupProperty ���b�Z�[�W �n���h��

void CSetupProperty::AddPages()
{
	//�o�[�W�������
	//�ump3infp Ver.2.03�v
	pageMain.m_staticInfo = SOFT_NAME;
	pageMain.m_staticInfo += "\n";
	//�uCopyright(c) 1998-2001 T-Matsuo\nmailto:tms@win32lab.com\nhttp://win32lab.com/�v
	pageMain.m_staticInfo += COPY_RIGHT;

	AddPage(&pageMain);
	AddPage(&pageMp3);
	AddPage(&pageWave);
	AddPage(&pageAvi);
	AddPage(&pageVqf);
	AddPage(&pageWma);
	AddPage(&pageM3u);
	AddPage(&pageOgg);
	AddPage(&pageApe);
	AddPage(&pageMp4);
}


BOOL CSetupProperty::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: ���̈ʒu�ɌŗL�̏�����ǉ����邩�A�܂��͊�{�N���X���Ăяo���Ă�������

	return CPropertySheet::PreCreateWindow(cs);
}

BOOL CSetupProperty::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	
	// TODO: ���̈ʒu�ɌŗL�̃R�[�h��ǉ����Ă�������
	CString strCaption;
	//�ump3infp�ݒ�v
	strCaption.LoadString(IDS_CPL_CAPTION);
	SetWindowText(strCaption);

	return bResult;
}

void CSetupProperty::OpenHtmlHelp(HWND hWnd,char *szViewFile)
{
	CString strHelpPath;
	// mp3infp�̃C���X�g�[���p�X
	char szMp3infpPath[MAX_PATH];
	regGetString(HKEY_LOCAL_MACHINE,MP3INFP_REG_ENTRY,"path",szMp3infpPath,"");
	AddTailYenSign(szMp3infpPath);
	// �I������
	CString strLanguage = regGetStringEx(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"Language",(char *)(LPCSTR )DEF_SETUP_MAIN_LANGUAGE);
	if(strLanguage.Compare(DEF_SETUP_MAIN_LANGUAGE) == 0)
	{
		// �f�t�H���g�I��
		strHelpPath = szMp3infpPath;
		strHelpPath += "\\mp3infp_eng.txt";
		ShellExecute(NULL,"open",strHelpPath,NULL,NULL,SW_SHOW);
		return;
	}
	else
	{
		strHelpPath = szMp3infpPath;
		strHelpPath += "language\\";
		strHelpPath += strLanguage;
		strHelpPath += ".chm";

		CFileStatus status;
		if(CFile::GetStatus(strHelpPath,status) == FALSE)
		{
			// chm���������txt���J��
			strHelpPath = szMp3infpPath;
			strHelpPath += "language\\";
			strHelpPath += strLanguage;
			strHelpPath += ".txt";
			ShellExecute(NULL,"open",strHelpPath,NULL,NULL,SW_SHOW);
			return;
		}
	}
TRACE("strHelPath=%s\n",strHelpPath);
//	DWORD dwCookie;
//	HtmlHelp(
//			NULL,
//			NULL,
//			HH_INITIALIZE,
//			(DWORD )&dwCookie);//Hhctrl.ocx �ɂ��Ԃ���� cookie
	
	HWND hwndHelp = ::HtmlHelp(hWnd,strHelpPath,HH_DISPLAY_INDEX,(DWORD_PTR )szViewFile);
/*	if(hwndHelp)
	{
		HtmlHelp(
				NULL,
				NULL,
				HH_UNINITIALIZE,
				(DWORD )dwCookie);//Hhctrl.ocx �ɂ��Ԃ���� cookie
		::SetForegroundWindow(hwndHelp);
	}
	else
	{
//		strcpy(szHelpFile,szHelpPath);
//		lstrcat(szHelpFile,README_FILE);
//		ShellExecute(hWnd,"open",szHelpFile,NULL,NULL,SW_SHOWNORMAL);
	}*/
}
