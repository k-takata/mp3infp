#if !defined(AFX_PAGEM3U_H__34A00BA4_7997_4432_8DC3_AEBD4B8BBD31__INCLUDED_)
#define AFX_PAGEM3U_H__34A00BA4_7997_4432_8DC3_AEBD4B8BBD31__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageM3u.h : �w�b�_�[ �t�@�C��
//

/////////////////////////////////////////////////////////////////////////////
// CPageM3u �_�C�A���O

class CPageM3u : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageM3u)

// �R���X�g���N�V����
public:
	CPageM3u();
	~CPageM3u();

// �_�C�A���O �f�[�^
	//{{AFX_DATA(CPageM3u)
	enum { IDD = IDD_SETUP_M3U };
	CButton	m_wndPropEnable;
	CButton	m_wndInfotipEnable;
	BOOL	m_bInfotipEnable;
	BOOL	m_bPropEnable;
	//}}AFX_DATA


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B

	//{{AFX_VIRTUAL(CPageM3u)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:
	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(CPageM3u)
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

#endif // !defined(AFX_PAGEM3U_H__34A00BA4_7997_4432_8DC3_AEBD4B8BBD31__INCLUDED_)
