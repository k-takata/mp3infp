// mp3infp.h : MP3INFP �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
//

#if !defined(AFX_MP3INFP_H__21500EA5_1E06_44AC_820D_06ABE22ECB4C__INCLUDED_)
#define AFX_MP3INFP_H__21500EA5_1E06_44AC_820D_06ABE22ECB4C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

//
// Initialize GUIDs (should be done only and at-least once per DLL/EXE)
//
#pragma data_seg(".text")
#define INITGUID
#include <initguid.h>
#include <shlguid.h>
#include "classID.h"
#include "ver.h"
#pragma data_seg()

//#include <Ntquery.h>
#include <htmlhelp.h>
//#include <atlbase.h>
#include <windowsx.h>

#include "Resource.h"
//#include "Frontend.h"
#include "RegAccess.h"
#include "GlobalCommand.h"
#include "MMCommand.h"
#include "Id3tagv1.h"
#include "Id3tagv2.h"
#include "RMP.h"
#include "RiffSIF.h"
#include "Vqf.h"
#include "Mp3Info.h"
#include "M3u.h"
#include "Tag_Ogg.h"
#include "Tag_Ape.h"
#include "Info_MonkeysAudio.h"
#include "Tag_OpenDML.h"
#include "Tag_Wma.h"
#include "Tag_Mp4.h"
#include "ShellExt.h"
#include "ShellExtClassFactory.h"
#include "mp3infp_ExportFunc.h"

extern UINT		g_cRefThisDll;// Reference count of this DLL.
extern DWORD	g_dwCookie;
#define REW_TIMER	1
#define FFW_TIMER	2

/////////////////////////////////////////////////////////////////////////////
// CMp3infpApp
// ���̃N���X�̓���̒�`�Ɋւ��Ă� mp3infp.cpp �t�@�C�����Q�Ƃ��Ă��������B
//

class CMp3infpApp : public CWinApp
{
public:
	CMp3infpApp();
	virtual ~CMp3infpApp();

	HINSTANCE		m_hResource;	//���\�[�XDLL�̃C���X�^���X�n���h��

	//ExportFunc�Ŏg�p
	CString			m_strAudioFormat;
	CString			m_strVideoFormat;
	CString			m_strStreamFormat;
	CString			m_strTime;
	BOOL			m_bAvi2;

	CString			m_strSelectFile;
	CShellExt::EN_FILETYPE	m_fileType;
	__int64			m_i64FileSize;
	CMp3Info		m_Mp3info;
	CId3tagv1		m_Id3tagv1;
//	BOOL			m_bId3v1ScmpxGenre;
	CId3tagv2		m_Id3tagv2;
	CRMP			m_Rmp3;
	CRiffSIF		m_RiffSIF;
	int				m_iWaveCodecFind;	// �R�[�f�b�N���̎擾���@
	CVqf			m_Vqf;
	CTag_Wma		m_Wma;
	CTag_Ogg		m_Ogg;
	CTag_Ape		m_Ape;
	CTag_Mp4		m_Mp4;
	CInfo_MonkeysAudio m_Monkeys;
	CTag_OpenDML	m_OpenDML;
	int				m_iAviCodecFind;	// �R�[�f�b�N���̎擾���@

// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CMp3infpApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CMp3infpApp)
		// ���� -  ClassWizard �͂��̈ʒu�Ƀ����o�֐���ǉ��܂��͍폜���܂��B
		//         ���̈ʒu�ɐ��������R�[�h��ҏW���Ȃ��ł��������B
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

extern CMp3infpApp theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_MP3INFP_H__21500EA5_1E06_44AC_820D_06ABE22ECB4C__INCLUDED_)
