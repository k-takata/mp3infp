// Tag_Mp4.h: CTag_Mp4 クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TAG_MP4_H__3AA1F354_4166_4AB1_AD82_5003C432CAC7__INCLUDED_)
#define AFX_TAG_MP4_H__3AA1F354_4166_4AB1_AD82_5003C432CAC7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTag_Mp4  
{
public:
	CTag_Mp4();
	virtual ~CTag_Mp4();

	void Release();
	BOOL IsEnable(){return m_bEnable;};
	DWORD Load(LPCTSTR szFileName);
	DWORD Save(LPCTSTR szFileName);

	CString GetTrackInfoString(){return m_strTrackInfo;};
	CString GetAudioInfoString(){return m_strAudioInfo;};
	CString GetVideoInfoString(){return m_strVideoInfo;};

	CString GetMetadata_Name(){return m_strMetadata_Name;};
	CString GetMetadata_Artist(){return m_strMetadata_Artist;};
	CString GetMetadata_Album(){return m_strMetadata_Album;};
	CString GetMetadata_Genre(){return m_strMetadata_Genre;};
	CString GetMetadata_Group(){return m_strMetadata_Group;};
	CString GetMetadata_Composer(){return m_strMetadata_Composer;};
	int GetMetadata_Track1(){return m_iMetadata_Track1;};
	int GetMetadata_Track2(){return m_iMetadata_Track2;};
	int GetMetadata_Disc1(){return m_iMetadata_Disc1;};
	int GetMetadata_Disc2(){return m_iMetadata_Disc2;};
	int GetMetadata_Tempo(){return m_iMetadata_Tempo;};
	CString GetMetadata_Year(){return m_strMetadata_Year;};
	int GetMetadata_Compilation(){return m_iMetadata_Compilation;};
	CString GetMetadata_Comment(){return m_strMetadata_Comment;};
	CString GetMetadata_Tool(){return m_strMetadata_Tool;};

	void ClearMetadata();

	void SetMetadata_Name(LPCTSTR val){m_strMetadata_Name = val;};
	void SetMetadata_Artist(LPCTSTR val){m_strMetadata_Artist = val;};
	void SetMetadata_Album(LPCTSTR val){m_strMetadata_Album = val;};
	void SetMetadata_Group(LPCTSTR val){m_strMetadata_Group = val;};
	void SetMetadata_Composer(LPCTSTR val){m_strMetadata_Composer = val;};
	void SetMetadata_Genre(LPCTSTR val){m_strMetadata_Genre = val;};
	void SetMetadata_Track1(int val){m_iMetadata_Track1 = val;};
	void SetMetadata_Track2(int val){m_iMetadata_Track2 = val;};
	void SetMetadata_Disc1(int val){m_iMetadata_Disc1 = val;};
	void SetMetadata_Disc2(int val){m_iMetadata_Disc2 = val;};
	void SetMetadata_Tempo(int val){m_iMetadata_Tempo = val;};
	void SetMetadata_Year(LPCTSTR val){m_strMetadata_Year = val;};
	void SetMetadata_Compilation(int val){m_iMetadata_Compilation = val;};
	void SetMetadata_Comment(LPCTSTR val){m_strMetadata_Comment = val;};
	void SetMetadata_Tool(LPCTSTR val){m_strMetadata_Tool = val;};

private:
	BOOL m_bEnable;

	CString m_strTrackInfo;
	CString m_strAudioInfo;
	CString m_strVideoInfo;
	CString m_strMetadata_Name;
	CString m_strMetadata_Artist;
	CString m_strMetadata_Album;
	CString m_strMetadata_Group;
	CString m_strMetadata_Composer;
	CString m_strMetadata_Genre;
	int m_iMetadata_Track1;
	int m_iMetadata_Track2;
	int m_iMetadata_Disc1;
	int m_iMetadata_Disc2;
	int m_iMetadata_Tempo;
	CString m_strMetadata_Year;
	int m_iMetadata_Compilation;
	CString m_strMetadata_Comment;
	CString m_strMetadata_Tool;
};

#endif // !defined(AFX_TAG_MP4_H__3AA1F354_4166_4AB1_AD82_5003C432CAC7__INCLUDED_)
