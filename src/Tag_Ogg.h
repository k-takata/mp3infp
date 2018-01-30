// Tag_Ogg.h: CTag_Ogg �N���X�̃C���^�[�t�F�C�X
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TAG_OGG_H__6C5D4E96_2B7C_4333_A4F8_3CF81D10E6F5__INCLUDED_)
#define AFX_TAG_OGG_H__6C5D4E96_2B7C_4333_A4F8_3CF81D10E6F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4786)
#include <map>
#include <list>

/*
Memo:
�E���O�͑啶���Ŋi�[���Ă���B�������啶���ōs�����ƁB

2002-01-20
#pragma comment(lib,"ogg_static.lib")
#pragma comment(lib,"vorbis_static.lib")
#pragma comment(lib,"vorbisfile_static.lib")
�����̃��C�u������msvcrt.dll���X�^�e�B�b�N�����N�Ŏg�p����悤�ɕύX����

*/

class CTag_Ogg  
{
public:
	CTag_Ogg();
	virtual ~CTag_Ogg();
	void	Release();
	BOOL	IsEnable(){return TRUE;};

	BOOL	AddComment(LPCTSTR name,LPCTSTR value);
	BOOL	ReplaceComment(LPCTSTR name,LPCTSTR value,int index);
	BOOL	DelComment(LPCTSTR name,int index);
	BOOL	GetComment(LPCTSTR name,int index,CString &strValue);
	void	GetCommentNames(CStringArray &strArray);

	DWORD	Load(LPCTSTR szFileName);
	DWORD	Save(LPCTSTR szFileName);

	CString GetAudioFormatString(){return m_strAudioFormat;};
	CString GetTimeString(){return m_strTime;};
	int		GetVersion(){return m_iVersion;};
	long	GetSerial(){return m_lSerial;};
	CString GetVendor(){return m_strVendor;};
	long	GetBitrateAvg(){return m_lBitrate_avg;};
	long	GetBitrateUpper(){return m_lBitrate_upper;};
	long	GetBitrateNominal(){return m_lBitrate_nominal;};
	long	GetBitrateLower(){return m_lBitrate_lower;};
	long	GetBitrateWindow(){return m_lBitrate_window;};

private:
	CString m_strAudioFormat;
	CString m_strTime;
	int		m_iVersion;
	long	m_lSerial;
	CString m_strVendor;
	long	m_lChannels;
	long	m_lSampleRate;
	long	m_lBitrate_avg;
	long	m_lBitrate_upper;
	long	m_lBitrate_nominal;
	long	m_lBitrate_lower;
	long	m_lBitrate_window;

//	CString m_strTagTITLE;
//	CString m_strTagTRACKNUMBER;
//	CString m_strTagARTIST;
//	CString m_strTagALBUM;
//	CString m_strTagCOMMENT;
//	CString m_strTagDATE;
//	CString m_strTagGENRE;
	//ORGANIZATION
	//DESCRIPTION
	//LOCATION
	//COPYRIGHT
	typedef std::multimap<CString,CString> CommentMap;
	CommentMap m_comments;

/*
	CString Acp2Utf8(const char *str);
	CString Utf82Acp(const char *str);
	void Ucs22Utf8(const WCHAR *str,char *buf,int size);
	void Utf82Ucs2(const char *str,WCHAR *wBuf,int size);
*/
};

#endif // !defined(AFX_TAG_OGG_H__6C5D4E96_2B7C_4333_A4F8_3CF81D10E6F5__INCLUDED_)
