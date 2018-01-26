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

void mbsncpy2(unsigned char *dst,unsigned char *src,int c);
//DLLのバージョン情報を取得
BOOL GetDLLVersion(IN LPTSTR szDLLFileName,
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
DWORD ExtractI4(unsigned char buf[4]);
WORD ExtractI2(unsigned char buf[2]);

CString getFileNameExtName(CString &path);
CString getExtName(CString &path);
CString getPathName(CString &path);
CString getFileName(CString &path);

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

//２バイト文字列の切りのいい文字数を取得
long check2ByteLength(const char *szTag,long lLimit);

//システムエラーメッセージボックスを表示
void sysError(HWND hWnd,LPTSTR mes = _T("Error"));
void errMessageBox(HWND hWnd,DWORD dwErrorCode,LPTSTR mes = _T("Error"));

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
CString divString(LPTSTR src,char c,int n);

// バイト列をCStringに変換する
CString DataToCString(const char *data, int size, int code);
#define DTC_CODE_ANSI		0
#define DTC_CODE_UTF16LE	1
#define DTC_CODE_UTF16BE	2
#define DTC_CODE_UTF8		3

// TSTRを指定コードのバイト列に変換する
int TstrToData(LPCTSTR tstr, int tlen, char *data, int dsize, int code);

// TSTRを指定コードのバイト列に変換する（メモリ自動割り当て）
// 使用後はfree()で解放すること
char *TstrToDataAlloc(LPCTSTR tstr, int tlen, int *dsize, int code);


__int64 GetFileSize64(HANDLE hFile);
__int64 SetFilePointer64(HANDLE hFile,__int64 llDistanceToMove,DWORD dwMoveMethod);

#endif //_GLOBALCOMMAND_H
