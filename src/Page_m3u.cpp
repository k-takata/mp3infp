#include "StdAfx.h"
#include "mp3infp.h"

UINT CALLBACK CShellExt::PageCallback_m3u(HWND hWnd,UINT uMessage,LPPROPSHEETPAGE ppsp)
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

static void DispInfo(HWND hDlg,CShellExt *lpcs)
{
	TRACE("DispInfo()\n");

	TRACE("DispInfo() - Load\n");
	int i = 0;
	CString strTmp;
	CString strEdit;
	while(lpcs->m_M3u.GetLine(i++,strTmp))
	{
		strEdit += strTmp;
		strEdit += "\r\n";
	}
	SetDlgItemText(hDlg,IDC_EDIT_LIST,(char *)(LPCSTR )strEdit);

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

BOOL CALLBACK CShellExt::PageDlgProc_m3u(HWND hDlg,UINT uMessage,WPARAM wParam,LPARAM lParam)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	LPCSHELLEXT	lpcs = (CShellExt *)GetWindowLong(hDlg,DWL_USER);
	CString strTmp;
	HANDLE hFile;

	switch(uMessage){
	case WM_INITDIALOG:
		{
			lpcs = (LPCSHELLEXT )((LPPROPSHEETPAGE )lParam)->lParam;
			lpcs->m_hwndPage = hDlg;
			SetWindowLong(hDlg, DWL_USER, (DWORD )lpcs);

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

			//����\��
			DispInfo(hDlg,lpcs);
			lpcs->m_bApply = FALSE;

			//�X�C�b�`��������
			switch(lpcs->m_bM3uProp_Fullpath){
			case 0:
				CheckRadioButton(hDlg,IDC_RADIO_PATH1,IDC_RADIO_PATH3,IDC_RADIO_PATH1);
				break;
			case 1:
				CheckRadioButton(hDlg,IDC_RADIO_PATH1,IDC_RADIO_PATH3,IDC_RADIO_PATH2);
				break;
			case 2:
			default:
				CheckRadioButton(hDlg,IDC_RADIO_PATH1,IDC_RADIO_PATH3,IDC_RADIO_PATH3);
				break;
			}
			DragAcceptFiles(hDlg,TRUE);//Drag&Drop on
		}
		break;

	//�t�@�C�����h���b�v���ꂽ�Ƃ��̏���
	case WM_DROPFILES:
	{
		CString strTmp;
		CWnd wnd;
		wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_LIST));
		wnd.GetWindowText(strTmp);
		wnd.Detach();

		char szFile[MAX_PATH];
		//�h���b�v���ꂽ�t�@�C����
		UINT iFileNum=DragQueryFile((HDROP )wParam,-1,NULL,0);
		for(UINT i=0; i<iFileNum; i++)
		{
			DragQueryFile((HDROP )wParam,i,szFile,MAX_PATH);
			switch(lpcs->m_bM3uProp_Fullpath){
			case 0:	// ���΃p�X�Œǉ�
				if(strncmp(szFile,lpcs->m_strSelectFile,3) == 0)
				{
					// ���΃p�X�����
					int offset = 0;
					int m3uLen = strlen(lpcs->m_strSelectFile);
					int mp3Len = strlen(szFile);
					for(int j=0; j<((m3uLen<mp3Len)?m3uLen:mp3Len); j++)
					{
						if(IsDBCSLeadByte(lpcs->m_strSelectFile[j]) && IsDBCSLeadByte(szFile[j]))
						{
							j++;
							continue;
						}
						if(IsDBCSLeadByte(lpcs->m_strSelectFile[j]) || IsDBCSLeadByte(szFile[j]))
						{
							break;
						}
						if(lpcs->m_strSelectFile[j] != szFile[j])
						{
							break;
						}
						if(lpcs->m_strSelectFile[j] == '\\')
						{
							offset = j+1;
						}
					}
					int yenCount = 0;
					unsigned char *yenPtr = (unsigned char *)&(((LPCTSTR )lpcs->m_strSelectFile)[offset]);
					while(1)
					{
						yenPtr = _mbschr((unsigned char *)yenPtr+1,'\\');
						if(yenPtr == NULL)
						{
							break;
						}
						yenCount++;
						strTmp += "..\\";
					}
					strTmp += &szFile[offset];
				}
				else
				{
					// ���΃p�X�ɂł��Ȃ�(�h���C�u���Ⴄ)
					strTmp += szFile;
				}
				break;
			case 1:	// �t���p�X�Œǉ�
				strTmp += szFile;
				break;
			case 2:	// �t�@�C�����Œǉ�
			default:
				strTmp += (char *)getFileName(szFile);
				break;
			}
			strTmp += "\r\n";
		}
		DragFinish((HDROP )wParam);

		//�ύX���G�f�B�b�g�R���g���[���ɔ��f
		SetWindowText(GetDlgItem(hDlg,IDC_EDIT_LIST),strTmp);
		PropSheet_Changed(GetParent(hDlg),hDlg);
		
		SendMessage(GetDlgItem(hDlg,IDC_EDIT_LIST),EM_SETMODIFY,TRUE,0);
		// �J�[�\���ʒu���I�[�ɃZ�b�g
		SendMessage(GetDlgItem(hDlg,IDC_EDIT_LIST),EM_SETSEL,strTmp.GetLength(),strTmp.GetLength());
		break;
	}

	case WM_COMMAND:
	switch(LOWORD(wParam)){
		case IDC_EDIT_LIST:
			if((HIWORD(wParam) == EN_CHANGE) &&
				(IsWindowEnabled((HWND )lParam)) )
			{
				PropSheet_Changed(GetParent(hDlg),hDlg);
			}
			break;
		case IDC_RADIO_PATH1:
		case IDC_RADIO_PATH2:
		case IDC_RADIO_PATH3:
			if(IsDlgButtonChecked(hDlg,IDC_RADIO_PATH1) == BST_CHECKED)
			{
				lpcs->m_bM3uProp_Fullpath = 0;
			}
			else if(IsDlgButtonChecked(hDlg,IDC_RADIO_PATH2) == BST_CHECKED)
			{
				lpcs->m_bM3uProp_Fullpath = 1;
			}
			else if(IsDlgButtonChecked(hDlg,IDC_RADIO_PATH3) == BST_CHECKED)
			{
				lpcs->m_bM3uProp_Fullpath = 2;
			}
			regSetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"m3u_Prop_Fullpath",(DWORD )lpcs->m_bM3uProp_Fullpath);
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
			regSetDword(HKEY_CURRENT_USER,MP3INFP_REG_ENTRY,"PropAOT",(DWORD )lpcs->m_bPropAOT);
			break;
		case IDC_SETUP:
			ShellExecute(hDlg,"open","rundll32.exe","shell32.dll,Control_RunDLL mp3infp.cpl,,7",NULL,SW_SHOW);
			break;
		case IDC_HELPVIEW:
			lpcs->OpenHtmlHelp(hDlg,"extension.htm");
			break;
		}
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
			TRACE("WM_NOTIFY(PSN_APPLY)\n");
			if(SendMessage(GetDlgItem(hDlg,IDC_EDIT_LIST),EM_GETMODIFY,0,0))
			{
				TRACE("WM_NOTIFY(PSN_APPLY) - �ۑ�\n");
				//�t�@�C�����������݉\�����ׂ�
				if(GetFileAttributes(lpcs->m_strSelectFile) & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY))
				{
					//�u���̃t�@�C���́u�ǂݍ��ݐ�p�v�ł��v
					AfxMessageBox(IDS_PAGE_THIS_READONLY,MB_ICONINFORMATION);
					//�K�p�{�^���͈��������L��
					SetWindowLong(hDlg,DWL_MSGRESULT,PSNRET_INVALID);
					break;
				}

				CString strTmp,strLine;
				CWnd wnd;
				wnd.Attach(GetDlgItem(hDlg,IDC_EDIT_LIST));
				wnd.GetWindowText(strTmp);
				wnd.Detach();

				lpcs->m_M3u.Release();
				//�I�[�ɉ��s������
				if( (strTmp.GetLength() > 1) &&
					(strcmp((char *)(LPCSTR )strTmp+(strTmp.GetLength()-2),"\r\n") != 0) )
				{
					strTmp += "\r\n";
				}
				int start = 0;
				int end = 0;
				while(1)
				{
					end = strTmp.Find("\r\n",start);
					if(end == -1)
					{
						break;
					}
					strLine = CString((char *)(LPCSTR )strTmp+start,end-start);
					lpcs->m_M3u.Add((char *)(LPCSTR )strLine);
					TRACE("lpcs->m_M3u.Add(%s)\n",strLine);
					
					start = end + 2;
				}

				//�^�C���X�^���v��ۑ�
				hFile = CreateFile(
							lpcs->m_strSelectFile,
							GENERIC_READ,
							FILE_SHARE_READ|FILE_SHARE_WRITE,
							NULL,
							OPEN_EXISTING,	//�t�@�C�����I�[�v�����܂��B�w��t�@�C�������݂��Ă��Ȃ��ꍇ�A�֐��͎��s���܂��B
							FILE_ATTRIBUTE_NORMAL,
							NULL);
				if(hFile != INVALID_HANDLE_VALUE)
				{
					GetFileTime(hFile,&lpcs->m_createTime,NULL,&lpcs->m_fileTime);
					CloseHandle(hFile);
				}

				DWORD dwRet = lpcs->m_M3u.Save(lpcs->m_strSelectFile);
				if(dwRet != ERROR_SUCCESS)
				{
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

				if(lpcs->m_bSaveTimeStamp)
				{
					//�^�C���X�^���v�𕜌�
					hFile = CreateFile(
								lpcs->m_strSelectFile,
								GENERIC_READ|GENERIC_WRITE,
								FILE_SHARE_READ|FILE_SHARE_WRITE,
								NULL,
								OPEN_EXISTING,	//�t�@�C�����I�[�v�����܂��B�w��t�@�C�������݂��Ă��Ȃ��ꍇ�A�֐��͎��s���܂��B
								FILE_ATTRIBUTE_NORMAL,
								NULL);
					if(hFile != INVALID_HANDLE_VALUE)
					{
						SetFileTime(hFile,&lpcs->m_createTime,NULL,&lpcs->m_fileTime);
						CloseHandle(hFile);
					}
				}

				//����\��
				DispInfo(hDlg,lpcs);
				lpcs->m_bApply = FALSE;

				SetWindowLong(hDlg,DWL_MSGRESULT,PSNRET_NOERROR);
				SendMessage(GetDlgItem(hDlg,IDC_EDIT_LIST),EM_SETMODIFY,FALSE,0);

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