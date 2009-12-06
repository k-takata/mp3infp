// Tag_OpenDML.h: CTag_OpenDML クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Tag_OpenDML_H__E33771A1_E2B4_4AE7_8315_1B62735A6280__INCLUDED_)
#define AFX_Tag_OpenDML_H__E33771A1_E2B4_4AE7_8315_1B62735A6280__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "DlgWriteProgress.h"

#pragma warning(disable:4786)
#include <map>
using namespace std;

#include <Mmsystem.h>

class CTag_OpenDML  
{
public:
	CTag_OpenDML();
	virtual ~CTag_OpenDML();
	void Release();
	BOOL IsEnable(){return m_bEnable;};
	void SetJunkHeader(LPCTSTR junk){m_strJunkHeader = junk;};
	DWORD GetStreamSize(){return m_dwStreamSize;};
	BOOL SetField(char id1,char id2,char id3,char id4,LPCTSTR szData);
	CString GetField(char id1,char id2,char id3,char id4);
	DWORD Load(LPCTSTR szFileName,char id1,char id2,char id3,char id4);
	DWORD Save(HWND hWnd,LPCTSTR szFileName);

protected:
	BOOL m_bEnable;
//	CString m_szFileName;
//	CDlgWriteProgress m_dlg;
private:
	DWORD FindJUNK_LISTINFO(HANDLE hFile,__int64 llFileSize);
	BOOL FindChunk(HANDLE hFile,__int64 llFileSize,UINT flag,FOURCC type,DWORD *pdwSize);
	DWORD GetTotalFieldSize();
	DWORD Save_1(HANDLE hFile);
	FOURCC m_type;
	DWORD GetInfoChunkSize();
	map<FOURCC,CString> m_fields;
	DWORD m_dwStreamSize;
	CString m_strJunkHeader;
};

#endif // !defined(AFX_Tag_OpenDML_H__E33771A1_E2B4_4AE7_8315_1B62735A6280__INCLUDED_)
