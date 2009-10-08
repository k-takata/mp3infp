#if !defined(AFX_PAGEVQF_H__1D5A869E_3041_4E4B_91DD_E03ECB1B4676__INCLUDED_)
#define AFX_PAGEVQF_H__1D5A869E_3041_4E4B_91DD_E03ECB1B4676__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageVqf.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// CPageVqf ダイアログ

class CPageVqf : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageVqf)

// コンストラクション
public:
	CPageVqf();
	~CPageVqf();

// ダイアログ データ
	//{{AFX_DATA(CPageVqf)
	enum { IDD = IDD_SETUP_VQF };
	CButton	m_wndPropEnable;
	CEdit	m_wndInfotipInfo;
	CEdit	m_wndInfotipFormat;
	CButton	m_wndInfotipEnable;
	CStatic	m_staticInfotipFormat;
	BOOL	m_bInfotipEnable;
	BOOL	m_bColumnEnable;
	BOOL	m_bPropEnable;
	CString	m_strInfotipFormat;
	CString	m_strInfotipInfo;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。

	//{{AFX_VIRTUAL(CPageVqf)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CPageVqf)
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

#endif // !defined(AFX_PAGEVQF_H__1D5A869E_3041_4E4B_91DD_E03ECB1B4676__INCLUDED_)
