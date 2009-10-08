// DlgWriteProgress.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "mp3infp.h"
#include "DlgWriteProgress.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgWriteProgress ダイアログ


CDlgWriteProgress::CDlgWriteProgress(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgWriteProgress::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgWriteProgress)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
	//}}AFX_DATA_INIT
}


void CDlgWriteProgress::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgWriteProgress)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgWriteProgress, CDialog)
	//{{AFX_MSG_MAP(CDlgWriteProgress)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgWriteProgress メッセージ ハンドラ
