#if !defined(AFX_PAGEM3U_H__34A00BA4_7997_4432_8DC3_AEBD4B8BBD31__INCLUDED_)
#define AFX_PAGEM3U_H__34A00BA4_7997_4432_8DC3_AEBD4B8BBD31__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageM3u.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// CPageM3u ダイアログ

class CPageM3u : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageM3u)

// コンストラクション
public:
	CPageM3u();
	~CPageM3u();

// ダイアログ データ
	//{{AFX_DATA(CPageM3u)
	enum { IDD = IDD_SETUP_M3U };
	CButton	m_wndPropEnable;
	CButton	m_wndInfotipEnable;
	BOOL	m_bInfotipEnable;
	BOOL	m_bPropEnable;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。

	//{{AFX_VIRTUAL(CPageM3u)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CPageM3u)
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

#endif // !defined(AFX_PAGEM3U_H__34A00BA4_7997_4432_8DC3_AEBD4B8BBD31__INCLUDED_)
