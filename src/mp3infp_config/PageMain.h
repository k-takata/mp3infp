#if !defined(AFX_PAGEMAIN_H__A50617C2_6D90_4F36_A9C0_37880745F547__INCLUDED_)
#define AFX_PAGEMAIN_H__A50617C2_6D90_4F36_A9C0_37880745F547__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageMain.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// CPageMain ダイアログ

class CPageMain : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageMain)

// コンストラクション
public:
	CPageMain();
	~CPageMain();

// ダイアログ データ
	//{{AFX_DATA(CPageMain)
	enum { IDD = IDD_SETUP };
	CButton	m_wndDisable_CDROM;
	CButton	m_wndDisable_FDD;
	CButton	m_wndDisable_NETWORK;
	CButton	m_wndDisable_REMOVABLE;
	CButton	m_wndSelectDrive;
	BOOL	m_bSaveTimeStamp;
	CString	m_staticInfo;
	BOOL	m_bDisable_FDD;
	BOOL	m_bDisable_REMOVABLE;
	BOOL	m_bDisable_NETWORK;
	BOOL	m_bSelectDrive;
	BOOL	m_bDisable_CDROM;
	BOOL	m_bContextMenu;
	BOOL	m_bColumn_8_3;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。

	//{{AFX_VIRTUAL(CPageMain)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CPageMain)
	afx_msg void OnDefault();
	afx_msg void OnSetCheck();
	afx_msg void SetLanguage();
	afx_msg void OnHelpview();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	CString	m_strLanguage;
	void CPageMain::SelectLang(const char *langfile);

private:
	void SetCheck();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_PAGEMAIN_H__A50617C2_6D90_4F36_A9C0_37880745F547__INCLUDED_)
