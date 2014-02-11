// mp3infp_cpl.h : MP3INFP_CPL �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
//

#if !defined(AFX_MP3INFP_CPL_H__BE5BCB1F_0339_4A80_AA8C_365A4C87F21F__INCLUDED_)
#define AFX_MP3INFP_CPL_H__BE5BCB1F_0339_4A80_AA8C_365A4C87F21F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// ���C�� �V���{��
#include <cpl.h>
#include <htmlhelp.h>

/////////////////////////////////////////////////////////////////////////////
// CMp3infp_cplApp
// ���̃N���X�̓���̒�`�Ɋւ��Ă� mp3infp_cpl.cpp �t�@�C�����Q�Ƃ��Ă��������B
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

// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CMp3infp_cplApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CMp3infp_cplApp)
		// ���� -  ClassWizard �͂��̈ʒu�Ƀ����o�֐���ǉ��܂��͍폜���܂��B
		//         ���̈ʒu�ɐ��������R�[�h��ҏW���Ȃ��ł��������B
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_MP3INFP_CPL_H__BE5BCB1F_0339_4A80_AA8C_365A4C87F21F__INCLUDED_)
