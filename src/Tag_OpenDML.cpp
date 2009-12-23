// Tag_OpenDML.cpp: CTag_OpenDML �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"		// ���C�� �V���{��
#include "GlobalCommand.h"
#include "Tag_OpenDML.h"
//#include <io.h>

//#include <Mmsystem.h>
//#pragma comment(lib,"winmm.lib")

//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////

CTag_OpenDML::CTag_OpenDML()
{
	Release();
}

CTag_OpenDML::~CTag_OpenDML()
{

}

void CTag_OpenDML::Release()
{
	m_dwStreamSize = 0;
	m_bEnable = FALSE;
	m_fields.clear();
}

BOOL CTag_OpenDML::SetField(char id1,char id2,char id3,char id4,const char *szData)
{
	m_fields.erase(mmioFOURCC(id1,id2,id3,id4));
	if(strlen(szData))
	{
		m_fields.insert(pair<FOURCC,CString>(mmioFOURCC(id1,id2,id3,id4),szData));
	}
	return TRUE;
}

CString CTag_OpenDML::GetField(char id1,char id2,char id3,char id4)
{
	map<FOURCC,CString>::iterator p;
	p = m_fields.find(mmioFOURCC(id1,id2,id3,id4));
	if(p == m_fields.end())
	{
		return "";
	}
	return (LPCSTR )p->second;
}

DWORD CTag_OpenDML::GetTotalFieldSize()
{
	DWORD dwSize = 0;
	map<FOURCC,CString>::iterator p;

	p = m_fields.begin();
	while(p != m_fields.end())
	{
		CString *pStr = &p->second;
		DWORD len = pStr->GetLength() + 1;
		dwSize += len + (len&0x1)?1:0;	//WORD���E���킹
		p++;
	}
	return dwSize;
}

DWORD CTag_OpenDML::FindJUNK_LISTINFO(HANDLE hFile,__int64 llFileSize)
{
	DWORD dwMax = 0;
	DWORD dwJunkSize = 0;
	FOURCC id;
	FOURCC fType;
	DWORD dwRet;
	DWORD dwSize;
	// 1G = 1073741824
	// 2G = 2147483648
	// 4G = 4294967296

	while(1)
	{
		__int64 llChunkHead = SetFilePointer64(hFile,0,FILE_CURRENT);
		if(llChunkHead >= llFileSize)
		{
			if(dwJunkSize)
			{
				if(dwMax < dwJunkSize)
				{
					dwMax = dwJunkSize;
				}
				dwJunkSize = 0;
			}
			break;
		}
		if(!ReadFile(hFile,&id,sizeof(id),&dwRet,NULL) || (dwRet != sizeof(id)))
		{
			SetFilePointer64(hFile,llChunkHead,FILE_BEGIN);
			return FALSE;
		}
		if(!ReadFile(hFile,&dwSize,sizeof(dwSize),&dwRet,NULL) || (dwRet != sizeof(dwSize)))
		{
			SetFilePointer64(hFile,llChunkHead,FILE_BEGIN);
			return FALSE;
		}
		if(dwSize%2)
		{
			dwSize++;
		}
		switch(id){
		case FOURCC_RIFF:
			if(dwJunkSize)
			{
				if(dwMax < dwJunkSize)
				{
					dwMax = dwJunkSize;
				}
				dwJunkSize = 0;
			}
			break;
			
		case FOURCC_LIST:
			if(!ReadFile(hFile,&fType,sizeof(fType),&dwRet,NULL) || (dwRet != sizeof(fType)))
			{
				SetFilePointer64(hFile,llChunkHead,FILE_BEGIN);
				return FALSE;
			}
			if(fType == mmioFOURCC('I','N','F','O'))
			{
				dwJunkSize += dwSize + 8;
			}
			else
			{
				if(dwJunkSize)
				{
					if(dwMax < dwJunkSize)
					{
						dwMax = dwJunkSize;
					}
					dwJunkSize = 0;
				}
			}
			dwSize -= 4;
			break;
			
		default:
			if(id == mmioFOURCC('J','U','N','K'))
			{
				dwJunkSize += dwSize + 8;
			}
			else
			{
				if(dwJunkSize)
				{
					if(dwMax < dwJunkSize)
					{
						dwMax = dwJunkSize;
					}
					dwJunkSize = 0;
				}
			}
			break;
		}
		TRACE("%c%c%c%c %c%c%c%c %I64u(%lu)\n",
			(((char *)(&id))[0]),(((char *)(&id))[1]),(((char *)(&id))[2]),(((char *)(&id))[3]),
			(((char *)(&fType))[0]),(((char *)(&fType))[1]),(((char *)(&fType))[2]),(((char *)(&fType))[3]),
			llChunkHead,dwSize
			);
		SetFilePointer64(hFile,(__int64 )dwSize,FILE_CURRENT);

	}
	return dwMax;
}

//�`�����N���������܂�
//return=TRUE�̂Ƃ��͌��������`�����N�̐擪+8�̈ʒu�ɂ��܂�
//return=FALSE�̂Ƃ��͍ŏI�`�����N�̍Ō��+1
BOOL CTag_OpenDML::FindChunk(HANDLE hFile,__int64 llFileSize,UINT flag,FOURCC type,DWORD *pdwSize)
{
	FOURCC id;
	FOURCC fType;
	DWORD dwRet;
	DWORD dwSize;
	// 1G = 1073741824
	// 2G = 2147483648
	// 4G = 4294967296

	while(1)
	{
		__int64 llChunkHead = SetFilePointer64(hFile,0,FILE_CURRENT);
		if(llChunkHead >= llFileSize)
		{
			break;
		}
		if(!ReadFile(hFile,&id,sizeof(id),&dwRet,NULL) || (dwRet != sizeof(id)))
		{
			SetFilePointer64(hFile,llChunkHead,FILE_BEGIN);
			return FALSE;
		}
		if(!ReadFile(hFile,&dwSize,sizeof(dwSize),&dwRet,NULL) || (dwRet != sizeof(dwSize)))
		{
			SetFilePointer64(hFile,llChunkHead,FILE_BEGIN);
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
				SetFilePointer64(hFile,llChunkHead,FILE_BEGIN);
				return FALSE;
			}
			*pdwSize -= sizeof(fType);
			if(flag != MMIO_FINDRIFF)
			{
				break;
			}
			if(fType == type)
			{
				// pdwSize�̕␳(���肦�Ȃ��l��Ԃ��Ȃ��悤��)
				__int64 ptr = llChunkHead + 12;
				if((llFileSize - ptr) < *pdwSize)
				{
					*pdwSize = llFileSize - ptr;
				}
				return TRUE;
			}
			break;
			
		case FOURCC_LIST:
			if(!ReadFile(hFile,&fType,sizeof(fType),&dwRet,NULL) || (dwRet != sizeof(fType)))
			{
				SetFilePointer64(hFile,llChunkHead,FILE_BEGIN);
				return FALSE;
			}
			*pdwSize -= sizeof(fType);
			if(flag != MMIO_FINDLIST)
			{
				break;
			}
			if(fType == type)
			{
				// pdwSize�̕␳(���肦�Ȃ��l��Ԃ��Ȃ��悤��)
				__int64 ptr = llChunkHead + 12;
				if((llFileSize - ptr) < *pdwSize)
				{
					*pdwSize = llFileSize - ptr;
				}
				return TRUE;
			}
			break;
			
		default:
			fType = 0x20202020;
			if(id == type)
			{
				// pdwSize�̕␳(���肦�Ȃ��l��Ԃ��Ȃ��悤��)
				__int64 ptr = llChunkHead + 8;
				if((llFileSize - ptr) < *pdwSize)
				{
					*pdwSize = llFileSize - ptr;
				}
				return TRUE;
			}
			break;
		}
		TRACE("%c%c%c%c %c%c%c%c %I64u(%lu)\n",
			(((char *)(&id))[0]),(((char *)(&id))[1]),(((char *)(&id))[2]),(((char *)(&id))[3]),
			(((char *)(&fType))[0]),(((char *)(&fType))[1]),(((char *)(&fType))[2]),(((char *)(&fType))[3]),
			llChunkHead,dwSize
			);
		SetFilePointer64(hFile,(__int64 )*pdwSize,FILE_CURRENT);

	}
	return FALSE;
}

/*
	���ׂĂ�"INAM(SIZE)::::::"�̒���
*/
DWORD CTag_OpenDML::GetInfoChunkSize()
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
	ret:	-1 = ���[�h���s
*/
DWORD CTag_OpenDML::Load(const char *szFileName,char id1,char id2,char id3,char id4)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;
	HANDLE hFile;
	DWORD dwRet;
	DWORD dwRiffAviSize;
	__int64 llFileSize;
//	__int64 llLastRiffPtr;
	FOURCC id,type;

	DWORD dwSize;

	Release();
	map<FOURCC,CString>::iterator p;
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//�t�@�C�����J��
	hFile = CreateFile(szFileName,
					GENERIC_READ,
					FILE_SHARE_READ,NULL,
					OPEN_EXISTING,	//�t�@�C�����J���܂��B�w�肵���t�@�C�������݂��Ă��Ȃ��ꍇ�A���̊֐��͎��s���܂��B 
					FILE_ATTRIBUTE_NORMAL,
					NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		dwWin32errorCode = GetLastError();
		goto exit;
	}
	
	llFileSize = GetFileSize64(hFile);

	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//RIFF-AVI ���m�F
	if(!ReadFile(hFile,&id,sizeof(id),&dwRet,NULL))
	{
		dwWin32errorCode = GetLastError();
		goto exit;
	}
	// ���T�C�Y
	if(!ReadFile(hFile,&dwRiffAviSize,sizeof(dwRiffAviSize),&dwRet,NULL))
	{
		dwWin32errorCode = GetLastError();
		goto exit;
	}
	// (�O�̂���)
	if((dwRiffAviSize+8) > llFileSize)
	{
		dwRiffAviSize = (llFileSize & 0x000000007fffffff) - 8;
		if(dwRiffAviSize <= 0)
		{
			goto exit;
		}
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
OutputDebugString("CTag_OpenDML::Load m_bEnable = TRUE\n");
	TRACE("%s Len=%I64d(%08I64x) RiffLen=%lu(%08x)\n",szFileName,llFileSize,llFileSize,dwRiffAviSize,dwRiffAviSize);

	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//LIST-INFO������(������Ȃ��Ƃ���Riff�_���I�[�Ɉړ�)
	if(FindChunk(hFile,dwRiffAviSize + 8,MMIO_FINDLIST,mmioFOURCC('I','N','F','O'),&dwSize) && (dwSize > 8))
	{
		// (�`�����N�̐擪+8�ɂ���)
		
		//�S�Ă̏�����荞��
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
				break;	//�p�S
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
				break;	//�p�S
			}
			//map�ɒǉ�
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
				dwRemainSize--;	//���[�h���E���킹
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


DWORD CTag_OpenDML::Save_1(HANDLE hFile)
{
	if(hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
	}
	return -2;
}

/*
	ret:	-1 = �X�V���s
			-2 = Avi2�̂��ߍX�V�ł����iavi2��riff-avix�̏��������͖������Ȃ��߁j
*/
DWORD CTag_OpenDML::Save(HWND hWnd,const char *szFileName)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;
	HANDLE hFile;
	DWORD dwRet;
//	LONG lAddressHeight;
	DWORD dwRiffAviSize;
	__int64 llFileSize;
	__int64 llLastJUNKPtr;
	DWORD dwLastJUNKSize;
	__int64 llLargeJUNKPtr;
	DWORD dwLargeJunkSize;
	DWORD dwInfoSize;
	FOURCC id,type;
	BOOL bFindJUNK;
	int i;

	DWORD dwSize;
	__int64 llOffset;

	map<FOURCC,CString>::iterator p;

	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//�t�@�C�����J��
	hFile = CreateFile(szFileName,
					GENERIC_READ|GENERIC_WRITE,
					FILE_SHARE_READ,NULL,
					OPEN_EXISTING,	//�t�@�C�����J���܂��B�w�肵���t�@�C�������݂��Ă��Ȃ��ꍇ�A���̊֐��͎��s���܂��B 
					FILE_ATTRIBUTE_NORMAL,
					NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		dwWin32errorCode = GetLastError();
		goto exit;
	}
	
	llFileSize = GetFileSize64(hFile);

	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//RIFF-AVI ���m�F
	if(!ReadFile(hFile,&id,sizeof(id),&dwRet,NULL))
	{
		dwWin32errorCode = GetLastError();
		goto exit;
	}
	// ���T�C�Y
	if(!ReadFile(hFile,&dwRiffAviSize,sizeof(dwRiffAviSize),&dwRet,NULL))
	{
		dwWin32errorCode = GetLastError();
		goto exit;
	}
	// (�O�̂���)
	if((dwRiffAviSize+8) > llFileSize)
	{
		dwRiffAviSize = (llFileSize & 0x000000007fffffff) - 8;
		if(dwRiffAviSize <= 0)
		{
			goto exit;
		}
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

	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	// LIST-INFO���������߂�X�y�[�X���v�Z
	dwLargeJunkSize = FindJUNK_LISTINFO(hFile,(__int64 )dwRiffAviSize);
		TRACE("FindJUNK_LISTINFO=%d\n",dwLargeJunkSize);

	// �t������LIST-INFO�̃T�C�Y���v�Z
	dwInfoSize = GetInfoChunkSize();
	dwInfoSize += 12;

	if((dwLargeJunkSize < (dwInfoSize+8/*JUNK____*/)) &&
		!(dwLargeJunkSize == dwInfoSize)	)
	{
		// LIST-INFO���I�[�ɒǉ��\���H
		//�擪�`�����N�ɖ߂�
		SetFilePointer64(hFile,0,FILE_BEGIN);
		if(FindChunk(hFile,llFileSize,MMIO_FINDRIFF,mmioFOURCC('A','V','I','X'),&dwSize))
		{
			// AVIX����ɑ����ꍇ�̓t�@�C���̍�蒼�����K�v
			return Save_1(hFile);
		}
	}

	//�擪�`�����N�ɖ߂�(���ׂĂ�LIST-INFO������)
	SetFilePointer64(hFile,12,FILE_BEGIN);
	while(1)
	{
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		//LIST-INFO������(������Ȃ��Ƃ���Riff�_���I�[�Ɉړ�)
		if(FindChunk(hFile,dwRiffAviSize + 8,MMIO_FINDLIST,mmioFOURCC('I','N','F','O'),&dwSize))
		{
			// (�`�����N�̐擪+8�ɂ���)
			llOffset = SetFilePointer64(hFile,0,FILE_CURRENT);
			if((llOffset + dwSize) < llFileSize)
			{
				//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
				//LIST-INFO���Ō���ɕt���Ă��Ȃ��Ƃ��́A����LIST-INFO��JUNK�ɒu������
				SetFilePointer64(hFile,-12,FILE_CURRENT);
				id = mmioFOURCC('J','U','N','K');
				WriteFile(hFile,&id,sizeof(id),&dwRet,NULL);
				
				//�擪�`�����N�ɖ߂�(���ׂĂ�LIST-INFO������)
				SetFilePointer64(hFile,12,FILE_BEGIN);
				continue;
			}
			else
			{
				//LIST-INFO�̐擪�Ɉړ�
				SetFilePointer64(hFile,-12,FILE_CURRENT);
				//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
				//LIST-INFO���폜
				SetEndOfFile(hFile);
				
				dwRiffAviSize = llOffset - 12 - 8;
				//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
				//RIFF-AVI �`�����N�T�C�Y���C��
				SetFilePointer(hFile,4,NULL,FILE_BEGIN);
				WriteFile(hFile,&dwRiffAviSize,sizeof(dwRiffAviSize),&dwRet,NULL);

				SetFilePointer64(hFile,dwRiffAviSize+8,FILE_BEGIN);
			}
		}
		break;
	}

	if(dwInfoSize == 12)
	{
		// �ۑ�����f�[�^�͂Ȃ�
		goto exit;
	}

	//�擪�`�����N�ɖ߂�(���ׂĂ�LIST-INFO������)
	SetFilePointer64(hFile,12,FILE_BEGIN);
	bFindJUNK = FALSE;
	dwLastJUNKSize = 0;
	dwLargeJunkSize = 0;
	while(1)
	{
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// JUNK�𓝍�
		if(FindChunk(hFile,llFileSize,MMIO_FINDCHUNK,mmioFOURCC('J','U','N','K'),&dwSize))
		{
			// (�`�����N�̐擪+8�ɂ���)
			if(bFindJUNK)
			{
				__int64 llNowPtr = SetFilePointer64(hFile,0,FILE_CURRENT) - 8;
				// ���O��JUNK�Ɠ����\���H
				if(llNowPtr == (llLastJUNKPtr + dwLastJUNKSize))
				{
					// ����
					SetFilePointer64(hFile,llLastJUNKPtr+4,FILE_BEGIN);
					dwLastJUNKSize += dwSize + 8;
					dwSize = dwLastJUNKSize - 8;
					WriteFile(hFile,&dwSize,sizeof(dwSize),&dwRet,NULL);
					for(int i=0; i<dwSize; i++)
					{
						if(m_strJunkHeader.GetLength() > i)
						{
							WriteFile(hFile,((char *)(LPCSTR )m_strJunkHeader)+i,1,&dwRet,NULL);
						}
						else
						{
							WriteFile(hFile,"",1,&dwRet,NULL);
						}
					}
				}
				else
				{
					SetFilePointer64(hFile,dwSize,FILE_CURRENT);
					bFindJUNK = FALSE;
				}
			}
			else
			{
				bFindJUNK = TRUE;
				llLastJUNKPtr = SetFilePointer64(hFile,0,FILE_CURRENT) - 8;
				dwLastJUNKSize = dwSize + 8;
				SetFilePointer64(hFile,dwSize,FILE_CURRENT);
			}
			if(dwLargeJunkSize < dwLastJUNKSize)
			{
				llLargeJUNKPtr = llLastJUNKPtr;
				dwLargeJunkSize = dwLastJUNKSize;
			}
			continue;
		}
		break;
	}

	// LIST-INFO��JUNK�̒��Ɏ��܂邩�v�Z
	if((dwLargeJunkSize < (dwInfoSize+8/*JUNK____*/)) &&
		!(dwLargeJunkSize == dwInfoSize)	)
	{
		// (���܂�Ȃ�)
		// LIST-INFO���I�[�ɒǉ�
		llLargeJUNKPtr = dwRiffAviSize+8;
		
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		//RIFF-AVI �`�����N�T�C�Y���C��
		dwRiffAviSize += dwInfoSize;
		SetFilePointer(hFile,4,NULL,FILE_BEGIN);
		WriteFile(hFile,&dwRiffAviSize,sizeof(dwRiffAviSize),&dwRet,NULL);
	}

	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//LIST-INFO����������
	SetFilePointer64(hFile,llLargeJUNKPtr,FILE_BEGIN);
	dwSize = dwInfoSize - 8;
	id = FOURCC_LIST;
	type = mmioFOURCC('I','N','F','O');
	WriteFile(hFile,&id,sizeof(id),&dwRet,NULL);
	WriteFile(hFile,&dwSize,sizeof(dwSize),&dwRet,NULL);
	WriteFile(hFile,&type,sizeof(type),&dwRet,NULL);
	//�S�Ẵt�B�[���h����ۑ�
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

	//////////////////////////////////////
	// �c��X�y�[�X��JUNK�Ŗ��߂�
		TRACE("dwLargeJunkSize=%d\n",dwLargeJunkSize);
		TRACE("dwInfoSize=%d\n",dwInfoSize);
	if(dwLargeJunkSize >= (dwInfoSize+8/*JUNK____*/))
	{
		dwSize = dwLargeJunkSize-dwInfoSize-8;
		id = mmioFOURCC('J','U','N','K');
		WriteFile(hFile,&id,sizeof(id),&dwRet,NULL);
		WriteFile(hFile,&dwSize,sizeof(dwSize),&dwRet,NULL);
		TRACE("Start dwSize=%d\n",dwSize);
		for(i=0; i<dwSize; i++)
		{
			if(m_strJunkHeader.GetLength() > i)
			{
				WriteFile(hFile,((char *)(LPCSTR )m_strJunkHeader)+i,1,&dwRet,NULL);
			}
			else
			{
				WriteFile(hFile,"",1,&dwRet,NULL);
			}
		}
		TRACE("End dwSize=%d\n",dwSize);
	}

exit:

	if(hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
	}

	return dwWin32errorCode;
}
