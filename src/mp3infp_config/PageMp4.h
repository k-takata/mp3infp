#if !defined(AFX_PAGEMP4_H__4373E84F_17E2_48F4_AA20_C5F03FFC4BEB__INCLUDED_)
#define AFX_PAGEMP4_H__4373E84F_17E2_48F4_AA20_C5F03FFC4BEB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageMp4.h : �w�b�_�[ �t�@�C��
//

/////////////////////////////////////////////////////////////////////////////
// CPageMp4 �_�C�A���O

class CPageMp4 : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageMp4)

// �R���X�g���N�V����
public:
	CPageMp4();
	~CPageMp4();

// �_�C�A���O �f�[�^
	//{{AFX_DATA(CPageMp4)
	enum { IDD = IDD_SETUP_MP4 };
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

	//{{AFX_VIRTUAL(CPageMp4)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:
	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(CPageMp4)
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

#endif // !defined(AFX_PAGEMP4_H__4373E84F_17E2_48F4_AA20_C5F03FFC4BEB__INCLUDED_)
