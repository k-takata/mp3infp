#if !defined(AFX_PAGEAPE_H__4373E84F_17E2_48F4_AA20_C5F03FFC4BEB__INCLUDED_)
#define AFX_PAGEAPE_H__4373E84F_17E2_48F4_AA20_C5F03FFC4BEB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageApe.h : �w�b�_�[ �t�@�C��
//

/////////////////////////////////////////////////////////////////////////////
// CPageApe �_�C�A���O

class CPageApe : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageApe)

// �R���X�g���N�V����
public:
	CPageApe();
	~CPageApe();

// �_�C�A���O �f�[�^
	//{{AFX_DATA(CPageApe)
	enum { IDD = IDD_SETUP_APE };
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

	//{{AFX_VIRTUAL(CPageApe)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:
	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(CPageApe)
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

#endif // !defined(AFX_PAGEAPE_H__4373E84F_17E2_48F4_AA20_C5F03FFC4BEB__INCLUDED_)
