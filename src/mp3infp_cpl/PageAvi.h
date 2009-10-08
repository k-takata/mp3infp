#if !defined(AFX_PAGEAVI_H__1D5A869E_3041_4E4B_91DD_E03ECB1B4676__INCLUDED_)
#define AFX_PAGEAVI_H__1D5A869E_3041_4E4B_91DD_E03ECB1B4676__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageAvi.h : �w�b�_�[ �t�@�C��
//

/////////////////////////////////////////////////////////////////////////////
// CPageAvi �_�C�A���O

class CPageAvi : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageAvi)

// �R���X�g���N�V����
public:
	CPageAvi();
	~CPageAvi();

// �_�C�A���O �f�[�^
	//{{AFX_DATA(CPageAvi)
	enum { IDD = IDD_SETUP_AVI };
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
	int		m_intAviCodecFind;
	//}}AFX_DATA


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B

	//{{AFX_VIRTUAL(CPageAvi)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:
	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(CPageAvi)
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

#endif // !defined(AFX_PAGEAVI_H__1D5A869E_3041_4E4B_91DD_E03ECB1B4676__INCLUDED_)
