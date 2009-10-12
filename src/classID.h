// 2000.10.18 - mp3infp向けに新規作成
// {DDB066B3-8D19-11d2-8763-006052014B08}
DEFINE_GUID(CLSID_ShellExt, 
0xddb066b3, 0x8d19, 0x11d2, 0x87, 0x63, 0x0, 0x60, 0x52, 0x1, 0x4b, 0x8);
#define CLSID_STR_ShellExt _T("{DDB066B3-8D19-11d2-8763-006052014B08}")

#define LANG_VER 248
#define SOFT_NAME _T("mp3infp Ver.2.54g/u2")
#define APP_NAME _T("mp3infp")
#define COPY_RIGHT _T("Copyright(c) 1998-2009 T-Matsuo\nmailto:tms2@win32lab.com\nhttp://win32lab.com/")
#define HELP_FILE _T("mp3infp.chm")
#define README_FILE _T("mp3infp.txt")
#define MP3INFP_REG_ENTRY _T("Software\\win32lab.com\\mp3infp")
#define M3U_INFOTIP_MAX_LINE 30

//初期値
#define DEF_SETUP_MAIN_SELECT_DRIVE			FALSE
#define DEF_SETUP_MAIN_DISABLE_FDD			TRUE
#define DEF_SETUP_MAIN_DISABLE_REMOVABLE	FALSE
#define DEF_SETUP_MAIN_DISABLE_CDROM		FALSE
#define DEF_SETUP_MAIN_DISABLE_NETWORK		FALSE
#define DEF_SETUP_MAIN_COLUMN_8_3			FALSE
#define DEF_SETUP_MAIN_LANGUAGE				_T("default")

#define DEF_SETUP_PROP_AOT					FALSE

#define DEF_M3U_PROP_ENABLE					TRUE
#define DEF_M3U_INFOTIP_ENABLE				TRUE
#define DEF_M3U_PROP_FULLPATH				2

#define DEF_OGG_PROP_ENABLE					TRUE
#define DEF_OGG_INFOTIP_ENABLE				TRUE
#define DEF_OGG_COLUMN_ENABLE				TRUE

#define DEF_APE_PROP_ENABLE					TRUE
#define DEF_APE_INFOTIP_ENABLE				TRUE
#define DEF_APE_COLUMN_ENABLE				TRUE

#define DEF_MP4_PROP_ENABLE					TRUE
#define DEF_MP4_INFOTIP_ENABLE				TRUE
#define DEF_MP4_COLUMN_ENABLE				TRUE
/* 全部入りInfoTip
ファイル名:\t\FILE\FEXT\nファイルサイズ(bytes):\t\SIZ1\nファイルサイズKB:\t\SIZK\nファイルサイズ(MB):\t\SIZM\nタイトル:\t\t\INAM\nアーティスト:\t\IART\nアルバム:\t\IPRD\nグループ:\t\IGRP\n作曲:\t\t\COMPOSER\nジャンル:\t\t\IGNR\nトラック番号:\t\TRACK\nディスク番号:\t\DISC\nテンポ(BPM):\tBPM\n西暦:\t\t\ICRD\nコンピレーション:\t\COMPILATION\nコメント:\t\t\ICMT\n音声フォーマット:\n\AFMT\n映像フォーマット:\n\VFMT
*/