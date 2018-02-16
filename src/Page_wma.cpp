#include "StdAfx.h"
#include "mp3infp.h"

static const int ids[] =
{
	-1,
	-1,
	-1,
	CLP_NAM,
	CLP_TRACK,
	CLP_ART,
	CLP_PRD,
	CLP_CMT,
	CLP_CRD,
	CLP_GNR,
	CLP_COP,
	CLP_URL1,
	CLP_URL2,
	0
};

static const int staticWnd[] =
{
	IDC_STATIC_FORMAT_A_,
	IDC_STATIC_FORMAT_V_,
	IDC_STATIC_TIME_,
	IDC_STATIC_NAM,
	IDC_STATIC_TRK,
	IDC_STATIC_ART,
	IDC_STATIC_PRD,
	IDC_STATIC_CMT,
	IDC_STATIC_CRD,
	IDC_STATIC_GNR,
	IDC_STATIC_COP,
	IDC_STATIC_URL1,
	IDC_STATIC_URL2,
	0
};

static const int editWnd[] =
{
	IDC_STATIC_FORMAT_A,
	IDC_STATIC_FORMAT_V,
	IDC_STATIC_TIME,
	IDC_EDIT_NAM,
	IDC_EDIT_TRK,
	IDC_EDIT_ART,
	IDC_EDIT_PRD,
	IDC_EDIT_CMT,
	IDC_EDIT_CRD,
	IDC_EDIT_GNR,
	IDC_EDIT_COP,
	IDC_EDIT_URL1,
	IDC_EDIT_URL2,
	0
};

UINT CALLBACK CShellExt::PageCallback_wma(HWND hWnd,UINT uMessage,LPPROPSHEETPAGE ppsp)
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
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_FORMAT_A_),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_FORMAT_A),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_STATIC_FORMAT_A),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_FORMAT_V_),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_FORMAT_V),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_STATIC_FORMAT_V),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_TIME_),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_TIME),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_STATIC_TIME),_T(""));

	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_NAM),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_NAM),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_NAM),bEnable);

	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_TRK),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_TRK),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_TRK),bEnable);

	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_ART),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_ART),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_ART),bEnable);

	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_PRD),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_PRD),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_PRD),bEnable);

	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_CMT),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_CMT),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_CMT),bEnable);

	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_CRD),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_CRD),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_CRD),bEnable);

	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_GNR),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_GNR),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_GNR),bEnable);

	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_COP),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_COP),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_COP),bEnable);

	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_URL1),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_URL1),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_URL1),bEnable);

	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_URL2),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_URL2),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_URL2),bEnable);

	//Seekable
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_SEEKABLE),bEnable);
	//Broadcast
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_BROADCAST),bEnable);
	//IsProtected
//	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_IS_PROTECTED),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_IS_PROTECTED),FALSE);
}

static void DispInfo(HWND hDlg,CShellExt *lpcs)
{
	if(lpcs->m_Wma.IsEnable())
	{
		EnableEdit(hDlg,lpcs,TRUE);

		//�����`��
		SetDlgItemText(hDlg,IDC_STATIC_FORMAT_A,lpcs->m_Wma.GetAudioFormatString());
		//�f���`��
		SetDlgItemText(hDlg,IDC_STATIC_FORMAT_V,lpcs->m_Wma.GetVideoFormatString());
		//����
		SetDlgItemText(hDlg,IDC_STATIC_TIME,lpcs->m_Wma.GetTimeString());

		//�^�C�g��
		SetDlgItemText(hDlg,IDC_EDIT_NAM,lpcs->m_Wma.GetNAM());
		//�g���b�N
		SetDlgItemText(hDlg,IDC_EDIT_TRK,lpcs->m_Wma.GetTRACK());
		//�A�[�e�B�X�g
		SetDlgItemText(hDlg,IDC_EDIT_ART,lpcs->m_Wma.GetART());
		//�A���o��
		SetDlgItemText(hDlg,IDC_EDIT_PRD,lpcs->m_Wma.GetPRD());
		//����
		SetDlgItemText(hDlg,IDC_EDIT_CMT,lpcs->m_Wma.GetCMT());
		//�쐬��
		SetDlgItemText(hDlg,IDC_EDIT_CRD,lpcs->m_Wma.GetCRD());
		//�W������
		SetDlgItemText(hDlg,IDC_EDIT_GNR,lpcs->m_Wma.GetGNR());
		//���쌠
		SetDlgItemText(hDlg,IDC_EDIT_COP,lpcs->m_Wma.GetCOPY());
		//URL(album)
		SetDlgItemText(hDlg,IDC_EDIT_URL1,lpcs->m_Wma.GetUrl1());
		//URL(�֘A)	
		SetDlgItemText(hDlg,IDC_EDIT_URL2,lpcs->m_Wma.GetUrl2());

		//Seekable
		EnableWindow(GetDlgItem(hDlg,IDC_STATIC_SEEKABLE),lpcs->m_Wma.GetSeekable());
		//Broadcast
		EnableWindow(GetDlgItem(hDlg,IDC_STATIC_BROADCAST),lpcs->m_Wma.GetBroadcast());
		//IsProtected
		CString strDrm;
		int drmVer = lpcs->m_Wma.GetDrmVer();
		if(drmVer)
		{
			// �o�[�W�����ԍ��̔���͕s���S
//			strDrm.Format(_T("DRM v%d"),drmVer);
			strDrm.Format(_T("DRM"));
			SetWindowText(GetDlgItem(hDlg,IDC_STATIC_IS_PROTECTED),strDrm);
		}
		EnableWindow(GetDlgItem(hDlg,IDC_STATIC_IS_PROTECTED),(drmVer?1:0));

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

BOOL CALLBACK CShellExt::PageDlgProc_wma(HWND hDlg,UINT uMessage,WPARAM wParam,LPARAM lParam)
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
				Static_SetImage_Icon(GetDlgItem(hDlg,IDC_ICON1),sfi.hIcon);
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
		case IDC_EDIT_TRK:
		case IDC_EDIT_ART:
		case IDC_EDIT_PRD:
		case IDC_EDIT_CMT:
		case IDC_EDIT_CRD:
		case IDC_EDIT_GNR:
		case IDC_EDIT_COP:
		case IDC_EDIT_URL1:
		case IDC_EDIT_URL2:
			if((HIWORD(wParam) == EN_CHANGE) &&
				(IsWindowEnabled((HWND )lParam)) )
			{
				PropSheet_Changed(GetParent(hDlg),hDlg);
				lpcs->m_bApply = TRUE;
			}
			break;
		case IDC_BUTTON_PLAY:
			{
				PlayWinamp(hDlg,lpcs->m_strSelectFile);
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
			OpenConfigPage(hDlg,6);
			break;
		case IDC_HELPVIEW:
			lpcs->OpenHtmlHelp(hDlg,_T("extension.htm"));
			break;
		DLG_CLIPBORD_MACRO(lpcs->m_strSelectFile);
		}
		break;
	// �R���e�L�X�g���j���[
	case WM_CONTEXTMENU:
		DlgContextMenu(hDlg,lParam,lpcs->m_Wma.IsEnable());
		break;
	//�󋵈ˑ��w���v
	case WM_HELP:
		OpenAboutDlg(hDlg);
		break;

	case WM_NOTIFY:
		switch(((NMHDR FAR *)lParam)->code){
//		case PSN_SETACTIVE:
//			break;
		case PSN_APPLY:
			//�ۑ�
			if(lpcs->m_bApply && lpcs->m_Wma.IsEnable())
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

				//�^�C�g��
				CString strTmp;
				CWnd wnd;
				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_NAM));
				wnd.GetWindowText(strTmp);
				lpcs->m_Wma.SetNAM(strTmp);
				wnd.Detach();
				//�g���b�N
				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_TRK));
				wnd.GetWindowText(strTmp);
				lpcs->m_Wma.SetTRACK(strTmp);
				wnd.Detach();
				//�A�[�e�B�X�g
				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_ART));
				wnd.GetWindowText(strTmp);
				lpcs->m_Wma.SetART(strTmp);
				wnd.Detach();
				//�A���o��
				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_PRD));
				wnd.GetWindowText(strTmp);
				lpcs->m_Wma.SetPRD(strTmp);
				wnd.Detach();
				//����
				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_CMT));
				wnd.GetWindowText(strTmp);
				lpcs->m_Wma.SetCMT(strTmp);
				wnd.Detach();
				//�쐬��
				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_CRD));
				wnd.GetWindowText(strTmp);
				lpcs->m_Wma.SetCRD(strTmp);
				wnd.Detach();
				//�W������
				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_GNR));
				wnd.GetWindowText(strTmp);
				lpcs->m_Wma.SetGNR(strTmp);
				wnd.Detach();
				//���쌠
				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_COP));
				wnd.GetWindowText(strTmp);
				lpcs->m_Wma.SetCOPY(strTmp);
				wnd.Detach();
				//URL(album)
				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_URL1));
				wnd.GetWindowText(strTmp);
				lpcs->m_Wma.SetUrl1(strTmp);
				wnd.Detach();
				//URL(�֘A)	
				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_URL2));
				wnd.GetWindowText(strTmp);
				lpcs->m_Wma.SetUrl2(strTmp);
				wnd.Detach();

				//�^�C���X�^���v��ۑ�
				lpcs->PushTimeStamp(lpcs->m_strSelectFile);

				DWORD dwRet = lpcs->m_Wma.Save(lpcs->m_strSelectFile);
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
