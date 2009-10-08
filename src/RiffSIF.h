// RiffSIF2.h: CRiffSIF クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RIFFSIF_H__E33771A1_E2B4_4AE7_8315_1B62735A6280__INCLUDED_)
#define AFX_RIFFSIF_H__E33771A1_E2B4_4AE7_8315_1B62735A6280__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "DlgWriteProgress.h"

#pragma warning(disable:4786)
#include <map>
using namespace std;

#include <Mmsystem.h>

class CRiffSIF  
{
public:
	CRiffSIF();
	virtual ~CRiffSIF();
	void Release();
	BOOL IsEnable(){return m_bEnable;};
	void SetJunkHeader(char *junk){m_strJunkHeader = junk;};
	DWORD GetStreamSize(){return m_dwStreamSize;};
	BOOL SetField(char id1,char id2,char id3,char id4,const char *szData);
	CString GetField(char id1,char id2,char id3,char id4);
	BOOL FindChunk(HANDLE hFile,DWORD dwFileSize,UINT flag,FOURCC type,DWORD *pdwSize,BOOL bModify);
	DWORD GetTotalFieldSize();
	DWORD Load(const char *szFileName,char id1,char id2,char id3,char id4);
	DWORD Save(HWND hWnd,const char *szFileName);

protected:
	BOOL m_bEnable;
//	CString m_szFileName;
//	CDlgWriteProgress m_dlg;
private:
	FOURCC m_type;
	DWORD GetInfoChunkSize();
	map<FOURCC,CString> m_fields;
	DWORD m_dwStreamSize;
	CString m_strJunkHeader;
};

#endif // !defined(AFX_RIFFSIF_H__E33771A1_E2B4_4AE7_8315_1B62735A6280__INCLUDED_)
