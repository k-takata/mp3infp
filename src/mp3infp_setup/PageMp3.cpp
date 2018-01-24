// PageMp3.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "SetupProperty.h"

#include "PageMp3.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageMp3 プロパティ ページ

IMPLEMENT_DYNCREATE(CPageMp3, CPropertyPage)

CPageMp3::CPageMp3() : CPropertyPage(CPageMp3::IDD)
{
	//{{AFX_DATA_INIT(CPageMp3)
	m_bId3v1ScmpxGenre = FALSE;
	m_bRiffSifDisable = TRUE;
	m_strRiffSoft = _T("");
	m_bInfotipEnable = FALSE;
	m_bColumnEnable = FALSE;
	m_strInfotipFormat = _T("");
	m_strInfotipInfo = _T("");
	m_bMp3PropEnable = FALSE;
	//}}AFX_DATA_INIT
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"mp3_PropEnable",(DWORD *)&m_bMp3PropEnable,TRUE);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"mp3_Id3v1ScmpxGenre",(DWORD *)&m_bId3v1ScmpxGenre,FALSE);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"mp3_RiffSifDisable",(DWORD *)&m_bRiffSifDisable,TRUE);
	m_strRiffSoft = regGetStringEx(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"mp3_RmpSoft",APP_NAME);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"mp3_InfotipEnable",(DWORD *)&m_bInfotipEnable,TRUE);

	CString strDefault;
	strDefault.LoadString(IDS_DEF_INFOTIP_MP3_FORMAT);
	m_strInfotipFormat = regGetStringEx(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"mp3_InfotipFormat",(char *)(LPCSTR )strDefault);
	m_strInfotipInfo = LoadResString(AfxGetResourceHandle(),IDS_DEF_INFOTIP_INF_MP3_FORMAT);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"mp3_ColumnEnable",(DWORD *)&m_bColumnEnable,TRUE);
}

CPageMp3::~CPageMp3()
{
}

void CPageMp3::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageMp3)
	DDX_Control(pDX, IDC_CHECK_PROP_ENABLE, m_wndMp3PropEnable);
	DDX_Control(pDX, IDC_STATIC_SOFT_DEFAULT, m_staticSoftDefault);
	DDX_Control(pDX, IDC_STATIC_INFOTIP_FORMAT, m_staticInfotipFormat);
	DDX_Control(pDX, IDC_EDIT_RIFF_SOFT, m_wndRiffSoft);
	DDX_Control(pDX, IDC_EDIT_INFOTIP_INFO, m_wndInfotipInfo);
	DDX_Control(pDX, IDC_EDIT_INFOTIP_FORMAT, m_wndInfotipFormat);
	DDX_Control(pDX, IDC_COLUMN_ENABLE, m_wndColumnEnable);
	DDX_Control(pDX, IDC_CHECK_INFOTIP_ENABLE, m_wndInfotipEnable);
	DDX_Control(pDX, IDC_CHECK_ID3V1SCMPX_GENRE, m_wndId3v1ScmpxGenre);
	DDX_Check(pDX, IDC_CHECK_ID3V1SCMPX_GENRE, m_bId3v1ScmpxGenre);
	DDX_Control(pDX, IDC_CHECK_RIFF_DISABLE, m_wndRiffSifDisable);
	DDX_Check(pDX, IDC_CHECK_RIFF_DISABLE, m_bRiffSifDisable);
	DDX_Text(pDX, IDC_EDIT_RIFF_SOFT, m_strRiffSoft);
	DDX_Check(pDX, IDC_CHECK_INFOTIP_ENABLE, m_bInfotipEnable);
	DDX_Check(pDX, IDC_COLUMN_ENABLE, m_bColumnEnable);
	DDX_Text(pDX, IDC_EDIT_INFOTIP_FORMAT, m_strInfotipFormat);
	DDX_Text(pDX, IDC_EDIT_INFOTIP_INFO, m_strInfotipInfo);
	DDX_Check(pDX, IDC_CHECK_PROP_ENABLE, m_bMp3PropEnable);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageMp3, CPropertyPage)
	//{{AFX_MSG_MAP(CPageMp3)
	ON_BN_CLICKED(IDC_DEFAULT, OnDefault)
	ON_BN_CLICKED(IDC_CHECK_ID3V1SCMPX_GENRE, OnSetCheck)
	ON_BN_CLICKED(IDC_CHECK_RIFF_DISABLE, OnSetCheck)
	ON_EN_CHANGE(IDC_EDIT_RIFF_SOFT, OnSetCheck)
	ON_BN_CLICKED(IDC_CHECK_INFOTIP_ENABLE, OnSetCheck)
	ON_EN_CHANGE(IDC_EDIT_INFOTIP_FORMAT, OnSetCheck)
	ON_BN_CLICKED(IDC_COLUMN_ENABLE, OnSetCheck)
	ON_BN_CLICKED(IDC_CHECK_PROP_ENABLE, OnSetCheck)
	ON_BN_CLICKED(IDC_HELPVIEW, OnHelpview)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageMp3 メッセージ ハンドラ


BOOL CPageMp3::OnApply() 
{
	regSetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"mp3_PropEnable",(DWORD )m_bMp3PropEnable);
	regSetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"mp3_Id3v1ScmpxGenre",(DWORD )m_bId3v1ScmpxGenre);
	regSetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"mp3_RiffSifDisable",(DWORD )m_bRiffSifDisable);
	regSetString(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"mp3_RmpSoft",m_strRiffSoft);
	regSetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"mp3_InfotipEnable",(DWORD )m_bInfotipEnable);
	regSetString(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"mp3_InfotipFormat",m_strInfotipFormat);
	regSetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"mp3_ColumnEnable",(DWORD )m_bColumnEnable);

	return CPropertyPage::OnApply();
}

void CPageMp3::OnDefault() 
{
	//デフォルト値を設定
	m_bMp3PropEnable = TRUE;
	m_bId3v1ScmpxGenre = FALSE;
	m_bRiffSifDisable = TRUE;
	m_strRiffSoft = APP_NAME;
	m_bInfotipEnable = TRUE;
	m_strInfotipFormat.LoadString(IDS_DEF_INFOTIP_MP3_FORMAT);
	m_bColumnEnable = TRUE;

	OnInitDialog();
	SetModified();
}

void CPageMp3::OnSetCheck()
{
	SetCheck();
	SetModified();
}

void CPageMp3::SetCheck()
{
	//プロパティ拡張有効
	if(m_wndMp3PropEnable.GetCheck())
	{
		m_wndId3v1ScmpxGenre.EnableWindow(TRUE);
		m_wndRiffSifDisable.EnableWindow(TRUE);
		m_wndRiffSoft.EnableWindow(TRUE);
		m_staticSoftDefault.EnableWindow(TRUE);
	}
	else
	{
		m_wndId3v1ScmpxGenre.EnableWindow(FALSE);
		m_wndRiffSifDisable.EnableWindow(FALSE);
		m_wndRiffSoft.EnableWindow(FALSE);
		m_staticSoftDefault.EnableWindow(FALSE);
	}
	//Infotip有効
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

BOOL CPageMp3::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	SetCheck();
	
	return TRUE;  // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
	              // 例外: OCX プロパティ ページの戻り値は FALSE となります
}


void CPageMp3::OnHelpview() 
{
	CSetupProperty *prop = (CSetupProperty *)GetParent();
	prop->OpenHtmlHelp(m_hWnd,NULL);
}
