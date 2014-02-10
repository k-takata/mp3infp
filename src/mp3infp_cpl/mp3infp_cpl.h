// mp3infp_cpl.h : MP3INFP_CPL アプリケーションのメイン ヘッダー ファイルです。
//

#if !defined(AFX_MP3INFP_CPL_H__BE5BCB1F_0339_4A80_AA8C_365A4C87F21F__INCLUDED_)
#define AFX_MP3INFP_CPL_H__BE5BCB1F_0339_4A80_AA8C_365A4C87F21F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// メイン シンボル
#include <cpl.h>
#include <htmlhelp.h>

/////////////////////////////////////////////////////////////////////////////
// CMp3infp_cplApp
// このクラスの動作の定義に関しては mp3infp_cpl.cpp ファイルを参照してください。
//

class CMp3infp_cplApp : public CWinApp
{
public:
	CMp3infp_cplApp();
	virtual ~CMp3infp_cplApp();
	static	LONG __stdcall CPlApplet(HWND hWnd,UINT uMsg,LPARAM lParam1,LPARAM lParam2);

	static	LONG	OnDblclk(HWND hWnd,UINT uAppNum,LONG_PTR lData);
	static	LONG	OnExit();
	static	LONG	OnGetCount();
	static	LONG	OnInit();
	static	LONG	OnInquire(UINT uAppNum,CPLINFO* pInfo);
	static	LONG	OnNewInquire(UINT uAppNum,NEWCPLINFO* pInfo);
	static	LONG	OnStop(UINT uAppNum,LONG_PTR lData);
	static	LONG LoadLanguage();

	static	CMp3infp_cplApp* m_pThis;
	HINSTANCE m_hResource;

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CMp3infp_cplApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CMp3infp_cplApp)
		// メモ -  ClassWizard はこの位置にメンバ関数を追加または削除します。
		//         この位置に生成されるコードを編集しないでください。
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_MP3INFP_CPL_H__BE5BCB1F_0339_4A80_AA8C_365A4C87F21F__INCLUDED_)
