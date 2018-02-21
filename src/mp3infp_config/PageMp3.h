#if !defined(AFX_PAGEMP3_H__BFF0441D_7050_40A9_9A6C_F658CE45823A__INCLUDED_)
#define AFX_PAGEMP3_H__BFF0441D_7050_40A9_9A6C_F658CE45823A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageMp3.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// CPageMp3 ダイアログ

class CPageMp3 : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageMp3)

// コンストラクション
public:
	CPageMp3();
	~CPageMp3();

// ダイアログ データ
	//{{AFX_DATA(CPageMp3)
	enum { IDD = IDD_SETUP_MP3 };
	CButton	m_staticId3v1;
	CButton	m_wndMp3PropEnable;
	CStatic	m_staticSoftDefault;
	CStatic	m_staticInfotipFormat;
	CEdit	m_wndRiffSoft;
	CEdit	m_wndInfotipInfo;
	CEdit	m_wndInfotipFormat;
	CButton	m_wndColumnEnable;
	CButton	m_wndInfotipEnable;
	CButton	m_wndId3v1ScmpxGenre;
	BOOL	m_bId3v1ScmpxGenre;
	CButton m_wndRiffSifDisable;
	BOOL	m_bRiffSifDisable;
	CString	m_strRiffSoft;
	BOOL	m_bInfotipEnable;
	BOOL	m_bColumnEnable;
	CString	m_strInfotipFormat;
	CString	m_strInfotipInfo;
	BOOL	m_bMp3PropEnable;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。

	//{{AFX_VIRTUAL(CPageMp3)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CPageMp3)
	afx_msg void OnDefault();
	afx_msg void OnSetCheck();
	virtual BOOL OnInitDialog();
	afx_msg void OnHelpview();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void SetCheck();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_PAGEMP3_H__BFF0441D_7050_40A9_9A6C_F658CE45823A__INCLUDED_)
