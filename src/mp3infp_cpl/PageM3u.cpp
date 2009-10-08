// PageM3u.cpp : �C���v�������e�[�V���� �t�@�C��
//

#include "stdafx.h"
#include "SetupProperty.h"

#include "PageM3u.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageM3u �v���p�e�B �y�[�W

IMPLEMENT_DYNCREATE(CPageM3u, CPropertyPage)

CPageM3u::CPageM3u() : CPropertyPage(CPageM3u::IDD)
{
	//{{AFX_DATA_INIT(CPageM3u)
	m_bInfotipEnable = FALSE;
	m_bPropEnable = FALSE;
	//}}AFX_DATA_INIT
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"m3u_PropEnable",(DWORD *)&m_bPropEnable,DEF_M3U_PROP_ENABLE);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"m3u_InfotipEnable",(DWORD *)&m_bInfotipEnable,DEF_M3U_INFOTIP_ENABLE);
}

CPageM3u::~CPageM3u()
{
}

void CPageM3u::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageM3u)
	DDX_Control(pDX, IDC_CHECK_PROP_ENABLE, m_wndPropEnable);
	DDX_Control(pDX, IDC_CHECK_INFOTIP_ENABLE, m_wndInfotipEnable);
	DDX_Check(pDX, IDC_CHECK_INFOTIP_ENABLE, m_bInfotipEnable);
	DDX_Check(pDX, IDC_CHECK_PROP_ENABLE, m_bPropEnable);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageM3u, CPropertyPage)
	//{{AFX_MSG_MAP(CPageM3u)
	ON_BN_CLICKED(IDC_DEFAULT, OnDefault)
	ON_BN_CLICKED(IDC_HELPVIEW, OnHelpview)
	ON_BN_CLICKED(IDC_CHECK_INFOTIP_ENABLE, OnSetCheck)
	ON_BN_CLICKED(IDC_CHECK_PROP_ENABLE, OnSetCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageM3u ���b�Z�[�W �n���h��

BOOL CPageM3u::OnApply() 
{
	regSetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"m3u_PropEnable",(DWORD )m_bPropEnable);
	regSetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"m3u_InfotipEnable",(DWORD )m_bInfotipEnable);

	return CPropertyPage::OnApply();
}

void CPageM3u::OnDefault() 
{
	//�f�t�H���g�l��ݒ�
	m_bPropEnable = TRUE;
	m_bInfotipEnable = TRUE;

	OnInitDialog();
	SetModified();
}

void CPageM3u::OnSetCheck()
{
	SetCheck();
	SetModified();
}

void CPageM3u::SetCheck()
{
	//Infotip�L��
	if(m_wndInfotipEnable.GetCheck())
	{
	}
	else
	{
	}
}

BOOL CPageM3u::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	SetCheck();
	
	return TRUE;  // �R���g���[���Ƀt�H�[�J�X��ݒ肵�Ȃ��Ƃ��A�߂�l�� TRUE �ƂȂ�܂�
	              // ��O: OCX �v���p�e�B �y�[�W�̖߂�l�� FALSE �ƂȂ�܂�
}


void CPageM3u::OnHelpview() 
{
	CSetupProperty *prop = (CSetupProperty *)GetParent();
	prop->OpenHtmlHelp(m_hWnd,NULL);
}

