// 2000.10.18 - mp3infp�����ɐV�K�쐬
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

//�����l
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
/* �S������InfoTip
�t�@�C����:\t\FILE\FEXT\n�t�@�C���T�C�Y(bytes):\t\SIZ1\n�t�@�C���T�C�YKB:\t\SIZK\n�t�@�C���T�C�Y(MB):\t\SIZM\n�^�C�g��:\t\t\INAM\n�A�[�e�B�X�g:\t\IART\n�A���o��:\t\IPRD\n�O���[�v:\t\IGRP\n���:\t\t\COMPOSER\n�W������:\t\t\IGNR\n�g���b�N�ԍ�:\t\TRACK\n�f�B�X�N�ԍ�:\t\DISC\n�e���|(BPM):\tBPM\n����:\t\t\ICRD\n�R���s���[�V����:\t\COMPILATION\n�R�����g:\t\t\ICMT\n�����t�H�[�}�b�g:\n\AFMT\n�f���t�H�[�}�b�g:\n\VFMT
*/