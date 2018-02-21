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
	enum EN_ENCODING {
		ENC_ANSI, ENC_UTF8N, ENC_UTF8B
	};

	CM3u();
	virtual ~CM3u();
	void	Release();
	BOOL	IsEnable(){return TRUE;};

	DWORD	Load(LPCTSTR szFileName);
	DWORD	Save(LPCTSTR szFileName);
	BOOL	Add(LPCTSTR szLine);
	BOOL	GetLine(int iLine,CString &str);

	//void		SetEncoding(EN_ENCODING enc){m_encoding = enc;};
	//EN_ENCODING	GetEncoding(){return m_encoding;};

private:

	CStringArray m_strLines;
	EN_ENCODING m_encoding;

};

#endif // !defined(AFX_M3U_H__6D04A85F_132F_48E2_A7D9_53F9C27CFEF4__INCLUDED_)
