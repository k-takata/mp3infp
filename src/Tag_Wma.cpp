// Tag_Wma.cpp: CTag_Wma クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"		// メイン シンボル
#include "GlobalCommand.h"
#include "MMCommand.h"
#include "Tag_Wma.h"

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

DEFINE_GUID(ASF_Header_Object						,0x75B22630,0x668E,0x11CF,0xA6,0xD9,0x00,0xAA,0x00,0x62,0xCE,0x6C);
DEFINE_GUID(ASF_Content_Description_Object			,0x75B22633,0x668E,0x11CF,0xA6,0xD9,0x00,0xAA,0x00,0x62,0xCE,0x6C);
DEFINE_GUID(ASF_Stream_Properties_Object			,0xB7DC0791,0xA9B7,0x11CF,0x8E,0xE6,0x00,0xC0,0x0C,0x20,0x53,0x65);
DEFINE_GUID(ASF_Video_Media							,0xBC19EFC0,0x5B4D,0x11CF,0xA8,0xFD,0x00,0x80,0x5F,0x5C,0x44,0x2B);
DEFINE_GUID(ASF_Codec_List_Object					,0x86D15240,0x311D,0x11D0,0xA3,0xA4,0x00,0xA0,0xC9,0x03,0x48,0xF6);
DEFINE_GUID(ASF_Extended_Content_Description_Object	,0xD2D0A440,0xE307,0x11D2,0x97,0xF0,0x00,0xA0,0xC9,0x5E,0xA8,0x50);
DEFINE_GUID(ASF_File_Properties_Object				,0x8CABDCA1,0xA947,0x11CF,0x8E,0xE4,0x00,0xC0,0x0C,0x20,0x53,0x65);

DEFINE_GUID(ASF_Header_Extension_Object				,0x5FBF03B5,0xA92E,0x11CF,0x8E,0xE3,0x00,0xC0,0x0C,0x20,0x53,0x65);
DEFINE_GUID(ASF_Extended_Stream_Properties_Object	,0x14E6A5CB,0xC672,0x4332,0x83,0x99,0xA9,0x69,0x52,0x06,0x5B,0x5A);
DEFINE_GUID(ASF_Content_Encryption_Object			,0x2211B3FB,0xBD23,0x11D2,0xB4,0xB7,0x00,0xA0,0xC9,0x55,0xFC,0x6E);
DEFINE_GUID(ASF_Extended_Content_Encryption_Object	,0x298AE614,0x2622,0x4C17,0xB9,0x35,0xDA,0xE0,0x7E,0xE9,0x28,0x9C);
DEFINE_GUID(ASF_Stream_Bitrate_Properties_Object	,0x7BF875CE,0x468D,0x11D1,0x8D,0x82,0x00,0x60,0x97,0xC9,0xA2,0xB2);

DEFINE_GUID(ASF_Data_Object							,0x75B22636,0x668E,0x11CF,0xA6,0xD9,0x00,0xAA,0x00,0x62,0xCE,0x6C);

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CTag_Wma::CTag_Wma()
{
	Release();
}

CTag_Wma::~CTag_Wma()
{

}

void CTag_Wma::Release()
{
	m_bEnable = FALSE;

	m_bIsSeekable = FALSE;
	m_bIsBroadcast = FALSE;
	m_bUseDRMv1 = FALSE;
	m_bUseDRMv7 = FALSE;
	m_llPlayTime = 0;
	m_strAudioCodec = _T("");
	m_strVideoCodec = _T("");
	m_dwVideoWidth = 0;
	m_dwVideoHeight = 0;
	m_dwBitCount = 0;
	
	m_strTitle = _T("");
	m_strAuthor = _T("");
	m_strCopyright = _T("");
	m_strDescription = _T("");
	m_strRating = _T("");
	m_comments.clear();
}

CString CTag_Wma::GetNAM()
{
	return m_strTitle;
}

void CTag_Wma::SetNAM(LPCTSTR str)
{
	m_strTitle = str;
	m_bEnable = TRUE;
}

CString CTag_Wma::GetTRACK()
{
	DWORD dwRet;
	CString ret;
	// "WM/TrackNumber"を優先的に読み込む
	if(GetCommentString(_T("WM/TrackNumber"),ret))
	{
		return ret;
	}
	// 互換性のためにSTRING or DWORDのWM/Trackも認識する
	else if(GetCommentDword(_T("WM/Track"),dwRet))
	{
		ret.Format(_T("%d"),dwRet+1);
	}
	else if(GetCommentString(_T("WM/Track"),ret))
	{
		ret.Format(_T("%d"),_ttoi(ret)+1);
	}
	return ret;
}

void CTag_Wma::SetTRACK(LPCTSTR str)
{
	if(lstrlen(str) == 0)
	{
		DellComment(_T("WM/Track"));
		DellComment(_T("WM/TrackNumber"));
	}
	else
	{
		// 0base
		AddCommentDword(_T("WM/Track"),_ttoi(str)-1);
		// 1base
		AddCommentString(_T("WM/TrackNumber"),str);
	}
	m_bEnable = TRUE;
}

CString CTag_Wma::GetART()
{
	return m_strAuthor;
}

void CTag_Wma::SetART(LPCTSTR str)
{
	m_strAuthor = str;
	m_bEnable = TRUE;
}

CString CTag_Wma::GetPRD()
{
	CString ret;
	if(GetCommentString(_T("WM/AlbumTitle"),ret))
	{
		return ret;
	}
	return _T("");
}

void CTag_Wma::SetPRD(LPCTSTR str)
{
	if(lstrlen(str) == 0)
	{
		DellComment(_T("WM/AlbumTitle"));
	}
	else
	{
		AddCommentString(_T("WM/AlbumTitle"),str);
	}
	m_bEnable = TRUE;
}

CString CTag_Wma::GetCMT()
{
	return m_strDescription;
}

void CTag_Wma::SetCMT(LPCTSTR str)
{
	m_strDescription = str;
	m_bEnable = TRUE;
}

CString CTag_Wma::GetCRD()
{
	CString ret;
	if(GetCommentString(_T("WM/Year"),ret))
	{
		return ret;
	}
	return _T("");
}

void CTag_Wma::SetCRD(LPCTSTR str)
{
	if(lstrlen(str) == 0)
	{
		DellComment(_T("WM/Year"));
	}
	else
	{
		AddCommentString(_T("WM/Year"),str);
	}
	m_bEnable = TRUE;
}

CString CTag_Wma::GetGNR()
{
	CString ret;
	if(GetCommentString(_T("WM/Genre"),ret))
	{
		return ret;
	}
	return _T("");
}

void CTag_Wma::SetGNR(LPCTSTR str)
{
	if(lstrlen(str) == 0)
	{
		DellComment(_T("WM/Genre"));
	}
	else
	{
		AddCommentString(_T("WM/Genre"),str);
	}
	m_bEnable = TRUE;
}

CString CTag_Wma::GetCOPY()
{
	return m_strCopyright;
}

void CTag_Wma::SetCOPY(LPCTSTR str)
{
	m_strCopyright = str;
	m_bEnable = TRUE;
}

CString CTag_Wma::GetUrl1()
{
	CString ret;
	if(GetCommentString(_T("WM/AlbumCoverURL"),ret))
	{
		return ret;
	}
	return _T("");
}

void CTag_Wma::SetUrl1(LPCTSTR str)
{
	if(lstrlen(str) == 0)
	{
		DellComment(_T("WM/AlbumCoverURL"));
	}
	else
	{
		AddCommentString(_T("WM/AlbumCoverURL"),str);
	}
	m_bEnable = TRUE;
}

CString CTag_Wma::GetUrl2()
{
	CString ret;
	if(GetCommentString(_T("WM/PromotionURL"),ret))
	{
		return ret;
	}
	return _T("");
}

void CTag_Wma::SetUrl2(LPCTSTR str)
{
	if(lstrlen(str) == 0)
	{
		DellComment(_T("WM/PromotionURL"));
	}
	else
	{
		AddCommentString(_T("WM/PromotionURL"),str);
	}
	m_bEnable = TRUE;
}

CString CTag_Wma::GetRating()
{
	return m_strRating;
}

void CTag_Wma::SetRating(LPCTSTR str)
{
	m_strRating = str;
	m_bEnable = TRUE;
}

//////////////////////////////////////////////////////////////////////////

DWORD CTag_Wma::Load(LPCTSTR szFileName)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;
	
//	DWORD dwRet;
	__int64 llFileSize;

	Release();
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//ファイルを開く
	HANDLE hFile = CreateFile(szFileName,
					GENERIC_READ,
					FILE_SHARE_READ,NULL,
					OPEN_EXISTING,	//ファイルを開きます。指定したファイルが存在していない場合、この関数は失敗します。 
					FILE_ATTRIBUTE_NORMAL,
					NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		dwWin32errorCode = GetLastError();
		goto exit;
	}
	llFileSize = GetFileSize64(hFile);

	while(1)
	{
		dwWin32errorCode = Walk(hFile,llFileSize);
		if(dwWin32errorCode != ERROR_SUCCESS)
		{
			break;
		}
	}
	// エラーは無視
	dwWin32errorCode = ERROR_SUCCESS;

exit:
	if(hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
	}

	return dwWin32errorCode;
}


DWORD CTag_Wma::ReadGUID_Size(HANDLE hFile,GUID_SIZE *guidSize)
{
	DWORD	dwRet;

	__int64 lldwHeaderPtr = SetFilePointer64(hFile,0,FILE_CURRENT);
	TRACE(_T("Ptr=0x%I64x\n"),lldwHeaderPtr);
	if(!ReadFile(hFile,guidSize,sizeof(GUID_SIZE),&dwRet,NULL))
	{
		return GetLastError();
	}
	if(dwRet < sizeof(GUID_SIZE))
	{
		return -1;
	}
	if(guidSize->size < sizeof(GUID_SIZE))
	{
		TRACE(_T("guidSize->size < sizeof(GUID_SIZE)\n"));
		return -1;
	}

	return ERROR_SUCCESS;
}

DWORD CTag_Wma::Walk(HANDLE hFile,__int64 llSize)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;
	GUID_SIZE guidSize;
	__int64 llTop = SetFilePointer64(hFile,0,FILE_CURRENT);
	__int64 llFileSize = GetFileSize64(hFile);
	
	dwWin32errorCode = ReadGUID_Size(hFile,&guidSize);
	if(dwWin32errorCode != ERROR_SUCCESS)
	{
		return dwWin32errorCode;
	}
	if(guidSize.size < sizeof(GUID_SIZE))
	{
		return -1;
	}
	// 2006-12-04
	if((llTop + guidSize.size) > llFileSize)
	{
		return -1;
	}
	TRACE(_T("GUID = {%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X} (%I64dbytes)\n"),
		guidSize.guid.Data1,guidSize.guid.Data2,guidSize.guid.Data3,
		guidSize.guid.Data4[0],guidSize.guid.Data4[1],guidSize.guid.Data4[2],guidSize.guid.Data4[3],guidSize.guid.Data4[4],guidSize.guid.Data4[5],guidSize.guid.Data4[6],guidSize.guid.Data4[7],
		guidSize.size
		);

	if(guidSize.guid == ASF_Header_Object)
	{
		TRACE(_T("(ASF_Header_Object)\n"));
		m_bEnable = TRUE;
		dwWin32errorCode = HeaderWalk(hFile,guidSize.size-sizeof(guidSize));
		if(dwWin32errorCode != ERROR_SUCCESS)
		{
			return dwWin32errorCode;
		}
	}
		
	SetFilePointer64(hFile,llTop+guidSize.size,FILE_BEGIN);
	// 2006-12-04
	DWORD dwError = GetLastError();
	if(dwError != NO_ERROR)
	{
		return -1;
	}
	return dwWin32errorCode;
}

DWORD CTag_Wma::HeaderWalk(HANDLE hFile,__int64 llSize)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;
	DWORD	dwRet;
	DWORD dwNumberOfHeaderObjects;
	BYTE reserved[2];
	if(!ReadFile(hFile,&dwNumberOfHeaderObjects,sizeof(dwNumberOfHeaderObjects),&dwRet,NULL))
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}
	TRACE(_T("dwNumberOfHeaderObjects = %d\n"),dwNumberOfHeaderObjects);
	if(!ReadFile(hFile,reserved,sizeof(reserved),&dwRet,NULL))
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}
	for(DWORD i=0; i<dwNumberOfHeaderObjects; i++)
	{
		GUID_SIZE guidSize;
		__int64 llTop = SetFilePointer64(hFile,0,FILE_CURRENT);
		
		if(ReadGUID_Size(hFile,&guidSize) != ERROR_SUCCESS)
		{
			dwWin32errorCode = GetLastError();
			return dwWin32errorCode;
		}
		TRACE(_T(">>GUID = {%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X} (%I64dbytes)\n"),
			guidSize.guid.Data1,guidSize.guid.Data2,guidSize.guid.Data3,
			guidSize.guid.Data4[0],guidSize.guid.Data4[1],guidSize.guid.Data4[2],guidSize.guid.Data4[3],guidSize.guid.Data4[4],guidSize.guid.Data4[5],guidSize.guid.Data4[6],guidSize.guid.Data4[7],
			guidSize.size
			);

		if(guidSize.guid == ASF_Extended_Stream_Properties_Object)
		{
			TRACE(_T(">>(ASF_Extended_Stream_Properties_Object)\n"));
		}
		else if(guidSize.guid == ASF_Stream_Properties_Object)
		{
			TRACE(_T(">>(ASF_Stream_Properties_Object)\n"));
			GUID streamType;
			GUID errorCorrectionType;
			ULONGLONG timeOffset;
			DWORD dwTypeSpecificDataLength;
			DWORD dwErroCorrectionDataLength;
			WORD wFlags;
			DWORD dwReserved;
			if(!ReadFile(hFile,&streamType,sizeof(streamType),&dwRet,NULL))
			{
				dwWin32errorCode = GetLastError();
				return dwWin32errorCode;
			}
			if(!ReadFile(hFile,&errorCorrectionType,sizeof(errorCorrectionType),&dwRet,NULL))
			{
				dwWin32errorCode = GetLastError();
				return dwWin32errorCode;
			}
			if(!ReadFile(hFile,&timeOffset,sizeof(timeOffset),&dwRet,NULL))
			{
				dwWin32errorCode = GetLastError();
				return dwWin32errorCode;
			}
			if(!ReadFile(hFile,&dwTypeSpecificDataLength,sizeof(dwTypeSpecificDataLength),&dwRet,NULL))
			{
				dwWin32errorCode = GetLastError();
				return dwWin32errorCode;
			}
			if(!ReadFile(hFile,&dwErroCorrectionDataLength,sizeof(dwErroCorrectionDataLength),&dwRet,NULL))
			{
				dwWin32errorCode = GetLastError();
				return dwWin32errorCode;
			}
			if(!ReadFile(hFile,&wFlags,sizeof(wFlags),&dwRet,NULL))
			{
				dwWin32errorCode = GetLastError();
				return dwWin32errorCode;
			}
			if(!ReadFile(hFile,&dwReserved,sizeof(dwReserved),&dwRet,NULL))
			{
				dwWin32errorCode = GetLastError();
				return dwWin32errorCode;
			}
			if(streamType == ASF_Video_Media)
			{
				DWORD dwEncodedImageWidth;
				DWORD dwEncodedImageHeight;
				BYTE reservedFlags;
				WORD wFormatDataSize;
				if(!ReadFile(hFile,&dwEncodedImageWidth,sizeof(dwEncodedImageWidth),&dwRet,NULL))
				{
					dwWin32errorCode = GetLastError();
					return dwWin32errorCode;
				}
				if(!ReadFile(hFile,&dwEncodedImageHeight,sizeof(dwEncodedImageHeight),&dwRet,NULL))
				{
					dwWin32errorCode = GetLastError();
					return dwWin32errorCode;
				}
				if(!ReadFile(hFile,&reservedFlags,sizeof(reservedFlags),&dwRet,NULL))
				{
					dwWin32errorCode = GetLastError();
					return dwWin32errorCode;
				}
				if(!ReadFile(hFile,&wFormatDataSize,sizeof(wFormatDataSize),&dwRet,NULL))
				{
					dwWin32errorCode = GetLastError();
					return dwWin32errorCode;
				}
				m_dwVideoWidth = dwEncodedImageWidth;
				m_dwVideoHeight = dwEncodedImageHeight;
				if(wFormatDataSize)
				{
					BITMAPINFOHEADER *formatData = (BITMAPINFOHEADER *)malloc(wFormatDataSize);
					if(!ReadFile(hFile,formatData,wFormatDataSize,&dwRet,NULL))
					{
						dwWin32errorCode = GetLastError();
						free(formatData);
						return dwWin32errorCode;
					}
					m_dwVideoWidth = formatData->biWidth;
					m_dwVideoHeight = formatData->biHeight;
					m_dwBitCount = formatData->biBitCount;
					CString strCodecName = _T("Unknown");
					findVideoCodecName1(strCodecName,0,formatData->biCompression);
					m_strVideoCodec.Format(_T("%s, %dx%d, %dBitColor"),strCodecName,m_dwVideoWidth,m_dwVideoHeight,m_dwBitCount);
					free(formatData);
				}
			}
		}
		else if(guidSize.guid == ASF_Codec_List_Object)
		{
			GUID reserved;
			DWORD codecEntriesCount;
			TRACE(_T(">>(ASF_Codec_List_Object)\n"));
			if(!ReadFile(hFile,&reserved,sizeof(reserved),&dwRet,NULL))
			{
				dwWin32errorCode = GetLastError();
				return dwWin32errorCode;
			}
			if(!ReadFile(hFile,&codecEntriesCount,sizeof(codecEntriesCount),&dwRet,NULL))
			{
				dwWin32errorCode = GetLastError();
				return dwWin32errorCode;
			}
			TRACE(_T("codecEntriesCount=%d\n"),codecEntriesCount);
			WORD wType;
			WORD wCodecNameLength;
			WORD wDescriptionLength;
			WORD wCodecInfomationLength;
			for(DWORD codecCount=0; codecCount<codecEntriesCount; codecCount++)
			{
				if(!ReadFile(hFile,&wType,sizeof(wType),&dwRet,NULL))
				{
					dwWin32errorCode = GetLastError();
					return dwWin32errorCode;
				}
				TRACE(_T("wType=%d\n"),wType);
				if(wType == 0x0001)	// Video codec
				{
					if(!ReadFile(hFile,&wCodecNameLength,sizeof(wCodecNameLength),&dwRet,NULL))
					{
						dwWin32errorCode = GetLastError();
						return dwWin32errorCode;
					}
					m_strVideoCodec = ReadWchar(hFile,wCodecNameLength*sizeof(WCHAR));
					if(!ReadFile(hFile,&wDescriptionLength,sizeof(wDescriptionLength),&dwRet,NULL))
					{
						dwWin32errorCode = GetLastError();
						return dwWin32errorCode;
					}
					CString strTmp = ReadWchar(hFile,wDescriptionLength*sizeof(WCHAR));
					if(strTmp.GetLength() > 0)
					{
						m_strVideoCodec += _T(" (") + strTmp + _T(")");
					}
					TRACE(_T("m_strVideoCodec=%s\n"),m_strVideoCodec);
					if(!ReadFile(hFile,&wCodecInfomationLength,sizeof(wCodecInfomationLength),&dwRet,NULL))
					{
						dwWin32errorCode = GetLastError();
						return dwWin32errorCode;
					}
					if(SetFilePointer(hFile,wCodecInfomationLength,NULL,FILE_CURRENT) == INVALID_SET_FILE_POINTER)
					{
						dwWin32errorCode = GetLastError();
						return dwWin32errorCode;
					}
				}
				else if(wType == 0x0002)	// Audio codec
				{
					if(!ReadFile(hFile,&wCodecNameLength,sizeof(wCodecNameLength),&dwRet,NULL))
					{
						dwWin32errorCode = GetLastError();
						return dwWin32errorCode;
					}
					m_strAudioCodec = ReadWchar(hFile,wCodecNameLength*sizeof(WCHAR));
					if(!ReadFile(hFile,&wDescriptionLength,sizeof(wDescriptionLength),&dwRet,NULL))
					{
						dwWin32errorCode = GetLastError();
						return dwWin32errorCode;
					}
					CString strTmp = ReadWchar(hFile,wDescriptionLength*sizeof(WCHAR));
					if(strTmp.GetLength() > 0)
					{
						m_strAudioCodec += _T(" (") + strTmp + _T(")");
					}
					TRACE(_T("m_strAudioCodec=%s\n"),m_strAudioCodec);
					if(!ReadFile(hFile,&wCodecInfomationLength,sizeof(wCodecInfomationLength),&dwRet,NULL))
					{
						dwWin32errorCode = GetLastError();
						return dwWin32errorCode;
					}
					if(SetFilePointer(hFile,wCodecInfomationLength,NULL,FILE_CURRENT) == INVALID_SET_FILE_POINTER)
					{
						dwWin32errorCode = GetLastError();
						return dwWin32errorCode;
					}
				}
			}
		}
		else if(guidSize.guid == ASF_File_Properties_Object)
		{
			GUID fileID;
			ULONGLONG fileSize;
			ULONGLONG creationDate;
			ULONGLONG dataPacketsCount;
			ULONGLONG playDuration;
			ULONGLONG sendDuration;
			ULONGLONG preroll;
			DWORD flags;
			DWORD minimumDataPacketSize;
			DWORD maximumDataPacketSize;
			DWORD maximumBitrate;

			TRACE(_T(">>(ASF_File_Properties_Object)\n"));
			if(!ReadFile(hFile,&fileID,sizeof(fileID),&dwRet,NULL))
			{
				dwWin32errorCode = GetLastError();
				return dwWin32errorCode;
			}
			if(!ReadFile(hFile,&fileSize,sizeof(fileSize),&dwRet,NULL))
			{
				dwWin32errorCode = GetLastError();
				return dwWin32errorCode;
			}
			TRACE(_T("   filesize=%I64d\n"),fileSize);
			if(!ReadFile(hFile,&creationDate,sizeof(creationDate),&dwRet,NULL))
			{
				dwWin32errorCode = GetLastError();
				return dwWin32errorCode;
			}
			if(!ReadFile(hFile,&dataPacketsCount,sizeof(dataPacketsCount),&dwRet,NULL))
			{
				dwWin32errorCode = GetLastError();
				return dwWin32errorCode;
			}
			if(!ReadFile(hFile,&playDuration,sizeof(playDuration),&dwRet,NULL))
			{
				dwWin32errorCode = GetLastError();
				return dwWin32errorCode;
			}
			m_llPlayTime = playDuration;
			if(!ReadFile(hFile,&sendDuration,sizeof(sendDuration),&dwRet,NULL))
			{
				dwWin32errorCode = GetLastError();
				return dwWin32errorCode;
			}
			if(!ReadFile(hFile,&preroll,sizeof(preroll),&dwRet,NULL))
			{
				dwWin32errorCode = GetLastError();
				return dwWin32errorCode;
			}
			m_llPlayTime -= preroll * 10000;	// プリロール時間(ミリ秒単位)を差し引く
			if(!ReadFile(hFile,&flags,sizeof(flags),&dwRet,NULL))
			{
				dwWin32errorCode = GetLastError();
				return dwWin32errorCode;
			}
			if(!ReadFile(hFile,&minimumDataPacketSize,sizeof(minimumDataPacketSize),&dwRet,NULL))
			{
				dwWin32errorCode = GetLastError();
				return dwWin32errorCode;
			}
			if(!ReadFile(hFile,&maximumDataPacketSize,sizeof(maximumDataPacketSize),&dwRet,NULL))
			{
				dwWin32errorCode = GetLastError();
				return dwWin32errorCode;
			}
			if(!ReadFile(hFile,&maximumBitrate,sizeof(maximumBitrate),&dwRet,NULL))
			{
				dwWin32errorCode = GetLastError();
				return dwWin32errorCode;
			}
			m_bIsSeekable = (flags&0x00000002)?TRUE:FALSE;
			m_bIsBroadcast = (flags&0x00000001)?TRUE:FALSE;
		}
		else if(guidSize.guid == ASF_Extended_Content_Description_Object)
		{
			////////////////////////////////////////////////////////
			TRACE(_T(">>(ASF_Extended_Content_Description_Object)\n"));
			dwWin32errorCode = ReadDescString(hFile);
			if(dwWin32errorCode != ERROR_SUCCESS)
			{
				return dwWin32errorCode;
			}
		}
		else if(guidSize.guid == ASF_Header_Extension_Object)
		{
			GUID reserved1;
			WORD reserved2;
			TRACE(_T(">>(ASF_Header_Extension_Object)\n"));
			if(!ReadFile(hFile,&reserved1,sizeof(reserved1),&dwRet,NULL))
			{
				dwWin32errorCode = GetLastError();
				return dwWin32errorCode;
			}
			if(!ReadFile(hFile,&reserved2,sizeof(reserved2),&dwRet,NULL))
			{
				dwWin32errorCode = GetLastError();
				return dwWin32errorCode;
			}
			DWORD dataSize;
			if(!ReadFile(hFile,&dataSize,sizeof(dataSize),&dwRet,NULL))
			{
				dwWin32errorCode = GetLastError();
				return dwWin32errorCode;
			}
			__int64 offset = 0;
			BYTE *data = (BYTE *)malloc(dataSize);
			if(!ReadFile(hFile,data,dataSize,&dwRet,NULL))
			{
				dwWin32errorCode = GetLastError();
				free(data);
				return dwWin32errorCode;
			}
			while(offset < dataSize)
			{
				if(offset+sizeof(GUID_SIZE) >= dataSize)
				{
					break;
				}
				GUID_SIZE *guidSize2 = (GUID_SIZE *)&data[offset];
				TRACE(_T(">> >>GUID = {%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X} (%I64dbytes)\n"),
					guidSize2->guid.Data1,guidSize2->guid.Data2,guidSize2->guid.Data3,
					guidSize2->guid.Data4[0],guidSize2->guid.Data4[1],guidSize2->guid.Data4[2],guidSize2->guid.Data4[3],guidSize2->guid.Data4[4],guidSize2->guid.Data4[5],guidSize2->guid.Data4[6],guidSize2->guid.Data4[7],
					guidSize2->size
					);
				if(guidSize2->size == 0)
				{
					break;
				}
				offset += guidSize2->size;
			}
			free(data);
		}
		else if(guidSize.guid == ASF_Content_Description_Object)
		{
			////////////////////////////////////////////////////////
			TRACE(_T(">>(ASF_Content_Description_Object)\n"));
			WORD wTitleLength;
			WORD wAuthorLength;
			WORD wCopyrightLength;
			WORD wDescriptionLength;
			WORD wRatingLength;
			if(!ReadFile(hFile,&wTitleLength,sizeof(wTitleLength),&dwRet,NULL))
			{
				dwWin32errorCode = GetLastError();
				return dwWin32errorCode;
			}
			if(!ReadFile(hFile,&wAuthorLength,sizeof(wAuthorLength),&dwRet,NULL))
			{
				dwWin32errorCode = GetLastError();
				return dwWin32errorCode;
			}
			if(!ReadFile(hFile,&wCopyrightLength,sizeof(wCopyrightLength),&dwRet,NULL))
			{
				dwWin32errorCode = GetLastError();
				return dwWin32errorCode;
			}
			if(!ReadFile(hFile,&wDescriptionLength,sizeof(wDescriptionLength),&dwRet,NULL))
			{
				dwWin32errorCode = GetLastError();
				return dwWin32errorCode;
			}
			if(!ReadFile(hFile,&wRatingLength,sizeof(wRatingLength),&dwRet,NULL))
			{
				dwWin32errorCode = GetLastError();
				return dwWin32errorCode;
			}
			m_strTitle = ReadWchar(hFile,wTitleLength);
			m_strAuthor = ReadWchar(hFile,wAuthorLength);
			m_strCopyright = ReadWchar(hFile,wCopyrightLength);
			m_strDescription = ReadWchar(hFile,wDescriptionLength);
			m_strRating = ReadWchar(hFile,wRatingLength);
		}
		else if(guidSize.guid == ASF_Content_Encryption_Object)
		{
			TRACE(_T(">>(ASF_Content_Encryption_Object) m_bUseDRMv1\n"));
			m_bUseDRMv1 = TRUE;
		}
		else if(guidSize.guid == ASF_Extended_Content_Encryption_Object)
		{
			TRACE(_T(">>(ASF_Extended_Content_Encryption_Object) m_bUseDRMv7\n"));
			m_bUseDRMv7 = TRUE;
		}
		else if(guidSize.guid == ASF_Stream_Bitrate_Properties_Object)
		{
			TRACE(_T(">>(ASF_Stream_Bitrate_Properties_Object)\n"));
		}
		else
		{
			TRACE(_T(">>(unknown)\n"));
		}

		SetFilePointer64(hFile,llTop+guidSize.size,FILE_BEGIN);
	}

	return dwWin32errorCode;
}

CString CTag_Wma::ReadWchar(HANDLE hFile,WORD len)
{
	if(len == 0)
	{
		return _T("");
	}
	CString ret;
	DWORD dwRet;
	WCHAR *tmp;
	tmp = (WCHAR *)malloc(len+sizeof(WCHAR));	// +sizeof(WCHAR)はNUL文字(入力にNULが無かった場合に備えて)
	if (tmp == NULL) {
		return _T("");
	}
	if(!ReadFile(hFile,tmp,len,&dwRet,NULL))
	{
		free(tmp);
		return _T("");
	}
	tmp[len/sizeof(WCHAR)] = L'\0';	// 入力にNULが無かった場合に備えて
	ret = tmp;
	free(tmp);
	
	return ret;
}

DWORD CTag_Wma::WriteWchar(HANDLE hFile,CString str)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;
	
	int size;
	char *ctmp = TstrToDataAlloc(str, -1, &size, DTC_CODE_UTF16LE);
	if (ctmp == NULL) {
		return ERROR_INSUFFICIENT_BUFFER;
	}
	
	DWORD dwRet;
	if(!WriteFile(hFile,ctmp,size,&dwRet,NULL))
	{
		dwWin32errorCode = GetLastError();
		free(ctmp);
		return dwWin32errorCode;
	}
	
	free(ctmp);
	return dwWin32errorCode;
}

DWORD CTag_Wma::ReadDescString(HANDLE hFile)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;
	WORD wCount;
	DWORD dwRet;
	if(!ReadFile(hFile,&wCount,sizeof(wCount),&dwRet,NULL))
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}
	CString ret;
	WORD wNameLength;
	WCHAR *tmp;
	CString strName;
	CString strVal;
	int clength;
	for(WORD i=0; i<wCount; i++)
	{
		// Descriptor Name
		if(!ReadFile(hFile,&wNameLength,sizeof(wNameLength),&dwRet,NULL))
		{
			dwWin32errorCode = GetLastError();
			return dwWin32errorCode;
		}
		tmp = (WCHAR *)malloc(wNameLength);
		if(!ReadFile(hFile,tmp,wNameLength,&dwRet,NULL))
		{
			dwWin32errorCode = GetLastError();
			free(tmp);
			return dwWin32errorCode;
		}
		strName = tmp;
		free(tmp);

		// Descriptor Val
		WORD wDataType;
		if(!ReadFile(hFile,&wDataType,sizeof(wDataType),&dwRet,NULL))
		{
			dwWin32errorCode = GetLastError();
			return dwWin32errorCode;
		}
		WORD wDataLen;
		if(!ReadFile(hFile,&wDataLen,sizeof(wDataLen),&dwRet,NULL))
		{
			dwWin32errorCode = GetLastError();
			return dwWin32errorCode;
		}
		char *dat = (char *)malloc(wDataLen);
		if(!ReadFile(hFile,dat,wDataLen,&dwRet,NULL))
		{
			dwWin32errorCode = GetLastError();
			free(dat);
			return dwWin32errorCode;
		}
		AddComment(strName,wDataType,dat,wDataLen);
		free(dat);
	}
	
	return dwWin32errorCode;
}

DWORD CTag_Wma::WriteDescString(HANDLE hFile,ULONGLONG &ullNewLength)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;
	DWORD	dwRet;
	///////////////////////////////
	// Content Descriptors Count
	WORD wCount = m_comments.size();
	if(!WriteFile(hFile,&wCount,sizeof(wCount),&dwRet,NULL))
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}
	ullNewLength += sizeof(WORD);

	map<CString,CTag_WmaComment>::iterator it;
	///////////////////////////////
	// Content Descriptors
	for(it=m_comments.begin(); it!=m_comments.end(); it++)
	{
		CTag_WmaComment *desc = (CTag_WmaComment *)&it->second;
		const CString *name = &it->first;
		WORD type = desc->m_wDataType;
		WORD len = desc->m_wLen;
		char *buf = desc->m_buf;
		// Descriptor Name Length
		int size;
		char *wname = TstrToDataAlloc(*name, -1, &size, DTC_CODE_UTF16LE);
		if (wname != NULL) {
			WORD clength = (WORD)size;
			if(!WriteFile(hFile,&clength,sizeof(clength),&dwRet,NULL))
			{
				dwWin32errorCode = GetLastError();
				free(wname);
				return dwWin32errorCode;
			}
			ullNewLength += sizeof(clength);
			// Descriptor Name
			if(!WriteFile(hFile,wname,clength,&dwRet,NULL))
			{
				dwWin32errorCode = GetLastError();
				free(wname);
				return dwWin32errorCode;
			}
			free(wname);
			ullNewLength += clength;
		}
		// Descriptor Value Data Type
		if(!WriteFile(hFile,&type,sizeof(type),&dwRet,NULL))
		{
			dwWin32errorCode = GetLastError();
			return dwWin32errorCode;
		}
		ullNewLength += sizeof(type);
		// Descriptor Value Length
		if(!WriteFile(hFile,&len,sizeof(len),&dwRet,NULL))
		{
			dwWin32errorCode = GetLastError();
			return dwWin32errorCode;
		}
		ullNewLength += sizeof(len);
		// Descriptor Value
		if(!WriteFile(hFile,buf,len,&dwRet,NULL))
		{
			dwWin32errorCode = GetLastError();
			return dwWin32errorCode;
		}
		ullNewLength += len;
	}
	
	return dwWin32errorCode;
}

BOOL CTag_Wma::AddComment(LPCTSTR name,WORD type,char *buf,WORD len)
{
	map<CString,CTag_WmaComment>::iterator p;
	p = m_comments.find(CString(name));
	if(p != m_comments.end())
	{
		// 元の値は削除
		m_comments.erase(p);
	}
	m_comments.insert(pair<CString,CTag_WmaComment>(name,CTag_WmaComment(type,buf,len)));
	
	return TRUE;
}

BOOL CTag_Wma::DellComment(LPCTSTR name)
{
	map<CString,CTag_WmaComment>::iterator p;
	p = m_comments.find(CString(name));
	if(p != m_comments.end())
	{
		// 元の値は削除
		m_comments.erase(p);
	}
	
	return TRUE;
}

BOOL CTag_Wma::AddCommentString(LPCTSTR name,CString str)
{
	BOOL ret = FALSE;
	int size;
	char *buf = TstrToDataAlloc(str, -1, &size, DTC_CODE_UTF16LE);
	if (buf != NULL) {
		ret = AddComment(name, 0, buf, size);
		free(buf);
	}
	return ret;
}

BOOL CTag_Wma::AddCommentDword(LPCTSTR name,DWORD val)
{
	return AddComment(name,3,(char *)&val,sizeof(DWORD));
}

BOOL CTag_Wma::GetComment(LPCTSTR name,WORD *type,WORD *len,char **buf)
{
	map<CString,CTag_WmaComment>::iterator p;
	p = m_comments.find(CString(name));
	if(p == m_comments.end())
	{
		return FALSE;
	}

	CTag_WmaComment *desc = (CTag_WmaComment *)&p->second;
	*type = desc->m_wDataType;
	*len = desc->m_wLen;
	*buf = desc->m_buf;
	
	return TRUE;
}

BOOL CTag_Wma::GetCommentString(LPCTSTR name,CString &str)
{
	WORD type;
	WORD len;
	char *buf;
	if(!GetComment(name,&type,&len,&buf))
	{
		return FALSE;
	}

	if(type != 0)
	{
		return FALSE;
	}
	LPWSTR wbuf = (LPWSTR)buf;
	int i;
	for (i = 0; i < len/sizeof(WCHAR); i++)
	{
		if (wbuf[i] == L'\0')
			break;
	}
	str = CString(wbuf, i);
	
	return TRUE;
}

BOOL CTag_Wma::GetCommentDword(LPCTSTR name,DWORD &val)
{
	WORD type;
	WORD len;
	char *buf;
	if(!GetComment(name,&type,&len,&buf))
	{
		return FALSE;
	}

	if((type != 3) || (len != sizeof(DWORD)))
	{
		return FALSE;
	}
	memcpy(&val,buf,sizeof(DWORD));
	
	return TRUE;
}

DWORD CTag_Wma::Save(LPCTSTR szFileName,LPTCPPROGRESS cbProgress)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;

	m_lldwPtr_File_Properties_Object = -1;
	GUID_SIZE guidSize;
	BYTE readBuf[64*1024];
	// 更新元ファイルを開く
	CString strPath;
	HANDLE hTempFile = INVALID_HANDLE_VALUE;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	hFile = CreateFile(szFileName,
						GENERIC_READ,
						FILE_SHARE_READ,
						NULL,
						OPEN_EXISTING,	// ファイルを開きます。指定したファイルが存在していない場合、この関数は失敗します。
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		dwWin32errorCode = GetLastError();
		goto err_exit;
	}
	if(cbProgress)
	{
		cbProgress(0,GetFileSize64(hFile));
	}

	//==================テンポラリを作成==================
	TCHAR szTempFile[MAX_PATH];
	strPath = getPathName(CString(szFileName));
	// テンポラリファイルを実際に作成
	if(!GetTempFileName(strPath,_T("tms"),0,szTempFile))
	{
		dwWin32errorCode = GetLastError();
		goto err_exit;
	}

	// テンポラリファイルオープン
	hTempFile = CreateFile(szTempFile,
						GENERIC_WRITE|GENERIC_READ,
						FILE_SHARE_READ,
						NULL,
						CREATE_ALWAYS,	// (指定ファイルがすでに存在している場合、そのファイルは上書きされます。)
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	if(hTempFile == INVALID_HANDLE_VALUE)
	{
		dwWin32errorCode = GetLastError();
		goto err_exit;
	}

	while(1)
	{
		ULONGLONG ullHeaderSize = 0;
		dwWin32errorCode = ReadGUID_Size(hFile,&guidSize);
		if(dwWin32errorCode != ERROR_SUCCESS)
		{
			dwWin32errorCode = ERROR_SUCCESS;
			break;	// EOF
		}
		TRACE(_T("GUID = {%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X} (%I64dbytes)\n"),
			guidSize.guid.Data1,guidSize.guid.Data2,guidSize.guid.Data3,
			guidSize.guid.Data4[0],guidSize.guid.Data4[1],guidSize.guid.Data4[2],guidSize.guid.Data4[3],guidSize.guid.Data4[4],guidSize.guid.Data4[5],guidSize.guid.Data4[6],guidSize.guid.Data4[7],
			guidSize.size
			);
		DWORD dwRet;
		// *****************************************
		// ヘッダの書き込み[仮]
		ullHeaderSize += sizeof(guidSize);
		__int64 lldwHeaderPtr = SetFilePointer64(hTempFile,0,FILE_CURRENT);
		if(!WriteFile(hTempFile,&guidSize,sizeof(guidSize),&dwRet,NULL))
		{
			dwWin32errorCode = GetLastError();
			goto err_exit;
		}

		if(guidSize.guid == ASF_Header_Object)
		{
			////////////////////////////////////////////////
			// ASF_Header_Object
			//
			TRACE(_T("--->(ASF_Header_Object)\n"));
			dwWin32errorCode = Save_Header_Object(hFile,hTempFile,guidSize.size-sizeof(guidSize),ullHeaderSize);
			if(dwWin32errorCode != ERROR_SUCCESS)
			{
				goto err_exit;
			}
			TRACE(_T("    len=%I64d\n"),ullHeaderSize);
			if(cbProgress)
			{
				cbProgress(SetFilePointer64(hFile,0,FILE_CURRENT),GetFileSize64(hFile));
			}
		}
		else
		{
			////////////////////////////////////////////////
			// それ以外
			//
			TRACE(_T("--->(それ以外)\n"));

			ULONGLONG ullRemain = guidSize.size - sizeof(guidSize);
			DWORD dwReadSize;
			// 64K毎にRead/Write
			do
			{
				// 読み取りサイズの決定
				if(ullRemain > sizeof(readBuf))
				{
					dwReadSize = sizeof(readBuf);
				}
				else
				{
					dwReadSize = (DWORD )ullRemain;
				}
				if(!ReadFile(hFile,readBuf,dwReadSize,&dwRet,NULL) || (dwRet != dwReadSize))
				{
					dwReadSize = 0;
					break;
				}
				if(!WriteFile(hTempFile,readBuf,dwReadSize,&dwRet,NULL))
				{
					dwWin32errorCode = GetLastError();
					goto err_exit;
				}
				ullRemain -= dwReadSize;
				ullHeaderSize += dwReadSize;
				if(cbProgress)
				{
					cbProgress(SetFilePointer64(hFile,0,FILE_CURRENT),GetFileSize64(hFile));
				}
			}
			while(ullRemain > 0);
			TRACE(_T("    len=%I64d\n"),ullHeaderSize);
		}
		// *****************************************
		// ヘッダの書き込み[確定]
		if(SetFilePointer64(hTempFile,lldwHeaderPtr+sizeof(guidSize.guid),FILE_BEGIN) < 0)
		{
			dwWin32errorCode = GetLastError();
			goto err_exit;
		}
//		ullHeaderSize -= 360;
		if(!WriteFile(hTempFile,&ullHeaderSize,sizeof(ullHeaderSize),&dwRet,NULL))
		{
			dwWin32errorCode = GetLastError();
			goto err_exit;
		}
		if(SetFilePointer64(hTempFile,ullHeaderSize-sizeof(guidSize),FILE_CURRENT) < 0)
		{
			dwWin32errorCode = GetLastError();
			goto err_exit;
		}
	}
	
	// File_Properties_ObjectのFileSizeを変更
	if(m_lldwPtr_File_Properties_Object != -1)
	{
		__int64 fileSize = GetFileSize64(hTempFile);
		if(SetFilePointer64(hTempFile,m_lldwPtr_File_Properties_Object+40/*File Size*/,FILE_BEGIN) < 0)
		{
			dwWin32errorCode = GetLastError();
			goto err_exit;
		}
		
		DWORD dwRet;
		if(!WriteFile(hTempFile,&fileSize,sizeof(fileSize),&dwRet,NULL) || (dwRet != sizeof(fileSize)))
		{
			dwWin32errorCode = GetLastError();
			goto err_exit;
		}
	}
	if(hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
	}
	if(hTempFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hTempFile);
	}
	//オリジナルファイルを退避(リネーム)
	TCHAR szPreFile[MAX_PATH];
	if(!GetTempFileName(strPath,_T("tms"),0,szPreFile))
	{
		dwWin32errorCode = GetLastError();
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}
	DeleteFile(szPreFile);//手抜き(^^;
	if(!MoveFile(szFileName,szPreFile))
	{
		dwWin32errorCode = GetLastError();
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}

	//完成品をリネーム
	if(!MoveFile(szTempFile,szFileName))
	{
		dwWin32errorCode = GetLastError();
		MoveFile(szPreFile,szFileName);
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}

	//オリジナルを削除
	DeleteFile(szPreFile);
	return dwWin32errorCode;

err_exit:
	if(hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
	}
	if(hTempFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hTempFile);
		DeleteFile(szTempFile);
	}
	return dwWin32errorCode;
}

DWORD CTag_Wma::Save_Header_Object(HANDLE hFile,HANDLE hTempFile,ULONGLONG size,ULONGLONG &writeSize)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;
	
	DWORD dwNumberOfHeaderObjects;
	BYTE reserved[2];
	DWORD dwRet;
	if(!ReadFile(hFile,&dwNumberOfHeaderObjects,sizeof(dwNumberOfHeaderObjects),&dwRet,NULL))
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}
	__int64 lldwNumberOfHeaderObjectsPtr = SetFilePointer64(hTempFile,0,FILE_CURRENT);
	if(!WriteFile(hTempFile,&dwNumberOfHeaderObjects,sizeof(dwNumberOfHeaderObjects),&dwRet,NULL))
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}
	writeSize += sizeof(dwNumberOfHeaderObjects);
	
	if(!ReadFile(hFile,reserved,sizeof(reserved),&dwRet,NULL))
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}
	if(!WriteFile(hTempFile,&reserved,sizeof(reserved),&dwRet,NULL))
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}
	writeSize += sizeof(reserved);
	
	__int64 llHeaderPtr;
	GUID_SIZE guidSize;
	ULONGLONG ullNewSize = 0;
	DWORD dwNewHeaderNum = 0;
	for(DWORD headerNum=0; headerNum<dwNumberOfHeaderObjects; headerNum++)
	{
		llHeaderPtr = SetFilePointer64(hTempFile,0,FILE_CURRENT);
		ullNewSize = 0;
		dwWin32errorCode = ReadGUID_Size(hFile,&guidSize);
		if(dwWin32errorCode != ERROR_SUCCESS)
		{
			dwWin32errorCode = ERROR_SUCCESS;
			break;	// 読み取り終了
		}
		TRACE(_T(">>GUID = {%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X} (%I64dbytes)\n"),
			guidSize.guid.Data1,guidSize.guid.Data2,guidSize.guid.Data3,
			guidSize.guid.Data4[0],guidSize.guid.Data4[1],guidSize.guid.Data4[2],guidSize.guid.Data4[3],guidSize.guid.Data4[4],guidSize.guid.Data4[5],guidSize.guid.Data4[6],guidSize.guid.Data4[7],
			guidSize.size
			);

		if(guidSize.guid == ASF_File_Properties_Object)
		{
			// ASF_File_Properties_Objectの場所を記憶(後でFileSizeを更新するため)
			m_lldwPtr_File_Properties_Object = SetFilePointer64(hTempFile,0,FILE_CURRENT);
		}

		if(	(guidSize.guid == ASF_Extended_Content_Description_Object) ||
			(guidSize.guid == ASF_Content_Description_Object) )
		{
			TRACE(_T("Skip\n"));
			if(SetFilePointer64(hFile,guidSize.size-sizeof(guidSize),FILE_CURRENT) < 0)
			{
				dwWin32errorCode = GetLastError();
				return dwWin32errorCode;
			}
			continue;
		}
		else
		{
			// *******************************************************
			// ヘッダの書き込み[1-仮]
			if(!WriteFile(hTempFile,&guidSize,sizeof(guidSize),&dwRet,NULL))
			{
				dwWin32errorCode = GetLastError();
				return dwWin32errorCode;
			}
			writeSize += sizeof(guidSize);
			ullNewSize += sizeof(guidSize);

			ULONGLONG ullRemain = guidSize.size - sizeof(guidSize);
			DWORD dwReadSize;
			BYTE readBuf[64*1024];
			// 64K毎にRead/Write
			do
			{
				// 読み取りサイズの決定
				if(ullRemain > sizeof(readBuf))
				{
					dwReadSize = sizeof(readBuf);
				}
				else
				{
					dwReadSize = (DWORD )ullRemain;
				}
				if(!ReadFile(hFile,readBuf,dwReadSize,&dwRet,NULL) || (dwRet != dwReadSize))
				{
					dwReadSize = 0;
					break;
				}
				if(!WriteFile(hTempFile,readBuf,dwReadSize,&dwRet,NULL))
				{
					dwWin32errorCode = GetLastError();
					return dwWin32errorCode;
				}
				writeSize += dwReadSize;
				ullNewSize += dwReadSize;
				ullRemain -= dwReadSize;
			}
			while(ullRemain > 0);
		}
		
		TRACE(_T(">>len=%I64d\n"),ullNewSize);
		// *******************************************************
		// ヘッダの書き込み[1-確定]
		if(SetFilePointer64(hTempFile,llHeaderPtr+sizeof(guidSize.guid),FILE_BEGIN) < 0)
		{
			dwWin32errorCode = GetLastError();
			return dwWin32errorCode;
		}
		if(!WriteFile(hTempFile,&ullNewSize,sizeof(ullNewSize),&dwRet,NULL))
		{
			dwWin32errorCode = GetLastError();
			return dwWin32errorCode;
		}
		if(SetFilePointer64(hTempFile,ullNewSize-sizeof(guidSize),FILE_CURRENT) < 0)
		{
			dwWin32errorCode = GetLastError();
			return dwWin32errorCode;
		}
		dwNewHeaderNum++;
	}

	////////////////////////////////////////////////////////
	// ASF_Content_Description_Object
	// 
	llHeaderPtr = SetFilePointer64(hTempFile,0,FILE_CURRENT);
	
	ullNewSize = 0;
	guidSize.guid = ASF_Content_Description_Object;
	// *******************************************************
	// ヘッダの書き込み[ASF_Content_Description_Object-仮]
	if(!WriteFile(hTempFile,&guidSize,sizeof(guidSize),&dwRet,NULL))
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}
	writeSize += sizeof(guidSize);
	ullNewSize += sizeof(guidSize);

	WORD clength;
	clength = (WORD)TstrToData(m_strTitle, -1, NULL, 0, DTC_CODE_UTF16LE);
	if(!WriteFile(hTempFile,&clength,sizeof(clength),&dwRet,NULL))
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}
	writeSize += sizeof(clength) + clength;
	ullNewSize += sizeof(clength) + clength;
	
	clength = (WORD)TstrToData(m_strAuthor, -1, NULL, 0, DTC_CODE_UTF16LE);
	if(!WriteFile(hTempFile,&clength,sizeof(clength),&dwRet,NULL))
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}
	writeSize += sizeof(clength) + clength;
	ullNewSize += sizeof(clength) + clength;
	
	clength = (WORD)TstrToData(m_strCopyright, -1, NULL, 0, DTC_CODE_UTF16LE);
	if(!WriteFile(hTempFile,&clength,sizeof(clength),&dwRet,NULL))
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}
	writeSize += sizeof(clength) + clength;
	ullNewSize += sizeof(clength) + clength;
	
	clength = (WORD)TstrToData(m_strDescription, -1, NULL, 0, DTC_CODE_UTF16LE);
	if(!WriteFile(hTempFile,&clength,sizeof(clength),&dwRet,NULL))
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}
	writeSize += sizeof(clength) + clength;
	ullNewSize += sizeof(clength) + clength;
	
	clength = (WORD)TstrToData(m_strRating, -1, NULL, 0, DTC_CODE_UTF16LE);
	if(!WriteFile(hTempFile,&clength,sizeof(clength),&dwRet,NULL))
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}
	writeSize += sizeof(clength) + clength;
	ullNewSize += sizeof(clength) + clength;
	
	WriteWchar(hTempFile,m_strTitle);
	WriteWchar(hTempFile,m_strAuthor);
	WriteWchar(hTempFile,m_strCopyright);
	WriteWchar(hTempFile,m_strDescription);
	WriteWchar(hTempFile,m_strRating);

	// *******************************************************
	// ヘッダの書き込み[ASF_Content_Description_Object-確定]
	if(SetFilePointer64(hTempFile,llHeaderPtr+sizeof(guidSize.guid),FILE_BEGIN) < 0)
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}
	if(!WriteFile(hTempFile,&ullNewSize,sizeof(ullNewSize),&dwRet,NULL))
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}
	if(SetFilePointer64(hTempFile,ullNewSize-sizeof(guidSize),FILE_CURRENT) < 0)
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}
	dwNewHeaderNum++;
	////////////////////////////////////////////////////////
	// ASF_Content_Description_Object ここまで
	////////////////////////////////////////////////////////
	
	////////////////////////////////////////////////////////
	// ASF_Extended_Content_Description_Object
	// 
	llHeaderPtr = SetFilePointer64(hTempFile,0,FILE_CURRENT);
	
	ullNewSize = 0;
	guidSize.guid = ASF_Extended_Content_Description_Object;
	// *******************************************************
	// ヘッダの書き込み[ASF_Extended_Content_Description_Object-仮]
	if(!WriteFile(hTempFile,&guidSize,sizeof(guidSize),&dwRet,NULL))
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}
	writeSize += sizeof(guidSize);
	ullNewSize += sizeof(guidSize);

	ULONGLONG ullNewDescLength = 0;
	WriteDescString(hTempFile,ullNewDescLength);
	writeSize += ullNewDescLength;
	ullNewSize += ullNewDescLength;

	// *******************************************************
	// ヘッダの書き込み[ASF_Extended_Content_Description_Object-確定]
	if(SetFilePointer64(hTempFile,llHeaderPtr+sizeof(guidSize.guid),FILE_BEGIN) < 0)
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}
	if(!WriteFile(hTempFile,&ullNewSize,sizeof(ullNewSize),&dwRet,NULL))
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}
	if(SetFilePointer64(hTempFile,ullNewSize-sizeof(guidSize),FILE_CURRENT) < 0)
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}
	dwNewHeaderNum++;
	////////////////////////////////////////////////////////
	// ASF_Extended_Content_Description_Object ここまで
	////////////////////////////////////////////////////////

	__int64 llNowPtr = SetFilePointer64(hTempFile,0,FILE_CURRENT);
	// ヘッダ数の確定
	if(SetFilePointer64(hTempFile,lldwNumberOfHeaderObjectsPtr,FILE_BEGIN) < 0)
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}
	if(!WriteFile(hTempFile,&dwNewHeaderNum,sizeof(dwNewHeaderNum),&dwRet,NULL))
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}

	if(SetFilePointer64(hTempFile,llNowPtr,FILE_BEGIN) < 0)
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}

	return dwWin32errorCode;
}
