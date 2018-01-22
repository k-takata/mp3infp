// PageMain.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "mp3infp_cpl.h"
#include "SetupProperty.h"

#include "PageMain.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CPageMain プロパティ ページ

IMPLEMENT_DYNCREATE(CPageMain, CPropertyPage)

CPageMain::CPageMain() : CPropertyPage(CPageMain::IDD)
{
	//{{AFX_DATA_INIT(CPageMain)
	m_bSaveTimeStamp = FALSE;
	m_staticInfo = _T("");
	m_bDisable_FDD = FALSE;
	m_bDisable_RAMOVABLE = FALSE;
	m_bDisable_NETWORK = FALSE;
	m_bSelectDrive = FALSE;
	m_bDisable_CDROM = FALSE;
	m_bContextMenu = TRUE;
	m_bColumn_8_3 = FALSE;
	//}}AFX_DATA_INIT
	//(共通)
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"SaveTimeStamp",(DWORD *)&m_bSaveTimeStamp,TRUE);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"ContextMenu",(DWORD *)&m_bContextMenu,TRUE);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"SelectDrive",(DWORD *)&m_bSelectDrive,DEF_SETUP_MAIN_SELECT_DRIVE);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"Disable_FDD",(DWORD *)&m_bDisable_FDD,DEF_SETUP_MAIN_DISABLE_FDD);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"Disable_RAMOVABLE",(DWORD *)&m_bDisable_RAMOVABLE,DEF_SETUP_MAIN_DISABLE_REMOVABLE);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"Disable_CDROM",(DWORD *)&m_bDisable_CDROM,DEF_SETUP_MAIN_DISABLE_CDROM);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"Disable_NETWORK",(DWORD *)&m_bDisable_NETWORK,DEF_SETUP_MAIN_DISABLE_NETWORK);
	regGetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"Column_8_3",(DWORD *)&m_bColumn_8_3,DEF_SETUP_MAIN_COLUMN_8_3);
	m_strLanguage = regGetStringEx(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"Language",(char *)(LPCSTR )DEF_SETUP_MAIN_LANGUAGE);

}

CPageMain::~CPageMain()
{
}

void CPageMain::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageMain)
	DDX_Control(pDX, IDC_CHECK_DISABLE_CDROM, m_wndDisable_CDROM);
	DDX_Control(pDX, IDC_CHECK_DISABLE_FDD, m_wndDisable_FDD);
	DDX_Control(pDX, IDC_CHECK_DISABLE_NETWORK, m_wndDisable_NETWORK);
	DDX_Control(pDX, IDC_CHECK_DISABLE_REMOVABLE, m_wndDisable_RAMOVABLE);
	DDX_Control(pDX, IDC_CHECK_SELECT_DRIVE, m_wndSelectDrive);
	DDX_Check(pDX, IDC_SAVE_TIMESTAMP, m_bSaveTimeStamp);
	DDX_Text(pDX, IDC_STATIC_INFO, m_staticInfo);
	DDX_Check(pDX, IDC_CHECK_DISABLE_FDD, m_bDisable_FDD);
	DDX_Check(pDX, IDC_CHECK_DISABLE_REMOVABLE, m_bDisable_RAMOVABLE);
	DDX_Check(pDX, IDC_CHECK_DISABLE_NETWORK, m_bDisable_NETWORK);
	DDX_Check(pDX, IDC_CHECK_SELECT_DRIVE, m_bSelectDrive);
	DDX_Check(pDX, IDC_CHECK_DISABLE_CDROM, m_bDisable_CDROM);
	DDX_Check(pDX, IDC_CHECK_ADD_CONTEXTMENU, m_bContextMenu);
	DDX_Check(pDX, IDC_CHECK_COLUMN_8_3, m_bColumn_8_3);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageMain, CPropertyPage)
	//{{AFX_MSG_MAP(CPageMain)
	ON_BN_CLICKED(IDC_DEFAULT, OnDefault)
	ON_BN_CLICKED(IDC_SAVE_TIMESTAMP, OnSetCheck)
	ON_CBN_SELCHANGE(IDC_COMBO_LANGUAGE, SetLanguage)
	ON_BN_CLICKED(IDC_HELPVIEW, OnHelpview)
	ON_BN_CLICKED(IDC_CHECK_ADD_CONTEXTMENU, OnSetCheck)
	ON_BN_CLICKED(IDC_CHECK_SELECT_DRIVE, OnSetCheck)
	ON_BN_CLICKED(IDC_CHECK_DISABLE_FDD, OnSetCheck)
	ON_BN_CLICKED(IDC_CHECK_DISABLE_REMOVABLE, OnSetCheck)
	ON_BN_CLICKED(IDC_CHECK_DISABLE_NETWORK, OnSetCheck)
	ON_BN_CLICKED(IDC_CHECK_DISABLE_CDROM, OnSetCheck)
	ON_BN_CLICKED(IDC_CHECK_COLUMN_8_3, OnSetCheck)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageMain メッセージ ハンドラ

BOOL CPageMain::OnApply() 
{
	CString strLanguage;
	LRESULT cur = ::SendMessage(
				GetDlgItem(IDC_COMBO_LANGUAGE)->GetSafeHwnd(),
				CB_GETCURSEL,
				0,
				0);
	if(cur != CB_ERR)
	{
		CString *str = (CString *)::SendMessage(
							GetDlgItem(IDC_COMBO_LANGUAGE)->GetSafeHwnd(),
							CB_GETITEMDATA,
							cur,
							0);
		if(str)
		{
			strLanguage = *str;
		}
		else
		{
			strLanguage = DEF_SETUP_MAIN_LANGUAGE;
		}
	}

	CString strLangPath;
	strLangPath = regGetStringEx(HKEY_LOCAL_MACHINE,MP3INFP_REG_ENTRY,"path","c:\\progra~1\\mp3infp");
	AddTailYenSign(strLangPath);
	strLangPath += "language\\";
	strLangPath += strLanguage;
	strLangPath += ".lng";
	HMODULE hResource = LoadLibrary(strLangPath);
	if(hResource)
	{
		char tmp[20];
		if((LoadString(hResource,IDS_LANGUAGE_VER,tmp,sizeof(tmp)) == 0) ||
			(atoi(tmp) < LANG_VER) )
		{
			CString str;
			// 「この言語パックはバージョンが古いために利用できません。→%s」
			str.LoadString(IDS_LANG_VER_ERROR);
			CString strTmp;
			strTmp.Format(str,strLangPath);
			AfxMessageBox(strTmp,MB_ICONINFORMATION);
			FreeLibrary(hResource);
			SelectLang(m_strLanguage);
			return FALSE;
		}
		FreeLibrary(hResource);
	}

	regSetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"SaveTimeStamp",(DWORD )m_bSaveTimeStamp);
	regSetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"ContextMenu",(DWORD )m_bContextMenu);
	regSetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"SelectDrive",(DWORD )m_bSelectDrive);
	regSetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"Disable_FDD",(DWORD )m_bDisable_FDD);
	regSetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"Disable_RAMOVABLE",(DWORD )m_bDisable_RAMOVABLE);
	regSetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"Disable_CDROM",(DWORD )m_bDisable_CDROM);
	regSetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"Disable_NETWORK",(DWORD )m_bDisable_NETWORK);
	regSetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"Column_8_3",(DWORD )m_bColumn_8_3);
	
	if(strcmp(m_strLanguage,strLanguage) != 0)
	{
		m_strLanguage = strLanguage;
		regSetString(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"Language",m_strLanguage);
		// 「言語パックの変更はダイアログの再起動後に有効です。」
		CString str;
		str.LoadString(IDS_RELOAD_CPL);
		AfxMessageBox(str,MB_YESNO|MB_ICONINFORMATION);
	}

	return CPropertyPage::OnApply();
}

void CPageMain::OnDefault() 
{
	//デフォルト値を設定
	m_bSaveTimeStamp		= TRUE;
	m_bContextMenu			= TRUE;
	m_bSelectDrive			= DEF_SETUP_MAIN_SELECT_DRIVE;
	m_bDisable_FDD			= DEF_SETUP_MAIN_DISABLE_FDD;
	m_bDisable_RAMOVABLE	= DEF_SETUP_MAIN_DISABLE_REMOVABLE;
	m_bDisable_CDROM		= DEF_SETUP_MAIN_DISABLE_CDROM;
	m_bDisable_NETWORK		= DEF_SETUP_MAIN_DISABLE_NETWORK;
	m_bColumn_8_3			= DEF_SETUP_MAIN_COLUMN_8_3;
	m_strLanguage			= DEF_SETUP_MAIN_LANGUAGE;
	
	CPropertyPage::OnInitDialog();
	OnSetCheck();
}

void CPageMain::OnSetCheck()
{
	SetCheck();
	SetModified();
}

void CPageMain::OnHelpview() 
{
	CSetupProperty *prop = (CSetupProperty *)GetParent();
	prop->OpenHtmlHelp(prop->m_hWnd,NULL);
}

BOOL CPageMain::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	//言語パックディレクトリを取得
	CString strLangPath;
	strLangPath = regGetStringEx(HKEY_LOCAL_MACHINE,MP3INFP_REG_ENTRY,"path","c:\\progra~1\\mp3infp");
	AddTailYenSign(strLangPath);
	strLangPath += "language\\*.lng";
	//コンボボックスの初期化
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(strLangPath,&fd);
	if(hFind != INVALID_HANDLE_VALUE)
	{
		char tmp[100];
		CString strLangDisp;
		do
		{
			strLangPath = regGetStringEx(HKEY_LOCAL_MACHINE,MP3INFP_REG_ENTRY,"path","c:\\progra~1\\mp3infp");
			AddTailYenSign(strLangPath);
			strLangPath += "language\\";
			strLangPath += fd.cFileName;
			HMODULE hResource = LoadLibrary(strLangPath);
			if(hResource == NULL)
			{
				continue;
			}
			::LoadString(hResource,IDS_LANGUAGE_NAME,tmp,sizeof(tmp));
			FreeLibrary(hResource);
			if(strlen(tmp) == 0)
			{
				continue;
			}
			strLangDisp.Format("%s - (%s)",tmp,fd.cFileName);
			::SendMessage(
					GetDlgItem(IDC_COMBO_LANGUAGE)->GetSafeHwnd(),
					CB_INSERTSTRING,
					0,
					(LPARAM )(LPCSTR )strLangDisp);
			char tmp[MAX_PATH];
			strcpy(tmp,fd.cFileName);
			cutExtName(tmp);
			CString *str = new CString(tmp);
			::SendMessage(
					GetDlgItem(IDC_COMBO_LANGUAGE)->GetSafeHwnd(),
					CB_SETITEMDATA,
					0,
					(LPARAM )str);
		}while(FindNextFile(hFind,&fd));
		FindClose(hFind);
	}
	::SendMessage(
			GetDlgItem(IDC_COMBO_LANGUAGE)->GetSafeHwnd(),
			CB_INSERTSTRING,
			0,
			(LPARAM )(LPCSTR )"English - (default)"
		);
	::SendMessage(
			GetDlgItem(IDC_COMBO_LANGUAGE)->GetSafeHwnd(),
			CB_SETITEMDATA,
			0,
			NULL);
	// 言語パックの選択
	SelectLang(m_strLanguage);
	// 翻訳者の表示
	CString str;
	str.LoadString(IDS_TRANSLATOR);
	LRESULT sel = ::SendMessage(
					GetDlgItem(IDC_COMBO_LANGUAGE)->GetSafeHwnd(),
					CB_GETCURSEL,
					0,
					0);
	if(sel != 0)
	{
		SetDlgItemText(IDC_STATIC_TRANSLATOR,str);
	}
	else
	{
		::ShowWindow(GetDlgItem(IDC_STATIC_TRANSLATOR)->GetSafeHwnd(),SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_STATIC_TRANSLATE)->GetSafeHwnd(),SW_HIDE);
	}
	SetCheck();

	return TRUE;  // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
	              // 例外: OCX プロパティ ページの戻り値は FALSE となります
}

void CPageMain::SelectLang(const char *langfile)
{
	// 指定ファイル名の言語をコンボボックスで選択
	LRESULT cnt = ::SendMessage(
				GetDlgItem(IDC_COMBO_LANGUAGE)->GetSafeHwnd(),
				CB_GETCOUNT,
				0,
				0);
	if(cnt == CB_ERR)
	{
		return;
	}
	for(long i=0; i<cnt; i++)
	{
		CString *str = (CString *)::SendMessage(
							GetDlgItem(IDC_COMBO_LANGUAGE)->GetSafeHwnd(),
							CB_GETITEMDATA,
							i,
							0);
		if(str)
		{
			if(str->CompareNoCase(langfile) == 0)
			{
				// 選択
				::SendMessage(
						GetDlgItem(IDC_COMBO_LANGUAGE)->GetSafeHwnd(),
						CB_SETCURSEL,
						i,
						0
					);
				break;
			}
		}
		else
		{
			// null(default)を選択
			::SendMessage(
					GetDlgItem(IDC_COMBO_LANGUAGE)->GetSafeHwnd(),
					CB_SETCURSEL,
					i,
					0
				);
		}
	}
}

void CPageMain::SetCheck()
{
	if(m_wndSelectDrive.GetCheck())
	{
		m_wndDisable_FDD.EnableWindow(TRUE);
		m_wndDisable_RAMOVABLE.EnableWindow(TRUE);
		m_wndDisable_CDROM.EnableWindow(TRUE);
		m_wndDisable_NETWORK.EnableWindow(TRUE);
	}
	else
	{
		m_wndDisable_FDD.EnableWindow(FALSE);
		m_wndDisable_RAMOVABLE.EnableWindow(FALSE);
		m_wndDisable_CDROM.EnableWindow(FALSE);
		m_wndDisable_NETWORK.EnableWindow(FALSE);
	}
}

void CPageMain::SetLanguage()
{
	SetModified();
}

void CPageMain::OnDestroy() 
{
	CPropertyPage::OnDestroy();
	
	// TODO: この位置にメッセージ ハンドラ用のコードを追加してください
	LRESULT cnt = ::SendMessage(
				GetDlgItem(IDC_COMBO_LANGUAGE)->GetSafeHwnd(),
				CB_GETCOUNT,
				0,
				0);
	if(cnt != CB_ERR)
	{
		for(long i=0; i<cnt; i++)
		{
			CString *str = (CString *)::SendMessage(
								GetDlgItem(IDC_COMBO_LANGUAGE)->GetSafeHwnd(),
								CB_GETITEMDATA,
								i,
								0);
			if(str)
			{
				delete str;
			}
		}
	}

}
