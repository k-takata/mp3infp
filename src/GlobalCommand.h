#ifndef _GLOBALCOMMAND_H
#define _GLOBALCOMMAND_H

#define sizeof_array(a) (sizeof(a)/sizeof(a[0]))

#ifndef IS_HIGH_SURROGATE
#define IS_HIGH_SURROGATE(wch)		(((wch) & 0xfc00) == 0xd800)
#define IS_LOW_SURROGATE(wch)		(((wch) & 0xfc00) == 0xdc00)
#define IS_SURROGATE_PAIR(hs, ls)	(IS_HIGH_SURROGATE(hs) && IS_LOW_SURROGATE(ls))
#endif

#ifdef UNICODE
#define IS_LEAD_TBYTE(tb)	IS_HIGH_SURROGATE(tb)
#else
#define IS_LEAD_TBYTE(tb)	IsDBCSLeadByte(tb)
#endif

#define Static_SetImage_Icon(hwndCtl, hIcon)	\
	((HICON)(UINT_PTR)SNDMSG((hwndCtl), STM_SETIMAGE, IMAGE_ICON, (LPARAM)(HICON)(hIcon)))

void mbsncpy2(unsigned char *dst,const unsigned char *src,int c);
//２バイト文字列の切りのいい文字数を取得
long check2ByteLength(const char *szTag,long lLimit);

//DLLのバージョン情報を取得
BOOL GetDLLVersion(IN LPCTSTR szDLLFileName,
				   IN DWORD *pdwMajor,
				   IN DWORD *pdwMinor,
				   IN DWORD *pdwBuildNumber1,
				   IN DWORD *pdwBuildNumber2);

  //文末がYenのときTRUE
BOOL IsTailYenSign(LPCTSTR szStr);

//文末に'\\'を追加(既に'\\'のときは何もしない)
void AddTailYenSign(LPTSTR szStr);
void AddTailYenSign(CString &Str);

//LoadStringのバグ対策(MSKB Q140452)
CString LoadResString(HINSTANCE hInstance,UINT uID);

//---------------------------------------------------------------------
//★ビッグエンディアン->リトルエンディアンの変換
static inline DWORD ExtractI4(const unsigned char buf[4])
{
	return ((DWORD)buf[0] << 24) | ((DWORD)buf[1] << 16) | ((DWORD)buf[2] << 8) | (DWORD)buf[3];
}

static inline WORD ExtractI2(const unsigned char buf[2])
{
	return ((WORD)buf[0] << 8) | (WORD)buf[1];
}

CString getFileNameExtName(const CString &path);
CString getExtName(const CString &path);
CString getPathName(const CString &path);
CString getFileName(const CString &path);

//////////////////////////////////////////////////////////////////////////////
//getFileNameExtName
//フルパス名からファイル名を取得する
//////////////////////////////////////////////////////////////////////////////
/*
引数：	char *szPath	ファイル名を含むパス名へのポインタ

戻り値：char * ファイル名へのポインタ
　　　　　　　ファイル名を含まないときは""へのポインタ
	[\],[/],[:]が見つからなかった場合、引数をファイル名とみなしてそのまま返す
*/
LPCTSTR getFileNameExtName(LPCTSTR szPath);

//////////////////////////////////////////////////////////////////////////////
//getExtName
//フルパス名から拡張子を取得する
//////////////////////////////////////////////////////////////////////////////
/*
引数：	char *szPath	拡張子を含むパス名へのポインタ

戻り値：char * 拡張子へのポインタ
　　　　　　　拡張子を含まないときは""へのポインタ
*/
LPCTSTR getExtName(LPCTSTR szPath);

//システムエラーメッセージボックスを表示
void sysError(HWND hWnd,LPCTSTR mes = _T("Error"));
void errMessageBox(HWND hWnd,DWORD dwErrorCode,LPCTSTR mes = _T("Error"));

//////////////////////////////////////////////////////////////////////////////
//cutFileName
//パス名からファイル名を取り除く
//////////////////////////////////////////////////////////////////////////////
/*
引数：	char *szPath	ファイル名を含むパス名へのポインタ
			（実行後、パス名からファイル名が切り離される）
戻り値：なし
*/
void cutFileName(LPTSTR szPath);

//////////////////////////////////////////////////////////////////////////////
//cutExtName
//ファイル名から拡張子を取り除く
//////////////////////////////////////////////////////////////////////////////
/*
引数：	char *szFileName	ファイル名へのポインタ
			（実行後、ファイル名から拡張子が切り離される）
戻り値：なし
*/
void cutExtName(LPTSTR szFileName);

//文字列srcを指定文字cでn文字ごとに区切る
CString divString(LPCTSTR src,char c,int n);


// Functions for encoding conversion
enum BTC_CODE {
	BTC_CODE_ANSI = 0,
	BTC_CODE_UTF16LE = 1,
	BTC_CODE_UTF16BE = 2,
	BTC_CODE_UTF8 = 3
};

// バイト列をCStringに変換する
CString BytesToCString(const char *data, int size, BTC_CODE code);

// TSTRを指定コードのバイト列に変換する
int TstrToBytes(LPCTSTR tstr, int tlen, char *data, int dsize, BTC_CODE code);

// TSTRを指定コードのバイト列に変換する（メモリ自動割り当て）
// 使用後はfree()で解放すること
// offset を指定すると、そのバイト数だけ余計にメモリを確保し、
// 先頭を空けてデータを書き込む
char *TstrToBytesAlloc(LPCTSTR tstr, int tlen, int *dsize, BTC_CODE code, int offset = 0);

// Convert UTF-16LE <-> UTF-16BE.
static inline void ConvertUTF16Endian(WCHAR *str, int len)
{
	for (int i = 0; i < len; i++)
	{
		str[i] = (str[i] << 8) | (str[i] >> 8);
	}
}

__int64 GetFileSize64(HANDLE hFile);
__int64 SetFilePointer64(HANDLE hFile,__int64 llDistanceToMove,DWORD dwMoveMethod);

static inline CString GetSizeString1(__int64 filesize)
{
	CString strSize;
	strSize.Format(_T("%I64u"),filesize);
	return divString(strSize,',',3);
}

static inline CString GetSizeStringK(__int64 filesize)
{
	CString strSize;
	__int64 fsize = (filesize + 1023) / 1024;
	strSize.Format(_T("%I64u"),fsize);
	return divString(strSize,',',3);
}

static inline CString GetSizeStringM(__int64 filesize)
{
	CString strSize;
	__int64 fsize = (filesize + 1048575) / 1048576;
	strSize.Format(_T("%I64u"),fsize);
	return divString(strSize,',',3);
}

// タイムスタンプ保存・復元
class CTimeStampSaver {
public:
	CTimeStampSaver() : m_bTimeStampPushed(FALSE) {};
	~CTimeStampSaver() {};
	BOOL Push(LPCTSTR szFile);
	BOOL Pop(LPCTSTR szFile = NULL);
	void Clear() {m_bTimeStampPushed = FALSE;};
protected:
	FILETIME	m_fileTime;	//タイムスタンプ保存用
	FILETIME	m_createTime;	//タイムスタンプ保存用
	BOOL		m_bTimeStampPushed;	//タイムスタンプが保存されている
	CString		m_strFile;
};

#endif //_GLOBALCOMMAND_H
