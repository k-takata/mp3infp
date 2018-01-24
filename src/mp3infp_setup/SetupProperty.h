#if !defined(AFX_SETUPPROPERTY_H__1ADE2790_4810_419E_ADA2_15FEBB2583E9__INCLUDED_)
#define AFX_SETUPPROPERTY_H__1ADE2790_4810_419E_ADA2_15FEBB2583E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetupProperty.h : �w�b�_�[ �t�@�C��
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

// �R���X�g���N�V����
public:
	CSetupProperty(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CSetupProperty(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// �A�g���r���[�g
public:

// �I�y���[�V����
public:

// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CSetupProperty)
	public:
	virtual BOOL OnInitDialog();
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
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

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
protected:
	//{{AFX_MSG(CSetupProperty)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_SETUPPROPERTY_H__1ADE2790_4810_419E_ADA2_15FEBB2583E9__INCLUDED_)
