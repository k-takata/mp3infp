// Dlg_ogg_exttag.cpp : �C���v�������e�[�V���� �t�@�C��
//

#include "stdafx.h"
#include "mp3infp.h"
#include "Dlg_ogg_exttag.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_ogg_exttag �_�C�A���O


CDlg_ogg_exttag::CDlg_ogg_exttag(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_ogg_exttag::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_ogg_exttag)
	m_strName = _T("");
	m_strValue = _T("");
	//}}AFX_DATA_INIT
}


void CDlg_ogg_exttag::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_ogg_exttag)
	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
	DDX_Text(pDX, IDC_EDIT_VALUE, m_strValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_ogg_exttag, CDialog)
	//{{AFX_MSG_MAP(CDlg_ogg_exttag)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_ogg_exttag ���b�Z�[�W �n���h��

BOOL CDlg_ogg_exttag::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if(m_strName.GetLength())
	{
		::SetFocus(::GetDlgItem(m_hWnd,IDC_EDIT_VALUE));
	}

	return FALSE;
//	return TRUE;  // �R���g���[���Ƀt�H�[�J�X��ݒ肵�Ȃ��Ƃ��A�߂�l�� TRUE �ƂȂ�܂�
	              // ��O: OCX �v���p�e�B �y�[�W�̖߂�l�� FALSE �ƂȂ�܂�
}

void CDlg_ogg_exttag::OnOK()
{
	CString strName;
	CString strValue;

	GetDlgItem(IDC_EDIT_NAME)->GetWindowText(strName);
	GetDlgItem(IDC_EDIT_VALUE)->GetWindowText(strValue);
	//�t�B�[���h�������`�F�b�N
	if(strName.GetLength() == 0)
	{
		AfxMessageBox(IDS_PLEASE_TAG_NAME);
		::SetFocus(::GetDlgItem(m_hWnd,IDC_EDIT_NAME));
		::SendMessage(::GetDlgItem(m_hWnd,IDC_EDIT_NAME),EM_SETSEL,0,-1);
		return;
	}
	//�t�B�[���h���`�F�b�N
	for(int i=0; i<strName.GetLength(); i++)
	{
		int c = strName[i];
		if(
			!((c >= ' ') && ('}' >= c))	//0x20 - 0x7d
			||
			(c == '=')	//0x3d
			)
		{
			//�֎~�����𔭌�
			AfxMessageBox(IDS_OGG_BAD_CHAR);
			::SetFocus(::GetDlgItem(m_hWnd,IDC_EDIT_NAME));
			::SendMessage(::GetDlgItem(m_hWnd,IDC_EDIT_NAME),EM_SETSEL,0,-1);
			return;
		}

	}
	
	CDialog::OnOK();
}
