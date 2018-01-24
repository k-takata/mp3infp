// Id3tagv2.cpp: CId3tagv2 �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "mp3infp_res/resource.h"		// ���C�� �V���{��
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

// 2004-09-24 UTF-16�̓��g���G���f�B�A���ŏ�������
//#define UTF16_BIGENDIAN

//////////////////////////////////////////////////////////////////////
// �\�z/����
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
	case 'T':	//�e�L�X�g���t���[��
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
	case 'W':	//URL�����N�t���[��x
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
		
		//��������ǂݔ�΂�
		ReadEncodedTextString(data[0], &data[1], p->second.GetSize()-1, &dwReadSize);
		//URL�{��
		//�K�i��͏�� ISO-8859-1�i�݊����̂��߁ABOM������΂������D��j
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

			// ���ꕶ����ǂݔ�΂��A�������擾
			CString desc = ReadEncodedTextString(data[0], &data[4], p->second.GetSize()-4, &dwReadSize);
			if(desc.Left(4) == _T("iTun"))
			{
				// iTunes�ŗL�̃R�����g�̓X�L�b�v (iTunNORM, iTunSMPB, etc.)
				continue;
			}
			//�{��
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

	//Load�����t�@�C���Ƀt���[�����Ȃ������ꍇ
	if((lstrlen(szString) == 0) && (strcmp(szId,"COMM") != 0))
	{
		m_frames.erase(dwId);	//����(�����)
		return;
	}
	
	unsigned char *data;
	int size = 0;
	int encode, i;
	switch(szId[0]){
	case 'T':	//�e�L�X�g���t���[��
		encode = m_encode;
		//�����������ISO-8859-1
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
			//Load�����t�@�C���Ƀt���[�����Ȃ������ꍇ
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
	case 'W':	//URL�����N�t���[��x
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
			data[1] = 0;	//������(�ȗ�)
			TstrToData(szString, -1, (char *)&data[2], size-2, DTC_CODE_ANSI);	//URL�{�́i���ISO-8859-1�j
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
			data[3] = 0;	//������(�ȗ�)
			data[4] = 0;
			TstrToData(szString, -1, (char *)&data[5], size-5, DTC_CODE_ANSI);	//URL�{�́i���ISO-8859-1�j
			break;
#else	// �r�b�N�G���f�B�A��
			size = TstrToData(szString, -1, NULL, 0, DTC_CODE_UTF16BE) + 5;
			data = (unsigned char *)malloc(size);
			if(!data)
			{
				return;
			}
			data[0] = 1;	//encoding
			data[1] = 0xfe;	//BOM
			data[2] = 0xff;
			data[3] = 0;	//������(�ȗ�)
			data[4] = 0;
			TstrToData(szString, -1, (char *)&data[5], size-5, DTC_CODE_ANSI);	//URL�{�́i���ISO-8859-1�j
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
			data[1] = 0;	//������(�ȗ�)
			data[2] = 0;
			TstrToData(szString, -1, (char *)&data[3], size-3, DTC_CODE_ANSI);	//URL�{�́i���ISO-8859-1�j
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
				data[1] = 0;	//������(�ȗ�)
				TstrToData(szString, -1, (char *)&data[2], size-2, DTC_CODE_ANSI);	//URL�{�́i���ISO-8859-1�j
				break;
			}
		}
		pp= m_frames.equal_range(dwId);
		if((pp.first == pp.second) || !(pp.first->second.GetSize()))
		{
			//Load�����t�@�C���Ƀt���[�����Ȃ������ꍇ
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
		// �R�����g�t���[��������
		for (pp = m_frames.equal_range(dwId); pp.first != pp.second; ++pp.first)
		{
			p = pp.first;
			if(p->second.GetSize() == 0)
			{
				continue;
			}
			unsigned char *data = p->second.GetData();

			// ���ꕶ����ǂݔ�΂��A�������擾
			DWORD dwReadSize;
			CString desc = ReadEncodedTextString(data[0], &data[4], p->second.GetSize()-4, &dwReadSize);
			if(desc.Left(4) == _T("iTun"))
			{
				// iTunes�ŗL�̃R�����g�̓X�L�b�v (iTunNORM, iTunSMPB, etc.)
				continue;
			}

			// ���������Ώۂ̃R�����g�t���[���𔭌�

			if(lstrlen(szString) == 0)
			{
				m_frames.erase(p);	//����
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
			data[4] = 0;	//������(�ȗ�)
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
			data[6] = 0;	//������(�ȗ�)
			data[7] = 0;
			data[8] = 0xff;	//BOM
			data[9] = 0xfe;
			TstrToData(szString, -1, (char *)&data[10], size-10, DTC_CODE_UTF16LE);
			break;
#else	// �r�b�O�G���f�B�A��
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
			data[6] = 0;	//������(�ȗ�)
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
			data[4] = 0;	//������(�ȗ�)
			data[5] = 0;	//������(�ȗ�)
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
				data[4] = 0;	//������(�ȗ�)
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

// �S�Ẵt���[���̕�����G���R�[�h���w�肷��
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
	//v2.3����v2.2�փt���[��ID��ϊ�
	int i;
	for (i = 0; id3_v23v22table[i].v23 != NULL; i++) {
		if (memcmp(v23ID, id3_v23v22table[i].v23, 4) == 0) {
			memcpy(v22ID, id3_v23v22table[i].v22, 3);
			break;
		}
	}
	if (id3_v23v22table[i].v23 == NULL) {
		memcpy(v22ID, "XXX", 3);	// �s��
	}
}

#if 0
// UNICODE�������BE/LE��ϊ�����(len=������)
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
	//�^�C�g��
	return GetId3String("TIT2");
}

void CId3tagv2::SetTitle(LPCTSTR title)
{
	//�^�C�g��
	SetId3String("TIT2",title);
}

CString CId3tagv2::GetTrackNo()
{
	//�g���b�N�ԍ�
	return GetId3String("TRCK");
}

void CId3tagv2::SetTrackNo(LPCTSTR szTrackNo)
{
	//�g���b�N�ԍ�
	SetId3String("TRCK",szTrackNo);
}

CString CId3tagv2::GetArtist()
{
	//�A�[�e�B�X�g
	return GetId3String("TPE1");
}

void CId3tagv2::SetArtist(LPCTSTR artist)
{
	//�A�[�e�B�X�g
	SetId3String("TPE1",artist);
}

CString CId3tagv2::GetAlbumArtist()
{
	//�A���o���A�[�e�B�X�g
	return GetId3String("TPE2");
}

void CId3tagv2::SetAlbumArtist(LPCTSTR albumartist)
{
	//�A���o���A�[�e�B�X�g
	SetId3String("TPE2",albumartist);
}

CString CId3tagv2::GetAlbum()
{
	//�A���o��
	return GetId3String("TALB");
}

void CId3tagv2::SetAlbum(LPCTSTR album)
{
	//�A���o��
	SetId3String("TALB",album);
}

CString CId3tagv2::GetYear()
{
	//����
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
	//����
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
	//�W������
	CString strGenre = GetId3String("TCON");
	int genre = -1;
//	unsigned char *data = (unsigned char *)(LPCSTR )strGenre;
	//�ŏ���()��ǂݔ�΂�����	Fix 2001-05-20
	while(1)
	{
		if(strGenre.GetLength() &&
			!IS_LEAD_TBYTE(strGenre[0]) &&
			(strGenre[0] == '(') )	//���ɃJ�b�R�����o
		{
			if((strGenre.GetLength() > 1) &&
				!IS_LEAD_TBYTE(strGenre[1]) &&
				(strGenre[1] == '(') )
			{
				strGenre.Delete(0);	//�擪��'('���폜
				break;
			}
			int index = strGenre.Find(')');	
			if(0 <= index)
			{
				if (genre == -1) {
					CString strGenreNum = strGenre.Mid(1, index);
					genre = _ttoi(strGenreNum);
				}
				strGenre.Delete(0,index+1);	//)'�ȑO���폜
				continue;	//Fix 2001-10-24
			}
		}
		break;
	}
	//�W���������ԍ��݂̂Ŏw�肳��Ă���ꍇ�́A������ɕϊ�
	if ((genre >= 0) && (strGenre == _T(""))) {
		CId3tagv1 id3tagv1(FALSE);
		strGenre = id3tagv1.GenreNum2String(genre);
	}
	return strGenre;
}

void CId3tagv2::SetGenre(LPCTSTR szGenre)
{
	//�W������
	CString strGenre;
/*	CId3tagv1 id3tagv1(m_bScmpxGenre);
	long genre = id3tagv1.GenreString2Num(szGenre);
	if((genre != -1) &&
		!((m_bScmpxGenre && (SCMPX_GENRE_NULL == genre)) || (!m_bScmpxGenre && (WINAMP_GENRE_NULL == genre)))
		)
	{
		strGenre.Format(_T("(%d)"),genre);
	}
	2003-01-25 iTunes���W�������R�[�h�ւ̎Q�Ƃ𐳂����ǂ߂Ȃ����߁A�W�������R�[�h�����Ȃ��悤�ɏC��
*/
	// 2004-05-16 "("�Ŏn�܂�ꍇ�͐擪��"("��ǉ�
	if(lstrlen(szGenre) &&
		!IS_LEAD_TBYTE(szGenre[0]) &&
		(szGenre[0] == '(') )	//���ɃJ�b�R�����o
	{
		strGenre += _T("(");
	}
	strGenre += szGenre;

	SetId3String("TCON",strGenre);
}

CString CId3tagv2::GetComment()
{
	//�R�����g
	return GetId3String("COMM");
}

void CId3tagv2::SetComment(LPCTSTR comment)
{
	//�R�����g
	SetId3String("COMM",comment);
}

CString CId3tagv2::GetComposer()
{
	//���
	return GetId3String("TCOM");
}

void CId3tagv2::SetComposer(LPCTSTR composer)
{
	//���
	SetId3String("TCOM",composer);
}

CString CId3tagv2::GetOrigArtist()
{
	//Orig.�A�[�e�B�X�g
	return GetId3String("TOPE");
}

void CId3tagv2::SetOrigArtist(LPCTSTR origArtist)
{
	//Orig.�A�[�e�B�X�g
	SetId3String("TOPE",origArtist);
}

CString CId3tagv2::GetCopyright()
{
	//���쌠
	return GetId3String("TCOP");
}

void CId3tagv2::SetCopyright(LPCTSTR copyright)
{
	//���쌠
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
	//�G���R�[�_�[
	return GetId3String("TSSE");
}

void CId3tagv2::SetEncoder(LPCTSTR encoder)
{
	//�G���R�[�_�[
	SetId3String("TSSE",encoder);
}

CString CId3tagv2::GetEncodest()
{
	//�G���R�[�h�����l�܂��͑g�D
	return GetId3String("TENC");
}

void CId3tagv2::SetEncodest(LPCTSTR encoder)
{
	//�G���R�[�h�����l�܂��͑g�D
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
				OPEN_EXISTING,	//�t�@�C�����I�[�v�����܂��B�w��t�@�C�������݂��Ă��Ȃ��ꍇ�A�֐��͎��s���܂��B
				FILE_ATTRIBUTE_NORMAL,
				NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}

	//�t�@�C�����I�[�v��
	ID3HEAD head;
	memcpy(&head,&m_head,sizeof(m_head));
	//ID3V2�w�b�_��ǂݍ���
	DWORD dwRet;
	if(!ReadFile(hFile,&head,sizeof(head),&dwRet,NULL) || (dwRet != sizeof(head)))
	{
		CloseHandle(hFile);
		return -1;
	}

	//ID3v2���m�F
	if(memcmp(head.id3des,"ID3",3) != 0)
	{
		CloseHandle(hFile);
		return -1;
	}
	
	//�o�[�W����
	WORD ver = (head.ver[0]<<8) | head.ver[1];
	if((ver != 0x0200) &&
		(ver != 0x0300) &&
		(ver != 0x0301) &&
		(ver != 0x0400) )
	{
		// ID3v2.4���傫���o�[�W�����̓T�|�[�g���Ȃ�
		CloseHandle(hFile);
		return -1;
	}
	m_wVer = ver;
	if(m_wVer == 0x0301)
	{
		m_wVer = 0x0400;
	}

	//Id3tag�T�C�Y
	DWORD dwId3Size = ExtractV2Size(head.size);

	//�S�t���[���̓Ǎ�
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

	//�񓯊����̉���
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
	//�g���w�b�_��ǂݔ�΂�(ID3v2.2�ɂ͑��݂��Ȃ�)
	if((ver != 0x0200) && (head.flag & 0x40))
	{
		dwRemainSize -= ExtractV2Size(buf) + 4;
	}
	head.flag &= ~0x40;	//����

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

//	if(m_wVer < 0x0300)	//v2.2��v2.3�֕ϊ����ĕۑ�����
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
	// �G���R�[�h�w��$2/$3���g����̂�v2.4�ȍ~
	if(m_wVer < 0x0400)
	{
		if(	(m_encode != ID3V2CHARENCODE_ISO_8859_1) &&
			(m_encode != ID3V2CHARENCODE_UTF_16) )
		{
			// ISO-8859-1�Ɏ����ݒ�
			m_encode = ID3V2CHARENCODE_ISO_8859_1;
		}
	}

	// �S�t���[���̕�����G���R�[�h��ݒ�
	_SetStringEncode(m_encode);
*/

	DWORD dwTotalFrameSize = GetTotalFrameSize();
	//�t���[�����������o������
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
		//�T�C�Y��0�̂Ƃ��̓t���[�����������܂Ȃ�
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
			//v2.3����v2.2�փt���[��ID��ϊ�
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
			// ��������
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
	//�񓯊���
	if(m_bUnSynchronization)
	{
		unsigned char *encData = (unsigned char *)malloc(dwTotalFrameSize*2);
		DWORD dwEncodeSize = EncodeUnSynchronization(framedata,dwTotalFrameSize,encData);
//		if(dwEncodeSize != dwTotalFrameSize)
//	2004-03-20 �v���p�e�B�ɔ񓯊����X�C�b�`���������߁A�񓯊����t���O�͕K�����Ă邱�Ƃɂ���
//	(�񓯊���ON���ۑ��ł��Ȃ��l�q�̓��[�U����݂Ă������ȓ����Ɍ����邽��)
		{
			//�񓯊����t���O���Z�b�g
			m_head.flag |= 0x80;
			dwTotalFrameSize = dwEncodeSize;
		}
		free(framedata);
		framedata = encData;
	}
	else
	{
		//�񓯊����t���O������
		m_head.flag &= ~0x80;
	}

	//Id3tag�T�C�Y
	DWORD dwId3Size = ExtractV2Size(m_head.size);

	if(dwTotalFrameSize > dwId3Size)
	{
		//[�p�f�B���O�̈��}��]
		DWORD dwPaddingSize = dwTotalFrameSize - dwId3Size + ID3V2_PADDING_SIZE;
		dwId3Size += dwPaddingSize;

		//==================���t�@�C�����������ɕۑ�==================
		hFile = CreateFile(
					szFileName,
					GENERIC_READ,
					FILE_SHARE_READ,
					NULL,
					OPEN_EXISTING,	//�w��t�@�C�������݂��Ă��Ȃ��ꍇ�A�֐��͎��s���܂��B
					FILE_ATTRIBUTE_NORMAL,
					NULL);
		if(hFile == INVALID_HANDLE_VALUE)
		{
			dwWin32errorCode = GetLastError();
			free(framedata);
			return dwWin32errorCode;
		}

		DWORD dwDataSize = GetFileSize(hFile,NULL);
		//�o�b�t�@�������̊m��
		char *pRawData = (char *)malloc(dwDataSize);
		if(!pRawData)
		{
			dwWin32errorCode = GetLastError();
			CloseHandle(hFile);
			free(framedata);
			return dwWin32errorCode;
		}
		//raw data�̓ǂݏo��
		if(!ReadFile(hFile,pRawData,dwDataSize,&dwWritten,NULL))
		{
			dwWin32errorCode = GetLastError();
			CloseHandle(hFile);
			free(pRawData);
			free(framedata);
			return dwWin32errorCode;
		}
		CloseHandle(hFile);

		//==================�e���|�������쐬==================
		//�e���|���������擾
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
		//�t�@�C���I�[�v��(�w��t�@�C�������łɑ��݂��Ă���ꍇ�A���̃t�@�C���͏㏑������܂��B)
		hFile = CreateFile(szTempFile,GENERIC_WRITE|GENERIC_READ,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		if(hFile == INVALID_HANDLE_VALUE)
		{
			dwWin32errorCode = GetLastError();
			free(pRawData);
			DeleteFile(szTempFile);
			free(framedata);
			return dwWin32errorCode;
		}
		//���܂��Ȃ�
		if(SetFilePointer(hFile,0,NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		{
			dwWin32errorCode = GetLastError();
			CloseHandle(hFile);
			free(pRawData);
			DeleteFile(szTempFile);
			free(framedata);
			return dwWin32errorCode;
		}
		//�p�f�B���O�̈���X�L�b�v
		if(SetFilePointer(hFile,dwPaddingSize,NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		{
			dwWin32errorCode = GetLastError();
			CloseHandle(hFile);
			free(pRawData);
			DeleteFile(szTempFile);
			free(framedata);
			return dwWin32errorCode;
		}
		//�ړ���ɃR�s�[
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
		//�t�@�C�����I�[�v��
		hFile = CreateFile(
					szFileName,
					GENERIC_READ|GENERIC_WRITE,
					FILE_SHARE_READ,
					NULL,
					OPEN_EXISTING,	//�t�@�C�����I�[�v�����܂��B�w��t�@�C�������݂��Ă��Ȃ��ꍇ�A�֐��͎��s���܂��B
					FILE_ATTRIBUTE_NORMAL,
					NULL);
		if(hFile == INVALID_HANDLE_VALUE)
		{
			dwWin32errorCode = GetLastError();
			free(framedata);
			return dwWin32errorCode;
		}
	}

	//ID3v2�w�b�_����������
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
	// ��������
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

	//�t���[�������t�@�C���ɏ����o��
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

	//�p�f�B���O�̈��0�Ńp�f�B���O
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
		//�I���W�i���t�@�C����ޔ�(���l�[��)
		TCHAR szPreFile[MAX_PATH];
		if(!GetTempFileName(szTempPath,_T("tms"),0,szPreFile))
		{
			dwWin32errorCode = GetLastError();
			DeleteFile(szTempFile);
			return dwWin32errorCode;
		}
		DeleteFile(szPreFile);//�蔲��(^^;
		if(!MoveFile(szFileName,szPreFile))
		{
			dwWin32errorCode = GetLastError();
			DeleteFile(szTempFile);
			return dwWin32errorCode;
		}

		//�����i�����l�[��
		if(!MoveFile(szTempFile,szFileName))
		{
			dwWin32errorCode = GetLastError();
			MoveFile(szPreFile,szFileName);
			DeleteFile(szTempFile);
			return dwWin32errorCode;
		}

		//�I���W�i�����폜
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

	//==================���t�@�C�����������ɕۑ�==================
	HANDLE hFile = CreateFile(
							szFileName,
							GENERIC_READ,
							FILE_SHARE_READ,
							NULL,
							OPEN_EXISTING,	//�w��t�@�C�������݂��Ă��Ȃ��ꍇ�A�֐��͎��s���܂��B
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

	//�o�b�t�@�������̊m��
	char *pRawData = (char *)malloc(dwDataSize);
	if(!pRawData)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		return dwWin32errorCode;
	}
	//raw data�̓ǂݏo��
	if(!ReadFile(hFile,pRawData,dwDataSize,&dwWritten,NULL))
	{
		dwWin32errorCode = GetLastError();
		free(pRawData);
		CloseHandle(hFile);
		return dwWin32errorCode;
	}
	CloseHandle(hFile);

	//==================�e���|�������쐬==================
	//�e���|���������擾
	lstrcpy(szTempPath,szFileName);
	cutFileName(szTempPath);
	if(!GetTempFileName(szTempPath,_T("tms"),0,szTempFile))
	{
		dwWin32errorCode = GetLastError();
		free(pRawData);
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}
	//�t�@�C���I�[�v��(�w��t�@�C�������łɑ��݂��Ă���ꍇ�A���̃t�@�C���͏㏑������܂��B)
	hFile = CreateFile(szTempFile,GENERIC_WRITE|GENERIC_READ,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		dwWin32errorCode = GetLastError();
		free(pRawData);
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}
	//���܂��Ȃ�
	if(SetFilePointer(hFile,0,NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		free(pRawData);
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}
	//�ړ���ɃR�s�[
	if(WriteFile(hFile,pRawData,dwDataSize,&dwWritten,NULL) == 0)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		free(pRawData);
		return dwWin32errorCode;
	}
	free(pRawData);
	CloseHandle(hFile);

	//�I���W�i���t�@�C����ޔ�(���l�[��)
	TCHAR szPreFile[MAX_PATH];
	if(!GetTempFileName(szTempPath,_T("tms"),0,szPreFile))
	{
		dwWin32errorCode = GetLastError();
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}
	DeleteFile(szPreFile);//�蔲��(^^;
	if(!MoveFile(szFileName,szPreFile))
	{
		dwWin32errorCode = GetLastError();
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}

	//�����i�����l�[��
	if(!MoveFile(szTempFile,szFileName))
	{
		dwWin32errorCode = GetLastError();
		MoveFile(szPreFile,szFileName);
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}

	//�I���W�i�����폜
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

	//�f�t�H���g���
	TCHAR szDefaultName[MAX_PATH];
	CString strFileName = szFileName;
	lstrcpy(szDefaultName,getFileName(strFileName));
	SetId3String("TIT2",szDefaultName);
	SetId3String("TSSE",m_strDefaultEnc);
	
	DWORD dwTotalFrameSize = GetTotalFrameSize();
	//[�p�f�B���O�̈��}��]
	DWORD dwPaddingSize = ID3V2_PADDING_SIZE - dwTotalFrameSize;

	//�t���[�����������o������
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
			//v2.3����v2.2�փt���[��ID��ϊ�
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
	//�񓯊���
	if(m_bUnSynchronization)
	{
		unsigned char *encData = (unsigned char *)malloc(dwTotalFrameSize*2);
		DWORD dwEncodeSize = EncodeUnSynchronization(framedata,dwTotalFrameSize,encData);
//		if(dwEncodeSize != dwTotalFrameSize)
//	2004-09-24 �v���p�e�B�ɔ񓯊����X�C�b�`���������߁A�񓯊����t���O�͕K�����Ă邱�Ƃɂ���
//	(�񓯊���ON���ۑ��ł��Ȃ��l�q�̓��[�U����݂Ă������ȓ����Ɍ����邽��)
		{
			//�񓯊����t���O���Z�b�g
			m_head.flag |= 0x80;
			dwTotalFrameSize = dwEncodeSize;
		}
		free(framedata);
		framedata = encData;
	}

	//==================���t�@�C�����������ɕۑ�==================
	HANDLE hFile = CreateFile(
				szFileName,
				GENERIC_READ,
				FILE_SHARE_READ,
				NULL,
				OPEN_EXISTING,	//�w��t�@�C�������݂��Ă��Ȃ��ꍇ�A�֐��͎��s���܂��B
				FILE_ATTRIBUTE_NORMAL,
				NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		dwWin32errorCode = GetLastError();
		free(framedata);
		return dwWin32errorCode;
	}

	DWORD dwDataSize = GetFileSize(hFile,NULL);
	//�o�b�t�@�������̊m��
	char *pRawData = (char *)malloc(dwDataSize);
	if(!pRawData)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		free(framedata);
		return dwWin32errorCode;
	}
	//raw data�̓ǂݏo��
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

	//==================�e���|�������쐬==================
	//�e���|���������擾
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
	//�t�@�C���I�[�v��(�w��t�@�C�������łɑ��݂��Ă���ꍇ�A���̃t�@�C���͏㏑������܂��B)
	hFile = CreateFile(szTempFile,GENERIC_WRITE|GENERIC_READ,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		dwWin32errorCode = GetLastError();
		free(pRawData);
		DeleteFile(szTempFile);
		free(framedata);
		return dwWin32errorCode;
	}
	//���܂��Ȃ�
	if(SetFilePointer(hFile,0,NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		free(pRawData);
		DeleteFile(szTempFile);
		free(framedata);
		return dwWin32errorCode;
	}
	//�p�f�B���O�̈���X�L�b�v
	if(SetFilePointer(hFile,dwPaddingSize + dwTotalFrameSize + 10,NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		free(pRawData);
		DeleteFile(szTempFile);
		free(framedata);
		return dwWin32errorCode;
	}
	//�ړ���ɃR�s�[
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

	//�t���[�������t�@�C���ɏ����o��
	if(!WriteFile(hFile,framedata,dwTotalFrameSize,&dwWritten,NULL))
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		DeleteFile(szTempFile);
		free(framedata);
		return dwWin32errorCode;
	}
	free(framedata);

	//�p�f�B���O�̈��0�Ńp�f�B���O
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

	//�I���W�i���t�@�C����ޔ�(���l�[��)
	TCHAR szPreFile[MAX_PATH];
	if(!GetTempFileName(szTempPath,_T("tms"),0,szPreFile))
	{
		dwWin32errorCode = GetLastError();
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}
	DeleteFile(szPreFile);//�蔲��(^^;
	if(!MoveFile(szFileName,szPreFile))
	{
		dwWin32errorCode = GetLastError();
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}

	//�����i�����l�[��
	if(!MoveFile(szTempFile,szFileName))
	{
		dwWin32errorCode = GetLastError();
		MoveFile(szPreFile,szFileName);
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}

	//�I���W�i�����폜
	DeleteFile(szPreFile);

	m_bEnable = TRUE;

	return dwWin32errorCode;
}

