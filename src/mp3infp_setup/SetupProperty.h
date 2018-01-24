#if !defined(AFX_SETUPPROPERTY_H__1ADE2790_4810_419E_ADA2_15FEBB2583E9__INCLUDED_)
#define AFX_SETUPPROPERTY_H__1ADE2790_4810_419E_ADA2_15FEBB2583E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetupProperty.h : ヘッダー ファイル
//

#include "resource.h"
#include "../RegAccess.h"
#include "../classID.h"
#include "../GlobalCommand.h"

#include "PageMain.h"
#include "PageMp3.h"
#include "PageWave.h"
#include "PageAvi.h"
#include "PageVqf.h"
#include "PageWma.h"
#include "PageM3u.h"
#include "PageOgg.h"
#include "PageApe.h"
#include "PageMp4.h"

typedef HWND (WINAPI *LPHTMLHELP)(HWND hwndCaller,LPCSTR pszFile,UINT uCommand,DWORD_PTR dwData);

/////////////////////////////////////////////////////////////////////////////
// CSetupProperty

class CSetupProperty : public CPropertySheet
{
	DECLARE_DYNAMIC(CSetupProperty)

// コンストラクション
public:
	CSetupProperty(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CSetupProperty(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// アトリビュート
public:

// オペレーション
public:

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CSetupProperty)
	public:
	virtual BOOL OnInitDialog();
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// インプリメンテーション
public:
	virtual ~CSetupProperty();
	void OpenHtmlHelp(HWND hWnd,char *szViewFile);
private:
	CPageMain pageMain;
	CPageMp3 pageMp3;
	CPageWave pageWave;
	CPageAvi pageAvi;
	CPageVqf pageVqf;
	CPageWma pageWma;
	CPageM3u pageM3u;
	CPageOgg pageOgg;
	CPageApe pageApe;
	CPageMp4 pageMp4;
	void AddPages();

	// 生成されたメッセージ マップ関数
protected:
	//{{AFX_MSG(CSetupProperty)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_SETUPPROPERTY_H__1ADE2790_4810_419E_ADA2_15FEBB2583E9__INCLUDED_)
