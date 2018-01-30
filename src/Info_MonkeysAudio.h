// Info_MonkeysAudio.h: CInfo_MonkeysAudio �N���X�̃C���^�[�t�F�C�X
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Info_MonkeysAudio_H__6C5D4E96_2B7C_4333_A4F8_3CF81D10E6F5__INCLUDED_)
#define AFX_Info_MonkeysAudio_H__6C5D4E96_2B7C_4333_A4F8_3CF81D10E6F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4786)
#include <map>
#include <list>

class CInfo_MonkeysAudio  
{
public:
	CInfo_MonkeysAudio();
	virtual ~CInfo_MonkeysAudio();
	void	Release();
	BOOL	IsEnable(){return TRUE;};
	DWORD	Load(LPCTSTR szFileName);

	CString GetAudioFormatString(){return m_strAudioFormat;};
	CString GetFormatInfoString(){return m_strFormatInfo;};
	CString GetAudioInfoString(){return m_strAudioInfo;};
	CString GetSizeInfoString(){return m_strSizeInfo;};
	CString GetTimeString(){return m_strTime;};

private:
#pragma pack(1)
	typedef struct _WAVE_HEADER
	{
		// RIFF header
		char cRIFFHeader[4];
		unsigned int nRIFFBytes;
		// data type
		char cDataTypeID[4];
		// wave format
		char cFormatHeader[4];
		unsigned int nFormatBytes;
		unsigned short nFormatTag;
		unsigned short nChannels;
		unsigned int nSamplesPerSec;
		unsigned int nAvgBytesPerSec;
		unsigned short nBlockAlign;
		unsigned short nBitsPerSample;
		// data chunk header
		char cDataHeader[4];
		unsigned int nDataBytes;
	}WAVE_HEADER;

	typedef struct _APE_COMMON_HEADER
	{
		char cID[4];							// "MAC "
		unsigned short	nVersion;				// �o�[�W�����ԍ� * 1000 (3.81 = 3810)
	}APE_COMMON_HEADER;

	// v3.97 �Ƃ���ȑO�p
	typedef struct _APE_HEADER_OLD
	{
//		char cID[4];							// "MAC "
//		unsigned short	nVersion;				// �o�[�W�����ԍ� * 1000 (3.81 = 3810)
		unsigned short	nCompressionLevel;		// ���k���x��
		unsigned short	nFormatFlags;			// �t�H�[�}�b�g�t���O
		unsigned short	nChannels;				// �`�����l���� (1 or 2)
		unsigned int	nSampleRate;			// �T���v�����[�g (��������44100)
		unsigned int	nHeaderBytes;			// MAC�w�b�_�ɑ���WAV�w�b�_�̃T�C�Y
		unsigned int	nTerminatingBytes;		// raw�f�[�^�̃T�C�Y (�g���f�[�^)
		unsigned int	nTotalFrames;			// �t�@�C���̑��t���[����
		unsigned int	nFinalFrameBlocks;		// �ŏI�t���[���ɂ�����T���v����
	}APE_HEADER_OLD;

	typedef struct _APE_DESCRIPTOR
	{
		char cID[4];							// "MAC "
		unsigned short	nVersion;				// �o�[�W�����ԍ� * 1000 (3.81 = 3810)
		short dummy;
		unsigned int	nDescriptorBytes;		// 
		unsigned int	nHeaderBytes;			// 
		unsigned int	nSeekTableBytes;		// 
		unsigned int	nHeaderDataBytes;		// 
		unsigned int	nAPEFrameDataBytes;		// 
		unsigned int	nAPEFrameDataBytesHigh;	// 
		unsigned int	nTerminatingDataBytes;	// 
		unsigned char	cFileMD5[16];			// 
	}APE_DESCRIPTOR;

	typedef struct _APE_HEADER
	{
		unsigned short	nCompressionLevel;		// ���k���x��
		unsigned short	nFormatFlags;			// �t�H�[�}�b�g�t���O
		unsigned int	nBlocksPerFrame;		// 1�t���[��������̃T���v����
		unsigned int	nFinalFrameBlocks;		// �ŏI�t���[���ɂ�����T���v����
		unsigned int	nTotalFrames;			// ���t���[����
		unsigned short	nBitsPerSample;			// 1�T���v���̃r�b�g�� (�����Ă�16)
		unsigned short	nChannels;				// �`���l���� (1 or 2)
		unsigned int	nSampleRate;			// �T���v�����[�g (�����Ă� 44100)
	}APE_HEADER;

#pragma pack()
	CString m_strAudioFormat;
	CString m_strFormatInfo;
	CString m_strAudioInfo;
	CString m_strSizeInfo;
	CString m_strTime;
	APE_HEADER_OLD m_apeHeaderOld;
	APE_DESCRIPTOR m_apeDescriptor;
	APE_HEADER m_apeHeader;

	std::map<CString,CString> m_comments;
};

#endif // !defined(AFX_Info_MonkeysAudio_H__6C5D4E96_2B7C_4333_A4F8_3CF81D10E6F5__INCLUDED_)
