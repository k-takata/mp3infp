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
#pragma pack(1)
	typedef struct _ID3_TAG
	{
		char TagHeader[3];
		char Title[30];
		char Artist[30];
		char Album[30];
		char Year[4];
		char Comment[29];
		unsigned char Track;
		unsigned char Genre;
	}ID3_TAG;
#pragma pack()
	CId3tagv1(BOOL bScmpxGenre = FALSE);
	virtual ~CId3tagv1();
	void SetScmpxGenre(BOOL bSwitch){m_bScmpxGenre = bSwitch;};
	BOOL IsEnable(){return m_bEnable;};

	CString GetTitle(){return m_szTitle;};
	void SetTitle(LPCTSTR title);
	CString GetArtist(){return m_szArtist;};
	void SetArtist(LPCTSTR artist);
	CString GetAlbum(){return m_szAlbum;};
	void SetAlbum(LPCTSTR album);
	CString GetYear(){return m_szYear;};
	void SetYear(LPCTSTR year);
	unsigned char GetGenreNum();
	CString GetGenre(){return GenreNum2String(m_cGenre);};
	void SetGenre(unsigned char cGenre){m_bEnable = TRUE; m_cGenre = cGenre;};
	void SetGenre(LPCTSTR szGenre){m_bEnable = TRUE; m_cGenre = GenreString2Num(szGenre);};
	unsigned char GetTrackNoNum(){return m_cTrackNo;};
	CString GetTrackNo();
	void SetTrackNo(unsigned char cTrackNo){m_bEnable = TRUE; m_cTrackNo = cTrackNo;};
	void SetTrackNo(LPCTSTR szTrackNo){m_bEnable = TRUE; m_cTrackNo = _ttoi(szTrackNo);};
	CString GetComment(){return m_szComment;};
	void SetComment(LPCTSTR comment);

	static CString GenreNum2String(unsigned char cGenre,BOOL bScmpxGenre);
	CString GenreNum2String(unsigned char cGenre) {
		return GenreNum2String(cGenre,m_bScmpxGenre);
	};
	long GenreString2Num(LPCTSTR szGenre);

	DWORD Load(LPCTSTR szFileName);
//	DWORD LoadMulti(LPCTSTR szFileName);
	DWORD Save(HWND hWnd,LPCTSTR szFileName);
	DWORD DelTag(HWND hWnd,LPCTSTR szFileName);
	static void GetId3tag(ID3_TAG *tag,BOOL bScmpxGenre);
	void GetId3tag(ID3_TAG *tag){GetId3tag(tag,m_bScmpxGenre);};
	DWORD MakeTag(HWND hWnd,LPCTSTR szFileName);
	static BOOL IsTagValid(const ID3_TAG *tag) {
		return memcmp(tag->TagHeader,"TAG",3) == 0;
	};

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
