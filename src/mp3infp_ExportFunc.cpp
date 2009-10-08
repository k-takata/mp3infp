#include "stdafx.h"
#include "mp3infp.h"

extern "C" DWORD __stdcall mp3infp_GetVer()
{
	return 0x1000*(MAJOR_REL/10)+0x0100*(MAJOR_REL%10)+0x0010*(MINOR_REL/10)+0x0001*(MINOR_REL%10);
}

static CString getFileType(LPCTSTR szFileName,DWORD dwPage)
{
	CString ret;

	if((stricmp(getExtName(szFileName),".mp3") == 0) ||
		(stricmp(getExtName(szFileName),".mp2") == 0) ||
		(stricmp(getExtName(szFileName),".mp1") == 0) ||
		(stricmp(getExtName(szFileName),".rmp") == 0) )
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
			return "";
		}
		while(1)	//ダミー
		{
			//id3v2タグを捜す
			char buf[16];
			if(!ReadFile(hFile,&buf,16,&dwRet,NULL))
			{
				CloseHandle(hFile);
				return "";
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
				return "";
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
				return "";
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
				return "";
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
			ret= "ID3v2";
		}
		else if(bApe)
		{
			ret= "APE";
		}
		else if(bRiffSIF)
		{
			ret= "Riff SIF";
		}
		else if(bId3v1)
		{
			ret= "ID3v1";
		}
		else
		{
			switch(dwPage){
			default:
			case 0:
				ret= "ID3v1(*)";
				break;
			case 1:
				ret= "ID3v2(*)";
				break;
			case 2:
				ret= "Riff SIF(*)";
				break;
			case 3:
				ret= "APE(*)";
				break;
			}
		}
	}

	return ret;
}

extern "C" int __stdcall mp3infp_ViewPropEx(HWND hWnd,const char *szFileName,DWORD dwPage,BOOL modeless,DWORD param1,DWORD param2)
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

extern "C" BOOL __stdcall mp3infp_ViewProp(HWND hWnd,const char *szFileName,DWORD dwPage)
{
	SHELLEXECUTEINFO execinfo;
	memset(&execinfo,0,sizeof(SHELLEXECUTEINFO));
	execinfo.cbSize		= sizeof(SHELLEXECUTEINFO);
	execinfo.fMask		= SEE_MASK_INVOKEIDLIST;
	execinfo.lpVerb		= "properties";
	execinfo.lpFile		= szFileName;
	execinfo.lpParameters= "mp3infp";
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

extern "C" DWORD __stdcall mp3infp_Load(HWND hWnd,const char *szFileName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	DWORD dwRet = ERROR_SUCCESS;
	theApp.m_strSelectFile = szFileName;
	theApp.m_fileType = CShellExt::UNKNOWN;

	theApp.m_strAudioFormat = "";
	theApp.m_strVideoFormat = "";
	theApp.m_strStreamFormat = "";
	theApp.m_strTime = "";

	switch(CShellExt::GetFileType((char *)szFileName)){
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
			GetWaveAudioFormat((LPCSTR )theApp.m_strSelectFile,
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
			GetAviFormat((LPCSTR )theApp.m_strSelectFile,
							theApp.m_strAudioFormat,
							theApp.m_strVideoFormat,
							theApp.m_strStreamFormat,
							theApp.m_strTime,
							theApp.m_bAvi2,
							theApp.m_iAviCodecFind);
			theApp.m_fileType = CShellExt::AVI;
		}
		theApp.m_OpenDML.SetJunkHeader("This file was made by "SOFT_NAME);
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

extern "C" BOOL __stdcall mp3infp_GetValue(const char *szValueName,char **buf)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	static CString strRet;
	DWORD 	dwSize;
	*buf = NULL;
	strRet = "";
	switch(theApp.m_fileType){
	case CShellExt::MP3:
		if(strcmp(szValueName,"FILE") == 0)
		{
			strRet = getFileName(theApp.m_strSelectFile);
		}
		else if(strcmp(szValueName,"FEXT") == 0)
		{
			strRet = getExtName(theApp.m_strSelectFile);
		}
		else if(strcmp(szValueName,"PATH") == 0)
		{
			strRet = getPathName(theApp.m_strSelectFile);
		}
		else if(strcmp(szValueName,"SIZ1") == 0)
		{
			CString strSize;
			CString strSize2;
			strSize.Format("%I64u",theApp.m_i64FileSize);
			strRet = divString((char *)(LPCSTR )strSize,',',3);
		}
		else if(strcmp(szValueName,"SIZK") == 0)
		{
			CString strSize;
			CString strSize2;
			__int64 fsize = (theApp.m_i64FileSize / 1024) + ((theApp.m_i64FileSize % 1024)?1:0);
			strSize.Format("%I64u",fsize);
			strRet = divString((char *)(LPCSTR )strSize,',',3);
		}
		else if(strcmp(szValueName,"SIZM") == 0)
		{
			CString strSize;
			CString strSize2;
			__int64 fsize = (theApp.m_i64FileSize / 1024) + ((theApp.m_i64FileSize % 1024)?1:0);
			fsize = (fsize / 1024) + ((fsize % 1024)?1:0);
			strSize.Format("%I64u",fsize);
			strRet = divString((char *)(LPCSTR )strSize,',',3);
		}
		else if(strcmp(szValueName,"AFMT") == 0)
		{
			strRet = theApp.m_Mp3info.GetFormatString();
		}
		else if(strcmp(szValueName,"TIME") == 0)
		{
			strRet = theApp.m_Mp3info.GetTimeString();
		}
		else if(strcmp(szValueName,"INAM_v1") == 0)
		{
			strRet = theApp.m_Id3tagv1.GetTitle();
		}
		else if(strcmp(szValueName,"IART_v1") == 0)
		{
			strRet = theApp.m_Id3tagv1.GetArtist();
		}
		else if(strcmp(szValueName,"IPRD_v1") == 0)
		{
			strRet = theApp.m_Id3tagv1.GetAlbum();
		}
		else if(strcmp(szValueName,"ICMT_v1") == 0)
		{
			strRet = theApp.m_Id3tagv1.GetComment();
		}
		else if(strcmp(szValueName,"ICRD_v1") == 0)
		{
			strRet = theApp.m_Id3tagv1.GetYear();
		}
		else if(strcmp(szValueName,"IGNR_v1") == 0)
		{
			strRet = theApp.m_Id3tagv1.GetGenre();
		}
		else if(strcmp(szValueName,"TRACK_v1") == 0)
		{
			strRet = theApp.m_Id3tagv1.GetTrackNo();
		}
		else if(strcmp(szValueName,"INAM_v2") == 0)
		{
			strRet = theApp.m_Id3tagv2.GetTitle();
		}
		else if(strcmp(szValueName,"IART_v2") == 0)
		{
			strRet = theApp.m_Id3tagv2.GetArtist();
		}
		else if(strcmp(szValueName,"IPRD_v2") == 0)
		{
			strRet = theApp.m_Id3tagv2.GetAlbum();
		}
		else if(strcmp(szValueName,"ICMT_v2") == 0)
		{
			strRet = theApp.m_Id3tagv2.GetComment();
		}
		else if(strcmp(szValueName,"ICRD_v2") == 0)
		{
			strRet = theApp.m_Id3tagv2.GetYear();
		}
		else if(strcmp(szValueName,"IGNR_v2") == 0)
		{
			strRet = theApp.m_Id3tagv2.GetGenre();
		}
		else if(strcmp(szValueName,"ICOP_v2") == 0)
		{
			strRet = theApp.m_Id3tagv2.GetCopyright();
		}
		else if(strcmp(szValueName,"ISFT_v2") == 0)
		{
			strRet = theApp.m_Id3tagv2.GetEncoder();
		}
		else if(strcmp(szValueName,"COMP_v2") == 0)
		{
			strRet = theApp.m_Id3tagv2.GetComposer();
		}
		else if(strcmp(szValueName,"OART_v2") == 0)
		{
			strRet = theApp.m_Id3tagv2.GetOrigArtist();
		}
		else if(strcmp(szValueName,"URL_v2") == 0)
		{
			strRet = theApp.m_Id3tagv2.GetUrl();
		}
		else if(strcmp(szValueName,"TRACK_v2") == 0)
		{
			strRet = theApp.m_Id3tagv2.GetTrackNo();
		}
		else if(strcmp(szValueName,"ENC2_v2") == 0)
		{
			strRet = theApp.m_Id3tagv2.GetEncodest();
		}
		else if(strcmp(szValueName,"INAM_rmp") == 0)
		{
			strRet = theApp.m_Rmp3.GetNAM();
		}
		else if(strcmp(szValueName,"IART_rmp") == 0)
		{
			strRet = theApp.m_Rmp3.GetART();
		}
		else if(strcmp(szValueName,"IPRD_rmp") == 0)
		{
			strRet = theApp.m_Rmp3.GetPRD();
		}
		else if(strcmp(szValueName,"ICMT_rmp") == 0)
		{
			strRet = theApp.m_Rmp3.GetCMT();
		}
		else if(strcmp(szValueName,"ICRD_rmp") == 0)
		{
			strRet = theApp.m_Rmp3.GetCRD();
		}
		else if(strcmp(szValueName,"IGNR_rmp") == 0)
		{
			strRet = theApp.m_Rmp3.GetGNR();
		}
		else if(strcmp(szValueName,"ICOP_rmp") == 0)
		{
			strRet = theApp.m_Rmp3.GetCOP();
		}
		else if(strcmp(szValueName,"ISFT_rmp") == 0)
		{
			strRet = theApp.m_Rmp3.GetSFT();
		}
		else if(strcmp(szValueName,"ISRC_rmp") == 0)
		{
			strRet = theApp.m_Rmp3.GetSRC();
		}
		else if(strcmp(szValueName,"IENG_rmp") == 0)
		{
			strRet = theApp.m_Rmp3.GetENG();
		}
		else if(strcmp(szValueName,"INAM_APE") == 0)
		{
			theApp.m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_TITLE,strRet);
		}
		else if(strcmp(szValueName,"TRACK_APE") == 0)
		{
			theApp.m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_TRACK,strRet);
		}
		else if(strcmp(szValueName,"IART_APE") == 0)
		{
			theApp.m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_ARTIST,strRet);
		}
		else if(strcmp(szValueName,"IPRD_APE") == 0)
		{
			theApp.m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_ALBUM,strRet);
		}
		else if(strcmp(szValueName,"ICMT_APE") == 0)
		{
			theApp.m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_COMMENT,strRet);
		}
		else if(strcmp(szValueName,"ICRD_APE") == 0)
		{
			theApp.m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_YEAR,strRet);
		}
		else if(strcmp(szValueName,"IGNR_APE") == 0)
		{
			theApp.m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_GENRE,strRet);
		}
		break;
	case CShellExt::WAVE:
		if(strcmp(szValueName,"FILE") == 0)
		{
			strRet = getFileName(theApp.m_strSelectFile);
		}
		else if(strcmp(szValueName,"FEXT") == 0)
		{
			strRet = getExtName(theApp.m_strSelectFile);
		}
		else if(strcmp(szValueName,"PATH") == 0)
		{
			strRet = getPathName(theApp.m_strSelectFile);
		}
		else if(strcmp(szValueName,"SIZ1") == 0)
		{
			CString strSize;
			CString strSize2;
			strSize.Format("%I64u",theApp.m_i64FileSize);
			strRet = divString((char *)(LPCSTR )strSize,',',3);
		}
		else if(strcmp(szValueName,"SIZK") == 0)
		{
			CString strSize;
			CString strSize2;
			__int64 fsize = (theApp.m_i64FileSize / 1024) + ((theApp.m_i64FileSize % 1024)?1:0);
			strSize.Format("%I64u",fsize);
			strRet = divString((char *)(LPCSTR )strSize,',',3);
		}
		else if(strcmp(szValueName,"SIZM") == 0)
		{
			CString strSize;
			CString strSize2;
			__int64 fsize = (theApp.m_i64FileSize / 1024) + ((theApp.m_i64FileSize % 1024)?1:0);
			fsize = (fsize / 1024) + ((fsize % 1024)?1:0);
			strSize.Format("%I64u",fsize);
			strRet = divString((char *)(LPCSTR )strSize,',',3);
		}
		else if(strcmp(szValueName,"AFMT") == 0)
		{
			strRet = theApp.m_strAudioFormat;
		}
		else if(strcmp(szValueName,"TIME") == 0)
		{
			strRet = theApp.m_strTime;
		}
		else if(theApp.m_RiffSIF.IsEnable())
		{
			if(strcmp(szValueName,"INAM") == 0)
			{
				strRet = theApp.m_RiffSIF.GetField('I','N','A','M');
			}
			else if(strcmp(szValueName,"ISBJ") == 0)
			{
				strRet = theApp.m_RiffSIF.GetField('I','S','B','J');
			}
			else if(strcmp(szValueName,"IART") == 0)
			{
				strRet = theApp.m_RiffSIF.GetField('I','A','R','T');
			}
			else if(strcmp(szValueName,"IPRD") == 0)
			{
				strRet = theApp.m_RiffSIF.GetField('I','P','R','D');
			}
			else if(strcmp(szValueName,"ICMT") == 0)
			{
				strRet = theApp.m_RiffSIF.GetField('I','C','M','T');
			}
			else if(strcmp(szValueName,"ICRD") == 0)
			{
				strRet = theApp.m_RiffSIF.GetField('I','C','R','D');
			}
			else if(strcmp(szValueName,"IGNR") == 0)
			{
				strRet = theApp.m_RiffSIF.GetField('I','G','N','R');
			}
			else if(strcmp(szValueName,"ICOP") == 0)
			{
				strRet = theApp.m_RiffSIF.GetField('I','C','O','P');
			}
			else if(strcmp(szValueName,"ISFT") == 0)
			{
				strRet = theApp.m_RiffSIF.GetField('I','S','F','T');
			}
			else if(strcmp(szValueName,"ISRC") == 0)
			{
				strRet = theApp.m_RiffSIF.GetField('I','S','R','C');
			}
			else if(strcmp(szValueName,"IENG") == 0)
			{
				strRet = theApp.m_RiffSIF.GetField('I','E','N','G');
			}
		}
		break;
	case CShellExt::AVI:
		if(strcmp(szValueName,"FILE") == 0)
		{
			strRet = getFileName(theApp.m_strSelectFile);
		}
		else if(strcmp(szValueName,"FEXT") == 0)
		{
			strRet = getExtName(theApp.m_strSelectFile);
		}
		else if(strcmp(szValueName,"PATH") == 0)
		{
			strRet = getPathName(theApp.m_strSelectFile);
		}
		else if(strcmp(szValueName,"SIZ1") == 0)
		{
			CString strSize;
			CString strSize2;
			strSize.Format("%I64u",theApp.m_i64FileSize);
			strRet = divString((char *)(LPCSTR )strSize,',',3);
		}
		else if(strcmp(szValueName,"SIZK") == 0)
		{
			CString strSize;
			CString strSize2;
			__int64 fsize = (theApp.m_i64FileSize / 1024) + ((theApp.m_i64FileSize % 1024)?1:0);
			strSize.Format("%I64u",fsize);
			strRet = divString((char *)(LPCSTR )strSize,',',3);
		}
		else if(strcmp(szValueName,"SIZM") == 0)
		{
			CString strSize;
			CString strSize2;
			__int64 fsize = (theApp.m_i64FileSize / 1024) + ((theApp.m_i64FileSize % 1024)?1:0);
			fsize = (fsize / 1024) + ((fsize % 1024)?1:0);
			strSize.Format("%I64u",fsize);
			strRet = divString((char *)(LPCSTR )strSize,',',3);
		}
		else if(strcmp(szValueName,"AFMT") == 0)
		{
			strRet = theApp.m_strAudioFormat;
		}
		else if(strcmp(szValueName,"VFMT") == 0)
		{
			strRet = theApp.m_strVideoFormat;
			strRet += ", ";
			strRet += theApp.m_strStreamFormat;
		}
		else if(strcmp(szValueName,"TIME") == 0)
		{
			strRet = theApp.m_strTime;
		}
		else if(theApp.m_OpenDML.IsEnable())
		{
			if(strcmp(szValueName,"ISBJ") == 0)
			{
				strRet = theApp.m_OpenDML.GetField('I','S','B','J');
			}
			else if(strcmp(szValueName,"INAM") == 0)
			{
				strRet = theApp.m_OpenDML.GetField('I','N','A','M');
			}
			else if(strcmp(szValueName,"IART") == 0)
			{
				strRet = theApp.m_OpenDML.GetField('I','A','R','T');
			}
			else if(strcmp(szValueName,"ICMT") == 0)
			{
				strRet = theApp.m_OpenDML.GetField('I','C','M','T');
			}
			else if(strcmp(szValueName,"ICRD") == 0)
			{
				strRet = theApp.m_OpenDML.GetField('I','C','R','D');
			}
			else if(strcmp(szValueName,"IGNR") == 0)
			{
				strRet = theApp.m_OpenDML.GetField('I','G','N','R');
			}
			else if(strcmp(szValueName,"ICOP") == 0)
			{
				strRet = theApp.m_OpenDML.GetField('I','C','O','P');
			}
			else if(strcmp(szValueName,"ISFT") == 0)
			{
				strRet = theApp.m_OpenDML.GetField('I','S','F','T');
			}
			else if(strcmp(szValueName,"ISRC") == 0)
			{
				strRet = theApp.m_OpenDML.GetField('I','S','R','C');
			}
			else if(strcmp(szValueName,"IENG") == 0)
			{
				strRet = theApp.m_OpenDML.GetField('I','E','N','G');
			}
			else if(strcmp(szValueName,"AVIV") == 0)
			{
				strRet = theApp.m_bAvi2?"AVI 2.0 (OpenDML)":"AVI 1.0 (VfW)";
			}
		}
		break;
	case CShellExt::VQF:
		if(strcmp(szValueName,"FILE") == 0)
		{
			strRet = getFileName(theApp.m_strSelectFile);
		}
		else if(strcmp(szValueName,"FEXT") == 0)
		{
			strRet = getExtName(theApp.m_strSelectFile);
		}
		else if(strcmp(szValueName,"PATH") == 0)
		{
			strRet = getPathName(theApp.m_strSelectFile);
		}
		else if(strcmp(szValueName,"SIZ1") == 0)
		{
			CString strSize;
			CString strSize2;
			strSize.Format("%I64u",theApp.m_i64FileSize);
			strRet = divString((char *)(LPCSTR )strSize,',',3);
		}
		else if(strcmp(szValueName,"SIZK") == 0)
		{
			CString strSize;
			CString strSize2;
			__int64 fsize = (theApp.m_i64FileSize / 1024) + ((theApp.m_i64FileSize % 1024)?1:0);
			strSize.Format("%I64u",fsize);
			strRet = divString((char *)(LPCSTR )strSize,',',3);
		}
		else if(strcmp(szValueName,"SIZM") == 0)
		{
			CString strSize;
			CString strSize2;
			__int64 fsize = (theApp.m_i64FileSize / 1024) + ((theApp.m_i64FileSize % 1024)?1:0);
			fsize = (fsize / 1024) + ((fsize % 1024)?1:0);
			strSize.Format("%I64u",fsize);
			strRet = divString((char *)(LPCSTR )strSize,',',3);
		}
		else if(!theApp.m_Vqf.IsEnable())
			break;
		if(strcmp(szValueName,"AFMT") == 0)
		{
			strRet = theApp.m_Vqf.GetFormatString();
		}
		else if(strcmp(szValueName,"TIME") == 0)
		{
			strRet = theApp.m_Vqf.GetTimeString();
		}
		else if(strcmp(szValueName,"INAM") == 0)
		{
			strRet = theApp.m_Vqf.GetField('N','A','M','E',&dwSize);
		}
		else if(strcmp(szValueName,"IART") == 0)
		{
			strRet = theApp.m_Vqf.GetField('A','U','T','H',&dwSize);
		}
		else if(strcmp(szValueName,"FILE") == 0)
		{
			strRet = theApp.m_Vqf.GetField('F','I','L','E',&dwSize);
		}
		else if(strcmp(szValueName,"ICOP") == 0)
		{
			strRet = theApp.m_Vqf.GetField('(','c',')',' ',&dwSize);
		}
		else if(strcmp(szValueName,"ICMT") == 0)
		{
			strRet = theApp.m_Vqf.GetField('C','O','M','T',&dwSize);
		}
		break;
	case CShellExt::WMA:
		if(strcmp(szValueName,"FILE") == 0)
		{
			strRet = getFileName(theApp.m_strSelectFile);
		}
		else if(strcmp(szValueName,"FEXT") == 0)
		{
			strRet = getExtName(theApp.m_strSelectFile);
		}
		else if(strcmp(szValueName,"PATH") == 0)
		{
			strRet = getPathName(theApp.m_strSelectFile);
		}
		else if(strcmp(szValueName,"SIZ1") == 0)
		{
			CString strSize;
			CString strSize2;
			strSize.Format("%I64u",theApp.m_i64FileSize);
			strRet = divString((char *)(LPCSTR )strSize,',',3);
		}
		else if(strcmp(szValueName,"SIZK") == 0)
		{
			CString strSize;
			CString strSize2;
			__int64 fsize = (theApp.m_i64FileSize / 1024) + ((theApp.m_i64FileSize % 1024)?1:0);
			strSize.Format("%I64u",fsize);
			strRet = divString((char *)(LPCSTR )strSize,',',3);
		}
		else if(strcmp(szValueName,"SIZM") == 0)
		{
			CString strSize;
			CString strSize2;
			__int64 fsize = (theApp.m_i64FileSize / 1024) + ((theApp.m_i64FileSize % 1024)?1:0);
			fsize = (fsize / 1024) + ((fsize % 1024)?1:0);
			strSize.Format("%I64u",fsize);
			strRet = divString((char *)(LPCSTR )strSize,',',3);
		}
		else if(!theApp.m_Wma.IsEnable())
			break;
		if(strcmp(szValueName,"AFMT") == 0)
		{
			strRet = theApp.m_Wma.GetAudioFormatString();
		}
		else if(strcmp(szValueName,"VFMT") == 0)
		{
			strRet = theApp.m_Wma.GetVideoFormatString();
		}
		else if(strcmp(szValueName,"TIME") == 0)
		{
			strRet = theApp.m_Wma.GetTimeString();
		}
		else if(strcmp(szValueName,"INAM") == 0)
		{
			strRet = theApp.m_Wma.GetNAM();
		}
		else if(strcmp(szValueName,"TRACK") == 0)
		{
			strRet = theApp.m_Wma.GetTRACK();
		}
		else if(strcmp(szValueName,"IART") == 0)
		{
			strRet = theApp.m_Wma.GetART();
		}
		else if(strcmp(szValueName,"IPRD") == 0)
		{
			strRet = theApp.m_Wma.GetPRD();
		}
		else if(strcmp(szValueName,"ICMT") == 0)
		{
			strRet = theApp.m_Wma.GetCMT();
		}
		else if(strcmp(szValueName,"ICRD") == 0)
		{
			strRet = theApp.m_Wma.GetCRD();
		}
		else if(strcmp(szValueName,"IGNR") == 0)
		{
			strRet = theApp.m_Wma.GetGNR();
		}
		else if(strcmp(szValueName,"ICOP") == 0)
		{
			strRet = theApp.m_Wma.GetCOPY();
		}
		else if(strcmp(szValueName,"URL1") == 0)
		{
			strRet = theApp.m_Wma.GetUrl1();
		}
		else if(strcmp(szValueName,"URL2") == 0)
		{
			strRet = theApp.m_Wma.GetUrl2();
		}
		break;
	case CShellExt::OGG:
		if(strcmp(szValueName,"FILE") == 0)
		{
			strRet = getFileName(theApp.m_strSelectFile);
		}
		else if(strcmp(szValueName,"FEXT") == 0)
		{
			strRet = getExtName(theApp.m_strSelectFile);
		}
		else if(strcmp(szValueName,"PATH") == 0)
		{
			strRet = getPathName(theApp.m_strSelectFile);
		}
		else if(strcmp(szValueName,"SIZ1") == 0)
		{
			CString strSize;
			CString strSize2;
			strSize.Format("%I64u",theApp.m_i64FileSize);
			strRet = divString((char *)(LPCSTR )strSize,',',3);
		}
		else if(strcmp(szValueName,"SIZK") == 0)
		{
			CString strSize;
			CString strSize2;
			__int64 fsize = (theApp.m_i64FileSize / 1024) + ((theApp.m_i64FileSize % 1024)?1:0);
			strSize.Format("%I64u",fsize);
			strRet = divString((char *)(LPCSTR )strSize,',',3);
		}
		else if(strcmp(szValueName,"SIZM") == 0)
		{
			CString strSize;
			CString strSize2;
			__int64 fsize = (theApp.m_i64FileSize / 1024) + ((theApp.m_i64FileSize % 1024)?1:0);
			fsize = (fsize / 1024) + ((fsize % 1024)?1:0);
			strSize.Format("%I64u",fsize);
			strRet = divString((char *)(LPCSTR )strSize,',',3);
		}
		else if(!theApp.m_Ogg.IsEnable())
			break;
		if(strncmp(szValueName,"AFMT",4) == 0)
		{
			strRet = theApp.m_Ogg.GetAudioFormatString();
		}
		else if(strncmp(szValueName,"TIME",4) == 0)
		{
			strRet = theApp.m_Ogg.GetTimeString();
		}
		else if(strncmp(szValueName,"INAM",4) == 0)
		{
			theApp.m_Ogg.GetComment("TITLE",0,strRet);
		}
		else if(strncmp(szValueName,"TRACK",5) == 0)
		{
			theApp.m_Ogg.GetComment("TRACKNUMBER",0,strRet);
		}
		else if(strncmp(szValueName,"IART",4) == 0)
		{
			theApp.m_Ogg.GetComment("ARTIST",0,strRet);
		}
		else if(strncmp(szValueName,"IPRD",4) == 0)
		{
			theApp.m_Ogg.GetComment("ALBUM",0,strRet);
		}
		else if(strncmp(szValueName,"ICMT",4) == 0)
		{
			theApp.m_Ogg.GetComment("COMMENT",0,strRet);
		}
		else if(strncmp(szValueName,"ICRD",4) == 0)
		{
			theApp.m_Ogg.GetComment("DATE",0,strRet);
		}
		else if(strncmp(szValueName,"IGNR",4) == 0)
		{
			theApp.m_Ogg.GetComment("GENRE",0,strRet);
		}
		break;
	case CShellExt::APE:
		if(strcmp(szValueName,"FILE") == 0)
		{
			strRet = getFileName(theApp.m_strSelectFile);
		}
		else if(strcmp(szValueName,"FEXT") == 0)
		{
			strRet = getExtName(theApp.m_strSelectFile);
		}
		else if(strcmp(szValueName,"PATH") == 0)
		{
			strRet = getPathName(theApp.m_strSelectFile);
		}
		else if(strcmp(szValueName,"SIZ1") == 0)
		{
			CString strSize;
			CString strSize2;
			strSize.Format("%I64u",theApp.m_i64FileSize);
			strRet = divString((char *)(LPCSTR )strSize,',',3);
		}
		else if(strcmp(szValueName,"SIZK") == 0)
		{
			CString strSize;
			CString strSize2;
			__int64 fsize = (theApp.m_i64FileSize / 1024) + ((theApp.m_i64FileSize % 1024)?1:0);
			strSize.Format("%I64u",fsize);
			strRet = divString((char *)(LPCSTR )strSize,',',3);
		}
		else if(strcmp(szValueName,"SIZM") == 0)
		{
			CString strSize;
			CString strSize2;
			__int64 fsize = (theApp.m_i64FileSize / 1024) + ((theApp.m_i64FileSize % 1024)?1:0);
			fsize = (fsize / 1024) + ((fsize % 1024)?1:0);
			strSize.Format("%I64u",fsize);
			strRet = divString((char *)(LPCSTR )strSize,',',3);
		}
		else if(!theApp.m_Ape.IsEnable())
			break;
		if(strncmp(szValueName,"AFMT",4) == 0)
		{
			strRet = theApp.m_Monkeys.GetAudioFormatString();
		}
		else if(strncmp(szValueName,"TIME",4) == 0)
		{
			strRet = theApp.m_Monkeys.GetTimeString();
		}
		else if(strncmp(szValueName,"INAM",4) == 0)
		{
			theApp.m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_TITLE,strRet);
		}
		else if(strncmp(szValueName,"TRACK",5) == 0)
		{
			theApp.m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_TRACK,strRet);
		}
		else if(strncmp(szValueName,"IART",4) == 0)
		{
			theApp.m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_ARTIST,strRet);
		}
		else if(strncmp(szValueName,"IPRD",4) == 0)
		{
			theApp.m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_ALBUM,strRet);
		}
		else if(strncmp(szValueName,"ICMT",4) == 0)
		{
			theApp.m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_COMMENT,strRet);
		}
		else if(strncmp(szValueName,"ICRD",4) == 0)
		{
			theApp.m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_YEAR,strRet);
		}
		else if(strncmp(szValueName,"IGNR",4) == 0)
		{
			theApp.m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_GENRE,strRet);
		}
		break;
	case CShellExt::MP4:
		if(strcmp(szValueName,"FILE") == 0)
		{
			strRet = getFileName(theApp.m_strSelectFile);
		}
		else if(strcmp(szValueName,"FEXT") == 0)
		{
			strRet = getExtName(theApp.m_strSelectFile);
		}
		else if(strcmp(szValueName,"PATH") == 0)
		{
			strRet = getPathName(theApp.m_strSelectFile);
		}
		else if(strcmp(szValueName,"SIZ1") == 0)
		{
			CString strSize;
			CString strSize2;
			strSize.Format("%I64u",theApp.m_i64FileSize);
			strRet = divString((char *)(LPCSTR )strSize,',',3);
		}
		else if(strcmp(szValueName,"SIZK") == 0)
		{
			CString strSize;
			CString strSize2;
			__int64 fsize = (theApp.m_i64FileSize / 1024) + ((theApp.m_i64FileSize % 1024)?1:0);
			strSize.Format("%I64u",fsize);
			strRet = divString((char *)(LPCSTR )strSize,',',3);
		}
		else if(strcmp(szValueName,"SIZM") == 0)
		{
			CString strSize;
			CString strSize2;
			__int64 fsize = (theApp.m_i64FileSize / 1024) + ((theApp.m_i64FileSize % 1024)?1:0);
			fsize = (fsize / 1024) + ((fsize % 1024)?1:0);
			strSize.Format("%I64u",fsize);
			strRet = divString((char *)(LPCSTR )strSize,',',3);
		}
		else if(!theApp.m_Mp4.IsEnable())
			break;
		if(strcmp(szValueName,"AFMT") == 0)
		{
			strRet = theApp.m_Mp4.GetAudioInfoString();
		}
		else if(strcmp(szValueName,"VFMT") == 0)
		{
			strRet = theApp.m_Mp4.GetVideoInfoString();
		}
		else if(strncmp(szValueName,"INAM",4) == 0)
		{
			strRet = theApp.m_Mp4.GetMetadata_Name();
		}
		else if(strncmp(szValueName,"IART",4) == 0)
		{
			strRet = theApp.m_Mp4.GetMetadata_Artist();
		}
		else if(strncmp(szValueName,"IPRD",4) == 0)
		{
			strRet = theApp.m_Mp4.GetMetadata_Album();
		}
		else if(strncmp(szValueName,"IGRP",4) == 0)
		{
			strRet = theApp.m_Mp4.GetMetadata_Group();
		}
		else if(strncmp(szValueName,"COMPOSER",8) == 0)
		{
			strRet = theApp.m_Mp4.GetMetadata_Composer();
		}
		else if(strncmp(szValueName,"IGNR",4) == 0)
		{
			strRet = theApp.m_Mp4.GetMetadata_Genre();
		}
		else if(strncmp(szValueName,"TRACK1",6) == 0)
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
		else if(strncmp(szValueName,"TRACK2",6) == 0)
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
		else if(strncmp(szValueName,"DISC1",5) == 0)
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
		else if(strncmp(szValueName,"DISC2",5) == 0)
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
		else if(strncmp(szValueName,"BPM",3) == 0)
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
		else if(strncmp(szValueName,"ICRD",4) == 0)
		{
			strRet = theApp.m_Mp4.GetMetadata_Year();
		}
		else if(strncmp(szValueName,"COMPILATION",11) == 0)
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
		else if(strncmp(szValueName,"ICMT",4) == 0)
		{
			strRet = theApp.m_Mp4.GetMetadata_Comment();
		}
		else if(strncmp(szValueName,"TOOL",4) == 0)
		{
			strRet = theApp.m_Mp4.GetMetadata_Tool();
		}
		break;
	case CShellExt::UNKNOWN:
	default:
		return FALSE;
	}

	*buf = (char *)(LPCSTR )strRet;

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

extern "C" BOOL __stdcall mp3infp_SetConf(char *tag,char *value)
{
	if(strcmp(tag,"wave_CodecFind") == 0)
	{
		int val = atoi(value);
		if((theApp.m_iWaveCodecFind <= 3) && (0 <= theApp.m_iWaveCodecFind))
		{
			theApp.m_iWaveCodecFind = val;
			return TRUE;
		}
	}
	else if(strcmp(tag,"avi_CodecFind") == 0)
	{
		int val = atoi(value);
		if((theApp.m_iAviCodecFind <= 3) && (0 <= theApp.m_iAviCodecFind))
		{
			theApp.m_iAviCodecFind = val;
			return TRUE;
		}
	}
	else if(strcmp(tag,"mp3_UseExtGenre") == 0)
	{
		int val = atoi(value);
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
	else if(strcmp(tag,"mp3_ID3v2Unicode") == 0)
	{
		int val = atoi(value);
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
	else if(strcmp(tag,"mp3_ID3v2Unsync") == 0)
	{
		int val = atoi(value);
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
	else if(strcmp(tag,"mp3_SaveID3v2Version") == 0)
	{
		if(strcmp(value,"2.2") == 0)
		{
			theApp.m_Id3tagv2.SetVer(0x0200);
		}
		else if(strcmp(value,"2.3") == 0)
		{
			theApp.m_Id3tagv2.SetVer(0x0300);
		}
		else if(strcmp(value,"2.4") == 0)
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

extern "C" DWORD __stdcall mp3infp_SetValue(const char *szValueName,const char *buf)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	switch(theApp.m_fileType){
	case CShellExt::MP3:
		if(strcmp(szValueName,"INAM_v1") == 0)
		{
			theApp.m_Id3tagv1.SetTitle(buf);
		}
		else if(strcmp(szValueName,"IART_v1") == 0)
		{
			theApp.m_Id3tagv1.SetArtist(buf);
		}
		else if(strcmp(szValueName,"IPRD_v1") == 0)
		{
			theApp.m_Id3tagv1.SetAlbum(buf);
		}
		else if(strcmp(szValueName,"ICMT_v1") == 0)
		{
			theApp.m_Id3tagv1.SetComment(buf);
		}
		else if(strcmp(szValueName,"ICRD_v1") == 0)
		{
			theApp.m_Id3tagv1.SetYear(buf);
		}
		else if(strcmp(szValueName,"IGNR_v1") == 0)
		{
			theApp.m_Id3tagv1.SetGenre(buf);
		}
		else if(strcmp(szValueName,"TRACK_v1") == 0)
		{
			theApp.m_Id3tagv1.SetTrackNo(buf);
		}
		else if(strcmp(szValueName,"INAM_v2") == 0)
		{
			theApp.m_Id3tagv2.SetTitle(buf);
		}
		else if(strcmp(szValueName,"IART_v2") == 0)
		{
			theApp.m_Id3tagv2.SetArtist(buf);
		}
		else if(strcmp(szValueName,"IPRD_v2") == 0)
		{
			theApp.m_Id3tagv2.SetAlbum(buf);
		}
		else if(strcmp(szValueName,"ICMT_v2") == 0)
		{
			theApp.m_Id3tagv2.SetComment(buf);
		}
		else if(strcmp(szValueName,"ICRD_v2") == 0)
		{
			theApp.m_Id3tagv2.SetYear(buf);
		}
		else if(strcmp(szValueName,"IGNR_v2") == 0)
		{
			theApp.m_Id3tagv2.SetGenre(buf);
		}
		else if(strcmp(szValueName,"ICOP_v2") == 0)
		{
			theApp.m_Id3tagv2.SetCopyright(buf);
		}
		else if(strcmp(szValueName,"ISFT_v2") == 0)
		{
			theApp.m_Id3tagv2.SetEncoder(buf);
		}
		else if(strcmp(szValueName,"COMP_v2") == 0)
		{
			theApp.m_Id3tagv2.SetComposer(buf);
		}
		else if(strcmp(szValueName,"OART_v2") == 0)
		{
			theApp.m_Id3tagv2.SetOrigArtist(buf);
		}
		else if(strcmp(szValueName,"URL_v2") == 0)
		{
			theApp.m_Id3tagv2.SetUrl(buf);
		}
		else if(strcmp(szValueName,"TRACK_v2") == 0)
		{
			theApp.m_Id3tagv2.SetTrackNo(buf);
		}
		else if(strcmp(szValueName,"ENC2_v2") == 0)
		{
			theApp.m_Id3tagv2.SetEncodest(buf);
		}
		else if(strcmp(szValueName,"INAM_rmp") == 0)
		{
			theApp.m_Rmp3.SetNAM(buf);
		}
		else if(strcmp(szValueName,"IART_rmp") == 0)
		{
			theApp.m_Rmp3.SetART(buf);
		}
		else if(strcmp(szValueName,"IPRD_rmp") == 0)
		{
			theApp.m_Rmp3.SetPRD(buf);
		}
		else if(strcmp(szValueName,"ICMT_rmp") == 0)
		{
			theApp.m_Rmp3.SetCMT(buf);
		}
		else if(strcmp(szValueName,"ICRD_rmp") == 0)
		{
			theApp.m_Rmp3.SetCRD(buf);
		}
		else if(strcmp(szValueName,"IGNR_rmp") == 0)
		{
			theApp.m_Rmp3.SetGNR(buf);
		}
		else if(strcmp(szValueName,"ICOP_rmp") == 0)
		{
			theApp.m_Rmp3.SetCOP(buf);
		}
		else if(strcmp(szValueName,"ISFT_rmp") == 0)
		{
			theApp.m_Rmp3.SetSFT(buf);
		}
		else if(strcmp(szValueName,"ISRC_rmp") == 0)
		{
			theApp.m_Rmp3.SetSRC(buf);
		}
		else if(strcmp(szValueName,"IENG_rmp") == 0)
		{
			theApp.m_Rmp3.SetENG(buf);
		}
		else if(strcmp(szValueName,"INAM_APE") == 0)
		{
			theApp.m_Ape.SetComment(CTag_Ape::APE_TAG_FIELD_TITLE,buf);
		}
		else if(strcmp(szValueName,"TRACK_APE") == 0)
		{
			theApp.m_Ape.SetComment(CTag_Ape::APE_TAG_FIELD_TRACK,buf);
		}
		else if(strcmp(szValueName,"IART_APE") == 0)
		{
			theApp.m_Ape.SetComment(CTag_Ape::APE_TAG_FIELD_ARTIST,buf);
		}
		else if(strcmp(szValueName,"IPRD_APE") == 0)
		{
			theApp.m_Ape.SetComment(CTag_Ape::APE_TAG_FIELD_ALBUM,buf);
		}
		else if(strcmp(szValueName,"ICMT_APE") == 0)
		{
			theApp.m_Ape.SetComment(CTag_Ape::APE_TAG_FIELD_COMMENT,buf);
		}
		else if(strcmp(szValueName,"ICRD_APE") == 0)
		{
			theApp.m_Ape.SetComment(CTag_Ape::APE_TAG_FIELD_YEAR,buf);
		}
		else if(strcmp(szValueName,"IGNR_APE") == 0)
		{
			theApp.m_Ape.SetComment(CTag_Ape::APE_TAG_FIELD_GENRE,buf);
		}
		break;
	case CShellExt::WAVE:
		if(strcmp(szValueName,"INAM") == 0)
		{
			theApp.m_RiffSIF.SetField('I','N','A','M',buf);
		}
		else if(strcmp(szValueName,"ISBJ") == 0)
		{
			theApp.m_RiffSIF.SetField('I','S','B','J',buf);
		}
		else if(strcmp(szValueName,"IART") == 0)
		{
			theApp.m_RiffSIF.SetField('I','A','R','T',buf);
		}
		else if(strcmp(szValueName,"IPRD") == 0)
		{
			theApp.m_RiffSIF.SetField('I','P','R','D',buf);
		}
		else if(strcmp(szValueName,"ICMT") == 0)
		{
			theApp.m_RiffSIF.SetField('I','C','M','T',buf);
		}
		else if(strcmp(szValueName,"ICRD") == 0)
		{
			theApp.m_RiffSIF.SetField('I','C','R','D',buf);
		}
		else if(strcmp(szValueName,"IGNR") == 0)
		{
			theApp.m_RiffSIF.SetField('I','G','N','R',buf);
		}
		else if(strcmp(szValueName,"ICOP") == 0)
		{
			theApp.m_RiffSIF.SetField('I','C','O','P',buf);
		}
		else if(strcmp(szValueName,"ISFT") == 0)
		{
			theApp.m_RiffSIF.SetField('I','S','F','T',buf);
		}
		else if(strcmp(szValueName,"ISRC") == 0)
		{
			theApp.m_RiffSIF.SetField('I','S','R','C',buf);
		}
		else if(strcmp(szValueName,"IENG") == 0)
		{
			theApp.m_RiffSIF.SetField('I','E','N','G',buf);
		}
		break;
	case CShellExt::AVI:
		if(strcmp(szValueName,"ISBJ") == 0)
		{
			theApp.m_OpenDML.SetField('I','S','B','J',buf);
		}
		else if(strcmp(szValueName,"INAM") == 0)
		{
			theApp.m_OpenDML.SetField('I','N','A','M',buf);
		}
		else if(strcmp(szValueName,"IART") == 0)
		{
			theApp.m_OpenDML.SetField('I','A','R','T',buf);
		}
		else if(strcmp(szValueName,"ICMT") == 0)
		{
			theApp.m_OpenDML.SetField('I','C','M','T',buf);
		}
		else if(strcmp(szValueName,"ICRD") == 0)
		{
			theApp.m_OpenDML.SetField('I','C','R','D',buf);
		}
		else if(strcmp(szValueName,"IGNR") == 0)
		{
			theApp.m_OpenDML.SetField('I','G','N','R',buf);
		}
		else if(strcmp(szValueName,"ICOP") == 0)
		{
			theApp.m_OpenDML.SetField('I','C','O','P',buf);
		}
		else if(strcmp(szValueName,"ISFT") == 0)
		{
			theApp.m_OpenDML.SetField('I','S','F','T',buf);
		}
		else if(strcmp(szValueName,"ISRC") == 0)
		{
			theApp.m_OpenDML.SetField('I','S','R','C',buf);
		}
		else if(strcmp(szValueName,"IENG") == 0)
		{
			theApp.m_OpenDML.SetField('I','E','N','G',buf);
		}
		break;
	case CShellExt::VQF:
		if(strcmp(szValueName,"INAM") == 0)
		{
			theApp.m_Vqf.SetField('N','A','M','E',(const unsigned char *)buf,strlen(buf));
		}
		else if(strcmp(szValueName,"IART") == 0)
		{
			theApp.m_Vqf.SetField('A','U','T','H',(const unsigned char *)buf,strlen(buf));
		}
		else if(strcmp(szValueName,"FILE") == 0)
		{
			theApp.m_Vqf.SetField('F','I','L','E',(const unsigned char *)buf,strlen(buf));
		}
		else if(strcmp(szValueName,"ICOP") == 0)
		{
			theApp.m_Vqf.SetField('(','c',')',' ',(const unsigned char *)buf,strlen(buf));
		}
		else if(strcmp(szValueName,"ICMT") == 0)
		{
			theApp.m_Vqf.SetField('C','O','M','T',(const unsigned char *)buf,strlen(buf));
		}
		break;
	case CShellExt::WMA:
		if(strcmp(szValueName,"INAM") == 0)
		{
			theApp.m_Wma.SetNAM(buf);
		}
		else if(strcmp(szValueName,"TRACK") == 0)
		{
			theApp.m_Wma.SetTRACK(buf);
		}
		else if(strcmp(szValueName,"IART") == 0)
		{
			theApp.m_Wma.SetART(buf);
		}
		else if(strcmp(szValueName,"IPRD") == 0)
		{
			theApp.m_Wma.SetPRD(buf);
		}
		else if(strcmp(szValueName,"ICMT") == 0)
		{
			theApp.m_Wma.SetCMT(buf);
		}
		else if(strcmp(szValueName,"ICRD") == 0)
		{
			theApp.m_Wma.SetCRD(buf);
		}
		else if(strcmp(szValueName,"IGNR") == 0)
		{
			theApp.m_Wma.SetGNR(buf);
		}
		else if(strcmp(szValueName,"ICOP") == 0)
		{
			theApp.m_Wma.SetCOPY(buf);
		}
		else if(strcmp(szValueName,"URL1") == 0)
		{
			theApp.m_Wma.SetUrl1(buf);
		}
		else if(strcmp(szValueName,"URL2") == 0)
		{
			theApp.m_Wma.SetUrl2(buf);
		}
		break;
	case CShellExt::OGG:
		if(strncmp(szValueName,"INAM",4) == 0)
		{
			theApp.m_Ogg.DelComment("TITLE",0);
			theApp.m_Ogg.AddComment("TITLE",buf);
		}
		else if(strncmp(szValueName,"TRACK",5) == 0)
		{
			theApp.m_Ogg.DelComment("TRACKNUMBER",0);
			theApp.m_Ogg.AddComment("TRACKNUMBER",buf);
		}
		else if(strncmp(szValueName,"IART",4) == 0)
		{
			theApp.m_Ogg.DelComment("ARTIST",0);
			theApp.m_Ogg.AddComment("ARTIST",buf);
		}
		else if(strncmp(szValueName,"IPRD",4) == 0)
		{
			theApp.m_Ogg.DelComment("ALBUM",0);
			theApp.m_Ogg.AddComment("ALBUM",buf);
		}
		else if(strncmp(szValueName,"ICMT",4) == 0)
		{
			theApp.m_Ogg.DelComment("COMMENT",0);
			theApp.m_Ogg.AddComment("COMMENT",buf);
		}
		else if(strncmp(szValueName,"ICRD",4) == 0)
		{
			theApp.m_Ogg.DelComment("DATE",0);
			theApp.m_Ogg.AddComment("DATE",buf);
		}
		else if(strncmp(szValueName,"IGNR",4) == 0)
		{
			theApp.m_Ogg.DelComment("GENRE",0);
			theApp.m_Ogg.AddComment("GENRE",buf);
		}
		break;
	case CShellExt::APE:
		if(strncmp(szValueName,"INAM",4) == 0)
		{
			theApp.m_Ape.SetComment(CTag_Ape::APE_TAG_FIELD_TITLE,buf);
		}
		else if(strncmp(szValueName,"TRACK",5) == 0)
		{
			theApp.m_Ape.SetComment(CTag_Ape::APE_TAG_FIELD_TRACK,buf);
		}
		else if(strncmp(szValueName,"IART",4) == 0)
		{
			theApp.m_Ape.SetComment(CTag_Ape::APE_TAG_FIELD_ARTIST,buf);
		}
		else if(strncmp(szValueName,"IPRD",4) == 0)
		{
			theApp.m_Ape.SetComment(CTag_Ape::APE_TAG_FIELD_ALBUM,buf);
		}
		else if(strncmp(szValueName,"ICMT",4) == 0)
		{
			theApp.m_Ape.SetComment(CTag_Ape::APE_TAG_FIELD_COMMENT,buf);
		}
		else if(strncmp(szValueName,"ICRD",4) == 0)
		{
			theApp.m_Ape.SetComment(CTag_Ape::APE_TAG_FIELD_YEAR,buf);
		}
		else if(strncmp(szValueName,"IGNR",4) == 0)
		{
			theApp.m_Ape.SetComment(CTag_Ape::APE_TAG_FIELD_GENRE,buf);
		}
		break;
	case CShellExt::MP4:
		if(strncmp(szValueName,"INAM",4) == 0)
		{
			theApp.m_Mp4.SetMetadata_Name(buf);
		}
		else if(strncmp(szValueName,"IART",4) == 0)
		{
			theApp.m_Mp4.SetMetadata_Artist(buf);
		}
		else if(strncmp(szValueName,"IPRD",4) == 0)
		{
			theApp.m_Mp4.SetMetadata_Album(buf);
		}
		else if(strncmp(szValueName,"IGRP",4) == 0)
		{
			theApp.m_Mp4.SetMetadata_Group(buf);
		}
		else if(strncmp(szValueName,"COMPOSER",8) == 0)
		{
			theApp.m_Mp4.SetMetadata_Composer(buf);
		}
		else if(strncmp(szValueName,"IGNR",4) == 0)
		{
			theApp.m_Mp4.SetMetadata_Genre(buf);
		}
		else if(strncmp(szValueName,"TRACK1",6) == 0)
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
		else if(strncmp(szValueName,"TRACK2",6) == 0)
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
		else if(strncmp(szValueName,"DISC1",5) == 0)
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
		else if(strncmp(szValueName,"DISC2",5) == 0)
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
		else if(strncmp(szValueName,"BPM",3) == 0)
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
		else if(strncmp(szValueName,"ICRD",4) == 0)
		{
			theApp.m_Mp4.SetMetadata_Year(buf);
		}
		else if(strncmp(szValueName,"COMPILATION",11) == 0)
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
		else if(strncmp(szValueName,"ICMT",4) == 0)
		{
			theApp.m_Mp4.SetMetadata_Comment(buf);
		}
		else if(strncmp(szValueName,"TOOL",4) == 0)
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

extern "C" DWORD __stdcall mp3infp_Save(const char *szFileName)
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

extern "C" DWORD __stdcall mp3infp_mp3_MakeId3v1(const char *szFileName)
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

extern "C" DWORD __stdcall mp3infp_mp3_DelId3v1(const char *szFileName)
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

extern "C" DWORD __stdcall mp3infp_mp3_MakeId3v2(const char *szFileName)
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

extern "C" DWORD __stdcall mp3infp_mp3_DelId3v2(const char *szFileName)
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

extern "C" DWORD __stdcall mp3infp_mp3_MakeRMP(const char *szFileName)
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

extern "C" DWORD __stdcall mp3infp_mp3_DelRMP(const char *szFileName)
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

extern "C" DWORD __stdcall mp3infp_mp3_MakeApeTag(const char *szFileName)
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

extern "C" DWORD __stdcall mp3infp_mp3_DelApeTag(const char *szFileName)
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

