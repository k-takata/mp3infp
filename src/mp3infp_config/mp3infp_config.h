// mp3infp_config.h : MP3INFP_CONFIG アプリケーションのメイン ヘッダー ファイルです。
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
#include <htmlhelp.h>
#include <windowsx.h>

/////////////////////////////////////////////////////////////////////////////
// CMp3infpConfigApp
// このクラスの動作の定義に関しては mp3infp_config.cpp ファイルを参照してください。
//

class CMp3infpConfigApp : public CWinApp
{
public:
	CMp3infpConfigApp();
	virtual ~CMp3infpConfigApp();
	LONG LoadLanguage();

	HINSTANCE m_hResource;

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CMp3infpConfigApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CMp3infpConfigApp)
		// メモ -  ClassWizard はこの位置にメンバ関数を追加または削除します。
		//         この位置に生成されるコードを編集しないでください。
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_MP3INFP_CPL_H__BE5BCB1F_0339_4A80_AA8C_365A4C87F21F__INCLUDED_)
