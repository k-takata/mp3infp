#include "StdAfx.h"
#include "mp3infp.h"
#include "Resource.h"

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
	0
};

UINT CALLBACK CShellExt::PageCallback_mp3_ID3V1(HWND hWnd,UINT uMessage,LPPROPSHEETPAGE ppsp)
{
	LPCSHELLEXT	lpcs = (CShellExt *)ppsp->lParam;
	switch(uMessage){
	case PSPCB_CREATE:	//�v���p�e�B�y�[�W���쐬�������
		return TRUE;
	case PSPCB_RELEASE:	//�v���p�e�B�y�[�W���j�󂳂����
		if(lpcs) 
		{
			lpcs->m_hwndId3v1 = NULL;
			lpcs->Release();
		}
		return TRUE;
	}

	return TRUE;
}

static void EnableButton(HWND hDlg,CShellExt *lpcs)
{
	if(lpcs->m_Rmp3.IsEnable() && lpcs->m_Id3tagv1.IsEnable())
	{
		EnableWindow(GetDlgItem(hDlg,IDC_COPY_FROM_RIFF),TRUE);	//Copy from RMP
	}
	else
	{
		EnableWindow(GetDlgItem(hDlg,IDC_COPY_FROM_RIFF),FALSE);	//Copy from RMP
	}
	if(lpcs->m_Id3tagv2.IsEnable() && lpcs->m_Id3tagv1.IsEnable())
	{
		EnableWindow(GetDlgItem(hDlg,IDC_COPY_FROM_ID3V2),TRUE);	//Copy from Id3v2
	}
	else
	{
		EnableWindow(GetDlgItem(hDlg,IDC_COPY_FROM_ID3V2),FALSE);	//Copy from Id3v2
	}
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
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_CRD),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_CRD),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_CRD),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_GNR),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_GNR),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_GNR),bEnable);
	SetWindowText(GetDlgItem(hDlg,IDC_EDIT_CMT),_T(""));
	EnableWindow(GetDlgItem(hDlg,IDC_EDIT_CMT),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_CMT),bEnable);
	if(bEnable)
	{
		EnableWindow(GetDlgItem(hDlg,IDC_BUTTON_DEL_TAG),TRUE);	//Make Id3Tag
		EnableWindow(GetDlgItem(hDlg,IDC_BUTTON_MAKE_TAG),FALSE);	//Make Id3Tag
		HWND hWndTab = PropSheet_GetTabControl(GetParent(hDlg));
		TC_ITEM tcItem;
		tcItem.mask = TCIF_TEXT;
		tcItem.pszText = _T("ID3v1");
		TabCtrl_SetItem(hWndTab,PropSheet_PageToIndex(GetParent(hDlg),lpcs->m_hpageId3v1),&tcItem);
		PropSheet_RecalcPageSizes(GetParent(hDlg));
	}
	else
	{
		EnableWindow(GetDlgItem(hDlg,IDC_BUTTON_DEL_TAG),FALSE);	//Del Id3Tag
		EnableWindow(GetDlgItem(hDlg,IDC_BUTTON_MAKE_TAG),TRUE);	//Make Id3Tag
		HWND hWndTab = PropSheet_GetTabControl(GetParent(hDlg));
		TC_ITEM tcItem;
		tcItem.mask = TCIF_TEXT;
		tcItem.pszText = _T("ID3v1(*)");
		TabCtrl_SetItem(hWndTab,PropSheet_PageToIndex(GetParent(hDlg),lpcs->m_hpageId3v1),&tcItem);
		PropSheet_RecalcPageSizes(GetParent(hDlg));
	}

	EnableButton(hDlg,lpcs);
}

static void DispInfo(HWND hDlg,CShellExt *lpcs)
{
	//mpeg���
	SetDlgItemText(hDlg,IDC_STATIC_FORMAT,lpcs->m_mp3info.GetFormatString());
	SetDlgItemText(hDlg,IDC_STATIC_TIME,lpcs->m_mp3info.GetTimeString());

	if(lpcs->m_Id3tagv1.IsEnable())
	{
		EnableEdit(hDlg,lpcs,TRUE);

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
		//�R�����g
		SetDlgItemText(hDlg,IDC_EDIT_CMT,lpcs->m_Id3tagv1.GetComment());
		//�W������
		ComboBox_SelectString(GetDlgItem(hDlg,IDC_EDIT_GNR),
					0, lpcs->m_Id3tagv1.GetGenre());
		lpcs->m_bId3v1Apply = FALSE;
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

void CShellExt::DrawControl(HWND hWnd,LPDRAWITEMSTRUCT lpInfo)
{
	HBITMAP	hBitmap,hOldBitmap;
	int		iRcId;
	HDC		hMemDC;

	//�I�[�i�[�`��{�^���ȊO�A�͏��O
	if(lpInfo->CtlType != ODT_BUTTON)
		return;

	// �{�^���C���[�W�r�b�g�}�b�v�����[�h
	switch(lpInfo->CtlID){
	case IDC_BUTTON_PLAY:
		iRcId = IDB_PLAY;
		break;
	case IDC_BUTTON_STOP:
		iRcId = IDB_STOP;
		break;
	case IDC_BUTTON_PAUSE:
		iRcId = IDB_PAUSE;
		break;
	case IDC_BUTTON_REW:
		iRcId = IDB_REW;
		//�^�C�}�[�̐����E�j��
		if(lpInfo->itemState & ODS_SELECTED)
		{
			SetTimer(hWnd,REW_TIMER,350,NULL);
		}
		else
		{
			KillTimer(hWnd,REW_TIMER);
		}
		break;
	case IDC_BUTTON_FFW:
		iRcId = IDB_FFW;
		//�^�C�}�[�̐����E�j��
		if(lpInfo->itemState & ODS_SELECTED)
		{
			SetTimer(hWnd,FFW_TIMER,350,NULL);
		}else
		{
			KillTimer(hWnd,FFW_TIMER);
		}
		break;
	default:
		return;
	}
	
	if(hBitmap = LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(iRcId)))
	{
		RECT rcFocus;

		hMemDC = CreateCompatibleDC(lpInfo->hDC);
		hOldBitmap = (HBITMAP )SelectObject(hMemDC,hBitmap);
		HBRUSH hBrush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));

		FillRect(lpInfo->hDC,&(lpInfo->rcItem),hBrush);	//�N���A
		if(lpInfo->itemState & ODS_SELECTED)
		{	//�������񂾏��
			BitBlt(lpInfo->hDC,
				(lpInfo->rcItem).left+1,
				(lpInfo->rcItem).top+1,
				(lpInfo->rcItem).right-(lpInfo->rcItem).left,
				(lpInfo->rcItem).bottom-(lpInfo->rcItem).top,
				hMemDC,0,0,SRCAND);
			//�E
			int i;
			for(i=(lpInfo->rcItem).top; i<(lpInfo->rcItem).bottom; i++)
				SetPixel(lpInfo->hDC,(lpInfo->rcItem).right-1,i,GetSysColor(COLOR_3DHILIGHT));
			//��
			for(i=(lpInfo->rcItem).left; i<(lpInfo->rcItem).right; i++)
				SetPixel(lpInfo->hDC,i,(lpInfo->rcItem).bottom-1,GetSysColor(COLOR_3DHILIGHT));
			//��
			for(i=(lpInfo->rcItem).top; i<(lpInfo->rcItem).bottom; i++)
				SetPixel(lpInfo->hDC,(lpInfo->rcItem).left,i,GetSysColor(COLOR_3DSHADOW));
			//��
			for(i=(lpInfo->rcItem).left; i<(lpInfo->rcItem).right; i++)
				SetPixel(lpInfo->hDC,i,(lpInfo->rcItem).top,GetSysColor(COLOR_3DSHADOW));
			//�t�H�[�J�X�g���쐬
			if(lpInfo->itemState & ODS_FOCUS)
			{
				SetRect(&rcFocus,
					(lpInfo->rcItem).left+4,
					(lpInfo->rcItem).top+4,
					(lpInfo->rcItem).right-2,
					(lpInfo->rcItem).bottom-2);
				DrawFocusRect(lpInfo->hDC,&rcFocus);
			}
		}
		else
		{
			BitBlt(lpInfo->hDC,
				(lpInfo->rcItem).left,
				(lpInfo->rcItem).top,
				(lpInfo->rcItem).right-(lpInfo->rcItem).left,
				(lpInfo->rcItem).bottom-(lpInfo->rcItem).top,
				hMemDC,0,0,SRCAND);
			int i;
			//�E
			for(i=(lpInfo->rcItem).top; i<(lpInfo->rcItem).bottom; i++)
				SetPixel(lpInfo->hDC,(lpInfo->rcItem).right-1,i,GetSysColor(COLOR_3DSHADOW));
			//��
			for(i=(lpInfo->rcItem).left; i<(lpInfo->rcItem).right; i++)
				SetPixel(lpInfo->hDC,i,(lpInfo->rcItem).bottom-1,GetSysColor(COLOR_3DSHADOW));
			//��
			for(i=(lpInfo->rcItem).top; i<(lpInfo->rcItem).bottom; i++)
				SetPixel(lpInfo->hDC,(lpInfo->rcItem).left,i,GetSysColor(COLOR_3DHILIGHT));
			//��
			for(i=(lpInfo->rcItem).left; i<(lpInfo->rcItem).right; i++)
				SetPixel(lpInfo->hDC,i,(lpInfo->rcItem).top,GetSysColor(COLOR_3DHILIGHT));
			//�t�H�[�J�X�g���쐬
			if(lpInfo->itemState & ODS_FOCUS)
			{
				SetRect(&rcFocus,
					(lpInfo->rcItem).left+3,
					(lpInfo->rcItem).top+3,
					(lpInfo->rcItem).right-3,
					(lpInfo->rcItem).bottom-3);
				DrawFocusRect(lpInfo->hDC,&rcFocus);
			}
		}
		DeleteObject(hBrush);
		SelectObject(hMemDC, hOldBitmap);
		DeleteObject(hBitmap);
		DeleteDC(hMemDC);
	}
}

BOOL CALLBACK CShellExt::PageDlgProc_mp3_ID3V1(HWND hDlg,UINT uMessage,WPARAM wParam,LPARAM lParam)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	LPCSHELLEXT	lpcs = (CShellExt *)GetWindowLongPtr(hDlg,DWLP_USER);
	CString strTmp;
	int id3maxline = 30;

	switch(uMessage){
	case WM_USER+1:	//�\���̃��t���b�V��
		EnableButton(hDlg,lpcs);
		DispInfo(hDlg,lpcs);
		lpcs->m_bId3v1Apply = FALSE;
		break;
	case WM_INITDIALOG:
		{
			lpcs = (LPCSHELLEXT )((LPPROPSHEETPAGE )lParam)->lParam;
			lpcs->m_hwndId3v1 = hDlg;
			SetWindowLongPtr(hDlg, DWLP_USER, (LONG_PTR)lpcs);
			lpcs->m_bId3v1Apply = FALSE;

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
/*			RECT rect,rect2;
			SetRect(&rect,145,208,145,75);//�{�^���ʒu�̊
			MapDialogRect(hDlg,&rect);
			SetWindowPos(GetDlgItem(hDlg,IDC_BUTTON_REW),NULL,rect.left+25*0,rect.top,23,23,SWP_NOZORDER | SWP_SHOWWINDOW);
			GetWindowRect(GetDlgItem(hDlg,IDC_STATIC_WINAMP),&rect2);
			TRACE(_T("GetDlgItem(rect.left,rect.top,rect.bottom,rect.right)=%d,%d,%d,%d\n"),rect2.left,rect2.top,rect2.bottom,rect2.right);
			SetWindowPos(GetDlgItem(hDlg,IDC_BUTTON_PLAY),NULL,rect.left+25*1,rect.top,23,23,SWP_NOZORDER | SWP_SHOWWINDOW);
			SetWindowPos(GetDlgItem(hDlg,IDC_BUTTON_PAUSE),NULL,rect.left+25*2,rect.top,23,23,SWP_NOZORDER | SWP_SHOWWINDOW);
			SetWindowPos(GetDlgItem(hDlg,IDC_BUTTON_STOP),NULL,rect.left+25*3,rect.top,23,23,SWP_NOZORDER | SWP_SHOWWINDOW);
			SetWindowPos(GetDlgItem(hDlg,IDC_BUTTON_FFW),NULL,rect.left+25*4,rect.top,23,23,SWP_NOZORDER | SWP_SHOWWINDOW);
*/			//����\��
			DispInfo(hDlg,lpcs);
			lpcs->m_bId3v1Apply = FALSE;

			break;
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

	case WM_CTLCOLOREDIT:
	{
		HBRUSH brush =  (HBRUSH )DefWindowProc(hDlg,uMessage,wParam,lParam);
		HDC  hdcStatic = (HDC )wParam;
		HWND hwndStatic = (HWND )lParam;
		int len = Edit_LineLength(hwndStatic,0);
		BOOL bColor = FALSE;
		switch(GetDlgCtrlID((HWND )lParam)){
		case IDC_EDIT_CRD:
			if(len > 4) bColor = TRUE;
			break;
		case IDC_EDIT_NAM:
		case IDC_EDIT_ART:
		case IDC_EDIT_PRD:
			if(len > 30) bColor = TRUE;
			break;
		case IDC_EDIT_CMT:
			if(!lpcs->m_Id3tagv1.GetTrackNo().IsEmpty())
			{
				if(len > 28) bColor = TRUE;
			}
			else
			{
				if(len > 30) bColor = TRUE;
			}
			break;
		default:
			break;
		}
		if(bColor)
		{
			SetTextColor(hdcStatic,RGB(255,0,0));
		}
		return (int )brush;
	}

	case WM_COMMAND:
		id3maxline = 30;
		switch(LOWORD(wParam)){
		case IDC_EDIT_CRD:
			id3maxline = 4;
		case IDC_EDIT_NAM:
		case IDC_EDIT_ART:
		case IDC_EDIT_PRD:
		case IDC_EDIT_CMT:
			InvalidateRect(GetDlgItem(hDlg,LOWORD(wParam)),NULL,FALSE);
			if((LOWORD(wParam) == IDC_EDIT_CMT) && (!lpcs->m_Id3tagv1.GetTrackNo().IsEmpty()))
				id3maxline = 28;
			//ID3TAG���͕������K�C�h
			{
				CString strFormatText;
				char szTmp[MAX_PATH];
				GetWindowTextA(GetDlgItem(hDlg,LOWORD(wParam)), szTmp, MAX_PATH);	// �o�C�g�����擾
				int len = lstrlenA(szTmp);
				if(lpcs->m_Id3tagv1.GetTrackNo().IsEmpty())
				{
					if(id3maxline < len)
					{
						//�u���p%d����(%d�����I�[�o�[) [max=%d (for ID3v1)]�v
						strFormatText.Format(IDS_PAGE_MP3ID3V1_COUNT_OVER,len,len-id3maxline,id3maxline);
					}
					else
					{
						//�u���p%d���� [max=%d (for ID3v1)]�v
						strFormatText.Format(IDS_PAGE_MP3ID3V1_COUNT,len,id3maxline);
					}
				}
				else
				{
					if(id3maxline < len)
					{
						//�u���p%d����(%d�����I�[�o�[) [max=%d (for ID3v1.1)]�v
						strFormatText.Format(IDS_PAGE_MP3ID3V11_COUNT_OVER,len,len-id3maxline,id3maxline);
					}
					else
					{
						//�u���p%d���� [max=%d (for ID3v1.1)]�v
						strFormatText.Format(IDS_PAGE_MP3ID3V11_COUNT,len,id3maxline);
					}
				}
				SetWindowText(GetDlgItem(hDlg,IDC_STATIC_ID3GUIDE),strFormatText);
			}
			if( (HIWORD(wParam) == EN_CHANGE) &&
				(IsWindowEnabled((HWND )lParam)) )
			{
				PropSheet_Changed(GetParent(hDlg),hDlg);
				lpcs->m_bId3v1Apply = TRUE;
			}
			break;
		case IDC_EDIT_TRACK:
			SetWindowText(GetDlgItem(hDlg,IDC_STATIC_ID3GUIDE),_T(""));
			if( (HIWORD(wParam) == EN_CHANGE) &&
				(IsWindowEnabled((HWND )lParam)) )
			{
				PropSheet_Changed(GetParent(hDlg),hDlg);
				lpcs->m_bId3v1Apply = TRUE;
			}
			break;
		case IDC_EDIT_GNR:
			SetWindowText(GetDlgItem(hDlg,IDC_STATIC_ID3GUIDE),_T(""));
			if( ((HIWORD(wParam) == CBN_SELENDOK) || (HIWORD(wParam) == CBN_EDITCHANGE)) &&
				(IsWindowEnabled((HWND )lParam)) )
			{
				PropSheet_Changed(GetParent(hDlg),hDlg);
				lpcs->m_bId3v1Apply = TRUE;
			}
			break;
		case IDC_BUTTON_DEL_TAG:
			//�uID3v1 TAG���폜���܂��B��낵���ł����H�v
			strTmp.Format(IDS_PAGE_MP3ID3V1_DEL_ID3TAG);
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
				//ID3TAG������
				if(lpcs->m_Rmp3.IsEnable())
				{
					lpcs->m_Rmp3.SetHasId3tag(FALSE);
					DWORD dwRet = lpcs->m_Rmp3.Save(hDlg,lpcs->m_strSelectFile);
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
				}
				else
				{
					DWORD dwRet = lpcs->m_Id3tagv1.DelTag(hDlg,lpcs->m_strSelectFile);
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
				}
				//�^�C���X�^���v�𕜌�
				if(lpcs->m_bSaveTimeStamp)
				{
					lpcs->PopTimeStamp(lpcs->m_strSelectFile);
				}
				//����\��
				DispInfo(hDlg,lpcs);
				lpcs->m_bId3v1Apply = FALSE;

				//�V�F���ɕύX��ʒm
				SHChangeNotify(SHCNE_UPDATEITEM,SHCNF_FLUSH|SHCNF_PATH,lpcs->m_strSelectFile,NULL);
				//���̌`���^�u���X�V
				if(lpcs->m_hwndId3v2)
					PostMessage(lpcs->m_hwndId3v2,WM_USER+1,0,0);
				if(lpcs->m_hwndMp3Ape)
					PostMessage(lpcs->m_hwndMp3Ape,WM_USER+1,0,0);
				if(lpcs->m_hwndRiffSIF)
					PostMessage(lpcs->m_hwndRiffSIF,WM_USER+1,0,0);
			}
			break;
		case IDC_BUTTON_MAKE_TAG:
			//�uID3 TAG���쐬���܂��B��낵���ł����H�v
			strTmp.Format(IDS_PAGE_MP3ID3V1_MAKE_ID3TAG);
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
				//ID3TAG���쐬
				if(lpcs->m_Rmp3.IsEnable())
				{
					lpcs->m_Rmp3.SetHasId3tag(TRUE);
					DWORD dwRet = lpcs->m_Rmp3.Save(hDlg,lpcs->m_strSelectFile);
					if(dwRet == -1)
					{
						lpcs->m_Rmp3.SetHasId3tag(FALSE);
						//�u�t�@�C���𐳂����X�V�ł��܂���ł����B�v
						AfxMessageBox(IDS_PAGE_BROKEN,MB_ICONINFORMATION);
						break;
					}
					else if(dwRet != ERROR_SUCCESS)
					{
						lpcs->m_Rmp3.SetHasId3tag(FALSE);
						//�V�X�e���G���[��\��
						errMessageBox(hDlg,dwRet);
						break;
					}
					lpcs->m_Id3tagv1.Load(lpcs->m_strSelectFile);
				}
				else
				{
					DWORD dwRet = lpcs->m_Id3tagv1.MakeTag(hDlg,lpcs->m_strSelectFile);
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
				}
				//�^�C���X�^���v�𕜌�
				if(lpcs->m_bSaveTimeStamp)
				{
					lpcs->PopTimeStamp(lpcs->m_strSelectFile);
				}
				//����\��
				DispInfo(hDlg,lpcs);
				lpcs->m_bId3v1Apply = FALSE;
				// �^�C�g���G�f�B�b�g�{�b�N�X�Ƀt�@�C������}������
				// (ID3v1�̐����ŃJ�b�g����Ă��܂�����)
				SetDlgItemText(hDlg,IDC_EDIT_NAM,getFileName(lpcs->m_strSelectFile));

				//�V�F���ɕύX��ʒm
				SHChangeNotify(SHCNE_UPDATEITEM,SHCNF_FLUSH|SHCNF_PATH,lpcs->m_strSelectFile,NULL);
				//���̌`���^�u���X�V
				if(lpcs->m_hwndId3v2)
					PostMessage(lpcs->m_hwndId3v2,WM_USER+1,0,0);
				if(lpcs->m_hwndMp3Ape)
					PostMessage(lpcs->m_hwndMp3Ape,WM_USER+1,0,0);
				if(lpcs->m_hwndRiffSIF)
					PostMessage(lpcs->m_hwndRiffSIF,WM_USER+1,0,0);
			}
			break;
		case IDC_COPY_FROM_ID3V2:
			SetWindowText(GetDlgItem(hDlg,IDC_EDIT_NAM),lpcs->m_Id3tagv2.GetTitle());
			SetWindowText(GetDlgItem(hDlg,IDC_EDIT_TRACK),lpcs->m_Id3tagv2.GetTrackNo());
			SetWindowText(GetDlgItem(hDlg,IDC_EDIT_ART),lpcs->m_Id3tagv2.GetArtist());
			SetWindowText(GetDlgItem(hDlg,IDC_EDIT_PRD),lpcs->m_Id3tagv2.GetAlbum());
			SetWindowText(GetDlgItem(hDlg,IDC_EDIT_CRD),lpcs->m_Id3tagv2.GetYear());
			ComboBox_SelectString(GetDlgItem(hDlg,IDC_EDIT_GNR), 0, lpcs->m_Id3tagv2.GetGenre());
			SetWindowText(GetDlgItem(hDlg,IDC_EDIT_CMT),lpcs->m_Id3tagv2.GetComment());
			break;
		case IDC_COPY_FROM_RIFF:
			SetWindowText(GetDlgItem(hDlg,IDC_EDIT_NAM),lpcs->m_Rmp3.GetNAM());
			SetWindowText(GetDlgItem(hDlg,IDC_EDIT_TRACK),_T(""));
			SetWindowText(GetDlgItem(hDlg,IDC_EDIT_ART),lpcs->m_Rmp3.GetART());
			SetWindowText(GetDlgItem(hDlg,IDC_EDIT_PRD),lpcs->m_Rmp3.GetPRD());
			SetWindowText(GetDlgItem(hDlg,IDC_EDIT_CRD),lpcs->m_Rmp3.GetCRD());
			ComboBox_SelectString(GetDlgItem(hDlg,IDC_EDIT_GNR), 0, lpcs->m_Rmp3.GetGNR());
			SetWindowText(GetDlgItem(hDlg,IDC_EDIT_CMT),lpcs->m_Rmp3.GetCMT());
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
			if(lpcs->m_hwndId3v2)
				PostMessage(lpcs->m_hwndId3v2,WM_USER+1,0,0);
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
		DlgContextMenu(hDlg,lParam,lpcs->m_Id3tagv1.IsEnable());
		break;
	//�󋵈ˑ��w���v
	case WM_HELP:
	{
		TCHAR szTmp[256];
		lstrcpy(szTmp,APP_NAME);
		lstrcat(szTmp,_T(" "));
		lstrcat(szTmp,COPY_RIGHT);
		MessageBox(GetParent(hDlg),szTmp,_T("About"),MB_APPLMODAL);
		break;
	}
	case WM_NOTIFY:
		switch(((NMHDR FAR *)lParam)->code){
//		case PSN_SETACTIVE:
//			break;
		case PSN_APPLY:
			//�ۑ�
			if(lpcs->m_bId3v1Apply && lpcs->m_Id3tagv1.IsEnable())
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
				lpcs->m_bId3v1Apply = FALSE;

				CString strTmp;
				CWnd wnd;
				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_NAM));
				wnd.GetWindowText(strTmp);
				lpcs->m_Id3tagv1.SetTitle(strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_TRACK));
				wnd.GetWindowText(strTmp);
				lpcs->m_Id3tagv1.SetTrackNo(strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_ART));
				wnd.GetWindowText(strTmp);
				lpcs->m_Id3tagv1.SetArtist(strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_PRD));
				wnd.GetWindowText(strTmp);
				lpcs->m_Id3tagv1.SetAlbum(strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_CRD));
				wnd.GetWindowText(strTmp);
				lpcs->m_Id3tagv1.SetYear(strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_GNR));
				wnd.GetWindowText(strTmp);
				lpcs->m_Id3tagv1.SetGenre(strTmp);
				wnd.Detach();

				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_CMT));
				wnd.GetWindowText(strTmp);
				lpcs->m_Id3tagv1.SetComment(strTmp);
				wnd.Detach();

				//�^�C���X�^���v��ۑ�
				lpcs->PushTimeStamp(lpcs->m_strSelectFile);
				
				DWORD dwRet = lpcs->m_Id3tagv1.Save(hDlg,lpcs->m_strSelectFile);
				if(dwRet != ERROR_SUCCESS)
				{
					lpcs->m_bId3v1Apply = TRUE;
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
				lpcs->m_bId3v1Apply = FALSE;

				SetWindowLongPtr(hDlg,DWLP_MSGRESULT,PSNRET_NOERROR);
				
				//�V�F���ɕύX��ʒm
				SHChangeNotify(SHCNE_UPDATEITEM,SHCNF_FLUSH|SHCNF_PATH,lpcs->m_strSelectFile,NULL);
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
