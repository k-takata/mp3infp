// Mp3Info.cpp: CMp3Info クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GlobalCommand.h"

#include "Mp3Info.h"

// 2003-08-18 フォーマット文字列が長くなりすぎるため簡略化
static const char *modes[4] = {"Stereo","J-Stereo","D-Channel","Mono"};
static const int tabsel_123[][3][15] = {
	   { {0,32,64,96,128,160,192,224,256,288,320,352,384,416,448},
		 {0,32,48,56, 64, 80, 96,112,128,160,192,224,256,320,384},
		 {0,32,40,48, 56, 64, 80, 96,112,128,160,192,224,256,320} },

	   { {0,32,48,56,64,80,96,112,128,144,160,176,192,224,256},
		 {0,8,16,24,32,40,48,56,64,80,96,112,128,144,160},
		 {0,8,16,24,32,40,48,56,64,80,96,112,128,144,160} }
	};
static const long freqs[9] = {44100,48000,32000,22050,24000,16000,11025,12000,8000};

#include <Mmsystem.h>
#pragma comment(lib,"winmm.lib")

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CMp3Info::CMp3Info()
{
}

CMp3Info::~CMp3Info()
{
	Release();
}

BOOL CMp3Info::Release()
{
	m_listFrame.clear();
	m_mpegInfo.bVbr = FALSE;
	m_strFormat = "unknown";
	m_strTime = "unknown";
	return TRUE;
}

//---------------------------------------------------------------------
//★mp3ヘッダの妥当性を検査
BOOL mp3head_check(unsigned long head)
{
    if((head & 0xffe00000) != 0xffe00000)
		return FALSE;
    if(!((head>>17)&3))
		return FALSE;	//layerI-III
    if(((head>>12)&0xf) == 0xf)
		return FALSE;	//bitrate
    if(((head>>10)&0x3) == 0x3)
		return FALSE;	//freq
    return TRUE;
}

BOOL CMp3Info::Load(const char *szFileName,BOOL bVbrScan)
{
	DWORD dwBeginPtr;
	unsigned char			hbuf[4];
	static unsigned long	head;
	unsigned char			xingTag[4+12];
	unsigned char			id3tag[128];
	//XING VBR ヘッダで使用
	const unsigned long FRAMES_FLAG		= 0x0001;
	const unsigned long BYTES_FLAG		= 0x0002;
	const unsigned long TOC_FLAG		= 0x0004;
	const unsigned long VBR_SCALE_FLAG	= 0x0008;

	Release();

	long lDataPtr = 0;	//mp3ストリームの開始位置
	ULONG dataSize = 0;
	//RMP形式のストリームサイズを取得する==========================
	HMMIO hmmio = mmioOpen((char *)szFileName,NULL,MMIO_COMPAT);
	if(hmmio)
	{
		//RMP3ファイルの確認
		char hdr[4];
		LONG ret = mmioRead(hmmio,hdr,sizeof(hdr));
		if((ret == 4) && (hdr[0] == 'R') && (hdr[1] == 'I') && (hdr[2] == 'F') && (hdr[3] == 'F'))
		{
			mmioSeek(hmmio,0,SEEK_SET);
			//RMP3チャンクへ移動
			MMCKINFO	mmckOutinfoParent;
			memset(&mmckOutinfoParent,0,sizeof(mmckOutinfoParent));
			mmckOutinfoParent.fccType = mmioFOURCC('R','M','P','3');
			if(mmioDescend(hmmio,&mmckOutinfoParent,NULL,MMIO_FINDRIFF) == MMSYSERR_NOERROR)
			{
				//dataチャンクへ移動
				MMCKINFO	mmckOutinfoSubchunk;
				memset(&mmckOutinfoSubchunk,0,sizeof(mmckOutinfoSubchunk));
				mmckOutinfoSubchunk.fccType = mmioFOURCC('d','a','t','a');
				if(mmioDescend(hmmio,&mmckOutinfoSubchunk,&mmckOutinfoParent,MMIO_FINDCHUNK) == MMSYSERR_NOERROR)
				{
					lDataPtr = mmioSeek(hmmio,0,SEEK_CUR);
					dataSize = mmckOutinfoSubchunk.cksize; //ストリームサイズを取得
				}
			}
		}
		mmioClose(hmmio,0);
	}

	DWORD dwRet;
	HANDLE hFile = CreateFile(
				szFileName,
				GENERIC_READ,
				FILE_SHARE_READ,
				NULL,
				OPEN_EXISTING,			//指定したファイルが存在していない場合、この関数は失敗します。 
				FILE_ATTRIBUTE_NORMAL,
				NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	if(dataSize == 0)
	{
		dataSize = GetFileSize(hFile,NULL);
		//id3tagヘッダを読み込む
		SetFilePointer(hFile,-128,NULL,FILE_END);
		if(!ReadFile(hFile,&id3tag,sizeof(id3tag),&dwRet,NULL))
		{
			CloseHandle(hFile);
			return FALSE;
		}
		if((dwRet == sizeof(id3tag)) && (memcmp(id3tag,"TAG",3) == 0))
		{
			//ID3TAGのサイズ分を差し引く
			dataSize -= 128;
		}
	}
	//ID3V2ヘッダを読み込む
	char id3v2head[10];
	SetFilePointer(hFile,0,NULL,FILE_BEGIN);
	if(ReadFile(hFile,&id3v2head,sizeof(id3v2head),&dwRet,NULL) &&
		(dwRet == sizeof(id3v2head)) &&
		(memcmp(id3v2head,"ID3",3) == 0) )
	{
		lDataPtr = (((long )(id3v2head[6]&0x7f)<<21) | ((long )(id3v2head[7]&0x7f)<<14) | ((long )(id3v2head[8]&0x7f)<<7) | (long )(id3v2head[9]&0x7f));
		lDataPtr += 10;
	}

	//Mp3ヘッダ情報の読み取り
	DWORD dwFrameCount = 0;
	MPEGINFO mpegHead;
	MPEGINFO *pMpegHead = &m_mpegInfo;
	SetFilePointer(hFile,lDataPtr,NULL,FILE_BEGIN);
	while(ReadFile(hFile,&hbuf,sizeof(hbuf),&dwRet,NULL) && (dwRet == sizeof(hbuf)))
	{
		head=((unsigned long )hbuf[0] << 24) | ((unsigned long) hbuf[1] << 16) |
				((unsigned long) hbuf[2] << 8) | (unsigned long) hbuf[3];
		//mp3ヘッダとしての妥当性をチェック
		if(!mp3head_check(head))
		{
			//+1
			if(SetFilePointer(hFile,-3,NULL,FILE_CURRENT) > (lDataPtr + 10 * 1024))
			{
				//先頭の10Kだけを調べる
				break;
			}
			continue;//１バイトずらしてもう一度
		}
		//pMpegHeadに戻すのは最初の1フレームだけ
		if(dwFrameCount > 0)
		{
			pMpegHead = &mpegHead;
		}
		else
		{
			dwBeginPtr = SetFilePointer(hFile,0,NULL,FILE_CURRENT) - 4;
			//最初のMP3ヘッダまでの余分なデータ分をサイズから差し引く
			dataSize -= dwBeginPtr;
		}

		if(head & (1<<20))
		{
			pMpegHead->lsf = (head & (1<<19)) ? 0x0 : 0x1;
			pMpegHead->mpeg25 = 0;
		}
		else
		{
			pMpegHead->lsf = 1;
			pMpegHead->mpeg25 = 1;
		}
		pMpegHead->lay = 4-((head>>17)&3);
		if(pMpegHead->mpeg25)
			pMpegHead->sampling_frequency = 6 + ((head>>10)&0x3);
		else
			pMpegHead->sampling_frequency = ((head>>10)&0x3) + (pMpegHead->lsf*3);
		if(pMpegHead->sampling_frequency > 8)
			pMpegHead->sampling_frequency = 8;
		pMpegHead->sampling_frequency = freqs[pMpegHead->sampling_frequency];
		pMpegHead->error_protection = ((head>>16)&0x1)^0x1;
		pMpegHead->bitrate_index	= ((head>>12)&0xf);
		pMpegHead->padding			= ((head>>9)&0x1);
		pMpegHead->extension		= ((head>>8)&0x1);
		pMpegHead->mode				= ((head>>6)&0x3);
		pMpegHead->mode_ext			= ((head>>4)&0x3);
		pMpegHead->copyright		= ((head>>3)&0x1);
		pMpegHead->original			= ((head>>2)&0x1);
		pMpegHead->emphasis			= head & 0x3;
		pMpegHead->stereo			= (pMpegHead->mode == 3) ? 1 : 2;
		pMpegHead->bps				=	 tabsel_123[pMpegHead->lsf][pMpegHead->lay-1][pMpegHead->bitrate_index];
		switch(pMpegHead->lay)
		{
		case 1:
			pMpegHead->framesize  = (long) tabsel_123[pMpegHead->lsf][0][pMpegHead->bitrate_index] * 12000;
			pMpegHead->framesize /= pMpegHead->sampling_frequency;
			pMpegHead->framesize  = ((pMpegHead->framesize+pMpegHead->padding)<<2)-4;
			break;
		case 2:
			pMpegHead->framesize = (long) tabsel_123[pMpegHead->lsf][1][pMpegHead->bitrate_index] * 144000;
			pMpegHead->framesize /= pMpegHead->sampling_frequency;
			pMpegHead->framesize += pMpegHead->padding - 4;
			break;
		case 3:
			pMpegHead->framesize  = (long) tabsel_123[pMpegHead->lsf][2][pMpegHead->bitrate_index] * 144000;
			pMpegHead->framesize /= pMpegHead->sampling_frequency<<(pMpegHead->lsf);
			pMpegHead->framesize += pMpegHead->padding - 4;
			break;
		}
		//フレーム数と録音時間を計算
		if(dwFrameCount == 0)
		{
			pMpegHead->size = dataSize;
			pMpegHead->msec=0;
			if(!(pMpegHead->framesize+4))
			{
				CloseHandle(hFile);
				return FALSE;//0除算防止
			}
//			_int64 i64Msec;
			pMpegHead->flmnum = pMpegHead->size/(pMpegHead->framesize+4);
//			i64Msec = (_int64 )pMpegHead->flmnum * 576 * (pMpegHead->lsf?1:2) * 1000 / pMpegHead->sampling_frequency;
//			pMpegHead->msec = (ULONG )i64Msec;
			if(pMpegHead->bps == 0)
			{
				CloseHandle(hFile);
				return FALSE;// 2004-02-19 0除算防止
			}
			pMpegHead->msec = pMpegHead->size * 8 / pMpegHead->bps;
			if(pMpegHead->lsf) 
			{
				//mpeg2/2.5
				if(pMpegHead->stereo == 2)
				{
					//+17+4
					SetFilePointer(hFile,17,NULL,FILE_CURRENT);
				}else
				{
					//+9+4
					SetFilePointer(hFile,9,NULL,FILE_CURRENT);
				}
			}
			else
			{
				//mpeg1
				if(pMpegHead->stereo == 2)
				{
					//+32+4
					SetFilePointer(hFile,32,NULL,FILE_CURRENT);
				}else
				{
					//+17+4
					SetFilePointer(hFile,17,NULL,FILE_CURRENT);
				}
			}
			//VBRタグを読み取る
			if(!ReadFile(hFile,&xingTag,sizeof(xingTag),&dwRet,NULL))
			{
				CloseHandle(hFile);
				return FALSE;
			}
			if((dwRet == sizeof(xingTag)) && (memcmp(xingTag,"Xing",4) == 0))
			{
				int iOffset = 4;
				ULONG flag = ExtractI4(&xingTag[iOffset]);
				if(flag & FRAMES_FLAG)
				{
					iOffset += 4;
					pMpegHead->flmnum = ExtractI4(&xingTag[iOffset])+1;
					pMpegHead->msec = (ULONG )((_int64 )pMpegHead->flmnum * 576 * (pMpegHead->lsf?1:2) * 1000 / pMpegHead->sampling_frequency);
				}
				if(flag & BYTES_FLAG)
				{
					iOffset += 4;
					pMpegHead->size = ExtractI4(&xingTag[iOffset]);
				}
				pMpegHead->bps = (_int64 )(pMpegHead->size*8)/(pMpegHead->msec);
				pMpegHead->bVbr = TRUE;
			}
			// 2004-10-04 追加
			// VBRI 形式タグを読み取る
			SetFilePointer(hFile,dwBeginPtr+sizeof(DWORD)*9,NULL,FILE_BEGIN);
			VBRI vbri;
			if(!ReadFile(hFile,&vbri,sizeof(vbri),&dwRet,NULL))
			{
				CloseHandle(hFile);
				return FALSE;
			}
			if(memcmp(vbri.magic,"VBRI",4) == 0)
			{
				vbri.version		= ExtractI2((unsigned char *)&vbri.version);
				vbri.delay			= ExtractI2((unsigned char *)&vbri.delay);
				vbri.quality		= ExtractI2((unsigned char *)&vbri.quality);
				vbri.streamBytes	= ExtractI4((unsigned char *)&vbri.streamBytes);
				vbri.streamFrames	= ExtractI4((unsigned char *)&vbri.streamFrames);
				vbri.tableSize		= ExtractI2((unsigned char *)&vbri.tableSize);
				vbri.tableScale		= ExtractI2((unsigned char *)&vbri.tableScale);
				vbri.entryBytes		= ExtractI2((unsigned char *)&vbri.entryBytes);
				vbri.entryFrames	= ExtractI2((unsigned char *)&vbri.entryFrames);
				pMpegHead->flmnum = vbri.streamFrames;
				pMpegHead->msec = (ULONG )((_int64 )pMpegHead->flmnum * 576 * (pMpegHead->lsf?1:2) * 1000 / pMpegHead->sampling_frequency);
				pMpegHead->size = vbri.streamBytes;
				pMpegHead->bps = (_int64 )(pMpegHead->size*8)/(pMpegHead->msec);
				pMpegHead->bVbr = TRUE;
			}

			SetFilePointer(hFile,dwBeginPtr+4,NULL,FILE_BEGIN);
		}
		dwFrameCount++;
		if(!m_mpegInfo.bVbr)	//VBRのときはすべてのフレームをスキャン
			break;
		if(!bVbrScan)
			break;
		FRAMEINFO frameInfo;
		frameInfo.dwPtr = SetFilePointer(hFile,0,NULL,FILE_CURRENT)-4;
		frameInfo.dwSize = pMpegHead->framesize;
		m_listFrame.push_back(frameInfo);
		if(!pMpegHead->framesize)
			break;//無限ループを防止
		SetFilePointer(hFile,pMpegHead->framesize,NULL,FILE_CURRENT);
	}

	if(bVbrScan)
	{
		pMpegHead->flmnum = dwFrameCount;
		pMpegHead->msec = (ULONG )((_int64 )pMpegHead->flmnum * 576 * (pMpegHead->lsf?1:2) * 1000 / pMpegHead->sampling_frequency);
	}

	CloseHandle(hFile);
	if(dwFrameCount == 0)
	{
		return FALSE;
	}
	else
	{
		// 2003-08-18 フレーム数をTimeからFormatに移動
		m_strFormat.Format(
			IsVbr()?"MPEG%s Layer%ld %ldKb/s(VBR) %ldHz %s %dframes":"MPEG%s Layer%ld %ldKb/s %ldHz %s %dframes",
			(LPCSTR )GetMpeg(),
			GetLayers(),
			GetBps(),
			GetFreqs(),
			GetModes(),
			GetFlmNum());
		m_strTime.Format(
			"%ld:%02ld (%ldsec)",
			(DWORD )GetMSec()/1000/60,
			(DWORD )GetMSec()/1000%60,
			(DWORD )GetMSec()/1000
			);
		return TRUE;
	}
}

const BOOL CMp3Info::IsVbr()
{
	return m_mpegInfo.bVbr;
}

const CString CMp3Info::GetMpeg()
{
	return (m_mpegInfo.mpeg25 ? "2.5" : (m_mpegInfo.lsf ? "2.0" : "1.0" ));
}

const long CMp3Info::GetLayers()
{
	return m_mpegInfo.lay;
}

const long CMp3Info::GetBps()
{
	return m_mpegInfo.bps;
}

const long CMp3Info::GetFreqs()
{
	return m_mpegInfo.sampling_frequency;
}

const CString CMp3Info::GetModes()
{
	return modes[m_mpegInfo.mode&0x3];
}

const unsigned long CMp3Info::GetMSec()
{
	return m_mpegInfo.msec;
}

const unsigned long CMp3Info::GetFlmNum()
{
	return m_mpegInfo.flmnum;
}

