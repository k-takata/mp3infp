#if !defined(AFX_DLGWRITEPROGRESS_H__8B92AFD2_EE4F_4B7D_B288_61A992129C70__INCLUDED_)
#define AFX_DLGWRITEPROGRESS_H__8B92AFD2_EE4F_4B7D_B288_61A992129C70__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgWriteProgress.h : �w�b�_�[ �t�@�C��
//

/////////////////////////////////////////////////////////////////////////////
// CDlgWriteProgress �_�C�A���O

class CDlgWriteProgress : public CDialog
{
// �R���X�g���N�V����
public:
	CDlgWriteProgress(CWnd* pParent = NULL);   // �W���̃R���X�g���N�^

// �_�C�A���O �f�[�^
	//{{AFX_DATA(CDlgWriteProgress)
	enum { IDD = IDD_WRITE_PROGRESS };
		// ����: ClassWizard �͂��̈ʒu�Ƀf�[�^ �����o��ǉ����܂��B
	//}}AFX_DATA


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CDlgWriteProgress)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(CDlgWriteProgress)
		// ����: ClassWizard �͂��̈ʒu�Ƀ����o�֐���ǉ����܂��B
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_DLGWRITEPROGRESS_H__8B92AFD2_EE4F_4B7D_B288_61A992129C70__INCLUDED_)
