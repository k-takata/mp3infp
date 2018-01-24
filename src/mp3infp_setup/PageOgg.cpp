// PageOgg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "SetupProperty.h"
#include "PageOgg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageOgg プロパティ ページ

IMPLEMENT_DYNCREATE(CPageOgg, CPropertyPage)

CPageOgg::CPageOgg() : CPropertyPage(CPageOgg::IDD)
{
	//{{AFX_DATA_INIT(CPageOgg)
	m_bInfotipEnable = FALSE;
	m_bColumnEnable = FALSE;
	m_bPropEnable = FALSE;
	m_strInfotipFormat = _T("");
	m_strInfotipInfo = _T("");
	//}}AFX_DATA_INIT
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"ogg_PropEnable",(DWORD *)&m_bPropEnable,DEF_OGG_PROP_ENABLE);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"ogg_InfotipEnable",(DWORD *)&m_bInfotipEnable,DEF_OGG_INFOTIP_ENABLE);
	CString strDefault;
	strDefault.LoadString(IDS_DEF_INFOTIP_OGG_FORMAT);
	m_strInfotipFormat = regGetStringEx(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"ogg_InfotipFormat",(char *)(LPCSTR )strDefault);
	m_strInfotipInfo = LoadResString(AfxGetResourceHandle(),IDS_DEF_INFOTIP_INF_OGG_FORMAT);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"ogg_ColumnEnable",(DWORD *)&m_bColumnEnable,DEF_OGG_COLUMN_ENABLE);
}

CPageOgg::~CPageOgg()
{
}

void CPageOgg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageOgg)
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
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageOgg, CPropertyPage)
	//{{AFX_MSG_MAP(CPageOgg)
	ON_BN_CLICKED(IDC_DEFAULT, OnDefault)
	ON_BN_CLICKED(IDC_HELPVIEW, OnHelpview)
	ON_BN_CLICKED(IDC_CHECK_INFOTIP_ENABLE, OnSetCheck)
	ON_EN_CHANGE(IDC_EDIT_INFOTIP_FORMAT, OnSetCheck)
	ON_BN_CLICKED(IDC_COLUMN_ENABLE, OnSetCheck)
	ON_BN_CLICKED(IDC_CHECK_PROP_ENABLE, OnSetCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageOgg メッセージ ハンドラ

BOOL CPageOgg::OnApply() 
{
	regSetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"ogg_PropEnable",(DWORD )m_bPropEnable);
	regSetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"ogg_InfotipEnable",(DWORD )m_bInfotipEnable);
	regSetString(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"ogg_InfotipFormat",m_strInfotipFormat);
	regSetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"ogg_ColumnEnable",(DWORD )m_bColumnEnable);

	return CPropertyPage::OnApply();
}

void CPageOgg::OnDefault() 
{
	//デフォルト値を設定
	m_bPropEnable = DEF_OGG_PROP_ENABLE;
	m_bInfotipEnable = DEF_OGG_INFOTIP_ENABLE;
	m_strInfotipFormat.LoadString(IDS_DEF_INFOTIP_OGG_FORMAT);
	m_bColumnEnable = DEF_OGG_COLUMN_ENABLE;

	OnInitDialog();
	SetModified();
}

void CPageOgg::OnSetCheck()
{
	SetCheck();
	SetModified();
}

void CPageOgg::SetCheck()
{
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

BOOL CPageOgg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	SetCheck();
	
	return TRUE;  // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
	              // 例外: OCX プロパティ ページの戻り値は FALSE となります
}


void CPageOgg::OnHelpview() 
{
	CSetupProperty *prop = (CSetupProperty *)GetParent();
	prop->OpenHtmlHelp(m_hWnd,NULL);
}

