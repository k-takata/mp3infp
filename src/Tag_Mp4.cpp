// Tag_Mp4.cpp: CTag_Mp4 クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Tag_Mp4.h"
#include "GlobalCommand.h"

#include "mp4v2/mp4v2.h"
#pragma comment (lib,"Ws2_32.lib")

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CTag_Mp4::CTag_Mp4()
{
	Release();
}

CTag_Mp4::~CTag_Mp4()
{
	Release();
}

void CTag_Mp4::Release()
{
	ClearMetadata();
	m_bEnable = FALSE;
}

void CTag_Mp4::ClearMetadata()
{
	m_strTrackInfo = _T("");
	m_strAudioInfo = _T("");
	m_strVideoInfo = _T("");

	m_strMetadata_Name = _T("");
	m_strMetadata_Artist = _T("");
	m_strMetadata_Album = _T("");
	m_strMetadata_Group = _T("");
	m_strMetadata_Composer = _T("");
	m_strMetadata_Genre = _T("");
	m_iMetadata_Track1 = -1;
	m_iMetadata_Track2 = -1;
	m_iMetadata_Disc1 = -1;
	m_iMetadata_Disc2 = -1;
	m_iMetadata_Tempo = -1;
	m_strMetadata_Year = _T("");
	m_iMetadata_Compilation = -1;
	m_strMetadata_Comment = _T("");
	m_strMetadata_Tool = _T("");
}

static CString _CnvMetadata(char *utf8val)
{
	return DataToCString(utf8val, -1, DTC_CODE_UTF8);
}

#if 0
static CString _Str2Utf8(LPCTSTR val)
{
	CString strRet;

	// MBCS -> UCS2
	int len = MultiByteToWideChar(CP_ACP,0,val,-1,NULL,0);
	if(!len)
	{
		return _T("");
	}
	WCHAR *wchar = (WCHAR *)malloc(len*sizeof(WCHAR));
	MultiByteToWideChar(CP_ACP,0,val,-1,wchar,len);

	// UCS2 -> UTF-8
	len = WideCharToMultiByte(CP_UTF8,0,wchar,-1,NULL,0,NULL,NULL);
	char *ch = (char *)malloc(len);
	WideCharToMultiByte(CP_UTF8,0,wchar,-1,ch,len,NULL,NULL);
	strRet = ch;
	
	free(wchar);
	free(ch);
	
	return strRet;
}
#endif

static void _StripAudioInfo(LPCTSTR info,CString &strAudio,CString &strVideo)
{
	CStringArray stra;
	CString tmp;
	int start = 0;
	int end = 0;
	while(end != -1)
	{
		tmp = info;
		end = tmp.Find(_T("\r\n"),start);
		if(end != -1)
		{
			tmp.Delete(end,tmp.GetLength()-end);
		}
		if(start)
		{
			tmp.Delete(0,start);
		}
		if(end != -1)
		{
			start = end + 2;
		}
		stra.Add(tmp);

		int idx = tmp.Find(_T("audio\t"));
		if(idx != -1)
		{
			tmp.Delete(0,idx+6);
			strAudio = tmp;
		}
		else
		{
			idx = tmp.Find(_T("video\t"));
			if(idx != -1)
			{
				tmp.Delete(0,idx+6);
				strVideo = tmp;
			}
		}
	}

}

DWORD CTag_Mp4::Load(LPCTSTR szFileName)
{
	DWORD dwWin32errorCode = ERROR_SUCCESS;
	Release();

	char *pFileName = TstrToDataAlloc(szFileName, -1, NULL, DTC_CODE_UTF8);
	if (pFileName == NULL) {
		return -1;
	}
	char *info = MP4FileInfo(pFileName);
	if(!info)
	{
		free(pFileName);
		Release();
		return -1;
	}
	m_strTrackInfo = info;
	free(info);
	m_strTrackInfo.Replace(_T("\n"),_T("\r\n"));

	// Audio/Video
	_StripAudioInfo(m_strTrackInfo,m_strAudioInfo,m_strVideoInfo);

	MP4FileHandle mp4file = MP4Read(pFileName);
	free(pFileName);
	if(mp4file == MP4_INVALID_FILE_HANDLE)
	{
		Release();
		return -1;
	}

	m_bEnable = TRUE;
	char *value;
	uint16_t numvalue, numvalue2;
	uint8_t int8value;
	
	if(MP4GetMetadataName(mp4file,&value) && (value != NULL))
	{
		m_strMetadata_Name = _CnvMetadata(value);
	}

	if(MP4GetMetadataArtist(mp4file,&value) && (value != NULL))
	{
		m_strMetadata_Artist = _CnvMetadata(value);
	}

	if(MP4GetMetadataAlbum(mp4file,&value) && (value != NULL))
	{
		m_strMetadata_Album = _CnvMetadata(value);
	}

	if(MP4GetMetadataGrouping(mp4file,&value) && (value != NULL))
	{
		m_strMetadata_Group = _CnvMetadata(value);
	}

	if(MP4GetMetadataWriter(mp4file,&value) && (value != NULL))
	{
		m_strMetadata_Composer = _CnvMetadata(value);
	}

	if(MP4GetMetadataGenre(mp4file,&value) && (value != NULL))
	{
		m_strMetadata_Genre = _CnvMetadata(value);
	}

	if(MP4GetMetadataTrack(mp4file,&numvalue,&numvalue2))
	{
		m_iMetadata_Track1 = numvalue;
		m_iMetadata_Track2 = numvalue2;
	}
	
	if(MP4GetMetadataDisk(mp4file,&numvalue,&numvalue2))
	{
		m_iMetadata_Disc1 = numvalue;
		m_iMetadata_Disc2 = numvalue2;
	}

	if(MP4GetMetadataTempo(mp4file,&numvalue))
	{
		m_iMetadata_Tempo = numvalue;
	}

	if(MP4GetMetadataYear(mp4file,&value) && (value != NULL))
	{
		m_strMetadata_Year = _CnvMetadata(value);
	}

	if(MP4GetMetadataCompilation(mp4file,&int8value))
	{
		m_iMetadata_Compilation = int8value;
	}

	if(MP4GetMetadataComment(mp4file,&value) && (value != NULL))
	{
		m_strMetadata_Comment = _CnvMetadata(value);
	}

	if(MP4GetMetadataTool(mp4file,&value) && (value != NULL))
	{
		m_strMetadata_Tool = _CnvMetadata(value);
	}

	MP4Close(mp4file);

	return dwWin32errorCode;
}

DWORD CTag_Mp4::Save(LPCTSTR szFileName)
{
	DWORD dwWin32errorCode = ERROR_SUCCESS;
	if(!m_bEnable)
	{
		return -1;
	}
	
	char *pFileName = TstrToDataAlloc(szFileName, -1, NULL, DTC_CODE_UTF8);
	if (pFileName == NULL) {
		return -1;
	}
	MP4FileHandle mp4file = MP4Modify(pFileName);
	free(pFileName);
	if(mp4file == MP4_INVALID_FILE_HANDLE)
	{
		return -1;
	}

	if(m_strMetadata_Name.GetLength())
	{
		char *buf = TstrToDataAlloc(m_strMetadata_Name, -1, NULL, DTC_CODE_UTF8);
		if (buf != NULL) {
			MP4SetMetadataName(mp4file,buf);
			free(buf);
		}
	}
	else
	{
		MP4DeleteMetadataName(mp4file);
	}

	if(m_strMetadata_Artist.GetLength())
	{
		char *buf = TstrToDataAlloc(m_strMetadata_Artist, -1, NULL, DTC_CODE_UTF8);
		if (buf != NULL) {
			MP4SetMetadataArtist(mp4file,buf);
			free(buf);
		}
	}
	else
	{
		MP4DeleteMetadataArtist(mp4file);
	}

	if(m_strMetadata_Album.GetLength())
	{
		char *buf = TstrToDataAlloc(m_strMetadata_Album, -1, NULL, DTC_CODE_UTF8);
		if (buf != NULL) {
			MP4SetMetadataAlbum(mp4file,buf);
			free(buf);
		}
	}
	else
	{
		MP4DeleteMetadataAlbum(mp4file);
	}

	if(m_strMetadata_Group.GetLength())
	{
		char *buf = TstrToDataAlloc(m_strMetadata_Group, -1, NULL, DTC_CODE_UTF8);
		if (buf != NULL) {
			MP4SetMetadataGrouping(mp4file,buf);
			free(buf);
		}
	}
	else
	{
		MP4DeleteMetadataGrouping(mp4file);
	}

	if(m_strMetadata_Composer.GetLength())
	{
		char *buf = TstrToDataAlloc(m_strMetadata_Composer, -1, NULL, DTC_CODE_UTF8);
		if (buf != NULL) {
			MP4SetMetadataWriter(mp4file,buf);
			free(buf);
		}
	}
	else
	{
		MP4DeleteMetadataWriter(mp4file);
	}

	if(m_strMetadata_Genre.GetLength())
	{
		char *buf = TstrToDataAlloc(m_strMetadata_Genre, -1, NULL, DTC_CODE_UTF8);
		if (buf != NULL) {
			MP4SetMetadataGenre(mp4file,buf);
			free(buf);
		}
	}
	else
	{
		MP4DeleteMetadataGenre(mp4file);
	}

	if((m_iMetadata_Track1 == -1) && (m_iMetadata_Track2 == -1))
	{
		MP4DeleteMetadataTrack(mp4file);
	}
	else if(m_iMetadata_Track1 == -1)
	{
		MP4SetMetadataTrack(mp4file,0,m_iMetadata_Track2);
	}
	else if(m_iMetadata_Track2 == -1)
	{
		MP4SetMetadataTrack(mp4file,m_iMetadata_Track1,0);
	}
	else
	{
		MP4SetMetadataTrack(mp4file,m_iMetadata_Track1,m_iMetadata_Track2);
	}

	if((m_iMetadata_Disc1 == -1) && (m_iMetadata_Disc2 == -1))
	{
		MP4DeleteMetadataDisk(mp4file);
	}
	else if(m_iMetadata_Disc1 == -1)
	{
		MP4SetMetadataDisk(mp4file,0,m_iMetadata_Disc2);
	}
	else if(m_iMetadata_Disc2 == -1)
	{
		MP4SetMetadataDisk(mp4file,m_iMetadata_Disc1,0);
	}
	else
	{
		MP4SetMetadataDisk(mp4file,m_iMetadata_Disc1,m_iMetadata_Disc2);
	}

	if(m_iMetadata_Tempo == -1)
	{
		MP4DeleteMetadataTempo(mp4file);
	}
	else
	{
		MP4SetMetadataTempo(mp4file,m_iMetadata_Tempo);
	}

	if(m_strMetadata_Year.GetLength())
	{
		char *buf = TstrToDataAlloc(m_strMetadata_Year, -1, NULL, DTC_CODE_UTF8);
		if (buf != NULL) {
			MP4SetMetadataYear(mp4file,buf);
			free(buf);
		}
	}
	else
	{
		MP4DeleteMetadataYear(mp4file);
	}

	if(m_iMetadata_Compilation != -1)
	{
		MP4SetMetadataCompilation(mp4file,1);
	}
	else
	{
		MP4DeleteMetadataCompilation(mp4file);
	}

	if(m_strMetadata_Comment.GetLength())
	{
		char *buf = TstrToDataAlloc(m_strMetadata_Comment, -1, NULL, DTC_CODE_UTF8);
		if (buf != NULL) {
			MP4SetMetadataComment(mp4file,buf);
			free(buf);
		}
	}
	else
	{
		MP4DeleteMetadataComment(mp4file);
	}

	if(m_strMetadata_Tool.GetLength())
	{
		char *buf = TstrToDataAlloc(m_strMetadata_Tool, -1, NULL, DTC_CODE_UTF8);
		if (buf != NULL) {
			MP4SetMetadataTool(mp4file,buf);
			free(buf);
		}
	}
	else
	{
		MP4DeleteMetadataTool(mp4file);
	}
	
	MP4Close(mp4file);

	return dwWin32errorCode;
}

