#if !defined(AFX_PAGEOGG_H__288F90B1_11C1_4526_83BF_077A38DD7DB3__INCLUDED_)
#define AFX_PAGEOGG_H__288F90B1_11C1_4526_83BF_077A38DD7DB3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageOgg.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// CPageOgg ダイアログ

class CPageOgg : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageOgg)

// コンストラクション
public:
	CPageOgg();
	~CPageOgg();

// ダイアログ データ
	//{{AFX_DATA(CPageOgg)
	enum { IDD = IDD_SETUP_OGG };
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

	//{{AFX_VIRTUAL(CPageOgg)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CPageOgg)
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

#endif // !defined(AFX_PAGEOGG_H__288F90B1_11C1_4526_83BF_077A38DD7DB3__INCLUDED_)
