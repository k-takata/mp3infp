#ifndef _GLOBALCOMMAND_H
#define _GLOBALCOMMAND_H

#define sizeof_array(a) (sizeof(a)/sizeof(a[0]))

void mbsncpy2(unsigned char *dst,unsigned char *src,int c);
//DLLのバージョン情報を取得
BOOL GetDLLVersion(IN LPSTR szDLLFileName,
				   IN DWORD *pdwMajor,
				   IN DWORD *pdwMinor,
				   IN DWORD *pdwBuildNumber1,
				   IN DWORD *pdwBuildNumber2);

  //文末がYenのときTRUE
BOOL IsTailYenSign(char *szStr);

//文末に'\\'を追加(既に'\\'のときは何もしない)
void AddTAilYenSigne(char *szStr);
void AddTAilYenSigne(CString &Str);

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
//getFileName
//フルパス名からファイル名を取得する
//////////////////////////////////////////////////////////////////////////////
/*
引数：	char *szPath	ファイル名を含むパス名へのポインタ

戻り値：char * ファイル名へのポインタ
　　　　　　　ファイル名を含まないときは""へのポインタ
	[\],[/],[:]が見つからなかった場合、引数をファイル名とみなしてそのまま返す
*/
const char *getFileName(const char *szPath);

//////////////////////////////////////////////////////////////////////////////
//getExtName
//フルパス名から拡張子を取得する
//////////////////////////////////////////////////////////////////////////////
/*
引数：	char *szPath	拡張子を含むパス名へのポインタ

戻り値：char * 拡張子へのポインタ
　　　　　　　拡張子を含まないときは""へのポインタ
*/
const char *getExtName(const char *szPath);

//２バイト文字列の切りのいい文字数を取得
long check2ByteLength(const char *szTag,long lLimit);

//システムエラーメッセージボックスを表示
void sysError(HWND hWnd,char *mes = "Error");
void errMessageBox(HWND hWnd,DWORD dwErrorCode,char *mes = "Error");

//////////////////////////////////////////////////////////////////////////////
//cutFileName
//パス名からファイル名を取り除く
//////////////////////////////////////////////////////////////////////////////
/*
引数：	char *szPath	ファイル名を含むパス名へのポインタ
			（実行後、パス名からファイル名が切り離される）
戻り値：なし
*/
void cutFileName(char *szPath);

//////////////////////////////////////////////////////////////////////////////
//cutExtName
//ファイル名から拡張子を取り除く
//////////////////////////////////////////////////////////////////////////////
/*
引数：	char *szFileName	ファイル名へのポインタ
			（実行後、ファイル名から拡張子が切り離される）
戻り値：なし
*/
void cutExtName(char *szFileName);

//文字列srcを指定文字cでn文字ごとに区切る
CString divString(char *src,char c,int n);

#endif //_GLOBALCOMMAND_H