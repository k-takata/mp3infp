// Vqf.h: CVqf クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VQF_H__E33771A1_E2B4_4AE7_8315_1B62735A6280__INCLUDED_)
#define AFX_VQF_H__E33771A1_E2B4_4AE7_8315_1B62735A6280__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4786)
#include <map>
using namespace std;

class CVqfTag
{
public:
	CVqfTag()
	{
		m_data = NULL;
		m_dwSize = 0;
	};
	virtual ~CVqfTag()
	{
		Release();
	};
	void Release()
	{
		if(m_data)
		{
			free(m_data);
		}
		m_data = NULL;
		m_dwSize = 0;
	}
	CVqfTag(const CVqfTag &obj)	//コピーコンストラクタ
	{
		m_data = (unsigned char *)malloc(obj.m_dwSize+1);
		memcpy(m_data,obj.m_data,obj.m_dwSize);
		m_data[obj.m_dwSize] = '\0';	//Cの文字列として使えるように
		m_dwSize = obj.m_dwSize;
	};
	DWORD GetSize(){return m_dwSize;};
	void SetSize(DWORD size){m_dwSize = size;};
	unsigned char *GetData(){return m_data;};
	void SetData(const unsigned char *data,DWORD size)
	{
		if(m_data)
		{
			free(m_data);
		}
		m_data = (unsigned char *)malloc(size+1);
		if(!m_data)
		{
			return;	//メモリを確保できなかった
		}
		memcpy(m_data,data,size);
		m_data[size] = '\0';	//Cの文字列として使えるように
		m_dwSize = size;
	};

private:
	unsigned char	*m_data;
	DWORD	m_dwSize;
};

class CVqf
{
public:
	CVqf();
	virtual ~CVqf();
	void	Release();
	BOOL	IsEnable(){return m_bEnable;};
	BOOL	SetField(char id1,char id2,char id3,char id4,const unsigned char *szData,DWORD dwSize);
	BOOL	SetField(char id1,char id2,char id3,char id4,LPCTSTR szStr);
	unsigned char *GetField(char id1,char id2,char id3,char id4,DWORD *pdwSize);
	CString GetField(char id1,char id2,char id3,char id4);
	DWORD	GetTotalFieldSize();

	DWORD	GetStreamSize(){return m_dwStreamSize;};
	DWORD	GetStereo(){return m_dwStereo;};
	DWORD	GetRate(){return m_dwCompRate;};
	DWORD	GetSamplFreq(){return m_dwSamplingFrequency;};
	CString GetVer(){return m_strVer;};
	
	DWORD	Load(LPCTSTR szFileName);
	DWORD	Save(HWND hWnd,LPCTSTR szFileName);

	CString GetFormatString();
	CString GetTimeString();

private:
	BOOL				m_bEnable;

	DWORD MakeKey(char id1,char id2,char id3,char id4)
	{
		return id1<<24 | id2<<16 | id3<<8 | id4;
	}
	void MakeBeSize(DWORD dwSize,char size[4])
	{
		size[3] = char(dwSize & 0xff);
		size[2] = char((dwSize>>8) & 0xff);
		size[1] = char((dwSize>>16) & 0xff);
		size[0] = char((dwSize>>24) & 0xff);
	}
	map<DWORD,CVqfTag>	m_fields;
	DWORD				m_dwStreamSize;
	DWORD				m_dwStereo;
	DWORD				m_dwCompRate;
	DWORD				m_dwSamplingFrequency;
	CString				m_strVer;
};

#endif // !defined(AFX_VQF_H__E33771A1_E2B4_4AE7_8315_1B62735A6280__INCLUDED_)
