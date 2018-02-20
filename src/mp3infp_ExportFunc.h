#ifndef _MP3INFP_EXPORT_FUNC_H
#define _MP3INFP_EXPORT_FUNC_H

/*
■改変履歴
2018-02-20 Ver.2.55
・mp3infp_GetValue()/mp3infp_SetValue()関連の修正
>MP3で"AART_v2", "DISC_v2"に対応
>MP4で"AART"に対応
>OGGで"AART", "DISC"に対応
・mp3infp_SetConf()の引数をconstポインタに変更

2009-12-01 Ver.2.54h/u8
・mp3infp_GetValue()/mp3infp_SetValue()関連の修正
>WAVで"TRACK"に対応

2009-10-12 Ver.2.54g/u1
Unicode版APIを追加

2006-07-17 Ver.2.53
mp3infp_GetType()/mp3infp_GetValue()/mp3infp_SetValue()でmp4に対応

  
2005-04-19 Ver.2.50
・mp3infp_GetValue()/mp3infp_SetValue()関連の修正
>"ENC_v2"を削除
>設定したmp3-apeタグが保存されない問題を修正
  
2005-04-18 Ver.2.49
・mp3infp_GetValue()/mp3infp_SetValue()関連の修正
>"IENG"が正しく機能しない問題を修正
>"ENC_v2" "ENC2_v2"を追加
>"INAM_APE"/"IART_APE"/"IPRD_APE"/"ICMT_APE"/"ICRD_APE"/"IGNR_APE"/"TRACK_APE"を追加

2005-02-21 Ver.2.48
・mp3infp_ViewPropEx()を追加

2004-10-13 Ver.2.47
・mp3infp_mp3_MakeApeTag()/mp3infp_mp3_DelApeTag()を追加
・#define MP3INFP_HAS_MP3_APEV1	0x00000100	// v2.47～
　#define MP3INFP_HAS_MP3_APEV2	0x00000200	// v2.47～
　を追加

2004-01-2x Ver.2.43
・IN hWndを廃止
・char *szFileName -> const char *szFileNameに変更
・char *szValueName -> const char *szValueNameに変更
・cahr *buf -> const char *bufに変更(mp3infp_SetValue()のみ)
・mp3infp_SetConf()の戻り値を 0/-1 -> TRUE/FALSE に変更
・書込み系APIを追加

■mp3infp APIの利用・dllの再配布について　2002-09-16/2004-01-20(編集)

1.APIを利用する前に必ず mp3infp_GetVer() を呼び出して、必要なバージョンを調べてください
(古いmp3infpはmp3infp_GetVer()をサポートしていないため、エントリポイントのNULLチェックも必要です)
2.mp3infp.dllのフリーソフトへの同梱はLGPLに従って行ってください。
・この場合、利用ソフトのカレントDirにmp3infp.dllを置いて利用します(system/system32やwindows/winntには置かないでください)
・カレントとWindowsシステム(mp3infpがインストールされているdir)のどちらを優先してロードするのか、よく考えて設計してください
(どちらを優先させるかは開発者の自由です)
*/

#if __cplusplus
extern "C"{
#endif

/*=======================================================================================
名前：	mp3infp_GetVer
概要：	mp3infpのバージョンを取得する
引数：	なし
戻値：	DWORD	バージョン情報 
			Ver.2.11	= 0x0211
備考：	
対応：	Ver2.11～
*/
DWORD __stdcall mp3infp_GetVer();
typedef DWORD (WINAPI *LPMP3INFP_GETVER)();

/*=======================================================================================
名前：	mp3infp_ViewPropEx
概要：	mp3infpに対応したファイル形式のプロパティを開く(モーダルダイアログ版)
説明：	指定ファイルのプロパティをmp3infpのタブをアクティブにして開きます
		※シェルエクステンションを使用せずにmp3infp.dll単独の動作となります
		※シェルエクステンション標準のプロパティページは表示されません
引数：
		IN HWND hWnd		呼び出し元ウインドウハンドル
							呼び出し元ウインドウ上にダイアログを表示します
							NULLならデスクトップを指定したとみなします
		IN char *szFileName	対象ファイル名をフルパスで指定。
		IN DWORD dwPage		・mp3infpの何ページ目をアクティブにするか指定する(0=ID3v1 / 1=ID3v2 / 2=RiffSIF / 3=APE(Ver2.47))
							・タグを含まないmp3の場合のみ有効
							・タグを含む場合はID3v2/APE/RiffSIF/ID3v1の順で検索して、最初に見つかったタグをアクティブにします
		IN BOOL modeless	TRUEならプロパティを表示したまま制御を返します。戻り値にはプロパティのウインドウハンドルが入ります。
							FALSEならプロパティを閉じるまで制御を返しません。
		DWORD param1		未使用(0を指定してください)
		DWORD param2		未使用(0を指定してください)

戻値：	int 成功=0以上/失敗=-1
対応：	Ver2.48～
*/
int __stdcall mp3infp_ViewPropExA(HWND hWnd,const char *szFileName,DWORD dwPage,BOOL modeless,DWORD param1,DWORD param2);
typedef int (WINAPI *LPMP3INFP_VIEWPROPEXA)(HWND hWnd,const char *szFileName,DWORD dwPage,BOOL modeless,DWORD param1,DWORD param2);

int __stdcall mp3infp_ViewPropExW(HWND hWnd,LPCWSTR szFileName,DWORD dwPage,BOOL modeless,DWORD param1,DWORD param2);
typedef int (WINAPI *LPMP3INFP_VIEWPROPEXW)(HWND hWnd,LPCWSTR szFileName,DWORD dwPage,BOOL modeless,DWORD param1,DWORD param2);

#ifdef UNICODE
#define mp3infp_ViewPropEx		mp3infp_ViewPropExW
#define LPMP3INFP_VIEWPROPEX	LPMP3INFP_VIEWPROPEXW
#else
#define mp3infp_ViewPropEx		mp3infp_ViewPropExA
#define LPMP3INFP_VIEWPROPEX	LPMP3INFP_VIEWPROPEXA
#endif

/*=======================================================================================
名前：	mp3infp_ViewProp
概要：	mp3infpに対応したファイル形式のプロパティを開く
説明：	指定ファイルのプロパティをmp3infpのタブをアクティブにして開きます
引数：
		IN HWND hWnd		呼び出し元ウインドウハンドル
		IN char *szFileName	対象ファイル名をフルパスで指定。
		IN DWORD dwPage		・mp3infpの何ページ目をアクティブにするか指定する(0=ID3v1 / 1=ID3v2 / 2=RiffSIF / 3=APE(Ver2.47))
							・タグを含まないmp3の場合のみ有効
							・タグを含む場合はID3v2/APE/RiffSIF/ID3v1の順で検索して、最初に見つかったタグをアクティブにします

戻値：	BOOL 成功=TRUE/失敗=FALSE
対応：	Ver2.00～
*/
BOOL __stdcall mp3infp_ViewPropA(HWND hWnd,const char *szFileName,DWORD dwPage);
typedef BOOL (WINAPI *LPMP3INFP_VIEWPROPA)(HWND hWnd,const char *szFileName,DWORD dwPage);

BOOL __stdcall mp3infp_ViewPropW(HWND hWnd,LPCWSTR szFileName,DWORD dwPage);
typedef BOOL (WINAPI *LPMP3INFP_VIEWPROPW)(HWND hWnd,LPCWSTR szFileName,DWORD dwPage);

#ifdef UNICODE
#define mp3infp_ViewProp	mp3infp_ViewPropW
#define LPMP3INFP_VIEWPROP	LPMP3INFP_VIEWPROPW
#else
#define mp3infp_ViewProp	mp3infp_ViewPropA
#define LPMP3INFP_VIEWPROP	LPMP3INFP_VIEWPROPA
#endif

/*=======================================================================================
名前：	mp3infp_Load
概要：	タグ情報をロードする
引数：
		IN HWND hWnd		呼び出し元ウインドウを指定します。無い場合はNULL。
		IN const char *szFileName	対象ファイル名をフルパスで指定。
戻値：	DWORD 
		-1=ロード失敗
		ERROR_SUCCESS=成功
		(その他)=Win32エラーコード (FormatMessageで文字列を取得できる)
対応：	Ver2.26～
*/
DWORD __stdcall mp3infp_LoadA(HWND hWnd,const char *szFileName);
typedef DWORD (WINAPI *LPMP3INFP_LOADA)(HWND hWnd,const char *szFileName);

DWORD __stdcall mp3infp_LoadW(HWND hWnd,LPCWSTR szFileName);
typedef DWORD (WINAPI *LPMP3INFP_LOADW)(HWND hWnd,LPCWSTR szFileName);

#ifdef UNICODE
#define mp3infp_Load	mp3infp_LoadW
#define LPMP3INFP_LOAD	LPMP3INFP_LOADW
#else
#define mp3infp_Load	mp3infp_LoadA
#define LPMP3INFP_LOAD	LPMP3INFP_LOADA
#endif

/*=======================================================================================
////////////////////////////////////////////////////////////////////
表1　mp3infp_GetValue()/mp3infp_SetValue()でszValueNameに指定する名前一覧
////////////////////////////////////////////////////////////////////

	[共通](※1)
	ファイル名				"FILE"	(v2.41～)
	拡張子					"FEXT"	(v2.41～)
	パス					"PATH"	(v2.41～)
	サイズ(byte単位)		"SIZ1"	(v2.41～)
	サイズ(Kbyte単位)		"SIZK"	(v2.41～)
	サイズ(Mbyte単位)		"SIZM"	(v2.41～)

	[MP3]					ID3v1		ID3v2		RiffSIF		APE
	フォーマット(※1)		"AFMT"		"AFMT"		"AFMT"		"AFMT"
	演奏時間(※1)			"TIME"		"TIME"		"TIME"		"TIME"
	タイトル				"INAM_v1"	"INAM_v2"	"INAM_rmp"	"INAM_APE"
	アーティスト			"IART_v1"	"IART_v2"	"IART_rmp"	"IART_APE"
	アルバム				"IPRD_v1"	"IPRD_v2"	"IPRD_rmp"	"IPRD_APE"
	コメント				"ICMT_v1"	"ICMT_v2"	"ICMT_rmp"	"ICMT_APE"
	作成日					"ICRD_v1"	"ICRD_v2"	"ICRD_rmp"	"ICRD_APE"
	ジャンル				"IGNR_v1"	"IGNR_v2"	"IGNR_rmp"	"IGNR_APE"
	(ID3v2/RiffSIF)
	著作権								"ICOP_v2"	"ICOP_rmp"
	ソフトウェア/エンコーダ				"ISFT_v2"	"ISFT_rmp"
	(ID3v2)
	アルバムアーティスト				"AART_v2" (v2.55～)
	作曲								"COMP_v2"
	ディスク番号						"DISC_v2" (v2.55～)
	Orig.アーティスト					"OART_v2"
	URL									"URL_v2"
	エンコードした人					"ENC2_v2"
	(RiffSIF)
	ソース											"ISRC_rmp"
	エンジニア										"IENG_rmp"
	(ID3v1/2/APE)
	トラック番号			"TRACK_v1"	"TRACK_v2"				"TRACK_APE"

	[WAV]
	フォーマット(※1)		"AFMT"
	演奏時間(※1)			"TIME"
	タイトル(※2)			"INAM"
	タイトル(※2)			"ISBJ"
	トラック				"TRACK"	(v2.54h/u8～)
	アーティスト			"IART"
	アルバム				"IPRD"
	コメント				"ICMT"
	作成日					"ICRD"
	ジャンル				"IGNR"
	著作権					"ICOP"
	ソフトウェア			"ISFT"
	ソース					"ISRC"
	エンジニア				"IENG"

	[AVI]
	音声フォーマット(※1)	"AFMT"	
	映像フォーマット(※1)	"VFMT"
	時間(※1)				"TIME"
	タイトル(※2)			"INAM"
	タイトル(※2)			"ISBJ"
	アーティスト			"IART"
	コメント				"ICMT"
	作成日					"ICRD"
	ジャンル				"IGNR"
	著作権					"ICOP"
	ソフトウェア			"ISFT"
	ソース					"ISRC"
	エンジニア				"IENG"
	AVIバージョン			"AVIV"	(v2.37～)

	[VQF]
	フォーマット(※1)		"AFMT"
	演奏時間(※1)			"TIME"
	タイトル				"INAM"
	アーティスト			"IART"
	コメント				"ICMT"
	著作権					"ICOP"
	保存名					"FILE"

	[WMA]
	音声フォーマット(※1)	"AFMT"
	映像フォーマット(※1)	"VFMT"
	時間(※1)				"TIME"
	タイトル				"INAM"
	トラック				"TRACK"
	アーティスト			"IART"
	アルバム				"IPRD"
	コメント				"ICMT"
	作成日					"ICRD"
	ジャンル				"IGNR"
	著作権					"ICOP"
	URL(Album)				"URL1"
	URL(関連)				"URL2"

	[OGG]
	フォーマット(※1)		"AFMT"
	演奏時間(※1)			"TIME"
	タイトル				"INAM"
	アーティスト			"IART"
	アルバム				"IPRD"
	アルバムアーティスト	"AART" (v2.55～)
	コメント				"ICMT"
	作成日					"ICRD"
	ジャンル				"IGNR"
	トラック番号			"TRACK"
	ディスク番号			"DISC" (v2.55～)

	[APE]
	フォーマット(※1)		"AFMT"
	演奏時間(※1)			"TIME"
	タイトル				"INAM"
	アーティスト			"IART"
	アルバム				"IPRD"
	コメント				"ICMT"
	作成日					"ICRD"
	ジャンル				"IGNR"
	トラック番号			"TRACK"

	[MP4]	(v2.53～)
	音声フォーマット(※1)	"AFMT"
	映像フォーマット(※1)	"VFMT"
	タイトル				"INAM"
	アーティスト			"IART"
	アルバム				"IPRD"
	アルバムアーティスト	"AART"	(v2.55～)
	グループ				"IGRP"
	作曲					"COMPOSER"
	ジャンル				"IGNR"
	トラック番号1			"TRACK1"		(1以上の数値)
	トラック番号2			"TRACK2"		(1以上の数値)
	ディスク番号1			"DISC1"			(1以上の数値)
	ディスク番号2			"DISC2"			(1以上の数値)
	テンポ					"BPM"			(数値)
	作成日					"ICRD"			(4桁の数値 例："2004")
	コンピレーション		"COMPILATION"	("1" or "0")
	コメント				"ICMT"
	ツール					"TOOL"


(※1)mp3infp_SetValue()では利用できません。
(※2)mp3infpではロード時にINAMを優先、無ければISBJを表示。セーブ時にはISBJを削除、INAMを保存します。

*/

#define MP3INFP_FILE_UNKNOWN	0x00
#define MP3INFP_FILE_MP3		0x01
#define MP3INFP_FILE_WAV		0x02
#define MP3INFP_FILE_AVI		0x03
#define MP3INFP_FILE_VQF		0x04
#define MP3INFP_FILE_WMA		0x05
#define MP3INFP_FILE_OGG		0x07
#define MP3INFP_FILE_APE		0x08
#define MP3INFP_FILE_MP4		0x09
/*=======================================================================================
名前：	mp3infp_GetType
概要：	ファイルの種類を取得する
補足：	mp3infp_Load()の後に呼び出してください
引数：	なし
戻値：	DWORD
#define MP3INFP_FILE_UNKNOWN	0x00
#define MP3INFP_FILE_MP3		0x01
#define MP3INFP_FILE_WAV		0x02
#define MP3INFP_FILE_AVI		0x03
#define MP3INFP_FILE_VQF		0x04
#define MP3INFP_FILE_WMA		0x05
#define MP3INFP_FILE_OGG		0x07
#define MP3INFP_FILE_APE		0x08
対応：	Ver2.26～
#define MP3INFP_FILE_MP4		0x09
対応：	Ver2.53～
*/
DWORD __stdcall mp3infp_GetType();
typedef DWORD (WINAPI *LPMP3INFP_GETTYPE)();

/*=======================================================================================
名前：	mp3infp_GetValue
概要：	タグ情報を取得する
補足：	mp3infp_Load()の後に呼び出してください
        本関数を複数回呼び出すと、以前の結果は無効になります
引数：	
		IN const char *szValueName	タグの種類を示す名前(表1を参照)
		OUT char **buf			タグ情報を示すバッファのポインタを受け取るポインタ
戻値：	BOOL 成功=TRUE/失敗=FALSE
対応：	Ver2.26～
*/
BOOL __stdcall mp3infp_GetValueA(const char *szValueName,char **buf);
typedef BOOL (WINAPI *LPMP3INFP_GETVALUEA)(const char *szValueName,char **buf);

BOOL __stdcall mp3infp_GetValueW(LPCWSTR szValueName,WCHAR **buf);
typedef BOOL (WINAPI *LPMP3INFP_GETVALUEW)(LPCWSTR szValueName,WCHAR **buf);

#ifdef UNICODE
#define mp3infp_GetValue	mp3infp_GetValueW
#define LPMP3INFP_GETVALUE	LPMP3INFP_GETVALUEW
#else
#define mp3infp_GetValue	mp3infp_GetValueA
#define LPMP3INFP_GETVALUE	LPMP3INFP_GETVALUEA
#endif

#define MP3INFP_HAS_MP3_ID3V1	0x00000001
#define MP3INFP_HAS_MP3_ID3V2	0x00000002
#define MP3INFP_HAS_MP3_RIFFSIF	0x00000004
#define MP3INFP_HAS_MP3_ID3V1_0	0x00000008	// v2.43～
#define MP3INFP_HAS_MP3_ID3V1_1	0x00000010	// v2.43～
#define MP3INFP_HAS_MP3_ID3V2_2	0x00000020	// v2.43～
#define MP3INFP_HAS_MP3_ID3V2_3	0x00000040	// v2.43～
#define MP3INFP_HAS_MP3_ID3V2_4	0x00000080	// v2.43～
#define MP3INFP_HAS_MP3_APEV1	0x00000100	// v2.47～
#define MP3INFP_HAS_MP3_APEV2	0x00000200	// v2.47～
/*=======================================================================================
名前：	mp3infp_mp3_GetTagType
概要：	mp3が持っているタグの種類を取得する
補足：	mp3infp_Load()の後に呼び出してください
引数：	なし
戻値：	DWORD (OR)
#define MP3INFP_HAS_MP3_ID3V1	0x00000001
#define MP3INFP_HAS_MP3_ID3V2	0x00000002
#define MP3INFP_HAS_MP3_RIFFSIF	0x00000004
#define MP3INFP_HAS_MP3_ID3V1_0	0x00000008	// v2.43～
#define MP3INFP_HAS_MP3_ID3V1_1	0x00000010	// v2.43～
#define MP3INFP_HAS_MP3_ID3V2_2	0x00000020	// v2.43～
#define MP3INFP_HAS_MP3_ID3V2_3	0x00000040	// v2.43～
#define MP3INFP_HAS_MP3_ID3V2_4	0x00000080	// v2.43～
#define MP3INFP_HAS_MP3_APEV1	0x00000100	// v2.47～
#define MP3INFP_HAS_MP3_APEV2	0x00000200	// v2.47～
対応：	Ver2.27～
*/
DWORD __stdcall mp3infp_mp3_GetTagType();
typedef DWORD (WINAPI *LPMP3INFP_MP3_GETTAGTYPE)();

/*=======================================================================================
名前：	mp3infp_SetConf
概要：	mp3infpの動作設定
引数：	IN const char *tag		設定項目(表2参照)
		IN const char *val		設定値(表2参照)
戻値：	BOOL 成功=TRUE/失敗=FALSE
対応：	Ver2.42～
補足：	・他のプロセスのmp3infp.dll/シェル拡張のmp3infpには影響しない
		・設定内容は保存されない
*/
BOOL __stdcall mp3infp_SetConfA(const char *tag,const char *value);
typedef BOOL (WINAPI *LPMP3INFP_SETCONFA)(const char *tag,const char *value);

BOOL __stdcall mp3infp_SetConfW(LPCWSTR tag,LPCWSTR value);
typedef BOOL (WINAPI *LPMP3INFP_SETCONFW)(LPCWSTR tag,LPCWSTR value);

#ifdef UNICODE
#define mp3infp_SetConf		mp3infp_SetConfW
#define LPMP3INFP_SETCONF	LPMP3INFP_SETCONFW
#else
#define mp3infp_SetConf		mp3infp_SetConfA
#define LPMP3INFP_SETCONF	LPMP3INFP_SETCONFA
#endif

/*=======================================================================================
////////////////////////////////////////////////////////////////////
mp3infp_SetConf()指定する設定項目・値一覧
////////////////////////////////////////////////////////////////////

	[Waveファイルのコーデック名称の取得方法](Ver2.42～)
	(項目名)
	"wave_CodecFind"
	(値)
	"0"(default)	mp3infp内蔵辞書 → Windows APIを利用の順で検索(高速)
	"1"				Windows APIを利用 → 自力解析の順で検索(低速)
	"2"				mp3infp内蔵辞書(高速)
	"3"				Windows APIを利用(低速)
	
	[Aviファイルのコーデック名称の取得方法](Ver2.42～)
	(項目名)
	"avi_CodecFind"
	(値)
	"0"(default)	mp3infp内蔵辞書 → Windows APIを利用の順で検索(高速)
	"1"				Windows APIを利用 → 自力解析の順で検索(低速)
	"2"				mp3infp内蔵辞書(高速)
	"3"				Windows APIを利用(低速)

	[ID3v1で拡張ジャンルを使用する](Ver2.43～)
	(項目名)
	"mp3_UseExtGenre"
	(値)
	"0"(default)	無効
	"1"				有効

	[ID3v2で文字列をUnicodeで書き込む](Ver2.43～)
	(項目名)
	"mp3_ID3v2Unicode"
	(値)
	"0"				無効
	"1"(default)	有効 (UTF-16)
	"2"				UTF-16BE (ID3v2.4のみ) (Ver2.55～)
	"3"				UTF-8 (ID3v2.4のみ) (Ver2.55～)

	[ID3v2を非同期化する](Ver2.43～)
	(項目名)
	"mp3_ID3v2Unsync"
	(値)
	"0"				無効
	"1"(default)	有効

	[保存時のID3v2バージョン](Ver2.43～)
	※この設定値はmp3infp_Load()の実行によって上書きされます。
	(項目名)
	"mp3_SaveID3v2Version"
	(値)
	"2.2"			ID3v2.2
	"2.3"(default)	ID3v2.3
	"2.4"			ID3v2.4
*/

/*=======================================================================================
名前：	mp3infp_SetValue
概要：	タグ情報を設定する
補足：	mp3infp_Load()の後に呼び出してください
引数：	
		IN const char *szValueName	タグの種類を示す名前(表1を参照)
		IN const char *buf			タグ情報を示す文字列
戻値：	DWORD
		-1=失敗
		ERROR_SUCCESS=成功
対応：	Ver.2.43～
*/
DWORD __stdcall mp3infp_SetValueA(const char *szValueName,const char *buf);
typedef DWORD (WINAPI *LPMP3INFP_SETVALUEA)(const char *szValueName,const char *buf);

DWORD __stdcall mp3infp_SetValueW(LPCWSTR szValueName,LPCWSTR buf);
typedef DWORD (WINAPI *LPMP3INFP_SETVALUEW)(LPCWSTR szValueName,LPCWSTR buf);

#ifdef UNICODE
#define mp3infp_SetValue	mp3infp_SetValueW
#define LPMP3INFP_SETVALUE	LPMP3INFP_SETVALUEW
#else
#define mp3infp_SetValue	mp3infp_SetValueA
#define LPMP3INFP_SETVALUE	LPMP3INFP_SETVALUEA
#endif

/*=======================================================================================
名前：	mp3infp_Save
概要：	タグ情報を保存する
補足：	mp3infp_Load()の後に呼び出してください
引数：	IN const char *szFileName	対象ファイル名をフルパスで指定。
戻値：	DWORD 
		-1=失敗
		-2=2Gバイトを超えるファイルを扱うことはできません。(WAVファイルのみ)
		ERROR_SUCCESS=成功
		(その他)=Win32エラーコード (FormatMessageで文字列を取得できる)
対応：	Ver.2.43～
*/
DWORD __stdcall mp3infp_SaveA(const char *szFileName);
typedef DWORD (WINAPI *LPMP3INFP_SAVEA)(const char *szValueName);

DWORD __stdcall mp3infp_SaveW(LPCWSTR szFileName);
typedef DWORD (WINAPI *LPMP3INFP_SAVEW)(LPCWSTR szValueName);

#ifdef UNICODE
#define mp3infp_Save	mp3infp_SaveW
#define LPMP3INFP_SAVE	LPMP3INFP_SAVEW
#else
#define mp3infp_Save	mp3infp_SaveA
#define LPMP3INFP_SAVE	LPMP3INFP_SAVEA
#endif

/*=======================================================================================
名前：	mp3infp_mp3_MakeId3v1
概要：	ID3TAG V1を作成する
補足：	mp3infp_Load()の後に呼び出してください
		変更は直ちに反映されます
		mp3ファイルにのみ利用してください(wavファイルは対象外)
引数：	IN char *szFileName	対象ファイル名をフルパスで指定。
戻値：	DWORD 
		-1=失敗
		ERROR_SUCCESS=成功
		(その他)=Win32エラーコード (FormatMessageで文字列を取得できる)
対応：	Ver.2.43～
*/
DWORD __stdcall mp3infp_mp3_MakeId3v1A(const char *szFileName);
typedef DWORD (WINAPI *LPMP3INFP_MP3_MAKEID3V1A)(const char *szValueName);

DWORD __stdcall mp3infp_mp3_MakeId3v1W(LPCWSTR szFileName);
typedef DWORD (WINAPI *LPMP3INFP_MP3_MAKEID3V1W)(LPCWSTR szValueName);

#ifdef UNICODE
#define mp3infp_mp3_MakeId3v1	mp3infp_mp3_MakeId3v1W
#define LPMP3INFP_MP3_MAKEID3V1	LPMP3INFP_MP3_MAKEID3V1W
#else
#define mp3infp_mp3_MakeId3v1	mp3infp_mp3_MakeId3v1A
#define LPMP3INFP_MP3_MAKEID3V1	LPMP3INFP_MP3_MAKEID3V1A
#endif

/*=======================================================================================
名前：	mp3infp_mp3_DelId3v1
概要：	ID3TAG V1を削除する
補足：	mp3infp_Load()の後に呼び出してください
		変更は直ちに反映されます
		mp3ファイルにのみ利用してください(wavファイルは対象外)
引数：	IN char *szFileName	対象ファイル名をフルパスで指定。
戻値：	DWORD 
		-1=失敗
		ERROR_SUCCESS=成功
		(その他)=Win32エラーコード (FormatMessageで文字列を取得できる)
対応：	Ver.2.43～
*/
DWORD __stdcall mp3infp_mp3_DelId3v1A(const char *szFileName);
typedef DWORD (WINAPI *LPMP3INFP_MP3_DELID3V1A)(const char *szValueName);

DWORD __stdcall mp3infp_mp3_DelId3v1W(LPCWSTR szFileName);
typedef DWORD (WINAPI *LPMP3INFP_MP3_DELID3V1W)(LPCWSTR szValueName);

#ifdef UNICODE
#define mp3infp_mp3_DelId3v1	mp3infp_mp3_DelId3v1W
#define LPMP3INFP_MP3_DELID3V1	LPMP3INFP_MP3_DELID3V1W
#else
#define mp3infp_mp3_DelId3v1	mp3infp_mp3_DelId3v1A
#define LPMP3INFP_MP3_DELID3V1	LPMP3INFP_MP3_DELID3V1A
#endif

/*=======================================================================================
名前：	mp3infp_mp3_MakeId3v2
概要：	ID3TAG V2を作成する
補足：	mp3infp_Load()の後に呼び出してください
		変更は直ちに反映されます
		mp3ファイルにのみ利用してください(wavファイルは対象外)
引数：	IN char *szFileName	対象ファイル名をフルパスで指定。
戻値：	DWORD 
		-1=失敗
		ERROR_SUCCESS=成功
		(その他)=Win32エラーコード (FormatMessageで文字列を取得できる)
対応：	Ver.2.43～
*/
DWORD __stdcall mp3infp_mp3_MakeId3v2A(const char *szFileName);
typedef DWORD (WINAPI *LPMP3INFP_MP3_MAKEID3V2A)(const char *szValueName);

DWORD __stdcall mp3infp_mp3_MakeId3v2W(LPCWSTR szFileName);
typedef DWORD (WINAPI *LPMP3INFP_MP3_MAKEID3V2W)(LPCWSTR szValueName);

#ifdef UNICODE
#define mp3infp_mp3_MakeId3v2	mp3infp_mp3_MakeId3v2W
#define LPMP3INFP_MP3_MAKEID3V2	LPMP3INFP_MP3_MAKEID3V2W
#else
#define mp3infp_mp3_MakeId3v2	mp3infp_mp3_MakeId3v2A
#define LPMP3INFP_MP3_MAKEID3V2	LPMP3INFP_MP3_MAKEID3V2A
#endif

/*=======================================================================================
名前：	mp3infp_mp3_DelId3v2
概要：	ID3TAG V2を削除する
補足：	mp3infp_Load()の後に呼び出してください
		変更は直ちに反映されます
		mp3ファイルにのみ利用してください(wavファイルは対象外)
引数：	IN char *szFileName	対象ファイル名をフルパスで指定。
戻値：	DWORD 
		-1=失敗
		ERROR_SUCCESS=成功
		(その他)=Win32エラーコード (FormatMessageで文字列を取得できる)
対応：	Ver.2.43～
*/
DWORD __stdcall mp3infp_mp3_DelId3v2A(const char *szFileName);
typedef DWORD (WINAPI *LPMP3INFP_MP3_DELID3V2A)(const char *szValueName);

DWORD __stdcall mp3infp_mp3_DelId3v2W(LPCWSTR szFileName);
typedef DWORD (WINAPI *LPMP3INFP_MP3_DELID3V2W)(LPCWSTR szValueName);

#ifdef UNICODE
#define mp3infp_mp3_DelId3v2	mp3infp_mp3_DelId3v2W
#define LPMP3INFP_MP3_DELID3V2	LPMP3INFP_MP3_DELID3V2W
#else
#define mp3infp_mp3_DelId3v2	mp3infp_mp3_DelId3v2A
#define LPMP3INFP_MP3_DELID3V2	LPMP3INFP_MP3_DELID3V2A
#endif

/*=======================================================================================
名前：	mp3infp_mp3_MakeRMP
概要：	mp3形式からRMP形式に変換する
補足：	mp3infp_Load()の後に呼び出してください
		変更は直ちに反映されます
		mp3ファイルにのみ利用してください(wavファイルは対象外)
引数：	IN char *szFileName	対象ファイル名をフルパスで指定。
戻値：	DWORD 
		-1=失敗
		ERROR_SUCCESS=成功
		(その他)=Win32エラーコード (FormatMessageで文字列を取得できる)
対応：	Ver.2.43～
*/
DWORD __stdcall mp3infp_mp3_MakeRMPA(const char *szFileName);
typedef DWORD (WINAPI *LPMP3INFP_MP3_MAKERMPA)(const char *szValueName);

DWORD __stdcall mp3infp_mp3_MakeRMPW(LPCWSTR szFileName);
typedef DWORD (WINAPI *LPMP3INFP_MP3_MAKERMPW)(LPCWSTR szValueName);

#ifdef UNICODE
#define mp3infp_mp3_MakeRMP		mp3infp_mp3_MakeRMPW
#define LPMP3INFP_MP3_MAKERMP	LPMP3INFP_MP3_MAKERMPW
#else
#define mp3infp_mp3_MakeRMP		mp3infp_mp3_MakeRMPA
#define LPMP3INFP_MP3_MAKERMP	LPMP3INFP_MP3_MAKERMPA
#endif

/*=======================================================================================
名前：	mp3infp_mp3_DelRMP
概要：	RMP形式からmp3形式に変換する
補足：	mp3infp_Load()の後に呼び出してください
		変更は直ちに反映されます
		mp3ファイルにのみ利用してください(wavファイルは対象外)
引数：	IN char *szFileName	対象ファイル名をフルパスで指定。
戻値：	DWORD 
		-1=失敗
		ERROR_SUCCESS=成功
		(その他)=Win32エラーコード (FormatMessageで文字列を取得できる)
対応：	Ver.2.43～
*/
DWORD __stdcall mp3infp_mp3_DelRMPA(const char *szFileName);
typedef DWORD (WINAPI *LPMP3INFP_MP3_DELRMPA)(const char *szValueName);

DWORD __stdcall mp3infp_mp3_DelRMPW(LPCWSTR szFileName);
typedef DWORD (WINAPI *LPMP3INFP_MP3_DELRMPW)(LPCWSTR szValueName);

#ifdef UNICODE
#define mp3infp_mp3_DelRMP		mp3infp_mp3_DelRMPW
#define LPMP3INFP_MP3_DELRMP	LPMP3INFP_MP3_DELRMPW
#else
#define mp3infp_mp3_DelRMP		mp3infp_mp3_DelRMPA
#define LPMP3INFP_MP3_DELRMP	LPMP3INFP_MP3_DELRMPA
#endif

/*=======================================================================================
名前：	mp3infp_mp3_MakeApeTag
概要：	APE Tagを作成する
補足：	mp3infp_Load()の後に呼び出してください
		変更は直ちに反映されます
		mp3ファイルにのみ利用してください(wavファイルは対象外)
引数：	IN char *szFileName	対象ファイル名をフルパスで指定。
戻値：	DWORD 
		-1=失敗
		ERROR_SUCCESS=成功
		(その他)=Win32エラーコード (FormatMessageで文字列を取得できる)
対応：	Ver.2.47～
*/
DWORD __stdcall mp3infp_mp3_MakeApeTagA(const char *szFileName);
typedef DWORD (WINAPI *LPMP3INFP_MP3_MAKEAPETAGA)(const char *szValueName);

DWORD __stdcall mp3infp_mp3_MakeApeTagW(LPCWSTR szFileName);
typedef DWORD (WINAPI *LPMP3INFP_MP3_MAKEAPETAGW)(LPCWSTR szValueName);

#ifdef UNICODE
#define mp3infp_mp3_MakeApeTag		mp3infp_mp3_MakeApeTagW
#define LPMP3INFP_MP3_MAKEAPETAG	LPMP3INFP_MP3_MAKEAPETAGW
#else
#define mp3infp_mp3_MakeApeTag		mp3infp_mp3_MakeApeTagA
#define LPMP3INFP_MP3_MAKEAPETAG	LPMP3INFP_MP3_MAKEAPETAGA
#endif

/*=======================================================================================
名前：	mp3infp_mp3_DelApeTag
概要：	APE Tagを削除する
補足：	mp3infp_Load()の後に呼び出してください
		変更は直ちに反映されます
		mp3ファイルにのみ利用してください(wavファイルは対象外)
引数：	IN char *szFileName	対象ファイル名をフルパスで指定。
戻値：	DWORD 
		-1=失敗
		ERROR_SUCCESS=成功
		(その他)=Win32エラーコード (FormatMessageで文字列を取得できる)
対応：	Ver.2.47～
*/
DWORD __stdcall mp3infp_mp3_DelApeTagA(const char *szFileName);
typedef DWORD (WINAPI *LPMP3INFP_MP3_DELAPETAGA)(const char *szValueName);

DWORD __stdcall mp3infp_mp3_DelApeTagW(LPCWSTR szFileName);
typedef DWORD (WINAPI *LPMP3INFP_MP3_DELAPETAGW)(LPCWSTR szValueName);

#ifdef UNICODE
#define mp3infp_mp3_DelApeTag	mp3infp_mp3_DelApeTagW
#define LPMP3INFP_MP3_DELAPETAG	LPMP3INFP_MP3_DELAPETAGW
#else
#define mp3infp_mp3_DelApeTag	mp3infp_mp3_DelApeTagA
#define LPMP3INFP_MP3_DELAPETAG	LPMP3INFP_MP3_DELAPETAGA
#endif

#if __cplusplus
}
#endif

#endif //_MP3INFP_EXPORT_FUNC_H
