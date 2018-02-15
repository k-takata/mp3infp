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
	IDC_EDIT_NAM,
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

UINT CALLBACK CShellExt::PageCallback_mp3_RMP(HWND hWnd, UINT uMessage, LPPROPSHEETPAGE ppsp)
{
	LPCSHELLEXT	lpcs = (CShellExt *)ppsp->lParam;
	switch(uMessage){
	case PSPCB_CREATE:	//�v���p�e�B�y�[�W���쐬�������
		return TRUE;
	case PSPCB_RELEASE:	//�v���p�e�B�y�[�W���j�󂳂����
		if(lpcs) 
		{
			lpcs->m_hwndRiffSIF = NULL;
			lpcs->Release();
		}
		return TRUE;
	}

	return TRUE;
}

static void EnableButton(HWND hDlg,CShellExt *lpcs)
{
	if(lpcs->m_Id3tagv1.IsEnable() && lpcs->m_Rmp3.IsEnable())
		EnableWindow(GetDlgItem(hDlg,IDC_COPY_FROM_ID3V1),TRUE);	//Copy from ID3v1
	else
		EnableWindow(GetDlgItem(hDlg,IDC_COPY_FROM_ID3V1),FALSE);	//Copy from ID3v1

	if(lpcs->m_Id3tagv2.IsEnable())
		EnableWindow(GetDlgItem(hDlg,IDC_COPY_FROM_ID3V2),TRUE);	//Copy from ID3v2
	else
		EnableWindow(GetDlgItem(hDlg,IDC_COPY_FROM_ID3V2),FALSE);	//Copy from ID3v2
}

static void EnableEdit(HWND hDlg,CShellExt *lpcs,BOOL bEnable)
{
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_NAM),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_NAM),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_NAM),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_ART),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_ART),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_ART),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_PRD),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_PRD),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_PRD),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_CRD),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_CRD),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_CRD),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_GNR),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_GNR),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_GNR),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_CMT),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_CMT),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_CMT),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_COP),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_COP),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_COP),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_ENG),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_ENG),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_ENG),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_SRC),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_SRC),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_SRC),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_SFT),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_SFT),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_SFT),bEnable);
	if(bEnable)
	{
		EnableWindow(GetDlgItem(hDlg,IDC_BUTTON_DEL_TAG),TRUE);	//Make Riff SIF
		EnableWindow(GetDlgItem(hDlg,IDC_BUTTON_MAKE_TAG),FALSE);	//Make Riff SIF
		HWND hWndTab = PropSheet_GetTabControl(GetParent(hDlg));
		TC_ITEM tcItem;
		tcItem.mask = TCIF_TEXT;
		tcItem.pszText = _T("Riff SIF");
		TabCtrl_SetItem(hWndTab,PropSheet_PageToIndex(GetParent(hDlg),lpcs->m_hpageRiffSIF),&tcItem);
		PropSheet_RecalcPageSizes(GetParent(hDlg));
	}
	else
	{
		EnableWindow(GetDlgItem(hDlg,IDC_BUTTON_DEL_TAG),FALSE);	//Del Riff SIF
		EnableWindow(GetDlgItem(hDlg,IDC_BUTTON_MAKE_TAG),TRUE);	//Make Riff SIF
		HWND hWndTab = PropSheet_GetTabControl(GetParent(hDlg));
		TC_ITEM tcItem;
		tcItem.mask = TCIF_TEXT;
		tcItem.pszText = _T("Riff SIF(*)");
		TabCtrl_SetItem(hWndTab,PropSheet_PageToIndex(GetParent(hDlg),lpcs->m_hpageRiffSIF),&tcItem);
		PropSheet_RecalcPageSizes(GetParent(hDlg));
	}

	EnableButton(hDlg,lpcs);
}

static void DispInfo(HWND hDlg,CShellExt *lpcs)
{
	//mpeg���
	SetDlgItemText(hDlg,IDC_STATIC_FORMAT,lpcs->m_mp3info.GetFormatString());
	SetDlgItemText(hDlg,IDC_STATIC_TIME,lpcs->m_mp3info.GetTimeString());

	if(lpcs->m_Rmp3.IsEnable())
	{
		EnableEdit(hDlg,lpcs,TRUE);

		//INAM songname
		SetDlgItemText(hDlg,IDC_EDIT_NAM,lpcs->m_Rmp3.GetNAM());
		//IART �A�[�e�B�X�g��
		SetDlgItemText(hDlg,IDC_EDIT_ART,lpcs->m_Rmp3.GetART());
		//IPRD �A���o����
		SetDlgItemText(hDlg,IDC_EDIT_PRD,lpcs->m_Rmp3.GetPRD());
		//ICMT �R�����g
		SetDlgItemText(hDlg,IDC_EDIT_CMT,lpcs->m_Rmp3.GetCMT());
		//ICRD ���t
		SetDlgItemText(hDlg,IDC_EDIT_CRD,lpcs->m_Rmp3.GetCRD());
		//IGNR �W������
		SetDlgItemText(hDlg,IDC_EDIT_GNR,lpcs->m_Rmp3.GetGNR());
		//ICOP ���쌠
		SetDlgItemText(hDlg,IDC_EDIT_COP,lpcs->m_Rmp3.GetCOP());
		//IENG �G���W�j�A	
		SetDlgItemText(hDlg,IDC_EDIT_ENG,lpcs->m_Rmp3.GetENG());
		//ISRC �\�[�X	
		SetDlgItemText(hDlg,IDC_EDIT_SRC,lpcs->m_Rmp3.GetSRC());
		//ISFT �\�t�g�E�F�A
		SetDlgItemText(hDlg,IDC_EDIT_SFT,lpcs->m_Rmp3.GetSFT());

		lpcs->m_bRiffApply = FALSE;
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

BOOL CALLBACK CShellExt::PageDlgProc_mp3_RMP(HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	LPCSHELLEXT	lpcs = (CShellExt *)GetWindowLongPtr(hDlg,DWLP_USER);
	CString strTmp;

	switch(uMessage){
	case WM_USER+1:	//�\���̃��t���b�V��
		EnableButton(hDlg,lpcs);
		DispInfo(hDlg,lpcs);
		lpcs->m_bRiffApply = FALSE;
		break;
	case WM_INITDIALOG:
		{
			lpcs = (LPCSHELLEXT )((LPPROPSHEETPAGE )lParam)->lParam;
			lpcs->m_hwndRiffSIF = hDlg;
			SetWindowLongPtr(hDlg, DWLP_USER, (LONG_PTR)lpcs);
			lpcs->m_bRiffApply = FALSE;

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

			//�R���{�{�b�N�X�̏�����
			ComboBox_AddString(GetDlgItem(hDlg,IDC_EDIT_GNR), _T(""));
			for(int i=0; i<256; i++)
			{
				if(lpcs->m_Id3tagv1.GenreNum2String(i).GetLength())
					ComboBox_AddString(GetDlgItem(hDlg,IDC_EDIT_GNR),
							lpcs->m_Id3tagv1.GenreNum2String(i));
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
			//����\��
*/			DispInfo(hDlg,lpcs);
			lpcs->m_bRiffApply = FALSE;
		
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
		case IDC_EDIT_NAM:
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
				lpcs->m_bRiffApply = TRUE;
			}
			break;
		case IDC_EDIT_GNR:
			if( ((HIWORD(wParam) == CBN_SELENDOK) || (HIWORD(wParam) == CBN_EDITCHANGE)) &&
				(IsWindowEnabled((HWND )lParam)) )
			{
				PropSheet_Changed(GetParent(hDlg),hDlg);
				lpcs->m_bRiffApply = TRUE;
			}
			break;
		case IDC_BUTTON_DEL_TAG:
			//�uMP3�`���ɕϊ����܂��B��낵���ł����H�v
			strTmp.Format(IDS_PAGE_MP3RIFF_CONV_MP3);
			if(MessageBox(hDlg,strTmp,APP_NAME,MB_APPLMODAL | MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES)
			{
				//�t�@�C�����������݉\�����ׂ�
				if(GetFileAttributes(lpcs->m_strSelectFile) & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY))
				{
					//�u���̃t�@�C���́u�ǂݍ��ݐ�p�v�ł��v
					AfxMessageBox(IDS_PAGE_THIS_READONLY,MB_ICONINFORMATION);
					break;
				}
				//�^�C���X�^���v��ۑ�
				lpcs->PushTimeStamp(lpcs->m_strSelectFile);
				//Riff SIF������
				DWORD dwRet = lpcs->m_Rmp3.DelTag(hDlg,lpcs->m_strSelectFile);
				if(dwRet == -1)
				{
					//�u�t�@�C���𐳂����X�V�ł��܂���ł����B�v
					AfxMessageBox(IDS_PAGE_BROKEN,MB_ICONINFORMATION);
					break;
				}
				else if(dwRet != ERROR_SUCCESS)
				{
					//�V�X�e���G���[��\��
					errMessageBox(hDlg,dwRet);
					break;
				}
				//�^�C���X�^���v�𕜌�
				if(lpcs->m_bSaveTimeStamp)
				{
					lpcs->PopTimeStamp(lpcs->m_strSelectFile);
				}
			}
			//����\��
			DispInfo(hDlg,lpcs);
			lpcs->m_bRiffApply = FALSE;
	
			//�V�F���ɕύX��ʒm
			SHChangeNotify(SHCNE_UPDATEITEM,SHCNF_PATH,lpcs->m_strSelectFile,NULL);
			//���̌`���^�u���X�V
			if(lpcs->m_hwndId3v1)
				PostMessage(lpcs->m_hwndId3v1,WM_USER+1,0,0);
			if(lpcs->m_hwndId3v2)
				PostMessage(lpcs->m_hwndId3v2,WM_USER+1,0,0);
			if(lpcs->m_hwndMp3Ape)
				PostMessage(lpcs->m_hwndMp3Ape,WM_USER+1,0,0);
			break;
		case IDC_BUTTON_MAKE_TAG:
			if(lpcs->m_Id3tagv2.IsEnable())
			{
				//�uID3v2 TAG�����邽�߁ARMP�`���ɕϊ��ł��܂���B�v
				strTmp.Format(IDS_PAGE_MP3RIFF_CAN_NOT_MAKE);
				MessageBox(hDlg,strTmp,APP_NAME,MB_ICONINFORMATION);
			}
			else if(lpcs->m_Ape.IsEnable())
			{
				//�uAPE Tag�����邽�߁ARMP�`���ɕϊ��ł��܂���B�v
				strTmp.Format(IDS_PAGE_MP3RIFF_CAN_NOT_MAKE_APE);
				MessageBox(hDlg,strTmp,APP_NAME,MB_ICONINFORMATION);
			}
			else
			{
				//�uRMP�`���ɕϊ����܂��B��낵���ł����H�v
				strTmp.Format(IDS_PAGE_MP3RIFF_CONV_RMP);
				if(MessageBox(hDlg,strTmp,APP_NAME,MB_APPLMODAL | MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES)
				{
					//�t�@�C�����������݉\�����ׂ�
					if(GetFileAttributes(lpcs->m_strSelectFile) & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY))
					{
						//�u���̃t�@�C���́u�ǂݍ��ݐ�p�v�ł��v
						AfxMessageBox(IDS_PAGE_THIS_READONLY,MB_ICONINFORMATION);
						break;
					}
					//�^�C���X�^���v��ۑ�
					lpcs->PushTimeStamp(lpcs->m_strSelectFile);
					//Riff SIF���쐬
					DWORD dwRet = lpcs->m_Rmp3.MakeTag(hDlg,lpcs->m_strSelectFile);
					if(dwRet == -1)
					{
						//�u�t�@�C���𐳂����X�V�ł��܂���ł����B�v
						AfxMessageBox(IDS_PAGE_BROKEN,MB_ICONINFORMATION);
						break;
					}
					else if(dwRet != ERROR_SUCCESS)
					{
						//�V�X�e���G���[��\��
						errMessageBox(hDlg,dwRet);
						break;
					}
					lpcs->m_Id3tagv1.Load(lpcs->m_strSelectFile);
					lpcs->m_Id3tagv2.Load(lpcs->m_strSelectFile);

					//�^�C���X�^���v�𕜌�
					if(lpcs->m_bSaveTimeStamp)
					{
						lpcs->PopTimeStamp(lpcs->m_strSelectFile);
					}
				}
				//����\��
				DispInfo(hDlg,lpcs);
				lpcs->m_bRiffApply = FALSE;
				
				//�V�F���ɕύX��ʒm
				SHChangeNotify(SHCNE_UPDATEITEM,SHCNF_PATH,lpcs->m_strSelectFile,NULL);
				//���̌`���^�u���X�V
				if(lpcs->m_hwndId3v1)
					PostMessage(lpcs->m_hwndId3v1,WM_USER+1,0,0);
				if(lpcs->m_hwndId3v2)
					PostMessage(lpcs->m_hwndId3v2,WM_USER+1,0,0);
				if(lpcs->m_hwndMp3Ape)
					PostMessage(lpcs->m_hwndMp3Ape,WM_USER+1,0,0);
			}
			break;
		case IDC_COPY_FROM_ID3V1:
			//INAM songname
			SetDlgItemText(hDlg,IDC_EDIT_NAM,lpcs->m_Id3tagv1.GetTitle());
			//IART �A�[�e�B�X�g��
			SetDlgItemText(hDlg,IDC_EDIT_ART,lpcs->m_Id3tagv1.GetArtist());
			//IPRD �A���o����
			SetDlgItemText(hDlg,IDC_EDIT_PRD,lpcs->m_Id3tagv1.GetAlbum());
			//ICMT �R�����g
			SetDlgItemText(hDlg,IDC_EDIT_CMT,lpcs->m_Id3tagv1.GetComment());
			//ICRD ���t
			SetDlgItemText(hDlg,IDC_EDIT_CRD,lpcs->m_Id3tagv1.GetYear());
			//IGNR �W������
			SetDlgItemText(hDlg,IDC_EDIT_GNR,lpcs->m_Id3tagv1.GetGenre());
			break;
		case IDC_COPY_FROM_ID3V2:
			//�uID3v2 TAG��RiffSIF(RMP�`��)�͋����ł��܂���B\nID3v2�̓��e���R�s�[�������ID3v2 TAG����菜���܂����H�v
			strTmp.Format(IDS_PAGE_MP3RIFF_CAN_NOT_MAKE2);
			if(MessageBox(hDlg,strTmp,APP_NAME,MB_ICONINFORMATION|MB_YESNO) == IDYES)
			{
				//�t�@�C�����������݉\�����ׂ�
				if(GetFileAttributes(lpcs->m_strSelectFile) & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY))
				{
					//�u���̃t�@�C���́u�ǂݍ��ݐ�p�v�ł��v
					AfxMessageBox(IDS_PAGE_THIS_READONLY,MB_ICONINFORMATION);
					break;
				}
				CString strTitle	= lpcs->m_Id3tagv2.GetTitle();
				CString strArtist	= lpcs->m_Id3tagv2.GetArtist();
				CString strAlbum	= lpcs->m_Id3tagv2.GetAlbum();
				CString strYear		= lpcs->m_Id3tagv2.GetYear();
				CString strGenre	= lpcs->m_Id3tagv2.GetGenre();
				CString strComment	= lpcs->m_Id3tagv2.GetComment();
//				CString strComposer	= lpcs->m_Id3tagv2.GetComposer();
//				CString strOrigArtist= lpcs->m_Id3tagv2.GetOrigArtist();
				CString strCopyright= lpcs->m_Id3tagv2.GetCopyright();
//				CString strUrl		= lpcs->m_Id3tagv2.GetUrl();
				CString strEncoder	= lpcs->m_Id3tagv2.GetEncoder();

				//�^�C���X�^���v��ۑ�
				lpcs->PushTimeStamp(lpcs->m_strSelectFile);
				DWORD dwRet = lpcs->m_Id3tagv2.DelTag(lpcs->m_strSelectFile);
				if(dwRet == -1)
				{
					//�u�t�@�C���𐳂����X�V�ł��܂���ł����B�v
					AfxMessageBox(IDS_PAGE_BROKEN,MB_ICONINFORMATION);
					break;
				}
				else if(dwRet != ERROR_SUCCESS)
				{
					//�V�X�e���G���[��\��
					errMessageBox(hDlg,dwRet);
					break;
				}
				dwRet = lpcs->m_Rmp3.MakeTag(hDlg,lpcs->m_strSelectFile);
				if(dwRet == -1)
				{
					//�u�t�@�C���𐳂����X�V�ł��܂���ł����B�v
					AfxMessageBox(IDS_PAGE_BROKEN,MB_ICONINFORMATION);
					break;
				}
				else if(dwRet != ERROR_SUCCESS)
				{
					//�V�X�e���G���[��\��
					errMessageBox(hDlg,dwRet);
					break;
				}
				//�^�C���X�^���v�𕜌�
				if(lpcs->m_bSaveTimeStamp)
				{
					lpcs->PopTimeStamp(lpcs->m_strSelectFile);
				}
				
				lpcs->m_Rmp3.SetNAM(strTitle);
				lpcs->m_Rmp3.SetART(strArtist);
				lpcs->m_Rmp3.SetPRD(strAlbum);
				lpcs->m_Rmp3.SetCRD(strYear);
				lpcs->m_Rmp3.SetGNR(strGenre);
				lpcs->m_Rmp3.SetCMT(strComment);
				lpcs->m_Rmp3.SetCOP(strCopyright);
//				lpcs->m_Rmp3.SetENG();
//				lpcs->m_Rmp3.SetSRC();
				lpcs->m_Rmp3.SetSFT(strEncoder);
				//����\��
				DispInfo(hDlg,lpcs);
				lpcs->m_bRiffApply = TRUE;
				
				//�V�F���ɕύX��ʒm
				SHChangeNotify(SHCNE_UPDATEITEM,SHCNF_PATH,lpcs->m_strSelectFile,NULL);
				
				//���̌`���^�u���X�V
				if(lpcs->m_hwndId3v2)
					PostMessage(lpcs->m_hwndId3v2,WM_USER+1,0,0);
				//�����X�V
				NMHDR nmhdr;
				nmhdr.code = PSN_APPLY;
				PageDlgProc_mp3_RMP(hDlg,WM_NOTIFY,0,(LPARAM )&nmhdr);
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
			if(lpcs->m_hwndId3v1)
				PostMessage(lpcs->m_hwndId3v1,WM_USER+1,0,0);
			if(lpcs->m_hwndId3v2)
				PostMessage(lpcs->m_hwndId3v2,WM_USER+1,0,0);
			if(lpcs->m_hwndMp3Ape)
				PostMessage(lpcs->m_hwndMp3Ape,WM_USER+1,0,0);
			regSetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("PropAOT"),(DWORD )lpcs->m_bPropAOT);
			break;
		case IDC_SETUP:
			OpenConfigPage(hDlg,2);
			break;
		case IDC_HELPVIEW:
			lpcs->OpenHtmlHelp(hDlg,_T("extension.htm"));
			break;
		DLG_CLIPBORD_MACRO(lpcs->m_strSelectFile);
		}
		break;

	// �R���e�L�X�g���j���[
	case WM_CONTEXTMENU:
		DlgContextMenu(hDlg,lParam,lpcs->m_Rmp3.IsEnable());
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
			if(lpcs->m_bRiffApply && lpcs->m_Rmp3.IsEnable())
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
				lpcs->m_bRiffApply = FALSE;

				CString strTmp;
				CWnd wnd;
				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_NAM));
				wnd.GetWindowText(strTmp);
				lpcs->m_Rmp3.SetNAM(strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_ART));
				wnd.GetWindowText(strTmp);
				lpcs->m_Rmp3.SetART(strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_PRD));
				wnd.GetWindowText(strTmp);
				lpcs->m_Rmp3.SetPRD(strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_CRD));
				wnd.GetWindowText(strTmp);
				lpcs->m_Rmp3.SetCRD(strTmp);
				wnd.Detach(); 

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_GNR));
				wnd.GetWindowText(strTmp);
				lpcs->m_Rmp3.SetGNR(strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_CMT));
				wnd.GetWindowText(strTmp);
				lpcs->m_Rmp3.SetCMT(strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_COP));
				wnd.GetWindowText(strTmp);
				lpcs->m_Rmp3.SetCOP(strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_ENG));
				wnd.GetWindowText(strTmp);
				lpcs->m_Rmp3.SetENG(strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_SRC));
				wnd.GetWindowText(strTmp);
				lpcs->m_Rmp3.SetSRC(strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_SFT));
				wnd.GetWindowText(strTmp);
				lpcs->m_Rmp3.SetSFT(strTmp);
				wnd.Detach();

				//�^�C���X�^���v��ۑ�
				lpcs->PushTimeStamp(lpcs->m_strSelectFile);

				DWORD dwRet = lpcs->m_Rmp3.Save(hDlg,lpcs->m_strSelectFile);
				if(dwRet != ERROR_SUCCESS)
				{
					lpcs->m_bRiffApply = TRUE;
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
				lpcs->m_bRiffApply = FALSE;

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
