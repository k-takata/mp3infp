// Id3tagv2.h: CId3tagv2 クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ID3TAGV2_H__92584511_76E1_4EE6_90A7_E196C9C0EF88__INCLUDED_)
#define AFX_ID3TAGV2_H__92584511_76E1_4EE6_90A7_E196C9C0EF88__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CId3Frame  
{
public:
	CId3Frame()
	{
		m_data = NULL;
		m_dwId = 0;
		m_dwSize = 0;
		m_wFlags = 0;
	};
	virtual ~CId3Frame()
	{
		Release();
	};
	void Release()
	{
		if(m_data)
		{
			free(m_data);
		}
		m_data = NULL;
		m_dwId = 0;
		m_dwSize = 0;
		m_wFlags = 0;
	}
	CId3Frame(const CId3Frame &obj)	//コピーコンストラクタ
	{
		m_data = (unsigned char *)malloc(obj.m_dwSize);
		memcpy(m_data,obj.m_data,obj.m_dwSize);
		m_dwId = obj.m_dwId;
		m_dwSize = obj.m_dwSize;
		m_wFlags = obj.m_wFlags;
	};
	DWORD LoadFrame2_4(const unsigned char *pData,DWORD dwSize);
	DWORD LoadFrame2_3(const unsigned char *pData,DWORD dwSize);
	DWORD LoadFrame2_2(const unsigned char *pData,DWORD dwSize);
	DWORD GetId() const {return m_dwId;};
	void SetId(DWORD id){m_dwId = id;};
	DWORD GetSize() const {return m_dwSize;};
	void SetSize(DWORD size){m_dwSize = size;};
	WORD GetFlags() const {return m_wFlags;};
	void SetFlags(WORD flags){m_wFlags = flags;};
	unsigned char *GetData(){return m_data;};
	const unsigned char *GetData() const {return m_data;};
	void SetData(const unsigned char *data,DWORD size)
	{
		if(m_data)
		{
			free(m_data);
		}
		m_data = (unsigned char *)malloc(size);
		if(!m_data)
		{
			return;	//メモリを確保できなかった
		}
		memcpy(m_data,data,size);
		m_dwSize = size;
	};
	DWORD GetDataOffset() const;
	bool IsTextFrame() const {return (m_dwId & 0xff) == 'T';};
	bool IsNumericFrame() const;

	enum Flags {
		MASK_FRAME_STATUS		= 0xff00,	// Mask for frame status flags
		MASK_FRAME_FORMAT		= 0x00ff,	// Mask for frame format flags
		FLAG_TAG_ALT_PRESERVE	= 0x4000,	// Tag alter preservation
		FLAG_FILE_ALT_PRESERVE	= 0x2000,	// File alter preservation
		FLAG_READ_ONLY			= 0x1000,	// Read only
		FLAG_GROUP				= 0x0040,	// Grouping identity
		FLAG_COMPRESS			= 0x0008,	// Compression
		FLAG_ENCRYPT			= 0x0004,	// Encryption
		FLAG_UNSYNC				= 0x0002,	// Unsynchronisation
		FLAG_DATA_LEN			= 0x0001,	// Data length indicator
	};

private:
	DWORD LoadApicFrame(const unsigned char *pData, DWORD dwSize, WORD wVer);

	void operator=(const CId3Frame &){};	// 代入演算子

	unsigned char	*m_data;
	DWORD	m_dwId;
	DWORD	m_dwSize;
	WORD	m_wFlags;
};

#pragma warning(disable:4786)
#include <map>

class CId3tagv2  
{
public:
#pragma pack(1)
	typedef struct _ID3HEAD
	{
		char id3des[3];
		char ver[2];
		unsigned char flag;
		unsigned char size[4];
	}ID3HEAD;
#pragma pack()
	enum HeaderFlags {
		HDR_FLAG_UNSYNC			= 0x80,		// Unsynchronisation
		HDR_FLAG_EXT_HEADER		= 0x40,		// Extended header
		HDR_FLAG_EXPERIMENTAL	= 0x20,		// Experimental indicator
		HDR_FLAG_FOOTER			= 0x10,		// Footer present
	};
//	CId3tagv2(WORD defaultVersion = 0x0300/* ID3v2.3 = 0x0300/ID3v2.4 = 0x0400*/);
	CId3tagv2();
	virtual ~CId3tagv2();
	void SetEncDefault(LPCTSTR szDefaultEnc);
	BOOL IsEnable(){return m_bEnable;};
//	void SetDefaultId3v2Version(DWORD version){m_wDefaultId3TagVersion = (WORD )version;};/* ID3v2.3 = 0x0300/ID3v2.4 = 0x0400*/
	void SetVer(WORD ver){m_wVer = ver;};
	WORD GetVer(){return m_wVer;};

	enum CharEncoding {
		ID3V2CHARENCODING_UNSPECIFIED = -1,
		ID3V2CHARENCODING_ISO_8859_1 = 0,
		ID3V2CHARENCODING_UTF_16 = 1,
		ID3V2CHARENCODING_UTF_16BE = 2,
		ID3V2CHARENCODING_UTF_8 = 3
	};
	void SetCharEncoding(CharEncoding encoding)
	{
		// エンコード指定$2/$3が使えるのはv2.4以降
		if(m_wVer < 0x0400)
		{
			if(	(encoding != ID3V2CHARENCODING_ISO_8859_1) &&
				(encoding != ID3V2CHARENCODING_UTF_16) )
			{
				// UTF-16に自動設定
				encoding = ID3V2CHARENCODING_UTF_16;
			}
		}
		m_encoding = encoding;
	};
	CharEncoding GetCharEncoding(){return m_encoding;};

	void SetUnSynchronization(BOOL bEnable){m_bUnSynchronization = bEnable;};
	BOOL GetUnSynchronization(){return m_bUnSynchronization;};
	
	CString GetTitle();	//TIT2
	void SetTitle(LPCTSTR title);
	CString GetTrackNo();
	void SetTrackNo(LPCTSTR szTrackNo);
	CString GetDiscNo();
	void SetDiscNo(LPCTSTR szDiscNo);
	CString GetArtist();
	void SetArtist(LPCTSTR artist);
	CString GetAlbumArtist();
	void SetAlbumArtist(LPCTSTR albumartist);
	CString GetAlbum();
	void SetAlbum(LPCTSTR album);
	CString GetYear();
	void SetYear(LPCTSTR year);
	CString GetGenre();
	void SetGenre(LPCTSTR szGenre);
	CString GetComment();
	void SetComment(LPCTSTR comment);
	CString GetComposer();
	void SetComposer(LPCTSTR composer);
	CString GetOrigArtist();
	void SetOrigArtist(LPCTSTR origArtist);
	CString GetCopyright();
	void SetCopyright(LPCTSTR copyright);
	CString GetUrl();
	void SetUrl(LPCTSTR url);
	CString GetEncoder();
	void SetEncoder(LPCTSTR encoder);
	CString GetEncodedBy();
	void SetEncodedBy(LPCTSTR encoder);

	DWORD Load(LPCTSTR szFileName);
	DWORD Save(LPCTSTR szFileName);
	DWORD DelTag(LPCTSTR szFileName);
	DWORD MakeTag(LPCTSTR szFileName);
	static BOOL IsTagValid(const ID3HEAD *head) {
		return memcmp(head->id3des,"ID3",3) == 0;
	};

	static inline DWORD ExtractV2Size(const unsigned char size[4]) {
		return (((DWORD )(size[0]&0x7f)<<21) | ((DWORD )(size[1]&0x7f)<<14) | ((DWORD )(size[2]&0x7f)<<7) | (DWORD )(size[3]&0x7f));
	};
	static DWORD DecodeUnSynchronization(unsigned char *data,DWORD dwSize);

private:
	static DWORD EncodeUnSynchronization(const unsigned char *srcData,DWORD dwSize,unsigned char *dstData);
	static void MakeV2Size(DWORD dwSize,unsigned char size[4]);
	CString GetId3String(const char szId[]);
	void SetId3String(const char szId[],LPCTSTR szString,LPCTSTR szDescription = NULL);
	DWORD GetTotalFrameSize();
//	void _SetStringEncoding(int encoding);
	static void v23IDtov22ID(const char *v23ID,char *v22ID);
	static CString ReadEncodedTextString(unsigned char encoding, const unsigned char *data, int dataize, DWORD *pdwReadSize);
	static CharEncoding GetFrameEncoding(const CId3Frame &frame);
	static DWORD ConvertApicToV22(const unsigned char *v23, DWORD dwSize, unsigned char *v22);

	void Release();
	BOOL m_bEnable;					//ID3v2が無い場合はFALSE
	CharEncoding m_encoding;		// 文字エンコードタイプ (0=ISO-8859-1/1=UTF-16/2=UTF-16BE/3=UTF-8)
//	BOOL m_bUnicodeEncoding;		//文字コードエンコードにUnicodeを使用する
	BOOL m_bUnSynchronization;		//非同期化する
	ID3HEAD m_head;
	WORD m_wVer;
	typedef std::multimap<DWORD,CId3Frame> FrameMap;
	FrameMap m_frames;
	CString m_strDefaultEnc;		//TENCのデフォルト値
//	WORD m_wDefaultId3TagVersion;	// ID3V2の初期値(新規作成時にこのバージョンとなる)
};

#endif // !defined(AFX_ID3TAGV2_H__92584511_76E1_4EE6_90A7_E196C9C0EF88__INCLUDED_)
