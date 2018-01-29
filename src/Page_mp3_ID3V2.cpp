#include "StdAfx.h"
#include "mp3infp.h"

#ifdef _DEBUG
#define ENABLE_UTF_16BE
#endif

static int ids[] =
{
	-1,
	-1,
	CLP_NAM,
	CLP_TRACK,
	CLP_ART,
	CLP_PRD,
	CLP_AART,
	CLP_CRD,
	CLP_GNR,
	CLP_CMT,
	CLP_COMPOSE,
	CLP_ORGART,
	CLP_COP,
	CLP_URL1,
	CLP_ENC,
	CLP_ENC2,
	-1,
	-1,
	0
};

static int staticWnd[] =
{
	IDC_STATIC_FORMAT_,
	IDC_STATIC_TIME_,
	IDC_STATIC_NAM,
	IDC_STATIC_TRACK,
	IDC_STATIC_ART,
	IDC_STATIC_PRD,
	IDC_STATIC_AART,
	IDC_STATIC_CRD,
	IDC_STATIC_GNR,
	IDC_STATIC_CMT,
	IDC_STATIC_COMP,
	IDC_STATIC_ORGART,
	IDC_STATIC_COPYRIGHT,
	IDC_STATIC_URL,
	IDC_STATIC_ENC,
	IDC_STATIC_ENC2,
	IDC_STATIC_ID3VER,
	IDC_STATIC_ENCODE,
	0
};

static int editWnd[] =
{
	IDC_STATIC_FORMAT,
	IDC_STATIC_TIME,
	IDC_EDIT_NAM,
	IDC_EDIT_TRACK,
	IDC_EDIT_ART,
	IDC_EDIT_PRD,
	IDC_EDIT_AART,
	IDC_EDIT_CRD,
	IDC_EDIT_GNR,
	IDC_EDIT_CMT,
	IDC_EDIT_COMP,
	IDC_EDIT_ORGART,
	IDC_EDIT_COPYRIGHT,
	IDC_EDIT_URL,
	IDC_EDIT_ENC,
	IDC_EDIT_ENC2,
	IDC_EDIT_ID3VER,
	IDC_EDIT_UNICODE,
	0
};

UINT CALLBACK CShellExt::PageCallback_mp3_ID3V2(HWND hWnd, UINT uMessage, LPPROPSHEETPAGE ppsp)
{
	LPCSHELLEXT	lpcs = (CShellExt *)ppsp->lParam;
	switch(uMessage){
	case PSPCB_CREATE:	//�v���p�e�B�y�[�W���쐬�������
		return TRUE;
	case PSPCB_RELEASE:	//�v���p�e�B�y�[�W���j�󂳂����
		if(lpcs) 
		{
			lpcs->m_hwndId3v2 = NULL;
			lpcs->Release();
		}
		return TRUE;
	}

	return TRUE;
}

static void SelectEncodeCB(HWND hDlg,int encode)
{
	switch(encode){
	default:
	case 0:	// ID3V2CHARENCODE_ISO_8859_1
		ComboBox_SetCurSel(GetDlgItem(hDlg,IDC_EDIT_UNICODE),0);
		break;
	case 1:	// ID3V2CHARENCODE_UTF_16
		ComboBox_SetCurSel(GetDlgItem(hDlg,IDC_EDIT_UNICODE),1);
		break;
	case 2:	// ID3V2CHARENCODE_UTF_16BE
		ComboBox_SetCurSel(GetDlgItem(hDlg,IDC_EDIT_UNICODE),3);
		break;
	case 3:	// ID3V2CHARENCODE_UTF_8
		ComboBox_SetCurSel(GetDlgItem(hDlg,IDC_EDIT_UNICODE),2);
		break;
	}
}

static void SetEncodeCB(HWND hDlg)
{
	// �R���{�{�b�N�X���N���A
	ComboBox_ResetContent(GetDlgItem(hDlg,IDC_EDIT_UNICODE));

	// ID3v2�̃o�[�W�����ɂ���ăR���{�{�b�N�X�̓��e�����܂�
	long cur = ComboBox_GetCurSel(GetDlgItem(hDlg,IDC_EDIT_ID3VER));
	if(cur >= 2)	// > v2.4
	{
#ifndef	ENABLE_UTF_16BE
		LPCTSTR encode[] = {_T("ISO-8859-1"),_T("UTF-16(Unicode)"),_T("UTF-8(Unicode)")};
#else
		LPCTSTR encode[] = {_T("ISO-8859-1"),_T("UTF-16(Unicode)"),_T("UTF-8(Unicode)"),_T("UTF-16BE")};
#endif
		for(int i=0; i<sizeof_array(encode); i++)
		{
			ComboBox_AddString(GetDlgItem(hDlg,IDC_EDIT_UNICODE),encode[i]);
		}
	}
	else
	{
		LPCTSTR encode[] = {_T("ISO-8859-1"),_T("UTF-16(Unicode)")};
		for(int i=0; i<sizeof_array(encode); i++)
		{
			ComboBox_AddString(GetDlgItem(hDlg,IDC_EDIT_UNICODE),encode[i]);
		}
	}
}

static void EnableButton(HWND hDlg,CShellExt *lpcs)
{
	if(lpcs->m_Id3tagv1.IsEnable() && lpcs->m_Id3tagv2.IsEnable())
		EnableWindow(GetDlgItem(hDlg,IDC_COPY_FROM_ID3V1),TRUE);	//Copy from ID3v1
	else
		EnableWindow(GetDlgItem(hDlg,IDC_COPY_FROM_ID3V1),FALSE);	//Copy from ID3v1

	if(lpcs->m_Rmp3.IsEnable())
		EnableWindow(GetDlgItem(hDlg,IDC_COPY_FROM_RIFF),TRUE);	//Copy from RMP
	else
		EnableWindow(GetDlgItem(hDlg,IDC_COPY_FROM_RIFF),FALSE);	//Copy from RMP
}

static void EnableEdit(HWND hDlg,CShellExt *lpcs,BOOL bEnable)
{
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_NAM),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_NAM),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_NAM),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_TRACK),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_TRACK),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_TRACK),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_ART),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_ART),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_ART),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_PRD),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_PRD),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_PRD),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_AART),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_AART),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_AART),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_CRD),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_CRD),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_CRD),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_GNR),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_GNR),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_GNR),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_CMT),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_CMT),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_CMT),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_COMP),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_COMP),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_COMP),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_ORGART),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_ORGART),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_ORGART),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_COPYRIGHT),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_COPYRIGHT),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_COPYRIGHT),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_URL),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_URL),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_URL),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_URL_JUMP),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_ENC),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_ENC),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_ENC),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_ENC2),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_ENC2),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_ENC2),bEnable);
//	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_ID3VER),_T(""));
//	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_ID3VER),bEnable);
//	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_ID3VER),bEnable);
//	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_UNICODE),_T(""));
//	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_UNICODE),bEnable);
//	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_UNICODE),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_CHECK_UNSYNC),bEnable);
	if(bEnable)
	{
		if(lpcs->m_Id3tagv2.IsEnable())
		{
			EnableWindow(GetDlgItem(hDlg,IDC_BUTTON_DEL_TAG),TRUE);	//Del Id3Tag
		}
		else
		{
			EnableWindow(GetDlgItem(hDlg,IDC_BUTTON_DEL_TAG),FALSE);	//Del Id3Tag
		}
		EnableWindow(GetDlgItem(hDlg,IDC_BUTTON_MAKE_TAG),FALSE);	//Make Id3Tag
		HWND hWndTab = PropSheet_GetTabControl(GetParent(hDlg));
		TC_ITEM tcItem;
		tcItem.mask = TCIF_TEXT;
		tcItem.pszText = _T("ID3v2");
		TabCtrl_SetItem(hWndTab,PropSheet_PageToIndex(GetParent(hDlg),lpcs->m_hpageId3v2),&tcItem);
		PropSheet_RecalcPageSizes(GetParent(hDlg));
	}
	else
	{
		CheckDlgButton(hDlg,IDC_CHECK_UNSYNC,BST_UNCHECKED);
		EnableWindow(GetDlgItem(hDlg,IDC_BUTTON_DEL_TAG),FALSE);	//Del Id3Tag
		if(lpcs->m_Id3tagv2.IsEnable())
		{
			EnableWindow(GetDlgItem(hDlg,IDC_BUTTON_MAKE_TAG),FALSE);	//Make Id3Tag
		}
		else
		{
			EnableWindow(GetDlgItem(hDlg,IDC_BUTTON_MAKE_TAG),TRUE);	//Make Id3Tag
		}
		HWND hWndTab = PropSheet_GetTabControl(GetParent(hDlg));
		TC_ITEM tcItem;
		tcItem.mask = TCIF_TEXT;
		tcItem.pszText = _T("ID3v2(*)");
		TabCtrl_SetItem(hWndTab,PropSheet_PageToIndex(GetParent(hDlg),lpcs->m_hpageId3v2),&tcItem);
		PropSheet_RecalcPageSizes(GetParent(hDlg));
	}

	EnableButton(hDlg,lpcs);
}

static void DispInfo(HWND hDlg,CShellExt *lpcs)
{
	//mpeg���
	SetDlgItemText(hDlg,IDC_STATIC_FORMAT,lpcs->m_mp3info.GetFormatString());
	SetDlgItemText(hDlg,IDC_STATIC_TIME,lpcs->m_mp3info.GetTimeString());

	if(lpcs->m_Id3tagv2.IsEnable())
	{
		switch(lpcs->m_Id3tagv2.GetVer()){
		case 0x0200:
			ComboBox_SetCurSel(GetDlgItem(hDlg,IDC_EDIT_ID3VER),0);
			break;
		case 0x0300:
			ComboBox_SetCurSel(GetDlgItem(hDlg,IDC_EDIT_ID3VER),1);
			break;
		case 0x0400:
			ComboBox_SetCurSel(GetDlgItem(hDlg,IDC_EDIT_ID3VER),2);
			break;
		}
//		if(lpcs->m_Id3tagv2.GetUniocdeEncode())
//		{
//			CheckDlgButton(hDlg,IDC_CHECK_UNICODE,BST_CHECKED);
//		}
//		else
//		{
//			CheckDlgButton(hDlg,IDC_CHECK_UNICODE,BST_UNCHECKED);
//		}
		SetEncodeCB(hDlg);
		SelectEncodeCB(hDlg,lpcs->m_Id3tagv2.GetCharEncode());
		if(lpcs->m_Id3tagv2.GetUnSynchronization())
		{
			CheckDlgButton(hDlg,IDC_CHECK_UNSYNC,BST_CHECKED);
		}
		else
		{
			CheckDlgButton(hDlg,IDC_CHECK_UNSYNC,BST_UNCHECKED);
		}
		{
			EnableEdit(hDlg,lpcs,TRUE);

			//�^�C�g��
			SetDlgItemText(hDlg,IDC_EDIT_NAM,lpcs->m_Id3tagv2.GetTitle());
			//�g���b�N�ԍ�
			SetDlgItemText(hDlg,IDC_EDIT_TRACK,lpcs->m_Id3tagv2.GetTrackNo());
			//�A�[�e�B�X�g
			SetDlgItemText(hDlg,IDC_EDIT_ART,lpcs->m_Id3tagv2.GetArtist());
			//�A���o��
			SetDlgItemText(hDlg,IDC_EDIT_PRD,lpcs->m_Id3tagv2.GetAlbum());
			//�A���o���A�[�e�B�X�g
			SetDlgItemText(hDlg,IDC_EDIT_AART,lpcs->m_Id3tagv2.GetAlbumArtist());
			//����
			SetDlgItemText(hDlg,IDC_EDIT_CRD,lpcs->m_Id3tagv2.GetYear());
			//�W������
			SetDlgItemText(hDlg,IDC_EDIT_GNR,lpcs->m_Id3tagv2.GetGenre());
			//�R�����g
			SetDlgItemText(hDlg,IDC_EDIT_CMT,lpcs->m_Id3tagv2.GetComment());
			//���
			SetDlgItemText(hDlg,IDC_EDIT_COMP,lpcs->m_Id3tagv2.GetComposer());
			//orig.�A�[�e�B�X�g
			SetDlgItemText(hDlg,IDC_EDIT_ORGART,lpcs->m_Id3tagv2.GetOrigArtist());
			//���쌠
			SetDlgItemText(hDlg,IDC_EDIT_COPYRIGHT,lpcs->m_Id3tagv2.GetCopyright());
			//URL
			SetDlgItemText(hDlg,IDC_EDIT_URL,lpcs->m_Id3tagv2.GetUrl());
			//�G���R�[�_
			SetDlgItemText(hDlg,IDC_EDIT_ENC,lpcs->m_Id3tagv2.GetEncoder());
			//�G���R�[�h�����l�܂��͑g�D
			SetDlgItemText(hDlg,IDC_EDIT_ENC2,lpcs->m_Id3tagv2.GetEncodest());
		}

		lpcs->m_bId3v2Apply = FALSE;
	}
	else
	{
		ComboBox_SetCurSel(GetDlgItem(hDlg,IDC_EDIT_UNICODE),-1);
		ComboBox_SetCurSel(GetDlgItem(hDlg,IDC_EDIT_ID3VER),-1);
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

BOOL CALLBACK CShellExt::PageDlgProc_mp3_ID3V2(HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	LPCSHELLEXT	lpcs = (CShellExt *)GetWindowLongPtr(hDlg,DWLP_USER);
	CString strTmp;

	switch(uMessage){
	case WM_USER+1:	//�\���̃��t���b�V��
		EnableButton(hDlg,lpcs);
		DispInfo(hDlg,lpcs);
		lpcs->m_bId3v2Apply = FALSE;
		break;
	case WM_INITDIALOG:
		{
			lpcs = (LPCSHELLEXT )((LPPROPSHEETPAGE )lParam)->lParam;
			lpcs->m_hwndId3v2 = hDlg;
			SetWindowLongPtr(hDlg, DWLP_USER, (LONG_PTR)lpcs);
			lpcs->m_bId3v2Apply = FALSE;

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
			LPCTSTR id3tags[] = {_T("v2.2"),_T("v2.3"),_T("v2.4")};
			for(int i=0; i<sizeof_array(id3tags); i++)
			{
				ComboBox_AddString(GetDlgItem(hDlg,IDC_EDIT_ID3VER),id3tags[i]);
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
			lpcs->m_bId3v2Apply = FALSE;
		}
		break;

	case WM_DESTROY:
		break;

	// �I�[�i�[�`��R���g���[��
	case WM_DRAWITEM:
		DrawControl(hDlg,(LPDRAWITEMSTRUCT )lParam);
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case IDC_CHECK_UNSYNC:
			if( HIWORD(wParam) == BN_CLICKED)
			{
				PropSheet_Changed(GetParent(hDlg),hDlg);
				lpcs->m_bId3v2Apply = TRUE;
			}
			break;
		case IDC_EDIT_NAM:
		case IDC_EDIT_TRACK:
		case IDC_EDIT_ART:
		case IDC_EDIT_PRD:
		case IDC_EDIT_AART:
		case IDC_EDIT_CRD:
		case IDC_EDIT_CMT:
		case IDC_EDIT_COMP:
		case IDC_EDIT_ORGART:
		case IDC_EDIT_COPYRIGHT:
		case IDC_EDIT_URL:
		case IDC_EDIT_ENC:
		case IDC_EDIT_ENC2:
			if( (HIWORD(wParam) == EN_CHANGE) &&
				(IsWindowEnabled((HWND )lParam)) )
			{
				PropSheet_Changed(GetParent(hDlg),hDlg);
				lpcs->m_bId3v2Apply = TRUE;
			}
			break;
		case IDC_EDIT_GNR:
			if( ((HIWORD(wParam) == CBN_SELENDOK) || (HIWORD(wParam) == CBN_EDITCHANGE)) &&
				(IsWindowEnabled((HWND )lParam)) )
			{
				PropSheet_Changed(GetParent(hDlg),hDlg);
				lpcs->m_bId3v2Apply = TRUE;
			}
			break;
		case IDC_EDIT_ID3VER:
			if( (HIWORD(wParam) == CBN_SELENDOK) &&
				(IsWindowEnabled((HWND )lParam)) )
			{
				PropSheet_Changed(GetParent(hDlg),hDlg);
				lpcs->m_bId3v2Apply = TRUE;
				long cur = ComboBox_GetCurSel(GetDlgItem(hDlg,IDC_EDIT_UNICODE));
				SetEncodeCB(hDlg);	// ID3�o�[�W�����ɂ���đI���\�ȃG���R�[�h���ς��
				long cnt = ComboBox_GetCount(GetDlgItem(hDlg,IDC_EDIT_UNICODE));
				if(cur >= cnt)
				{
					cur = cnt - 1;
				}
				ComboBox_SetCurSel(GetDlgItem(hDlg,IDC_EDIT_UNICODE),cur);
			}
			break;
		case IDC_EDIT_UNICODE:
			if( (HIWORD(wParam) == CBN_SELENDOK) &&
				(IsWindowEnabled((HWND )lParam)) )
			{
				PropSheet_Changed(GetParent(hDlg),hDlg);
				lpcs->m_bId3v2Apply = TRUE;
			}
			break;
		case IDC_BUTTON_DEL_TAG:
			//�uID3v2 TAG���폜���܂��B��낵���ł����H�v
			strTmp.Format(IDS_PAGE_MP3ID3V2_DEL_ID3TAG);
			if(MessageBox(hDlg,strTmp,APP_NAME,MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES)
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
				
				// Winamp����U��~
/*				int nowPlaying = IsPlayingWinamp((char *)(LPCTSTR )lpcs->m_strSelectFile);
				int nowPlayPos = 0;
				if(nowPlaying)
				{
					nowPlayPos = GetPosWinamp();
					StopWinamp();
				}*/
				
				//ID3TAG������
				DWORD dwRet = lpcs->m_Id3tagv2.DelTag(lpcs->m_strSelectFile);
				
				//�^�C���X�^���v�𕜌�
				if(lpcs->m_bSaveTimeStamp)
				{
					lpcs->PopTimeStamp(lpcs->m_strSelectFile);
				}

				// Winamp���ĊJ
/*				if(nowPlaying)
				{
					PlayWinamp(hDlg,(LPCTSTR )lpcs->m_strSelectFile);
					if(nowPlaying == 3)
					{
						PauseWinamp();
					}
					SetPosWinamp(nowPlayPos);
				}*/
				
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
				//���̌`���^�u���X�V
				if(lpcs->m_hwndId3v1)
					PostMessage(lpcs->m_hwndId3v1,WM_USER+1,0,0);
				if(lpcs->m_hwndMp3Ape)
					PostMessage(lpcs->m_hwndMp3Ape,WM_USER+1,0,0);
				if(lpcs->m_hwndRiffSIF)
					PostMessage(lpcs->m_hwndRiffSIF,WM_USER+1,0,0);
			}
			//����\��
			DispInfo(hDlg,lpcs);
			lpcs->m_bId3v2Apply = FALSE;
	
			//�V�F���ɕύX��ʒm
			SHChangeNotify(SHCNE_UPDATEITEM,SHCNF_FLUSH|SHCNF_PATH,lpcs->m_strSelectFile,NULL);
			break;
		case IDC_BUTTON_MAKE_TAG:
			if(lpcs->m_Rmp3.IsEnable())
			{
				//�u����RMP�`���̂��߁AID3v2 TAG���쐬�ł��܂���BRiff SIF���폜���Ă��������B�v
				strTmp.Format(IDS_PAGE_MP3ID3V2_CAN_NOT_MAKE);
				MessageBox(hDlg,strTmp,APP_NAME,MB_ICONINFORMATION);
			}
			else
			{
				//�uID3v2 TAG���쐬���܂��B��낵���ł����H�v
				strTmp.Format(IDS_PAGE_MP3ID3V2_MAKE_ID3TAG);
				if(MessageBox(hDlg,strTmp,APP_NAME,MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES)
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
					// �쐬����ID3�o�[�W����������
					long cur = ComboBox_GetCurSel(GetDlgItem(hDlg,IDC_EDIT_ID3VER));
					BOOL bUnsync = TRUE;
					switch(cur){
					case 0:	// v2.2
						lpcs->m_Id3tagv2.SetVer(0x0200);
						break;
					case 1:	// v2.3
					default:
						lpcs->m_Id3tagv2.SetVer(0x0300);
						break;
					case 2:	// v2.4
						lpcs->m_Id3tagv2.SetVer(0x0400);
						bUnsync = FALSE;
						break;
					}
					// Unsync/Unicode�w��
					cur = ComboBox_GetCurSel(GetDlgItem(hDlg,IDC_EDIT_UNICODE));
					switch(cur){
					default:
					case 0:
						lpcs->m_Id3tagv2.SetCharEncode(CId3tagv2::ID3V2CHARENCODE_ISO_8859_1);
						break;
					case 1:
						lpcs->m_Id3tagv2.SetCharEncode(CId3tagv2::ID3V2CHARENCODE_UTF_16);
						break;
					case 2:
						lpcs->m_Id3tagv2.SetCharEncode(CId3tagv2::ID3V2CHARENCODE_UTF_8);
						break;
					case 3:
						lpcs->m_Id3tagv2.SetCharEncode(CId3tagv2::ID3V2CHARENCODE_UTF_16BE);
						break;
					}
					lpcs->m_Id3tagv2.SetUnSynchronization(bUnsync);
					// Winamp����U��~
/*					int nowPlaying = IsPlayingWinamp((char *)(LPCTSTR )lpcs->m_strSelectFile);
					int nowPlayPos = 0;
					if(nowPlaying)
					{
						nowPlayPos = GetPosWinamp();
						StopWinamp();
					}*/
					//ID3TAG���쐬
					DWORD dwRet = lpcs->m_Id3tagv2.MakeTag(lpcs->m_strSelectFile);
					
					//�^�C���X�^���v�𕜌�
					if(lpcs->m_bSaveTimeStamp)
					{
						lpcs->PopTimeStamp(lpcs->m_strSelectFile);
					}
					
					// Winamp���ĊJ
/*					if(nowPlaying)
					{
						PlayWinamp(hDlg,(LPCTSTR )lpcs->m_strSelectFile);
						if(nowPlaying == 3)
						{
							PauseWinamp();
						}
						SetPosWinamp(nowPlayPos);
					}*/
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
					//����\��
					DispInfo(hDlg,lpcs);
					lpcs->m_bId3v2Apply = FALSE;
					
					//�V�F���ɕύX��ʒm
					SHChangeNotify(SHCNE_UPDATEITEM,SHCNF_FLUSH|SHCNF_PATH,lpcs->m_strSelectFile,NULL);
					
					//���̌`���^�u���X�V
					if(lpcs->m_hwndId3v1)
						PostMessage(lpcs->m_hwndId3v1,WM_USER+1,0,0);
					if(lpcs->m_hwndMp3Ape)
						PostMessage(lpcs->m_hwndMp3Ape,WM_USER+1,0,0);
					if(lpcs->m_hwndRiffSIF)
						PostMessage(lpcs->m_hwndRiffSIF,WM_USER+1,0,0);
				}
			}
			break;
		case IDC_COPY_FROM_ID3V1:
			//�^�C�g��
			SetDlgItemText(hDlg,IDC_EDIT_NAM,lpcs->m_Id3tagv1.GetTitle());
			//�g���b�N�ԍ�
			SetDlgItemText(hDlg,IDC_EDIT_TRACK,lpcs->m_Id3tagv1.GetTrackNo());
			//�A�[�e�B�X�g
			SetDlgItemText(hDlg,IDC_EDIT_ART,lpcs->m_Id3tagv1.GetArtist());
			//�A���o��
			SetDlgItemText(hDlg,IDC_EDIT_PRD,lpcs->m_Id3tagv1.GetAlbum());
			//����
			SetDlgItemText(hDlg,IDC_EDIT_CRD,lpcs->m_Id3tagv1.GetYear());
			//�W������
			SetDlgItemText(hDlg,IDC_EDIT_GNR,lpcs->m_Id3tagv1.GetGenre());
			//�R�����g
			SetDlgItemText(hDlg,IDC_EDIT_CMT,lpcs->m_Id3tagv1.GetComment());
			break;
		case IDC_COPY_FROM_RIFF:
			//�uRiffSIF(RMP�`��)��ID3v2�͋����ł��܂���B\nRiffSIF�̓��e���R�s�[�������RiffSIF����菜���܂����H�v
			strTmp.Format(IDS_PAGE_MP3ID3V2_CAN_NOT_MAKE2);
			if(MessageBox(hDlg,strTmp,APP_NAME,MB_ICONINFORMATION|MB_YESNO) == IDYES)
			{
				//�t�@�C�����������݉\�����ׂ�
				if(GetFileAttributes(lpcs->m_strSelectFile) & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY))
				{
					//�u���̃t�@�C���́u�ǂݍ��ݐ�p�v�ł��v
					AfxMessageBox(IDS_PAGE_THIS_READONLY,MB_ICONINFORMATION);
					break;
				}
				CString strTitle	= lpcs->m_Rmp3.GetNAM();
				CString strArtist	= lpcs->m_Rmp3.GetART();
				CString strAlbum	= lpcs->m_Rmp3.GetPRD();
				CString strComment	= lpcs->m_Rmp3.GetCMT();
				CString strYear		= lpcs->m_Rmp3.GetCRD();
				CString strGenre	= lpcs->m_Rmp3.GetGNR();
				CString strCopyright= lpcs->m_Rmp3.GetCOP();
				CString strEncoder	= lpcs->m_Rmp3.GetSFT();

				//�^�C���X�^���v��ۑ�
				lpcs->PushTimeStamp(lpcs->m_strSelectFile);
				// Winamp����U��~
/*				int nowPlaying = IsPlayingWinamp((char *)(LPCTSTR )lpcs->m_strSelectFile);
				int nowPlayPos = 0;
				if(nowPlaying)
				{
					nowPlayPos = GetPosWinamp();
					StopWinamp();
				}*/
				DWORD dwRet = lpcs->m_Rmp3.DelTag(hDlg,lpcs->m_strSelectFile);
				
				// Winamp���ĊJ
/*				if(nowPlaying)
				{
					PlayWinamp(hDlg,(LPCTSTR )lpcs->m_strSelectFile);
					if(nowPlaying == 3)
					{
						PauseWinamp();
					}
					SetPosWinamp(nowPlayPos);
				}*/
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
				dwRet = lpcs->m_Id3tagv2.MakeTag(lpcs->m_strSelectFile);
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
				
				lpcs->m_Id3tagv2.SetTitle(strTitle);
				lpcs->m_Id3tagv2.SetArtist(strArtist);
				lpcs->m_Id3tagv2.SetAlbum(strAlbum);
				lpcs->m_Id3tagv2.SetYear(strYear);
				lpcs->m_Id3tagv2.SetGenre(strGenre);
				lpcs->m_Id3tagv2.SetComment(strComment);
				lpcs->m_Id3tagv2.SetCopyright(strCopyright);
				lpcs->m_Id3tagv2.SetEncoder(strEncoder);
				//����\��
				DispInfo(hDlg,lpcs);
				lpcs->m_bId3v2Apply = TRUE;
				
				//�V�F���ɕύX��ʒm
				SHChangeNotify(SHCNE_UPDATEITEM,SHCNF_FLUSH|SHCNF_PATH,lpcs->m_strSelectFile,NULL);

				//���̌`���^�u���X�V
				if(lpcs->m_hwndRiffSIF)
					PostMessage(lpcs->m_hwndRiffSIF,WM_USER+1,0,0);
				//�����X�V
				NMHDR nmhdr;
				nmhdr.code = PSN_APPLY;
				PageDlgProc_mp3_ID3V2(hDlg,WM_NOTIFY,0,(LPARAM )&nmhdr);
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
		case IDC_URL_JUMP:
			{
				CString strTmp;
				CWnd wnd;

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_URL));
				wnd.GetWindowText(strTmp);
				wnd.Detach();
				if(strTmp.GetLength())
				{
					ShellExecute(hDlg,_T("open"),strTmp,NULL,NULL,SW_SHOW);
				}
			}
			break;
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
			if(lpcs->m_hwndRiffSIF)
				PostMessage(lpcs->m_hwndRiffSIF,WM_USER+1,0,0);
			if(lpcs->m_hwndMp3Ape)
				PostMessage(lpcs->m_hwndMp3Ape,WM_USER+1,0,0);
			regSetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,_T("PropAOT"),(DWORD )lpcs->m_bPropAOT);
			break;
		case IDC_SETUP:
			OpenSetupPage(hDlg,2);
			break;
		case IDC_HELPVIEW:
			lpcs->OpenHtmlHelp(hDlg,_T("extension.htm"));
			break;
		DLG_CLIPBORD_MACRO(lpcs->m_strSelectFile);
		}
		break;

	// �R���e�L�X�g���j���[
	case WM_CONTEXTMENU:
		DlgContextMenu(hDlg,lParam,lpcs->m_Id3tagv2.IsEnable());
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

	case WM_NOTIFY:
		switch (((NMHDR FAR *)lParam)->code){
//		case PSN_SETACTIVE:
//			break;
		case PSN_APPLY:
			//�ۑ�
			if(lpcs->m_bId3v2Apply && lpcs->m_Id3tagv2.IsEnable())
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
				lpcs->m_bId3v2Apply = FALSE;

				long cur = ComboBox_GetCurSel(GetDlgItem(hDlg,IDC_EDIT_ID3VER));
				switch(cur){
				case 0:	// v2.2
					lpcs->m_Id3tagv2.SetVer(0x0200);
					break;
				case 1:	// v2.3
					lpcs->m_Id3tagv2.SetVer(0x0300);
					break;
				case 2:	// v2.4
					lpcs->m_Id3tagv2.SetVer(0x0400);
					CheckDlgButton(hDlg,IDC_CHECK_UNSYNC,BST_UNCHECKED);
					break;
				default:
				case CB_ERR:
					break;
				}
				// Unsync/Unicode�w��
				cur = ComboBox_GetCurSel(GetDlgItem(hDlg,IDC_EDIT_UNICODE));
				switch(cur){
				default:
				case 0:
					lpcs->m_Id3tagv2.SetCharEncode(CId3tagv2::ID3V2CHARENCODE_ISO_8859_1);
					break;
				case 1:
					lpcs->m_Id3tagv2.SetCharEncode(CId3tagv2::ID3V2CHARENCODE_UTF_16);
					break;
				case 2:
					lpcs->m_Id3tagv2.SetCharEncode(CId3tagv2::ID3V2CHARENCODE_UTF_8);
					break;
				case 3:
					lpcs->m_Id3tagv2.SetCharEncode(CId3tagv2::ID3V2CHARENCODE_UTF_16BE);
					break;
				}
				if(IsDlgButtonChecked(hDlg,IDC_CHECK_UNSYNC))
				{
					lpcs->m_Id3tagv2.SetUnSynchronization(TRUE);
				}
				else
				{
					lpcs->m_Id3tagv2.SetUnSynchronization(FALSE);
				}
				CString strTmp;
				CWnd wnd;

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_NAM));
				wnd.GetWindowText(strTmp);
				lpcs->m_Id3tagv2.SetTitle(strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_TRACK));
				wnd.GetWindowText(strTmp);
				lpcs->m_Id3tagv2.SetTrackNo(strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_ART));
				wnd.GetWindowText(strTmp);
				lpcs->m_Id3tagv2.SetArtist(strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_PRD));
				wnd.GetWindowText(strTmp);
				lpcs->m_Id3tagv2.SetAlbum(strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_AART));
				wnd.GetWindowText(strTmp);
				lpcs->m_Id3tagv2.SetAlbumArtist(strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_CRD));
				wnd.GetWindowText(strTmp);
				lpcs->m_Id3tagv2.SetYear(strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_GNR));
				wnd.GetWindowText(strTmp);
				lpcs->m_Id3tagv2.SetGenre(strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_CMT));
				wnd.GetWindowText(strTmp);
				lpcs->m_Id3tagv2.SetComment(strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_COMP));
				wnd.GetWindowText(strTmp);
				lpcs->m_Id3tagv2.SetComposer(strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_ORGART));
				wnd.GetWindowText(strTmp);
				lpcs->m_Id3tagv2.SetOrigArtist(strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_COPYRIGHT));
				wnd.GetWindowText(strTmp);
				lpcs->m_Id3tagv2.SetCopyright(strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_URL));
				wnd.GetWindowText(strTmp);
				lpcs->m_Id3tagv2.SetUrl(strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_ENC));
				wnd.GetWindowText(strTmp);
				lpcs->m_Id3tagv2.SetEncoder(strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_ENC2));
				wnd.GetWindowText(strTmp);
				lpcs->m_Id3tagv2.SetEncodest(strTmp);
				wnd.Detach();

				//�^�C���X�^���v��ۑ�
				lpcs->PushTimeStamp(lpcs->m_strSelectFile);

				// Winamp����U��~
/*				int nowPlaying = IsPlayingWinamp((char *)(LPCTSTR )lpcs->m_strSelectFile);
				int nowPlayPos = 0;
				if(nowPlaying)
				{
					nowPlayPos = GetPosWinamp();
					StopWinamp();
				}*/

				DWORD dwRet = lpcs->m_Id3tagv2.Save(lpcs->m_strSelectFile);
				
				//�^�C���X�^���v�𕜌�
				if(lpcs->m_bSaveTimeStamp)
				{
					lpcs->PopTimeStamp(lpcs->m_strSelectFile);
				}
				
				// Winamp���ĊJ
/*				if(nowPlaying)
				{
					PlayWinamp(hDlg,(LPCTSTR )lpcs->m_strSelectFile);
					if(nowPlaying == 3)
					{
						PauseWinamp();
					}
					SetPosWinamp(nowPlayPos);
				}*/
				if(dwRet != ERROR_SUCCESS)
				{
					lpcs->m_bId3v2Apply = TRUE;
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

				//����\��
				DispInfo(hDlg,lpcs);
				lpcs->m_bId3v2Apply = FALSE;

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
