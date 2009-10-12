// Id3tagv1.h: CId3tagv1 クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ID3TAGV1_H__81689DF7_1DE7_47C6_A34A_41AFEECC3184__INCLUDED_)
#define AFX_ID3TAGV1_H__81689DF7_1DE7_47C6_A34A_41AFEECC3184__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CId3tagv1  
{
public:
	CId3tagv1(BOOL bScmpxGenre = FALSE);
	virtual ~CId3tagv1();
	void SetScmpxGenre(BOOL bSwitch);
	BOOL IsEnable(){return m_bEnable;};

	CString GetTitle();
	void SetTitle(LPCTSTR title);
	CString GetArtist();
	void SetArtist(LPCTSTR artist);
	CString GetAlbum();
	void SetAlbum(LPCTSTR album);
	CString GetYear();
	void SetYear(LPCTSTR year);
	unsigned char GetGenreNum();
	CString GetGenre();
	void SetGenre(unsigned char cGenre);
	void SetGenre(LPCTSTR szGenre);
	unsigned char GetTrackNoNum();
	CString GetTrackNo();
	void SetTrackNo(unsigned char cTrackNo);
	void SetTrackNo(LPCTSTR szTrackNo);
	CString GetComment();
	void SetComment(LPCTSTR comment);

	CString GenreNum2String(unsigned char cGenre);
	long GenreString2Num(LPCTSTR szGenre);

	DWORD Load(LPCTSTR szFileName);
	DWORD LoadMulti(LPCTSTR szFileName);
	DWORD Save(HWND hWnd,LPCTSTR szFileName);
	DWORD DelTag(HWND hWnd,LPCTSTR szFileName);
	void GetId3tagString(char *szTag);
	DWORD MakeTag(HWND hWnd,LPCTSTR szFileName);

private:
	void Release();
	BOOL m_bEnable;			//ID3TAGが無い場合はFALSE
	BOOL m_bScmpxGenre;		//SCMPX拡張ジャンルを使用する
	char m_szTitle[30+1];	//タイトル
	char m_szArtist[30+1];	//アーティスト
	char m_szAlbum[30+1];	//アルバム
	char m_szYear[4+1];		//西暦
	unsigned char m_cGenre;	//ジャンル
	char m_szComment[30+1];	//コメント
	unsigned char m_cTrackNo;//トラック番号(1～255 0=N/A)

};

#endif // !defined(AFX_ID3TAGV1_H__81689DF7_1DE7_47C6_A34A_41AFEECC3184__INCLUDED_)
