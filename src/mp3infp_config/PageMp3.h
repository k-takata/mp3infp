#if !defined(AFX_PAGEMP3_H__BFF0441D_7050_40A9_9A6C_F658CE45823A__INCLUDED_)
#define AFX_PAGEMP3_H__BFF0441D_7050_40A9_9A6C_F658CE45823A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageMp3.h : �w�b�_�[ �t�@�C��
//

/////////////////////////////////////////////////////////////////////////////
// CPageMp3 �_�C�A���O

class CPageMp3 : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageMp3)

// �R���X�g���N�V����
public:
	CPageMp3();
	~CPageMp3();

// �_�C�A���O �f�[�^
	//{{AFX_DATA(CPageMp3)
	enum { IDD = IDD_SETUP_MP3 };
	CButton	m_staticId3v1;
	CButton	m_wndMp3PropEnable;
	CStatic	m_staticSoftDefault;
	CStatic	m_staticInfotipFormat;
	CEdit	m_wndRiffSoft;
	CEdit	m_wndInfotipInfo;
	CEdit	m_wndInfotipFormat;
	CButton	m_wndColumnEnable;
	CButton	m_wndInfotipEnable;
	CButton	m_wndId3v1ScmpxGenre;
	BOOL	m_bId3v1ScmpxGenre;
	CButton m_wndRiffSifDisable;
	BOOL	m_bRiffSifDisable;
	CString	m_strRiffSoft;
	BOOL	m_bInfotipEnable;
	BOOL	m_bColumnEnable;
	CString	m_strInfotipFormat;
	CString	m_strInfotipInfo;
	BOOL	m_bMp3PropEnable;
	//}}AFX_DATA


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B

	//{{AFX_VIRTUAL(CPageMp3)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:
	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(CPageMp3)
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

#endif // !defined(AFX_PAGEMP3_H__BFF0441D_7050_40A9_9A6C_F658CE45823A__INCLUDED_)
