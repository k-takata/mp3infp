// Mp3Info.h: CMp3Info クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MP3INFO_H__0B43E243_8C43_45E9_A96A_F793478F30CC__INCLUDED_)
#define AFX_MP3INFO_H__0B43E243_8C43_45E9_A96A_F793478F30CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4786)
#include <list>

class CMp3Info  
{
private:
#pragma pack(1)
	typedef struct tFRAMEINFO
	{
		DWORD dwPtr;
		DWORD dwSize;
	}FRAMEINFO;

	typedef struct tMPEGINFO
	{
		int lsf;
		int mpeg25;
		int lay;
		int sampling_frequency;
		int error_protection;
		int bitrate_index;
		int padding;
		int extension;
		int mode;
		int mode_ext;
		int copyright;
		int original;
		int emphasis;
		int stereo;
		unsigned long bps;
		int framesize;
		unsigned long msec;
		unsigned long size;
		unsigned long flmnum;
		BOOL bVbr;
	}MPEGINFO;

	typedef struct _VBRI
	{
		char magic[4];
		unsigned short version;
		unsigned short delay;
		unsigned short quality;
		unsigned long streamBytes;
		unsigned long streamFrames;
		unsigned short tableSize;
		unsigned short tableScale; 
		unsigned short entryBytes;
		unsigned short entryFrames;
	}VBRI;
#pragma pack()

public:
	CMp3Info();
	virtual ~CMp3Info();
	BOOL Release();
	BOOL Load(LPCTSTR szFileName,BOOL bVbrScan = FALSE);

	CString GetFormatString(){return m_strFormat;};
	CString GetTimeString(){return m_strTime;};

private:
	std::list<FRAMEINFO> m_listFrame;
	MPEGINFO m_mpegInfo;

	const BOOL IsVbr();
	const CString GetMpeg();
	const long GetLayers();
	const long GetBps();
	const long GetFreqs();
	const CString GetModes();
	const unsigned long GetMSec();
	const unsigned long GetFlmNum();
	CString m_strFormat;
	CString m_strTime;

};

#endif // !defined(AFX_MP3INFO_H__0B43E243_8C43_45E9_A96A_F793478F30CC__INCLUDED_)
