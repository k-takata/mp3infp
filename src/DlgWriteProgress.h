#if !defined(AFX_DLGWRITEPROGRESS_H__8B92AFD2_EE4F_4B7D_B288_61A992129C70__INCLUDED_)
#define AFX_DLGWRITEPROGRESS_H__8B92AFD2_EE4F_4B7D_B288_61A992129C70__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgWriteProgress.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// CDlgWriteProgress ダイアログ

class CDlgWriteProgress : public CDialog
{
// コンストラクション
public:
	CDlgWriteProgress(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(CDlgWriteProgress)
	enum { IDD = IDD_WRITE_PROGRESS };
		// メモ: ClassWizard はこの位置にデータ メンバを追加します。
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CDlgWriteProgress)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CDlgWriteProgress)
		// メモ: ClassWizard はこの位置にメンバ関数を追加します。
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_DLGWRITEPROGRESS_H__8B92AFD2_EE4F_4B7D_B288_61A992129C70__INCLUDED_)
