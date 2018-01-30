// Tag_Ape.h: CTag_Ape クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Tag_Ape_H__6C5D4E96_2B7C_4333_A4F8_3CF81D10E6F5__INCLUDED_)
#define AFX_Tag_Ape_H__6C5D4E96_2B7C_4333_A4F8_3CF81D10E6F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4786)
#include <map>
#include <list>

/*
Memo:
ID3tag->Ape tagの順でロードする
セーブするとApe tagのみが書き込まれる
*/

class CTag_Ape  
{
public:
	CTag_Ape(BOOL bScmpxGenre = TRUE);
	virtual ~CTag_Ape();
	void	Release();
	BOOL	IsEnable(){return m_bHasApetag;};
//	BOOL	HasId3tag(){return m_bHasId3tag;};
//	void	SetId3tag(BOOL val){m_bHasId3tag = val;};
//	BOOL	HasApetag(){return m_bHasApetag;};
//	void	SetApetag(BOOL val){m_bHasApetag = val;};
	int		GetApeVersion(){return m_apeVersion;};
	void	SetDonotsaveId3v1(BOOL val){m_bDoNotSaveId3v1 = val;};
	BOOL	GetDonotsaveId3v1(){return m_bDoNotSaveId3v1;};
//	long	GenreString2Num(LPCTSTR szGenre);
//	CString GenreNum2String(unsigned char cGenre);
//	void	SetScmpxGenre(BOOL bSwitch);

	BOOL	SetComment(LPCTSTR name,LPCTSTR value);
	BOOL	GetComment(LPCTSTR name,CString &strValue);
//	void	GetCommentNames(CStringArray &strArray);

	DWORD	Load(LPCTSTR szFileName);
	DWORD	Save(LPCTSTR szFileName);
	DWORD	DelTag(LPCTSTR szFileName);
	DWORD	MakeTag(LPCTSTR szFileName);

	static const int CTag_Ape::CURRENT_APE_TAG_VERSION;
	static LPCTSTR CTag_Ape::APE_TAG_FIELD_TITLE;
	static LPCTSTR CTag_Ape::APE_TAG_FIELD_ARTIST;
	static LPCTSTR CTag_Ape::APE_TAG_FIELD_ALBUM;
	static LPCTSTR CTag_Ape::APE_TAG_FIELD_COMMENT;
	static LPCTSTR CTag_Ape::APE_TAG_FIELD_YEAR;
	static LPCTSTR CTag_Ape::APE_TAG_FIELD_TRACK;
	static LPCTSTR CTag_Ape::APE_TAG_FIELD_GENRE;

private:
//	void _GetId3tagString(char *szTag);
//	DWORD _MakeId3Tag(LPCTSTR szFileName);
//	DWORD _MakeApeTag(LPCTSTR szFileName);
	DWORD _LoadId3Tag(LPCTSTR szFileName);
	DWORD _LoadApeTagV1(HANDLE hFile);
	DWORD _LoadApeTagV2(HANDLE hFile);
	DWORD _SaveId3TagV1(LPCTSTR szFileName);
	DWORD _SaveApeTagV2(LPCTSTR szFileName);
//	DWORD _SaveId3V1(HANDLE hFile);
	DWORD _DelTag(LPCTSTR szFileName);

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
	typedef struct _APE_TAG_FOOTER
	{
		char id[8];		// "APETAGEX"
		int version;		// タグバージョン
		int size;			// このフッタを含むタグサイズ
		int fields;			// タグに含まれるフィールド数
		int flags;			// フラグ
		char reserved[8];	// 予約
	}APE_TAG_FOOTER;
#pragma pack()
	ID3_TAG m_id3tag;
	APE_TAG_FOOTER m_footer;
	BOOL m_bHasId3tag;
	BOOL m_bHasApetag;
	BOOL m_bScmpxGenre;		//SCMPX拡張ジャンルを使用する
	int m_apeVersion;
	BOOL m_bDoNotSaveId3v1;	// ID3v1を保存しない

	typedef std::map<CString,CString> CommentMap;
	CommentMap m_comments;

};

#endif // !defined(AFX_Tag_Ape_H__6C5D4E96_2B7C_4333_A4F8_3CF81D10E6F5__INCLUDED_)
