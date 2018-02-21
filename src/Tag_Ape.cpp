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
	CommentMap::iterator it = m_comments.begin();
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

	if(value[0] == '\0')
	{
		return TRUE;	// 空はセットしない
	}
	m_comments.insert(std::pair<CString,CString>(name,CString(value)));
	
	return TRUE;
}

BOOL CTag_Ape::GetComment(LPCTSTR name,CString &strValue)
{
	strValue = _T("");

	CommentMap::iterator it = m_comments.begin();
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
	CommentMap::iterator it = m_comments.begin();
	
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
	SetFilePointer(hFile,-(int)sizeof(ID3_TAG),NULL,FILE_END);
	if(!ReadFile(hFile,&m_id3tag,sizeof(m_id3tag),&dwRet,NULL) || (dwRet != sizeof(m_id3tag)))
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		return dwWin32errorCode;
	}

	// ID3TAGの正当性をチェック
	m_bHasId3tag = CId3tagv1::IsTagValid(&m_id3tag);

	CloseHandle(hFile);
	return ERROR_SUCCESS;
}

DWORD CTag_Ape::_LoadApeTagV1(HANDLE hFile)
{
	DWORD dwRet;
	DWORD dwWin32errorCode = ERROR_SUCCESS;

	// ID3TAGを探す
	SetFilePointer(hFile,-(int)sizeof(ID3_TAG),NULL,FILE_END);
	if(!ReadFile(hFile,&m_id3tag,sizeof(m_id3tag),&dwRet,NULL) || (dwRet != sizeof(m_id3tag)))
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		return dwWin32errorCode;
	}

	// ID3TAGの正当性をチェック
	BOOL bId3v1 = CId3tagv1::IsTagValid(&m_id3tag);
	
	DWORD rawFieldBytes = m_footer.size - sizeof(APE_TAG_FOOTER);
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
	SetFilePointer(hFile,-(int)sizeof(ID3_TAG),NULL,FILE_END);
	if(!ReadFile(hFile,&m_id3tag,sizeof(m_id3tag),&dwRet,NULL) || (dwRet != sizeof(m_id3tag)))
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		return dwWin32errorCode;
	}

	// ID3TAGの正当性をチェック
	BOOL bId3v1 = CId3tagv1::IsTagValid(&m_id3tag);
	
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
		CString str = BytesToCString(pFieldBuffer, fieldValueSize, BTC_CODE_UTF8);
		rawTagOffset += fieldValueSize;
		SetComment(CString(nameBuffer),str);
		TRACE(_T("APE:%s:%s\n"),nameBuffer,pFieldBuffer);
		free(pFieldBuffer);
	}
	free(pRawTag);

	return ERROR_SUCCESS;
}

BOOL CTag_Ape::IsTagValid(const APE_TAG_FOOTER *footer)
{

	if((strncmp(footer->id,"APETAGEX",8) == 0) &&
		(footer->version <= CURRENT_APE_TAG_VERSION) &&
		(footer->fields < 65536) &&
		(footer->size < (1024 * 1024 * 16))
		)
	{
		return TRUE;
	}
	return FALSE;
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
	SetFilePointer(hFile,-(int)sizeof(ID3_TAG),NULL,FILE_END);
	if(!ReadFile(hFile,&m_id3tag,sizeof(m_id3tag),&dwRet,NULL) || (dwRet != sizeof(m_id3tag)))
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		return dwWin32errorCode;
	}

	BOOL bId3v1 = FALSE;
	// ID3TAGの正当性をチェック
	if(CId3tagv1::IsTagValid(&m_id3tag))
	{
		bId3v1 = TRUE;
	}
	
	// apeフッタを探す
	SetFilePointer(hFile,-(int)sizeof(APE_TAG_FOOTER)-(bId3v1?128:0),NULL,FILE_END);

	// APE_TAG_FOOTERを読みとる
	if(!ReadFile(hFile,&m_footer,sizeof(m_footer),&dwRet,NULL) || (dwRet != sizeof(m_footer)))
	{
		CloseHandle(hFile);
		return dwWin32errorCode;
	}
	// APE_TAG_FOOTERの正当性をチェック
	if(IsTagValid(&m_footer))
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

	CommentMap::iterator it = m_comments.begin();
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
			dataUtf8 = (unsigned char *)TstrToBytesAlloc(it->second, lstrlen(it->second), &utf8len, BTC_CODE_UTF8);
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
		size = TstrToBytes(it->first, -1, nameBuffer, sizeof(nameBuffer), BTC_CODE_UTF8);
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
	SetFilePointer(hFile,-(int)sizeof(ID3_TAG),NULL,FILE_END);
	if(ReadFile(hFile,&id3tag,sizeof(id3tag),&dwRet,NULL) && (dwRet == sizeof(id3tag)))
	{
		// ID3TAGの正当性をチェック
		if(CId3tagv1::IsTagValid(&id3tag))
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
	SetFilePointer(hFile,-(int)sizeof(APE_TAG_FOOTER),NULL,FILE_END);

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
	CString strFileName = szFileName;
	CString strDefaultName = getFileName(strFileName);
	
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

#if 0
void CTag_Ape::_GetId3tagString(char *szTag)
{
	memset(szTag,0x00,128);
	strncpy(szTag,"TAG",3);
	if(m_bScmpxGenre)
		szTag[127] = (char )SCMPX_GENRE_NULL;
	else
		szTag[127] = (char )WINAMP_GENRE_NULL;
}
#endif
