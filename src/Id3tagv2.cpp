// Id3tagv2.cpp: CId3tagv2 クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "mp3infp_res/resource.h"		// メイン シンボル
#include "GlobalCommand.h"
//#include "ClassID.h"
#include "Id3tagv1.h"

#include "Id3tagv2.h"

static const unsigned long ID3V2_PADDING_SIZE = 0x0800;
//static const unsigned char SCMPX_GENRE_NULL = 247;
//static const unsigned char WINAMP_GENRE_NULL = 255;
const int CId3tagv2::ID3V2CHARENCODE_ISO_8859_1	= 0;
const int CId3tagv2::ID3V2CHARENCODE_UTF_16		= 1;
const int CId3tagv2::ID3V2CHARENCODE_UTF_16BE	= 2;
const int CId3tagv2::ID3V2CHARENCODE_UTF_8		= 3;

const char *CId3tagv2::numeric_tags[] = {
	"TBPM", "TDAT", "TDLY", "TIME", "TLEN", "TPOS", "TRCK", "TSIZ", "TYER",
	"TDEN", "TDOR", "TDRC", "TDRL", "TDTG",
	NULL
};

// 2004-09-24 UTF-16はリトルエンディアンで書き込む
//#define UTF16_BIGENDIAN

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CId3tagv2::CId3tagv2()
{
	m_encode = ID3V2CHARENCODE_ISO_8859_1;
	m_bUnSynchronization = TRUE;
//	m_wDefaultId3TagVersion = 0x0300;
	Release();
}

CId3tagv2::~CId3tagv2()
{
}

void CId3tagv2::Release()
{
	m_bEnable = FALSE;
	memset(&m_head,0,sizeof(m_head));
	memcpy(m_head.id3des,"ID3",3);

	m_wVer = 0x0300;
	m_frames.clear();
}

void CId3tagv2::SetEncDefault(LPCTSTR szDefaultEnc)
{
	m_strDefaultEnc = szDefaultEnc;
}

CString CId3tagv2::ReadEncodedTextString(unsigned char encoding,
		unsigned char *data, int datasize, DWORD *pdwReadSize)
{
	DWORD len = 0;
	DWORD readsize = 0;
	DWORD dwDummy;
	if (pdwReadSize == NULL) {
		pdwReadSize = &dwDummy;
	}
	if ((data == NULL) || (datasize <= 0)) {
		*pdwReadSize = readsize;
		return _T("");
	}
	
	int code;
	unsigned char *start = data;
	switch (encoding) {
	case 0:
		code = DTC_CODE_ANSI;
		break;
	case 1:
		if (datasize < 2) {
			*pdwReadSize = readsize;
			return _T("");
		}
		readsize += 2;	// BOM
		start += 2;
		if (memcmp(data, "\xff\xfe", 2) == 0) {
			code = DTC_CODE_UTF16LE;
		} else if (memcmp(data, "\xfe\xff", 2) == 0) {
			code = DTC_CODE_UTF16BE;
		} else {
			*pdwReadSize = readsize;
			return _T("");
		}
		break;
	case 2:
		code = DTC_CODE_UTF16BE;
		break;
	case 3:
		code = DTC_CODE_UTF8;
		break;
	}
	if ((encoding == 1) || (encoding == 2)) {
		for (; readsize < datasize; readsize += 2) {
			if (*(WCHAR*)(data + readsize) == L'\0') {
				break;
			}
		}
		len = readsize - (start - data);
		if ((readsize + 2 <= datasize) && (*(WCHAR*)(data + readsize) == L'\0')) {
			readsize += 2;
		}
	} else {
		for (; readsize < datasize; readsize++) {
			if (data[readsize] == '\0') {
				break;
			}
		}
		len = readsize;
		if ((readsize + 1 <= datasize) && (data[readsize] == '\0')) {
			readsize++;
		}
	}
	*pdwReadSize = readsize;
	return DataToCString((const char *)start, len, code);
}

CString CId3tagv2::GetId3String(const char szId[])
{
	pair< multimap< DWORD , CId3Frame >::iterator, multimap< DWORD , CId3Frame >::iterator > pp;
	multimap<DWORD,CId3Frame>::iterator p;
	DWORD dwId;
	DWORD dwReadSize;
	unsigned char *data;
	DWORD i;
	switch(szId[0]){
	case 'T':	//テキスト情報フレーム
		memcpy(&dwId,szId,sizeof(dwId));
		pp= m_frames.equal_range(dwId);
		if(pp.first == pp.second)
		{
			break;
		}
		p = pp.first;
		if(p->second.GetSize() == 0)
		{
			break;
		}
		data = p->second.GetData();
		return ReadEncodedTextString(data[0], &data[1], p->second.GetSize()-1, NULL);
	case 'W':	//URLリンクフレームx
		if(strcmp(szId,"WXXX") != 0)
		{
			break;
		}
		memcpy(&dwId,szId,sizeof(dwId));
		pp= m_frames.equal_range(dwId);
		if(pp.first == pp.second)
		{
			break;
		}
		p = pp.first;
		if(p->second.GetSize() == 0)
		{
			break;
		}
		data = p->second.GetData();
		
		//説明文を読み飛ばす
		ReadEncodedTextString(data[0], &data[1], p->second.GetSize()-1, &dwReadSize);
		//URL本体
		//規格上は常に ISO-8859-1（互換性のため、BOMがあればそちらを優先）
		if ((p->second.GetSize()-1-dwReadSize >= 2)
				&& (data[0] == 1)
				&& ((memcmp(&data[1+dwReadSize], "\xff\xfe", 2) == 0)
					|| (memcmp(&data[1+dwReadSize], "\xfe\xff", 2) == 0))) {
			return ReadEncodedTextString(1, &data[1+dwReadSize], p->second.GetSize()-1-dwReadSize, NULL);
		}
		return ReadEncodedTextString(0, &data[1+dwReadSize], p->second.GetSize()-1-dwReadSize, NULL);
	case 'C':
		if(strcmp(szId,"COMM") != 0)
		{
			break;
		}
		memcpy(&dwId,szId,sizeof(dwId));
		for (pp = m_frames.equal_range(dwId); pp.first != pp.second; ++pp.first)
		{
			p = pp.first;
			if(p->second.GetSize() == 0)
			{
				continue;
			}
			data = p->second.GetData();

			// 言語文字列読み飛ばし、説明文取得
			CString desc = ReadEncodedTextString(data[0], &data[4], p->second.GetSize()-4, &dwReadSize);
			if(desc.Left(4) == _T("iTun"))
			{
				// iTunes固有のコメントはスキップ (iTunNORM, iTunSMPB, etc.)
				continue;
			}
			//本文
			return ReadEncodedTextString(data[0], &data[4+dwReadSize], p->second.GetSize()-4-dwReadSize, NULL);
		}
		break;
	}
	return _T("");
}

void CId3tagv2::SetId3String(const char szId[],LPCTSTR szString,LPCTSTR szDescription)
{
	pair< multimap< DWORD , CId3Frame >::iterator, multimap< DWORD , CId3Frame >::iterator > pp;
	multimap<DWORD,CId3Frame>::iterator p;
	CId3Frame *pFrame;
	DWORD dwId;
	memcpy(&dwId,szId,sizeof(dwId));

	//Loadしたファイルにフレームがなかった場合
	if((lstrlen(szString) == 0) && (strcmp(szId,"COMM") != 0))
	{
		m_frames.erase(dwId);	//消す(あれば)
		return;
	}
	
	unsigned char *data;
	int size = 0;
	int encode, i;
	switch(szId[0]){
	case 'T':	//テキスト情報フレーム
		encode = m_encode;
		//数字文字列はISO-8859-1
		for (i = 0; numeric_tags[i] != NULL; i++) {
			if (strcmp(szId, numeric_tags[i]) == 0) {
				encode = ID3V2CHARENCODE_ISO_8859_1;
				break;
			}
		}
		switch(encode){
		case ID3V2CHARENCODE_ISO_8859_1:
		default:	// ISO-8859-1
			size = TstrToData(szString, -1, NULL, 0, DTC_CODE_ANSI) + 1;
			data = (unsigned char *)malloc(size);
			if(!data)
			{
				return;
			}
			data[0] = 0;	//encoding
			TstrToData(szString, -1, (char *)&data[1], size-1, DTC_CODE_ANSI);
			break;
		case ID3V2CHARENCODE_UTF_16:	// UTF-16
#ifndef UTF16_BIGENDIAN
			size = TstrToData(szString, -1, NULL, 0, DTC_CODE_UTF16LE) + 3;
			data = (unsigned char *)malloc(size);
			if(!data)
			{
				return;
			}
			data[0] = 1;	//encoding
			data[1] = 0xff;	//BOM
			data[2] = 0xfe;
			TstrToData(szString, -1, (char *)&data[3], size-3, DTC_CODE_UTF16LE);
			break;
#else
			size = TstrToData(szString, -1, NULL, 0, DTC_CODE_UTF16BE) + 3;
			data = (unsigned char *)malloc(size);
			if(!data)
			{
				return;
			}
			data[0] = 1;	//encoding
			data[1] = 0xfe;	//BOM
			data[2] = 0xff;
			TstrToData(szString, -1, (char *)&data[3], size-3, DTC_CODE_UTF16BE);
			break;
#endif
		case ID3V2CHARENCODE_UTF_16BE:	// UTF-16BE
			size = TstrToData(szString, -1, NULL, 0, DTC_CODE_UTF16BE) + 1;
			data = (unsigned char *)malloc(size);
			if(!data)
			{
				return;
			}
			data[0] = 0x02;	//encoding
			TstrToData(szString, -1, (char *)&data[1], size-1, DTC_CODE_UTF16BE);
			break;
		case ID3V2CHARENCODE_UTF_8:	// UTF-8
			{
				size = TstrToData(szString, -1, NULL, 0, DTC_CODE_UTF8) + 1;
				data = (unsigned char *)malloc(size);
				if(!data)
				{
					return;
				}
				data[0] = 3;	//encoding
				TstrToData(szString, -1, (char *)&data[1], size-1, DTC_CODE_UTF8);
				break;
			}
		}
//		p = m_frames.find(dwId);
		pp= m_frames.equal_range(dwId);
		if((pp.first == pp.second) || !(pp.first->second.GetSize()))
		{
			//Loadしたファイルにフレームがなかった場合
			CId3Frame frame;
			frame.SetId(dwId);
			frame.SetFlags(0);
			frame.SetData(data,size);
			m_frames.insert(pair<DWORD,CId3Frame>(frame.GetId(),frame));
		}
		else
		{
			p = pp.first;
			pFrame = &p->second;
			pFrame->SetData(data,size);
		}
		free(data);
		break;
	case 'W':	//URLリンクフレームx
		if(strcmp(szId,"WXXX") != 0)
		{
			break;
		}
		switch(m_encode){
		case ID3V2CHARENCODE_ISO_8859_1:
		default:	// ISO-8859-1
			size = TstrToData(szString, -1, NULL, 0, DTC_CODE_ANSI) + 2;
			data = (unsigned char *)malloc(size);
			if(!data)
			{
				return;
			}
			data[0] = 0;	//encoding
			data[1] = 0;	//説明文(省略)
			TstrToData(szString, -1, (char *)&data[2], size-2, DTC_CODE_ANSI);	//URL本体（常にISO-8859-1）
			break;
		case ID3V2CHARENCODE_UTF_16:	// UTF-16
#ifndef UTF16_BIGENDIAN
			size = TstrToData(szString, -1, NULL, 0, DTC_CODE_UTF16LE) + 5;
			data = (unsigned char *)malloc(size);
			if(!data)
			{
				return;
			}
			data[0] = 1;	//encoding
			data[1] = 0xff;	//BOM
			data[2] = 0xfe;
			data[3] = 0;	//説明文(省略)
			data[4] = 0;
			TstrToData(szString, -1, (char *)&data[5], size-5, DTC_CODE_ANSI);	//URL本体（常にISO-8859-1）
			break;
#else	// ビックエンディアン
			size = TstrToData(szString, -1, NULL, 0, DTC_CODE_UTF16BE) + 5;
			data = (unsigned char *)malloc(size);
			if(!data)
			{
				return;
			}
			data[0] = 1;	//encoding
			data[1] = 0xfe;	//BOM
			data[2] = 0xff;
			data[3] = 0;	//説明文(省略)
			data[4] = 0;
			TstrToData(szString, -1, (char *)&data[5], size-5, DTC_CODE_ANSI);	//URL本体（常にISO-8859-1）
			break;
#endif
		case ID3V2CHARENCODE_UTF_16BE:	// UTF-16BE
			size = TstrToData(szString, -1, NULL, 0, DTC_CODE_UTF16BE) + 3;
			data = (unsigned char *)malloc(size);
			if(!data)
			{
				return;
			}
			data[0] = 2;	//encoding
			data[1] = 0;	//説明文(省略)
			data[2] = 0;
			TstrToData(szString, -1, (char *)&data[3], size-3, DTC_CODE_ANSI);	//URL本体（常にISO-8859-1）
			break;
		case ID3V2CHARENCODE_UTF_8:	// UTF-8
			{
				size = TstrToData(szString, -1, NULL, 0, DTC_CODE_UTF8) + 2;
				data = (unsigned char *)malloc(size);
				if(!data)
				{
					return;
				}
				data[0] = 3;	//encoding
				data[1] = 0;	//説明文(省略)
				TstrToData(szString, -1, (char *)&data[2], size-2, DTC_CODE_ANSI);	//URL本体（常にISO-8859-1）
				break;
			}
		}
		pp= m_frames.equal_range(dwId);
		if((pp.first == pp.second) || !(pp.first->second.GetSize()))
		{
			//Loadしたファイルにフレームがなかった場合
			CId3Frame frame;
			frame.SetId(dwId);
			frame.SetFlags(0);
			frame.SetData(data,size);
			m_frames.insert(pair<DWORD,CId3Frame>(frame.GetId(),frame));
		}
		else
		{
			p = pp.first;
			pFrame = &p->second;
			pFrame->SetData(data,size);
		}
		free(data);
		break;
	case 'C':
		if(strcmp(szId,"COMM") != 0)
		{
			break;
		}
		// コメントフレームを検索
		for (pp = m_frames.equal_range(dwId); pp.first != pp.second; ++pp.first)
		{
			p = pp.first;
			if(p->second.GetSize() == 0)
			{
				continue;
			}
			unsigned char *data = p->second.GetData();

			// 言語文字列読み飛ばし、説明文取得
			DWORD dwReadSize;
			CString desc = ReadEncodedTextString(data[0], &data[4], p->second.GetSize()-4, &dwReadSize);
			if(desc.Left(4) == _T("iTun"))
			{
				// iTunes固有のコメントはスキップ (iTunNORM, iTunSMPB, etc.)
				continue;
			}

			// 書き換え対象のコメントフレームを発見

			if(lstrlen(szString) == 0)
			{
				m_frames.erase(p);	//消す
				return;
			}
			break;
		}
		switch(m_encode){
		case ID3V2CHARENCODE_ISO_8859_1:
		default:	// ISO-8859-1
			size = TstrToData(szString, -1, NULL, 0, DTC_CODE_ANSI) + 5;
			data = (unsigned char *)malloc(size);
			if(!data)
			{
				return;
			}
			data[0] = 0;	//encoding
			data[1] = 'e';	//Language
			data[2] = 'n';
			data[3] = 'g';
			data[4] = 0;	//説明文(省略)
			TstrToData(szString, -1, (char *)&data[5], size-5, DTC_CODE_ANSI);
			break;
		case ID3V2CHARENCODE_UTF_16:	// UTF-16
#ifndef UTF16_BIGENDIAN
			size = TstrToData(szString, -1, NULL, 0, DTC_CODE_UTF16LE) + 10;
			data = (unsigned char *)malloc(size);
			if(!data)
			{
				return;
			}
			data[0] = 1;	//encoding
			data[1] = 'e';	//Language
			data[2] = 'n';
			data[3] = 'g';
			data[4] = 0xff;	//BOM
			data[5] = 0xfe;
			data[6] = 0;	//説明文(省略)
			data[7] = 0;
			data[8] = 0xff;	//BOM
			data[9] = 0xfe;
			TstrToData(szString, -1, (char *)&data[10], size-10, DTC_CODE_UTF16LE);
			break;
#else	// ビッグエンディアン
			size = TstrToData(szString, -1, NULL, 0, DTC_CODE_UTF16BE) + 10;
			data = (unsigned char *)malloc(size);
			if(!data)
			{
				return;
			}
			data[0] = 1;	//encoding
			data[1] = 'e';	//Language
			data[2] = 'n';
			data[3] = 'g';
			data[4] = 0xfe;	//BOM
			data[5] = 0xff;
			data[6] = 0;	//説明文(省略)
			data[7] = 0;
			data[8] = 0xfe;	//BOM
			data[9] = 0xff;
			TstrToData(szString, -1, (char *)&data[10], size-10, DTC_CODE_UTF16BE);
			break;
#endif
		case ID3V2CHARENCODE_UTF_16BE:	// UTF-16BE
			size = TstrToData(szString, -1, NULL, 0, DTC_CODE_UTF16BE) + 6;
			data = (unsigned char *)malloc(size);
			if(!data)
			{
				return;
			}
			data[0] = 2;	//encoding
			data[1] = 'e';	//Language
			data[2] = 'n';
			data[3] = 'g';
			data[4] = 0;	//説明文(省略)
			data[5] = 0;	//説明文(省略)
			TstrToData(szString, -1, (char *)&data[6], size-6, DTC_CODE_UTF16BE);
			break;
		case ID3V2CHARENCODE_UTF_8:	// UTF-8
			{
				size = TstrToData(szString, -1, NULL, 0, DTC_CODE_UTF8) + 5;
				data = (unsigned char *)malloc(size);
				if(!data)
				{
					return;
				}
				data[0] = 3;	//encoding
				data[1] = 'e';	//Language
				data[2] = 'n';
				data[3] = 'g';
				data[4] = 0;	//説明文(省略)
				TstrToData(szString, -1, (char *)&data[5], size-5, DTC_CODE_UTF8);
				break;
			}
		}
	//	pp= m_frames.equal_range(dwId);
		if((pp.first == pp.second) || !(pp.first->second.GetSize()))
		{
			CId3Frame frame;
			frame.SetId(dwId);
			frame.SetFlags(0);
			frame.SetData(data,size);
			m_frames.insert(pair<DWORD,CId3Frame>(frame.GetId(),frame));
		}
		else
		{
			p = pp.first;
			pFrame = &p->second;
			pFrame->SetData(data,size);
		}
		free(data);
		break;
	}
	return;
}

DWORD CId3tagv2::GetTotalFrameSize()
{
	DWORD dwSize = 0;
	multimap<DWORD,CId3Frame>::iterator p;

	p = m_frames.begin();
	while(p != m_frames.end())
	{
		CId3Frame *pFrame = &p->second;
		if(m_wVer == 0x0200)
		{
			dwSize += pFrame->GetSize() + 6;
		}
		else
		{
			dwSize += pFrame->GetSize() + 10;
		}

		p++;
	}
	return dwSize;
}

// 全てのフレームの文字列エンコードを指定する
void CId3tagv2::_SetStringEncode(int encode)
{
	multimap<DWORD,CId3Frame>::iterator p;

	p = m_frames.begin();
	while(p != m_frames.end())
	{
		CId3Frame *pFrame = &p->second;
		unsigned char *data = pFrame->GetData();
		
		switch(encode)
		{
		default:
		case ID3V2CHARENCODE_ISO_8859_1:
			data[0] = 0;
			break;
		case ID3V2CHARENCODE_UTF_16:
			data[0] = 1;
			break;
		case ID3V2CHARENCODE_UTF_16BE:
			data[0] = 2;
			break;
		case ID3V2CHARENCODE_UTF_8:
			data[0] = 3;
			break;
		}

		p++;
	}
}

id3_v23v22table_t id3_v23v22table[] = {
	{"AENC", "CRA"},
	{"APIC", "PIC"},
	{"COMM", "COM"},
	{"EQUA", "EQU"},
	{"ETCO", "ETC"},
	{"GEOB", "GEO"},
	{"IPLS", "IPL"},
	{"MCDI", "MCI"},
	{"MLLT", "MLL"},
	{"PCNT", "CNT"},
	{"POPM", "POP"},
	{"RBUF", "BUF"},
	{"RVAD", "RVA"},
	{"RVRB", "REV"},
	{"SYLT", "SLT"},
	{"SYTC", "STC"},
	{"TALB", "TAL"},
	{"TBPM", "TBP"},
	{"TCOM", "TCM"},
	{"TCON", "TCO"},
	{"TCOP", "TCR"},
	{"TDAT", "TDA"},
	{"TDLY", "TDY"},
	{"TENC", "TEN"},
	{"TFLT", "TFT"},
	{"TIME", "TIM"},
	{"TIT1", "TT1"},
	{"TIT2", "TT2"},
	{"TIT3", "TT3"},
	{"TKEY", "TKE"},
	{"TLAN", "TLA"},
	{"TLEN", "TLE"},
	{"TMED", "TMT"},
	{"TOAL", "TOT"},
	{"TOFN", "TOF"},
	{"TOLY", "TOL"},
	{"TOPE", "TOA"},
	{"TORY", "TOR"},
	{"TPE1", "TP1"},
	{"TPE2", "TP2"},
	{"TPE3", "TP3"},
	{"TPE4", "TP4"},
	{"TPOS", "TPA"},
	{"TPUB", "TPB"},
	{"TRCK", "TRK"},
	{"TRDA", "TRD"},
	{"TSIZ", "TSI"},
	{"TSRC", "TRC"},
	{"TSSE", "TSS"},
	{"TYER", "TYE"},
	{"TXXX", "TXX"},
	{"UFID", "UFI"},
	{"USLT", "ULT"},
	{"WCOM", "WCM"},
	{"WCOP", "WCP"},
	{"WOAF", "WAF"},
	{"WOAR", "WAR"},
	{"WOAS", "WAS"},
	{"WPUB", "WPB"},
	{"WXXX", "WXX"},
	{NULL, NULL}
};

void CId3tagv2::v23IDtov22ID(char *v23ID,char *v22ID)
{
	//v2.3からv2.2へフレームIDを変換
	int i;
	for (i = 0; id3_v23v22table[i].v23 != NULL; i++) {
		if (memcmp(v23ID, id3_v23v22table[i].v23, 4) == 0) {
			memcpy(v22ID, id3_v23v22table[i].v22, 3);
			break;
		}
	}
	if (id3_v23v22table[i].v23 == NULL) {
		memcpy(v22ID, "XXX", 3);	// 不明
	}
}

#if 0
// UNICODE文字列のBE/LEを変換する(len=文字数)
void CId3tagv2::UTF16toUTF16BE(WCHAR *str,int len)
{
	for(int i=0; i<len; i++)
	{
		str[i] = (str[i] << 8) | (str[i] >> 8);
	}
}
#endif

CString CId3tagv2::GetTitle()
{
	//タイトル
	return GetId3String("TIT2");
}

void CId3tagv2::SetTitle(LPCTSTR title)
{
	//タイトル
	SetId3String("TIT2",title);
}

CString CId3tagv2::GetTrackNo()
{
	//トラック番号
	return GetId3String("TRCK");
}

void CId3tagv2::SetTrackNo(LPCTSTR szTrackNo)
{
	//トラック番号
	SetId3String("TRCK",szTrackNo);
}

CString CId3tagv2::GetArtist()
{
	//アーティスト
	return GetId3String("TPE1");
}

void CId3tagv2::SetArtist(LPCTSTR artist)
{
	//アーティスト
	SetId3String("TPE1",artist);
}

CString CId3tagv2::GetAlbumArtist()
{
	//アルバムアーティスト
	return GetId3String("TPE2");
}

void CId3tagv2::SetAlbumArtist(LPCTSTR albumartist)
{
	//アルバムアーティスト
	SetId3String("TPE2",albumartist);
}

CString CId3tagv2::GetAlbum()
{
	//アルバム
	return GetId3String("TALB");
}

void CId3tagv2::SetAlbum(LPCTSTR album)
{
	//アルバム
	SetId3String("TALB",album);
}

CString CId3tagv2::GetYear()
{
	//西暦
	if(m_wVer < 0x0400)
	{
		return GetId3String("TYER");
	}
	else
	{
		return GetId3String("TDRC");
	}
}

void CId3tagv2::SetYear(LPCTSTR year)
{
	//西暦
	if(m_wVer < 0x0400)
	{
		SetId3String("TDRC",_T(""));
		SetId3String("TYER",year);
	}
	else
	{
		SetId3String("TDRC",year);
		SetId3String("TYER",_T(""));
	}
}

CString CId3tagv2::GetGenre()
{
	//ジャンル
	CString strGenre = GetId3String("TCON");
	int genre = -1;
//	unsigned char *data = (unsigned char *)(LPCSTR )strGenre;
	//最初の()を読み飛ばす処理	Fix 2001-05-20
	while(1)
	{
		if(strGenre.GetLength() &&
			!IS_LEAD_TBYTE(strGenre[0]) &&
			(strGenre[0] == '(') )	//頭にカッコを検出
		{
			if((strGenre.GetLength() > 1) &&
				!IS_LEAD_TBYTE(strGenre[1]) &&
				(strGenre[1] == '(') )
			{
				strGenre.Delete(0);	//先頭の'('を削除
				break;
			}
			int index = strGenre.Find(')');	
			if(0 <= index)
			{
				if (genre == -1) {
					CString strGenreNum = strGenre.Mid(1, index);
					genre = _ttoi(strGenreNum);
				}
				strGenre.Delete(0,index+1);	//)'以前を削除
				continue;	//Fix 2001-10-24
			}
		}
		break;
	}
	//ジャンルが番号のみで指定されている場合は、文字列に変換
	if ((genre >= 0) && (strGenre == _T(""))) {
		CId3tagv1 id3tagv1(FALSE);
		strGenre = id3tagv1.GenreNum2String(genre);
	}
	return strGenre;
}

void CId3tagv2::SetGenre(LPCTSTR szGenre)
{
	//ジャンル
	CString strGenre;
/*	CId3tagv1 id3tagv1(m_bScmpxGenre);
	long genre = id3tagv1.GenreString2Num(szGenre);
	if((genre != -1) &&
		!((m_bScmpxGenre && (SCMPX_GENRE_NULL == genre)) || (!m_bScmpxGenre && (WINAMP_GENRE_NULL == genre)))
		)
	{
		strGenre.Format(_T("(%d)"),genre);
	}
	2003-01-25 iTunesがジャンルコードへの参照を正しく読めないため、ジャンルコードを入れないように修正
*/
	// 2004-05-16 "("で始まる場合は先頭に"("を追加
	if(lstrlen(szGenre) &&
		!IS_LEAD_TBYTE(szGenre[0]) &&
		(szGenre[0] == '(') )	//頭にカッコを検出
	{
		strGenre += _T("(");
	}
	strGenre += szGenre;

	SetId3String("TCON",strGenre);
}

CString CId3tagv2::GetComment()
{
	//コメント
	return GetId3String("COMM");
}

void CId3tagv2::SetComment(LPCTSTR comment)
{
	//コメント
	SetId3String("COMM",comment);
}

CString CId3tagv2::GetComposer()
{
	//作曲
	return GetId3String("TCOM");
}

void CId3tagv2::SetComposer(LPCTSTR composer)
{
	//作曲
	SetId3String("TCOM",composer);
}

CString CId3tagv2::GetOrigArtist()
{
	//Orig.アーティスト
	return GetId3String("TOPE");
}

void CId3tagv2::SetOrigArtist(LPCTSTR origArtist)
{
	//Orig.アーティスト
	SetId3String("TOPE",origArtist);
}

CString CId3tagv2::GetCopyright()
{
	//著作権
	return GetId3String("TCOP");
}

void CId3tagv2::SetCopyright(LPCTSTR copyright)
{
	//著作権
	SetId3String("TCOP",copyright);
}

CString CId3tagv2::GetUrl()
{
	//URL
	return GetId3String("WXXX");
}

void CId3tagv2::SetUrl(LPCTSTR url)
{
	//URL
	SetId3String("WXXX",url);
}

CString CId3tagv2::GetEncoder()
{
	//エンコーダー
	return GetId3String("TSSE");
}

void CId3tagv2::SetEncoder(LPCTSTR encoder)
{
	//エンコーダー
	SetId3String("TSSE",encoder);
}

CString CId3tagv2::GetEncodest()
{
	//エンコードした人または組織
	return GetId3String("TENC");
}

void CId3tagv2::SetEncodest(LPCTSTR encoder)
{
	//エンコードした人または組織
	SetId3String("TENC",encoder);
}

DWORD CId3tagv2::DecodeUnSynchronization(unsigned char *data,DWORD dwSize)
{
	DWORD dwDecodeSize = 0;
	unsigned char *writePtr = data;
	BOOL bHitFF = FALSE;

	for(DWORD i=0; i<dwSize; i++)
	{
		if(data[i] == 0xff)
		{
			bHitFF = TRUE;
		}
		else
		{
			if(bHitFF && (data[i] == 0x00))
			{
				bHitFF = FALSE;
				continue;
			}
			bHitFF = FALSE;
		}
		writePtr[dwDecodeSize] = data[i];
		dwDecodeSize++;
	}
	return dwDecodeSize;
}

DWORD CId3tagv2::EncodeUnSynchronization(unsigned char *srcData,DWORD dwSize,unsigned char *dstData)
{
	DWORD dwDecodeSize = 0;
	unsigned char *writePtr = dstData;
	BOOL bHitFF = FALSE;

	for(DWORD i=0; i<dwSize; i++)
	{
		if(bHitFF && (((srcData[i]&0xe0) == 0xe0) || (srcData[i] == 0x00)) )
		{
			writePtr[dwDecodeSize] = 0x00;
			dwDecodeSize++;
		}
		if(srcData[i] == 0xff)
		{
			bHitFF = TRUE;
		}
		else
		{
			bHitFF = FALSE;
		}
		writePtr[dwDecodeSize] = srcData[i];
		dwDecodeSize++;
	}
	return dwDecodeSize;
}

DWORD CId3tagv2::ExtractV2Size(const unsigned char size[4])
{
	return (((DWORD )(size[0]&0x7f)<<21) | ((DWORD )(size[1]&0x7f)<<14) | ((DWORD )(size[2]&0x7f)<<7) | (DWORD )(size[3]&0x7f));
}

void CId3tagv2::MakeV2Size(DWORD dwSize,unsigned char size[4])
{
	size[3] = dwSize & 0x7f;
	size[2] = (dwSize>>7) & 0x7f;
	size[1] = (dwSize>>14) & 0x7f;
	size[0] = (dwSize>>21) & 0x7f;
}

DWORD CId3tagv2::Load(LPCTSTR szFileName)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;
	Release();
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

	//ファイルをオープン
	ID3HEAD head;
	memcpy(&head,&m_head,sizeof(m_head));
	//ID3V2ヘッダを読み込む
	DWORD dwRet;
	if(!ReadFile(hFile,&head,sizeof(head),&dwRet,NULL) || (dwRet != sizeof(head)))
	{
		CloseHandle(hFile);
		return -1;
	}

	//ID3v2を確認
	if(memcmp(head.id3des,"ID3",3) != 0)
	{
		CloseHandle(hFile);
		return -1;
	}
	
	//バージョン
	WORD ver = (head.ver[0]<<8) | head.ver[1];
	if((ver != 0x0200) &&
		(ver != 0x0300) &&
		(ver != 0x0301) &&
		(ver != 0x0400) )
	{
		// ID3v2.4より大きいバージョンはサポートしない
		CloseHandle(hFile);
		return -1;
	}
	m_wVer = ver;
	if(m_wVer == 0x0301)
	{
		m_wVer = 0x0400;
	}

	//Id3tagサイズ
	DWORD dwId3Size = ExtractV2Size(head.size);

	//全フレームの読込
	unsigned char *buf = (unsigned char *)malloc(dwId3Size);
	if(!buf)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		return dwWin32errorCode;
	}
	if(!ReadFile(hFile,buf,dwId3Size,&dwRet,NULL) || (dwRet != dwId3Size))
	{
		free(buf);
		CloseHandle(hFile);
		return -1;
	}

	//非同期化の解除
	if(head.flag & 0x80)
	{
		dwId3Size = DecodeUnSynchronization(buf,dwId3Size);
		m_bUnSynchronization = TRUE;
	}
	else
	{
		m_bUnSynchronization = FALSE;
	}

	DWORD dwRemainSize = dwId3Size;
	//拡張ヘッダを読み飛ばす(ID3v2.2には存在しない)
	if((ver != 0x0200) && (head.flag & 0x40))
	{
		dwRemainSize -= ExtractV2Size(buf) + 4;
	}
	head.flag &= ~0x40;	//解除

	m_encode = ID3V2CHARENCODE_ISO_8859_1;
	while(dwRemainSize)
	{
		CId3Frame frame;
		DWORD dwReadSize;
		if(m_wVer < 0x0300)
		{
			dwReadSize = frame.LoadFrame2_2(buf+(dwId3Size-dwRemainSize),dwRemainSize);
		}
		else if(m_wVer < 0x0400)
		{
			dwReadSize = frame.LoadFrame2_3(buf+(dwId3Size-dwRemainSize),dwRemainSize);
		}
		else
		{
			dwReadSize = frame.LoadFrame2_4(buf+(dwId3Size-dwRemainSize),dwRemainSize);
		}
		if(!dwReadSize)
			break;
		unsigned char *data = frame.GetData();
		if(frame.GetSize() && data)
		{
			switch(data[0]){
			case 0:
			default:	// ISO-8859-1
				break;
			case 1:		// UTF-16
				if(data[0] > ID3V2CHARENCODE_ISO_8859_1)
				{
					m_encode = ID3V2CHARENCODE_UTF_16;
				}
				break;
			case 2:		// UTF-16BE
				if(data[0] > ID3V2CHARENCODE_UTF_16)
				{
					m_encode = ID3V2CHARENCODE_UTF_16BE;
				}
				break;
			case 3:		// UTF-8
				if(data[0] > ID3V2CHARENCODE_UTF_16BE)
				{
					m_encode = ID3V2CHARENCODE_UTF_8;
				}
				break;
			}
		}
		m_frames.insert(pair<DWORD,CId3Frame>(frame.GetId(),frame));
		dwRemainSize -= dwReadSize;
	}

	free(buf);
	CloseHandle(hFile);

//	if(m_wVer < 0x0300)	//v2.2はv2.3へ変換して保存する
//	{
//		head.ver[0] = 0x03;
//		head.ver[1] = 0x00;
//		m_wVer = 0x0300;
//	}

	memcpy(&m_head,&head,sizeof(m_head));
	m_bEnable = TRUE;

	return dwWin32errorCode;
}

DWORD CId3tagv2::Save(LPCTSTR szFileName)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;
	HANDLE hFile;
	BOOL bInsPadding = FALSE;
	DWORD	dwWritten = 0;
	TCHAR szTempPath[MAX_PATH];
	TCHAR szTempFile[MAX_PATH];
	
	if(!m_bEnable)
	{
		return -1;
	}

/*
	// エンコード指定$2/$3が使えるのはv2.4以降
	if(m_wVer < 0x0400)
	{
		if(	(m_encode != ID3V2CHARENCODE_ISO_8859_1) &&
			(m_encode != ID3V2CHARENCODE_UTF_16) )
		{
			// ISO-8859-1に自動設定
			m_encode = ID3V2CHARENCODE_ISO_8859_1;
		}
	}

	// 全フレームの文字列エンコードを設定
	_SetStringEncode(m_encode);
*/

	DWORD dwTotalFrameSize = GetTotalFrameSize();
	//フレーム情報を書き出す準備
	unsigned char *framedata = (unsigned char *)malloc(dwTotalFrameSize);
	if(!framedata)
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}
	DWORD dwFrameDataPtr = 0;
	multimap<DWORD,CId3Frame>::iterator p;
	p = m_frames.begin();
	while(p != m_frames.end())
	{
		CId3Frame *pFrame = &p->second;
		DWORD dwSize = pFrame->GetSize();
		DWORD id = pFrame->GetId();
		//サイズが0のときはフレームを書き込まない
		if(p->second.GetSize() < 2)
		{
			p++;
			continue;
		}
		WORD flags = pFrame->GetFlags();
		WORD flagsBe = ((flags<<8)|(flags>>8));
		unsigned char *data = pFrame->GetData();
		if(m_wVer == 0x0200)
		{
			unsigned char size[3];
			size[2] = dwSize & 0xff;
			size[1] = (dwSize>>8) & 0xff;
			size[0] = (dwSize>>16) & 0xff;
			// id3v2.2
			char v22id[3];
			//v2.3からv2.2へフレームIDを変換
			v23IDtov22ID((char *)&id,v22id);
			memcpy(&framedata[dwFrameDataPtr],&v22id,sizeof(v22id));
			dwFrameDataPtr += sizeof(v22id);
			memcpy(&framedata[dwFrameDataPtr],size,sizeof(size));
			dwFrameDataPtr += sizeof(size);
			memcpy(&framedata[dwFrameDataPtr],data,dwSize);
			dwFrameDataPtr += dwSize;
		}
		else if(m_wVer == 0x0300)
		{
			unsigned char size[4];
			size[3] = dwSize & 0xff;
			size[2] = (dwSize>>8) & 0xff;
			size[1] = (dwSize>>16) & 0xff;
			size[0] = (dwSize>>24) & 0xff;
			// id3v2.3-
			memcpy(&framedata[dwFrameDataPtr],&id,sizeof(id));
			dwFrameDataPtr += sizeof(id);
			memcpy(&framedata[dwFrameDataPtr],size,sizeof(size));
			dwFrameDataPtr += sizeof(size);
			memcpy(&framedata[dwFrameDataPtr],&flagsBe,sizeof(flagsBe));
			dwFrameDataPtr += sizeof(flagsBe);
			memcpy(&framedata[dwFrameDataPtr],data,dwSize);
			dwFrameDataPtr += dwSize;
		}
		else
		{
			unsigned char size[4];
			MakeV2Size(dwSize,size);
			// 長さ制限
			dwSize &= 0x0fffffff;
			// id3v2.4
			memcpy(&framedata[dwFrameDataPtr],&id,sizeof(id));
			dwFrameDataPtr += sizeof(id);
			memcpy(&framedata[dwFrameDataPtr],size,sizeof(size));
			dwFrameDataPtr += sizeof(size);
			memcpy(&framedata[dwFrameDataPtr],&flagsBe,sizeof(flagsBe));
			dwFrameDataPtr += sizeof(flagsBe);
			memcpy(&framedata[dwFrameDataPtr],data,dwSize);
			dwFrameDataPtr += dwSize;
		}
		p++;
	}
//	ASSERT(dwFrameDataPtr == dwTotalFrameSize);
	//非同期化
	if(m_bUnSynchronization)
	{
		unsigned char *encData = (unsigned char *)malloc(dwTotalFrameSize*2);
		DWORD dwEncodeSize = EncodeUnSynchronization(framedata,dwTotalFrameSize,encData);
//		if(dwEncodeSize != dwTotalFrameSize)
//	2004-03-20 プロパティに非同期化スイッチをつけたため、非同期化フラグは必ず立てることにした
//	(非同期化ONが保存できない様子はユーザからみておかしな動きに見えるため)
		{
			//非同期化フラグをセット
			m_head.flag |= 0x80;
			dwTotalFrameSize = dwEncodeSize;
		}
		free(framedata);
		framedata = encData;
	}
	else
	{
		//非同期化フラグを解除
		m_head.flag &= ~0x80;
	}

	//Id3tagサイズ
	DWORD dwId3Size = ExtractV2Size(m_head.size);

	if(dwTotalFrameSize > dwId3Size)
	{
		//[パディング領域を挿入]
		DWORD dwPaddingSize = dwTotalFrameSize - dwId3Size + ID3V2_PADDING_SIZE;
		dwId3Size += dwPaddingSize;

		//==================元ファイルをメモリに保存==================
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
			free(framedata);
			return dwWin32errorCode;
		}

		DWORD dwDataSize = GetFileSize(hFile,NULL);
		//バッファメモリの確保
		char *pRawData = (char *)malloc(dwDataSize);
		if(!pRawData)
		{
			dwWin32errorCode = GetLastError();
			CloseHandle(hFile);
			free(framedata);
			return dwWin32errorCode;
		}
		//raw dataの読み出し
		if(!ReadFile(hFile,pRawData,dwDataSize,&dwWritten,NULL))
		{
			dwWin32errorCode = GetLastError();
			CloseHandle(hFile);
			free(pRawData);
			free(framedata);
			return dwWin32errorCode;
		}
		CloseHandle(hFile);

		//==================テンポラリを作成==================
		//テンポラリ名を取得
		lstrcpy(szTempPath,szFileName);
		cutFileName(szTempPath);
		if(!GetTempFileName(szTempPath,_T("tms"),0,szTempFile))
		{
			dwWin32errorCode = GetLastError();
			free(pRawData);
			DeleteFile(szTempFile);
			free(framedata);
			return dwWin32errorCode;
		}
		//ファイルオープン(指定ファイルがすでに存在している場合、そのファイルは上書きされます。)
		hFile = CreateFile(szTempFile,GENERIC_WRITE|GENERIC_READ,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		if(hFile == INVALID_HANDLE_VALUE)
		{
			dwWin32errorCode = GetLastError();
			free(pRawData);
			DeleteFile(szTempFile);
			free(framedata);
			return dwWin32errorCode;
		}
		//おまじない
		if(SetFilePointer(hFile,0,NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		{
			dwWin32errorCode = GetLastError();
			CloseHandle(hFile);
			free(pRawData);
			DeleteFile(szTempFile);
			free(framedata);
			return dwWin32errorCode;
		}
		//パディング領域をスキップ
		if(SetFilePointer(hFile,dwPaddingSize,NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		{
			dwWin32errorCode = GetLastError();
			CloseHandle(hFile);
			free(pRawData);
			DeleteFile(szTempFile);
			free(framedata);
			return dwWin32errorCode;
		}
		//移動先にコピー
		if(WriteFile(hFile,pRawData,dwDataSize,&dwWritten,NULL) == 0)
		{
			dwWin32errorCode = GetLastError();
			CloseHandle(hFile);
			free(pRawData);
			DeleteFile(szTempFile);
			free(framedata);
			return dwWin32errorCode;
		}

		free(pRawData);

		bInsPadding = TRUE;
	}
	else
	{
		//ファイルをオープン
		hFile = CreateFile(
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
			free(framedata);
			return dwWin32errorCode;
		}
	}

	//ID3v2ヘッダを書き込む
	if(SetFilePointer(hFile,0,NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		DeleteFile(szTempFile);
		free(framedata);
		return dwWin32errorCode;
	}
	ID3HEAD head;
	m_head.ver[0] = (m_wVer & 0xff00) >> 8;
	m_head.ver[1] = (m_wVer & 0x00ff);
	memcpy(&head,&m_head,sizeof(m_head));
	MakeV2Size(dwId3Size,head.size);
	// 長さ制限
	dwId3Size &= 0x0fffffff;
	if(WriteFile(hFile,&head,sizeof(head),&dwWritten,NULL) == 0)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		DeleteFile(szTempFile);
		free(framedata);
		return dwWin32errorCode;
	}
	memcpy(&m_head,&head,sizeof(head));

	//フレーム情報をファイルに書き出す
	if(SetFilePointer(hFile,10,NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		DeleteFile(szTempFile);
		free(framedata);
		return dwWin32errorCode;
	}
	if(!WriteFile(hFile,framedata,dwTotalFrameSize,&dwWritten,NULL))
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		if(bInsPadding)
		{
			DeleteFile(szTempFile);
		}
		free(framedata);
		return dwWin32errorCode;
	}
	free(framedata);

	//パディング領域を0でパディング
	for(int i=0; i<(dwId3Size - dwTotalFrameSize); i++)
	{
		DWORD dwRet;
		unsigned char pad = 0x00;
		if(!WriteFile(hFile,&pad,1,&dwRet,NULL))
		{
			dwWin32errorCode = GetLastError();
			CloseHandle(hFile);
			if(bInsPadding)
			{
				DeleteFile(szTempFile);
			}
			return dwWin32errorCode;
		}
	}

	CloseHandle(hFile);

	if(bInsPadding)
	{
		//オリジナルファイルを退避(リネーム)
		TCHAR szPreFile[MAX_PATH];
		if(!GetTempFileName(szTempPath,_T("tms"),0,szPreFile))
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
	}

	return dwWin32errorCode;
}

DWORD CId3tagv2::DelTag(LPCTSTR szFileName)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;
	DWORD	dwWritten = 0;
	TCHAR szTempPath[MAX_PATH];
	TCHAR szTempFile[MAX_PATH];

	if(!m_bEnable)
	{
		return -1;
	}

	//==================元ファイルをメモリに保存==================
	HANDLE hFile = CreateFile(
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
	DWORD dwId3v2Size = ExtractV2Size(m_head.size) + 10;
	if(dwDataSize < dwId3v2Size)
	{
		return -1;
	}
	dwDataSize -= dwId3v2Size;
	SetFilePointer(hFile,dwId3v2Size,NULL,FILE_BEGIN);

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
		free(pRawData);
		CloseHandle(hFile);
		return dwWin32errorCode;
	}
	CloseHandle(hFile);

	//==================テンポラリを作成==================
	//テンポラリ名を取得
	lstrcpy(szTempPath,szFileName);
	cutFileName(szTempPath);
	if(!GetTempFileName(szTempPath,_T("tms"),0,szTempFile))
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
	//おまじない
	if(SetFilePointer(hFile,0,NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		free(pRawData);
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}
	//移動先にコピー
	if(WriteFile(hFile,pRawData,dwDataSize,&dwWritten,NULL) == 0)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		free(pRawData);
		return dwWin32errorCode;
	}
	free(pRawData);
	CloseHandle(hFile);

	//オリジナルファイルを退避(リネーム)
	TCHAR szPreFile[MAX_PATH];
	if(!GetTempFileName(szTempPath,_T("tms"),0,szPreFile))
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

DWORD CId3tagv2::MakeTag(LPCTSTR szFileName)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;
	if(m_bEnable)
	{
		return -1;
	}

	//デフォルト情報
	TCHAR szDefaultName[MAX_PATH];
	CString strFileName = szFileName;
	lstrcpy(szDefaultName,getFileName(strFileName));
	SetId3String("TIT2",szDefaultName);
	SetId3String("TSSE",m_strDefaultEnc);
	
	DWORD dwTotalFrameSize = GetTotalFrameSize();
	//[パディング領域を挿入]
	DWORD dwPaddingSize = ID3V2_PADDING_SIZE - dwTotalFrameSize;

	//フレーム情報を書き出す準備
	unsigned char *framedata = (unsigned char *)malloc(dwTotalFrameSize);
	if(!framedata)
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}
	DWORD dwFrameDataPtr = 0;
	multimap<DWORD,CId3Frame>::iterator p;
	p = m_frames.begin();
	while(p != m_frames.end())
	{
		CId3Frame *pFrame = &p->second;
		DWORD dwSize = pFrame->GetSize();
		DWORD id = pFrame->GetId();
		WORD flags = pFrame->GetFlags();
		WORD flagsBe = ((flags<<8)|(flags>>8));
		unsigned char *data = pFrame->GetData();
		if(m_wVer == 0x0200)
		{
			unsigned char size[3];
			size[2] = dwSize & 0xff;
			size[1] = (dwSize>>8) & 0xff;
			size[0] = (dwSize>>16) & 0xff;
			// id3v2.2
			char v22id[3];
			//v2.3からv2.2へフレームIDを変換
			v23IDtov22ID((char *)&id,v22id);
			memcpy(&framedata[dwFrameDataPtr],&v22id,sizeof(v22id));
			dwFrameDataPtr += sizeof(v22id);
			memcpy(&framedata[dwFrameDataPtr],size,sizeof(size));
			dwFrameDataPtr += sizeof(size);
			memcpy(&framedata[dwFrameDataPtr],data,dwSize);
			dwFrameDataPtr += dwSize;
		}
		else
		{
			// ID3v2.3-
			unsigned char size[4];
			size[3] = dwSize & 0xff;
			size[2] = (dwSize>>8) & 0xff;
			size[1] = (dwSize>>16) & 0xff;
			size[0] = (dwSize>>24) & 0xff;
			memcpy(&framedata[dwFrameDataPtr],&id,sizeof(id));
			dwFrameDataPtr += sizeof(id);
			memcpy(&framedata[dwFrameDataPtr],size,sizeof(size));
			dwFrameDataPtr += sizeof(size);
			memcpy(&framedata[dwFrameDataPtr],&flagsBe,sizeof(flagsBe));
			dwFrameDataPtr += sizeof(flagsBe);
			memcpy(&framedata[dwFrameDataPtr],data,dwSize);
			dwFrameDataPtr += dwSize;
		}
		p++;
	}
	ASSERT(dwFrameDataPtr == dwTotalFrameSize);
	//非同期化
	if(m_bUnSynchronization)
	{
		unsigned char *encData = (unsigned char *)malloc(dwTotalFrameSize*2);
		DWORD dwEncodeSize = EncodeUnSynchronization(framedata,dwTotalFrameSize,encData);
//		if(dwEncodeSize != dwTotalFrameSize)
//	2004-09-24 プロパティに非同期化スイッチをつけたため、非同期化フラグは必ず立てることにした
//	(非同期化ONが保存できない様子はユーザからみておかしな動きに見えるため)
		{
			//非同期化フラグをセット
			m_head.flag |= 0x80;
			dwTotalFrameSize = dwEncodeSize;
		}
		free(framedata);
		framedata = encData;
	}

	//==================元ファイルをメモリに保存==================
	HANDLE hFile = CreateFile(
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
		free(framedata);
		return dwWin32errorCode;
	}

	DWORD dwDataSize = GetFileSize(hFile,NULL);
	//バッファメモリの確保
	char *pRawData = (char *)malloc(dwDataSize);
	if(!pRawData)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		free(framedata);
		return dwWin32errorCode;
	}
	//raw dataの読み出し
	DWORD dwWritten;
	if(!ReadFile(hFile,pRawData,dwDataSize,&dwWritten,NULL))
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		free(pRawData);
		free(framedata);
		return dwWin32errorCode;
	}
	CloseHandle(hFile);

	//==================テンポラリを作成==================
	//テンポラリ名を取得
	TCHAR szTempPath[MAX_PATH];
	TCHAR szTempFile[MAX_PATH];
	lstrcpy(szTempPath,szFileName);
	cutFileName(szTempPath);
	if(!GetTempFileName(szTempPath,_T("tms"),0,szTempFile))
	{
		dwWin32errorCode = GetLastError();
		free(pRawData);
		DeleteFile(szTempFile);
		free(framedata);
		return dwWin32errorCode;
	}
	//ファイルオープン(指定ファイルがすでに存在している場合、そのファイルは上書きされます。)
	hFile = CreateFile(szTempFile,GENERIC_WRITE|GENERIC_READ,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		dwWin32errorCode = GetLastError();
		free(pRawData);
		DeleteFile(szTempFile);
		free(framedata);
		return dwWin32errorCode;
	}
	//おまじない
	if(SetFilePointer(hFile,0,NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		free(pRawData);
		DeleteFile(szTempFile);
		free(framedata);
		return dwWin32errorCode;
	}
	//パディング領域をスキップ
	if(SetFilePointer(hFile,dwPaddingSize + dwTotalFrameSize + 10,NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		free(pRawData);
		DeleteFile(szTempFile);
		free(framedata);
		return dwWin32errorCode;
	}
	//移動先にコピー
	if(WriteFile(hFile,pRawData,dwDataSize,&dwWritten,NULL) == 0)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		free(pRawData);
		free(framedata);
		return dwWin32errorCode;
	}
	free(pRawData);

	if(SetFilePointer(hFile,0,NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		DeleteFile(szTempFile);
		free(framedata);
		return dwWin32errorCode;
	}
	ID3HEAD head;
	m_head.ver[0] = (m_wVer & 0xff00) >> 8;
	m_head.ver[1] = (m_wVer & 0x00ff);
	memcpy(&head,&m_head,sizeof(m_head));
	MakeV2Size(dwPaddingSize+dwTotalFrameSize,head.size);
	if(WriteFile(hFile,&head,sizeof(head),&dwWritten,NULL) == 0)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		DeleteFile(szTempFile);
		free(framedata);
		return dwWin32errorCode;
	}
	memcpy(&m_head,&head,sizeof(head));

	//フレーム情報をファイルに書き出す
	if(!WriteFile(hFile,framedata,dwTotalFrameSize,&dwWritten,NULL))
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		DeleteFile(szTempFile);
		free(framedata);
		return dwWin32errorCode;
	}
	free(framedata);

	//パディング領域を0でパディング
	for(int i=0; i<dwPaddingSize; i++)
	{
		DWORD dwRet;
		unsigned char pad = 0x00;
		if(!WriteFile(hFile,&pad,1,&dwRet,NULL))
		{
			dwWin32errorCode = GetLastError();
			CloseHandle(hFile);
			DeleteFile(szTempFile);
			return dwWin32errorCode;
		}
	}
	CloseHandle(hFile);

	//オリジナルファイルを退避(リネーム)
	TCHAR szPreFile[MAX_PATH];
	if(!GetTempFileName(szTempPath,_T("tms"),0,szPreFile))
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

