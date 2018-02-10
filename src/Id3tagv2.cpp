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

static const char *numeric_frames[] = {
	"TBPM", "TDAT", "TDLY", "TIME", "TLEN", "TPOS", "TRCK", "TSIZ", "TYER",
	"TDEN", "TDOR", "TDRC", "TDRL", "TDTG",
	NULL
};

struct id3_v23v22table_t {
	char *v23;
	char *v22;
};
static const id3_v23v22table_t id3_v23v22table[] = {
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


// 2004-09-24 UTF-16はリトルエンディアンで書き込む
//#define UTF16_BIGENDIAN

//////////////////////////////////////////////////////////////////////

DWORD CId3Frame::LoadFrame2_4(const unsigned char *pData,DWORD dwSize)
{
	Release();
	if(dwSize < 10)
	{
		return 0;	//フレームヘッダがない場合は終了
	}
	DWORD size = CId3tagv2::ExtractV2Size(&pData[4]);
	if((size+10) > dwSize)
	{
		return 0;	//ヘッダサイズが入力データを超過している
	}
	DWORD dwId;
	memcpy(&dwId,pData,sizeof(dwId));
	if(!dwId)
	{
		return 0;	//無効なフレームID
	}
	m_dwId = dwId;
	m_wFlags = ExtractI2(&pData[8]);
	if (memcmp(&m_dwId, "APIC", sizeof(m_dwId)) == 0)
	{
		return LoadApicFrame(pData, size, 0x0400);
	}

	m_data = (unsigned char *)malloc(size);
	if(!m_data)
	{
		return 0;	//メモリを確保できなかった
	}
	m_dwSize = size;
	memcpy(m_data,&pData[10],size);
	if(m_wFlags & FLAG_UNSYNC)
	{
		m_dwSize = CId3tagv2::DecodeUnSynchronization(m_data, size);
	}
	return (size + 10);
}

DWORD CId3Frame::LoadFrame2_3(const unsigned char *pData,DWORD dwSize)
{
	Release();
	if(dwSize < 10)
	{
		return 0;	//フレームヘッダがない場合は終了
	}
	DWORD size = ExtractI4(&pData[4]);
	if((size+10) > dwSize)
	{
		return 0;	//ヘッダサイズが入力データを超過している
	}
	DWORD dwId;
	memcpy(&dwId,pData,sizeof(dwId));
	//	BYTE id[5];
	//	memcpy(id,pData,sizeof(dwId));
	//	id[4] = '\0';
	//	TRACE(_T("id=%s (size=%d)\n"),id,size);
	if(!dwId)
	{
		return 0;	//無効なフレームID
	}
	m_dwId = dwId;
	WORD wFlags = ExtractI2(&pData[8]);
	// Convert the flags to v2.4 format.
	m_wFlags = ((wFlags & 0xe000) >> 1) | ((wFlags & 0xc0) >> 4) | ((wFlags & 0x20) << 1);
	if (memcmp(&m_dwId, "APIC", sizeof(m_dwId)) == 0)
	{
		return LoadApicFrame(pData, size, 0x0300);
	}

	m_data = (unsigned char *)malloc(size);
	if(!m_data)
	{
		return 0;	//メモリを確保できなかった
	}
	m_dwSize = size;
	memcpy(m_data,&pData[10],size);
	return (size + 10);
}

DWORD CId3Frame::LoadFrame2_2(const unsigned char *pData,DWORD dwSize)
{
	Release();
	if(dwSize < 6)
	{
		return 0;	//フレームヘッダがない場合は終了
	}
	DWORD size = (((DWORD )pData[3]<<16) | ((DWORD )pData[4]<<8) | (DWORD )pData[5]);
	if((size+6) > dwSize)
	{
		return 0;	//ヘッダサイズが入力データを超過している
	}
	BYTE id[3+1];
	memcpy(&id,pData,sizeof(id));
	id[3] = '\0';
	TRACE(_T("id=%s (size=%d)\n"),id,size);
	//v2.2からv2.3へフレームIDを変換
	int i;
	for (i = 0; id3_v23v22table[i].v23 != NULL; i++) {
		if (memcmp(id, id3_v23v22table[i].v22, sizeof(id)) == 0) {
			memcpy(&m_dwId, id3_v23v22table[i].v23, sizeof(m_dwId));
			break;
		}
	}
	if (id3_v23v22table[i].v23 == NULL) {
	//	memcpy(&m_dwId, "XXXX", sizeof(m_dwId));	// 不明
		return 0;	//無効なフレームID
	}
	m_wFlags = 0;	//v2.2
	if (memcmp(&m_dwId, "APIC", sizeof(m_dwId)) == 0)
	{
		return LoadApicFrame(pData, size, 0x0200);
	}

	m_data = (unsigned char *)malloc(size);
	if(!m_data)
	{
		return 0;	//メモリを確保できなかった
	}
	m_dwSize = size;
	memcpy(m_data,&pData[6],size);
	return (size + 6);
}

DWORD CId3Frame::LoadApicFrame(const unsigned char *pData, DWORD dwSize, WORD wVer)
{
	DWORD dwAddSize = 0;
	int hdrsize = (wVer >= 0x0300) ? 10 : 6;
	pData += hdrsize;

	if(memcmp(&pData[1], "JPG", 3) == 0 || memcmp(&pData[1], "PNG", 3) == 0)
	{
		// v2.2 format
		const char *mime;
		if(memcmp(&pData[1], "JPG", 3) == 0)
		{
			mime = "image/jpeg";
		}
		else
		{
			mime = "image/png";
		}
		int len = strlen(mime) + 1;
		dwAddSize = len - 3;
		m_data = (unsigned char *)malloc(dwSize + dwAddSize);
		if(!m_data)
		{
			return 0;	//メモリを確保できなかった
		}
		m_data[0] = pData[0];	// text encoding
		memcpy(&m_data[1], mime, len);
		memcpy(&m_data[1+len], &pData[1+3], dwSize-4);
	}
	else
	{
		// v2.3 (v2.4) format
		m_data = (unsigned char *)malloc(dwSize);
		if(!m_data)
		{
			return 0;	//メモリを確保できなかった
		}
		memcpy(m_data, pData, dwSize);
	}

	m_dwSize = dwSize + dwAddSize;
	if((wVer == 0x0400) && (m_wFlags & FLAG_UNSYNC))
	{
		m_dwSize = CId3tagv2::DecodeUnSynchronization(m_data, m_dwSize);
	}
	return dwSize + hdrsize;
}

DWORD CId3Frame::GetDataOffset() const
{
	DWORD offset = 0;
	if(m_wFlags & FLAG_GROUP)
	{
		offset++;
	}
	if(m_wFlags & FLAG_ENCRYPT)
	{
		offset++;
	}
	if(m_wFlags & (FLAG_DATA_LEN | FLAG_COMPRESS))
	{
		offset += 4;
	}
	return offset;
}

bool CId3Frame::IsTextFrame() const
{
	return (m_dwId & 0xff) == 'T';
}

bool CId3Frame::IsNumericFrame() const
{
	for (int i = 0; numeric_frames[i] != NULL; i++) {
		if (memcmp(&m_dwId, numeric_frames[i], sizeof(m_dwId)) == 0) {
			return true;
		}
	}
	return false;
}


//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CId3tagv2::CId3tagv2()
{
	m_encoding = ID3V2CHARENCODING_UTF_16;
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
		const unsigned char *data, int datasize, DWORD *pdwReadSize)
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
	const unsigned char *start = data;
	switch (encoding) {
	case ID3V2CHARENCODING_ISO_8859_1:
		code = DTC_CODE_ANSI;
		break;
	case ID3V2CHARENCODING_UTF_16:
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
	case ID3V2CHARENCODING_UTF_16BE:
		code = DTC_CODE_UTF16BE;
		break;
	case ID3V2CHARENCODING_UTF_8:
		code = DTC_CODE_UTF8;
		break;
	}
	if ((encoding == ID3V2CHARENCODING_UTF_16) || (encoding == ID3V2CHARENCODING_UTF_16BE)) {
		for (; readsize < datasize; readsize += 2) {
			if (*(LPCWSTR)(data + readsize) == L'\0') {
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
	std::pair<FrameMap::iterator, FrameMap::iterator> pp;
	FrameMap::iterator p;
	DWORD dwId;
	DWORD dwReadSize;
	unsigned char *data;
	DWORD i;
	const WORD frameMask = CId3Frame::MASK_FRAME_FORMAT
		& ~(CId3Frame::FLAG_GROUP | CId3Frame::FLAG_DATA_LEN);
	DWORD offset = 0;
	switch(szId[0]){
	case 'T':	//テキスト情報フレーム
		memcpy(&dwId,szId,sizeof(dwId));
		pp = m_frames.equal_range(dwId);
		if(pp.first == pp.second)
		{
			break;
		}
		p = pp.first;
		if(p->second.GetSize() == 0 || (p->second.GetFlags() & frameMask))
		{
			break;
		}
		offset = p->second.GetDataOffset();
		data = p->second.GetData();
		return ReadEncodedTextString(data[offset], &data[offset+1], p->second.GetSize()-offset-1, NULL);
	case 'W':	//URLリンクフレームx
		if(strcmp(szId,"WXXX") != 0)
		{
			break;
		}
		memcpy(&dwId,szId,sizeof(dwId));
		pp = m_frames.equal_range(dwId);
		if(pp.first == pp.second)
		{
			break;
		}
		p = pp.first;
		if(p->second.GetSize() == 0 || (p->second.GetFlags() & frameMask))
		{
			break;
		}
		offset = p->second.GetDataOffset();
		data = p->second.GetData();
		
		//説明文を読み飛ばす
		ReadEncodedTextString(data[offset], &data[offset+1], p->second.GetSize()-offset-1, &dwReadSize);
		//URL本体
		//規格上は常に ISO-8859-1（互換性のため、BOMがあればそちらを優先）
		if ((p->second.GetSize()-1-dwReadSize >= 2)
				&& (data[0] == ID3V2CHARENCODING_UTF_16)
				&& ((memcmp(&data[1+dwReadSize], "\xff\xfe", 2) == 0)
					|| (memcmp(&data[1+dwReadSize], "\xfe\xff", 2) == 0))) {
			return ReadEncodedTextString(1, &data[offset+1+dwReadSize], p->second.GetSize()-offset-1-dwReadSize, NULL);
		}
		return ReadEncodedTextString(0, &data[offset+1+dwReadSize], p->second.GetSize()-offset-1-dwReadSize, NULL);
	case 'C':
		if(strcmp(szId,"COMM") != 0)
		{
			break;
		}
		memcpy(&dwId,szId,sizeof(dwId));
		for (pp = m_frames.equal_range(dwId); pp.first != pp.second; ++pp.first)
		{
			p = pp.first;
			if(p->second.GetSize() == 0 || (p->second.GetFlags() & frameMask))
			{
				continue;
			}
			offset = p->second.GetDataOffset();
			data = p->second.GetData();

			// 言語文字列読み飛ばし、説明文取得
			CString desc = ReadEncodedTextString(data[offset], &data[offset+4], p->second.GetSize()-offset-4, &dwReadSize);
			if(desc.Left(4) == _T("iTun"))
			{
				// iTunes固有のコメントはスキップ (iTunNORM, iTunSMPB, etc.)
				continue;
			}
			//本文
			return ReadEncodedTextString(data[offset], &data[offset+4+dwReadSize], p->second.GetSize()-offset-4-dwReadSize, NULL);
		}
		break;
	}
	return _T("");
}

void CId3tagv2::SetId3String(const char szId[],LPCTSTR szString,LPCTSTR szDescription)
{
	std::pair<FrameMap::iterator, FrameMap::iterator> pp;
	FrameMap::iterator p;
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
	int i;
	CharEncoding encoding;
	switch(szId[0]){
	case 'T':	//テキスト情報フレーム
		encoding = m_encoding;
		//数字文字列はISO-8859-1
		for (i = 0; numeric_frames[i] != NULL; i++) {
			if (strcmp(szId, numeric_frames[i]) == 0) {
				encoding = ID3V2CHARENCODING_ISO_8859_1;
				break;
			}
		}
		switch(encoding){
		case ID3V2CHARENCODING_ISO_8859_1:
		default:	// ISO-8859-1
			data = (unsigned char *)TstrToDataAlloc(szString, -1, &size, DTC_CODE_ANSI, 1);
			if(!data)
			{
				return;
			}
			data[0] = 0;	//encoding
			break;
		case ID3V2CHARENCODING_UTF_16:	// UTF-16
#ifndef UTF16_BIGENDIAN
			data = (unsigned char *)TstrToDataAlloc(szString, -1, &size, DTC_CODE_UTF16LE, 3);
			if(!data)
			{
				return;
			}
			data[0] = 1;	//encoding
			data[1] = 0xff;	//BOM
			data[2] = 0xfe;
			break;
#else
			data = (unsigned char *)TstrToDataAlloc(szString, -1, &size, DTC_CODE_UTF16BE, 3);
			if(!data)
			{
				return;
			}
			data[0] = 1;	//encoding
			data[1] = 0xfe;	//BOM
			data[2] = 0xff;
			break;
#endif
		case ID3V2CHARENCODING_UTF_16BE:	// UTF-16BE
			data = (unsigned char *)TstrToDataAlloc(szString, -1, &size, DTC_CODE_UTF16BE, 1);
			if(!data)
			{
				return;
			}
			data[0] = 2;	//encoding
			break;
		case ID3V2CHARENCODING_UTF_8:	// UTF-8
			data = (unsigned char *)TstrToDataAlloc(szString, -1, &size, DTC_CODE_UTF8, 1);
			if(!data)
			{
				return;
			}
			data[0] = 3;	//encoding
			break;
		}
//		p = m_frames.find(dwId);
		pp = m_frames.equal_range(dwId);
		if((pp.first == pp.second) || !(pp.first->second.GetSize()))
		{
			//Loadしたファイルにフレームがなかった場合
			CId3Frame frame;
			frame.SetId(dwId);
			frame.SetFlags(0);
			frame.SetData(data,size);
			m_frames.insert(std::pair<DWORD,CId3Frame>(frame.GetId(),frame));
		}
		else
		{
			p = pp.first;
			pFrame = &p->second;
			pFrame->SetFlags(0);
			pFrame->SetData(data,size);
		}
		free(data);
		break;
	case 'W':	//URLリンクフレームx
		if(strcmp(szId,"WXXX") != 0)
		{
			break;
		}
		switch(m_encoding){
		case ID3V2CHARENCODING_ISO_8859_1:
		default:	// ISO-8859-1
			data = (unsigned char *)TstrToDataAlloc(szString, -1, &size, DTC_CODE_ANSI, 2);	//URL本体（常にISO-8859-1）
			if(!data)
			{
				return;
			}
			data[0] = 0;	//encoding
			data[1] = 0;	//説明文(省略)
			break;
		case ID3V2CHARENCODING_UTF_16:	// UTF-16
#ifndef UTF16_BIGENDIAN
			data = (unsigned char *)TstrToDataAlloc(szString, -1, &size, DTC_CODE_ANSI, 5);	//URL本体（常にISO-8859-1）
			if(!data)
			{
				return;
			}
			data[0] = 1;	//encoding
			data[1] = 0xff;	//BOM
			data[2] = 0xfe;
			data[3] = 0;	//説明文(省略)
			data[4] = 0;
			break;
#else	// ビックエンディアン
			data = (unsigned char *)TstrToDataAlloc(szString, -1, &size, DTC_CODE_ANSI, 5);	//URL本体（常にISO-8859-1）
			if(!data)
			{
				return;
			}
			data[0] = 1;	//encoding
			data[1] = 0xfe;	//BOM
			data[2] = 0xff;
			data[3] = 0;	//説明文(省略)
			data[4] = 0;
			break;
#endif
		case ID3V2CHARENCODING_UTF_16BE:	// UTF-16BE
			data = (unsigned char *)TstrToDataAlloc(szString, -1, &size, DTC_CODE_ANSI, 3);	//URL本体（常にISO-8859-1）
			if(!data)
			{
				return;
			}
			data[0] = 2;	//encoding
			data[1] = 0;	//説明文(省略)
			data[2] = 0;
			break;
		case ID3V2CHARENCODING_UTF_8:	// UTF-8
			data = (unsigned char *)TstrToDataAlloc(szString, -1, &size, DTC_CODE_ANSI, 2);	//URL本体（常にISO-8859-1）
			if(!data)
			{
				return;
			}
			data[0] = 3;	//encoding
			data[1] = 0;	//説明文(省略)
			break;
		}
		pp = m_frames.equal_range(dwId);
		if((pp.first == pp.second) || !(pp.first->second.GetSize()))
		{
			//Loadしたファイルにフレームがなかった場合
			CId3Frame frame;
			frame.SetId(dwId);
			frame.SetFlags(0);
			frame.SetData(data,size);
			m_frames.insert(std::pair<DWORD,CId3Frame>(frame.GetId(),frame));
		}
		else
		{
			p = pp.first;
			pFrame = &p->second;
			pFrame->SetFlags(0);
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
			const WORD frameMask = CId3Frame::MASK_FRAME_FORMAT
				& ~(CId3Frame::FLAG_GROUP | CId3Frame::FLAG_DATA_LEN);
			if(p->second.GetSize() == 0 || (p->second.GetFlags() & frameMask))
			{
				continue;
			}
			DWORD offset = p->second.GetDataOffset();
			unsigned char *data = p->second.GetData();

			// 言語文字列読み飛ばし、説明文取得
			DWORD dwReadSize;
			CString desc = ReadEncodedTextString(data[offset], &data[offset+4], p->second.GetSize()-offset-4, &dwReadSize);
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
		switch(m_encoding){
		case ID3V2CHARENCODING_ISO_8859_1:
		default:	// ISO-8859-1
			data = (unsigned char *)TstrToDataAlloc(szString, -1, &size, DTC_CODE_ANSI, 5);
			if(!data)
			{
				return;
			}
			data[0] = 0;	//encoding
			data[1] = 'e';	//Language
			data[2] = 'n';
			data[3] = 'g';
			data[4] = 0;	//説明文(省略)
			break;
		case ID3V2CHARENCODING_UTF_16:	// UTF-16
#ifndef UTF16_BIGENDIAN
			data = (unsigned char *)TstrToDataAlloc(szString, -1, &size, DTC_CODE_UTF16LE, 10);
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
			break;
#else	// ビッグエンディアン
			data = (unsigned char *)TstrToDataAlloc(szString, -1, &size, DTC_CODE_UTF16BE, 10);
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
			break;
#endif
		case ID3V2CHARENCODING_UTF_16BE:	// UTF-16BE
			data = (unsigned char *)TstrToDataAlloc(szString, -1, &size, DTC_CODE_UTF16BE, 6);
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
			break;
		case ID3V2CHARENCODING_UTF_8:	// UTF-8
			data = (unsigned char *)TstrToDataAlloc(szString, -1, &size, DTC_CODE_UTF8, 5);
			if(!data)
			{
				return;
			}
			data[0] = 3;	//encoding
			data[1] = 'e';	//Language
			data[2] = 'n';
			data[3] = 'g';
			data[4] = 0;	//説明文(省略)
			break;
		}
	//	pp = m_frames.equal_range(dwId);
		if((pp.first == pp.second) || !(pp.first->second.GetSize()))
		{
			CId3Frame frame;
			frame.SetId(dwId);
			frame.SetFlags(0);
			frame.SetData(data,size);
			m_frames.insert(std::pair<DWORD,CId3Frame>(frame.GetId(),frame));
		}
		else
		{
			p = pp.first;
			pFrame = &p->second;
			pFrame->SetFlags(0);
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
	FrameMap::iterator p;

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

#if 0
// 全てのフレームの文字列エンコードを指定する
void CId3tagv2::_SetStringEncoding(int encoding)
{
	FrameMap::iterator p;

	p = m_frames.begin();
	while(p != m_frames.end())
	{
		CId3Frame *pFrame = &p->second;
		unsigned char *data = pFrame->GetData();
		
		switch(encoding)
		{
		default:
		case ID3V2CHARENCODING_ISO_8859_1:
			data[0] = 0;
			break;
		case ID3V2CHARENCODING_UTF_16:
			data[0] = 1;
			break;
		case ID3V2CHARENCODING_UTF_16BE:
			data[0] = 2;
			break;
		case ID3V2CHARENCODING_UTF_8:
			data[0] = 3;
			break;
		}

		p++;
	}
}
#endif

void CId3tagv2::v23IDtov22ID(const char *v23ID,char *v22ID)
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

CString CId3tagv2::GetDiscNo()
{
	//Disc number (Part of a set)
	return GetId3String("TPOS");
}

void CId3tagv2::SetDiscNo(LPCTSTR szDiscNo)
{
	//Disc number (Part of a set)
	SetId3String("TPOS",szDiscNo);
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
	//西暦(および月日)
	if(m_wVer < 0x0400)
	{
		CString strYear = GetId3String("TYER");
		CString strDate = GetId3String("TDAT");
		if(strDate.GetLength() == 4)
		{
			if(strDate.Left(2) == _T("00"))
			{
				// YYYY-MM
				strYear += _T("-") + strDate.Right(2);
			}
			else
			{
				// YYYY-MM-DD
				strYear += _T("-") + strDate.Right(2) + _T("-") + strDate.Left(2);
			}
		}
		return strYear;
	}
	else
	{
		return GetId3String("TDRC");
	}
}

void CId3tagv2::SetYear(LPCTSTR year)
{
	//西暦(および月日)
	if(m_wVer < 0x0400)
	{
		SetId3String("TDRC",_T(""));
		CString strYear = year;
		if(strYear.GetLength() == 7 && strYear[4] == _T('-'))
		{
			// YYYY-MM -> YYYY, 00MM
			SetId3String("TYER",strYear.Left(4));
			CString strDate = _T("00") + strYear.Right(2);
			SetId3String("TDAT",strDate);
		}
		else if(strYear.GetLength() >= 10
				&& strYear[4] == _T('-') && strYear[7] == _T('-'))
		{
			// YYYY-MM-DD -> YYYY, DDMM
			SetId3String("TYER",strYear.Left(4));
			CString strDate = strYear.Mid(8,2) + strYear.Mid(5,2);
			SetId3String("TDAT",strDate);
		}
		else
		{
			// YYYY or unknown format
			SetId3String("TYER",strYear);
			SetId3String("TDAT",_T(""));
		}
	}
	else
	{
		SetId3String("TDRC",year);
		SetId3String("TYER",_T(""));
		SetId3String("TDAT",_T(""));
	}
}

CString CId3tagv2::GetGenre()
{
	//ジャンル
	CString strGenre = GetId3String("TCON");
	int genre = -1;
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
	if ((genre >= 0) && strGenre.IsEmpty()) {
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

CString CId3tagv2::GetEncodedBy()
{
	//エンコードした人または組織
	return GetId3String("TENC");
}

void CId3tagv2::SetEncodedBy(LPCTSTR encoder)
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

DWORD CId3tagv2::EncodeUnSynchronization(const unsigned char *srcData,DWORD dwSize,unsigned char *dstData)
{
	DWORD dwEncodeSize = 0;
	unsigned char *writePtr = dstData;
	BOOL bHitFF = FALSE;

	for(DWORD i=0; i<dwSize; i++)
	{
		if(bHitFF && (((srcData[i]&0xe0) == 0xe0) || (srcData[i] == 0x00)) )
		{
			writePtr[dwEncodeSize] = 0x00;
			dwEncodeSize++;
		}
		if(srcData[i] == 0xff)
		{
			bHitFF = TRUE;
		}
		else
		{
			bHitFF = FALSE;
		}
		writePtr[dwEncodeSize] = srcData[i];
		dwEncodeSize++;
	}
	return dwEncodeSize;
}

void CId3tagv2::MakeV2Size(DWORD dwSize,unsigned char size[4])
{
	size[3] = dwSize & 0x7f;
	size[2] = (dwSize>>7) & 0x7f;
	size[1] = (dwSize>>14) & 0x7f;
	size[0] = (dwSize>>21) & 0x7f;
}

CId3tagv2::CharEncoding CId3tagv2::GetFrameEncoding(const CId3Frame &frame)
{
	const WORD frameMask = CId3Frame::MASK_FRAME_FORMAT
		& ~(CId3Frame::FLAG_GROUP | CId3Frame::FLAG_DATA_LEN);
	const unsigned char *data = frame.GetData();
	if (!data || frame.GetSize() == 0 || (frame.GetFlags() & frameMask))
	{
		return ID3V2CHARENCODING_UNSPECIFIED;
	}
	if (!frame.IsTextFrame() || frame.IsNumericFrame())
	{
		return ID3V2CHARENCODING_UNSPECIFIED;
	}
	return (CharEncoding)data[frame.GetDataOffset()];
}

DWORD CId3tagv2::Load(LPCTSTR szFileName)
{
	bool unsyncTag = true;	// Decode unsynchronization for whole tag.
retry:
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
	if(head.flag & HDR_FLAG_UNSYNC)
	{
		if(unsyncTag)
		{
			// v2.3 or earlier, or v2.4 with incorrect (tag level) unsynchronization
			dwId3Size = DecodeUnSynchronization(buf,dwId3Size);
		}
		m_bUnSynchronization = TRUE;
	}
	else
	{
		m_bUnSynchronization = FALSE;
	}

	DWORD dwRemainSize = dwId3Size;
	//拡張ヘッダを読み飛ばす(ID3v2.2には存在しない)
	if((ver != 0x0200) && (head.flag & HDR_FLAG_EXT_HEADER))
	{
		if(ver < 0x400)
		{
			dwRemainSize -= ExtractI4(buf) + 4;
		}
		else
		{
			dwRemainSize -= ExtractV2Size(buf);
		}
	}
	head.flag &= ~HDR_FLAG_EXT_HEADER;	//解除

	m_encoding = ID3V2CHARENCODING_UTF_16;
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
			if(frame.GetFlags() & CId3Frame::FLAG_UNSYNC)
			{
				// v2.4 with correct (per-frame) unsynchronization is found.
				if(unsyncTag)
				{
					// Retry with the correct method.
					unsyncTag = false;
					free(buf);
					CloseHandle(hFile);
					goto retry;
				}
				m_bUnSynchronization = TRUE;
				frame.SetFlags(frame.GetFlags() & ~CId3Frame::FLAG_UNSYNC);	// Drop unsync flag.
			}
		}
		if(!dwReadSize)
			break;
		CharEncoding enc = GetFrameEncoding(frame);
		if(enc != ID3V2CHARENCODING_UNSPECIFIED)
		{
			if((m_wVer < 0x0400) && (enc > ID3V2CHARENCODING_UTF_16))
			{
				// v2.3 or earlier doesn't support UTF-8 or UTF-16BE.
				m_encoding = ID3V2CHARENCODING_UTF_16;
			}
			else
			{
				m_encoding = enc;
			}
		}
		m_frames.insert(std::pair<DWORD,CId3Frame>(frame.GetId(),frame));
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

// Convert an APIC frame in v2.3 format without header to a PIC frame v2.2
// format with header.  Return output size.
DWORD CId3tagv2::ConvertApicToV22(const unsigned char *v23, DWORD dwSize, unsigned char *v22)
{
	DWORD dwDiff;
	const char *format;
	if (strcmp((char*)&v23[1], "image/jpeg") == 0)
	{
		format = "JPG";
		dwDiff = 11 - 3;
	}
	else if (strcmp((char*)&v23[1], "image/png") == 0)
	{
		format = "PNG";
		dwDiff = 10 - 3;
	}
	else
	{
		return 0;
	}

	DWORD offset = 0;
	DWORD dwNewSize = dwSize - dwDiff;
	unsigned char size[3];
	size[2] = dwNewSize & 0xff;
	size[1] = (dwNewSize>>8) & 0xff;
	size[0] = (dwNewSize>>16) & 0xff;
	memcpy(&v22[offset], "PIC", 3);
	offset += 3;
	memcpy(&v22[offset], size, sizeof(size));
	offset += sizeof(size);
	v22[offset] = v23[0];	// text encoding
	offset += 1;
	memcpy(&v22[offset], format, 3);
	offset += 3;
	memcpy(&v22[offset], &v23[1 + dwDiff + 3], dwNewSize - 1 - 3);

	return dwNewSize + 6;
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
		if(	(m_encoding != ID3V2CHARENCODING_ISO_8859_1) &&
			(m_encoding != ID3V2CHARENCODING_UTF_16) )
		{
			// ISO-8859-1に自動設定
			m_encoding = ID3V2CHARENCODING_ISO_8859_1;
		}
	}

	// 全フレームの文字列エンコードを設定
	_SetStringEncoding(m_encoding);
*/

	DWORD dwTotalFrameSize = GetTotalFrameSize();
	//フレーム情報を書き出す準備
	unsigned char *framedata;
	if(m_wVer == 0x400 && m_bUnSynchronization)
	{
		framedata = (unsigned char *)malloc(dwTotalFrameSize * 2);
	}
	else
	{
		framedata = (unsigned char *)malloc(dwTotalFrameSize);
	}
	if(!framedata)
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}
	DWORD dwFrameDataPtr = 0;
	FrameMap::iterator p;
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
		unsigned char *data = pFrame->GetData();
		if(m_wVer == 0x0200)
		{
			const WORD frameMask = CId3Frame::MASK_FRAME_FORMAT
				& ~(CId3Frame::FLAG_GROUP | CId3Frame::FLAG_DATA_LEN);
			if(flags & (CId3Frame::FLAG_TAG_ALT_PRESERVE | frameMask))
			{
				// Should be discarded or cannot convert to v2.2.
				p++;
				continue;
			}
			DWORD offset = pFrame->GetDataOffset();
			dwSize -= offset;
			if(memcmp(&id, "APIC", 4) == 0)
			{
				dwFrameDataPtr += ConvertApicToV22(&data[offset], dwSize, &framedata[dwFrameDataPtr]);
			}
			else
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
				memcpy(&framedata[dwFrameDataPtr],&data[offset],dwSize);
				dwFrameDataPtr += dwSize;
			}
		}
		else if(m_wVer == 0x0300)
		{
			if(flags & CId3Frame::FLAG_TAG_ALT_PRESERVE)
			{
				// Should be discarded
				p++;
				continue;
			}
			DWORD offset = 0;
			if((flags & (CId3Frame::FLAG_DATA_LEN | CId3Frame::FLAG_COMPRESS | CId3Frame::FLAG_ENCRYPT)) == CId3Frame::FLAG_DATA_LEN)
			{
				// TODO: Do we need to care about grouping identity flag?
				// The order of grouping and data length are different between
				// v2.3 and v2.4.  Drop it for now.
				flags &= ~CId3Frame::FLAG_GROUP;

				offset = pFrame->GetDataOffset();
				dwSize -= offset;
			}
			flags = ((flags & 0x7000) << 1) | ((flags & 0x0c) << 4) | ((flags & 0x40) >> 1);
			WORD flagsBe = ((flags<<8)|(flags>>8));
			unsigned char size[4];
			size[3] = dwSize & 0xff;
			size[2] = (dwSize>>8) & 0xff;
			size[1] = (dwSize>>16) & 0xff;
			size[0] = (dwSize>>24) & 0xff;
			// id3v2.3
			memcpy(&framedata[dwFrameDataPtr],&id,sizeof(id));
			dwFrameDataPtr += sizeof(id);
			memcpy(&framedata[dwFrameDataPtr],size,sizeof(size));
			dwFrameDataPtr += sizeof(size);
			memcpy(&framedata[dwFrameDataPtr],&flagsBe,sizeof(flagsBe));
			dwFrameDataPtr += sizeof(flagsBe);
			memcpy(&framedata[dwFrameDataPtr],&data[offset],dwSize);
			dwFrameDataPtr += dwSize;
		}
		else
		{
			if(flags & CId3Frame::FLAG_TAG_ALT_PRESERVE)
			{
				// Should be discarded.
				p++;
				continue;
			}
			WORD flagsBe = ((flags<<8)|(flags>>8));
			unsigned char size[4];
			MakeV2Size(dwSize,size);
			// 長さ制限
			dwSize &= 0x0fffffff;
			// id3v2.4
			memcpy(&framedata[dwFrameDataPtr],&id,sizeof(id));
			dwFrameDataPtr += sizeof(id);

			if(m_bUnSynchronization)
			{
				DWORD dwEncodeSize = EncodeUnSynchronization(data, dwSize, &framedata[dwFrameDataPtr+sizeof(size)+sizeof(flagsBe)]);
				if(dwEncodeSize != dwSize)
				{
					dwSize = dwEncodeSize;
					MakeV2Size(dwSize,size);
					flagsBe |= CId3Frame::FLAG_UNSYNC << 8;	// unsync
				}
			}
			else
			{
				memcpy(&framedata[dwFrameDataPtr+sizeof(size)+sizeof(flagsBe)], data, dwSize);
			}

			memcpy(&framedata[dwFrameDataPtr],size,sizeof(size));
			dwFrameDataPtr += sizeof(size);
			memcpy(&framedata[dwFrameDataPtr],&flagsBe,sizeof(flagsBe));
			dwFrameDataPtr += sizeof(flagsBe);
			dwFrameDataPtr += dwSize;
		}
		p++;
	}
	dwTotalFrameSize = dwFrameDataPtr;
//	ASSERT(dwFrameDataPtr == dwTotalFrameSize);
	//非同期化
	if(m_wVer == 0x400 && m_bUnSynchronization)
	{
		//非同期化フラグをセット
		m_head.flag |= HDR_FLAG_UNSYNC;
	}
	else if(m_bUnSynchronization)
	{
		unsigned char *encData = (unsigned char *)malloc(dwTotalFrameSize*2);
		DWORD dwEncodeSize = EncodeUnSynchronization(framedata,dwTotalFrameSize,encData);
//		if(dwEncodeSize != dwTotalFrameSize)
//	2004-03-20 プロパティに非同期化スイッチをつけたため、非同期化フラグは必ず立てることにした
//	(非同期化ONが保存できない様子はユーザからみておかしな動きに見えるため)
		{
			//非同期化フラグをセット
			m_head.flag |= HDR_FLAG_UNSYNC;
			dwTotalFrameSize = dwEncodeSize;
		}
		free(framedata);
		framedata = encData;
	}
	else
	{
		//非同期化フラグを解除
		m_head.flag &= ~HDR_FLAG_UNSYNC;
	}
	// Drop footer flag.
	m_head.flag &= ~HDR_FLAG_FOOTER;

	//Id3tagサイズ
	DWORD dwId3Size = ExtractV2Size(m_head.size);

	if(dwTotalFrameSize > dwId3Size)
	{
		//[パディング領域を挿入]
		DWORD dwPaddingSize = (ID3V2_PADDING_SIZE + dwTotalFrameSize - 1) / ID3V2_PADDING_SIZE * ID3V2_PADDING_SIZE - dwId3Size;
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
	lstrcpy(szDefaultName,getFileName(CString(szFileName)));
	SetId3String("TIT2",szDefaultName);
	SetId3String("TSSE",m_strDefaultEnc);
	
	DWORD dwTotalFrameSize = GetTotalFrameSize();
	//[パディング領域を挿入]
	DWORD dwPaddingSize = (ID3V2_PADDING_SIZE + dwTotalFrameSize - 1) / ID3V2_PADDING_SIZE * ID3V2_PADDING_SIZE - dwTotalFrameSize;

	//フレーム情報を書き出す準備
	unsigned char *framedata = (unsigned char *)malloc(dwTotalFrameSize);
	if(!framedata)
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}
	DWORD dwFrameDataPtr = 0;
	FrameMap::iterator p;
	p = m_frames.begin();
	while(p != m_frames.end())
	{
		CId3Frame *pFrame = &p->second;
		DWORD dwSize = pFrame->GetSize();
		DWORD id = pFrame->GetId();
		WORD flags = pFrame->GetFlags();
		unsigned char *data = pFrame->GetData();
		if(m_wVer == 0x0200)
		{
#if 0
			if(memcmp(&id, "APIC", 4) == 0)
			{
				dwFrameDataPtr += ConvertApicToV22(data, dwSize, &framedata[dwFrameDataPtr]);
			}
			else
#endif
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
		}
		else if(m_wVer == 0x0300)
		{
			flags = ((flags & 0x7000) << 1) | ((flags & 0x0c) << 4) | ((flags & 0x40) >> 1);
			WORD flagsBe = ((flags<<8)|(flags>>8));
			// ID3v2.3
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
		else
		{
			WORD flagsBe = ((flags<<8)|(flags>>8));
			unsigned char size[4];
			MakeV2Size(dwSize,size);
			// 長さ制限
			dwSize &= 0x0fffffff;
			// id3v2.4
			memcpy(&framedata[dwFrameDataPtr],&id,sizeof(id));
			dwFrameDataPtr += sizeof(id);

			if(m_bUnSynchronization)
			{
				DWORD dwEncodeSize = EncodeUnSynchronization(data, dwSize, &framedata[dwFrameDataPtr+sizeof(size)+sizeof(flagsBe)]);
				if(dwEncodeSize != dwSize)
				{
					dwSize = dwEncodeSize;
					MakeV2Size(dwSize,size);
					flagsBe |= CId3Frame::FLAG_UNSYNC << 8;	// unsync
				}
			}
			else
			{
				memcpy(&framedata[dwFrameDataPtr+sizeof(size)+sizeof(flagsBe)], data, dwSize);
			}

			memcpy(&framedata[dwFrameDataPtr],size,sizeof(size));
			dwFrameDataPtr += sizeof(size);
			memcpy(&framedata[dwFrameDataPtr],&flagsBe,sizeof(flagsBe));
			dwFrameDataPtr += sizeof(flagsBe);
			dwFrameDataPtr += dwSize;
		}
		p++;
	}
	dwTotalFrameSize = dwFrameDataPtr;
//	ASSERT(dwFrameDataPtr == dwTotalFrameSize);
	//非同期化
	if(m_wVer == 0x400 && m_bUnSynchronization)
	{
		//非同期化フラグをセット
		m_head.flag |= HDR_FLAG_UNSYNC;
	}
	else if(m_bUnSynchronization)
	{
		unsigned char *encData = (unsigned char *)malloc(dwTotalFrameSize*2);
		DWORD dwEncodeSize = EncodeUnSynchronization(framedata,dwTotalFrameSize,encData);
//		if(dwEncodeSize != dwTotalFrameSize)
//	2004-09-24 プロパティに非同期化スイッチをつけたため、非同期化フラグは必ず立てることにした
//	(非同期化ONが保存できない様子はユーザからみておかしな動きに見えるため)
		{
			//非同期化フラグをセット
			m_head.flag |= HDR_FLAG_UNSYNC;
			dwTotalFrameSize = dwEncodeSize;
		}
		free(framedata);
		framedata = encData;
	}
	// Drop footer flag.
	m_head.flag &= ~HDR_FLAG_FOOTER;

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

