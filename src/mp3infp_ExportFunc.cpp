#include "stdafx.h"
#include "mp3infp.h"
#include "GlobalCommand.h"

extern "C" DWORD __stdcall mp3infp_GetVer()
{
	return 0x1000*(MAJOR_REL/10)+0x0100*(MAJOR_REL%10)+0x0010*(MINOR_REL/10)+0x0001*(MINOR_REL%10);
}

static CString getFileType(LPCTSTR szFileName,DWORD dwPage)
{
	CString ret;

	if((lstrcmpi(getExtName(szFileName),_T(".mp3")) == 0) ||
		(lstrcmpi(getExtName(szFileName),_T(".mp2")) == 0) ||
		(lstrcmpi(getExtName(szFileName),_T(".mp1")) == 0) ||
		(lstrcmpi(getExtName(szFileName),_T(".rmp")) == 0) )
	{
		BOOL bId3v1 = FALSE;
		BOOL bId3v2 = FALSE;
		BOOL bRiffSIF = FALSE;
		BOOL bApe = FALSE;

		DWORD dwRet;
		HANDLE hFile = CreateFile(
					szFileName,
					GENERIC_READ,
					FILE_SHARE_READ,
					NULL,
					OPEN_EXISTING,			//指定したファイルが存在していない場合、この関数は失敗します。 
					FILE_ATTRIBUTE_NORMAL,
					NULL);
		if(hFile == INVALID_HANDLE_VALUE)
		{
			return _T("");
		}
		while(1)	//ダミー
		{
			//id3v2タグを捜す
			char buf[16];
			if(!ReadFile(hFile,&buf,16,&dwRet,NULL))
			{
				CloseHandle(hFile);
				return _T("");
			}
			if((dwRet >= 10) && (memcmp(buf,"ID3",3) == 0))
			{
				bId3v2 = TRUE;
				break;
			}
			buf[4] = '_';
			buf[5] = '_';
			buf[6] = '_';
			buf[7] = '_';
			if((dwRet == 16) && (memcmp(buf,"RIFF____RMP3data",16) == 0))
			{
				bRiffSIF = TRUE;
				break;
			}
			//id3tagタグを捜す
			SetFilePointer(hFile,-128,NULL,FILE_END);
			char id3tag[128];
			if(!ReadFile(hFile,&id3tag,sizeof(id3tag),&dwRet,NULL))
			{
				CloseHandle(hFile);
				return _T("");
			}
			if((dwRet == sizeof(id3tag)) && (memcmp(id3tag,"TAG",3) == 0))
			{
				bId3v1 = TRUE;
			}
			//Apeタグを捜す
			typedef struct _APE_TAG_FOOTER
			{
				char id[8];		// "APETAGEX"
				int version;		// タグバージョン
				int size;			// このフッタを含むタグサイズ
				int fields;			// タグに含まれるフィールド数
				int flags;			// フラグ
				char reserved[8];	// 予約
			}APE_TAG_FOOTER;
			APE_TAG_FOOTER footer;
			// apeフッタを探す1
			SetFilePointer(hFile,-sizeof(APE_TAG_FOOTER)-(bId3v1?128:0),NULL,FILE_END);
			// APE_TAG_FOOTERを読みとる
			if(!ReadFile(hFile,&footer,sizeof(footer),&dwRet,NULL) || (dwRet != sizeof(footer)))
			{
				CloseHandle(hFile);
				return _T("");
			}
			if((strncmp(footer.id,"APETAGEX",8) == 0) &&
				(footer.version <= 2000) &&
				(footer.fields < 65536) &&
				(footer.size < (1024 * 1024 * 16))
				)
			{
				bApe = TRUE;
			}
			// apeフッタを探す2
			SetFilePointer(hFile,-sizeof(APE_TAG_FOOTER)-128,NULL,FILE_END);
			// APE_TAG_FOOTERを読みとる
			if(!ReadFile(hFile,&footer,sizeof(footer),&dwRet,NULL) || (dwRet != sizeof(footer)))
			{
				CloseHandle(hFile);
				return _T("");
			}
			if((strncmp(footer.id,"APETAGEX",8) == 0) &&
				(footer.version <= 2000) &&
				(footer.fields < 65536) &&
				(footer.size < (1024 * 1024 * 16))
				)
			{
				bApe = TRUE;
			}
			break;
		}
		CloseHandle(hFile);

		if(bId3v2)
		{
			ret= _T("ID3v2");
		}
		else if(bApe)
		{
			ret= _T("APE");
		}
		else if(bRiffSIF)
		{
			ret= _T("Riff SIF");
		}
		else if(bId3v1)
		{
			ret= _T("ID3v1");
		}
		else
		{
			switch(dwPage){
			default:
			case 0:
				ret= _T("ID3v1(*)");
				break;
			case 1:
				ret= _T("ID3v2(*)");
				break;
			case 2:
				ret= _T("Riff SIF(*)");
				break;
			case 3:
				ret= _T("APE(*)");
				break;
			}
		}
	}

	return ret;
}

#ifdef UNICODE
extern "C" int __stdcall mp3infp_ViewPropExA(HWND hWnd,const char *szFileName,DWORD dwPage,BOOL modeless,DWORD param1,DWORD param2)
{
	return mp3infp_ViewPropExW(hWnd,CString(szFileName),dwPage,modeless,param1,param2);
}
#endif

extern "C" int __stdcall mp3infp_ViewPropEx(HWND hWnd,LPCTSTR szFileName,DWORD dwPage,BOOL modeless,DWORD param1,DWORD param2)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	CShellExt se;
	se.Initialize(szFileName);
	se.AddPages(se.AddPropSheetPageProc,(DWORD )&se);

	if(se.m_tmpPropCount == 0)
	{
		return FALSE;
	}

	PROPSHEETHEADER psh;
	psh.dwSize = sizeof(PROPSHEETHEADER);
	psh.dwFlags = PSH_USEPSTARTPAGE|(modeless?PSH_MODELESS:0);
	if(hWnd)
	{
		psh.hwndParent	= hWnd;
	}
	else
	{
		psh.hwndParent	= GetDesktopWindow();
	}
	psh.pszCaption = szFileName;
	psh.nPages = se.m_tmpPropCount;
	psh.phpage = se.m_tmpProp;
	CString retTabName = getFileType(szFileName,dwPage);
	if(retTabName.GetLength() == 0)
	{
		psh.pStartPage = _T("mp3infp");
	}
	else
	{
		psh.pStartPage = retTabName;
	}

	return PropertySheet(&psh);
}

#ifdef UNICODE
extern "C" BOOL __stdcall mp3infp_ViewPropA(HWND hWnd,const char *szFileName,DWORD dwPage)
{
	return mp3infp_ViewPropW(hWnd,CString(szFileName),dwPage);
}
#endif

extern "C" BOOL __stdcall mp3infp_ViewProp(HWND hWnd,LPCTSTR szFileName,DWORD dwPage)
{
	SHELLEXECUTEINFO execinfo;
	memset(&execinfo,0,sizeof(SHELLEXECUTEINFO));
	execinfo.cbSize		= sizeof(SHELLEXECUTEINFO);
	execinfo.fMask		= SEE_MASK_INVOKEIDLIST;
	execinfo.lpVerb		= _T("properties");
	execinfo.lpFile		= szFileName;
	execinfo.lpParameters= _T("mp3infp");
	execinfo.nShow		= SW_SHOWNORMAL;
	if(hWnd)
	{
		execinfo.hwnd	= hWnd;
	}
	else
	{
		execinfo.hwnd	= GetDesktopWindow();
	}
	CString retTabName = getFileType(szFileName,dwPage);
	if(retTabName.GetLength() == 0)
	{
		execinfo.lpParameters = _T("mp3infp");
	}
	else
	{
		execinfo.lpParameters = retTabName;
	}

	return ShellExecuteEx(&execinfo);
}

#ifdef UNICODE
extern "C" DWORD __stdcall mp3infp_LoadA(HWND hWnd,const char *szFileName)
{
	return mp3infp_LoadW(hWnd,CString(szFileName));
}
#endif

extern "C" DWORD __stdcall mp3infp_Load(HWND hWnd,LPCTSTR szFileName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	DWORD dwRet = ERROR_SUCCESS;
	theApp.m_strSelectFile = szFileName;
	theApp.m_fileType = CShellExt::UNKNOWN;

	theApp.m_strAudioFormat = _T("");
	theApp.m_strVideoFormat = _T("");
	theApp.m_strStreamFormat = _T("");
	theApp.m_strTime = _T("");

	switch(CShellExt::GetFileType(szFileName)){
	case CShellExt::MP3:
		theApp.m_Id3tagv1.Load(szFileName);
		theApp.m_Id3tagv2.Load(szFileName);
		theApp.m_Rmp3.Load(szFileName);
		theApp.m_Mp3info.Load(szFileName);
		theApp.m_Ape.SetDonotsaveId3v1(FALSE);
		theApp.m_Ape.Load(szFileName);
		if(theApp.m_Rmp3.IsEnable())
		{
			theApp.m_Ape.Release();
		}
		theApp.m_fileType = CShellExt::MP3;
		break;
	case CShellExt::WAVE:
		dwRet = theApp.m_RiffSIF.Load(szFileName,'W','A','V','E');
		if(dwRet == ERROR_SUCCESS)
		{
			GetWaveAudioFormat(theApp.m_strSelectFile,
							theApp.m_RiffSIF.GetStreamSize(),
							theApp.m_strAudioFormat,
							theApp.m_strTime,
							theApp.m_iWaveCodecFind);
			theApp.m_fileType = CShellExt::WAVE;
		}
		break;
	case CShellExt::AVI:
		dwRet = theApp.m_OpenDML.Load(szFileName,'A','V','I',' ');
		if(dwRet == ERROR_SUCCESS)
		{
			GetAviFormat(theApp.m_strSelectFile,
							theApp.m_strAudioFormat,
							theApp.m_strVideoFormat,
							theApp.m_strStreamFormat,
							theApp.m_strTime,
							theApp.m_bAvi2,
							theApp.m_iAviCodecFind);
			theApp.m_fileType = CShellExt::AVI;
		}
		theApp.m_OpenDML.SetJunkHeader(_T("This file was made by ") SOFT_NAME);
		break;
	case CShellExt::VQF:
		dwRet = theApp.m_Vqf.Load(szFileName);
		if(dwRet == ERROR_SUCCESS)
		{
			theApp.m_fileType = CShellExt::VQF;
		}
		break;
	case CShellExt::WMA:
		if(theApp.m_Wma.Load(szFileName) == ERROR_SUCCESS)
		{
			theApp.m_fileType = CShellExt::WMA;
		}
		else
		{
			dwRet = -1;
		}
		break;
	case CShellExt::OGG:
		dwRet = theApp.m_Ogg.Load(szFileName);
		if(dwRet == ERROR_SUCCESS)
		{
			theApp.m_fileType = CShellExt::OGG;
		}
		break;
	case CShellExt::APE:
		dwRet = theApp.m_Monkeys.Load(szFileName);
		if(dwRet == ERROR_SUCCESS)
		{
			theApp.m_Ape.SetDonotsaveId3v1(TRUE);
			theApp.m_Ape.Load(szFileName);
			theApp.m_fileType = CShellExt::APE;
		}
		break;
	case CShellExt::MP4:
		dwRet = theApp.m_Mp4.Load(szFileName);
		if(dwRet == ERROR_SUCCESS)
		{
			theApp.m_fileType = CShellExt::MP4;
		}
		break;
	case CShellExt::UNKNOWN:
		break;
	}
	// ファイルサイズの取得
	theApp.m_i64FileSize = 0;
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(theApp.m_strSelectFile,&fd);
	if(hFind != INVALID_HANDLE_VALUE)
	{
		FindClose(hFind);
		theApp.m_i64FileSize = (((__int64 )fd.nFileSizeHigh)<<32)|(__int64 )fd.nFileSizeLow;
	}

	return dwRet;
}

extern "C" DWORD __stdcall mp3infp_GetType()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	switch(theApp.m_fileType){
	case CShellExt::MP3:
		return MP3INFP_FILE_MP3;
	case CShellExt::WAVE:
		return MP3INFP_FILE_WAV;
	case CShellExt::AVI:
		return MP3INFP_FILE_AVI;
	case CShellExt::VQF:
		return MP3INFP_FILE_VQF;
	case CShellExt::WMA:
		return MP3INFP_FILE_WMA;
	case CShellExt::OGG:
		return MP3INFP_FILE_OGG;
	case CShellExt::APE:
		return MP3INFP_FILE_APE;
	case CShellExt::MP4:
		return MP3INFP_FILE_MP4;
	case CShellExt::UNKNOWN:
	default:
		return MP3INFP_FILE_UNKNOWN;
	}
	return MP3INFP_FILE_UNKNOWN;
}

#ifdef UNICODE
extern "C" BOOL __stdcall mp3infp_GetValueA(const char *szValueName,char **buf)
{
	static char strRet[512];
	TCHAR *tbuf;
	
	*buf = NULL;
	BOOL ret = mp3infp_GetValueW(CString(szValueName), &tbuf);
	if (ret) {
		strRet[0] = '\0';
		TstrToData(tbuf, -1, strRet, sizeof(strRet), DTC_CODE_ANSI);
		*buf = strRet;	// QQQ strRetのサイズをオーバーするときは値を返せない
	}
	return ret;
}
#endif

extern "C" BOOL __stdcall mp3infp_GetValue(LPCTSTR szValueName,TCHAR **buf)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	static CString strRet;
	*buf = NULL;
	strRet = _T("");
	switch(theApp.m_fileType){
	case CShellExt::MP3:
		if(lstrcmp(szValueName,_T("FILE")) == 0)
		{
			strRet = getFileName(theApp.m_strSelectFile);
		}
		else if(lstrcmp(szValueName,_T("FEXT")) == 0)
		{
			strRet = getExtName(theApp.m_strSelectFile);
		}
		else if(lstrcmp(szValueName,_T("PATH")) == 0)
		{
			strRet = getPathName(theApp.m_strSelectFile);
		}
		else if(lstrcmp(szValueName,_T("SIZ1")) == 0)
		{
			CString strSize;
			CString strSize2;
			strSize.Format(_T("%I64u"),theApp.m_i64FileSize);
			strRet = divString((LPTSTR)(LPCTSTR )strSize,',',3);
		}
		else if(lstrcmp(szValueName,_T("SIZK")) == 0)
		{
			CString strSize;
			CString strSize2;
			__int64 fsize = (theApp.m_i64FileSize / 1024) + ((theApp.m_i64FileSize % 1024)?1:0);
			strSize.Format(_T("%I64u"),fsize);
			strRet = divString((LPTSTR)(LPCTSTR )strSize,',',3);
		}
		else if(lstrcmp(szValueName,_T("SIZM")) == 0)
		{
			CString strSize;
			CString strSize2;
			__int64 fsize = (theApp.m_i64FileSize / 1024) + ((theApp.m_i64FileSize % 1024)?1:0);
			fsize = (fsize / 1024) + ((fsize % 1024)?1:0);
			strSize.Format(_T("%I64u"),fsize);
			strRet = divString((LPTSTR)(LPCTSTR )strSize,',',3);
		}
		else if(lstrcmp(szValueName,_T("AFMT")) == 0)
		{
			strRet = theApp.m_Mp3info.GetFormatString();
		}
		else if(lstrcmp(szValueName,_T("TIME")) == 0)
		{
			strRet = theApp.m_Mp3info.GetTimeString();
		}
		else if(lstrcmp(szValueName,_T("INAM_v1")) == 0)
		{
			strRet = theApp.m_Id3tagv1.GetTitle();
		}
		else if(lstrcmp(szValueName,_T("IART_v1")) == 0)
		{
			strRet = theApp.m_Id3tagv1.GetArtist();
		}
		else if(lstrcmp(szValueName,_T("IPRD_v1")) == 0)
		{
			strRet = theApp.m_Id3tagv1.GetAlbum();
		}
		else if(lstrcmp(szValueName,_T("ICMT_v1")) == 0)
		{
			strRet = theApp.m_Id3tagv1.GetComment();
		}
		else if(lstrcmp(szValueName,_T("ICRD_v1")) == 0)
		{
			strRet = theApp.m_Id3tagv1.GetYear();
		}
		else if(lstrcmp(szValueName,_T("IGNR_v1")) == 0)
		{
			strRet = theApp.m_Id3tagv1.GetGenre();
		}
		else if(lstrcmp(szValueName,_T("TRACK_v1")) == 0)
		{
			strRet = theApp.m_Id3tagv1.GetTrackNo();
		}
		else if(lstrcmp(szValueName,_T("INAM_v2")) == 0)
		{
			strRet = theApp.m_Id3tagv2.GetTitle();
		}
		else if(lstrcmp(szValueName,_T("IART_v2")) == 0)
		{
			strRet = theApp.m_Id3tagv2.GetArtist();
		}
		else if(lstrcmp(szValueName,_T("IPRD_v2")) == 0)
		{
			strRet = theApp.m_Id3tagv2.GetAlbum();
		}
		else if(lstrcmp(szValueName,_T("ICMT_v2")) == 0)
		{
			strRet = theApp.m_Id3tagv2.GetComment();
		}
		else if(lstrcmp(szValueName,_T("ICRD_v2")) == 0)
		{
			strRet = theApp.m_Id3tagv2.GetYear();
		}
		else if(lstrcmp(szValueName,_T("IGNR_v2")) == 0)
		{
			strRet = theApp.m_Id3tagv2.GetGenre();
		}
		else if(lstrcmp(szValueName,_T("ICOP_v2")) == 0)
		{
			strRet = theApp.m_Id3tagv2.GetCopyright();
		}
		else if(lstrcmp(szValueName,_T("ISFT_v2")) == 0)
		{
			strRet = theApp.m_Id3tagv2.GetEncoder();
		}
		else if(lstrcmp(szValueName,_T("COMP_v2")) == 0)
		{
			strRet = theApp.m_Id3tagv2.GetComposer();
		}
		else if(lstrcmp(szValueName,_T("OART_v2")) == 0)
		{
			strRet = theApp.m_Id3tagv2.GetOrigArtist();
		}
		else if(lstrcmp(szValueName,_T("URL_v2")) == 0)
		{
			strRet = theApp.m_Id3tagv2.GetUrl();
		}
		else if(lstrcmp(szValueName,_T("TRACK_v2")) == 0)
		{
			strRet = theApp.m_Id3tagv2.GetTrackNo();
		}
		else if(lstrcmp(szValueName,_T("ENC2_v2")) == 0)
		{
			strRet = theApp.m_Id3tagv2.GetEncodest();
		}
		else if(lstrcmp(szValueName,_T("INAM_rmp")) == 0)
		{
			strRet = theApp.m_Rmp3.GetNAM();
		}
		else if(lstrcmp(szValueName,_T("IART_rmp")) == 0)
		{
			strRet = theApp.m_Rmp3.GetART();
		}
		else if(lstrcmp(szValueName,_T("IPRD_rmp")) == 0)
		{
			strRet = theApp.m_Rmp3.GetPRD();
		}
		else if(lstrcmp(szValueName,_T("ICMT_rmp")) == 0)
		{
			strRet = theApp.m_Rmp3.GetCMT();
		}
		else if(lstrcmp(szValueName,_T("ICRD_rmp")) == 0)
		{
			strRet = theApp.m_Rmp3.GetCRD();
		}
		else if(lstrcmp(szValueName,_T("IGNR_rmp")) == 0)
		{
			strRet = theApp.m_Rmp3.GetGNR();
		}
		else if(lstrcmp(szValueName,_T("ICOP_rmp")) == 0)
		{
			strRet = theApp.m_Rmp3.GetCOP();
		}
		else if(lstrcmp(szValueName,_T("ISFT_rmp")) == 0)
		{
			strRet = theApp.m_Rmp3.GetSFT();
		}
		else if(lstrcmp(szValueName,_T("ISRC_rmp")) == 0)
		{
			strRet = theApp.m_Rmp3.GetSRC();
		}
		else if(lstrcmp(szValueName,_T("IENG_rmp")) == 0)
		{
			strRet = theApp.m_Rmp3.GetENG();
		}
		else if(lstrcmp(szValueName,_T("INAM_APE")) == 0)
		{
			theApp.m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_TITLE,strRet);
		}
		else if(lstrcmp(szValueName,_T("TRACK_APE")) == 0)
		{
			theApp.m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_TRACK,strRet);
		}
		else if(lstrcmp(szValueName,_T("IART_APE")) == 0)
		{
			theApp.m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_ARTIST,strRet);
		}
		else if(lstrcmp(szValueName,_T("IPRD_APE")) == 0)
		{
			theApp.m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_ALBUM,strRet);
		}
		else if(lstrcmp(szValueName,_T("ICMT_APE")) == 0)
		{
			theApp.m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_COMMENT,strRet);
		}
		else if(lstrcmp(szValueName,_T("ICRD_APE")) == 0)
		{
			theApp.m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_YEAR,strRet);
		}
		else if(lstrcmp(szValueName,_T("IGNR_APE")) == 0)
		{
			theApp.m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_GENRE,strRet);
		}
		break;
	case CShellExt::WAVE:
		if(lstrcmp(szValueName,_T("FILE")) == 0)
		{
			strRet = getFileName(theApp.m_strSelectFile);
		}
		else if(lstrcmp(szValueName,_T("FEXT")) == 0)
		{
			strRet = getExtName(theApp.m_strSelectFile);
		}
		else if(lstrcmp(szValueName,_T("PATH")) == 0)
		{
			strRet = getPathName(theApp.m_strSelectFile);
		}
		else if(lstrcmp(szValueName,_T("SIZ1")) == 0)
		{
			CString strSize;
			CString strSize2;
			strSize.Format(_T("%I64u"),theApp.m_i64FileSize);
			strRet = divString((LPTSTR)(LPCTSTR )strSize,',',3);
		}
		else if(lstrcmp(szValueName,_T("SIZK")) == 0)
		{
			CString strSize;
			CString strSize2;
			__int64 fsize = (theApp.m_i64FileSize / 1024) + ((theApp.m_i64FileSize % 1024)?1:0);
			strSize.Format(_T("%I64u"),fsize);
			strRet = divString((LPTSTR)(LPCTSTR )strSize,',',3);
		}
		else if(lstrcmp(szValueName,_T("SIZM")) == 0)
		{
			CString strSize;
			CString strSize2;
			__int64 fsize = (theApp.m_i64FileSize / 1024) + ((theApp.m_i64FileSize % 1024)?1:0);
			fsize = (fsize / 1024) + ((fsize % 1024)?1:0);
			strSize.Format(_T("%I64u"),fsize);
			strRet = divString((LPTSTR)(LPCTSTR )strSize,',',3);
		}
		else if(lstrcmp(szValueName,_T("AFMT")) == 0)
		{
			strRet = theApp.m_strAudioFormat;
		}
		else if(lstrcmp(szValueName,_T("TIME")) == 0)
		{
			strRet = theApp.m_strTime;
		}
		else if(theApp.m_RiffSIF.IsEnable())
		{
			if(lstrcmp(szValueName,_T("INAM")) == 0)
			{
				strRet = theApp.m_RiffSIF.GetField('I','N','A','M');
			}
			else if(lstrcmp(szValueName,_T("ISBJ")) == 0)
			{
				strRet = theApp.m_RiffSIF.GetField('I','S','B','J');
			}
			else if(lstrcmp(szValueName,_T("TRACK")) == 0)
			{
				strRet = theApp.m_RiffSIF.GetField('I','T','R','K');
			}
			else if(lstrcmp(szValueName,_T("IART")) == 0)
			{
				strRet = theApp.m_RiffSIF.GetField('I','A','R','T');
			}
			else if(lstrcmp(szValueName,_T("IPRD")) == 0)
			{
				strRet = theApp.m_RiffSIF.GetField('I','P','R','D');
			}
			else if(lstrcmp(szValueName,_T("ICMT")) == 0)
			{
				strRet = theApp.m_RiffSIF.GetField('I','C','M','T');
			}
			else if(lstrcmp(szValueName,_T("ICRD")) == 0)
			{
				strRet = theApp.m_RiffSIF.GetField('I','C','R','D');
			}
			else if(lstrcmp(szValueName,_T("IGNR")) == 0)
			{
				strRet = theApp.m_RiffSIF.GetField('I','G','N','R');
			}
			else if(lstrcmp(szValueName,_T("ICOP")) == 0)
			{
				strRet = theApp.m_RiffSIF.GetField('I','C','O','P');
			}
			else if(lstrcmp(szValueName,_T("ISFT")) == 0)
			{
				strRet = theApp.m_RiffSIF.GetField('I','S','F','T');
			}
			else if(lstrcmp(szValueName,_T("ISRC")) == 0)
			{
				strRet = theApp.m_RiffSIF.GetField('I','S','R','C');
			}
			else if(lstrcmp(szValueName,_T("IENG")) == 0)
			{
				strRet = theApp.m_RiffSIF.GetField('I','E','N','G');
			}
		}
		break;
	case CShellExt::AVI:
		if(lstrcmp(szValueName,_T("FILE")) == 0)
		{
			strRet = getFileName(theApp.m_strSelectFile);
		}
		else if(lstrcmp(szValueName,_T("FEXT")) == 0)
		{
			strRet = getExtName(theApp.m_strSelectFile);
		}
		else if(lstrcmp(szValueName,_T("PATH")) == 0)
		{
			strRet = getPathName(theApp.m_strSelectFile);
		}
		else if(lstrcmp(szValueName,_T("SIZ1")) == 0)
		{
			CString strSize;
			CString strSize2;
			strSize.Format(_T("%I64u"),theApp.m_i64FileSize);
			strRet = divString((LPTSTR)(LPCTSTR )strSize,',',3);
		}
		else if(lstrcmp(szValueName,_T("SIZK")) == 0)
		{
			CString strSize;
			CString strSize2;
			__int64 fsize = (theApp.m_i64FileSize / 1024) + ((theApp.m_i64FileSize % 1024)?1:0);
			strSize.Format(_T("%I64u"),fsize);
			strRet = divString((LPTSTR)(LPCTSTR )strSize,',',3);
		}
		else if(lstrcmp(szValueName,_T("SIZM")) == 0)
		{
			CString strSize;
			CString strSize2;
			__int64 fsize = (theApp.m_i64FileSize / 1024) + ((theApp.m_i64FileSize % 1024)?1:0);
			fsize = (fsize / 1024) + ((fsize % 1024)?1:0);
			strSize.Format(_T("%I64u"),fsize);
			strRet = divString((LPTSTR)(LPCTSTR )strSize,',',3);
		}
		else if(lstrcmp(szValueName,_T("AFMT")) == 0)
		{
			strRet = theApp.m_strAudioFormat;
		}
		else if(lstrcmp(szValueName,_T("VFMT")) == 0)
		{
			strRet = theApp.m_strVideoFormat;
			strRet += _T(", ");
			strRet += theApp.m_strStreamFormat;
		}
		else if(lstrcmp(szValueName,_T("TIME")) == 0)
		{
			strRet = theApp.m_strTime;
		}
		else if(theApp.m_OpenDML.IsEnable())
		{
			if(lstrcmp(szValueName,_T("ISBJ")) == 0)
			{
				strRet = theApp.m_OpenDML.GetField('I','S','B','J');
			}
			else if(lstrcmp(szValueName,_T("INAM")) == 0)
			{
				strRet = theApp.m_OpenDML.GetField('I','N','A','M');
			}
			else if(lstrcmp(szValueName,_T("IART")) == 0)
			{
				strRet = theApp.m_OpenDML.GetField('I','A','R','T');
			}
			else if(lstrcmp(szValueName,_T("ICMT")) == 0)
			{
				strRet = theApp.m_OpenDML.GetField('I','C','M','T');
			}
			else if(lstrcmp(szValueName,_T("ICRD")) == 0)
			{
				strRet = theApp.m_OpenDML.GetField('I','C','R','D');
			}
			else if(lstrcmp(szValueName,_T("IGNR")) == 0)
			{
				strRet = theApp.m_OpenDML.GetField('I','G','N','R');
			}
			else if(lstrcmp(szValueName,_T("ICOP")) == 0)
			{
				strRet = theApp.m_OpenDML.GetField('I','C','O','P');
			}
			else if(lstrcmp(szValueName,_T("ISFT")) == 0)
			{
				strRet = theApp.m_OpenDML.GetField('I','S','F','T');
			}
			else if(lstrcmp(szValueName,_T("ISRC")) == 0)
			{
				strRet = theApp.m_OpenDML.GetField('I','S','R','C');
			}
			else if(lstrcmp(szValueName,_T("IENG")) == 0)
			{
				strRet = theApp.m_OpenDML.GetField('I','E','N','G');
			}
			else if(lstrcmp(szValueName,_T("AVIV")) == 0)
			{
				strRet = theApp.m_bAvi2?_T("AVI 2.0 (OpenDML)"):_T("AVI 1.0 (VfW)");
			}
		}
		break;
	case CShellExt::VQF:
		if(lstrcmp(szValueName,_T("FILE")) == 0)
		{
			strRet = getFileName(theApp.m_strSelectFile);
		}
		else if(lstrcmp(szValueName,_T("FEXT")) == 0)
		{
			strRet = getExtName(theApp.m_strSelectFile);
		}
		else if(lstrcmp(szValueName,_T("PATH")) == 0)
		{
			strRet = getPathName(theApp.m_strSelectFile);
		}
		else if(lstrcmp(szValueName,_T("SIZ1")) == 0)
		{
			CString strSize;
			CString strSize2;
			strSize.Format(_T("%I64u"),theApp.m_i64FileSize);
			strRet = divString((LPTSTR)(LPCTSTR )strSize,',',3);
		}
		else if(lstrcmp(szValueName,_T("SIZK")) == 0)
		{
			CString strSize;
			CString strSize2;
			__int64 fsize = (theApp.m_i64FileSize / 1024) + ((theApp.m_i64FileSize % 1024)?1:0);
			strSize.Format(_T("%I64u"),fsize);
			strRet = divString((LPTSTR)(LPCTSTR )strSize,',',3);
		}
		else if(lstrcmp(szValueName,_T("SIZM")) == 0)
		{
			CString strSize;
			CString strSize2;
			__int64 fsize = (theApp.m_i64FileSize / 1024) + ((theApp.m_i64FileSize % 1024)?1:0);
			fsize = (fsize / 1024) + ((fsize % 1024)?1:0);
			strSize.Format(_T("%I64u"),fsize);
			strRet = divString((LPTSTR)(LPCTSTR )strSize,',',3);
		}
		else if(!theApp.m_Vqf.IsEnable())
			break;
		if(lstrcmp(szValueName,_T("AFMT")) == 0)
		{
			strRet = theApp.m_Vqf.GetFormatString();
		}
		else if(lstrcmp(szValueName,_T("TIME")) == 0)
		{
			strRet = theApp.m_Vqf.GetTimeString();
		}
		else if(lstrcmp(szValueName,_T("INAM")) == 0)
		{
			strRet = theApp.m_Vqf.GetField('N','A','M','E');
		}
		else if(lstrcmp(szValueName,_T("IART")) == 0)
		{
			strRet = theApp.m_Vqf.GetField('A','U','T','H');
		}
		else if(lstrcmp(szValueName,_T("FILE")) == 0)
		{
			strRet = theApp.m_Vqf.GetField('F','I','L','E');
		}
		else if(lstrcmp(szValueName,_T("ICOP")) == 0)
		{
			strRet = theApp.m_Vqf.GetField('(','c',')',' ');
		}
		else if(lstrcmp(szValueName,_T("ICMT")) == 0)
		{
			strRet = theApp.m_Vqf.GetField('C','O','M','T');
		}
		break;
	case CShellExt::WMA:
		if(lstrcmp(szValueName,_T("FILE")) == 0)
		{
			strRet = getFileName(theApp.m_strSelectFile);
		}
		else if(lstrcmp(szValueName,_T("FEXT")) == 0)
		{
			strRet = getExtName(theApp.m_strSelectFile);
		}
		else if(lstrcmp(szValueName,_T("PATH")) == 0)
		{
			strRet = getPathName(theApp.m_strSelectFile);
		}
		else if(lstrcmp(szValueName,_T("SIZ1")) == 0)
		{
			CString strSize;
			CString strSize2;
			strSize.Format(_T("%I64u"),theApp.m_i64FileSize);
			strRet = divString((LPTSTR)(LPCTSTR )strSize,',',3);
		}
		else if(lstrcmp(szValueName,_T("SIZK")) == 0)
		{
			CString strSize;
			CString strSize2;
			__int64 fsize = (theApp.m_i64FileSize / 1024) + ((theApp.m_i64FileSize % 1024)?1:0);
			strSize.Format(_T("%I64u"),fsize);
			strRet = divString((LPTSTR)(LPCTSTR )strSize,',',3);
		}
		else if(lstrcmp(szValueName,_T("SIZM")) == 0)
		{
			CString strSize;
			CString strSize2;
			__int64 fsize = (theApp.m_i64FileSize / 1024) + ((theApp.m_i64FileSize % 1024)?1:0);
			fsize = (fsize / 1024) + ((fsize % 1024)?1:0);
			strSize.Format(_T("%I64u"),fsize);
			strRet = divString((LPTSTR)(LPCTSTR )strSize,',',3);
		}
		else if(!theApp.m_Wma.IsEnable())
			break;
		if(lstrcmp(szValueName,_T("AFMT")) == 0)
		{
			strRet = theApp.m_Wma.GetAudioFormatString();
		}
		else if(lstrcmp(szValueName,_T("VFMT")) == 0)
		{
			strRet = theApp.m_Wma.GetVideoFormatString();
		}
		else if(lstrcmp(szValueName,_T("TIME")) == 0)
		{
			strRet = theApp.m_Wma.GetTimeString();
		}
		else if(lstrcmp(szValueName,_T("INAM")) == 0)
		{
			strRet = theApp.m_Wma.GetNAM();
		}
		else if(lstrcmp(szValueName,_T("TRACK")) == 0)
		{
			strRet = theApp.m_Wma.GetTRACK();
		}
		else if(lstrcmp(szValueName,_T("IART")) == 0)
		{
			strRet = theApp.m_Wma.GetART();
		}
		else if(lstrcmp(szValueName,_T("IPRD")) == 0)
		{
			strRet = theApp.m_Wma.GetPRD();
		}
		else if(lstrcmp(szValueName,_T("ICMT")) == 0)
		{
			strRet = theApp.m_Wma.GetCMT();
		}
		else if(lstrcmp(szValueName,_T("ICRD")) == 0)
		{
			strRet = theApp.m_Wma.GetCRD();
		}
		else if(lstrcmp(szValueName,_T("IGNR")) == 0)
		{
			strRet = theApp.m_Wma.GetGNR();
		}
		else if(lstrcmp(szValueName,_T("ICOP")) == 0)
		{
			strRet = theApp.m_Wma.GetCOPY();
		}
		else if(lstrcmp(szValueName,_T("URL1")) == 0)
		{
			strRet = theApp.m_Wma.GetUrl1();
		}
		else if(lstrcmp(szValueName,_T("URL2")) == 0)
		{
			strRet = theApp.m_Wma.GetUrl2();
		}
		break;
	case CShellExt::OGG:
		if(lstrcmp(szValueName,_T("FILE")) == 0)
		{
			strRet = getFileName(theApp.m_strSelectFile);
		}
		else if(lstrcmp(szValueName,_T("FEXT")) == 0)
		{
			strRet = getExtName(theApp.m_strSelectFile);
		}
		else if(lstrcmp(szValueName,_T("PATH")) == 0)
		{
			strRet = getPathName(theApp.m_strSelectFile);
		}
		else if(lstrcmp(szValueName,_T("SIZ1")) == 0)
		{
			CString strSize;
			CString strSize2;
			strSize.Format(_T("%I64u"),theApp.m_i64FileSize);
			strRet = divString((LPTSTR)(LPCTSTR )strSize,',',3);
		}
		else if(lstrcmp(szValueName,_T("SIZK")) == 0)
		{
			CString strSize;
			CString strSize2;
			__int64 fsize = (theApp.m_i64FileSize / 1024) + ((theApp.m_i64FileSize % 1024)?1:0);
			strSize.Format(_T("%I64u"),fsize);
			strRet = divString((LPTSTR)(LPCTSTR )strSize,',',3);
		}
		else if(lstrcmp(szValueName,_T("SIZM")) == 0)
		{
			CString strSize;
			CString strSize2;
			__int64 fsize = (theApp.m_i64FileSize / 1024) + ((theApp.m_i64FileSize % 1024)?1:0);
			fsize = (fsize / 1024) + ((fsize % 1024)?1:0);
			strSize.Format(_T("%I64u"),fsize);
			strRet = divString((LPTSTR)(LPCTSTR )strSize,',',3);
		}
		else if(!theApp.m_Ogg.IsEnable())
			break;
		if(_tcsncmp(szValueName,_T("AFMT"),4) == 0)
		{
			strRet = theApp.m_Ogg.GetAudioFormatString();
		}
		else if(_tcsncmp(szValueName,_T("TIME"),4) == 0)
		{
			strRet = theApp.m_Ogg.GetTimeString();
		}
		else if(_tcsncmp(szValueName,_T("INAM"),4) == 0)
		{
			theApp.m_Ogg.GetComment(_T("TITLE"),0,strRet);
		}
		else if(_tcsncmp(szValueName,_T("TRACK"),5) == 0)
		{
			theApp.m_Ogg.GetComment(_T("TRACKNUMBER"),0,strRet);
		}
		else if(_tcsncmp(szValueName,_T("IART"),4) == 0)
		{
			theApp.m_Ogg.GetComment(_T("ARTIST"),0,strRet);
		}
		else if(_tcsncmp(szValueName,_T("IPRD"),4) == 0)
		{
			theApp.m_Ogg.GetComment(_T("ALBUM"),0,strRet);
		}
		else if(_tcsncmp(szValueName,_T("ICMT"),4) == 0)
		{
			theApp.m_Ogg.GetComment(_T("COMMENT"),0,strRet);
		}
		else if(_tcsncmp(szValueName,_T("ICRD"),4) == 0)
		{
			theApp.m_Ogg.GetComment(_T("DATE"),0,strRet);
		}
		else if(_tcsncmp(szValueName,_T("IGNR"),4) == 0)
		{
			theApp.m_Ogg.GetComment(_T("GENRE"),0,strRet);
		}
		break;
	case CShellExt::APE:
		if(lstrcmp(szValueName,_T("FILE")) == 0)
		{
			strRet = getFileName(theApp.m_strSelectFile);
		}
		else if(lstrcmp(szValueName,_T("FEXT")) == 0)
		{
			strRet = getExtName(theApp.m_strSelectFile);
		}
		else if(lstrcmp(szValueName,_T("PATH")) == 0)
		{
			strRet = getPathName(theApp.m_strSelectFile);
		}
		else if(lstrcmp(szValueName,_T("SIZ1")) == 0)
		{
			CString strSize;
			CString strSize2;
			strSize.Format(_T("%I64u"),theApp.m_i64FileSize);
			strRet = divString((LPTSTR)(LPCTSTR )strSize,',',3);
		}
		else if(lstrcmp(szValueName,_T("SIZK")) == 0)
		{
			CString strSize;
			CString strSize2;
			__int64 fsize = (theApp.m_i64FileSize / 1024) + ((theApp.m_i64FileSize % 1024)?1:0);
			strSize.Format(_T("%I64u"),fsize);
			strRet = divString((LPTSTR)(LPCTSTR )strSize,',',3);
		}
		else if(lstrcmp(szValueName,_T("SIZM")) == 0)
		{
			CString strSize;
			CString strSize2;
			__int64 fsize = (theApp.m_i64FileSize / 1024) + ((theApp.m_i64FileSize % 1024)?1:0);
			fsize = (fsize / 1024) + ((fsize % 1024)?1:0);
			strSize.Format(_T("%I64u"),fsize);
			strRet = divString((LPTSTR)(LPCTSTR )strSize,',',3);
		}
		else if(!theApp.m_Ape.IsEnable())
			break;
		if(_tcsncmp(szValueName,_T("AFMT"),4) == 0)
		{
			strRet = theApp.m_Monkeys.GetAudioFormatString();
		}
		else if(_tcsncmp(szValueName,_T("TIME"),4) == 0)
		{
			strRet = theApp.m_Monkeys.GetTimeString();
		}
		else if(_tcsncmp(szValueName,_T("INAM"),4) == 0)
		{
			theApp.m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_TITLE,strRet);
		}
		else if(_tcsncmp(szValueName,_T("TRACK"),5) == 0)
		{
			theApp.m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_TRACK,strRet);
		}
		else if(_tcsncmp(szValueName,_T("IART"),4) == 0)
		{
			theApp.m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_ARTIST,strRet);
		}
		else if(_tcsncmp(szValueName,_T("IPRD"),4) == 0)
		{
			theApp.m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_ALBUM,strRet);
		}
		else if(_tcsncmp(szValueName,_T("ICMT"),4) == 0)
		{
			theApp.m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_COMMENT,strRet);
		}
		else if(_tcsncmp(szValueName,_T("ICRD"),4) == 0)
		{
			theApp.m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_YEAR,strRet);
		}
		else if(_tcsncmp(szValueName,_T("IGNR"),4) == 0)
		{
			theApp.m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_GENRE,strRet);
		}
		break;
	case CShellExt::MP4:
		if(lstrcmp(szValueName,_T("FILE")) == 0)
		{
			strRet = getFileName(theApp.m_strSelectFile);
		}
		else if(lstrcmp(szValueName,_T("FEXT")) == 0)
		{
			strRet = getExtName(theApp.m_strSelectFile);
		}
		else if(lstrcmp(szValueName,_T("PATH")) == 0)
		{
			strRet = getPathName(theApp.m_strSelectFile);
		}
		else if(lstrcmp(szValueName,_T("SIZ1")) == 0)
		{
			CString strSize;
			CString strSize2;
			strSize.Format(_T("%I64u"),theApp.m_i64FileSize);
			strRet = divString((LPTSTR)(LPCTSTR )strSize,',',3);
		}
		else if(lstrcmp(szValueName,_T("SIZK")) == 0)
		{
			CString strSize;
			CString strSize2;
			__int64 fsize = (theApp.m_i64FileSize / 1024) + ((theApp.m_i64FileSize % 1024)?1:0);
			strSize.Format(_T("%I64u"),fsize);
			strRet = divString((LPTSTR)(LPCTSTR )strSize,',',3);
		}
		else if(lstrcmp(szValueName,_T("SIZM")) == 0)
		{
			CString strSize;
			CString strSize2;
			__int64 fsize = (theApp.m_i64FileSize / 1024) + ((theApp.m_i64FileSize % 1024)?1:0);
			fsize = (fsize / 1024) + ((fsize % 1024)?1:0);
			strSize.Format(_T("%I64u"),fsize);
			strRet = divString((LPTSTR)(LPCTSTR )strSize,',',3);
		}
		else if(!theApp.m_Mp4.IsEnable())
			break;
		if(lstrcmp(szValueName,_T("AFMT")) == 0)
		{
			strRet = theApp.m_Mp4.GetAudioInfoString();
		}
		else if(lstrcmp(szValueName,_T("VFMT")) == 0)
		{
			strRet = theApp.m_Mp4.GetVideoInfoString();
		}
		else if(_tcsncmp(szValueName,_T("INAM"),4) == 0)
		{
			strRet = theApp.m_Mp4.GetMetadata_Name();
		}
		else if(_tcsncmp(szValueName,_T("IART"),4) == 0)
		{
			strRet = theApp.m_Mp4.GetMetadata_Artist();
		}
		else if(_tcsncmp(szValueName,_T("IPRD"),4) == 0)
		{
			strRet = theApp.m_Mp4.GetMetadata_Album();
		}
		else if(_tcsncmp(szValueName,_T("IGRP"),4) == 0)
		{
			strRet = theApp.m_Mp4.GetMetadata_Group();
		}
		else if(_tcsncmp(szValueName,_T("COMPOSER"),8) == 0)
		{
			strRet = theApp.m_Mp4.GetMetadata_Composer();
		}
		else if(_tcsncmp(szValueName,_T("IGNR"),4) == 0)
		{
			strRet = theApp.m_Mp4.GetMetadata_Genre();
		}
		else if(_tcsncmp(szValueName,_T("TRACK1"),6) == 0)
		{
			int val = theApp.m_Mp4.GetMetadata_Track1();
			if(val != -1)
			{
				strRet.Format(_T("%d"),val);
			}
			else
			{
				strRet = _T("");
			}
		}
		else if(_tcsncmp(szValueName,_T("TRACK2"),6) == 0)
		{
			int val = theApp.m_Mp4.GetMetadata_Track2();
			if(val != -1)
			{
				strRet.Format(_T("%d"),val);
			}
			else
			{
				strRet = _T("");
			}
		}
		else if(_tcsncmp(szValueName,_T("DISC1"),5) == 0)
		{
			int val = theApp.m_Mp4.GetMetadata_Disc1();
			if(val != -1)
			{
				strRet.Format(_T("%d"),val);
			}
			else
			{
				strRet = _T("");
			}
		}
		else if(_tcsncmp(szValueName,_T("DISC2"),5) == 0)
		{
			int val = theApp.m_Mp4.GetMetadata_Disc2();
			if(val != -1)
			{
				strRet.Format(_T("%d"),val);
			}
			else
			{
				strRet = _T("");
			}
		}
		else if(_tcsncmp(szValueName,_T("BPM"),3) == 0)
		{
			int val = theApp.m_Mp4.GetMetadata_Tempo();
			if(val != -1)
			{
				strRet.Format(_T("%d"),val);
			}
			else
			{
				strRet = _T("");
			}
		}
		else if(_tcsncmp(szValueName,_T("ICRD"),4) == 0)
		{
			strRet = theApp.m_Mp4.GetMetadata_Year();
		}
		else if(_tcsncmp(szValueName,_T("COMPILATION"),11) == 0)
		{
			int val = theApp.m_Mp4.GetMetadata_Compilation();
			if(val != -1)
			{
				strRet.Format(_T("%d"),val);
			}
			else
			{
				strRet = _T("");
			}
		}
		else if(_tcsncmp(szValueName,_T("ICMT"),4) == 0)
		{
			strRet = theApp.m_Mp4.GetMetadata_Comment();
		}
		else if(_tcsncmp(szValueName,_T("TOOL"),4) == 0)
		{
			strRet = theApp.m_Mp4.GetMetadata_Tool();
		}
		break;
	case CShellExt::UNKNOWN:
	default:
		return FALSE;
	}

	*buf = (LPTSTR)(LPCTSTR )strRet;

	return TRUE;
}

extern "C" DWORD __stdcall mp3infp_mp3_GetTagType()
{
	DWORD dwRet = 0;
	if(theApp.m_Id3tagv1.IsEnable())
	{
		dwRet |= MP3INFP_HAS_MP3_ID3V1;
		if(theApp.m_Id3tagv1.GetTrackNo().GetLength())
		{
			dwRet |= MP3INFP_HAS_MP3_ID3V1_1;
		}
		else
		{
			dwRet |= MP3INFP_HAS_MP3_ID3V1_0;
		}
	}
	if(theApp.m_Id3tagv2.IsEnable())
	{
		dwRet |= MP3INFP_HAS_MP3_ID3V2;
		WORD wVer = theApp.m_Id3tagv2.GetVer();
		switch(wVer){
		case 0x0200:
			dwRet |= MP3INFP_HAS_MP3_ID3V2_2;
			break;
		case 0x0300:
			dwRet |= MP3INFP_HAS_MP3_ID3V2_3;
			break;
		case 0x0400:
			dwRet |= MP3INFP_HAS_MP3_ID3V2_4;
			break;
		}
	}
	if(theApp.m_Ape.IsEnable())
	{
		if(theApp.m_Ape.GetApeVersion() < 2000)
		{
			dwRet |= MP3INFP_HAS_MP3_APEV1;
		}
		else
		{
			dwRet |= MP3INFP_HAS_MP3_APEV2;
		}
	}
	dwRet |= theApp.m_Rmp3.IsEnable()?MP3INFP_HAS_MP3_RIFFSIF:0;
	
	return dwRet;
}

#ifdef UNICODE
extern "C" BOOL __stdcall mp3infp_SetConfA(char *tag,char *value)
{
	return mp3infp_SetConfW((LPTSTR)(LPCTSTR)CString(tag),(LPTSTR)(LPCTSTR)CString(value));
}
#endif

extern "C" BOOL __stdcall mp3infp_SetConf(TCHAR *tag,TCHAR *value)
{
	if(lstrcmp(tag,_T("wave_CodecFind")) == 0)
	{
		int val = _ttoi(value);
		if((theApp.m_iWaveCodecFind <= 3) && (0 <= theApp.m_iWaveCodecFind))
		{
			theApp.m_iWaveCodecFind = val;
			return TRUE;
		}
	}
	else if(lstrcmp(tag,_T("avi_CodecFind")) == 0)
	{
		int val = _ttoi(value);
		if((theApp.m_iAviCodecFind <= 3) && (0 <= theApp.m_iAviCodecFind))
		{
			theApp.m_iAviCodecFind = val;
			return TRUE;
		}
	}
	else if(lstrcmp(tag,_T("mp3_UseExtGenre")) == 0)
	{
		int val = _ttoi(value);
		switch(val){
		case 0:
			theApp.m_Id3tagv1.SetScmpxGenre(FALSE);
			break;
		case 1:
		default:
			theApp.m_Id3tagv1.SetScmpxGenre(TRUE);
			break;
		}
		return TRUE;
	}
	else if(lstrcmp(tag,_T("mp3_ID3v2Unicode")) == 0)
	{
		int val = _ttoi(value);
		switch(val){
		case 0:
			theApp.m_Id3tagv2.SetCharEncode(CId3tagv2::ID3V2CHARENCODE_ISO_8859_1);
			break;
		case 1:
		default:
			theApp.m_Id3tagv2.SetCharEncode(CId3tagv2::ID3V2CHARENCODE_UTF_16);
			break;
		}
		return TRUE;
	}
	else if(lstrcmp(tag,_T("mp3_ID3v2Unsync")) == 0)
	{
		int val = _ttoi(value);
		switch(val){
		case 0:
			theApp.m_Id3tagv2.SetUnSynchronization(FALSE);
			break;
		case 1:
		default:
			theApp.m_Id3tagv2.SetUnSynchronization(TRUE);
			break;
		}
		return TRUE;
	}
	else if(lstrcmp(tag,_T("mp3_SaveID3v2Version")) == 0)
	{
		if(lstrcmp(value,_T("2.2")) == 0)
		{
			theApp.m_Id3tagv2.SetVer(0x0200);
		}
		else if(lstrcmp(value,_T("2.3")) == 0)
		{
			theApp.m_Id3tagv2.SetVer(0x0300);
		}
		else if(lstrcmp(value,_T("2.4")) == 0)
		{
			theApp.m_Id3tagv2.SetVer(0x0400);
		}
		else
		{
			return FALSE;
		}
		return TRUE;
	}

	return FALSE;
}

#ifdef UNICODE
extern "C" DWORD __stdcall mp3infp_SetValueA(const char *szValueName,const char *buf)
{
	return mp3infp_SetValueW(CString(szValueName),CString(buf));
}
#endif

extern "C" DWORD __stdcall mp3infp_SetValue(LPCTSTR szValueName,LPCTSTR buf)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	switch(theApp.m_fileType){
	case CShellExt::MP3:
		if(lstrcmp(szValueName,_T("INAM_v1")) == 0)
		{
			theApp.m_Id3tagv1.SetTitle(buf);
		}
		else if(lstrcmp(szValueName,_T("IART_v1")) == 0)
		{
			theApp.m_Id3tagv1.SetArtist(buf);
		}
		else if(lstrcmp(szValueName,_T("IPRD_v1")) == 0)
		{
			theApp.m_Id3tagv1.SetAlbum(buf);
		}
		else if(lstrcmp(szValueName,_T("ICMT_v1")) == 0)
		{
			theApp.m_Id3tagv1.SetComment(buf);
		}
		else if(lstrcmp(szValueName,_T("ICRD_v1")) == 0)
		{
			theApp.m_Id3tagv1.SetYear(buf);
		}
		else if(lstrcmp(szValueName,_T("IGNR_v1")) == 0)
		{
			theApp.m_Id3tagv1.SetGenre(buf);
		}
		else if(lstrcmp(szValueName,_T("TRACK_v1")) == 0)
		{
			theApp.m_Id3tagv1.SetTrackNo(buf);
		}
		else if(lstrcmp(szValueName,_T("INAM_v2")) == 0)
		{
			theApp.m_Id3tagv2.SetTitle(buf);
		}
		else if(lstrcmp(szValueName,_T("IART_v2")) == 0)
		{
			theApp.m_Id3tagv2.SetArtist(buf);
		}
		else if(lstrcmp(szValueName,_T("IPRD_v2")) == 0)
		{
			theApp.m_Id3tagv2.SetAlbum(buf);
		}
		else if(lstrcmp(szValueName,_T("ICMT_v2")) == 0)
		{
			theApp.m_Id3tagv2.SetComment(buf);
		}
		else if(lstrcmp(szValueName,_T("ICRD_v2")) == 0)
		{
			theApp.m_Id3tagv2.SetYear(buf);
		}
		else if(lstrcmp(szValueName,_T("IGNR_v2")) == 0)
		{
			theApp.m_Id3tagv2.SetGenre(buf);
		}
		else if(lstrcmp(szValueName,_T("ICOP_v2")) == 0)
		{
			theApp.m_Id3tagv2.SetCopyright(buf);
		}
		else if(lstrcmp(szValueName,_T("ISFT_v2")) == 0)
		{
			theApp.m_Id3tagv2.SetEncoder(buf);
		}
		else if(lstrcmp(szValueName,_T("COMP_v2")) == 0)
		{
			theApp.m_Id3tagv2.SetComposer(buf);
		}
		else if(lstrcmp(szValueName,_T("OART_v2")) == 0)
		{
			theApp.m_Id3tagv2.SetOrigArtist(buf);
		}
		else if(lstrcmp(szValueName,_T("URL_v2")) == 0)
		{
			theApp.m_Id3tagv2.SetUrl(buf);
		}
		else if(lstrcmp(szValueName,_T("TRACK_v2")) == 0)
		{
			theApp.m_Id3tagv2.SetTrackNo(buf);
		}
		else if(lstrcmp(szValueName,_T("ENC2_v2")) == 0)
		{
			theApp.m_Id3tagv2.SetEncodest(buf);
		}
		else if(lstrcmp(szValueName,_T("INAM_rmp")) == 0)
		{
			theApp.m_Rmp3.SetNAM(buf);
		}
		else if(lstrcmp(szValueName,_T("IART_rmp")) == 0)
		{
			theApp.m_Rmp3.SetART(buf);
		}
		else if(lstrcmp(szValueName,_T("IPRD_rmp")) == 0)
		{
			theApp.m_Rmp3.SetPRD(buf);
		}
		else if(lstrcmp(szValueName,_T("ICMT_rmp")) == 0)
		{
			theApp.m_Rmp3.SetCMT(buf);
		}
		else if(lstrcmp(szValueName,_T("ICRD_rmp")) == 0)
		{
			theApp.m_Rmp3.SetCRD(buf);
		}
		else if(lstrcmp(szValueName,_T("IGNR_rmp")) == 0)
		{
			theApp.m_Rmp3.SetGNR(buf);
		}
		else if(lstrcmp(szValueName,_T("ICOP_rmp")) == 0)
		{
			theApp.m_Rmp3.SetCOP(buf);
		}
		else if(lstrcmp(szValueName,_T("ISFT_rmp")) == 0)
		{
			theApp.m_Rmp3.SetSFT(buf);
		}
		else if(lstrcmp(szValueName,_T("ISRC_rmp")) == 0)
		{
			theApp.m_Rmp3.SetSRC(buf);
		}
		else if(lstrcmp(szValueName,_T("IENG_rmp")) == 0)
		{
			theApp.m_Rmp3.SetENG(buf);
		}
		else if(lstrcmp(szValueName,_T("INAM_APE")) == 0)
		{
			theApp.m_Ape.SetComment(CTag_Ape::APE_TAG_FIELD_TITLE,buf);
		}
		else if(lstrcmp(szValueName,_T("TRACK_APE")) == 0)
		{
			theApp.m_Ape.SetComment(CTag_Ape::APE_TAG_FIELD_TRACK,buf);
		}
		else if(lstrcmp(szValueName,_T("IART_APE")) == 0)
		{
			theApp.m_Ape.SetComment(CTag_Ape::APE_TAG_FIELD_ARTIST,buf);
		}
		else if(lstrcmp(szValueName,_T("IPRD_APE")) == 0)
		{
			theApp.m_Ape.SetComment(CTag_Ape::APE_TAG_FIELD_ALBUM,buf);
		}
		else if(lstrcmp(szValueName,_T("ICMT_APE")) == 0)
		{
			theApp.m_Ape.SetComment(CTag_Ape::APE_TAG_FIELD_COMMENT,buf);
		}
		else if(lstrcmp(szValueName,_T("ICRD_APE")) == 0)
		{
			theApp.m_Ape.SetComment(CTag_Ape::APE_TAG_FIELD_YEAR,buf);
		}
		else if(lstrcmp(szValueName,_T("IGNR_APE")) == 0)
		{
			theApp.m_Ape.SetComment(CTag_Ape::APE_TAG_FIELD_GENRE,buf);
		}
		break;
	case CShellExt::WAVE:
		if(lstrcmp(szValueName,_T("INAM")) == 0)
		{
			theApp.m_RiffSIF.SetField('I','N','A','M',buf);
		}
		else if(lstrcmp(szValueName,_T("ISBJ")) == 0)
		{
			theApp.m_RiffSIF.SetField('I','S','B','J',buf);
		}
		else if(lstrcmp(szValueName,_T("TRACK")) == 0)
		{
			theApp.m_RiffSIF.SetField('I','T','R','K',buf);
		}
		else if(lstrcmp(szValueName,_T("IART")) == 0)
		{
			theApp.m_RiffSIF.SetField('I','A','R','T',buf);
		}
		else if(lstrcmp(szValueName,_T("IPRD")) == 0)
		{
			theApp.m_RiffSIF.SetField('I','P','R','D',buf);
		}
		else if(lstrcmp(szValueName,_T("ICMT")) == 0)
		{
			theApp.m_RiffSIF.SetField('I','C','M','T',buf);
		}
		else if(lstrcmp(szValueName,_T("ICRD")) == 0)
		{
			theApp.m_RiffSIF.SetField('I','C','R','D',buf);
		}
		else if(lstrcmp(szValueName,_T("IGNR")) == 0)
		{
			theApp.m_RiffSIF.SetField('I','G','N','R',buf);
		}
		else if(lstrcmp(szValueName,_T("ICOP")) == 0)
		{
			theApp.m_RiffSIF.SetField('I','C','O','P',buf);
		}
		else if(lstrcmp(szValueName,_T("ISFT")) == 0)
		{
			theApp.m_RiffSIF.SetField('I','S','F','T',buf);
		}
		else if(lstrcmp(szValueName,_T("ISRC")) == 0)
		{
			theApp.m_RiffSIF.SetField('I','S','R','C',buf);
		}
		else if(lstrcmp(szValueName,_T("IENG")) == 0)
		{
			theApp.m_RiffSIF.SetField('I','E','N','G',buf);
		}
		break;
	case CShellExt::AVI:
		if(lstrcmp(szValueName,_T("ISBJ")) == 0)
		{
			theApp.m_OpenDML.SetField('I','S','B','J',buf);
		}
		else if(lstrcmp(szValueName,_T("INAM")) == 0)
		{
			theApp.m_OpenDML.SetField('I','N','A','M',buf);
		}
		else if(lstrcmp(szValueName,_T("IART")) == 0)
		{
			theApp.m_OpenDML.SetField('I','A','R','T',buf);
		}
		else if(lstrcmp(szValueName,_T("ICMT")) == 0)
		{
			theApp.m_OpenDML.SetField('I','C','M','T',buf);
		}
		else if(lstrcmp(szValueName,_T("ICRD")) == 0)
		{
			theApp.m_OpenDML.SetField('I','C','R','D',buf);
		}
		else if(lstrcmp(szValueName,_T("IGNR")) == 0)
		{
			theApp.m_OpenDML.SetField('I','G','N','R',buf);
		}
		else if(lstrcmp(szValueName,_T("ICOP")) == 0)
		{
			theApp.m_OpenDML.SetField('I','C','O','P',buf);
		}
		else if(lstrcmp(szValueName,_T("ISFT")) == 0)
		{
			theApp.m_OpenDML.SetField('I','S','F','T',buf);
		}
		else if(lstrcmp(szValueName,_T("ISRC")) == 0)
		{
			theApp.m_OpenDML.SetField('I','S','R','C',buf);
		}
		else if(lstrcmp(szValueName,_T("IENG")) == 0)
		{
			theApp.m_OpenDML.SetField('I','E','N','G',buf);
		}
		break;
	case CShellExt::VQF:
		if(lstrcmp(szValueName,_T("INAM")) == 0)
		{
			theApp.m_Vqf.SetField('N','A','M','E',buf);
		}
		else if(lstrcmp(szValueName,_T("IART")) == 0)
		{
			theApp.m_Vqf.SetField('A','U','T','H',buf);
		}
		else if(lstrcmp(szValueName,_T("FILE")) == 0)
		{
			theApp.m_Vqf.SetField('F','I','L','E',buf);
		}
		else if(lstrcmp(szValueName,_T("ICOP")) == 0)
		{
			theApp.m_Vqf.SetField('(','c',')',' ',buf);
		}
		else if(lstrcmp(szValueName,_T("ICMT")) == 0)
		{
			theApp.m_Vqf.SetField('C','O','M','T',buf);
		}
		break;
	case CShellExt::WMA:
		if(lstrcmp(szValueName,_T("INAM")) == 0)
		{
			theApp.m_Wma.SetNAM(buf);
		}
		else if(lstrcmp(szValueName,_T("TRACK")) == 0)
		{
			theApp.m_Wma.SetTRACK(buf);
		}
		else if(lstrcmp(szValueName,_T("IART")) == 0)
		{
			theApp.m_Wma.SetART(buf);
		}
		else if(lstrcmp(szValueName,_T("IPRD")) == 0)
		{
			theApp.m_Wma.SetPRD(buf);
		}
		else if(lstrcmp(szValueName,_T("ICMT")) == 0)
		{
			theApp.m_Wma.SetCMT(buf);
		}
		else if(lstrcmp(szValueName,_T("ICRD")) == 0)
		{
			theApp.m_Wma.SetCRD(buf);
		}
		else if(lstrcmp(szValueName,_T("IGNR")) == 0)
		{
			theApp.m_Wma.SetGNR(buf);
		}
		else if(lstrcmp(szValueName,_T("ICOP")) == 0)
		{
			theApp.m_Wma.SetCOPY(buf);
		}
		else if(lstrcmp(szValueName,_T("URL1")) == 0)
		{
			theApp.m_Wma.SetUrl1(buf);
		}
		else if(lstrcmp(szValueName,_T("URL2")) == 0)
		{
			theApp.m_Wma.SetUrl2(buf);
		}
		break;
	case CShellExt::OGG:
		if(_tcsncmp(szValueName,_T("INAM"),4) == 0)
		{
			theApp.m_Ogg.DelComment(_T("TITLE"),0);
			theApp.m_Ogg.AddComment(_T("TITLE"),buf);
		}
		else if(_tcsncmp(szValueName,_T("TRACK"),5) == 0)
		{
			theApp.m_Ogg.DelComment(_T("TRACKNUMBER"),0);
			theApp.m_Ogg.AddComment(_T("TRACKNUMBER"),buf);
		}
		else if(_tcsncmp(szValueName,_T("IART"),4) == 0)
		{
			theApp.m_Ogg.DelComment(_T("ARTIST"),0);
			theApp.m_Ogg.AddComment(_T("ARTIST"),buf);
		}
		else if(_tcsncmp(szValueName,_T("IPRD"),4) == 0)
		{
			theApp.m_Ogg.DelComment(_T("ALBUM"),0);
			theApp.m_Ogg.AddComment(_T("ALBUM"),buf);
		}
		else if(_tcsncmp(szValueName,_T("ICMT"),4) == 0)
		{
			theApp.m_Ogg.DelComment(_T("COMMENT"),0);
			theApp.m_Ogg.AddComment(_T("COMMENT"),buf);
		}
		else if(_tcsncmp(szValueName,_T("ICRD"),4) == 0)
		{
			theApp.m_Ogg.DelComment(_T("DATE"),0);
			theApp.m_Ogg.AddComment(_T("DATE"),buf);
		}
		else if(_tcsncmp(szValueName,_T("IGNR"),4) == 0)
		{
			theApp.m_Ogg.DelComment(_T("GENRE"),0);
			theApp.m_Ogg.AddComment(_T("GENRE"),buf);
		}
		break;
	case CShellExt::APE:
		if(_tcsncmp(szValueName,_T("INAM"),4) == 0)
		{
			theApp.m_Ape.SetComment(CTag_Ape::APE_TAG_FIELD_TITLE,buf);
		}
		else if(_tcsncmp(szValueName,_T("TRACK"),5) == 0)
		{
			theApp.m_Ape.SetComment(CTag_Ape::APE_TAG_FIELD_TRACK,buf);
		}
		else if(_tcsncmp(szValueName,_T("IART"),4) == 0)
		{
			theApp.m_Ape.SetComment(CTag_Ape::APE_TAG_FIELD_ARTIST,buf);
		}
		else if(_tcsncmp(szValueName,_T("IPRD"),4) == 0)
		{
			theApp.m_Ape.SetComment(CTag_Ape::APE_TAG_FIELD_ALBUM,buf);
		}
		else if(_tcsncmp(szValueName,_T("ICMT"),4) == 0)
		{
			theApp.m_Ape.SetComment(CTag_Ape::APE_TAG_FIELD_COMMENT,buf);
		}
		else if(_tcsncmp(szValueName,_T("ICRD"),4) == 0)
		{
			theApp.m_Ape.SetComment(CTag_Ape::APE_TAG_FIELD_YEAR,buf);
		}
		else if(_tcsncmp(szValueName,_T("IGNR"),4) == 0)
		{
			theApp.m_Ape.SetComment(CTag_Ape::APE_TAG_FIELD_GENRE,buf);
		}
		break;
	case CShellExt::MP4:
		if(_tcsncmp(szValueName,_T("INAM"),4) == 0)
		{
			theApp.m_Mp4.SetMetadata_Name(buf);
		}
		else if(_tcsncmp(szValueName,_T("IART"),4) == 0)
		{
			theApp.m_Mp4.SetMetadata_Artist(buf);
		}
		else if(_tcsncmp(szValueName,_T("IPRD"),4) == 0)
		{
			theApp.m_Mp4.SetMetadata_Album(buf);
		}
		else if(_tcsncmp(szValueName,_T("IGRP"),4) == 0)
		{
			theApp.m_Mp4.SetMetadata_Group(buf);
		}
		else if(_tcsncmp(szValueName,_T("COMPOSER"),8) == 0)
		{
			theApp.m_Mp4.SetMetadata_Composer(buf);
		}
		else if(_tcsncmp(szValueName,_T("IGNR"),4) == 0)
		{
			theApp.m_Mp4.SetMetadata_Genre(buf);
		}
		else if(_tcsncmp(szValueName,_T("TRACK1"),6) == 0)
		{
			if(_tcslen(buf))
			{
				theApp.m_Mp4.SetMetadata_Track1(_ttoi(buf));
			}
			else
			{
				theApp.m_Mp4.SetMetadata_Track1(-1);
			}
		}
		else if(_tcsncmp(szValueName,_T("TRACK2"),6) == 0)
		{
			if(_tcslen(buf))
			{
				theApp.m_Mp4.SetMetadata_Track2(_ttoi(buf));
			}
			else
			{
				theApp.m_Mp4.SetMetadata_Track2(-1);
			}
		}
		else if(_tcsncmp(szValueName,_T("DISC1"),5) == 0)
		{
			if(_tcslen(buf))
			{
				theApp.m_Mp4.SetMetadata_Disc1(_ttoi(buf));
			}
			else
			{
				theApp.m_Mp4.SetMetadata_Disc1(-1);
			}
		}
		else if(_tcsncmp(szValueName,_T("DISC2"),5) == 0)
		{
			if(_tcslen(buf))
			{
				theApp.m_Mp4.SetMetadata_Disc2(_ttoi(buf));
			}
			else
			{
				theApp.m_Mp4.SetMetadata_Disc2(-1);
			}
		}
		else if(_tcsncmp(szValueName,_T("BPM"),3) == 0)
		{
			if(_tcslen(buf))
			{
				theApp.m_Mp4.SetMetadata_Tempo(_ttoi(buf));
			}
			else
			{
				theApp.m_Mp4.SetMetadata_Tempo(-1);
			}
		}
		else if(_tcsncmp(szValueName,_T("ICRD"),4) == 0)
		{
			theApp.m_Mp4.SetMetadata_Year(buf);
		}
		else if(_tcsncmp(szValueName,_T("COMPILATION"),11) == 0)
		{
			if(_tcslen(buf))
			{
				theApp.m_Mp4.SetMetadata_Compilation(_ttoi(buf));
			}
			else
			{
				theApp.m_Mp4.SetMetadata_Compilation(-1);
			}
		}
		else if(_tcsncmp(szValueName,_T("ICMT"),4) == 0)
		{
			theApp.m_Mp4.SetMetadata_Comment(buf);
		}
		else if(_tcsncmp(szValueName,_T("TOOL"),4) == 0)
		{
			theApp.m_Mp4.SetMetadata_Tool(buf);
		}
		break;
	case CShellExt::UNKNOWN:
	default:
		return -1;
	}

	return ERROR_SUCCESS;
}

#ifdef UNICODE
extern "C" DWORD __stdcall mp3infp_SaveA(const char *szFileName)
{
	return mp3infp_SaveW(CString(szFileName));
}
#endif

extern "C" DWORD __stdcall mp3infp_Save(LPCTSTR szFileName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	DWORD dwRet = -1;

	FILETIME	fileTime;	//タイムスタンプ保存用
	FILETIME	createTime;	//タイムスタンプ保存用
	//タイムスタンプを保存
	HANDLE hFile = CreateFile(
						szFileName,
						GENERIC_READ,
						FILE_SHARE_READ|FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING,	//ファイルをオープンします。指定ファイルが存在していない場合、関数は失敗します。
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return GetLastError();
	}
	GetFileTime(hFile,&createTime,NULL,&fileTime);
	CloseHandle(hFile);
	
	switch(theApp.m_fileType){
	case CShellExt::MP3:
		if(theApp.m_Id3tagv1.IsEnable())
		{
			dwRet = theApp.m_Id3tagv1.Save(NULL,szFileName);
			if(dwRet != ERROR_SUCCESS)
			{
				break;
			}
		}
		if(theApp.m_Id3tagv2.IsEnable())
		{
			dwRet = theApp.m_Id3tagv2.Save(szFileName);
			if(dwRet != ERROR_SUCCESS)
			{
				break;
			}
		}
		else if(theApp.m_Rmp3.IsEnable())
		{
			dwRet = theApp.m_Rmp3.Save(NULL,szFileName);
			if(dwRet != ERROR_SUCCESS)
			{
				break;
			}
		}
		if(theApp.m_Ape.IsEnable())
		{
			dwRet = theApp.m_Ape.Save(szFileName);
			if(dwRet != ERROR_SUCCESS)
			{
				break;
			}
		}
		break;
	case CShellExt::WAVE:
		dwRet = theApp.m_RiffSIF.Save(NULL,szFileName);
		break;
	case CShellExt::AVI:
		dwRet = theApp.m_OpenDML.Save(NULL,szFileName);
		break;
	case CShellExt::VQF:
		dwRet = theApp.m_Vqf.Save(NULL,szFileName);
		break;
	case CShellExt::WMA:
		dwRet = theApp.m_Wma.Save(szFileName);
		break;
	case CShellExt::OGG:
		dwRet = theApp.m_Ogg.Save(szFileName);
		break;
	case CShellExt::APE:
		dwRet = theApp.m_Ape.Save(szFileName);
		break;
	case CShellExt::MP4:
		dwRet = theApp.m_Mp4.Save(szFileName);
		break;
	case CShellExt::UNKNOWN:
	default:
		return FALSE;
	}

	//タイムスタンプを復元
	hFile = CreateFile(
						szFileName,
						GENERIC_READ|GENERIC_WRITE,
						FILE_SHARE_READ|FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING,	//ファイルをオープンします。指定ファイルが存在していない場合、関数は失敗します。
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		SetFileTime(hFile,&createTime,NULL,&fileTime);
		CloseHandle(hFile);
	}

	//シェルに変更を通知
	SHChangeNotify(SHCNE_UPDATEITEM,SHCNF_FLUSH|SHCNF_PATH,szFileName,NULL);

	return dwRet;
}

#ifdef UNICODE
extern "C" DWORD __stdcall mp3infp_mp3_MakeId3v1A(const char *szFileName)
{
	return mp3infp_mp3_MakeId3v1W(CString(szFileName));
}
#endif

extern "C" DWORD __stdcall mp3infp_mp3_MakeId3v1(LPCTSTR szFileName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	DWORD dwRet = -1;

	if(theApp.m_fileType != CShellExt::MP3)
	{
		return -1;
	}
	if(theApp.m_Id3tagv1.IsEnable())
	{
		return -1;	// 既にid3v1が存在します
	}

	FILETIME	fileTime;	//タイムスタンプ保存用
	FILETIME	createTime;	//タイムスタンプ保存用
	//タイムスタンプを保存
	HANDLE hFile = CreateFile(
						szFileName,
						GENERIC_READ,
						FILE_SHARE_READ|FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING,	//ファイルをオープンします。指定ファイルが存在していない場合、関数は失敗します。
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return GetLastError();
	}
	GetFileTime(hFile,&createTime,NULL,&fileTime);
	CloseHandle(hFile);
	
	//ID3TAGを作成
	if(theApp.m_Rmp3.IsEnable())
	{
		theApp.m_Rmp3.SetHasId3tag(TRUE);
		dwRet = theApp.m_Rmp3.Save(NULL,szFileName);
		if(dwRet != ERROR_SUCCESS)
		{
			theApp.m_Rmp3.SetHasId3tag(FALSE);
		}
	}
	else
	{
		dwRet = theApp.m_Id3tagv1.MakeTag(NULL,szFileName);
	}

	//タイムスタンプを復元
	hFile = CreateFile(
						szFileName,
						GENERIC_READ|GENERIC_WRITE,
						FILE_SHARE_READ|FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING,	//ファイルをオープンします。指定ファイルが存在していない場合、関数は失敗します。
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		SetFileTime(hFile,&createTime,NULL,&fileTime);
		CloseHandle(hFile);
	}

	//シェルに変更を通知
	SHChangeNotify(SHCNE_UPDATEITEM,SHCNF_FLUSH|SHCNF_PATH,szFileName,NULL);
	
	return dwRet;
}

#ifdef UNICODE
extern "C" DWORD __stdcall mp3infp_mp3_DelId3v1A(const char *szFileName)
{
	return mp3infp_mp3_DelId3v1W(CString(szFileName));
}
#endif

extern "C" DWORD __stdcall mp3infp_mp3_DelId3v1(LPCTSTR szFileName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	DWORD dwRet = -1;

	if(!theApp.m_Id3tagv1.IsEnable())
	{
		return -1;	// id3v1が存在しません
	}

	FILETIME	fileTime;	//タイムスタンプ保存用
	FILETIME	createTime;	//タイムスタンプ保存用
	//タイムスタンプを保存
	HANDLE hFile = CreateFile(
						szFileName,
						GENERIC_READ,
						FILE_SHARE_READ|FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING,	//ファイルをオープンします。指定ファイルが存在していない場合、関数は失敗します。
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return GetLastError();
	}
	GetFileTime(hFile,&createTime,NULL,&fileTime);
	CloseHandle(hFile);
	
	//ID3TAGを消去
	if(theApp.m_Rmp3.IsEnable())
	{
		BOOL bHasBack = theApp.m_Rmp3.HasId3tag();
		theApp.m_Rmp3.SetHasId3tag(FALSE);
		dwRet = theApp.m_Rmp3.Save(NULL,szFileName);
		if(dwRet != ERROR_SUCCESS)
		{
			// 書込みに失敗したら設定を戻しておく
			theApp.m_Rmp3.SetHasId3tag(bHasBack);
		}
		theApp.m_Id3tagv1.Load(szFileName);
	}
	else
	{
		dwRet = theApp.m_Id3tagv1.DelTag(NULL,szFileName);
	}

	//タイムスタンプを復元
	hFile = CreateFile(
						szFileName,
						GENERIC_READ|GENERIC_WRITE,
						FILE_SHARE_READ|FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING,	//ファイルをオープンします。指定ファイルが存在していない場合、関数は失敗します。
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		SetFileTime(hFile,&createTime,NULL,&fileTime);
		CloseHandle(hFile);
	}

	//シェルに変更を通知
	SHChangeNotify(SHCNE_UPDATEITEM,SHCNF_FLUSH|SHCNF_PATH,szFileName,NULL);
	
	return ERROR_SUCCESS;
}

#ifdef UNICODE
extern "C" DWORD __stdcall mp3infp_mp3_MakeId3v2A(const char *szFileName)
{
	return mp3infp_mp3_MakeId3v2W(CString(szFileName));
}
#endif

extern "C" DWORD __stdcall mp3infp_mp3_MakeId3v2(LPCTSTR szFileName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	DWORD dwRet = -1;

	if(theApp.m_Rmp3.IsEnable())
	{
		//「現在RMP形式のため、ID3v2 TAGを作成できません。Riff SIFを削除してください。」
		return -1;
	}
	if(theApp.m_Id3tagv2.IsEnable())
	{
		return -1;	// 既にid3v2が存在します
	}

	//ファイルが書き込み可能か調べる
	if(GetFileAttributes(szFileName) & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY))
	{
		//「このファイルは「読み込み専用」です」
		return -1;
	}
	
	FILETIME	fileTime;	//タイムスタンプ保存用
	FILETIME	createTime;	//タイムスタンプ保存用
	//タイムスタンプを保存
	HANDLE hFile = CreateFile(
						szFileName,
						GENERIC_READ,
						FILE_SHARE_READ|FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING,	//ファイルをオープンします。指定ファイルが存在していない場合、関数は失敗します。
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return GetLastError();
	}
	GetFileTime(hFile,&createTime,NULL,&fileTime);
	CloseHandle(hFile);
	
	//ID3TAGを作成
	dwRet = theApp.m_Id3tagv2.MakeTag(szFileName);
	
	//タイムスタンプを復元
	hFile = CreateFile(
						szFileName,
						GENERIC_READ|GENERIC_WRITE,
						FILE_SHARE_READ|FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING,	//ファイルをオープンします。指定ファイルが存在していない場合、関数は失敗します。
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		SetFileTime(hFile,&createTime,NULL,&fileTime);
		CloseHandle(hFile);
	}

	//シェルに変更を通知
	SHChangeNotify(SHCNE_UPDATEITEM,SHCNF_FLUSH|SHCNF_PATH,szFileName,NULL);
	
	return ERROR_SUCCESS;
}

#ifdef UNICODE
extern "C" DWORD __stdcall mp3infp_mp3_DelId3v2A(const char *szFileName)
{
	return mp3infp_mp3_DelId3v2W(CString(szFileName));
}
#endif

extern "C" DWORD __stdcall mp3infp_mp3_DelId3v2(LPCTSTR szFileName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	DWORD dwRet = -1;

	//ファイルが書き込み可能か調べる
	if(GetFileAttributes(szFileName) & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY))
	{
		//「このファイルは「読み込み専用」です」
		return -1;
	}
	if(!theApp.m_Id3tagv2.IsEnable())
	{
		return -1;	// 既にid3v2が存在しない
	}

	FILETIME	fileTime;	//タイムスタンプ保存用
	FILETIME	createTime;	//タイムスタンプ保存用
	//タイムスタンプを保存
	HANDLE hFile = CreateFile(
						szFileName,
						GENERIC_READ,
						FILE_SHARE_READ|FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING,	//ファイルをオープンします。指定ファイルが存在していない場合、関数は失敗します。
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return GetLastError();
	}
	GetFileTime(hFile,&createTime,NULL,&fileTime);
	CloseHandle(hFile);
	
	//ID3TAGを消去
	dwRet = theApp.m_Id3tagv2.DelTag(szFileName);
	
	//タイムスタンプを復元
	hFile = CreateFile(
						szFileName,
						GENERIC_READ|GENERIC_WRITE,
						FILE_SHARE_READ|FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING,	//ファイルをオープンします。指定ファイルが存在していない場合、関数は失敗します。
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		SetFileTime(hFile,&createTime,NULL,&fileTime);
		CloseHandle(hFile);
	}

	//シェルに変更を通知
	SHChangeNotify(SHCNE_UPDATEITEM,SHCNF_FLUSH|SHCNF_PATH,szFileName,NULL);
	
	return ERROR_SUCCESS;
}

#ifdef UNICODE
extern "C" DWORD __stdcall mp3infp_mp3_MakeRMPA(const char *szFileName)
{
	return mp3infp_mp3_MakeRMPW(CString(szFileName));
}
#endif

extern "C" DWORD __stdcall mp3infp_mp3_MakeRMP(LPCTSTR szFileName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	DWORD dwRet = -1;

	if(theApp.m_Id3tagv2.IsEnable())
	{
		//「ID3v2 TAGがあるため、RMP形式に変換できません。」
		return -1;
	}
	if(theApp.m_Rmp3.IsEnable())
	{
		return -1;	// 既にRiff形式です
	}

	//ファイルが書き込み可能か調べる
	if(GetFileAttributes(szFileName) & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY))
	{
		//「このファイルは「読み込み専用」です」
		return -1;
	}

	FILETIME	fileTime;	//タイムスタンプ保存用
	FILETIME	createTime;	//タイムスタンプ保存用
	//タイムスタンプを保存
	HANDLE hFile = CreateFile(
						szFileName,
						GENERIC_READ,
						FILE_SHARE_READ|FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING,	//ファイルをオープンします。指定ファイルが存在していない場合、関数は失敗します。
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return GetLastError();
	}
	GetFileTime(hFile,&createTime,NULL,&fileTime);
	CloseHandle(hFile);
	
	//Riff SIFを作成
	dwRet = theApp.m_Rmp3.MakeTag(NULL,szFileName);
	
	theApp.m_Id3tagv1.Load(szFileName);
	theApp.m_Id3tagv2.Load(szFileName);

	//タイムスタンプを復元
	hFile = CreateFile(
						szFileName,
						GENERIC_READ|GENERIC_WRITE,
						FILE_SHARE_READ|FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING,	//ファイルをオープンします。指定ファイルが存在していない場合、関数は失敗します。
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		SetFileTime(hFile,&createTime,NULL,&fileTime);
		CloseHandle(hFile);
	}

	//シェルに変更を通知
	SHChangeNotify(SHCNE_UPDATEITEM,SHCNF_FLUSH|SHCNF_PATH,szFileName,NULL);
	
	return ERROR_SUCCESS;
}

#ifdef UNICODE
extern "C" DWORD __stdcall mp3infp_mp3_DelRMPA(const char *szFileName)
{
	return mp3infp_mp3_DelRMPW(CString(szFileName));
}
#endif

extern "C" DWORD __stdcall mp3infp_mp3_DelRMP(LPCTSTR szFileName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	DWORD dwRet = -1;

	//ファイルが書き込み可能か調べる
	if(GetFileAttributes(szFileName) & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY))
	{
		//「このファイルは「読み込み専用」です」
		return -1;
	}
	if(!theApp.m_Rmp3.IsEnable())
	{
		return -1;	// Riff形式ではありません
	}

	FILETIME	fileTime;	//タイムスタンプ保存用
	FILETIME	createTime;	//タイムスタンプ保存用
	//タイムスタンプを保存
	HANDLE hFile = CreateFile(
						szFileName,
						GENERIC_READ,
						FILE_SHARE_READ|FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING,	//ファイルをオープンします。指定ファイルが存在していない場合、関数は失敗します。
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return GetLastError();
	}
	GetFileTime(hFile,&createTime,NULL,&fileTime);
	CloseHandle(hFile);
	
	//Riff SIFを消去
	dwRet = theApp.m_Rmp3.DelTag(NULL,szFileName);
	
	//タイムスタンプを復元
	hFile = CreateFile(
						szFileName,
						GENERIC_READ|GENERIC_WRITE,
						FILE_SHARE_READ|FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING,	//ファイルをオープンします。指定ファイルが存在していない場合、関数は失敗します。
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		SetFileTime(hFile,&createTime,NULL,&fileTime);
		CloseHandle(hFile);
	}

	//シェルに変更を通知
	SHChangeNotify(SHCNE_UPDATEITEM,SHCNF_FLUSH|SHCNF_PATH,szFileName,NULL);
	
	return ERROR_SUCCESS;
}

#ifdef UNICODE
extern "C" DWORD __stdcall mp3infp_mp3_MakeApeTagA(const char *szFileName)
{
	return mp3infp_mp3_MakeApeTagW(CString(szFileName));
}
#endif

extern "C" DWORD __stdcall mp3infp_mp3_MakeApeTag(LPCTSTR szFileName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	DWORD dwRet = -1;

	if(theApp.m_Rmp3.IsEnable())
	{
		//「現在RMP形式のため、APE TAGを作成できません。Riff SIFを削除してください。」
		return -1;
	}
	if(theApp.m_Ape.IsEnable())
	{
		return -1;	// 既にAPE Tagが存在します
	}

	//ファイルが書き込み可能か調べる
	if(GetFileAttributes(szFileName) & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY))
	{
		//「このファイルは「読み込み専用」です」
		return -1;
	}
	
	FILETIME	fileTime;	//タイムスタンプ保存用
	FILETIME	createTime;	//タイムスタンプ保存用
	//タイムスタンプを保存
	HANDLE hFile = CreateFile(
						szFileName,
						GENERIC_READ,
						FILE_SHARE_READ|FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING,	//ファイルをオープンします。指定ファイルが存在していない場合、関数は失敗します。
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return GetLastError();
	}
	GetFileTime(hFile,&createTime,NULL,&fileTime);
	CloseHandle(hFile);
	
	//ID3TAGを作成
	dwRet = theApp.m_Ape.MakeTag(szFileName);
	
	//タイムスタンプを復元
	hFile = CreateFile(
						szFileName,
						GENERIC_READ|GENERIC_WRITE,
						FILE_SHARE_READ|FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING,	//ファイルをオープンします。指定ファイルが存在していない場合、関数は失敗します。
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		SetFileTime(hFile,&createTime,NULL,&fileTime);
		CloseHandle(hFile);
	}

	//シェルに変更を通知
	SHChangeNotify(SHCNE_UPDATEITEM,SHCNF_FLUSH|SHCNF_PATH,szFileName,NULL);
	
	return ERROR_SUCCESS;
}

#ifdef UNICODE
extern "C" DWORD __stdcall mp3infp_mp3_DelApeTagA(const char *szFileName)
{
	return mp3infp_mp3_DelApeTagW(CString(szFileName));
}
#endif

extern "C" DWORD __stdcall mp3infp_mp3_DelApeTag(LPCTSTR szFileName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	DWORD dwRet = -1;

	//ファイルが書き込み可能か調べる
	if(GetFileAttributes(szFileName) & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY))
	{
		//「このファイルは「読み込み専用」です」
		return -1;
	}
	if(!theApp.m_Ape.IsEnable())
	{
		return -1;	// 既にid3v2が存在しない
	}

	FILETIME	fileTime;	//タイムスタンプ保存用
	FILETIME	createTime;	//タイムスタンプ保存用
	//タイムスタンプを保存
	HANDLE hFile = CreateFile(
						szFileName,
						GENERIC_READ,
						FILE_SHARE_READ|FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING,	//ファイルをオープンします。指定ファイルが存在していない場合、関数は失敗します。
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return GetLastError();
	}
	GetFileTime(hFile,&createTime,NULL,&fileTime);
	CloseHandle(hFile);
	
	//APE Tagを消去
	dwRet = theApp.m_Ape.DelTag(szFileName);
	
	//タイムスタンプを復元
	hFile = CreateFile(
						szFileName,
						GENERIC_READ|GENERIC_WRITE,
						FILE_SHARE_READ|FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING,	//ファイルをオープンします。指定ファイルが存在していない場合、関数は失敗します。
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		SetFileTime(hFile,&createTime,NULL,&fileTime);
		CloseHandle(hFile);
	}

	//シェルに変更を通知
	SHChangeNotify(SHCNE_UPDATEITEM,SHCNF_FLUSH|SHCNF_PATH,szFileName,NULL);
	
	return ERROR_SUCCESS;
}

