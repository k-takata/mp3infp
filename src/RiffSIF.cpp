// RiffSIF.cpp: CRiffSIF クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"		// メイン シンボル
#include "GlobalCommand.h"
#include "RiffSIF.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CRiffSIF::CRiffSIF()
{
	Release();
}

CRiffSIF::~CRiffSIF()
{

}

void CRiffSIF::Release()
{
	m_dwStreamSize = 0;
	m_bEnable = FALSE;
	m_fields.clear();
}

BOOL CRiffSIF::SetField(char id1,char id2,char id3,char id4,const char *szData)
{
	m_fields.erase(mmioFOURCC(id1,id2,id3,id4));
	if(strlen(szData))
	{
		m_fields.insert(pair<FOURCC,CString>(mmioFOURCC(id1,id2,id3,id4),szData));
	}
	return TRUE;
}

CString CRiffSIF::GetField(char id1,char id2,char id3,char id4)
{
	map<FOURCC,CString>::iterator p;
	p = m_fields.find(mmioFOURCC(id1,id2,id3,id4));
	if(p == m_fields.end())
	{
		return "";
	}
	return (LPCSTR )p->second;
}

DWORD CRiffSIF::GetTotalFieldSize()
{
	DWORD dwSize = 0;
	map<FOURCC,CString>::iterator p;

	p = m_fields.begin();
	while(p != m_fields.end())
	{
		CString *pStr = &p->second;
		DWORD len = pStr->GetLength() + 1;
		dwSize += len + (len&0x1)?1:0;	//WORD境界合わせ
		p++;
	}
	return dwSize;
}

//チャンクを検索します
//return=TRUEのときは見つかったチャンクの先頭+8の位置にいます
//return=FALSEのときは最終チャンクの最後尾+1
BOOL CRiffSIF::FindChunk(HANDLE hFile,DWORD dwFileSize,UINT flag,FOURCC type,DWORD *pdwSize,BOOL bModify)
{
	FOURCC id;
	FOURCC fType;
	DWORD dwRet;
	DWORD dwSize;
	LONG lAddressHeight;
	// 1G = 1073741824
	// 2G = 2147483648
	// 4G = 4294967296

	while(1)
	{
		DWORD dwChunkHead = SetFilePointer(hFile,0,NULL,FILE_CURRENT);
		if(!ReadFile(hFile,&id,sizeof(id),&dwRet,NULL) || (dwRet != sizeof(id)))
		{
			lAddressHeight = 0;
			SetFilePointer(hFile,dwChunkHead,&lAddressHeight,FILE_BEGIN);
			return FALSE;
		}
		if(!ReadFile(hFile,&dwSize,sizeof(dwSize),&dwRet,NULL) || (dwRet != sizeof(dwSize)))
		{
			lAddressHeight = 0;
			SetFilePointer(hFile,dwChunkHead,&lAddressHeight,FILE_BEGIN);
			return FALSE;
		}
		if(dwSize%2)
		{
			dwSize++;
		}
		*pdwSize = dwSize;
		switch(id){
		case FOURCC_RIFF:
			if(!ReadFile(hFile,&fType,sizeof(fType),&dwRet,NULL) || (dwRet != sizeof(fType)))
			{
				lAddressHeight = 0;
				SetFilePointer(hFile,dwChunkHead,&lAddressHeight,FILE_BEGIN);
				return FALSE;
			}
			*pdwSize -= sizeof(fType);
			if(flag != MMIO_FINDRIFF)
			{
				break;
			}
			if(fType == type)
			{
				return TRUE;
			}
			break;
			
		case FOURCC_LIST:
			if(!ReadFile(hFile,&fType,sizeof(fType),&dwRet,NULL) || (dwRet != sizeof(fType)))
			{
				lAddressHeight = 0;
				SetFilePointer(hFile,dwChunkHead,&lAddressHeight,FILE_BEGIN);
				return FALSE;
			}
			*pdwSize -= sizeof(fType);
			if(flag != MMIO_FINDLIST)
			{
				break;
			}
			if(fType == type)
			{
				return TRUE;
			}
			break;
			
		default:
			if(id == type)
			{
				return TRUE;
			}
			break;
		}
		TRACE("%c%c%c%c %c%c%c%c %lu(%lu)\n",
			(((char *)(&id))[0]),(((char *)(&id))[1]),(((char *)(&id))[2]),(((char *)(&id))[3]),
			(((char *)(&fType))[0]),(((char *)(&fType))[1]),(((char *)(&fType))[2]),(((char *)(&fType))[3]),
			dwChunkHead,dwSize
			);
		if(bModify)
		{
			if((dwChunkHead+8+dwSize) > dwFileSize)
			{
				//最後のチャンクサイズが不正（修正する）
				// 実際のファイルサイズよりも、チャンクサイズの終端が後にきている
				// ためにチャンクサイズをファイルサイズ分まで補正する
				dwSize -= (dwChunkHead+8+dwSize) - dwFileSize;
				lAddressHeight = 0;
				SetFilePointer(hFile,dwChunkHead+4,&lAddressHeight,FILE_BEGIN);
				DWORD dwRet;
				WriteFile(hFile,&dwSize,sizeof(dwSize),&dwRet,NULL);
				lAddressHeight = 0;
				SetFilePointer(hFile,dwSize,&lAddressHeight,FILE_CURRENT);
				return FALSE;
			}
		}
		lAddressHeight = 0;
		SetFilePointer(hFile,*pdwSize,&lAddressHeight,FILE_CURRENT);

	}
	return FALSE;
}

/*
	すべての"INAM(SIZE)::::::"の長さ
*/
DWORD CRiffSIF::GetInfoChunkSize()
{
	DWORD dwSize = 0; 
	map<FOURCC,CString>::iterator p = m_fields.begin();
	while(p != m_fields.end())
	{
		FOURCC id = p->first;
		CString *pStr = &p->second;
		DWORD len = pStr->GetLength() + 1;
		dwSize += len + 8 + ((len%2)?1:0);
		p++;
	}

	return dwSize;
}
/*
	ret:	-1 = ロード失敗
			-2 = ファイルサイズが2Gを超過している
*/
DWORD CRiffSIF::Load(const char *szFileName,char id1,char id2,char id3,char id4)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;
	HANDLE hFile;
	DWORD dwRet;
	DWORD dwTotalSize;
	DWORD dwFileSize;
	DWORD dwFileSizeHight;
	FOURCC id,type;

	DWORD dwSize;
	DWORD dwPtr;

	map<FOURCC,CString>::iterator p;

	Release();
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//ファイルを開く
	hFile = CreateFile(szFileName,
					GENERIC_READ,
					FILE_SHARE_READ,NULL,
					OPEN_EXISTING,	//ファイルを開きます。指定したファイルが存在していない場合、この関数は失敗します。 
					FILE_ATTRIBUTE_NORMAL,
					NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		dwWin32errorCode = GetLastError();
		goto exit;
	}
	
	dwFileSize = GetFileSize(hFile,&dwFileSizeHight);
	if(dwFileSizeHight & 0x80000000)
	{
		// 2Gを超えるファイルは読み取れない
		dwWin32errorCode = -2;
		goto exit;
	}

	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//RIFF-AVI を確認
	if(!ReadFile(hFile,&id,sizeof(id),&dwRet,NULL))
	{
		dwWin32errorCode = GetLastError();
		goto exit;
	}
	// 総サイズ
	if(!ReadFile(hFile,&dwTotalSize,sizeof(dwTotalSize),&dwRet,NULL))
	{
		dwWin32errorCode = GetLastError();
		goto exit;
	}
	// Riff type
	if(!ReadFile(hFile,&type,sizeof(type),&dwRet,NULL))
	{
		dwWin32errorCode = GetLastError();
		goto exit;
	}
	m_type = mmioFOURCC(id1,id2,id3,id4);
	if((FOURCC_RIFF != id) || (m_type != type))
	{
		dwWin32errorCode = GetLastError();
		goto exit;
	}
	m_bEnable = TRUE;

	dwPtr = SetFilePointer(hFile,0,NULL,FILE_CURRENT);
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//dataチャンクを検索(見つからないときはRiff論理終端に移動)
	if(FindChunk(hFile,dwFileSize,MMIO_FINDCHUNK,mmioFOURCC('d','a','t','a'),&dwSize,FALSE) && (dwSize > 8))
	{
		// (チャンクの先頭+8にいる)
		m_dwStreamSize = dwSize;
	}
	SetFilePointer(hFile,dwPtr,NULL,FILE_BEGIN);
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//LIST-INFOを検索(見つからないときはRiff論理終端に移動・チャンクサイズ修正機能付き)
	if(FindChunk(hFile,dwFileSize,MMIO_FINDLIST,mmioFOURCC('I','N','F','O'),&dwSize,FALSE) && (dwSize > 8))
	{
		// (チャンクの先頭+8にいる)
		
		//全ての情報を取り込む
		char *buf = (char *)malloc(dwSize);
		if(buf == NULL)
		{
			dwWin32errorCode = -1;
			goto exit;
		}
		if(!ReadFile(hFile,buf,dwSize,&dwRet,NULL))
		{
			dwWin32errorCode = GetLastError();
			free(buf);
			goto exit;
		}
		
		DWORD dwRemainSize = dwSize;
		while(dwRemainSize > 0)
		{
			if(dwRemainSize < 8)
			{
				break;	//用心
			}
			FOURCC id =	mmioFOURCC(
								buf[dwSize-dwRemainSize],
								buf[dwSize-dwRemainSize+1],
								buf[dwSize-dwRemainSize+2],
								buf[dwSize-dwRemainSize+3]
								);
			DWORD size = *(DWORD *)&buf[dwSize-dwRemainSize+4];
			char *data = (char *)&buf[dwSize-dwRemainSize+8];
			if(dwRemainSize < (8+size))
			{
				break;	//用心
			}
			//mapに追加
			if(data[size-1] == '\0')
			{
				m_fields.insert(pair<FOURCC,CString>(id,data));
			}
			else
			{
				m_fields.insert(pair<FOURCC,CString>(id,CString(data,size)));
			}
			if(dwRemainSize <= (size+8))
			{
				break;
			}
			dwRemainSize -= size + 8;
			if(dwRemainSize & 0x01)
			{
				dwRemainSize--;	//ワード境界合わせ
			}
		}
		free(buf);
	}

exit:

	if(hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
	}

	return dwWin32errorCode;
}


/*
	ret:	-1 = 更新失敗
			-2 = ファイルサイズが2Gを超過している
*/
DWORD CRiffSIF::Save(HWND hWnd,const char *szFileName)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;
	HANDLE hFile;
	DWORD dwRet;
//	LONG lAddressHeight;
	DWORD dwTotalSize;
	DWORD dwFileSize;
	DWORD dwFileSizeHight;
	DWORD dwInfoSize;
	FOURCC id,type;

	DWORD dwSize;
	DWORD dwOffset;
	DWORD dwListInfoHead;

	map<FOURCC,CString>::iterator p;

	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//ファイルを開く
	hFile = CreateFile(szFileName,
					GENERIC_READ|GENERIC_WRITE,
					FILE_SHARE_READ,NULL,
					OPEN_EXISTING,	//ファイルを開きます。指定したファイルが存在していない場合、この関数は失敗します。 
					FILE_ATTRIBUTE_NORMAL,
					NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		dwWin32errorCode = GetLastError();
		goto exit;
	}
	
	dwFileSize = GetFileSize(hFile,&dwFileSizeHight);
	if(dwFileSizeHight & 0x80000000)
	{
		// 2Gを超えるファイルは編集できない
		dwWin32errorCode = -2;
		goto exit;
	}

	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//RIFF-AVI を確認
	if(!ReadFile(hFile,&id,sizeof(id),&dwRet,NULL))
	{
		dwWin32errorCode = GetLastError();
		goto exit;
	}
	// 総サイズ
	if(!ReadFile(hFile,&dwTotalSize,sizeof(dwTotalSize),&dwRet,NULL))
	{
		dwWin32errorCode = GetLastError();
		goto exit;
	}
	// Riff type
	if(!ReadFile(hFile,&type,sizeof(type),&dwRet,NULL))
	{
		dwWin32errorCode = GetLastError();
		goto exit;
	}
	if((FOURCC_RIFF != id) && (m_type != type))
	{
		dwWin32errorCode = GetLastError();
		goto exit;
	}

	while(1)
	{
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		//LIST-INFOを検索(見つからないときはRiff論理終端に移動・チャンクサイズ修正機能付き)
		if(FindChunk(hFile,dwFileSize,MMIO_FINDLIST,mmioFOURCC('I','N','F','O'),&dwSize,TRUE))
		{
			// (チャンクの先頭+8にいる)

			dwOffset = SetFilePointer(hFile,0,NULL,FILE_CURRENT);
			if((dwOffset + dwSize) < dwFileSize)
			{
				//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
				//LIST-INFOが最後尾に付いていないときは、現存LIST-INFOをJUNKに置換する
				SetFilePointer(hFile,-12,NULL,FILE_CURRENT);
				id = mmioFOURCC('J','U','N','K');
				WriteFile(hFile,&id,sizeof(id),&dwRet,NULL);
				
				//先頭チャンクに戻る
				SetFilePointer(hFile,12,NULL,FILE_BEGIN);
				continue;
			}
			else
			{
				//LIST-INFOの先頭に移動
				SetFilePointer(hFile,-12,NULL,FILE_CURRENT);
			}
		}
		break;
	}

	// 付加するLIST-INFOのサイズを計算
	dwInfoSize = GetInfoChunkSize();
	if(dwInfoSize == 0)
	{
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		//LIST-INFOを削除
		if(!SetEndOfFile(hFile))
		{
			dwWin32errorCode = GetLastError();
		}
		goto exit;
	}
	dwInfoSize += 12;
	dwListInfoHead = SetFilePointer(hFile,0,NULL,FILE_CURRENT);
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//2G overチェック
	if((dwListInfoHead + dwInfoSize) & 0x80000000)
	{
		dwWin32errorCode = -2;
		goto exit;
	}

	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//ファイルサイズを修正(LIST-INFO込みに)
	SetFilePointer(hFile,dwInfoSize,NULL,FILE_CURRENT);
	if(!SetEndOfFile(hFile))
	{
		dwWin32errorCode = GetLastError();
		goto exit;
	}

	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//LIST-INFOを終端に追加
	SetFilePointer(hFile,-dwInfoSize,NULL,FILE_CURRENT);
	dwSize = dwInfoSize - 8;
	id = FOURCC_LIST;
	type = mmioFOURCC('I','N','F','O');
	WriteFile(hFile,&id,sizeof(id),&dwRet,NULL);
	WriteFile(hFile,&dwSize,sizeof(dwSize),&dwRet,NULL);
	WriteFile(hFile,&type,sizeof(type),&dwRet,NULL);
	//全てのフィールド情報を保存
	p = m_fields.begin();
	while(p != m_fields.end())
	{
		FOURCC id = p->first;
		CString *pStr = &p->second;
		DWORD len = pStr->GetLength() + 1;
		if(len > 1)
		{
			WriteFile(hFile,&id,sizeof(id),&dwRet,NULL);
			WriteFile(hFile,&len,sizeof(len),&dwRet,NULL);
			WriteFile(hFile,*pStr,len,&dwRet,NULL);
			if(len%2)
			{
				WriteFile(hFile,"\x00",1,&dwRet,NULL);
			}
		}
		p++;
	}

	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//RIFF-AVI チャンクサイズを修正
	dwSize = dwListInfoHead + dwInfoSize - 8;
	SetFilePointer(hFile,4,NULL,FILE_BEGIN);
	WriteFile(hFile,&dwSize,sizeof(dwSize),&dwRet,NULL);

exit:

	if(hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
	}

	return dwWin32errorCode;
}
