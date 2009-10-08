// M3u.h: CM3u クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_M3U_H__6D04A85F_132F_48E2_A7D9_53F9C27CFEF4__INCLUDED_)
#define AFX_M3U_H__6D04A85F_132F_48E2_A7D9_53F9C27CFEF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CM3u  
{
public:
	CM3u();
	virtual ~CM3u();
	void	Release();
	BOOL	IsEnable(){return TRUE;};

	DWORD	Load(const char *szFileName);
	DWORD	Save(const char *szFileName);
	BOOL	Add(char *szLine);
	BOOL	GetLine(int iLine,CString &str);

private:

	CStringArray m_strLines;

};

#endif // !defined(AFX_M3U_H__6D04A85F_132F_48E2_A7D9_53F9C27CFEF4__INCLUDED_)
