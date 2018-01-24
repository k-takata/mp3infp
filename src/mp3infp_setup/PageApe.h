#if !defined(AFX_PAGEAPE_H__4373E84F_17E2_48F4_AA20_C5F03FFC4BEB__INCLUDED_)
#define AFX_PAGEAPE_H__4373E84F_17E2_48F4_AA20_C5F03FFC4BEB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageApe.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// CPageApe ダイアログ

class CPageApe : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageApe)

// コンストラクション
public:
	CPageApe();
	~CPageApe();

// ダイアログ データ
	//{{AFX_DATA(CPageApe)
	enum { IDD = IDD_SETUP_APE };
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

	//{{AFX_VIRTUAL(CPageApe)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CPageApe)
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

#endif // !defined(AFX_PAGEAPE_H__4373E84F_17E2_48F4_AA20_C5F03FFC4BEB__INCLUDED_)
