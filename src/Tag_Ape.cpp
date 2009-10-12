// Tag_Ape.cpp: CTag_Ape クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <io.h>
//#include <fcntl.h>
#include "GlobalCommand.h"

#include "Tag_Ape.h"

#define APE_FLAG_TAG_HAS_HEADER			0x80000000
#define APE_FLAG_TAG_NOT_HAS_FOOTER		0x40000000
#define APE_FLAG_THIS_IS_THE_HEADER		0x20000000

#define APE_FLAG_TEXTINFO_MASK			0x00000006
#define APE_FLAG_TEXTINFO_UTF8			0x00000000

const int CTag_Ape::CURRENT_APE_TAG_VERSION		= 2000;
LPCTSTR CTag_Ape::APE_TAG_FIELD_TITLE		= _T("Title");
LPCTSTR CTag_Ape::APE_TAG_FIELD_ARTIST		= _T("Artist");
LPCTSTR CTag_Ape::APE_TAG_FIELD_ALBUM		= _T("Album");
LPCTSTR CTag_Ape::APE_TAG_FIELD_COMMENT		= _T("Comment");
LPCTSTR CTag_Ape::APE_TAG_FIELD_YEAR		= _T("Year");
LPCTSTR CTag_Ape::APE_TAG_FIELD_TRACK		= _T("Track");
LPCTSTR CTag_Ape::APE_TAG_FIELD_GENRE		= _T("Genre");

static const unsigned char SCMPX_GENRE_NULL = 247;
static const unsigned char WINAMP_GENRE_NULL = 255;
static const TCHAR	szId3gnr[256][30]={
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

CTag_Ape::CTag_Ape(BOOL bScmpxGenre)
{
	m_bDoNotSaveId3v1 = FALSE;
	m_bScmpxGenre = bScmpxGenre;
	Release();
}

CTag_Ape::~CTag_Ape()
{

}

void CTag_Ape::Release()
{
	m_bHasApetag = FALSE;
	m_bHasId3tag = FALSE;

	memset(&m_id3tag,0,sizeof(m_id3tag));
	memcpy(m_id3tag.TagHeader,"TAG",3);
	m_comments.clear();
}

BOOL CTag_Ape::SetComment(LPCTSTR name,LPCTSTR value)
{
	m_bHasApetag = TRUE;
	
	// 現在のコメントを削除
	map<CString,CString>::iterator it = m_comments.begin();
	while(it != m_comments.end())
	{
		// 大文字小文字を区別せずに比較
		if(it->first.CompareNoCase(name) == 0)
		{
			m_comments.erase(it->first);
			it = m_comments.begin();
			continue;
		}
		it++;
	}

	if(lstrlen(value) == 0)
	{
		return TRUE;	// 空はセットしない
	}
	m_comments.insert(pair<CString,CString>(name,CString(value)));
	
	return TRUE;
}

BOOL CTag_Ape::GetComment(LPCTSTR name,CString &strValue)
{
	strValue = _T("");

	map<CString,CString>::iterator it = m_comments.begin();
	while(it != m_comments.end())
	{
		// 大文字小文字を区別せずに比較
		if(it->first.CompareNoCase(name) == 0)
		{
			strValue = it->second;
			return TRUE;
		}
		it++;
	}
	
	return FALSE;
}

/*void CTag_Ape::GetCommentNames(CStringArray &strArray)
{
	//nameリストを返す
	map<CString,CString>::iterator it = m_comments.begin();
	
	while(it != m_comments.end())
	{
		strArray.Add(it->first);
		it++;
	}
}*/

DWORD CTag_Ape::_LoadId3Tag(LPCTSTR szFileName)
{
	DWORD dwRet;
	DWORD dwWin32errorCode = ERROR_SUCCESS;
	
	m_bHasId3tag = FALSE;
	//ファイルをオープン
	HANDLE hFile = CreateFile(
				szFileName,
				GENERIC_READ,
				FILE_SHARE_READ,
				NULL,
				OPEN_EXISTING,	//ファイルをオープンします。指定ファイルが存在していない場合、関数は失敗します。
				FILE_ATTRIBUTE_NORMAL,
				NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}
	
	// ID3TAGを探す
	SetFilePointer(hFile,-sizeof(ID3_TAG),NULL,FILE_END);
	if(!ReadFile(hFile,&m_id3tag,sizeof(m_id3tag),&dwRet,NULL) || (dwRet != sizeof(m_id3tag)))
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		return dwWin32errorCode;
	}

	// ID3TAGの正当性をチェック
	if((m_id3tag.TagHeader[0] != 'T') ||
		(m_id3tag.TagHeader[1] != 'A') ||
		(m_id3tag.TagHeader[2] != 'G'))
	{
		CloseHandle(hFile);
		return ERROR_SUCCESS;
	}
	
	m_bHasId3tag = TRUE;
	CloseHandle(hFile);
	return ERROR_SUCCESS;
}

DWORD CTag_Ape::_LoadApeTagV1(HANDLE hFile)
{
	DWORD dwRet;
	DWORD dwWin32errorCode = ERROR_SUCCESS;

	// ID3TAGを探す
	SetFilePointer(hFile,-sizeof(ID3_TAG),NULL,FILE_END);
	if(!ReadFile(hFile,&m_id3tag,sizeof(m_id3tag),&dwRet,NULL) || (dwRet != sizeof(m_id3tag)))
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		return dwWin32errorCode;
	}

	BOOL bId3v1 = FALSE;
	// ID3TAGの正当性をチェック
	if((m_id3tag.TagHeader[0] == 'T') &&
		(m_id3tag.TagHeader[1] == 'A') &&
		(m_id3tag.TagHeader[2] == 'G'))
	{
		bId3v1 = TRUE;
	}
	
	int rawFieldBytes = m_footer.size - sizeof(APE_TAG_FOOTER);
	char *pRawTag = (char *)malloc(rawFieldBytes);
	// ヘッダの先頭に移動
	SetFilePointer(hFile,-m_footer.size-(bId3v1?128:0),NULL,FILE_END);
	// ヘッダをバッファに取り込む
	if(!ReadFile(hFile,pRawTag,rawFieldBytes,&dwRet,NULL) || (dwRet != rawFieldBytes))
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		return dwWin32errorCode;
	}
	// 各フィールドの取り込み
	DWORD rawTagOffset = 0;
	for(int i=0; i<m_footer.fields; i++)
	{
		if(rawFieldBytes < 4+4+1)
		{
			// バッファオーバーフロー
			free(pRawTag);
			CloseHandle(hFile);
			return -1;
		}
		// フィールドサイズ
		DWORD fieldValueSize = *(DWORD *)&pRawTag[rawTagOffset];
		if(fieldValueSize > rawFieldBytes)
		{
			free(pRawTag);
			CloseHandle(hFile);
			return -1;
		}
		rawTagOffset += 4;
		// フィールドフラグ
		int fieldFlags = *(int *)&pRawTag[rawTagOffset];
		rawTagOffset += 4;
		// フィールド名
		char nameBuffer[256];
		strncpy(nameBuffer,&pRawTag[rawTagOffset],min(sizeof(nameBuffer)-1,rawFieldBytes-rawTagOffset));
		nameBuffer[255] = '\0';
		rawTagOffset += strlen(nameBuffer) + 1;
		// 値
		if(rawFieldBytes < rawTagOffset)
		{
			// バッファオーバーフロー
			free(pRawTag);
			CloseHandle(hFile);
			return -1;
		}
		char *pFieldBuffer = (char *)malloc(fieldValueSize+1);
		memcpy(pFieldBuffer,&pRawTag[rawTagOffset],fieldValueSize);
		pFieldBuffer[fieldValueSize] = '\0';
		rawTagOffset += fieldValueSize;
		// S-JISと仮定して処理する
		SetComment(CString(nameBuffer),CString(pFieldBuffer));
		TRACE(_T("APE:%s:%s\n"),nameBuffer,pFieldBuffer);
		free(pFieldBuffer);
	}
	free(pRawTag);

	return ERROR_SUCCESS;
}

#if 0
CString Utf8toSjis(char *utf8,int len)
{
	// UTF-8 -> Ansi
	// (UTF-8 -> UNICODE)
	int size = MultiByteToWideChar(CP_UTF8,0,utf8,len,NULL,0);
	size++;
	WCHAR *buf = new WCHAR[size];
	if(!buf) return "";
	MultiByteToWideChar(CP_UTF8,0,utf8,len,buf,size-1);
	buf[size-1] = L'\0';
	// (UNICODE -> Ansi)
	size = WideCharToMultiByte(CP_UTF8,0,buf,-1,0,0,NULL,NULL);
	char *buf2 = new char[size];
	if(!buf2)
	{
		delete buf;
		return "";
	}
	WideCharToMultiByte(CP_UTF8,0,buf,-1,buf2,size,NULL,NULL);
	CString strRet(buf);
	delete buf;
	delete buf2;

	return strRet;
}
#endif

DWORD CTag_Ape::_LoadApeTagV2(HANDLE hFile)
{
	DWORD dwRet;
	DWORD dwWin32errorCode = ERROR_SUCCESS;
	
	// ID3TAGを探す
	SetFilePointer(hFile,-sizeof(ID3_TAG),NULL,FILE_END);
	if(!ReadFile(hFile,&m_id3tag,sizeof(m_id3tag),&dwRet,NULL) || (dwRet != sizeof(m_id3tag)))
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		return dwWin32errorCode;
	}

	BOOL bId3v1 = FALSE;
	// ID3TAGの正当性をチェック
	if((m_id3tag.TagHeader[0] == 'T') &&
		(m_id3tag.TagHeader[1] == 'A') &&
		(m_id3tag.TagHeader[2] == 'G'))
	{
		bId3v1 = TRUE;
	}
	
	DWORD rawFieldBytes = m_footer.size - ((m_footer.flags&APE_FLAG_TAG_HAS_HEADER)?sizeof(APE_TAG_FOOTER):0);
	char *pRawTag = (char *)malloc(rawFieldBytes);
	// ヘッダの先頭に移動
	SetFilePointer(hFile,-m_footer.size-(bId3v1?128:0),NULL,FILE_END);
	// ヘッダをバッファに取り込む
	if(!ReadFile(hFile,pRawTag,rawFieldBytes,&dwRet,NULL) || (dwRet != rawFieldBytes))
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		return dwWin32errorCode;
	}
	// 各フィールドの取り込み
	int rawTagOffset = 0;
	for(int i=0; i<m_footer.fields; i++)
	{
		if(rawFieldBytes < 4+4+1)
		{
			// バッファオーバーフロー
			free(pRawTag);
			CloseHandle(hFile);
			return -1;
		}
		// フィールドサイズ
		DWORD fieldValueSize = *(DWORD *)&pRawTag[rawTagOffset];
		if(fieldValueSize > rawFieldBytes)
		{
			free(pRawTag);
			CloseHandle(hFile);
			return -1;
		}
		rawTagOffset += 4;
		// フィールドフラグ
		int fieldFlags = *(int *)&pRawTag[rawTagOffset];
		rawTagOffset += 4;
		// フィールド名
		char nameBuffer[256];
		strncpy(nameBuffer,&pRawTag[rawTagOffset],min(sizeof(nameBuffer)-1,rawFieldBytes-rawTagOffset));
		nameBuffer[255] = '\0';
		rawTagOffset += strlen(nameBuffer) + 1;
		if((fieldFlags & APE_FLAG_TEXTINFO_MASK) != APE_FLAG_TEXTINFO_UTF8)
		{
			// UTF8文字列でないときはスキップ
			rawTagOffset += fieldValueSize;
			continue;
		}
		// 値
		if(rawFieldBytes < rawTagOffset)
		{
			// バッファオーバーフロー
			free(pRawTag);
			CloseHandle(hFile);
			return -1;
		}
		char *pFieldBuffer = (char *)malloc(fieldValueSize+1);
		memcpy(pFieldBuffer,&pRawTag[rawTagOffset],fieldValueSize);
		pFieldBuffer[fieldValueSize] = '\0';
		// UTF-8->TCHAR
		CString str = DataToCString(pFieldBuffer, fieldValueSize, DTC_CODE_UTF8);
		rawTagOffset += fieldValueSize;
		SetComment(CString(nameBuffer),str);
		TRACE(_T("APE:%s:%s\n"),nameBuffer,pFieldBuffer);
		free(pFieldBuffer);
	}
	free(pRawTag);

	return ERROR_SUCCESS;
}

DWORD CTag_Ape::Load(LPCTSTR szFileName)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;
	TRACE(_T("CTag_Ape::Load(%s)\n"),szFileName);
	Release();

	//ファイルをオープン
	HANDLE hFile = CreateFile(
				szFileName,
				GENERIC_READ,
				FILE_SHARE_READ,
				NULL,
				OPEN_EXISTING,	//ファイルをオープンします。指定ファイルが存在していない場合、関数は失敗します。
				FILE_ATTRIBUTE_NORMAL,
				NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}

	DWORD dwRet;
	// ID3TAGを探す
	SetFilePointer(hFile,-sizeof(ID3_TAG),NULL,FILE_END);
	if(!ReadFile(hFile,&m_id3tag,sizeof(m_id3tag),&dwRet,NULL) || (dwRet != sizeof(m_id3tag)))
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		return dwWin32errorCode;
	}

	BOOL bId3v1 = FALSE;
	// ID3TAGの正当性をチェック
	if((m_id3tag.TagHeader[0] == 'T') &&
		(m_id3tag.TagHeader[1] == 'A') &&
		(m_id3tag.TagHeader[2] == 'G'))
	{
		bId3v1 = TRUE;
	}
	
	// apeフッタを探す
	SetFilePointer(hFile,-sizeof(APE_TAG_FOOTER)-(bId3v1?128:0),NULL,FILE_END);

	// APE_TAG_FOOTERを読みとる
	if(!ReadFile(hFile,&m_footer,sizeof(m_footer),&dwRet,NULL) || (dwRet != sizeof(m_footer)))
	{
		CloseHandle(hFile);
		return dwWin32errorCode;
	}
	// APE_TAG_FOOTERの正当性をチェック
	if((strncmp(m_footer.id,"APETAGEX",8) == 0) &&
		(m_footer.version <= CURRENT_APE_TAG_VERSION) &&
		(m_footer.fields < 65536) &&
		(m_footer.size < (1024 * 1024 * 16))
		)
	{
		m_bHasApetag = TRUE;
		m_apeVersion = m_footer.version;
		if(m_footer.version < 2000)
		{
			dwWin32errorCode = _LoadApeTagV1(hFile);
		}
		else
		{
			dwWin32errorCode = _LoadApeTagV2(hFile);
		}
	}

	CloseHandle(hFile);
	
	// ID3tagを読み込む
	dwWin32errorCode = _LoadId3Tag(szFileName);

	return dwWin32errorCode;
}

DWORD CTag_Ape::_SaveId3TagV1(LPCTSTR szFileName)
{
	DWORD dwWritten;
	DWORD dwWin32errorCode = ERROR_SUCCESS;
	HANDLE hFile = CreateFile(
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
	SetFilePointer(hFile,0,NULL,FILE_END);
	if(WriteFile(hFile,&m_id3tag,128,&dwWritten,NULL) == 0)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		return dwWin32errorCode;
	}
	if(!SetEndOfFile(hFile))
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		return dwWin32errorCode;
	}
	CloseHandle(hFile);

	return ERROR_SUCCESS;
}

// id3V1はあらかじめ除去しておくこと
DWORD CTag_Ape::_SaveApeTagV2(LPCTSTR szFileName)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;
	DWORD dwRet;

	//ファイルをオープン
	HANDLE hFile = CreateFile(
				szFileName,
				GENERIC_READ|GENERIC_WRITE,
				FILE_SHARE_READ,
				NULL,
				OPEN_EXISTING,	//ファイルをオープンします。指定ファイルが存在していない場合、関数は失敗します。
				FILE_ATTRIBUTE_NORMAL,
				NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}

	map<CString,CString>::iterator it = m_comments.begin();
	APE_TAG_FOOTER footer;
	// ApeTag header を構築
	APE_TAG_FOOTER header;
	memset(&header,0,sizeof(header));
	strncpy(header.id,"APETAGEX",8);
	header.version = CURRENT_APE_TAG_VERSION;
	header.size = sizeof(header);
	header.fields = m_comments.size();
	header.flags =	APE_FLAG_TAG_HAS_HEADER | APE_FLAG_THIS_IS_THE_HEADER;

	// ApeTag fotter を構築
	memset(&footer,0,sizeof(footer));
	strncpy(footer.id,"APETAGEX",8);
	footer.version = CURRENT_APE_TAG_VERSION;
	footer.size = sizeof(footer);
	footer.fields = m_comments.size();
	footer.flags =	APE_FLAG_TAG_HAS_HEADER;

	// ファイル最後尾に移動
	SetFilePointer(hFile,0,NULL,FILE_END);
	DWORD dwHeaderPtr = SetFilePointer(hFile,0,NULL,FILE_CURRENT);
	WriteFile(hFile,&header,sizeof(header),&dwRet,NULL);
	it = m_comments.begin();
	while(it != m_comments.end())
	{
		TRACE(_T("APE(save):%s:%s\n"),(it->first),it->second);
		int utf8len = 0;
		unsigned char *dataUtf8 = NULL;
		{
			dataUtf8 = (unsigned char *)TstrToDataAlloc(it->second, lstrlen(it->second), &utf8len, DTC_CODE_UTF8);
			if(!dataUtf8)
			{
				continue;
			}
		}
		DWORD size = utf8len;
		DWORD flag = 0x00000000;
		WriteFile(hFile,&size,sizeof(size),&dwRet,NULL);
		header.size += 4;
		footer.size += 4;
		WriteFile(hFile,&flag,sizeof(flag),&dwRet,NULL);
		header.size += 4;
		footer.size += 4;
		
		char nameBuffer[256];
		size = TstrToData(it->first, -1, nameBuffer, sizeof(nameBuffer), DTC_CODE_UTF8);
		WriteFile(hFile,nameBuffer,size,&dwRet,NULL);
		header.size += size;
		footer.size += size;
		WriteFile(hFile,dataUtf8,utf8len,&dwRet,NULL);
		header.size += utf8len;
		footer.size += utf8len;
		free(dataUtf8);
		it++;
	}

	WriteFile(hFile,&footer,sizeof(footer),&dwRet,NULL);
	// ヘッダを確定
	DWORD dwFooterPtr = SetFilePointer(hFile,0,NULL,FILE_CURRENT);
	SetFilePointer(hFile,dwHeaderPtr,NULL,FILE_BEGIN);
	WriteFile(hFile,&header,sizeof(header),&dwRet,NULL);
	SetFilePointer(hFile,dwFooterPtr,NULL,FILE_BEGIN);

	// 長さを確定
	if(!SetEndOfFile(hFile))
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		return dwWin32errorCode;
	}
	CloseHandle(hFile);

	return dwWin32errorCode;
}

DWORD CTag_Ape::Save(LPCTSTR szFileName)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;

	// ID3v1をバックアップ
	dwWin32errorCode = _LoadId3Tag(szFileName);
	if(dwWin32errorCode != ERROR_SUCCESS)
	{
		return dwWin32errorCode;
	}

	// ID3v1/APE Tagを消去
	dwWin32errorCode = _DelTag(szFileName);
	if(dwWin32errorCode != ERROR_SUCCESS)
	{
		return dwWin32errorCode;
	}
	m_bHasApetag = FALSE;

	// Apeタグを保存
	dwWin32errorCode = _SaveApeTagV2(szFileName);
	if(dwWin32errorCode != ERROR_SUCCESS)
	{
		return dwWin32errorCode;
	}
	
	// Id3v1タグを保存
	if(m_bHasId3tag && !m_bDoNotSaveId3v1)
	{
		dwWin32errorCode = _SaveId3TagV1(szFileName);
		if(dwWin32errorCode != ERROR_SUCCESS)
		{
			return dwWin32errorCode;
		}
	}

	return ERROR_SUCCESS;
}

DWORD CTag_Ape::DelTag(LPCTSTR szFileName)
{
	// ID3v1をバックアップ
	DWORD dwWin32errorCode = _LoadId3Tag(szFileName);
	if(dwWin32errorCode != ERROR_SUCCESS)
	{
		return dwWin32errorCode;
	}

	m_comments.clear();
	// ID3v1/APE Tagを消去
	dwWin32errorCode = _DelTag(szFileName);
	if(dwWin32errorCode != ERROR_SUCCESS)
	{
		return dwWin32errorCode;
	}
	m_bHasApetag = FALSE;

	// Id3v1タグを復元
	if(m_bHasId3tag && !m_bDoNotSaveId3v1)
	{
		_SaveId3TagV1(szFileName);
	}

	return ERROR_SUCCESS;
}

DWORD CTag_Ape::_DelTag(LPCTSTR szFileName)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;
	HANDLE	hFile;
	
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

	DWORD dwRet;
	// ID3TAGを探す
	ID3_TAG id3tag;
	SetFilePointer(hFile,-sizeof(ID3_TAG),NULL,FILE_END);
	if(ReadFile(hFile,&id3tag,sizeof(id3tag),&dwRet,NULL) && (dwRet == sizeof(id3tag)))
	{
		// ID3TAGの正当性をチェック
		if((id3tag.TagHeader[0] == 'T') &&
			(id3tag.TagHeader[1] == 'A') &&
			(id3tag.TagHeader[2] == 'G'))
		{
			// ID3tagをカット
			SetFilePointer(hFile,-128,NULL,FILE_END);
			// 長さを確定
			if(!SetEndOfFile(hFile))
			{
				dwWin32errorCode = GetLastError();
				CloseHandle(hFile);
				return dwWin32errorCode;
			}
		}
	}

	// フッタを探す
	APE_TAG_FOOTER footer;
	SetFilePointer(hFile,-sizeof(APE_TAG_FOOTER),NULL,FILE_END);

	// APE_TAG_FOOTERを読みとる
	if(ReadFile(hFile,&footer,sizeof(footer),&dwRet,NULL) && (dwRet == sizeof(footer)))
	{
		// APE_TAG_FOOTERの正当性をチェック
		if(strncmp(m_footer.id,"APETAGEX",8) == 0)
		{
			// ApeTagをカット
			if(m_footer.version < 2000)
			{
				SetFilePointer(hFile,-footer.size,NULL,FILE_END);
			}
			else
			{
				SetFilePointer(hFile,-footer.size-((m_footer.flags&APE_FLAG_TAG_HAS_HEADER)?sizeof(APE_TAG_FOOTER):0),NULL,FILE_END);
			}
			// 長さを確定
			if(!SetEndOfFile(hFile))
			{
				dwWin32errorCode = GetLastError();
				CloseHandle(hFile);
				return dwWin32errorCode;
			}
		}
	}

	CloseHandle(hFile);

	return dwWin32errorCode;
}

DWORD CTag_Ape::MakeTag(LPCTSTR szFileName)
{
	// ID3v1をバックアップ
	DWORD dwWin32errorCode = _LoadId3Tag(szFileName);
	if(dwWin32errorCode != ERROR_SUCCESS)
	{
		return dwWin32errorCode;
	}

	// 現存タグを削除
	m_comments.clear();
	dwWin32errorCode = _DelTag(szFileName);
	if(dwWin32errorCode != ERROR_SUCCESS)
	{
		return dwWin32errorCode;
	}

	// Apeタグを保存
	CString strDefaultName = getFileName(CString(szFileName));
	
	SetComment(CTag_Ape::APE_TAG_FIELD_TITLE,strDefaultName);
	
	dwWin32errorCode = _SaveApeTagV2(szFileName);
	if(dwWin32errorCode != ERROR_SUCCESS)
	{
		return dwWin32errorCode;
	}

	// Id3v1タグを復元
	if(m_bHasId3tag && !m_bDoNotSaveId3v1)
	{
		_SaveId3TagV1(szFileName);
	}

	return ERROR_SUCCESS;
}

void CTag_Ape::_GetId3tagString(char *szTag)
{
	memset(szTag,0x00,128);
	strncpy(szTag,"TAG",3);
	if(m_bScmpxGenre)
		szTag[127] = (char )SCMPX_GENRE_NULL;
	else
		szTag[127] = (char )WINAMP_GENRE_NULL;
}

