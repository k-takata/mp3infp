// PageWave.cpp : �C���v�������e�[�V���� �t�@�C��
//

#include "stdafx.h"
#include "SetupProperty.h"

#include "PageWave.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageWave �v���p�e�B �y�[�W

IMPLEMENT_DYNCREATE(CPageWave, CPropertyPage)

CPageWave::CPageWave() : CPropertyPage(CPageWave::IDD)
{
	//{{AFX_DATA_INIT(CPageWave)
	m_bInfotipEnable = FALSE;
	m_bColumnEnable = FALSE;
	m_bPropEnable = FALSE;
	m_strInfotipFormat = _T("");
	m_strInfotipInfo = _T("");
	m_intAviCodecFind = 0;
	//}}AFX_DATA_INIT
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"wave_PropEnable",(DWORD *)&m_bPropEnable,TRUE);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"wave_InfotipEnable",(DWORD *)&m_bInfotipEnable,TRUE);
	CString strDefault;
	strDefault.LoadString(IDS_DEF_INFOTIP_WAVE_FORMAT);
	m_strInfotipFormat = regGetStringEx(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"wave_InfotipFormat",(char *)(LPCSTR )strDefault);
	m_strInfotipInfo = LoadResString(AfxGetResourceHandle(),IDS_DEF_INFOTIP_INF_WAVE_FORMAT);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"wave_ColumnEnable",(DWORD *)&m_bColumnEnable,TRUE);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"wave_CodecFind",(DWORD *)&m_intAviCodecFind,0);
}

CPageWave::~CPageWave()
{
}

void CPageWave::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageWave)
	DDX_Control(pDX, IDC_CHECK_PROP_ENABLE, m_wndPropEnable);
	DDX_Control(pDX, IDC_EDIT_INFOTIP_INFO, m_wndInfotipInfo);
	DDX_Control(pDX, IDC_EDIT_INFOTIP_FORMAT, m_wndInfotipFormat);
	DDX_Control(pDX, IDC_CHECK_INFOTIP_ENABLE, m_wndInfotipEnable);
	DDX_Control(pDX, IDC_STATIC_INFOTIP_FORMAT, m_staticInfotipFormat);
	DDX_Check(pDX, IDC_CHECK_INFOTIP_ENABLE, m_bInfotipEnable);
	DDX_Check(pDX, IDC_COLUMN_ENABLE, m_bColumnEnable);
	DDX_Check(pDX, IDC_CHECK_PROP_ENABLE, m_bPropEnable);
	DDX_Text(pDX, IDC_EDIT_INFOTIP_FORMAT, m_strInfotipFormat);
	DDX_Text(pDX, IDC_EDIT_INFOTIP_INFO, m_strInfotipInfo);
	DDX_Radio(pDX, IDC_RADIO_AVICODEC_FIND1, m_intAviCodecFind);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageWave, CPropertyPage)
	//{{AFX_MSG_MAP(CPageWave)
	ON_BN_CLICKED(IDC_DEFAULT, OnDefault)
	ON_BN_CLICKED(IDC_HELPVIEW, OnHelpview)
	ON_BN_CLICKED(IDC_CHECK_INFOTIP_ENABLE, OnSetCheck)
	ON_EN_CHANGE(IDC_EDIT_INFOTIP_FORMAT, OnSetCheck)
	ON_BN_CLICKED(IDC_COLUMN_ENABLE, OnSetCheck)
	ON_BN_CLICKED(IDC_CHECK_PROP_ENABLE, OnSetCheck)
	ON_BN_CLICKED(IDC_RADIO_AVICODEC_FIND1, OnSetCheck)
	ON_BN_CLICKED(IDC_RADIO_AVICODEC_FIND2, OnSetCheck)
	ON_BN_CLICKED(IDC_RADIO_AVICODEC_FIND3, OnSetCheck)
	ON_BN_CLICKED(IDC_RADIO_AVICODEC_FIND4, OnSetCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageWave ���b�Z�[�W �n���h��

BOOL CPageWave::OnApply() 
{
	regSetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"wave_PropEnable",(DWORD )m_bPropEnable);
	regSetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"wave_InfotipEnable",(DWORD )m_bInfotipEnable);
	regSetString(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"wave_InfotipFormat",m_strInfotipFormat);
	regSetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"wave_ColumnEnable",(DWORD )m_bColumnEnable);
	regSetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"wave_CodecFind",(DWORD )m_intAviCodecFind);

	return CPropertyPage::OnApply();
}

void CPageWave::OnDefault() 
{
	//�f�t�H���g�l��ݒ�
	m_bPropEnable = TRUE;
	m_bInfotipEnable = TRUE;
	m_strInfotipFormat.LoadString(IDS_DEF_INFOTIP_WAVE_FORMAT);
	m_bColumnEnable = TRUE;
	m_intAviCodecFind = 0;

	OnInitDialog();
	SetModified();
}

void CPageWave::OnSetCheck()
{
	SetCheck();
	SetModified();
}

void CPageWave::SetCheck()
{
	//Infotip�L��
	if(m_wndInfotipEnable.GetCheck())
	{
		m_wndInfotipFormat.EnableWindow(TRUE);
		m_wndInfotipInfo.EnableWindow(TRUE);
		m_staticInfotipFormat.EnableWindow(TRUE);
	}
	else
	{
		m_wndInfotipFormat.EnableWindow(FALSE);
		m_wndInfotipInfo.EnableWindow(FALSE);
		m_staticInfotipFormat.EnableWindow(FALSE);
	}
}

BOOL CPageWave::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	SetCheck();
	
	return TRUE;  // �R���g���[���Ƀt�H�[�J�X��ݒ肵�Ȃ��Ƃ��A�߂�l�� TRUE �ƂȂ�܂�
	              // ��O: OCX �v���p�e�B �y�[�W�̖߂�l�� FALSE �ƂȂ�܂�
}


void CPageWave::OnHelpview() 
{
	CSetupProperty *prop = (CSetupProperty *)GetParent();
	prop->OpenHtmlHelp(m_hWnd,NULL);
}

