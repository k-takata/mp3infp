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
//�Q�o�C�g������̐؂�̂������������擾
long check2ByteLength(const char *szTag,long lLimit);

//DLL�̃o�[�W���������擾
BOOL GetDLLVersion(IN LPCTSTR szDLLFileName,
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

//�V�X�e���G���[���b�Z�[�W�{�b�N�X��\��
void sysError(HWND hWnd,LPCTSTR mes = _T("Error"));
void errMessageBox(HWND hWnd,DWORD dwErrorCode,LPCTSTR mes = _T("Error"));

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
CString divString(LPCTSTR src,char c,int n);


// Functions for encoding conversion
enum DTC_CODE {
	DTC_CODE_ANSI = 0,
	DTC_CODE_UTF16LE = 1,
	DTC_CODE_UTF16BE = 2,
	DTC_CODE_UTF8 = 3
};

// �o�C�g���CString�ɕϊ�����
CString DataToCString(const char *data, int size, DTC_CODE code);

// TSTR���w��R�[�h�̃o�C�g��ɕϊ�����
int TstrToData(LPCTSTR tstr, int tlen, char *data, int dsize, DTC_CODE code);

// TSTR���w��R�[�h�̃o�C�g��ɕϊ�����i�������������蓖�āj
// �g�p���free()�ŉ�����邱��
// offset ���w�肷��ƁA���̃o�C�g�������]�v�Ƀ��������m�ۂ��A
// �擪���󂯂ăf�[�^����������
char *TstrToDataAlloc(LPCTSTR tstr, int tlen, int *dsize, DTC_CODE code, int offset = 0);

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

// �^�C���X�^���v�ۑ��E����
class CTimeStampSaver {
public:
	CTimeStampSaver() : m_bTimeStampPushed(FALSE) {};
	~CTimeStampSaver() {};
	BOOL Push(LPCTSTR szFile);
	BOOL Pop(LPCTSTR szFile = NULL);
	void Clear() {m_bTimeStampPushed = FALSE;};
protected:
	FILETIME	m_fileTime;	//�^�C���X�^���v�ۑ��p
	FILETIME	m_createTime;	//�^�C���X�^���v�ۑ��p
	BOOL		m_bTimeStampPushed;	//�^�C���X�^���v���ۑ�����Ă���
	CString		m_strFile;
};

#endif //_GLOBALCOMMAND_H
