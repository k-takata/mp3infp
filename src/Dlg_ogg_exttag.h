#if !defined(AFX_DLG_OGG_EXTTAG_H__C42A8D30_5A92_4310_859E_81A32C7A461A__INCLUDED_)
#define AFX_DLG_OGG_EXTTAG_H__C42A8D30_5A92_4310_859E_81A32C7A461A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_ogg_exttag.h : �w�b�_�[ �t�@�C��
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_ogg_exttag �_�C�A���O

class CDlg_ogg_exttag : public CDialog
{
// �R���X�g���N�V����
public:
	CDlg_ogg_exttag(CWnd* pParent = NULL);   // �W���̃R���X�g���N�^

// �_�C�A���O �f�[�^
	//{{AFX_DATA(CDlg_ogg_exttag)
	enum { IDD = IDD_DIALOG_OGG_EXTTAG };
	CString	m_strName;
	CString	m_strValue;
	//}}AFX_DATA


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CDlg_ogg_exttag)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(CDlg_ogg_exttag)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_DLG_OGG_EXTTAG_H__C42A8D30_5A92_4310_859E_81A32C7A461A__INCLUDED_)
