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
		SendMessage(GetDlgItem(hDlg,IDC_EDIT_UNICODE),CB_SETCURSEL,0,0);
		break;
	case 1:	// ID3V2CHARENCODE_UTF_16
		SendMessage(GetDlgItem(hDlg,IDC_EDIT_UNICODE),CB_SETCURSEL,1,0);
		break;
	case 2:	// ID3V2CHARENCODE_UTF_16BE
		SendMessage(GetDlgItem(hDlg,IDC_EDIT_UNICODE),CB_SETCURSEL,3,0);
		break;
	case 3:	// ID3V2CHARENCODE_UTF_8
		SendMessage(GetDlgItem(hDlg,IDC_EDIT_UNICODE),CB_SETCURSEL,2,0);
		break;
	}
}

static void SetEncodeCB(HWND hDlg)
{
	// �R���{�{�b�N�X���N���A
	while(1)
	{
		SendMessage(
				GetDlgItem(hDlg,IDC_EDIT_UNICODE),
				CB_DELETESTRING,
				0,
				0
				);
		int ret = SendMessage(GetDlgItem(hDlg,IDC_EDIT_UNICODE),CB_GETCOUNT,0,0);
		if((ret == CB_ERR) || (ret == 0))
		{
			break;
		}
	}
	// ID3v2�̃o�[�W�����ɂ���ăR���{�{�b�N�X�̓��e�����܂�
	long cur = ::SendMessage(
				GetDlgItem(hDlg,IDC_EDIT_ID3VER),
				CB_GETCURSEL,
				0,
				0);
	if(cur >= 2)	// > v2.4
	{
#ifndef	ENABLE_UTF_16BE
		char *encode[] = {"ISO-8859-1","UTF-16(Unicode)","UTF-8(Unicode)"};
#else
		char *encode[] = {"ISO-8859-1","UTF-16(Unicode)","UTF-8(Unicode)","UTF-16BE"};
#endif
		for(int i=0; i<sizeof_array(encode); i++)
		{
			SendMessage(
					GetDlgItem(hDlg,IDC_EDIT_UNICODE),
					CB_ADDSTRING,
					0,
					(LPARAM )(LPCSTR )encode[i]
					);
		}
	}
	else
	{
		char *encode[] = {"ISO-8859-1","UTF-16(Unicode)"};
		for(int i=0; i<sizeof_array(encode); i++)
		{
			SendMessage(
					GetDlgItem(hDlg,IDC_EDIT_UNICODE),
					CB_ADDSTRING,
					0,
					(LPARAM )(LPCSTR )encode[i]
					);
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
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_NAM),"");
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_NAM),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_NAM),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_TRACK),"");
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_TRACK),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_TRACK),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_ART),"");
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_ART),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_ART),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_PRD),"");
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_PRD),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_PRD),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_CRD),"");
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_CRD),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_CRD),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_GNR),"");
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_GNR),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_GNR),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_CMT),"");
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_CMT),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_CMT),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_COMP),"");
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_COMP),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_COMP),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_ORGART),"");
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_ORGART),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_ORGART),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_COPYRIGHT),"");
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_COPYRIGHT),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_COPYRIGHT),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_URL),"");
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_URL),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_URL),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_URL_JUMP),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_ENC),"");
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_ENC),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_ENC),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_ENC2),"");
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_ENC2),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_ENC2),bEnable);
//	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_ID3VER),"");
//	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_ID3VER),bEnable);
//	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_ID3VER),bEnable);
//	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_UNICODE),"");
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
		HWND hWndTab = (HWND )::SendMessage(GetParent(hDlg),PSM_GETTABCONTROL,0,0);
		TC_ITEM tcItem;
		tcItem.mask = TCIF_TEXT;
		tcItem.pszText = "ID3v2";
		::SendMessage(hWndTab,TCM_SETITEM,(WPARAM )PropSheet_PageToIndex(GetParent(hDlg),lpcs->m_hpageId3v2),(LPARAM )&tcItem);
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
		HWND hWndTab = (HWND )::SendMessage(GetParent(hDlg),PSM_GETTABCONTROL,0,0);
		TC_ITEM tcItem;
		tcItem.mask = TCIF_TEXT;
		tcItem.pszText = "ID3v2(*)";
		::SendMessage(hWndTab,TCM_SETITEM,(WPARAM )PropSheet_PageToIndex(GetParent(hDlg),lpcs->m_hpageId3v2),(LPARAM )&tcItem);
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
			SendMessage(GetDlgItem(hDlg,IDC_EDIT_ID3VER),CB_SETCURSEL,0,0);
			break;
		case 0x0300:
			SendMessage(GetDlgItem(hDlg,IDC_EDIT_ID3VER),CB_SETCURSEL,1,0);
			break;
		case 0x0400:
			SendMessage(GetDlgItem(hDlg,IDC_EDIT_ID3VER),CB_SETCURSEL,2,0);
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
		SendMessage(GetDlgItem(hDlg,IDC_EDIT_UNICODE),CB_SETCURSEL,-1,0);
		SendMessage(GetDlgItem(hDlg,IDC_EDIT_ID3VER),CB_SETCURSEL,-1,0);
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

	LPCSHELLEXT	lpcs = (CShellExt *)GetWindowLong(hDlg,DWL_USER);
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
			SetWindowLong(hDlg, DWL_USER, (DWORD )lpcs);
			lpcs->m_bId3v2Apply = FALSE;

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
					(LPARAM )(LPCSTR )""	//��
				);
			for(int i=0; i<256; i++)
			{
				if(lpcs->m_Id3tagv1.GenreNum2String(i).GetLength())
					SendMessage(
							GetDlgItem(hDlg,IDC_EDIT_GNR),
							CB_ADDSTRING,
							0,
							(LPARAM )(LPCSTR )lpcs->m_Id3tagv1.GenreNum2String(i)
						);
			}
			char *id3tags[] = {"v2.2","v2.3","v2.4"};
			for(i=0; i<sizeof_array(id3tags); i++)
			{
				SendMessage(
						GetDlgItem(hDlg,IDC_EDIT_ID3VER),
						CB_ADDSTRING,
						0,
						(LPARAM )(LPCSTR )id3tags[i]
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
				long cur = ::SendMessage(
							GetDlgItem(hDlg,IDC_EDIT_UNICODE),
							CB_GETCURSEL,
							0,
							0);
				SetEncodeCB(hDlg);	// ID3�o�[�W�����ɂ���đI���\�ȃG���R�[�h���ς��
				SelectEncodeCB(hDlg,cur);
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
					PlayWinamp(hDlg,(char *)(LPCTSTR )lpcs->m_strSelectFile);
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
					long cur = ::SendMessage(
								GetDlgItem(hDlg,IDC_EDIT_ID3VER),
								CB_GETCURSEL,
								0,
								0);
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
						break;
					}
					// Unsync/Unicode�w��
					cur = ::SendMessage(
								GetDlgItem(hDlg,IDC_EDIT_UNICODE),
								CB_GETCURSEL,
								0,
								0);
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
					lpcs->m_Id3tagv2.SetUnSynchronization(TRUE);
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
						PlayWinamp(hDlg,(char *)(LPCTSTR )lpcs->m_strSelectFile);
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
				
				//�^�C���X�^���v�𕜌�
				if(lpcs->m_bSaveTimeStamp)
				{
					lpcs->PopTimeStamp(lpcs->m_strSelectFile);
				}
				
				// Winamp���ĊJ
/*				if(nowPlaying)
				{
					PlayWinamp(hDlg,(char *)(LPCTSTR )lpcs->m_strSelectFile);
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
		case IDC_URL_JUMP:
			{
				CString strTmp;
				CWnd wnd;

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_URL));
				wnd.GetWindowText(strTmp);
				wnd.Detach();
				if(strTmp.GetLength())
				{
					ShellExecute(hDlg,"open",strTmp,NULL,NULL,SW_SHOW);
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
			regSetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"PropAOT",(DWORD )lpcs->m_bPropAOT);
			break;
		case IDC_SETUP:
			ShellExecute(hDlg,"open","rundll32.exe","shell32.dll,Control_RunDLL mp3infp.cpl,,2",NULL,SW_SHOW);
			break;
		case IDC_HELPVIEW:
			lpcs->OpenHtmlHelp(hDlg,"extension.htm");
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
		char szTmp[256];
		strcpy(szTmp,SOFT_NAME);
		strcat(szTmp," ");
		strcat(szTmp,COPY_RIGHT);
		MessageBox(hDlg,szTmp,"About",MB_APPLMODAL);
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
					SetWindowLong(hDlg,DWL_MSGRESULT,PSNRET_INVALID);
					break;
				}
				lpcs->m_bId3v2Apply = FALSE;

				long cur = ::SendMessage(
							GetDlgItem(hDlg,IDC_EDIT_ID3VER),
							CB_GETCURSEL,
							0,
							0);
				switch(cur){
				case 0:	// v2.2
					lpcs->m_Id3tagv2.SetVer(0x0200);
					break;
				case 1:	// v2.3
					lpcs->m_Id3tagv2.SetVer(0x0300);
					break;
				case 2:	// v2.4
					lpcs->m_Id3tagv2.SetVer(0x0400);
					break;
				default:
				case CB_ERR:
					break;
				}
				// Unsync/Unicode�w��
				cur = ::SendMessage(
							GetDlgItem(hDlg,IDC_EDIT_UNICODE),
							CB_GETCURSEL,
							0,
							0);
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
					PlayWinamp(hDlg,(char *)(LPCTSTR )lpcs->m_strSelectFile);
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
					SetWindowLong(hDlg,DWL_MSGRESULT,PSNRET_INVALID);
					break;
				}

				//����\��
				DispInfo(hDlg,lpcs);
				lpcs->m_bId3v2Apply = FALSE;

				SetWindowLong(hDlg,DWL_MSGRESULT,PSNRET_NOERROR);
				
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