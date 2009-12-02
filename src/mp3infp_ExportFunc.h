#ifndef _MP3INFP_EXPORT_FUNC_H
#define _MP3INFP_EXPORT_FUNC_H

/*
�����ϗ���
2009-12-01 Ver.2.54h/u8
�Emp3infp_GetValue()/mp3infp_SetValue()�֘A�̏C��
>WAV��"TRACK"�ɑΉ�

2009-10-12 Ver.2.54g/u1
Unicode��API��ǉ�

2006-07-17 Ver.2.53
mp3infp_GetType()/mp3infp_GetValue()/mp3infp_SetValue()��mp4�ɑΉ�

  
2005-04-19 Ver.2.50
�Emp3infp_GetValue()/mp3infp_SetValue()�֘A�̏C��
>"ENC_v2"���폜
>�ݒ肵��mp3-ape�^�O���ۑ�����Ȃ������C��
  
2005-04-18 Ver.2.49
�Emp3infp_GetValue()/mp3infp_SetValue()�֘A�̏C��
>"IENG"���������@�\���Ȃ������C��
>"ENC_v2" "ENC2_v2"��ǉ�
>"INAM_APE"/"IART_APE"/"IPRD_APE"/"ICMT_APE"/"ICRD_APE"/"IGNR_APE"/"TRACK_APE"��ǉ�

2005-02-21 Ver.2.48
�Emp3infp_ViewPropEx()��ǉ�

2004-10-13 Ver.2.47
�Emp3infp_mp3_MakeApeTag()/mp3infp_mp3_DelApeTag()��ǉ�
�E#define MP3INFP_HAS_MP3_APEV1	0x00000100	// v2.47�`
�@#define MP3INFP_HAS_MP3_APEV2	0x00000200	// v2.47�`
�@��ǉ�

2004-01-2x Ver.2.43
�EIN hWnd��p�~
�Echar *szFileName -> const char *szFileName�ɕύX
�Echar *szValueName -> const char *szValueName�ɕύX
�Ecahr *buf -> const char *buf�ɕύX(mp3infp_SetValue()�̂�)
�Emp3infp_SetConf()�̖߂�l�� 0/-1 -> TRUE/FALSE �ɕύX
�E�����݌nAPI��ǉ�

��mp3infp API�̗��p�Edll�̍Ĕz�z�ɂ��ā@2002-09-16/2004-01-20(�ҏW)

1.API�𗘗p����O�ɕK�� mp3infp_GetVer() ���Ăяo���āA�K�v�ȃo�[�W�����𒲂ׂĂ�������
(�Â�mp3infp��mp3infp_GetVer()���T�|�[�g���Ă��Ȃ����߁A�G���g���|�C���g��NULL�`�F�b�N���K�v�ł�)
2.mp3infp.dll�̃t���[�\�t�g�ւ̓�����LGPL�ɏ]���čs���Ă��������B
�E���̏ꍇ�A���p�\�t�g�̃J�����gDir��mp3infp.dll��u���ė��p���܂�(system/system32��windows/winnt�ɂ͒u���Ȃ��ł�������)
�E�J�����g��Windows�V�X�e��(mp3infp���C���X�g�[������Ă���dir)�̂ǂ����D�悵�ă��[�h����̂��A�悭�l���Đ݌v���Ă�������
(�ǂ����D�悳���邩�͊J���҂̎��R�ł�)
*/

#if __cplusplus
extern "C"{
#endif

/*=======================================================================================
���O�F	mp3infp_GetVer
�T�v�F	mp3infp�̃o�[�W�������擾����
�����F	�Ȃ�
�ߒl�F	DWORD	�o�[�W������� 
			Ver.2.11	= 0x0211
���l�F	
�Ή��F	Ver2.11�`
*/
DWORD __stdcall mp3infp_GetVer();
typedef DWORD (WINAPI *LPMP3INFP_GETVER)();

/*=======================================================================================
���O�F	mp3infp_ViewPropEx
�T�v�F	mp3infp�ɑΉ������t�@�C���`���̃v���p�e�B���J��(���[�_���_�C�A���O��)
�����F	�w��t�@�C���̃v���p�e�B��mp3infp�̃^�u���A�N�e�B�u�ɂ��ĊJ���܂�
		���V�F���G�N�X�e���V�������g�p������mp3infp.dll�P�Ƃ̓���ƂȂ�܂�
		���V�F���G�N�X�e���V�����W���̃v���p�e�B�y�[�W�͕\������܂���
�����F
		IN HWND hWnd		�Ăяo�����E�C���h�E�n���h��
							�Ăяo�����E�C���h�E��Ƀ_�C�A���O��\�����܂�
							NULL�Ȃ�f�X�N�g�b�v���w�肵���Ƃ݂Ȃ��܂�
		IN char *szFileName	�Ώۃt�@�C�������t���p�X�Ŏw��B
		IN DWORD dwPage		�Emp3infp�̉��y�[�W�ڂ��A�N�e�B�u�ɂ��邩�w�肷��(0=ID3v1 / 1=ID3v2 / 2=RiffSIF / 3=APE(Ver2.47))
							�E�^�O���܂܂Ȃ�mp3�̏ꍇ�̂ݗL��
							�E�^�O���܂ޏꍇ��ID3v2/APE/RiffSIF/ID3v1�̏��Ō������āA�ŏ��Ɍ��������^�O���A�N�e�B�u�ɂ��܂�
		IN BOOL modeless	TRUE�Ȃ�v���p�e�B��\�������܂ܐ����Ԃ��܂��B�߂�l�ɂ̓v���p�e�B�̃E�C���h�E�n���h��������܂��B
							FALSE�Ȃ�v���p�e�B�����܂Ő����Ԃ��܂���B
		DWORD param1		���g�p(0���w�肵�Ă�������)
		DWORD param2		���g�p(0���w�肵�Ă�������)

�ߒl�F	int ����=0�ȏ�/���s=-1
�Ή��F	Ver2.48�`
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
���O�F	mp3infp_ViewProp
�T�v�F	mp3infp�ɑΉ������t�@�C���`���̃v���p�e�B���J��
�����F	�w��t�@�C���̃v���p�e�B��mp3infp�̃^�u���A�N�e�B�u�ɂ��ĊJ���܂�
�����F
		IN HWND hWnd		�Ăяo�����E�C���h�E�n���h��
		IN char *szFileName	�Ώۃt�@�C�������t���p�X�Ŏw��B
		IN DWORD dwPage		�Emp3infp�̉��y�[�W�ڂ��A�N�e�B�u�ɂ��邩�w�肷��(0=ID3v1 / 1=ID3v2 / 2=RiffSIF / 3=APE(Ver2.47))
							�E�^�O���܂܂Ȃ�mp3�̏ꍇ�̂ݗL��
							�E�^�O���܂ޏꍇ��ID3v2/APE/RiffSIF/ID3v1�̏��Ō������āA�ŏ��Ɍ��������^�O���A�N�e�B�u�ɂ��܂�

�ߒl�F	BOOL ����=TRUE/���s=FALSE
�Ή��F	Ver2.00�`
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
���O�F	mp3infp_Load
�T�v�F	�^�O�������[�h����
�����F
		IN HWND hWnd		�Ăяo�����E�C���h�E���w�肵�܂��B�����ꍇ��NULL�B
		IN const char *szFileName	�Ώۃt�@�C�������t���p�X�Ŏw��B
�ߒl�F	DWORD 
		-1=���[�h���s
		ERROR_SUCCESS=����
		(���̑�)=Win32�G���[�R�[�h (FormatMessage�ŕ�������擾�ł���)
�Ή��F	Ver2.26�`
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
�\1�@mp3infp_GetValue()/mp3infp_SetValue()��szValueName�Ɏw�肷�閼�O�ꗗ
////////////////////////////////////////////////////////////////////

	[����](��1)
	�t�@�C����				"FILE"	(v2.41�`)
	�g���q					"FEXT"	(v2.41�`)
	�p�X					"PATH"	(v2.41�`)
	�T�C�Y(byte�P��)		"SIZ1"	(v2.41�`)
	�T�C�Y(Kbyte�P��)		"SIZK"	(v2.41�`)
	�T�C�Y(Mbyte�P��)		"SIZM"	(v2.41�`)

	[MP3]					ID3v1		ID3v2		RiffSIF		APE
	�t�H�[�}�b�g(��1)		"AFMT"		"AFMT"		"AFMT"		"AFMT"
	���t����(��1)			"TIME"		"TIME"		"TIME"		"TIME"
	�^�C�g��				"INAM_v1"	"INAM_v2"	"INAM_rmp"	"INAM_APE"
	�A�[�e�B�X�g			"IART_v1"	"IART_v2"	"IART_rmp"	"IART_APE"
	�A���o��				"IPRD_v1"	"IPRD_v2"	"IPRD_rmp"	"IPRD_APE"
	�R�����g				"ICMT_v1"	"ICMT_v2"	"ICMT_rmp"	"ICMT_APE"
	�쐬��					"ICRD_v1"	"ICRD_v2"	"ICRD_rmp"	"ICRD_APE"
	�W������				"IGNR_v1"	"IGNR_v2"	"IGNR_rmp"	"IGNR_APE"
	(ID3v2/RiffSIF)
	���쌠								"ICOP_v2"	"ICOP_rmp"
	�\�t�g�E�F�A/�G���R�[�_				"ISFT_v2"	"ISFT_rmp"
	(ID3v2)
	���								"COMP_v2"
	Orig.�A�[�e�B�X�g					"OART_v2"
	URL									"URL_v2"
	�G���R�[�h�����l					"ENC2_v2"
	(RiffSIF)
	�\�[�X											"ISRC_rmp"
	�G���W�j�A										"IENG_rmp"
	(ID3v1/2)
	�g���b�N�ԍ�			"TRACK_v1"	"TRACK_v2"				"TRACK_APE"

	[WAV]
	�t�H�[�}�b�g(��1)		"AFMT"
	���t����(��1)			"TIME"
	�^�C�g��(��2)			"INAM"
	�^�C�g��(��2)			"ISBJ"
	�g���b�N				"TRACK"	(v2.54h/u8�`)
	�A�[�e�B�X�g			"IART"
	�A���o��				"IPRD"
	�R�����g				"ICMT"
	�쐬��					"ICRD"
	�W������				"IGNR"
	���쌠					"ICOP"
	�\�t�g�E�F�A			"ISFT"
	�\�[�X					"ISRC"
	�G���W�j�A				"IENG"

	[AVI]
	�����t�H�[�}�b�g(��1)	"AFMT"	
	�f���t�H�[�}�b�g(��1)	"VFMT"
	����(��1)				"TIME"
	�^�C�g��(��2)			"INAM"
	�^�C�g��(��2)			"ISBJ"
	�A�[�e�B�X�g			"IART"
	�R�����g				"ICMT"
	�쐬��					"ICRD"
	�W������				"IGNR"
	���쌠					"ICOP"
	�\�t�g�E�F�A			"ISFT"
	�\�[�X					"ISRC"
	�G���W�j�A				"IENG"
	AVI�o�[�W����			"AVIV"	(v2.37�`)

	[VQF]
	�t�H�[�}�b�g(��1)		"AFMT"
	���t����(��1)			"TIME"
	�^�C�g��				"INAM"
	�A�[�e�B�X�g			"IART"
	�R�����g				"ICMT"
	���쌠					"ICOP"
	�ۑ���					"FILE"

	[WMA]
	�����t�H�[�}�b�g(��1)	"AFMT"
	�f���t�H�[�}�b�g(��1)	"VFMT"
	����(��1)				"TIME"
	�^�C�g��				"INAM"
	�g���b�N				"TRACK"
	�A�[�e�B�X�g			"IART"
	�A���o��				"IPRD"
	�R�����g				"ICMT"
	�쐬��					"ICRD"
	�W������				"IGNR"
	���쌠					"ICOP"
	URL(Album)				"URL1"
	URL(�֘A)				"URL2"

	[OGG]
	�t�H�[�}�b�g(��1)		"AFMT"
	���t����(��1)			"TIME"
	�^�C�g��				"INAM"
	�A�[�e�B�X�g			"IART"
	�A���o��				"IPRD"
	�R�����g				"ICMT"
	�쐬��					"ICRD"
	�W������				"IGNR"
	�g���b�N�ԍ�			"TRACK"

	[APE]
	�t�H�[�}�b�g(��1)		"AFMT"
	���t����(��1)			"TIME"
	�^�C�g��				"INAM"
	�A�[�e�B�X�g			"IART"
	�A���o��				"IPRD"
	�R�����g				"ICMT"
	�쐬��					"ICRD"
	�W������				"IGNR"
	�g���b�N�ԍ�			"TRACK"

	[MP4]	(v2.53�`)
	�����t�H�[�}�b�g(��1)	"AFMT"
	�f���t�H�[�}�b�g(��1)	"VFMT"
	�^�C�g��				"INAM"
	�A�[�e�B�X�g			"IART"
	�A���o��				"IPRD"
	�O���[�v				"IGRP"
	���					"COMPOSER"
	�W������				"IGNR"
	�g���b�N�ԍ�1			"TRACK1"		(1�ȏ�̐��l)
	�g���b�N�ԍ�2			"TRACK2"		(1�ȏ�̐��l)
	�f�B�X�N�ԍ�1			"DISC1"			(1�ȏ�̐��l)
	�f�B�X�N�ԍ�2			"DISC2"			(1�ȏ�̐��l)
	�e���|					"BPM"			(���l)
	�쐬��					"ICRD"			(4���̐��l ��F"2004")
	�R���s���[�V����		"COMPILATION"	("1" or "0")
	�R�����g				"ICMT"
	�c�[��					"TOOL"


(��1)mp3infp_SetValue()�ł͗��p�ł��܂���B
(��2)mp3infp�ł̓��[�h����INAM��D��A�������ISBJ��\���B�Z�[�u���ɂ�ISBJ���폜�AINAM��ۑ����܂��B

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
���O�F	mp3infp_GetType
�T�v�F	�t�@�C���̎�ނ��擾����
�⑫�F	mp3infp_Load()�̌�ɌĂяo���Ă�������
�����F	�Ȃ�
�ߒl�F	DWORD
#define MP3INFP_FILE_UNKNOWN	0x00
#define MP3INFP_FILE_MP3		0x01
#define MP3INFP_FILE_WAV		0x02
#define MP3INFP_FILE_AVI		0x03
#define MP3INFP_FILE_VQF		0x04
#define MP3INFP_FILE_WMA		0x05
#define MP3INFP_FILE_OGG		0x07
#define MP3INFP_FILE_APE		0x08
�Ή��F	Ver2.26�`
#define MP3INFP_FILE_MP4		0x09
�Ή��F	Ver2.53�`
*/
DWORD __stdcall mp3infp_GetType();
typedef DWORD (WINAPI *LPMP3INFP_GETTYPE)();

/*=======================================================================================
���O�F	mp3infp_GetValue
�T�v�F	�^�O�����擾����
�⑫�F	mp3infp_Load()�̌�ɌĂяo���Ă�������
�����F	
		IN const char *szValueName	�^�O�̎�ނ��������O(�\1���Q��)
		OUT char **buf			�^�O���������o�b�t�@�̃|�C���^���󂯎��|�C���^
�ߒl�F	BOOL ����=TRUE/���s=FALSE
�Ή��F	Ver2.26�`
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
#define MP3INFP_HAS_MP3_ID3V1_0	0x00000008	// v2.43�`
#define MP3INFP_HAS_MP3_ID3V1_1	0x00000010	// v2.43�`
#define MP3INFP_HAS_MP3_ID3V2_2	0x00000020	// v2.43�`
#define MP3INFP_HAS_MP3_ID3V2_3	0x00000040	// v2.43�`
#define MP3INFP_HAS_MP3_ID3V2_4	0x00000080	// v2.43�`
#define MP3INFP_HAS_MP3_APEV1	0x00000100	// v2.47�`
#define MP3INFP_HAS_MP3_APEV2	0x00000200	// v2.47�`
/*=======================================================================================
���O�F	mp3infp_mp3_GetTagType
�T�v�F	mp3�������Ă���^�O�̎�ނ��擾����
�⑫�F	mp3infp_Load()�̌�ɌĂяo���Ă�������
�����F	�Ȃ�
�ߒl�F	DWORD (OR)
#define MP3INFP_HAS_MP3_ID3V1	0x00000001
#define MP3INFP_HAS_MP3_ID3V2	0x00000002
#define MP3INFP_HAS_MP3_RIFFSIF	0x00000004
#define MP3INFP_HAS_MP3_ID3V1_0	0x00000008	// v2.43�`
#define MP3INFP_HAS_MP3_ID3V1_1	0x00000010	// v2.43�`
#define MP3INFP_HAS_MP3_ID3V2_2	0x00000020	// v2.43�`
#define MP3INFP_HAS_MP3_ID3V2_3	0x00000040	// v2.43�`
#define MP3INFP_HAS_MP3_ID3V2_4	0x00000080	// v2.43�`
#define MP3INFP_HAS_MP3_APEV1	0x00000100	// v2.47�`
#define MP3INFP_HAS_MP3_APEV2	0x00000200	// v2.47�`
�Ή��F	Ver2.27�`
*/
DWORD __stdcall mp3infp_mp3_GetTagType();
typedef DWORD (WINAPI *LPMP3INFP_MP3_GETTAGTYPE)();

/*=======================================================================================
���O�F	mp3infp_SetConf
�T�v�F	mp3infp�̓���ݒ�
�����F	IN char *tag		�ݒ荀��(�\2�Q��)
		IN char *val		�ݒ�l(�\2�Q��)
�ߒl�F	BOOL ����=TRUE/���s=FALSE
�Ή��F	Ver2.42�`
�⑫�F	�E���̃v���Z�X��mp3infp.dll/�V�F���g����mp3infp�ɂ͉e�����Ȃ�
		�E�ݒ���e�͕ۑ�����Ȃ�
*/
BOOL __stdcall mp3infp_SetConfA(char *tag,char *value);
typedef BOOL (WINAPI *LPMP3INFP_SETCONFA)(char *tag,char *value);

BOOL __stdcall mp3infp_SetConfW(WCHAR *tag,WCHAR *value);
typedef BOOL (WINAPI *LPMP3INFP_SETCONFW)(WCHAR *tag,WCHAR *value);

#ifdef UNICODE
#define mp3infp_SetConf		mp3infp_SetConfW
#define LPMP3INFP_SETCONF	LPMP3INFP_SETCONFW
#else
#define mp3infp_SetConf		mp3infp_SetConfA
#define LPMP3INFP_SETCONF	LPMP3INFP_SETCONFA
#endif

/*=======================================================================================
////////////////////////////////////////////////////////////////////
mp3infp_SetConf()�w�肷��ݒ荀�ځE�l�ꗗ
////////////////////////////////////////////////////////////////////

	[Wave�t�@�C���̃R�[�f�b�N���̂̎擾���@](Ver2.42�`)
	(���ږ�)
	"wave_CodecFind"
	(�l)
	"0"(default)	mp3infp�������� �� Windows API�𗘗p�̏��Ō���(����)
	"1"				Windows API�𗘗p �� ���͉�͂̏��Ō���(�ᑬ)
	"2"				mp3infp��������(����)
	"3"				Windows API�𗘗p(�ᑬ)
	
	[Avi�t�@�C���̃R�[�f�b�N���̂̎擾���@](Ver2.42�`)
	(���ږ�)
	"avi_CodecFind"
	(�l)
	"0"(default)	mp3infp�������� �� Windows API�𗘗p�̏��Ō���(����)
	"1"				Windows API�𗘗p �� ���͉�͂̏��Ō���(�ᑬ)
	"2"				mp3infp��������(����)
	"3"				Windows API�𗘗p(�ᑬ)

	[ID3v1�Ŋg���W���������g�p����](Ver2.43�`)
	(���ږ�)
	"mp3_UseExtGenre"
	(�l)
	"0"(default)	����
	"1"				�L��

	[ID3v2�ŕ������Unicode�ŏ�������](Ver2.43�`)
	(���ږ�)
	"mp3_ID3v2Unicode"
	(�l)
	"0"(default)	����
	"1"				�L��

	[ID3v2��񓯊�������](Ver2.43�`)
	(���ږ�)
	"mp3_ID3v2Unsync"
	(�l)
	"0"				����
	"1"(default)	�L��

	[�ۑ�����ID3v2�o�[�W����](Ver2.43�`)
	�����̐ݒ�l��mp3infp_Load()�̎��s�ɂ���ď㏑������܂��B
	(���ږ�)
	"mp3_SaveID3v2Version"
	(�l)
	"2.2"			ID3v2.2
	"2.3"(default)	ID3v2.3
	"2.4"			ID3v2.4
*/

/*=======================================================================================
���O�F	mp3infp_SetValue
�T�v�F	�^�O����ݒ肷��
�⑫�F	mp3infp_Load()�̌�ɌĂяo���Ă�������
�����F	
		IN const char *szValueName	�^�O�̎�ނ��������O(�\1���Q��)
		IN const char *buf			�^�O��������������
�ߒl�F	DWORD
		-1=���s
		ERROR_SUCCESS=����
�Ή��F	Ver.2.43�`
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
���O�F	mp3infp_Save
�T�v�F	�^�O����ۑ�����
�⑫�F	mp3infp_Load()�̌�ɌĂяo���Ă�������
�����F	IN const char *szFileName	�Ώۃt�@�C�������t���p�X�Ŏw��B
�ߒl�F	DWORD 
		-1=���s
		-2=2G�o�C�g�𒴂���t�@�C�����������Ƃ͂ł��܂���B(WAV�t�@�C���̂�)
		ERROR_SUCCESS=����
		(���̑�)=Win32�G���[�R�[�h (FormatMessage�ŕ�������擾�ł���)
�Ή��F	Ver.2.43�`
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
���O�F	mp3infp_mp3_MakeId3v1
�T�v�F	ID3TAG V1���쐬����
�⑫�F	mp3infp_Load()�̌�ɌĂяo���Ă�������
		�ύX�͒����ɔ��f����܂�
		mp3�t�@�C���ɂ̂ݗ��p���Ă�������(wav�t�@�C���͑ΏۊO)
�����F	IN char *szFileName	�Ώۃt�@�C�������t���p�X�Ŏw��B
�ߒl�F	DWORD 
		-1=���s
		ERROR_SUCCESS=����
		(���̑�)=Win32�G���[�R�[�h (FormatMessage�ŕ�������擾�ł���)
�Ή��F	Ver.2.43�`
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
���O�F	mp3infp_mp3_DelId3v1
�T�v�F	ID3TAG V1���폜����
�⑫�F	mp3infp_Load()�̌�ɌĂяo���Ă�������
		�ύX�͒����ɔ��f����܂�
		mp3�t�@�C���ɂ̂ݗ��p���Ă�������(wav�t�@�C���͑ΏۊO)
�����F	IN char *szFileName	�Ώۃt�@�C�������t���p�X�Ŏw��B
�ߒl�F	DWORD 
		-1=���s
		ERROR_SUCCESS=����
		(���̑�)=Win32�G���[�R�[�h (FormatMessage�ŕ�������擾�ł���)
�Ή��F	Ver.2.43�`
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
���O�F	mp3infp_mp3_MakeId3v2
�T�v�F	ID3TAG V2���쐬����
�⑫�F	mp3infp_Load()�̌�ɌĂяo���Ă�������
		�ύX�͒����ɔ��f����܂�
		mp3�t�@�C���ɂ̂ݗ��p���Ă�������(wav�t�@�C���͑ΏۊO)
�����F	IN char *szFileName	�Ώۃt�@�C�������t���p�X�Ŏw��B
�ߒl�F	DWORD 
		-1=���s
		ERROR_SUCCESS=����
		(���̑�)=Win32�G���[�R�[�h (FormatMessage�ŕ�������擾�ł���)
�Ή��F	Ver.2.43�`
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
���O�F	mp3infp_mp3_DelId3v2
�T�v�F	ID3TAG V2���폜����
�⑫�F	mp3infp_Load()�̌�ɌĂяo���Ă�������
		�ύX�͒����ɔ��f����܂�
		mp3�t�@�C���ɂ̂ݗ��p���Ă�������(wav�t�@�C���͑ΏۊO)
�����F	IN char *szFileName	�Ώۃt�@�C�������t���p�X�Ŏw��B
�ߒl�F	DWORD 
		-1=���s
		ERROR_SUCCESS=����
		(���̑�)=Win32�G���[�R�[�h (FormatMessage�ŕ�������擾�ł���)
�Ή��F	Ver.2.43�`
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
���O�F	mp3infp_mp3_MakeRMP
�T�v�F	mp3�`������RMP�`���ɕϊ�����
�⑫�F	mp3infp_Load()�̌�ɌĂяo���Ă�������
		�ύX�͒����ɔ��f����܂�
		mp3�t�@�C���ɂ̂ݗ��p���Ă�������(wav�t�@�C���͑ΏۊO)
�����F	IN char *szFileName	�Ώۃt�@�C�������t���p�X�Ŏw��B
�ߒl�F	DWORD 
		-1=���s
		ERROR_SUCCESS=����
		(���̑�)=Win32�G���[�R�[�h (FormatMessage�ŕ�������擾�ł���)
�Ή��F	Ver.2.43�`
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
���O�F	mp3infp_mp3_DelRMP
�T�v�F	RMP�`������mp3�`���ɕϊ�����
�⑫�F	mp3infp_Load()�̌�ɌĂяo���Ă�������
		�ύX�͒����ɔ��f����܂�
		mp3�t�@�C���ɂ̂ݗ��p���Ă�������(wav�t�@�C���͑ΏۊO)
�����F	IN char *szFileName	�Ώۃt�@�C�������t���p�X�Ŏw��B
�ߒl�F	DWORD 
		-1=���s
		ERROR_SUCCESS=����
		(���̑�)=Win32�G���[�R�[�h (FormatMessage�ŕ�������擾�ł���)
�Ή��F	Ver.2.43�`
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
���O�F	mp3infp_mp3_MakeApeTag
�T�v�F	APE Tag���쐬����
�⑫�F	mp3infp_Load()�̌�ɌĂяo���Ă�������
		�ύX�͒����ɔ��f����܂�
		mp3�t�@�C���ɂ̂ݗ��p���Ă�������(wav�t�@�C���͑ΏۊO)
�����F	IN char *szFileName	�Ώۃt�@�C�������t���p�X�Ŏw��B
�ߒl�F	DWORD 
		-1=���s
		ERROR_SUCCESS=����
		(���̑�)=Win32�G���[�R�[�h (FormatMessage�ŕ�������擾�ł���)
�Ή��F	Ver.2.47�`
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
���O�F	mp3infp_mp3_DelApeTag
�T�v�F	APE Tag���폜����
�⑫�F	mp3infp_Load()�̌�ɌĂяo���Ă�������
		�ύX�͒����ɔ��f����܂�
		mp3�t�@�C���ɂ̂ݗ��p���Ă�������(wav�t�@�C���͑ΏۊO)
�����F	IN char *szFileName	�Ώۃt�@�C�������t���p�X�Ŏw��B
�ߒl�F	DWORD 
		-1=���s
		ERROR_SUCCESS=����
		(���̑�)=Win32�G���[�R�[�h (FormatMessage�ŕ�������擾�ł���)
�Ή��F	Ver.2.47�`
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
