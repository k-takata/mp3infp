// Id3tagv1.cpp: CId3tagv1 クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GlobalCommand.h"
#include "Id3tagv1.h"
#include <io.h>

static const unsigned char SCMPX_GENRE_NULL = 247;
static const unsigned char WINAMP_GENRE_NULL = 255;
static const char	szId3gnr[256][30]={
	"Blues","Classic Rock","Country","Dance","Disco","Funk","Grunge",
	"Hip-Hop","Jazz","Metal","New Age","Oldies","Other","Pop","R&B","Rap",
	"Reggae","Rock","Techno","Industrial","Alternative","Ska","Death Metal",
	"Pranks","Soundtrack","Euro-Techno","Ambient","Trip-Hop","Vocal",
	"Jazz+Funk","Fusion","Trance","Classical","Instrumental","Acid","House",
	"Game","Sound Clip","Gospel","Noise","Alt. Rock","Bass","Soul","Punk",
	"Space","Meditative","Instrumental Pop","Instrumental Rock","Ethnic",
	"Gothic","Darkwave","Techno-Industrial","Electronic","Pop-Folk",
	"Eurodance","Dream","Southern Rock","Comedy","Cult","Gangsta Rap",
	"Top 40","Christian Rap","Pop/Funk","Jungle","Native American","Cabaret",
	"New Wave","Psychedelic","Rave","Showtunes","Trailer","Lo-Fi","Tribal",
	"Acid Punk","Acid Jazz","Polka","Retro","Musical","Rock & Roll",
	"Hard Rock","Folk","Folk/Rock","National Folk","Swing","Fast-Fusion",
	"Bebob","Latin","Revival","Celtic","Bluegrass","Avantgarde","Gothic Rock",
	"Progressive Rock","Psychedelic Rock","Symphonic Rock","Slow Rock",
	"Big Band","Chorus","Easy Listening","Acoustic","Humour","Speech",
	"Chanson","Opera","Chamber Music","Sonata","Symphony","Booty Bass",
	"Primus","Porn Groove","Satire","Slow Jam","Club","Tango","Samba",
	"Folklore","Ballad","Power Ballad","Rhythmic Soul","Freestyle","Duet",
	"Punk Rock","Drum Solo","A Cappella","Euro-House","Dance Hall","Goa",
	"Drum & Bass","Club-House","Hardcore","Terror","Indie","BritPop",
	"Negerpunk","Polsk Punk","Beat","Christian Gangsta Rap","Heavy Metal",
	"Black Metal","Crossover","Contemporary Christian","Christian Rock",
	"Merengue","Salsa","Thrash Metal","Anime","JPop","Synthpop",
	"","","","","","","","","","",
	"","","","","","","","","","",
	"","","","","","","","","","",
	"","","","","","","","","","",
	"","","","","","","","","","",
	"","","","","","","","","","",
	"","","","","","","","","","",
	"","","","","","","","","","",
	"","","","","","","","","","",
	"","","","","","","","","","",
	"Heavy Rock(J)","Doom Rock(J)","J-POP(J)","Seiyu(J)","Tecno Ambient(J)","Moemoe(J)","Tokusatsu(J)","Anime(J)"
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

void CId3tagv1::SetTitle(const char *title)
{
	m_bEnable = TRUE;
	long length = check2ByteLength(title,30);
	strncpy(m_szTitle,title,length);
	m_szTitle[length] = '\0';
}

CString CId3tagv1::GetArtist()
{
	return m_szArtist;
}

void CId3tagv1::SetArtist(const char *artist)
{
	m_bEnable = TRUE;
	long length = check2ByteLength(artist,30);
	strncpy(m_szArtist,artist,length);
	m_szArtist[length] = '\0';
}

CString CId3tagv1::GetAlbum()
{
	return m_szAlbum;
}

void CId3tagv1::SetAlbum(const char *album)
{
	m_bEnable = TRUE;
	long length = check2ByteLength(album,30);
	strncpy(m_szAlbum,album,length);
	m_szAlbum[length] = '\0';
}

CString CId3tagv1::GetYear()
{
	return m_szYear;
}

void CId3tagv1::SetYear(const char *year)
{
	m_bEnable = TRUE;
	long length = check2ByteLength(year,4);
	strncpy(m_szYear,year,length);
	m_szYear[length] = '\0';
}

unsigned char CId3tagv1::GetGenreNum()
{
	if(!m_bScmpxGenre && (m_cGenre >= SCMPX_GENRE_NULL))
		return WINAMP_GENRE_NULL;
	return m_cGenre;
}

CString CId3tagv1::GetGenre()
{
	if(!m_bScmpxGenre && (m_cGenre >= SCMPX_GENRE_NULL))
		return "";
	return szId3gnr[m_cGenre];
}

void CId3tagv1::SetGenre(unsigned char cGenre)
{
	m_bEnable = TRUE;
	m_cGenre = cGenre;
}

void CId3tagv1::SetGenre(const char *szGenre)
{
	m_bEnable = TRUE;
	for(int i=0; i<256; i++)
	{
		if(strcmp(szId3gnr[i],szGenre) == 0)
		{
			m_cGenre = i;
			break;
		}
	}
}

unsigned char CId3tagv1::GetTrackNoNum()
{
	return m_cTrackNo;
}

CString CId3tagv1::GetTrackNo()
{
	CString str;
	if(m_cTrackNo)
		str.Format("%d",m_cTrackNo);
	return str;
}

void CId3tagv1::SetTrackNo(unsigned char cTrackNo)
{
	m_cTrackNo = cTrackNo;
}

void CId3tagv1::SetTrackNo(const char *szTrackNo)
{
	if(strlen(szTrackNo) == 0)
		m_cTrackNo = 0;
	else
		m_cTrackNo = atoi(szTrackNo);
}

CString CId3tagv1::GetComment()
{
	return m_szComment;
}

void CId3tagv1::SetComment(const char *comment)
{
	m_bEnable = TRUE;
	int len=30;
	if(m_cTrackNo)
		len = 28;
	long length = check2ByteLength(comment,len);
	strncpy(m_szComment,comment,length);
	m_szComment[length] = '\0';
}

CString CId3tagv1::GenreNum2String(unsigned char cGenre)
{
	if(!m_bScmpxGenre && (cGenre >= SCMPX_GENRE_NULL))
		return "";
	return szId3gnr[cGenre];
}

long CId3tagv1::GenreString2Num(const char *szGenre)
{
	long cGenre = -1;
	if(strlen(szGenre) == 0)
	{
		if(m_bScmpxGenre)
			return SCMPX_GENRE_NULL;
		else
			return WINAMP_GENRE_NULL;
	}
	for(long i=0; i<256; i++)
	{
		if(strcmp(szId3gnr[i],szGenre) == 0)
		{
			cGenre = i;
			break;
		}
	}
	return cGenre;
}

DWORD CId3tagv1::Load(const char *szFileName)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;
	Release();
	FILE *fp;
	if((fp=fopen(szFileName,"rb")) == NULL)
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

DWORD CId3tagv1::LoadMulti(const char *szFileName)
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
			this->SetTitle("");
		}
		if(!this->GetArtist().Compare(tag.GetArtist()))
		{
			this->SetArtist("");
		}
		if(!this->GetAlbum().Compare(tag.GetAlbum()))
		{
			this->SetAlbum("");
		}
		if(!this->GetYear().Compare(tag.GetYear()))
		{
			this->SetYear("");
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
			this->SetComment("");
		}
		if(!this->GetTrackNo().Compare(tag.GetTrackNo()))
		{
			this->SetTrackNo("");
		}
	}
	else
	{
		this->SetTitle("");
		this->SetArtist("");
		this->SetAlbum("");
		this->SetYear("");
		if(m_bScmpxGenre)
			this->SetGenre(SCMPX_GENRE_NULL);
		else
			this->SetGenre(WINAMP_GENRE_NULL);
		this->SetComment("");
		this->SetTrackNo("");
	}

	return ERROR_SUCCESS;
}

DWORD CId3tagv1::Save(HWND hWnd,const char *szFileName)
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

	if((fp = fopen(szFileName,"r+b")) == NULL)
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

DWORD CId3tagv1::DelTag(HWND hWnd,const char *szFileName)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;
	FILE	*fp;
	char	szTag[4];

	//開く
	if((fp=fopen(szFileName,"r+b")) == NULL)
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

DWORD CId3tagv1::MakeTag(HWND hWnd,const char *szFileName)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;
	HANDLE	hFile;
	DWORD	dwWritten = 0;
	char	szTag[128];
	char	szDefaultName[MAX_PATH];
	
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

	//ファイルの終端までまでSEEK
	if(SetFilePointer(hFile,0,NULL,FILE_END) == INVALID_SET_FILE_POINTER)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		return dwWin32errorCode;
	}
	//ID3タグを作成
	GetId3tagString(szTag);
	strcpy(szDefaultName,getFileName(CString(szFileName)));
	strncpy(szTag+3,szDefaultName,30);
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

