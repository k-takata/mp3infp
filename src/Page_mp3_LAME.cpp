#include "StdAfx.h"
#include "mp3infp.h"

UINT CALLBACK CShellExt::PageCallback_mp3_LAME(HWND hWnd, UINT uMessage, LPPROPSHEETPAGE ppsp)
{
	LPCSHELLEXT	lpcs = (CShellExt *)ppsp->lParam;
	switch(uMessage){
	case PSPCB_CREATE:	//�v���p�e�B�y�[�W���쐬�������
		return TRUE;
	case PSPCB_RELEASE:	//�v���p�e�B�y�[�W���j�󂳂����
		if(lpcs) 
		{
//			lpcs->m_hwndRiffSIF = NULL;
			lpcs->Release();
		}
		return TRUE;
	}

	return TRUE;
}

static void DispInfo(HWND hDlg,CShellExt *lpcs)
{
}

BOOL CALLBACK CShellExt::PageDlgProc_mp3_LAME(HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	LPCSHELLEXT	lpcs = (CShellExt *)GetWindowLongPtr(hDlg,DWLP_USER);
	CString strTmp;

	switch(uMessage){
	case WM_USER+1:	//�\���̃��t���b�V��
		DispInfo(hDlg,lpcs);
		break;
	case WM_INITDIALOG:
		{
			lpcs = (LPCSHELLEXT )((LPPROPSHEETPAGE )lParam)->lParam;
			SetWindowLongPtr(hDlg, DWLP_USER, (LONG_PTR)lpcs);
			//Ver���
			SetDlgItemText(hDlg,IDC_STATIC_VER_INFO,SOFT_NAME);
			//����\��
			DispInfo(hDlg,lpcs);
		}
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case IDC_CHECK_AOT:
			if(IsDlgButtonChecked(hDlg,IDC_CHECK_AOT) == BST_CHECKED)
			{
				lpcs->m_bPropAOT = TRUE;
				SetWindowPos(GetParent(hDlg),HWND_TOPMOST,0,0,0,0,
					SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOOWNERZORDER|SWP_NOSIZE);
			}
			else
			{
				lpcs->m_bPropAOT = FALSE;
				SetWindowPos(GetParent(hDlg),HWND_NOTOPMOST,0,0,0,0,
					SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOOWNERZORDER|SWP_NOSIZE);
			}
			if(lpcs->m_hwndId3v1)
				PostMessage(lpcs->m_hwndId3v1,WM_USER+1,0,0);
			if(lpcs->m_hwndId3v2)
				PostMessage(lpcs->m_hwndId3v2,WM_USER+1,0,0);
			regSetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("PropAOT"),(DWORD )lpcs->m_bPropAOT);
			break;
		case IDC_SETUP:
			OpenSetupPage(hDlg,2);
			break;
		case IDC_HELPVIEW:
			lpcs->OpenHtmlHelp(hDlg,_T("extension.htm"));
			break;
		}
		break;

	//�󋵈ˑ��w���v
	case WM_HELP:
	{
		TCHAR szTmp[256];
		lstrcpy(szTmp,SOFT_NAME);
		lstrcat(szTmp,_T(" "));
		lstrcat(szTmp,COPY_RIGHT);
		MessageBox(hDlg,szTmp,_T("About"),MB_APPLMODAL);
		break;
	}


	default:
		return FALSE;
	
	}

	return TRUE;
}
