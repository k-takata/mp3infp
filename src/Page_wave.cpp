#include "StdAfx.h"
#include "mp3infp.h"

static int ids[] =
{
	-1,
	-1,
	CLP_NAM,
	CLP_ART,
	CLP_PRD,
	CLP_CRD,
	CLP_GNR,
	CLP_CMT,
	CLP_COP,
	CLP_ENG,
	CLP_SRC,
	CLP_SFT,
	0
};

static int staticWnd[] =
{
	IDC_STATIC_FORMAT_,
	IDC_STATIC_TIME_,
	IDC_STATIC_NAM,
	IDC_STATIC_ART,
	IDC_STATIC_PRD,
	IDC_STATIC_CRD,
	IDC_STATIC_GNR,
	IDC_STATIC_CMT,
	IDC_STATIC_COP,
	IDC_STATIC_ENG,
	IDC_STATIC_SRC,
	IDC_STATIC_SFT,
	0
};

static int editWnd[] =
{
	IDC_STATIC_FORMAT,
	IDC_STATIC_TIME,
	IDC_EDIT_SBJ,
	IDC_EDIT_ART,
	IDC_EDIT_PRD,
	IDC_EDIT_CRD,
	IDC_EDIT_GNR,
	IDC_EDIT_CMT,
	IDC_EDIT_COP,
	IDC_EDIT_ENG,
	IDC_EDIT_SRC,
	IDC_EDIT_SFT,
	0
};

UINT CALLBACK CShellExt::PageCallback_wave(HWND hWnd,UINT uMessage,LPPROPSHEETPAGE ppsp)
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
	SetWindowText(GetDlgItem(hDlg,IDC_STATIC_FORMAT),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_FORMAT),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_TIME_),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_STATIC_TIME),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_TIME),bEnable);

	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_NAM),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_SBJ),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_SBJ),bEnable);

	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_ART),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_ART),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_ART),bEnable);

	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_PRD),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_PRD),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_PRD),bEnable);

	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_CRD),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_CRD),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_CRD),bEnable);

	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_GNR),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_GNR),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_GNR),bEnable);

	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_CMT),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_CMT),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_CMT),bEnable);

	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_COP),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_COP),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_COP),bEnable);

	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_ENG),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_ENG),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_ENG),bEnable);

	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_SRC),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_SRC),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_SRC),bEnable);

	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_SFT),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_SFT),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_SFT),bEnable);

	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_WINAMP),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_BUTTON_REW),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_BUTTON_PLAY),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_BUTTON_PAUSE),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_BUTTON_STOP),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_BUTTON_FFW),bEnable);
}

static void DispInfo(HWND hDlg,CShellExt *lpcs)
{
	if(lpcs->m_RiffSIF.IsEnable())
	{
		EnableEdit(hDlg,lpcs,TRUE);

		//wave
		CString strFormat;
		CString strTime;
		GetWaveAudioFormat((LPCTSTR )lpcs->m_strSelectFile,
						lpcs->m_RiffSIF.GetStreamSize(),
						strFormat,
						strTime,
						lpcs->m_iWaveCodecFind);
		SetDlgItemText(hDlg,IDC_STATIC_FORMAT,strFormat);
		SetDlgItemText(hDlg,IDC_STATIC_TIME,strTime);

		//ISBJ songname
//2002-01-30
//INAM��D��A�������ISBJ��\��
//		SetDlgItemText(hDlg,IDC_EDIT_SBJ,lpcs->m_RiffSIF.GetField('I','S','B','J'));
		CString tmp;
		tmp = lpcs->m_RiffSIF.GetField('I','N','A','M');
		if(tmp.GetLength() == 0)
		{
			tmp = lpcs->m_RiffSIF.GetField('I','S','B','J');
		}
		SetDlgItemText(hDlg,IDC_EDIT_SBJ,tmp);
		//IART �A�[�e�B�X�g��
		SetDlgItemText(hDlg,IDC_EDIT_ART,lpcs->m_RiffSIF.GetField('I','A','R','T'));
		//IPRD �A���o����
		SetDlgItemText(hDlg,IDC_EDIT_PRD,lpcs->m_RiffSIF.GetField('I','P','R','D'));
		//ICMT �R�����g
		SetDlgItemText(hDlg,IDC_EDIT_CMT,lpcs->m_RiffSIF.GetField('I','C','M','T'));
		//ICRD ���t
		SetDlgItemText(hDlg,IDC_EDIT_CRD,lpcs->m_RiffSIF.GetField('I','C','R','D'));
		//IGNR �W������
		SetDlgItemText(hDlg,IDC_EDIT_GNR,lpcs->m_RiffSIF.GetField('I','G','N','R'));
		//ICOP ���쌠
		SetDlgItemText(hDlg,IDC_EDIT_COP,lpcs->m_RiffSIF.GetField('I','C','O','P'));
		//IENG �G���W�j�A	
		SetDlgItemText(hDlg,IDC_EDIT_ENG,lpcs->m_RiffSIF.GetField('I','E','N','G'));
		//ISRC �\�[�X	
		SetDlgItemText(hDlg,IDC_EDIT_SRC,lpcs->m_RiffSIF.GetField('I','S','R','C'));
		//ISFT �\�t�g�E�F�A
		SetDlgItemText(hDlg,IDC_EDIT_SFT,lpcs->m_RiffSIF.GetField('I','S','F','T'));

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

BOOL CALLBACK CShellExt::PageDlgProc_wave(HWND hDlg,UINT uMessage,WPARAM wParam,LPARAM lParam)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

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

			//�R���{�{�b�N�X�̏�����
			SendMessage(
					GetDlgItem(hDlg,IDC_EDIT_GNR),
					CB_ADDSTRING,
					0,
					(LPARAM )(LPCTSTR )_T("")	//��
				);
			for(int i=0; i<256; i++)
			{
				if(lpcs->m_Id3tagv1.GenreNum2String(i).GetLength())
					SendMessage(
							GetDlgItem(hDlg,IDC_EDIT_GNR),
							CB_ADDSTRING,
							0,
							(LPARAM )(LPCTSTR )lpcs->m_Id3tagv1.GenreNum2String(i)
						);
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

			//�u���̃t�@�C����WAV�`���ł��BID3�^�O�͏������߂܂���B\n(������������̂�RiffSIF�݂̂ł�)�v
			if((lstrcmpi(getExtName(lpcs->m_strSelectFile),_T(".mp3")) == 0) ||
				(lstrcmpi(getExtName(lpcs->m_strSelectFile),_T(".mp2")) == 0) ||
				(lstrcmpi(getExtName(lpcs->m_strSelectFile),_T(".mp1")) == 0) ||
				(lstrcmpi(getExtName(lpcs->m_strSelectFile),_T(".rmp")) == 0) )
			{
				ShowWindow(GetDlgItem(hDlg,IDC_STATIC_NOT_MP3),SW_SHOWNORMAL);
			}
			else
			{
				ShowWindow(GetDlgItem(hDlg,IDC_STATIC_NOT_MP3),SW_HIDE);
			}

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
		case IDC_EDIT_CRD:
		case IDC_EDIT_SBJ:
		case IDC_EDIT_ART:
		case IDC_EDIT_PRD:
		case IDC_EDIT_CMT:
		case IDC_EDIT_COP:
		case IDC_EDIT_ENG:
		case IDC_EDIT_SRC:
		case IDC_EDIT_SFT:
			if((HIWORD(wParam) == EN_CHANGE) &&
				(IsWindowEnabled((HWND )lParam)) )
			{
				PropSheet_Changed(GetParent(hDlg),hDlg);
				lpcs->m_bApply = TRUE;
			}
			break;
		case IDC_EDIT_GNR:
			if( ((HIWORD(wParam) == CBN_SELENDOK) || (HIWORD(wParam) == CBN_EDITCHANGE)) &&
				(IsWindowEnabled((HWND )lParam)) )
			{
				PropSheet_Changed(GetParent(hDlg),hDlg);
				lpcs->m_bApply = TRUE;
			}
			break;
		case IDC_BUTTON_PLAY:
			{
				PlayWinamp(hDlg,(LPCTSTR )lpcs->m_strSelectFile);
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
			regSetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("PropAOT"),(DWORD )lpcs->m_bPropAOT);
			break;
		case IDC_SETUP:
			ShellExecute(hDlg,_T("open"),_T("rundll32.exe"),_T("shell32.dll,Control_RunDLL mp3infp.cpl,,3"),NULL,SW_SHOW);
			break;
		case IDC_HELPVIEW:
			lpcs->OpenHtmlHelp(hDlg,_T("extension.htm"));
			break;
		DLG_CLIPBORD_MACRO(lpcs->m_strSelectFile);
		}
		break;
	// �R���e�L�X�g���j���[
	case WM_CONTEXTMENU:
		DlgContextMenu(hDlg,lParam,lpcs->m_RiffSIF.IsEnable());
		break;
	//�󋵈ˑ��w���v
	case WM_HELP:
	{
		TCHAR szTmp[256];
		lstrcpy(szTmp,APP_NAME);
		lstrcat(szTmp,_T(" "));
		lstrcat(szTmp,COPY_RIGHT);
		MessageBox(hDlg,szTmp,_T("About"),MB_APPLMODAL);
		break;
	}
	case WM_NOTIFY:
		switch(((NMHDR FAR *)lParam)->code){
//		case PSN_SETACTIVE:
//			break;
		case PSN_APPLY:
			//�ۑ�
			if(lpcs->m_bApply && lpcs->m_RiffSIF.IsEnable())
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
				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_SBJ));
				wnd.GetWindowText(strTmp);

//2002-01-30
//INAM��D��AISBJ�͓P��
				lpcs->m_RiffSIF.SetField('I','S','B','J',_T(""));
				lpcs->m_RiffSIF.SetField('I','N','A','M',strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_ART));
				wnd.GetWindowText(strTmp);
				lpcs->m_RiffSIF.SetField('I','A','R','T',strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_PRD));
				wnd.GetWindowText(strTmp);
				lpcs->m_RiffSIF.SetField('I','P','R','D',strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_CRD));
				wnd.GetWindowText(strTmp);
				lpcs->m_RiffSIF.SetField('I','C','R','D',strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_GNR));
				wnd.GetWindowText(strTmp);
				lpcs->m_RiffSIF.SetField('I','G','N','R',strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_CMT));
				wnd.GetWindowText(strTmp);
				lpcs->m_RiffSIF.SetField('I','C','M','T',strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_COP));
				wnd.GetWindowText(strTmp);
				lpcs->m_RiffSIF.SetField('I','C','O','P',strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_ENG));
				wnd.GetWindowText(strTmp);
				lpcs->m_RiffSIF.SetField('I','E','N','G',strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_SRC));
				wnd.GetWindowText(strTmp);
				lpcs->m_RiffSIF.SetField('I','S','R','C',strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_SFT));
				wnd.GetWindowText(strTmp);
				lpcs->m_RiffSIF.SetField('I','S','F','T',strTmp);
				wnd.Detach();

				//�^�C���X�^���v��ۑ�
				lpcs->PushTimeStamp(lpcs->m_strSelectFile);

				DWORD dwRet = lpcs->m_RiffSIF.Save(hDlg,lpcs->m_strSelectFile);
				if(dwRet != ERROR_SUCCESS)
				{
					lpcs->m_bApply = TRUE;
					if(dwRet == -1)
						//�u�t�@�C���𐳂����X�V�ł��܂���ł����B�v
						AfxMessageBox(IDS_PAGE_BROKEN,MB_ICONINFORMATION);
					else if(dwRet == -2)
						//�u2G�o�C�g�𒴂���t�@�C�����������Ƃ͂ł��܂���B�v
						AfxMessageBox(IDS_OVER_2G,MB_ICONINFORMATION);
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