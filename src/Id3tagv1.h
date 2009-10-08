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
	void SetTitle(const char *title);
	CString GetArtist();
	void SetArtist(const char *artist);
	CString GetAlbum();
	void SetAlbum(const char *album);
	CString GetYear();
	void SetYear(const char *year);
	unsigned char GetGenreNum();
	CString GetGenre();
	void SetGenre(unsigned char cGenre);
	void SetGenre(const char *szGenre);
	unsigned char GetTrackNoNum();
	CString GetTrackNo();
	void SetTrackNo(unsigned char cTrackNo);
	void SetTrackNo(const char *szTrackNo);
	CString GetComment();
	void SetComment(const char *comment);

	CString GenreNum2String(unsigned char cGenre);
	long GenreString2Num(const char *szGenre);

	DWORD Load(const char *szFileName);
	DWORD LoadMulti(const char *szFileName);
	DWORD Save(HWND hWnd,const char *szFileName);
	DWORD DelTag(HWND hWnd,const char *szFileName);
	void GetId3tagString(char *szTag);
	DWORD MakeTag(HWND hWnd,const char *szFileName);

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
