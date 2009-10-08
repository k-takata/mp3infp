#ifndef _GLOBALCOMMAND_H
#define _GLOBALCOMMAND_H

#define sizeof_array(a) (sizeof(a)/sizeof(a[0]))

void mbsncpy2(unsigned char *dst,unsigned char *src,int c);
//DLL�̃o�[�W���������擾
BOOL GetDLLVersion(IN LPSTR szDLLFileName,
				   IN DWORD *pdwMajor,
				   IN DWORD *pdwMinor,
				   IN DWORD *pdwBuildNumber1,
				   IN DWORD *pdwBuildNumber2);

  //������Yen�̂Ƃ�TRUE
BOOL IsTailYenSign(char *szStr);

//������'\\'��ǉ�(����'\\'�̂Ƃ��͉������Ȃ�)
void AddTAilYenSigne(char *szStr);
void AddTAilYenSigne(CString &Str);

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
//getFileName
//�t���p�X������t�@�C�������擾����
//////////////////////////////////////////////////////////////////////////////
/*
�����F	char *szPath	�t�@�C�������܂ރp�X���ւ̃|�C���^

�߂�l�Fchar * �t�@�C�����ւ̃|�C���^
�@�@�@�@�@�@�@�t�@�C�������܂܂Ȃ��Ƃ���""�ւ̃|�C���^
	[\],[/],[:]��������Ȃ������ꍇ�A�������t�@�C�����Ƃ݂Ȃ��Ă��̂܂ܕԂ�
*/
const char *getFileName(const char *szPath);

//////////////////////////////////////////////////////////////////////////////
//getExtName
//�t���p�X������g���q���擾����
//////////////////////////////////////////////////////////////////////////////
/*
�����F	char *szPath	�g���q���܂ރp�X���ւ̃|�C���^

�߂�l�Fchar * �g���q�ւ̃|�C���^
�@�@�@�@�@�@�@�g���q���܂܂Ȃ��Ƃ���""�ւ̃|�C���^
*/
const char *getExtName(const char *szPath);

//�Q�o�C�g������̐؂�̂������������擾
long check2ByteLength(const char *szTag,long lLimit);

//�V�X�e���G���[���b�Z�[�W�{�b�N�X��\��
void sysError(HWND hWnd,char *mes = "Error");
void errMessageBox(HWND hWnd,DWORD dwErrorCode,char *mes = "Error");

//////////////////////////////////////////////////////////////////////////////
//cutFileName
//�p�X������t�@�C��������菜��
//////////////////////////////////////////////////////////////////////////////
/*
�����F	char *szPath	�t�@�C�������܂ރp�X���ւ̃|�C���^
			�i���s��A�p�X������t�@�C�������؂藣�����j
�߂�l�F�Ȃ�
*/
void cutFileName(char *szPath);

//////////////////////////////////////////////////////////////////////////////
//cutExtName
//�t�@�C��������g���q����菜��
//////////////////////////////////////////////////////////////////////////////
/*
�����F	char *szFileName	�t�@�C�����ւ̃|�C���^
			�i���s��A�t�@�C��������g���q���؂藣�����j
�߂�l�F�Ȃ�
*/
void cutExtName(char *szFileName);

//������src���w�蕶��c��n�������Ƃɋ�؂�
CString divString(char *src,char c,int n);

#endif //_GLOBALCOMMAND_H