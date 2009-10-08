// RMP.cpp: CRMP クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"		// メイン シンボル
#include "GlobalCommand.h"
#include "RMP.h"
#include <io.h>

static const unsigned char SCMPX_GENRE_NULL = 247;
static const unsigned char WINAMP_GENRE_NULL = 255;

//#pragma comment(lib,"winmm.lib")

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CRMP::CRMP(BOOL bScmpxGenre)
{
	m_bScmpxGenre = bScmpxGenre;
	Release();
}

CRMP::~CRMP()
{

}

void CRMP::Release()
{
	m_bEnable = FALSE;
	m_bHasId3tag = FALSE;
	m_strNAM = "";		//INAM songname
	m_strART = "";		//IART アーティスト名
	m_strPRD = "";		//IPRD アルバム名
	m_strCMT = "";		//ICMT コメント
	m_strCRD = "";		//ICRD 日付
	m_strGNR = "";		//IGNR ジャンル
	m_strCOP = "";		//ICOP 著作権
	m_strENG = "";		//IENG エンジニア
	m_strSRC = "";		//ISRC ソース
	m_strSFT = "";		//ISFT ソフトウェア
	m_strKEY = "";		//IKEY キーワード
	m_strTCH = "";		//ITCH 技術者
	m_strLYC = "";		//ILYC 歌詞
	m_strCMS = "";		//ICMS コミッション
	m_strMED = "";		//IMED 中間
	m_strSBJ = "";		//ISBJ subject
	m_strMP3 = "";		//IMP3 mp3 info
}

void CRMP::SetScmpxGenre(BOOL bSwitch)
{
	m_bScmpxGenre = bSwitch;
}

void CRMP::SetSftDefault(const char *szDefaultSoft)
{
	m_strDefaultSft = szDefaultSoft;
}

CString CRMP::ReadChunk(HMMIO hmmio,MMCKINFO mmckinfo,FOURCC id)
{
	CString retString;
	MMCKINFO	mmckOutinfoSubchunk;
	memset(&mmckOutinfoSubchunk,0,sizeof(mmckOutinfoSubchunk));
	mmckOutinfoSubchunk.ckid = id;
	if(mmioDescend(hmmio,&mmckOutinfoSubchunk,&mmckinfo,MMIO_FINDCHUNK) == MMSYSERR_NOERROR)
	{
		mmioRead(hmmio,retString.GetBuffer(mmckOutinfoSubchunk.cksize),mmckOutinfoSubchunk.cksize);
		//2001-05-12 ILYC対策
		if( (mmckOutinfoSubchunk.cksize > 11) && 
			(mmioFOURCC('I','L','Y','C') == id) &&
			(strncmp((LPCSTR )retString+1,"LYRICSBEGIN",11) == 0) )
		{
			(char )(((LPCSTR )retString)[0]) = '_';	//とりあえず'_'を入れておく(後で\0に戻す)
		}
		//長さを修正
		retString.GetBufferSetLength(strlen((LPCSTR )retString));
		mmioAscend(hmmio,&mmckOutinfoSubchunk,0);
	}
	return retString;
}

DWORD CRMP::Load(const char *szFileName)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;
	Release();

	// バッファ付きI/Oを使ってファイルを開く
	HMMIO hmmio = mmioOpen((char *)szFileName,NULL,MMIO_COMPAT);
	if(!hmmio)
	{
		return -1;
	}

	//RMP3ファイルの確認
	char hdr[4];
	LONG ret = mmioRead(hmmio,hdr,sizeof(hdr));
	if((ret != 4) || (hdr[0] != 'R') || (hdr[1] != 'I') || (hdr[2] != 'F') || (hdr[3] != 'F'))
	{
		mmioClose(hmmio,0);
		return ERROR_SUCCESS;
	}

	mmioSeek(hmmio,0,SEEK_SET);
	//RMP3チャンクへ移動
	MMCKINFO	mmckOutinfoParent;
	memset(&mmckOutinfoParent,0,sizeof(mmckOutinfoParent));
	mmckOutinfoParent.fccType = mmioFOURCC('R','M','P','3');
	if(mmioDescend(hmmio,&mmckOutinfoParent,NULL,MMIO_FINDRIFF) != MMSYSERR_NOERROR)
	{
		mmioClose(hmmio,0);
		return ERROR_SUCCESS;
	}
	m_bEnable = TRUE;
	//INFOチャンクへ移動
	MMCKINFO	mmckOutinfoSubchunk;
	memset(&mmckOutinfoSubchunk,0,sizeof(mmckOutinfoSubchunk));
	mmckOutinfoSubchunk.fccType = mmioFOURCC('I','N','F','O');
	if(mmioDescend(hmmio,&mmckOutinfoSubchunk,&mmckOutinfoParent,MMIO_FINDLIST) != MMSYSERR_NOERROR)
	{
		mmioClose(hmmio,0);
		return ERROR_SUCCESS;
	}
	long lListTop = mmioSeek(hmmio,0,SEEK_CUR);

	//INAM songname
	m_strNAM = ReadChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','N','A','M'));
	mmioSeek(hmmio,lListTop,SEEK_SET);
	
	//IART アーティスト名
	m_strART = ReadChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','A','R','T'));
	mmioSeek(hmmio,lListTop,SEEK_SET);

	//IPRD アルバム名
	m_strPRD = ReadChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','P','R','D'));
	mmioSeek(hmmio,lListTop,SEEK_SET);

	//ICMT コメント
	m_strCMT = ReadChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','C','M','T'));
	mmioSeek(hmmio,lListTop,SEEK_SET);

	//ICRD 日付
	m_strCRD = ReadChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','C','R','D'));
	mmioSeek(hmmio,lListTop,SEEK_SET);

	//IGNR ジャンル
	m_strGNR = ReadChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','G','N','R'));
	mmioSeek(hmmio,lListTop,SEEK_SET);

	//ICOP 著作権
	m_strCOP = ReadChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','C','O','P'));
	mmioSeek(hmmio,lListTop,SEEK_SET);

	//IENG エンジニア
	m_strENG = ReadChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','E','N','G'));
	mmioSeek(hmmio,lListTop,SEEK_SET);

	//ISRC ソース
	m_strSRC = ReadChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','S','R','C'));
	mmioSeek(hmmio,lListTop,SEEK_SET);

	//ISFT ソフトウェア
	m_strSFT = ReadChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','S','F','T'));
	mmioSeek(hmmio,lListTop,SEEK_SET);

	//IKEY キーワード
	m_strKEY = ReadChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','K','E','Y'));
	mmioSeek(hmmio,lListTop,SEEK_SET);

	//ITCH 技術者
	m_strTCH = ReadChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','T','C','H'));
	mmioSeek(hmmio,lListTop,SEEK_SET);

	//ILYC 歌詞
	m_strLYC = ReadChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','L','Y','C'));
	mmioSeek(hmmio,lListTop,SEEK_SET);

	//ICMS コミッション
	m_strCMS = ReadChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','C','M','S'));
	mmioSeek(hmmio,lListTop,SEEK_SET);

	//IMED 中間
	m_strMED = ReadChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','M','E','D'));
	mmioSeek(hmmio,lListTop,SEEK_SET);

	//ISBJ subject
	m_strSBJ = ReadChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','S','B','J'));
	mmioSeek(hmmio,lListTop,SEEK_SET);

	//IMP3 mp3 info
	m_strMP3 = ReadChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','M','P','3'));
	mmioSeek(hmmio,lListTop,SEEK_SET);

	//IID3
	MMCKINFO	mmckOutinfoSubSubchunk;
	memset(&mmckOutinfoSubSubchunk,0,sizeof(mmckOutinfoSubSubchunk));
	mmckOutinfoSubSubchunk.ckid = mmioFOURCC('I','I','D','3');
	if(mmioDescend(hmmio,&mmckOutinfoSubSubchunk,&mmckOutinfoSubchunk,MMIO_FINDCHUNK) == MMSYSERR_NOERROR)
	{
		mmioAscend(hmmio,&mmckOutinfoSubSubchunk,0);
		m_bHasId3tag = TRUE;
	}
	mmioSeek(hmmio,lListTop,SEEK_SET);
	
	mmioClose(hmmio,0);

	return dwWin32errorCode;
}

DWORD CRMP::Save(HWND hWnd,const char *szFileName)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;
	DWORD		mp3Size;
	DWORD		listSize;
	DWORD		newSize;
	DWORD		dwNewSize;	//予想される増加サイズ
	DWORD		dwOldSize;	//
	FILE		*fp;
	HANDLE		hFile;
	DWORD		dwWritten;

	//予想される増加サイズ
	dwNewSize = lstrlen("LIST----INFO");
	dwNewSize += m_strNAM.GetLength()+2+4+4;//+2はワードアラインメントを考慮しての数値
	dwNewSize += m_strART.GetLength()+2+4+4;
	dwNewSize += m_strPRD.GetLength()+2+4+4;
	dwNewSize += m_strCMT.GetLength()+2+4+4;
	dwNewSize += m_strCRD.GetLength()+2+4+4;
	dwNewSize += m_strGNR.GetLength()+2+4+4;
	dwNewSize += m_strCOP.GetLength()+2+4+4;
	dwNewSize += m_strENG.GetLength()+2+4+4;
	dwNewSize += m_strSRC.GetLength()+2+4+4;
	dwNewSize += m_strSFT.GetLength()+2+4+4;
	dwNewSize += m_strKEY.GetLength()+2+4+4;
	dwNewSize += m_strTCH.GetLength()+2+4+4;
	dwNewSize += m_strLYC.GetLength()+2+4+4;
	dwNewSize += m_strCMS.GetLength()+2+4+4;
	dwNewSize += m_strMED.GetLength()+2+4+4;
	dwNewSize += m_strSBJ.GetLength()+2+4+4;
	dwNewSize += m_strMP3.GetLength()+2+4+4;
	dwNewSize += 128+2+4+4;	//szId3dummy

	hFile = CreateFile(szFileName,GENERIC_WRITE|GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}
	//ID3TAGv1情報の保存
	char id3tag[128];
	if(SetFilePointer(hFile,-128,NULL,FILE_END) == INVALID_SET_FILE_POINTER)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		return dwWin32errorCode;
	}
	if(	!ReadFile(hFile,id3tag,sizeof(id3tag),&dwWritten,NULL) ||
		(strncmp("TAG",id3tag,3) != 0) ||
		(dwWritten != sizeof(id3tag)) )
	{
		if(m_bHasId3tag)
		{
			char	szDefaultName[MAX_PATH];
			memset(id3tag,0,sizeof(id3tag));

			memcpy(id3tag,"TAG",3);
			if(m_bScmpxGenre)
				id3tag[127] = (char )SCMPX_GENRE_NULL;
			else
				id3tag[127] = (char )WINAMP_GENRE_NULL;
			strcpy(szDefaultName,getFileName(szFileName));
			cutExtName(szDefaultName);
			strncpy(id3tag+3,szDefaultName,30);
		}
	}
	//試しにファイルを大きくしてみる
	dwOldSize = GetFileSize(hFile,NULL);
	if(SetFilePointer(hFile,dwNewSize,NULL,FILE_END) == INVALID_SET_FILE_POINTER)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		return dwWin32errorCode;
	}
	if(SetEndOfFile(hFile) == 0)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		return dwWin32errorCode;
	}
	//元に戻す
	if(SetFilePointer(hFile,dwOldSize,NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		return dwWin32errorCode;
	}
	if(SetEndOfFile(hFile) == 0)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		return dwWin32errorCode;
	}
	CloseHandle(hFile);

	// バッファ付きI/Oを使ってファイルを開く
	HMMIO hmmio = mmioOpen((LPSTR )szFileName,NULL,MMIO_COMPAT);
	if(!hmmio)
	{
		return -1;
	}
	//RMP3ファイルの確認
	char hdr[4];
	LONG ret = mmioRead(hmmio,hdr,sizeof(hdr));
	if((ret != 4) || (hdr[0] != 'R') || (hdr[1] != 'I') || (hdr[2] != 'F') || (hdr[3] != 'F'))
	{
		mmioClose(hmmio,0);
		return -1;
	}
	mmioSeek(hmmio,0,SEEK_SET);

	//RMP3チャンクへ移動
	MMCKINFO mmckOutinfoParent;
	memset(&mmckOutinfoParent,0,sizeof(mmckOutinfoParent));
	mmckOutinfoParent.fccType = mmioFOURCC('R','M','P','3');
	if(mmioDescend(hmmio,&mmckOutinfoParent,NULL,MMIO_FINDRIFF) != MMSYSERR_NOERROR)
	{
		mmioClose(hmmio,0);
		return -1;
	}
	mp3Size = mmckOutinfoParent.cksize;

	MMCKINFO mmckOutinfoSubchunk;
	memset(&mmckOutinfoSubchunk,0,sizeof(mmckOutinfoSubchunk));
	//INFOチャンクへ移動
	mmckOutinfoSubchunk.fccType = mmioFOURCC('I','N','F','O');
	if(mmioDescend(hmmio,&mmckOutinfoSubchunk,&mmckOutinfoParent,MMIO_FINDLIST) != MMSYSERR_NOERROR)
	{
		mmioClose(hmmio,0);
		return -1;
	}
//	listSize=mmckOutinfoSubchunk.cksize;//INFOチャンクサイズが正しくないデータに対応
	listSize = mmioSeek(hmmio,0,SEEK_CUR);
	if(listSize == -1)
	{
		mmioClose(hmmio,0);
		return -1;
	}
	listSize = dwOldSize - listSize + 4;
	mmioClose(hmmio,0);

	//後ろを切り取る
	if((fp=fopen(szFileName,"r+b")) == NULL)
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}
	if(fseek(fp,4,SEEK_SET) != 0)
	{
		dwWin32errorCode = GetLastError();
		fclose(fp);
		return dwWin32errorCode;
	}
	newSize = mp3Size-listSize-8;
	fwrite(&newSize,1,4,fp);
	if(fseek(fp,-(long )(listSize+8),SEEK_END) != 0)
	{
		dwWin32errorCode = GetLastError();
		fclose(fp);
		return dwWin32errorCode;
	}
	//切りつめ
	_chsize(fileno(fp),ftell(fp));
	fclose(fp);

	// バッファ付きI/Oを使ってファイルを開く
	if((hmmio = mmioOpen((LPSTR )szFileName,NULL,MMIO_COMPAT | MMIO_READWRITE)) == NULL)
	{
		return -1;
	}
	//RMP3ファイルの確認
	ret = mmioRead(hmmio,hdr,sizeof(hdr));
	if((ret != 4) || (hdr[0] != 'R') || (hdr[1] != 'I') || (hdr[2] != 'F') || (hdr[3] != 'F'))
	{
		mmioClose(hmmio,0);
		return -1;
	}
	mmioSeek(hmmio,0,SEEK_SET);

	//RMP3チャンクへ移動
	memset(&mmckOutinfoParent,0,sizeof(mmckOutinfoParent));
	mmckOutinfoParent.fccType = mmioFOURCC('R','M','P','3');
	if(mmioDescend(hmmio,&mmckOutinfoParent,NULL,MMIO_FINDRIFF) != MMSYSERR_NOERROR)
	{
		mmioClose(hmmio,0);
		return -1;
	}
	mmckOutinfoParent.dwFlags = MMIO_DIRTY;//サイズ変更可に設定

	//ファイル終端に飛ぶ
	mmioSeek(hmmio,0,SEEK_END);

	//INFOチャンク作成・移動
	memset(&mmckOutinfoSubchunk,0,sizeof(mmckOutinfoSubchunk));
	mmckOutinfoSubchunk.fccType = mmioFOURCC('I','N','F','O');
	mmckOutinfoSubchunk.cksize = 0L;
	if(mmioCreateChunk(hmmio,&mmckOutinfoSubchunk,MMIO_CREATELIST) != 0)
	{
		mmioClose(hmmio,0);
		return -1;
	}
	mmckOutinfoSubchunk.dwFlags = MMIO_DIRTY;//サイズ変更可に設定
	//INAM songname
	WriteChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','N','A','M'),(LPCSTR )m_strNAM,m_strNAM.GetLength()+1);
	//IART アーティスト名
	WriteChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','A','R','T'),(LPCSTR )m_strART,m_strART.GetLength()+1);
	//IPRD アルバム名
	WriteChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','P','R','D'),(LPCSTR )m_strPRD,m_strPRD.GetLength()+1);
	//ICMT コメント
	WriteChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','C','M','T'),(LPCSTR )m_strCMT,m_strCMT.GetLength()+1);
	//ICRD 日付
	WriteChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','C','R','D'),(LPCSTR )m_strCRD,m_strCRD.GetLength()+1);
	//IGNR ジャンル
	WriteChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','G','N','R'),(LPCSTR )m_strGNR,m_strGNR.GetLength()+1);
	//ICOP 著作権
	WriteChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','C','O','P'),(LPCSTR )m_strCOP,m_strCOP.GetLength()+1);
	//IENG エンジニア
	WriteChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','E','N','G'),(LPCSTR )m_strENG,m_strENG.GetLength()+1);
	//ISRC ソース
	WriteChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','S','R','C'),(LPCSTR )m_strSRC,m_strSRC.GetLength()+1);
	//ISFT ソフトウェア
	WriteChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','S','F','T'),(LPCSTR )m_strSFT,m_strSFT.GetLength()+1);
	//IKEY キーワード
	WriteChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','K','E','Y'),(LPCSTR )m_strKEY,m_strKEY.GetLength()+1);
	//ITCH 技術者
	WriteChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','T','C','H'),(LPCSTR )m_strTCH,m_strTCH.GetLength()+1);
	//ILYC 歌詞
	WriteChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','L','Y','C'),(LPCSTR )m_strLYC,m_strLYC.GetLength()+1);
	//ICMS コミッション
	WriteChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','C','M','S'),(LPCSTR )m_strCMS,m_strCMS.GetLength()+1);
	//IMED 中間
	WriteChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','M','E','D'),(LPCSTR )m_strMED,m_strMED.GetLength()+1);
	//ISBJ subject
	WriteChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','S','B','J'),(LPCSTR )m_strSBJ,m_strSBJ.GetLength()+1);
	//IMP3 mp3 info
	WriteChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','M','P','3'),(LPCSTR )m_strMP3,m_strMP3.GetLength()+1);
	//IID3 ID3V1 TAG
	if(m_bHasId3tag)
		WriteChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','I','D','3'),(LPCSTR )id3tag,sizeof(id3tag));

	//サイズの確定
	if(mmioAscend(hmmio,&mmckOutinfoSubchunk,0) != MMSYSERR_NOERROR)
	{
		mmioClose(hmmio,0);
		return -1;
	}
	if(mmioAscend(hmmio,&mmckOutinfoParent,0) != MMSYSERR_NOERROR)
	{
		mmioClose(hmmio,0);
		return -1;
	}
	mmioClose(hmmio,0);

	return dwWin32errorCode;
}

DWORD CRMP::DelTag(HWND hWnd,const char *szFileName)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;
	// バッファ付きI/Oを使ってファイルを開く
	HMMIO hmmio = mmioOpen((char *)szFileName,NULL,MMIO_COMPAT);
	if(hmmio == NULL)
	{
		return -1;
	}
	//RMP3ファイルの確認
	char hdr[4];
	LONG ret = mmioRead(hmmio,hdr,sizeof(hdr));
	if((ret != 4) || (hdr[0] != 'R') || (hdr[1] != 'I') || (hdr[2] != 'F') || (hdr[3] != 'F'))
	{
		mmioClose(hmmio,0);
		return -1;
	}
	mmioSeek(hmmio,0,SEEK_SET);

	//RMP3チャンクへ移動
	MMCKINFO	mmckOutinfoParent;
	memset(&mmckOutinfoParent,0,sizeof(mmckOutinfoParent));
	mmckOutinfoParent.fccType = mmioFOURCC('R','M','P','3');
	if(mmioDescend(hmmio,&mmckOutinfoParent,NULL,MMIO_FINDRIFF) == MMIOERR_CHUNKNOTFOUND)
	{
		mmioClose(hmmio,0);
		return -1;
	}
	DWORD mp3Size = mmckOutinfoParent.cksize;
	//dataチャンクへ移動
	MMCKINFO	mmckOutinfoSubchunk;
	memset(&mmckOutinfoSubchunk,0,sizeof(mmckOutinfoSubchunk));
	mmckOutinfoSubchunk.ckid = mmioFOURCC('d','a','t','a');
	if(mmioDescend(hmmio,&mmckOutinfoSubchunk,&mmckOutinfoParent,MMIO_FINDCHUNK) == MMIOERR_CHUNKNOTFOUND)
	{
		mmioClose(hmmio,0);
		return -1;
	}
	DWORD dataSize = mmckOutinfoSubchunk.cksize;
	//バッファメモリの確保
	char *pRawData = (char *)malloc(dataSize);
	if(!pRawData)
	{
		mmioClose(hmmio,0);
		return -1;
	}
	//raw dataの読み出し
	if(mmioRead(hmmio,pRawData,dataSize) < 1)
	{
		mmioClose(hmmio,0);
		free(pRawData);
		return -1;
	}
	mmioClose(hmmio,0);

	//ID3TAGv1情報の保存
	HANDLE hFile = CreateFile(szFileName,GENERIC_WRITE|GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		dwWin32errorCode = GetLastError();
		free(pRawData);
		return dwWin32errorCode;
	}
	char id3tag[128];
	if(SetFilePointer(hFile,-128,NULL,FILE_END) == INVALID_SET_FILE_POINTER)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		free(pRawData);
		return dwWin32errorCode;
	}
	DWORD dwWritten;
	if(	ReadFile(hFile,id3tag,sizeof(id3tag),&dwWritten,NULL) &&
		(strncmp("TAG",id3tag,3) == 0) &&
		(dwWritten == sizeof(id3tag)) )
	{
		m_bHasId3tag = TRUE;
	}
	else
	{
		m_bHasId3tag = FALSE;
	}
	CloseHandle(hFile);

	//テンポラリ名を取得
	char szTempPath[MAX_PATH];
	strcpy(szTempPath,szFileName);
	cutFileName(szTempPath);
	char szTempFile[MAX_PATH];
	if(!GetTempFileName(szTempPath,"tms",0,szTempFile))
	{
		dwWin32errorCode = GetLastError();
		free(pRawData);
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}
	//ファイルオープン(指定ファイルがすでに存在している場合、そのファイルは上書きされます。)
	hFile = CreateFile(szTempFile,GENERIC_WRITE|GENERIC_READ,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		dwWin32errorCode = GetLastError();
		free(pRawData);
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}
	if(SetFilePointer(hFile,0,NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		free(pRawData);
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}
	
	//書き出し(data)
	if(WriteFile(hFile,pRawData,dataSize,&dwWritten,NULL) == FALSE)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		free(pRawData);
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}
	free(pRawData);
	//書き出し(id3tag)
	if(m_bHasId3tag)
	{
		if(WriteFile(hFile,id3tag,sizeof(id3tag),&dwWritten,NULL) == FALSE)
		{
			dwWin32errorCode = GetLastError();
			CloseHandle(hFile);
			DeleteFile(szTempFile);
			return dwWin32errorCode;
		}
	}
	if(CloseHandle(hFile) == 0)
	{
		dwWin32errorCode = GetLastError();
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}

	//オリジナルファイルを退避(リネーム)
	char szPreFile[MAX_PATH];
	if(!GetTempFileName(szTempPath,"tms",0,szPreFile))
	{
		dwWin32errorCode = GetLastError();
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}
	DeleteFile(szPreFile);//手抜き(^^;
	if(!MoveFile(szFileName,szPreFile))
	{
		dwWin32errorCode = GetLastError();
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}

	//完成品をリネーム
	if(!MoveFile(szTempFile,szFileName))
	{
		dwWin32errorCode = GetLastError();
		MoveFile(szPreFile,szFileName);
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}

	//オリジナルを削除
	DeleteFile(szPreFile);

	Release();

	return dwWin32errorCode;
}

BOOL CRMP::WriteChunk(HMMIO hmmio,MMCKINFO mmckinfo,FOURCC id,const char *pData,DWORD dwSize)
{
	if(dwSize < 2)
		return TRUE;	//サイズが0のデータは記録しない

	MMCKINFO	mmckOutinfoSubchunk;
	memset(&mmckOutinfoSubchunk,0,sizeof(mmckOutinfoSubchunk));
	mmckOutinfoSubchunk.ckid = id;
	mmckOutinfoSubchunk.cksize = 0L;
	if(mmioCreateChunk(hmmio,&mmckOutinfoSubchunk,0) != MMSYSERR_NOERROR)
	{
		//失敗
		mmioClose(hmmio,0);
		return FALSE;
	}
	else
	{
		mmckOutinfoSubchunk.dwFlags = MMIO_DIRTY;//サイズ変更可に設定
		//2001-05-12 ILYC対策
		if( (dwSize > 11) && 
			(mmioFOURCC('I','L','Y','C') == id) &&
			(strncmp(pData,"_LYRICSBEGIN",12) == 0) )
		{
			if(mmioWrite(hmmio,"\0",1) == -1)
			{
				mmioClose(hmmio,0);
				return FALSE;
			}
			if(mmioWrite(hmmio,pData+1,dwSize-1) == -1)//先頭の'_'を除いて
			{
				mmioClose(hmmio,0);
				return FALSE;
			}
		}
		else
		{
			if(mmioWrite(hmmio,pData,dwSize) == -1)
			{
				mmioClose(hmmio,0);
				return FALSE;
			}
		}
		if(mmioAscend(hmmio,&mmckOutinfoSubchunk,0) != MMSYSERR_NOERROR)
		{
			mmioClose(hmmio,0);
			return FALSE;
		}
	}
	return TRUE;
}

DWORD CRMP::MakeTag(HWND hWnd,const char *szFileName)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;
	HANDLE	hFile;
	DWORD	dwWritten;

	Release();
	
	hFile = CreateFile(
				szFileName,
				GENERIC_READ,
				FILE_SHARE_READ,
				NULL,
				OPEN_EXISTING,	//指定ファイルが存在していない場合、関数は失敗します。
				FILE_ATTRIBUTE_NORMAL,
				NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}

	DWORD dwDataSize = GetFileSize(hFile,NULL);
	//バッファメモリの確保
	char *pRawData = (char *)malloc(dwDataSize);
	if(!pRawData)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		return dwWin32errorCode;
	}
	//raw dataの読み出し
	if(!ReadFile(hFile,pRawData,dwDataSize,&dwWritten,NULL))
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		free(pRawData);
		return dwWin32errorCode;
	}
	//ID3TAGv1情報の保存
	char id3tag[128];
	if(SetFilePointer(hFile,-128,NULL,FILE_END) == INVALID_SET_FILE_POINTER)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		free(pRawData);
		return dwWin32errorCode;
	}
	if(	ReadFile(hFile,id3tag,sizeof(id3tag),&dwWritten,NULL) &&
		(strncmp("TAG",id3tag,3) == 0) &&
		(dwWritten == sizeof(id3tag)) )
	{
		m_bHasId3tag = TRUE;
		dwDataSize -= sizeof(id3tag);
	}
	else
	{
		m_bHasId3tag = FALSE;
	}

	CloseHandle(hFile);

	//テンポラリ名を取得
	char szTempPath[MAX_PATH];
	strcpy(szTempPath,szFileName);
	cutFileName(szTempPath);
	char szTempFile[MAX_PATH];
	if(!GetTempFileName(szTempPath,"tms",0,szTempFile))
	{
		dwWin32errorCode = GetLastError();
		free(pRawData);
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}
	//ファイルオープン(指定ファイルがすでに存在している場合、そのファイルは上書きされます。)
	hFile = CreateFile(szTempFile,GENERIC_WRITE|GENERIC_READ,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		dwWin32errorCode = GetLastError();
		free(pRawData);
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}
	if(SetFilePointer(hFile,0,NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		free(pRawData);
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}
	CloseHandle(hFile);

	// バッファ付きI/Oを使ってテンポラリファイルを開く(他のプロセスからの書込禁止)
	//(テンポラリにRiff形式のmp3ファイルを書く)
	HMMIO hmmio = mmioOpen(szTempFile,NULL,MMIO_CREATE | MMIO_WRITE | MMIO_ALLOCBUF | MMIO_DENYWRITE);
	if(!hmmio)
	{
		free(pRawData);
		DeleteFile(szTempFile);
		return -1;
	}

	//RMP3チャンクを作成
	MMCKINFO	mmckOutinfoParent;
	memset(&mmckOutinfoParent,0,sizeof(mmckOutinfoParent));
	mmckOutinfoParent.fccType = mmioFOURCC('R','M','P','3');
	mmckOutinfoParent.cksize = 0L;
	if(mmioCreateChunk(hmmio,&mmckOutinfoParent,MMIO_CREATERIFF) != MMSYSERR_NOERROR)
	{
		free(pRawData);
		mmioClose(hmmio,0);
		DeleteFile(szTempFile);
		return -1;
	}
	mmckOutinfoParent.dwFlags = MMIO_DIRTY;//サイズ変更可に設定
	//dataチャンクを作成
	MMCKINFO	mmckOutinfoSubchunk;
	memset(&mmckOutinfoSubchunk,0,sizeof(mmckOutinfoSubchunk));
	mmckOutinfoSubchunk.ckid = mmioFOURCC('d','a','t','a');
	mmckOutinfoSubchunk.cksize = 0L;
	if(mmioCreateChunk(hmmio,&mmckOutinfoSubchunk,0) != MMSYSERR_NOERROR)
	{
		free(pRawData);
		mmioClose(hmmio,0);
		DeleteFile(szTempFile);
		return -1;
	}
	mmckOutinfoSubchunk.dwFlags = MMIO_DIRTY;//サイズ変更可に設定
	//dataチャンクへ書き込み
	if(mmioWrite(hmmio,pRawData,dwDataSize) == -1)
	{
		free(pRawData);
		mmioClose(hmmio,0);
		DeleteFile(szTempFile);
		return -1;
	}
	free(pRawData);
	//dataチャンクを確定
	if(mmioAscend(hmmio,&mmckOutinfoSubchunk,0) != MMSYSERR_NOERROR)
	{
		mmioClose(hmmio,0);
		DeleteFile(szTempFile);
		return -1;
	}
	//INFOチャンク作成・移動
	memset(&mmckOutinfoSubchunk,0,sizeof(mmckOutinfoSubchunk));
	mmckOutinfoSubchunk.fccType = mmioFOURCC('I','N','F','O');
	mmckOutinfoSubchunk.cksize = 0L;
	if(mmioCreateChunk(hmmio,&mmckOutinfoSubchunk,MMIO_CREATELIST) != 0)
	{
		mmioClose(hmmio,0);
		DeleteFile(szTempFile);
		return -1;
	}

	m_strSFT = m_strDefaultSft;
	//INAM songname
	if(m_strNAM.GetLength() == 0)
	{
		//デフォルト曲名(ファイル名)
		char szDefaultName[MAX_PATH];
		strcpy(szDefaultName,getFileName(szFileName));
		cutExtName(szDefaultName);
		m_strNAM = szDefaultName;
	}
	WriteChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','N','A','M'),(LPCSTR )m_strNAM,m_strNAM.GetLength()+1);
	//IART アーティスト名
	WriteChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','A','R','T'),(LPCSTR )m_strART,m_strART.GetLength()+1);
	//IPRD アルバム名
	WriteChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','P','R','D'),(LPCSTR )m_strPRD,m_strPRD.GetLength()+1);
	//ICMT コメント
	WriteChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','C','M','T'),(LPCSTR )m_strCMT,m_strCMT.GetLength()+1);
	//ICRD 日付
	WriteChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','C','R','D'),(LPCSTR )m_strCRD,m_strCRD.GetLength()+1);
	//IGNR ジャンル
	WriteChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','G','N','R'),(LPCSTR )m_strGNR,m_strGNR.GetLength()+1);
	//ICOP 著作権
	WriteChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','C','O','P'),(LPCSTR )m_strCOP,m_strCOP.GetLength()+1);
	//IENG エンジニア
	WriteChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','E','N','G'),(LPCSTR )m_strENG,m_strENG.GetLength()+1);
	//ISRC ソース
	WriteChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','S','R','C'),(LPCSTR )m_strSRC,m_strSRC.GetLength()+1);
	//ISFT ソフトウェア
	WriteChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','S','F','T'),(LPCSTR )m_strSFT,m_strSFT.GetLength()+1);
	//IKEY キーワード
	WriteChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','K','E','Y'),(LPCSTR )m_strKEY,m_strKEY.GetLength()+1);
	//ITCH 技術者
	WriteChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','T','C','H'),(LPCSTR )m_strTCH,m_strTCH.GetLength()+1);
	//ILYC 歌詞
	WriteChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','L','Y','C'),(LPCSTR )m_strLYC,m_strLYC.GetLength()+1);
	//ICMS コミッション
	WriteChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','C','M','S'),(LPCSTR )m_strCMS,m_strCMS.GetLength()+1);
	//IMED 中間
	WriteChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','M','E','D'),(LPCSTR )m_strMED,m_strMED.GetLength()+1);
	//ISBJ subject
	WriteChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','S','B','J'),(LPCSTR )m_strSBJ,m_strSBJ.GetLength()+1);
	//IMP3 mp3 info
	WriteChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','M','P','3'),(LPCSTR )m_strMP3,m_strMP3.GetLength()+1);
	//IID3 ID3V1 TAG
	if(m_bHasId3tag)
		WriteChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','I','D','3'),(LPCSTR )id3tag,sizeof(id3tag));

	//サイズの書き込み
	if(mmioAscend(hmmio,&mmckOutinfoSubchunk,0) != MMSYSERR_NOERROR)
	{
		mmioClose(hmmio,0);
		DeleteFile(szTempFile);
		return -1;
	}
	if(mmioAscend(hmmio,&mmckOutinfoParent,0) != MMSYSERR_NOERROR)
	{
		mmioClose(hmmio,0);
		DeleteFile(szTempFile);
		return -1;
	}

	if(mmioClose(hmmio,0))
	{	//作製失敗したとき
		DeleteFile(szTempFile);
		return -1;
	}

	//オリジナルファイルを退避(リネーム)
	char szPreFile[MAX_PATH];
	if(!GetTempFileName(szTempPath,"tms",0,szPreFile))
	{
		dwWin32errorCode = GetLastError();
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}
	DeleteFile(szPreFile);//手抜き(^^;
	if(!MoveFile(szFileName,szPreFile))
	{
		dwWin32errorCode = GetLastError();
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}

	//完成品をリネーム
	if(!MoveFile(szTempFile,szFileName))
	{
		dwWin32errorCode = GetLastError();
		MoveFile(szPreFile,szFileName);
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}

	//オリジナルを削除
	DeleteFile(szPreFile);

	m_bEnable = TRUE;

	return dwWin32errorCode;
}
