// RiffSIF.cpp: CRiffSIF �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"		// ���C�� �V���{��
#include "GlobalCommand.h"
#include "RiffSIF.h"

//////////////////////////////////////////////////////////////////////
// �\�z/����
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

BOOL CRiffSIF::SetField(char id1,char id2,char id3,char id4,LPCTSTR szData)
{
	m_fields.erase(mmioFOURCC(id1,id2,id3,id4));
	if(lstrlen(szData))
	{
		m_fields.insert(std::pair<FOURCC,CString>(mmioFOURCC(id1,id2,id3,id4),szData));
	}
	return TRUE;
}

CString CRiffSIF::GetField(char id1,char id2,char id3,char id4)
{
	FieldMap::iterator p;
	p = m_fields.find(mmioFOURCC(id1,id2,id3,id4));
	if(p == m_fields.end())
	{
		return _T("");
	}
	return p->second;
}

DWORD CRiffSIF::GetTotalFieldSize()
{
	DWORD dwSize = 0;
	FieldMap::iterator p;

	p = m_fields.begin();
	while(p != m_fields.end())
	{
		CString *pStr = &p->second;
		DWORD len = TstrToData(*pStr, -1, NULL, 0, DTC_CODE_ANSI);
		dwSize += len + (len&0x1)?1:0;	//WORD���E���킹
		p++;
	}
	return dwSize;
}

//�`�����N���������܂�
//return=TRUE�̂Ƃ��͌��������`�����N�̐擪+8�̈ʒu�ɂ��܂�
//return=FALSE�̂Ƃ��͍ŏI�`�����N�̍Ō��+1
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
		TRACE(_T("%c%c%c%c %c%c%c%c %lu(%lu)\n"),
			(((char *)(&id))[0]),(((char *)(&id))[1]),(((char *)(&id))[2]),(((char *)(&id))[3]),
			(((char *)(&fType))[0]),(((char *)(&fType))[1]),(((char *)(&fType))[2]),(((char *)(&fType))[3]),
			dwChunkHead,dwSize
			);
		if(bModify)
		{
			if((dwChunkHead+8+dwSize) > dwFileSize)
			{
				//�Ō�̃`�����N�T�C�Y���s���i�C������j
				// ���ۂ̃t�@�C���T�C�Y�����A�`�����N�T�C�Y�̏I�[����ɂ��Ă���
				// ���߂Ƀ`�����N�T�C�Y���t�@�C���T�C�Y���܂ŕ␳����
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
	���ׂĂ�"INAM(SIZE)::::::"�̒���
*/
DWORD CRiffSIF::GetInfoChunkSize()
{
	DWORD dwSize = 0; 
	FieldMap::iterator p = m_fields.begin();
	while(p != m_fields.end())
	{
		FOURCC id = p->first;
		CString *pStr = &p->second;
		DWORD len = TstrToData(*pStr, -1, NULL, 0, DTC_CODE_ANSI);
		dwSize += len + 8 + ((len%2)?1:0);
		p++;
	}

	return dwSize;
}
/*
	ret:	-1 = ���[�h���s
			-2 = �t�@�C���T�C�Y��2G�𒴉߂��Ă���
*/
DWORD CRiffSIF::Load(LPCTSTR szFileName,char id1,char id2,char id3,char id4)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;
	HANDLE hFile;
	DWORD dwRet;
	DWORD dwTotalSize;
	__int64 llFileSize;
	FOURCC id,type;

	DWORD dwSize;
	DWORD dwPtr;

	FieldMap::iterator p;

	Release();
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
	if(llFileSize >= 0x80000000)
	{
		// 2G�𒴂���t�@�C���͓ǂݎ��Ȃ�
		dwWin32errorCode = -2;
		goto exit;
	}

	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//RIFF-AVI ���m�F
	if(!ReadFile(hFile,&id,sizeof(id),&dwRet,NULL))
	{
		dwWin32errorCode = GetLastError();
		goto exit;
	}
	// ���T�C�Y
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
	//data�`�����N������(������Ȃ��Ƃ���Riff�_���I�[�Ɉړ�)
	if(FindChunk(hFile,(DWORD)llFileSize,MMIO_FINDCHUNK,mmioFOURCC('d','a','t','a'),&dwSize,FALSE) && (dwSize > 8))
	{
		// (�`�����N�̐擪+8�ɂ���)
		m_dwStreamSize = dwSize;
	}
	SetFilePointer(hFile,dwPtr,NULL,FILE_BEGIN);
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//LIST-INFO������(������Ȃ��Ƃ���Riff�_���I�[�Ɉړ��E�`�����N�T�C�Y�C���@�\�t��)
	if(FindChunk(hFile,(DWORD)llFileSize,MMIO_FINDLIST,mmioFOURCC('I','N','F','O'),&dwSize,FALSE) && (dwSize > 8))
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
			if(size>0)
			{
				//map�ɒǉ�
				if(data[size-1] == '\0')
				{
					m_fields.insert(std::pair<FOURCC,CString>(id,data));
				}
				else
				{
					m_fields.insert(std::pair<FOURCC,CString>(id,CString(data,size)));
				}
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


/*
	ret:	-1 = �X�V���s
			-2 = �t�@�C���T�C�Y��2G�𒴉߂��Ă���
*/
DWORD CRiffSIF::Save(HWND hWnd,LPCTSTR szFileName)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;
	HANDLE hFile;
	DWORD dwRet;
//	LONG lAddressHeight;
	DWORD dwTotalSize;
	__int64 llFileSize;
	DWORD dwInfoSize;
	FOURCC id,type;

	DWORD dwSize;
	DWORD dwOffset;
	DWORD dwListInfoHead;

	FieldMap::iterator p;

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
	if(llFileSize >= 0x80000000)
	{
		// 2G�𒴂���t�@�C���͕ҏW�ł��Ȃ�
		dwWin32errorCode = -2;
		goto exit;
	}

	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//RIFF-AVI ���m�F
	if(!ReadFile(hFile,&id,sizeof(id),&dwRet,NULL))
	{
		dwWin32errorCode = GetLastError();
		goto exit;
	}
	// ���T�C�Y
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
		//LIST-INFO������(������Ȃ��Ƃ���Riff�_���I�[�Ɉړ��E�`�����N�T�C�Y�C���@�\�t��)
		if(FindChunk(hFile,(DWORD)llFileSize,MMIO_FINDLIST,mmioFOURCC('I','N','F','O'),&dwSize,TRUE))
		{
			// (�`�����N�̐擪+8�ɂ���)

			dwOffset = SetFilePointer(hFile,0,NULL,FILE_CURRENT);
			if((dwOffset + dwSize) < llFileSize)
			{
				//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
				//LIST-INFO���Ō���ɕt���Ă��Ȃ��Ƃ��́A����LIST-INFO��JUNK�ɒu������
				SetFilePointer(hFile,-12,NULL,FILE_CURRENT);
				id = mmioFOURCC('J','U','N','K');
				WriteFile(hFile,&id,sizeof(id),&dwRet,NULL);
				
				//�擪�`�����N�ɖ߂�
				SetFilePointer(hFile,12,NULL,FILE_BEGIN);
				continue;
			}
			else
			{
				//LIST-INFO�̐擪�Ɉړ�
				SetFilePointer(hFile,-12,NULL,FILE_CURRENT);
			}
		}
		break;
	}

	// �t������LIST-INFO�̃T�C�Y���v�Z
	dwInfoSize = GetInfoChunkSize();
	if(dwInfoSize == 0)
	{
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		//LIST-INFO���폜
		if(!SetEndOfFile(hFile))
		{
			dwWin32errorCode = GetLastError();
		}
		goto exit;
	}
	dwInfoSize += 12;
	dwListInfoHead = SetFilePointer(hFile,0,NULL,FILE_CURRENT);
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//2G over�`�F�b�N
	if((dwListInfoHead + dwInfoSize) & 0x80000000)
	{
		dwWin32errorCode = -2;
		goto exit;
	}

	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//�t�@�C���T�C�Y���C��(LIST-INFO���݂�)
	SetFilePointer(hFile,dwInfoSize,NULL,FILE_CURRENT);
	if(!SetEndOfFile(hFile))
	{
		dwWin32errorCode = GetLastError();
		goto exit;
	}

	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//LIST-INFO���I�[�ɒǉ�
	SetFilePointer(hFile,-(LONG)dwInfoSize,NULL,FILE_CURRENT);
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
		DWORD len;
		char *str = TstrToDataAlloc(*pStr, -1, (int*)&len, DTC_CODE_ANSI);
		if(str)
		{
			if(len > 1)
			{
				WriteFile(hFile,&id,sizeof(id),&dwRet,NULL);
				WriteFile(hFile,&len,sizeof(len),&dwRet,NULL);
				WriteFile(hFile,str,len,&dwRet,NULL);
				if(len%2)
				{
					WriteFile(hFile,"\x00",1,&dwRet,NULL);
				}
			}
			free(str);
		}
		p++;
	}

	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//RIFF-AVI �`�����N�T�C�Y���C��
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
