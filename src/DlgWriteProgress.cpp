// DlgWriteProgress.cpp : �C���v�������e�[�V���� �t�@�C��
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
// CDlgWriteProgress �_�C�A���O


CDlgWriteProgress::CDlgWriteProgress(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgWriteProgress::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgWriteProgress)
		// ���� - ClassWizard �͂��̈ʒu�Ƀ}�b�s���O�p�̃}�N����ǉ��܂��͍폜���܂��B
	//}}AFX_DATA_INIT
}


void CDlgWriteProgress::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgWriteProgress)
		// ���� - ClassWizard �͂��̈ʒu�Ƀ}�b�s���O�p�̃}�N����ǉ��܂��͍폜���܂��B
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgWriteProgress, CDialog)
	//{{AFX_MSG_MAP(CDlgWriteProgress)
		// ���� - ClassWizard �͂��̈ʒu�Ƀ}�b�s���O�p�̃}�N����ǉ��܂��͍폜���܂��B
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgWriteProgress ���b�Z�[�W �n���h��
