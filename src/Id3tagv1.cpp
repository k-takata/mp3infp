// Id3tagv1.cpp: CId3tagv1 クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GlobalCommand.h"
#include "Id3tagv1.h"
#include <io.h>

static const unsigned char SCMPX_GENRE_NULL = 247;
static const unsigned char WINAMP_GENRE_NULL = 255;
static const LPCTSTR szId3gnr[256]={
	_T("Blues"),_T("Classic Rock"),_T("Country"),_T("Dance"),_T("Disco"),_T("Funk"),_T("Grunge"),
	_T("Hip-Hop"),_T("Jazz"),_T("Metal"),_T("New Age"),_T("Oldies"),_T("Other"),_T("Pop"),_T("R&B"),_T("Rap"),
	_T("Reggae"),_T("Rock"),_T("Techno"),_T("Industrial"),_T("Alternative"),_T("Ska"),_T("Death Metal"),
	_T("Pranks"),_T("Soundtrack"),_T("Euro-Techno"),_T("Ambient"),_T("Trip-Hop"),_T("Vocal"),
	_T("Jazz+Funk"),_T("Fusion"),_T("Trance"),_T("Classical"),_T("Instrumental"),_T("Acid"),_T("House"),
	_T("Game"),_T("Sound Clip"),_T("Gospel"),_T("Noise"),_T("Alt. Rock"),_T("Bass"),_T("Soul"),_T("Punk"),
	_T("Space"),_T("Meditative"),_T("Instrumental Pop"),_T("Instrumental Rock"),_T("Ethnic"),
	_T("Gothic"),_T("Darkwave"),_T("Techno-Industrial"),_T("Electronic"),_T("Pop-Folk"),
	_T("Eurodance"),_T("Dream"),_T("Southern Rock"),_T("Comedy"),_T("Cult"),_T("Gangsta Rap"),
	_T("Top 40"),_T("Christian Rap"),_T("Pop/Funk"),_T("Jungle"),_T("Native American"),_T("Cabaret"),
	_T("New Wave"),_T("Psychedelic"),_T("Rave"),_T("Showtunes"),_T("Trailer"),_T("Lo-Fi"),_T("Tribal"),
	_T("Acid Punk"),_T("Acid Jazz"),_T("Polka"),_T("Retro"),_T("Musical"),_T("Rock & Roll"),
	_T("Hard Rock"),_T("Folk"),_T("Folk/Rock"),_T("National Folk"),_T("Swing"),_T("Fast-Fusion"),
	_T("Bebob"),_T("Latin"),_T("Revival"),_T("Celtic"),_T("Bluegrass"),_T("Avantgarde"),_T("Gothic Rock"),
	_T("Progressive Rock"),_T("Psychedelic Rock"),_T("Symphonic Rock"),_T("Slow Rock"),
	_T("Big Band"),_T("Chorus"),_T("Easy Listening"),_T("Acoustic"),_T("Humour"),_T("Speech"),
	_T("Chanson"),_T("Opera"),_T("Chamber Music"),_T("Sonata"),_T("Symphony"),_T("Booty Bass"),
	_T("Primus"),_T("Porn Groove"),_T("Satire"),_T("Slow Jam"),_T("Club"),_T("Tango"),_T("Samba"),
	_T("Folklore"),_T("Ballad"),_T("Power Ballad"),_T("Rhythmic Soul"),_T("Freestyle"),_T("Duet"),
	_T("Punk Rock"),_T("Drum Solo"),_T("A Cappella"),_T("Euro-House"),_T("Dance Hall"),_T("Goa"),
	_T("Drum & Bass"),_T("Club-House"),_T("Hardcore"),_T("Terror"),_T("Indie"),_T("BritPop"),
	_T("Negerpunk"),_T("Polsk Punk"),_T("Beat"),_T("Christian Gangsta Rap"),_T("Heavy Metal"),
	_T("Black Metal"),_T("Crossover"),_T("Contemporary Christian"),_T("Christian Rock"),
	_T("Merengue"),_T("Salsa"),_T("Thrash Metal"),_T("Anime"),_T("JPop"),_T("Synthpop"),
	_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),
	_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),
	_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),
	_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),
	_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),
	_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),
	_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),
	_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),
	_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),
	_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),
	_T("Heavy Rock(J)"),_T("Doom Rock(J)"),_T("J-POP(J)"),_T("Seiyu(J)"),_T("Tecno Ambient(J)"),_T("Moemoe(J)"),_T("Tokusatsu(J)"),_T("Anime(J)")
	};

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CId3tagv1::CId3tagv1(BOOL bScmpxGenre)
{
	m_bScmpxGenre = bScmpxGenre;
	Release();
}

CId3tagv1::~CId3tagv1()
{

}

void CId3tagv1::Release()
{
	m_bEnable = FALSE;
	strcpy(m_szTitle,"");
	strcpy(m_szArtist,"");
	strcpy(m_szAlbum,"");
	strcpy(m_szYear,"");
	if(m_bScmpxGenre)
		m_cGenre = SCMPX_GENRE_NULL;
	else
		m_cGenre = WINAMP_GENRE_NULL;
	strcpy(m_szComment,"");
	m_cTrackNo = 0;
}

void CId3tagv1::SetScmpxGenre(BOOL bSwitch)
{
	m_bScmpxGenre = bSwitch;
}

CString CId3tagv1::GetTitle()
{
	return m_szTitle;
}

void CId3tagv1::SetTitle(LPCTSTR title)
{
#ifdef UNICODE
	char *buf = TstrToDataAlloc(title, -1, NULL, DTC_CODE_ANSI);
#else
	char *buf = title;
#endif
	m_bEnable = TRUE;
	long length = check2ByteLength(buf,30);
	strncpy(m_szTitle,buf,length);
	m_szTitle[length] = '\0';
#ifdef UNICODE
	free(buf);
#endif
}

CString CId3tagv1::GetArtist()
{
	return m_szArtist;
}

void CId3tagv1::SetArtist(LPCTSTR artist)
{
#ifdef UNICODE
	char *buf = TstrToDataAlloc(artist, -1, NULL, DTC_CODE_ANSI);
#else
	char *buf = artist;
#endif
	m_bEnable = TRUE;
	long length = check2ByteLength(buf,30);
	strncpy(m_szArtist,buf,length);
	m_szArtist[length] = '\0';
#ifdef UNICODE
	free(buf);
#endif
}

CString CId3tagv1::GetAlbum()
{
	return m_szAlbum;
}

void CId3tagv1::SetAlbum(LPCTSTR album)
{
#ifdef UNICODE
	char *buf = TstrToDataAlloc(album, -1, NULL, DTC_CODE_ANSI);
#else
	char *buf = album;
#endif
	m_bEnable = TRUE;
	long length = check2ByteLength(buf,30);
	strncpy(m_szAlbum,buf,length);
	m_szAlbum[length] = '\0';
#ifdef UNICODE
	free(buf);
#endif
}

CString CId3tagv1::GetYear()
{
	return m_szYear;
}

void CId3tagv1::SetYear(LPCTSTR year)
{
#ifdef UNICODE
	char *buf = TstrToDataAlloc(year, -1, NULL, DTC_CODE_ANSI);
#else
	char *buf = year;
#endif
	m_bEnable = TRUE;
	long length = check2ByteLength(buf,4);
	strncpy(m_szYear,buf,length);
	m_szYear[length] = '\0';
#ifdef UNICODE
	free(buf);
#endif
}

unsigned char CId3tagv1::GetGenreNum()
{
	if(!m_bScmpxGenre && (m_cGenre >= SCMPX_GENRE_NULL))
		return WINAMP_GENRE_NULL;
	return m_cGenre;
}

CString CId3tagv1::GetGenre()
{
	return GenreNum2String(m_cGenre);
}

void CId3tagv1::SetGenre(unsigned char cGenre)
{
	m_bEnable = TRUE;
	m_cGenre = cGenre;
}

void CId3tagv1::SetGenre(LPCTSTR szGenre)
{
	m_bEnable = TRUE;
	m_cGenre = GenreString2Num(szGenre);
}

unsigned char CId3tagv1::GetTrackNoNum()
{
	return m_cTrackNo;
}

CString CId3tagv1::GetTrackNo()
{
	CString str;
	if(m_cTrackNo)
		str.Format(_T("%d"),m_cTrackNo);
	return str;
}

void CId3tagv1::SetTrackNo(unsigned char cTrackNo)
{
	m_cTrackNo = cTrackNo;
}

void CId3tagv1::SetTrackNo(LPCTSTR szTrackNo)
{
	if(lstrlen(szTrackNo) == 0)
		m_cTrackNo = 0;
	else
		m_cTrackNo = _ttoi(szTrackNo);
}

CString CId3tagv1::GetComment()
{
	return m_szComment;
}

void CId3tagv1::SetComment(LPCTSTR comment)
{
#ifdef UNICODE
	char *buf = TstrToDataAlloc(comment, -1, NULL, DTC_CODE_ANSI);
#else
	char *buf = comment;
#endif
	m_bEnable = TRUE;
	int len=30;
	if(m_cTrackNo)
		len = 28;
	long length = check2ByteLength(buf,len);
	strncpy(m_szComment,buf,length);
	m_szComment[length] = '\0';
#ifdef UNICODE
	free(buf);
#endif
}

CString CId3tagv1::GenreNum2String(unsigned char cGenre)
{
	if(!m_bScmpxGenre && (cGenre >= SCMPX_GENRE_NULL))
		return _T("");
	return szId3gnr[cGenre];
}

long CId3tagv1::GenreString2Num(LPCTSTR szGenre)
{
	long cGenre = -1;
	if(lstrlen(szGenre) == 0)
	{
		if(m_bScmpxGenre)
			return SCMPX_GENRE_NULL;
		else
			return WINAMP_GENRE_NULL;
	}
	for(long i=0; i<256; i++)
	{
		if(lstrcmp(szId3gnr[i],szGenre) == 0)
		{
			cGenre = i;
			break;
		}
	}
	return cGenre;
}

DWORD CId3tagv1::Load(LPCTSTR szFileName)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;
	Release();
	FILE *fp;
	if((fp=_tfopen(szFileName,_T("rb"))) == NULL)
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}

	//ID3タグがあるはずの位置までSEEK
	if(fseek(fp,-128,SEEK_END) != 0)
	{
		dwWin32errorCode = GetLastError();
		fclose(fp);
		return dwWin32errorCode;
	}

	//ID3タグかどうかをチェック
	char szTmp[128];
	fread(szTmp,1,128,fp);
	fclose(fp);
	if(strncmp("TAG",szTmp,3) != 0)
	{
		return -1;
	}
	
	m_bEnable = TRUE;
	//情報の採取
	char *p;
	int i;
	p = szTmp+3;
	mbsncpy2((unsigned char *)m_szTitle,(unsigned char *)p,30);
	m_szTitle[30] = '\0';
	for(i=29; i>=0; i--)
	{
		if(m_szTitle[i] == ' ')
			m_szTitle[i] = '\0';
		else
			break;
	}
	p += 30;
	mbsncpy2((unsigned char *)m_szArtist,(unsigned char *)p,30);
	m_szArtist[30] = '\0';
	for(i=29; i>=0; i--)
	{
		if(m_szArtist[i] == ' ')
			m_szArtist[i] = '\0';
		else
			break;
	}
	p+=30;
	mbsncpy2((unsigned char *)m_szAlbum,(unsigned char *)p,30);
	m_szAlbum[30] = '\0';
	for(i=29; i>=0; i--)
	{
		if(m_szAlbum[i] == ' ')
			m_szAlbum[i] = '\0';
		else
			break;
	}
	p+=30;
	mbsncpy2((unsigned char *)m_szYear,(unsigned char *)p,4);
	m_szYear[4] = '\0';
	for(i=3; i>=0; i--)
	{
		if(m_szYear[i] == ' ')
			m_szYear[i] = '\0';
		else
			break;
	}
	p+=4;
	if((szTmp[125] == '\0') && szTmp[126])
	{
		m_cTrackNo = szTmp[126];
		mbsncpy2((unsigned char *)m_szComment,(unsigned char *)p,28);
		m_szComment[28] = '\0';
		for(i=27; i>=0; i--)
		{
			if(m_szComment[i] == ' ')
				m_szComment[i] = '\0';
			else
				break;
		}
	}
	else
	{
		m_cTrackNo = 0;
		mbsncpy2((unsigned char *)m_szComment,(unsigned char *)p,30);
		m_szComment[30] = '\0';
		for(i=29; i>=0; i--)
		{
			if(m_szComment[i] == ' ')
				m_szComment[i] = '\0';
			else
				break;
		}
	}
	p+=30;
	m_cGenre = *p;
	return dwWin32errorCode;
}

DWORD CId3tagv1::LoadMulti(LPCTSTR szFileName)
{
	DWORD dwError;
	CId3tagv1 tag;
	tag.SetScmpxGenre(m_bScmpxGenre);
	dwError = tag.Load(szFileName);
	if(dwError != ERROR_SUCCESS)
	{
		return dwError;
	}

	if(tag.IsEnable())
	{
		if(!this->GetTitle().Compare(tag.GetTitle()))
		{
			this->SetTitle(_T(""));
		}
		if(!this->GetArtist().Compare(tag.GetArtist()))
		{
			this->SetArtist(_T(""));
		}
		if(!this->GetAlbum().Compare(tag.GetAlbum()))
		{
			this->SetAlbum(_T(""));
		}
		if(!this->GetYear().Compare(tag.GetYear()))
		{
			this->SetYear(_T(""));
		}
		if(!(this->GetGenreNum() == tag.GetGenreNum()))
		{
			if(m_bScmpxGenre)
				this->SetGenre(SCMPX_GENRE_NULL);
			else
				this->SetGenre(WINAMP_GENRE_NULL);
		}
		if(!this->GetComment().Compare(tag.GetComment()))
		{
			this->SetComment(_T(""));
		}
		if(!this->GetTrackNo().Compare(tag.GetTrackNo()))
		{
			this->SetTrackNo(_T(""));
		}
	}
	else
	{
		this->SetTitle(_T(""));
		this->SetArtist(_T(""));
		this->SetAlbum(_T(""));
		this->SetYear(_T(""));
		if(m_bScmpxGenre)
			this->SetGenre(SCMPX_GENRE_NULL);
		else
			this->SetGenre(WINAMP_GENRE_NULL);
		this->SetComment(_T(""));
		this->SetTrackNo(_T(""));
	}

	return ERROR_SUCCESS;
}

DWORD CId3tagv1::Save(HWND hWnd,LPCTSTR szFileName)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;
	FILE	*fp;
	char	szTmp[128];
	char	szTagTmp[4];
	char	*p;

	//情報の保存
	p = szTmp;
	memset(p,0x00,128-3);
	strncpy(p,m_szTitle,strlen(m_szTitle));
	p += 30;
	strncpy(p,m_szArtist,strlen(m_szArtist));
	p += 30;
	strncpy(p,m_szAlbum,strlen(m_szAlbum));
	p += 30;
	strncpy(p,m_szYear,strlen(m_szYear));
	p += 4;
	strncpy(p,m_szComment,strlen(m_szComment));
	p += 28;
	if(m_cTrackNo)
	{
		*p = '\0';
		*(p+1) = m_cTrackNo;
	}
	p += 2;
	*p = m_cGenre;

	if((fp = _tfopen(szFileName,_T("r+b"))) == NULL)
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}
	//ID3タグがあるはずの位置までSEEK
	if(fseek(fp,-128,SEEK_END) != 0)
	{
		dwWin32errorCode = GetLastError();
		fclose(fp);
		return dwWin32errorCode;
	}
	//ID3タグかどうかをチェック
	if(fread(szTagTmp,1,3,fp) < 3)
	{
//		dwWin32errorCode = GetLastError();//?
		fclose(fp);
		return -1;
	}
	if(strncmp("TAG",szTagTmp,3) != 0)
	{
		//ID3TAGが見つからない
		if(fseek(fp,0,SEEK_END))
		{
			dwWin32errorCode = GetLastError();
			fclose(fp);
			return dwWin32errorCode;
		}
		fwrite("TAG",1,3,fp);
	}
	if(fseek(fp,0,SEEK_CUR))
	{
		dwWin32errorCode = GetLastError();
		fclose(fp);
		return dwWin32errorCode;
	}
	if(fwrite(szTmp,1,128-3,fp) < 128-3)
	{
		dwWin32errorCode = GetLastError();
		fclose(fp);
		return dwWin32errorCode;
	}
	if(fclose(fp) == EOF)	//ライトプロテクトはここで検出
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}

	return dwWin32errorCode;
}

DWORD CId3tagv1::DelTag(HWND hWnd,LPCTSTR szFileName)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;
	FILE	*fp;
	char	szTag[4];

	//開く
	if((fp=_tfopen(szFileName,_T("r+b"))) == NULL)
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}
	//ID3タグがあるはずの位置までSEEK
	if(fseek(fp,-128,SEEK_END) != 0)
	{
		dwWin32errorCode = GetLastError();
		fclose(fp);
		return dwWin32errorCode;
	}
	//ID3タグかどうかをチェック
	if(fread(szTag,1,3,fp) < 3)
	{
//		dwWin32errorCode = GetLastError();
		fclose(fp);
		return -1;
	}
	if(strncmp("TAG",szTag,3) != 0)
	{
		fclose(fp);
		//ID3タグが無いならそれもOK
		return -1;
	}
	if(fseek(fp,-3,SEEK_CUR))
	{
		dwWin32errorCode = GetLastError();
		fclose(fp);
		return dwWin32errorCode;
	}
	//切りつめ
	if(_chsize(fileno(fp),ftell(fp)) == -1)
	{
		dwWin32errorCode = GetLastError();
		fclose(fp);
		return dwWin32errorCode;
	}

	if(fclose(fp) == EOF)	//ライトプロテクトはここで検出
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}

	Release();

	return dwWin32errorCode;
}

void CId3tagv1::GetId3tagString(char *szTag)
{
	memset(szTag,0x00,128);
	strncpy(szTag,"TAG",3);
	if(m_bScmpxGenre)
		szTag[127] = (char )SCMPX_GENRE_NULL;
	else
		szTag[127] = (char )WINAMP_GENRE_NULL;
}

DWORD CId3tagv1::MakeTag(HWND hWnd,LPCTSTR szFileName)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;
	HANDLE	hFile;
	DWORD	dwWritten = 0;
	char	szTag[128];
	TCHAR	szDefaultName[MAX_PATH];
	
	hFile = CreateFile(
				szFileName,
				GENERIC_WRITE|GENERIC_READ,
				FILE_SHARE_READ,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}

	//ファイルの終端までSEEK
	if(SetFilePointer(hFile,0,NULL,FILE_END) == INVALID_SET_FILE_POINTER)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		return dwWin32errorCode;
	}
	//ID3タグを作成
	GetId3tagString(szTag);
	lstrcpy(szDefaultName,getFileName(CString(szFileName)));
	char *buf = TstrToDataAlloc(szDefaultName, -1, NULL, DTC_CODE_ANSI);
	if (buf != NULL) {
		strncpy(szTag+3,buf,30);
		free(buf);
	}
	if(WriteFile(hFile,szTag,128,&dwWritten,NULL) == 0)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		return dwWin32errorCode;
	}
	CloseHandle(hFile);

	Release();
	SetTitle(szDefaultName);
	
	return dwWin32errorCode;
}

