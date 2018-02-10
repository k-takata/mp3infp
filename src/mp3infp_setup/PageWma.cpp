// PageWma.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "SetupProperty.h"

#include "PageWma.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageWma プロパティ ページ

IMPLEMENT_DYNCREATE(CPageWma, CPropertyPage)

CPageWma::CPageWma() : CPropertyPage(CPageWma::IDD)
{
	//{{AFX_DATA_INIT(CPageWma)
	m_bInfotipEnable = FALSE;
	m_bColumnEnable = FALSE;
	m_bPropEnable = FALSE;
	m_strInfotipFormat = _T("");
	m_strInfotipInfo = _T("");
	//}}AFX_DATA_INIT
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"wma_PropEnable",(DWORD *)&m_bPropEnable,TRUE);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"wma_InfotipEnable",(DWORD *)&m_bInfotipEnable,TRUE);
	CString strDefault;
	strDefault.LoadString(IDS_DEF_INFOTIP_WMA_FORMAT);
	m_strInfotipFormat = regGetStringEx(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"wma_InfotipFormat",strDefault);
	m_strInfotipInfo = LoadResString(AfxGetResourceHandle(),IDS_DEF_INFOTIP_INF_WMA_FORMAT);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"wma_ColumnEnable",(DWORD *)&m_bColumnEnable,TRUE);
}

CPageWma::~CPageWma()
{
}

void CPageWma::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageWma)
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


BEGIN_MESSAGE_MAP(CPageWma, CPropertyPage)
	//{{AFX_MSG_MAP(CPageWma)
	ON_BN_CLICKED(IDC_DEFAULT, OnDefault)
	ON_BN_CLICKED(IDC_HELPVIEW, OnHelpview)
	ON_BN_CLICKED(IDC_CHECK_INFOTIP_ENABLE, OnSetCheck)
	ON_EN_CHANGE(IDC_EDIT_INFOTIP_FORMAT, OnSetCheck)
	ON_BN_CLICKED(IDC_COLUMN_ENABLE, OnSetCheck)
	ON_BN_CLICKED(IDC_CHECK_PROP_ENABLE, OnSetCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageWma メッセージ ハンドラ

BOOL CPageWma::OnApply() 
{
	regSetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"wma_PropEnable",(DWORD )m_bPropEnable);
	regSetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"wma_InfotipEnable",(DWORD )m_bInfotipEnable);
	regSetString(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"wma_InfotipFormat",m_strInfotipFormat);
	regSetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"wma_ColumnEnable",(DWORD )m_bColumnEnable);

	return CPropertyPage::OnApply();
}

void CPageWma::OnDefault() 
{
	//デフォルト値を設定
	m_bPropEnable = TRUE;
	m_bInfotipEnable = TRUE;
	m_strInfotipFormat.LoadString(IDS_DEF_INFOTIP_WMA_FORMAT);
	m_bColumnEnable = TRUE;

	OnInitDialog();
	SetModified();
}

void CPageWma::OnSetCheck()
{
	SetCheck();
	SetModified();
}

void CPageWma::SetCheck()
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

BOOL CPageWma::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	SetCheck();
	
	return TRUE;  // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
	              // 例外: OCX プロパティ ページの戻り値は FALSE となります
}


void CPageWma::OnHelpview() 
{
	CSetupProperty *prop = (CSetupProperty *)GetParent();
	prop->OpenHtmlHelp(m_hWnd,NULL);
}

