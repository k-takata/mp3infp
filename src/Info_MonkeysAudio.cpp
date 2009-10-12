// Info_MonkeysAudio.cpp: CInfo_MonkeysAudio クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include <io.h>
//#include <fcntl.h>
#include "GlobalCommand.h"

#include "Info_MonkeysAudio.h"

static const int COMPRESSION_LEVEL_FAST			= 1000;
static const int COMPRESSION_LEVEL_NORMAL		= 2000;
static const int COMPRESSION_LEVEL_HIGH			= 3000;
static const int COMPRESSION_LEVEL_EXTRA_HIGH	= 4000;

static const int MAC_FORMAT_FLAG_8_BIT				= 1;
static const int MAC_FORMAT_FLAG_CRC				= 2;
static const int MAC_FORMAT_FLAG_HAS_PEAK_LEVEL		= 4;
static const int MAC_FORMAT_FLAG_24_BIT				= 8;
static const int MAC_FORMAT_FLAG_HAS_SEEK_ELEMENTS	= 16;
static const int MAC_FORMAT_FLAG_CREATE_WAV_HEADER	= 32;

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CInfo_MonkeysAudio::CInfo_MonkeysAudio()
{
	Release();
}

CInfo_MonkeysAudio::~CInfo_MonkeysAudio()
{

}

void CInfo_MonkeysAudio::Release()
{
	m_strAudioFormat = "";
	m_strFormatInfo = "";
	m_strAudioInfo = "";
	m_strSizeInfo = "";
	m_strTime = "";
}

DWORD CInfo_MonkeysAudio::Load(LPCTSTR szFileName)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;
	Release();

	//ファイルをオープン
	HANDLE hFile = CreateFile(
				szFileName,
				GENERIC_READ,
				FILE_SHARE_READ,
				NULL,
				OPEN_EXISTING,	//ファイルをオープンします。指定ファイルが存在していない場合、関数は失敗します。
				FILE_ATTRIBUTE_NORMAL,
				NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}

	DWORD dwRet;
	// APEヘッダの先頭4バイトをロード
	APE_COMMON_HEADER header;
	if(!ReadFile(hFile,&header,sizeof(header),&dwRet,NULL) || (dwRet != sizeof(header)))
	{
		CloseHandle(hFile);
		return dwWin32errorCode;
	}
	if(strncmp(header.cID,"MAC ",4) != 0)
	{
		CloseHandle(hFile);
		return dwWin32errorCode;
	}
	if(header.nVersion <= 3970)
	{
		//
		// v3.97とそれ以前の古いヘッダの読込
		//
		if(!ReadFile(hFile,(char *)&m_apeHeaderOld,sizeof(m_apeHeaderOld),&dwRet,NULL) || (dwRet != sizeof(m_apeHeaderOld)))
		{
			CloseHandle(hFile);
			return dwWin32errorCode;
		}
		int nPeakLevel = -1;
		if(m_apeHeaderOld.nFormatFlags & MAC_FORMAT_FLAG_HAS_PEAK_LEVEL)
		{
			if(!ReadFile(hFile,(char *)&nPeakLevel,sizeof(nPeakLevel),&dwRet,NULL) || (dwRet != sizeof(nPeakLevel)))
			{
				CloseHandle(hFile);
				return dwWin32errorCode;
			}
		}
		// フォーマット文字列を作成
		int nBitsPerSample;
		if(m_apeHeaderOld.nFormatFlags & MAC_FORMAT_FLAG_8_BIT)
		{
			nBitsPerSample = 8;
		}
		else if(m_apeHeaderOld.nFormatFlags & MAC_FORMAT_FLAG_24_BIT)
		{
			nBitsPerSample = 24;
		}
		else
		{
			nBitsPerSample = 16;
		}
		m_strAudioFormat.Format(_T("Monkey's Audio v%.2lf, %dbit, %dHz, %dch"),
								(double )header.nVersion/1000.0,
								nBitsPerSample,
								m_apeHeaderOld.nSampleRate,
								m_apeHeaderOld.nChannels);
		CString strCompLevel;
		switch(m_apeHeaderOld.nCompressionLevel){
		case COMPRESSION_LEVEL_FAST:
			strCompLevel.Format(_T("Fast"));	break;
		case COMPRESSION_LEVEL_NORMAL:
			strCompLevel.Format(_T("Normal"));	break;
		case COMPRESSION_LEVEL_HIGH:
			strCompLevel.Format(_T("High"));	break;
		case COMPRESSION_LEVEL_EXTRA_HIGH:
			strCompLevel.Format(_T("Extra High"));break;
		default:
			strCompLevel.Format(_T("Unknown"));	break;
		}
		m_strFormatInfo.Format(_T("Version: %.2lf\n")
							_T("Mode: %s\n")
							_T("Format Flags: 0x%x\n"),
				(double )header.nVersion/1000.0,
				strCompLevel,
				m_apeHeaderOld.nFormatFlags);
		m_strAudioInfo.Format(_T("Sample Rate: %d\n")
							_T("Channels: %d\n")
							_T("Bits Per Sample: %d\n")
							_T("Peak Level: %d\n"),
				m_apeHeaderOld.nSampleRate,
				m_apeHeaderOld.nChannels,
				nBitsPerSample,
				nPeakLevel);
		int nBlocksPerFrame = ((header.nVersion >= 3900) || ((header.nVersion >= 3800) && (m_apeHeaderOld.nCompressionLevel == COMPRESSION_LEVEL_EXTRA_HIGH))) ? 73728 : 9216;
		if ((header.nVersion >= 3950)) nBlocksPerFrame = 73728 * 4;
		int nTotalBlocks = (!m_apeHeaderOld.nTotalFrames)?0:((m_apeHeaderOld.nTotalFrames-1)*nBlocksPerFrame)+m_apeHeaderOld.nFinalFrameBlocks;
		int msec = 0;
		if(m_apeHeaderOld.nSampleRate)	// 0除算防止
		{
			msec = (int )((__int64 )nTotalBlocks * (__int64 )1000 / (__int64 )m_apeHeaderOld.nSampleRate);
		}
		int nBlockAlign = nBitsPerSample / 8 * m_apeHeaderOld.nChannels;
		int wavDataBytes = nBlockAlign*nTotalBlocks;
		int wavHeaderBytes = (m_apeHeaderOld.nFormatFlags&MAC_FORMAT_FLAG_CREATE_WAV_HEADER)?sizeof(WAVE_HEADER):m_apeHeaderOld.nHeaderBytes;
		int wavTerminatingBytes	= m_apeHeaderOld.nTerminatingBytes;

		int wavSize = wavDataBytes+wavHeaderBytes+wavTerminatingBytes;
		int apeSize = GetFileSize(hFile,NULL);
		m_strSizeInfo.Format(_T("Length: %d:%d\n")
							_T("WAV: %.1lfMB\n")
							_T("APE: %.1lfMB\n")
							_T("Compression: %.1lf%%\n"),
							msec/1000/60,msec/1000%60,
							(double )(wavSize/1024/1024.0),
							(double )(apeSize/1024/1024.0),
							(wavSize==0)?0.0:((double )apeSize/(double )wavSize*100.0)
							);
		m_strTime.Format(_T("%ld:%02ld (%ldsec)"),
							msec/1000/60,
							msec/1000%60,
							msec/1000);
	}
	else
	{
		// apeデクリプタの読込
		SetFilePointer(hFile,0,NULL,FILE_BEGIN);
		if(!ReadFile(hFile,(char *)&m_apeDescriptor,sizeof(APE_DESCRIPTOR),&dwRet,NULL))
		{
			CloseHandle(hFile);
			return dwWin32errorCode;
		}
		if(m_apeDescriptor.nDescriptorBytes - dwRet)
		{
			SetFilePointer(hFile,m_apeDescriptor.nDescriptorBytes - dwRet,NULL,FILE_CURRENT);
		}
		// apeヘッダの読込
		if(!ReadFile(hFile,(char *)&m_apeHeader,sizeof(m_apeHeader),&dwRet,NULL))
		{
			CloseHandle(hFile);
			return dwWin32errorCode;
		}
		if(m_apeDescriptor.nHeaderBytes - dwRet)
		{
			SetFilePointer(hFile,m_apeDescriptor.nHeaderBytes - dwRet,NULL,FILE_CURRENT);
		}
		// フォーマット文字列を作成
		m_strAudioFormat.Format(_T("Monkey's Audio v%.2lf, %dbit, %dHz, %dch"),
								(double )header.nVersion/1000.0,
								m_apeHeader.nBitsPerSample,
								m_apeHeader.nSampleRate,
								m_apeHeader.nChannels);
		CString strCompLevel;
		switch(m_apeHeader.nCompressionLevel){
		case COMPRESSION_LEVEL_FAST:
			strCompLevel.Format(_T("Fast"));	break;
		case COMPRESSION_LEVEL_NORMAL:
			strCompLevel.Format(_T("Normal"));	break;
		case COMPRESSION_LEVEL_HIGH:
			strCompLevel.Format(_T("High"));	break;
		case COMPRESSION_LEVEL_EXTRA_HIGH:
			strCompLevel.Format(_T("Extra High"));break;
		default:
			strCompLevel.Format(_T("Unknown"));	break;
		}
		m_strFormatInfo.Format(_T("Version: %.2lf\n")
							_T("Mode: %s\n")
							_T("Format Flags: 0x%x\n"),
				(double )header.nVersion/1000.0,
				strCompLevel,
				m_apeHeader.nFormatFlags);
		m_strAudioInfo.Format(_T("Sample Rate: %d\n")
							_T("Channels: %d\n")
							_T("Bits Per Sample: %d\n"),
				m_apeHeader.nSampleRate,
				m_apeHeader.nChannels,
				m_apeHeader.nBitsPerSample
				);
		int nBlocksPerFrame = m_apeHeader.nBlocksPerFrame;
		int nTotalBlocks =  (m_apeHeader.nTotalFrames == 0) ? 0 : ((m_apeHeader.nTotalFrames -  1) * m_apeHeader.nBlocksPerFrame) + m_apeHeader.nFinalFrameBlocks;
		int msec = int((double(nTotalBlocks) * double(1000)) / double(m_apeHeader.nSampleRate));
		
		int nBlockAlign = m_apeHeader.nBitsPerSample / 8 * m_apeHeader.nChannels;
		int wavDataBytes = nBlockAlign*nTotalBlocks;
		
		int wavHeaderBytes = (m_apeHeader.nFormatFlags & MAC_FORMAT_FLAG_CREATE_WAV_HEADER) ? sizeof(WAVE_HEADER) : m_apeDescriptor.nHeaderDataBytes;
		int wavTerminatingBytes	= m_apeDescriptor.nTerminatingDataBytes;

		int wavSize = wavDataBytes+wavHeaderBytes+wavTerminatingBytes;
		int apeSize = GetFileSize(hFile,NULL);
		m_strSizeInfo.Format(_T("Length: %d:%d\n")
							_T("WAV: %.1lfMB\n")
							_T("APE: %.1lfMB\n")
							_T("Compression: %.1lf%%\n"),
							msec/1000/60,msec/1000%60,
							(double )(wavSize/1024/1024.0),
							(double )(apeSize/1024/1024.0),
							(wavSize==0)?0.0:((double )apeSize/(double )wavSize*100.0)
							);
		m_strTime.Format(_T("%ld:%02ld (%ldsec)"),
							msec/1000/60,
							msec/1000%60,
							msec/1000);
	}

	CloseHandle(hFile);

	return dwWin32errorCode;
}

