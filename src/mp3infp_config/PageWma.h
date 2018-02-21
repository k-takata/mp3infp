#if !defined(AFX_PAGEWMA_H__)
#define AFX_PAGEWMA_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageWma.h : �w�b�_�[ �t�@�C��
//

/////////////////////////////////////////////////////////////////////////////
// CPageWma �_�C�A���O

class CPageWma : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageWma)

// �R���X�g���N�V����
public:
	CPageWma();
	~CPageWma();

// �_�C�A���O �f�[�^
	//{{AFX_DATA(CPageWma)
	enum { IDD = IDD_SETUP_WMA };
	CButton	m_wndPropEnable;
	CEdit	m_wndInfotipInfo;
	CEdit	m_wndInfotipFormat;
	CButton	m_wndInfotipEnable;
	CStatic	m_staticInfotipFormat;
	BOOL	m_bInfotipEnable;
	BOOL	m_bColumnEnable;
	BOOL	m_bPropEnable;
	CString	m_strInfotipFormat;
	CString	m_strInfotipInfo;
	//}}AFX_DATA


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B

	//{{AFX_VIRTUAL(CPageWma)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:
	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(CPageWma)
	afx_msg void OnDefault();
	afx_msg void OnSetCheck();
	virtual BOOL OnInitDialog();
	afx_msg void OnHelpview();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void SetCheck();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_PAGEWMA_H__)
