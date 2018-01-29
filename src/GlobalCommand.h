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
//DLL�̃o�[�W���������擾
BOOL GetDLLVersion(IN LPTSTR szDLLFileName,
				   IN DWORD *pdwMajor,
				   IN DWORD *pdwMinor,
				   IN DWORD *pdwBuildNumber1,
				   IN DWORD *pdwBuildNumber2);

  //������Yen�̂Ƃ�TRUE
BOOL IsTailYenSign(LPCTSTR szStr);

//������'\\'��ǉ�(����'\\'�̂Ƃ��͉������Ȃ�)
void AddTailYenSign(LPTSTR szStr);
void AddTailYenSign(CString &Str);

//LoadString�̃o�O�΍�(MSKB Q140452)
CString LoadResString(HINSTANCE hInstance,UINT uID);

//---------------------------------------------------------------------
//���r�b�O�G���f�B�A��->���g���G���f�B�A���̕ϊ�
DWORD ExtractI4(unsigned char buf[4]);
WORD ExtractI2(unsigned char buf[2]);

CString getFileNameExtName(CString &path);
CString getExtName(CString &path);
CString getPathName(CString &path);
CString getFileName(CString &path);

//////////////////////////////////////////////////////////////////////////////
//getFileNameExtName
//�t���p�X������t�@�C�������擾����
//////////////////////////////////////////////////////////////////////////////
/*
�����F	char *szPath	�t�@�C�������܂ރp�X���ւ̃|�C���^

�߂�l�Fchar * �t�@�C�����ւ̃|�C���^
�@�@�@�@�@�@�@�t�@�C�������܂܂Ȃ��Ƃ���""�ւ̃|�C���^
	[\],[/],[:]��������Ȃ������ꍇ�A�������t�@�C�����Ƃ݂Ȃ��Ă��̂܂ܕԂ�
*/
LPCTSTR getFileNameExtName(LPCTSTR szPath);

//////////////////////////////////////////////////////////////////////////////
//getExtName
//�t���p�X������g���q���擾����
//////////////////////////////////////////////////////////////////////////////
/*
�����F	char *szPath	�g���q���܂ރp�X���ւ̃|�C���^

�߂�l�Fchar * �g���q�ւ̃|�C���^
�@�@�@�@�@�@�@�g���q���܂܂Ȃ��Ƃ���""�ւ̃|�C���^
*/
LPCTSTR getExtName(LPCTSTR szPath);

//�Q�o�C�g������̐؂�̂������������擾
long check2ByteLength(const char *szTag,long lLimit);

//�V�X�e���G���[���b�Z�[�W�{�b�N�X��\��
void sysError(HWND hWnd,LPTSTR mes = _T("Error"));
void errMessageBox(HWND hWnd,DWORD dwErrorCode,LPTSTR mes = _T("Error"));

//////////////////////////////////////////////////////////////////////////////
//cutFileName
//�p�X������t�@�C��������菜��
//////////////////////////////////////////////////////////////////////////////
/*
�����F	char *szPath	�t�@�C�������܂ރp�X���ւ̃|�C���^
			�i���s��A�p�X������t�@�C�������؂藣�����j
�߂�l�F�Ȃ�
*/
void cutFileName(LPTSTR szPath);

//////////////////////////////////////////////////////////////////////////////
//cutExtName
//�t�@�C��������g���q����菜��
//////////////////////////////////////////////////////////////////////////////
/*
�����F	char *szFileName	�t�@�C�����ւ̃|�C���^
			�i���s��A�t�@�C��������g���q���؂藣�����j
�߂�l�F�Ȃ�
*/
void cutExtName(LPTSTR szFileName);

//������src���w�蕶��c��n�������Ƃɋ�؂�
CString divString(LPTSTR src,char c,int n);

// �o�C�g���CString�ɕϊ�����
CString DataToCString(const char *data, int size, int code);
#define DTC_CODE_ANSI		0
#define DTC_CODE_UTF16LE	1
#define DTC_CODE_UTF16BE	2
#define DTC_CODE_UTF8		3

// TSTR���w��R�[�h�̃o�C�g��ɕϊ�����
int TstrToData(LPCTSTR tstr, int tlen, char *data, int dsize, int code);

// TSTR���w��R�[�h�̃o�C�g��ɕϊ�����i�������������蓖�āj
// �g�p���free()�ŉ�����邱��
char *TstrToDataAlloc(LPCTSTR tstr, int tlen, int *dsize, int code);


__int64 GetFileSize64(HANDLE hFile);
__int64 SetFilePointer64(HANDLE hFile,__int64 llDistanceToMove,DWORD dwMoveMethod);

#endif //_GLOBALCOMMAND_H
