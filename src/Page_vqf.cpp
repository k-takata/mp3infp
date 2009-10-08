#include "StdAfx.h"
#include "mp3infp.h"

static int ids[] =
{
	-1,
	-1,
	CLP_NAM,
	CLP_ART,
	CLP_FILE,
	CLP_COP,
	CLP_CMT,
	0
};

static int staticWnd[] =
{
	IDC_STATIC_FORMAT_,
	IDC_STATIC_TIME_,
	IDC_STATIC_NAM,
	IDC_STATIC_ART,
	IDC_STATIC_FILE,
	IDC_STATIC_COP,
	IDC_STATIC_CMT,
	0
};

static int editWnd[] =
{
	IDC_STATIC_FORMAT,
	IDC_STATIC_TIME,
	IDC_EDIT_NAM,
	IDC_EDIT_ART,
	IDC_EDIT_FILE,
	IDC_EDIT_COP,
	IDC_EDIT_CMT,
	0
};

UINT CALLBACK CShellExt::PageCallback_vqf(HWND hWnd,UINT uMessage,LPPROPSHEETPAGE ppsp)
{
	LPCSHELLEXT	lpcs = (CShellExt *)ppsp->lParam;
	switch(uMessage){
	case PSPCB_CREATE:	//�v���p�e�B�y�[�W���쐬�������
		return TRUE;
	case PSPCB_RELEASE:	//�v���p�e�B�y�[�W���j�󂳂����
		if(lpcs) 
		{
			lpcs->m_hwndPage = NULL;
			lpcs->Release();
		}
		return TRUE;
	}

	return TRUE;
}

static void EnableEdit(HWND hDlg,CShellExt *lpcs,BOOL bEnable)
{
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_FORMAT_),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_STATIC_FORMAT),"");
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_FORMAT),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_TIME_),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_STATIC_TIME),"");
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_TIME),bEnable);

	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_NAM),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_NAM),"");
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_NAM),bEnable);

	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_ART),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_ART),"");
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_ART),bEnable);

	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_FILE),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_FILE),"");
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_FILE),bEnable);

	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_COP),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_COP),"");
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_COP),bEnable);

	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_CMT),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_CMT),"");
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_CMT),bEnable);
}

static void DispInfo(HWND hDlg,CShellExt *lpcs)
{
	if(lpcs->m_Vqf.IsEnable())
	{
		EnableEdit(hDlg,lpcs,TRUE);

		//vqf
		SetDlgItemText(hDlg,IDC_STATIC_FORMAT,lpcs->m_Vqf.GetFormatString());
		SetDlgItemText(hDlg,IDC_STATIC_TIME,lpcs->m_Vqf.GetTimeString());

		DWORD dwSize;
		unsigned char *data;
		//�^�C�g��
		data = lpcs->m_Vqf.GetField('N','A','M','E',&dwSize);
		if(data)
		{
			SetDlgItemText(hDlg,IDC_EDIT_NAM,(const char *)data);
		}
		//�A�[�e�B�X�g
		data = lpcs->m_Vqf.GetField('A','U','T','H',&dwSize);
		if(data)
		{
			SetDlgItemText(hDlg,IDC_EDIT_ART,(const char *)data);
		}
		//�ۑ���
		data = lpcs->m_Vqf.GetField('F','I','L','E',&dwSize);
		if(data)
		{
			SetDlgItemText(hDlg,IDC_EDIT_FILE,(const char *)data);
		}
		//���쌠
		data = lpcs->m_Vqf.GetField('(','c',')',' ',&dwSize);
		if(data)
		{
			SetDlgItemText(hDlg,IDC_EDIT_COP,(const char *)data);
		}
		//�R�����g
		data = lpcs->m_Vqf.GetField('C','O','M','T',&dwSize);
		if(data)
		{
			SetDlgItemText(hDlg,IDC_EDIT_CMT,(const char *)data);
		}

		lpcs->m_bApply = FALSE;
	}
	else
	{
		EnableEdit(hDlg,lpcs,FALSE);
	}
	if(lpcs->m_bPropAOT)
	{
		CheckDlgButton(hDlg,IDC_CHECK_AOT,BST_CHECKED);
		SetWindowPos(GetParent(hDlg),HWND_TOPMOST,0,0,0,0,
			SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOOWNERZORDER|SWP_NOSIZE);
	}
	else
	{
		CheckDlgButton(hDlg,IDC_CHECK_AOT,BST_UNCHECKED);
		SetWindowPos(GetParent(hDlg),HWND_NOTOPMOST,0,0,0,0,
			SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOOWNERZORDER|SWP_NOSIZE);
	}
}

BOOL CALLBACK CShellExt::PageDlgProc_vqf(HWND hDlg,UINT uMessage,WPARAM wParam,LPARAM lParam)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	TRACE(_T("PageDlgProc_avi(mes=%08x,wParam=%08x,lParam=%08x\r\n"),uMessage,wParam,lParam);
	LPCSHELLEXT	lpcs = (CShellExt *)GetWindowLongPtr(hDlg,DWLP_USER);
	CString strTmp;

	switch(uMessage){
	case WM_INITDIALOG:
		{
			lpcs = (LPCSHELLEXT )((LPPROPSHEETPAGE )lParam)->lParam;
			lpcs->m_hwndPage = hDlg;
			SetWindowLongPtr(hDlg, DWLP_USER, (LONG_PTR)lpcs);
			lpcs->m_bApply = FALSE;

			//Ver���
			SetDlgItemText(hDlg,IDC_STATIC_VER_INFO,SOFT_NAME);
			//�^�C�g��
			SetDlgItemText(hDlg,IDC_EDIT_FILENAME,getFileName(lpcs->m_strSelectFile));
			//�A�C�R����\��
			SHFILEINFO sfi;
			if(SHGetFileInfo(lpcs->m_strSelectFile,0,&sfi,sizeof(sfi),SHGFI_ICON))
			{
				SendMessage(GetDlgItem(hDlg,IDC_ICON1),
					STM_SETIMAGE,IMAGE_ICON,
					(LPARAM )sfi.hIcon);
			}

			//�I�[�i�[�h���[�{�^���̏�����
/*			RECT rect;
			SetRect(&rect,145,208,145,75);//�{�^���ʒu�̊
			MapDialogRect(hDlg,&rect);
			SetWindowPos(GetDlgItem(hDlg,IDC_BUTTON_REW),NULL,rect.left+25*0,rect.top,23,23,SWP_NOZORDER | SWP_SHOWWINDOW);
			SetWindowPos(GetDlgItem(hDlg,IDC_BUTTON_PLAY),NULL,rect.left+25*1,rect.top,23,23,SWP_NOZORDER | SWP_SHOWWINDOW);
			SetWindowPos(GetDlgItem(hDlg,IDC_BUTTON_PAUSE),NULL,rect.left+25*2,rect.top,23,23,SWP_NOZORDER | SWP_SHOWWINDOW);
			SetWindowPos(GetDlgItem(hDlg,IDC_BUTTON_STOP),NULL,rect.left+25*3,rect.top,23,23,SWP_NOZORDER | SWP_SHOWWINDOW);
			SetWindowPos(GetDlgItem(hDlg,IDC_BUTTON_FFW),NULL,rect.left+25*4,rect.top,23,23,SWP_NOZORDER | SWP_SHOWWINDOW);
*/			//����\��
			DispInfo(hDlg,lpcs);
			
		}
		break;

	case WM_TIMER:
		switch(wParam){
		case REW_TIMER:
			SendMessage(hDlg,WM_COMMAND,
				MAKEWPARAM((UINT )IDC_BUTTON_REW,(UINT )BN_CLICKED),
				0);
			break;
		case FFW_TIMER:
			SendMessage(hDlg,WM_COMMAND,
				MAKEWPARAM((UINT )IDC_BUTTON_FFW,(UINT )BN_CLICKED),
				0);
			break;
		}
		break;

	// �I�[�i�[�`��R���g���[��
	case WM_DRAWITEM:
		DrawControl(hDlg,(LPDRAWITEMSTRUCT )lParam);
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case IDC_EDIT_NAM:
		case IDC_EDIT_ART:
		case IDC_EDIT_FILE:
		case IDC_EDIT_COP:
		case IDC_EDIT_CMT:
			if((HIWORD(wParam) == EN_CHANGE) &&
				(IsWindowEnabled((HWND )lParam)) )
			{
				PropSheet_Changed(GetParent(hDlg),hDlg);
				lpcs->m_bApply = TRUE;
			}
			break;
		case IDC_BUTTON_PLAY:
			{
				PlayWinamp(hDlg,(char *)(LPCSTR )lpcs->m_strSelectFile);
				break;
			}
		case IDC_BUTTON_PAUSE:
			{
				PauseWinamp();
				break;
			}
		case IDC_BUTTON_STOP:
			{
				StopWinamp();
				break;
			}
		case IDC_BUTTON_REW:
			{
				Rew5Winamp();
				break;
			}
		case IDC_BUTTON_FFW:
			{
				Ffw5Winamp();
				break;
			}
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
			regSetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"PropAOT",(DWORD )lpcs->m_bPropAOT);
			break;
		case IDC_SETUP:
			ShellExecute(hDlg,"open","rundll32.exe","shell32.dll,Control_RunDLL mp3infp.cpl,,5",NULL,SW_SHOW);
			break;
		case IDC_HELPVIEW:
			lpcs->OpenHtmlHelp(hDlg,"extension.htm");
			break;
		DLG_CLIPBORD_MACRO(lpcs->m_strSelectFile);
		}
		break;
	// �R���e�L�X�g���j���[
	case WM_CONTEXTMENU:
		DlgContextMenu(hDlg,lParam,lpcs->m_Vqf.IsEnable());
		break;
	//�󋵈ˑ��w���v
	case WM_HELP:
	{
		char szTmp[256];
		strcpy(szTmp,APP_NAME);
		strcat(szTmp," ");
		strcat(szTmp,COPY_RIGHT);
		MessageBox(hDlg,szTmp,"About",MB_APPLMODAL);
		break;
	}
	case WM_NOTIFY:
		switch(((NMHDR FAR *)lParam)->code){
//		case PSN_SETACTIVE:
//			break;
		case PSN_APPLY:
			//�ۑ�
			if(lpcs->m_bApply && lpcs->m_Vqf.IsEnable())
			{
				//�t�@�C�����������݉\�����ׂ�
				if(GetFileAttributes(lpcs->m_strSelectFile) & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY))
				{
					//�u���̃t�@�C���́u�ǂݍ��ݐ�p�v�ł��v
					AfxMessageBox(IDS_PAGE_THIS_READONLY,MB_ICONINFORMATION);
					//�K�p�{�^���͈��������L��
					SetWindowLongPtr(hDlg,DWLP_MSGRESULT,PSNRET_INVALID);
					break;
				}
				lpcs->m_bApply = FALSE;

				CString strTmp;
				CWnd wnd;
				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_NAM));
				wnd.GetWindowText(strTmp);
				lpcs->m_Vqf.SetField('N','A','M','E',(const unsigned char *)(LPCSTR )strTmp,strTmp.GetLength());
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_ART));
				wnd.GetWindowText(strTmp);
				lpcs->m_Vqf.SetField('A','U','T','H',(const unsigned char *)(LPCSTR )strTmp,strTmp.GetLength());
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_FILE));
				wnd.GetWindowText(strTmp);
				lpcs->m_Vqf.SetField('F','I','L','E',(const unsigned char *)(LPCSTR )strTmp,strTmp.GetLength());
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_COP));
				wnd.GetWindowText(strTmp);
				lpcs->m_Vqf.SetField('(','c',')',' ',(const unsigned char *)(LPCSTR )strTmp,strTmp.GetLength());
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_CMT));
				wnd.GetWindowText(strTmp);
				lpcs->m_Vqf.SetField('C','O','M','T',(const unsigned char *)(LPCSTR )strTmp,strTmp.GetLength());
				wnd.Detach();

				//�^�C���X�^���v��ۑ�
				lpcs->PushTimeStamp(lpcs->m_strSelectFile);

				DWORD dwRet = lpcs->m_Vqf.Save(hDlg,lpcs->m_strSelectFile);
				if(dwRet != ERROR_SUCCESS)
				{
					lpcs->m_bApply = TRUE;
					if(dwRet == -1)
						//�u�t�@�C���𐳂����X�V�ł��܂���ł����B�v
						AfxMessageBox(IDS_PAGE_BROKEN,MB_ICONINFORMATION);
					else
						//�V�X�e���G���[��\��
						errMessageBox(hDlg,dwRet);
					//�K�p�{�^���͈��������L��
					SetWindowLongPtr(hDlg,DWLP_MSGRESULT,PSNRET_INVALID);
					break;
				}

				//�^�C���X�^���v�𕜌�
				if(lpcs->m_bSaveTimeStamp)
				{
					lpcs->PopTimeStamp(lpcs->m_strSelectFile);
				}

				//����\��
				DispInfo(hDlg,lpcs);

				SetWindowLongPtr(hDlg,DWLP_MSGRESULT,PSNRET_NOERROR);
				
				//�V�F���ɕύX��ʒm
				SHChangeNotify(SHCNE_UPDATEITEM,SHCNF_PATH,lpcs->m_strSelectFile,NULL);
			}
			break;
		default:
			break;
		}
		break;

	default:
		return FALSE;
	
	}

	return TRUE;
}