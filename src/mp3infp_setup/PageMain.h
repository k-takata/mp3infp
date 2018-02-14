#if !defined(AFX_PAGEMAIN_H__A50617C2_6D90_4F36_A9C0_37880745F547__INCLUDED_)
#define AFX_PAGEMAIN_H__A50617C2_6D90_4F36_A9C0_37880745F547__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageMain.h : �w�b�_�[ �t�@�C��
//

/////////////////////////////////////////////////////////////////////////////
// CPageMain �_�C�A���O

class CPageMain : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageMain)

// �R���X�g���N�V����
public:
	CPageMain();
	~CPageMain();

// �_�C�A���O �f�[�^
	//{{AFX_DATA(CPageMain)
	enum { IDD = IDD_SETUP };
	CButton	m_wndDisable_CDROM;
	CButton	m_wndDisable_FDD;
	CButton	m_wndDisable_NETWORK;
	CButton	m_wndDisable_REMOVABLE;
	CButton	m_wndSelectDrive;
	BOOL	m_bSaveTimeStamp;
	CString	m_staticInfo;
	BOOL	m_bDisable_FDD;
	BOOL	m_bDisable_REMOVABLE;
	BOOL	m_bDisable_NETWORK;
	BOOL	m_bSelectDrive;
	BOOL	m_bDisable_CDROM;
	BOOL	m_bContextMenu;
	BOOL	m_bColumn_8_3;
	//}}AFX_DATA


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B

	//{{AFX_VIRTUAL(CPageMain)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:
	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(CPageMain)
	afx_msg void OnDefault();
	afx_msg void OnSetCheck();
	afx_msg void SetLanguage();
	afx_msg void OnHelpview();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	CString	m_strLanguage;
	void CPageMain::SelectLang(const char *langfile);

private:
	void SetCheck();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_PAGEMAIN_H__A50617C2_6D90_4F36_A9C0_37880745F547__INCLUDED_)
