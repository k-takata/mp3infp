// mp3infp_config.h : MP3INFP_CONFIG �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
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
#include <htmlhelp.h>
#include <windowsx.h>

/////////////////////////////////////////////////////////////////////////////
// CMp3infpConfigApp
// ���̃N���X�̓���̒�`�Ɋւ��Ă� mp3infp_config.cpp �t�@�C�����Q�Ƃ��Ă��������B
//

class CMp3infpConfigApp : public CWinApp
{
public:
	CMp3infpConfigApp();
	virtual ~CMp3infpConfigApp();
	LONG LoadLanguage();

	HINSTANCE m_hResource;

// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CMp3infpConfigApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CMp3infpConfigApp)
		// ���� -  ClassWizard �͂��̈ʒu�Ƀ����o�֐���ǉ��܂��͍폜���܂��B
		//         ���̈ʒu�ɐ��������R�[�h��ҏW���Ȃ��ł��������B
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_MP3INFP_CPL_H__BE5BCB1F_0339_4A80_AA8C_365A4C87F21F__INCLUDED_)
