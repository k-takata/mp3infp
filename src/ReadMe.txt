mp3infp/u �V�F���g���^�^�O�G�f�B�^ (mp3�C���t�s�[) (Unicode��)
Ver.2.55 beta3 Source code for Windows 7/8/10
Copyright(C) 1998-2007 T-Matsuo (win32lab.com)
Copyright(C) 2009 Rem
Copyright(C) 2009-2018 K.Takata

���r���h��(�ʓr���肵�Ă�������)
�EWindows Driver Kit 7.1.0 (7600.16385.1)
https://www.microsoft.com/en-us/download/details.aspx?id=11800
�EWindows Driver Kit 6001.18002 (Win2k �p�Ƀr���h����ꍇ)

��mp3infp/u (Unicode��) �̌��J�֐�
���܂ł� mp3infp_�` �Ƃ����֐��ɉ����Amp3infp_�`A�Amp3infp_�`W �Ƃ���
�֐����G�N�X�|�[�g���Ă��܂��B�imp3infp_unicode.def �Q�Ɓj

��)
  mp3infp_Load      �]���ʂ�� ANSI �Ŋ֐�
  mp3infp_LoadA     mp3infp_Load �Ɠ���
  mp3infp_LoadW     Unicode �Ŋ֐�

���r���h���@
�{�o�[�W�����́AWDK 7.1.0 ���g���ăr���h����Ă��܂��B�iWDK �ɂ� MFC ���܂܂�
�Ă��܂��B�j
x64 �r���h�̏ꍇ�A�X�^�[�g���j���[����A

  Windows Driver Kits
    WDK 7600.16385.1
      Build Environments
        Windows Server 2003  (���邢�� Windows 7)
          x64 Free Build Environment

��I������� x64 �����[�X�r���h�p�̊����J���܂��̂ŁA
mp3infp �� src �f�B���N�g���Ɉړ����āA�R�}���h���C������ build -g ��
���s����ƃr���h����܂��Bmp3infp.dll �� dll �f�B���N�g���̉���
�f�B���N�g�����쐬����A���̉��ɏo�͂���܂��B

x86 �r���h�̏ꍇ�A�X�^�[�g���j���[����ȉ���I�����Ă��������B

  Windows Driver Kits
    WDK 7600.16385.1
      Build Environments
        Windows XP
          x86 Free Build Environment

Win2k ���� x86 �r���h���K�v�ȏꍇ�AWDK 6001.18002 ���C���X�g�[��������ŁA
�X�^�[�g���j���[����

  Windows Driver Kits
    WDK 6001.18002
      Build Environments
        Windows 2000
          Open a build window for Windows 2000 x86 Free Build Environment

��I�����Ă��������B

�� WDK 7.0.0 �ȍ~�ł́AWin2k �p�̎��s�t�@�C���̓r���h�̓T�|�[�g����Ă��܂���B
   ���o�[�W������ WDK 6001.18002 ���K�v�ł��B
   �܂��AWDK 6001.18002 ���g�����ꍇ�ł��A�ꕔ�̊֐����g�p����ƁA
   Win2k �ł͎��s�ł��Ȃ��t�@�C�����ł��Ă��܂��܂��Bw2kstub.cpp ��
   ����𖳗����C�����邽�߂̃\�[�X�ł��i�ꕔ XP �p�j�B

�� WDK �Ɋ܂܂�Ă��� MFC �� 6.0 �ł��B�iVisual C++ 6.0 �Ɠ����j
   MFC 7.0 ����� CString ���g������AUnicode �������₷���Ȃ��Ă���
   �̂ł����A����͂����̋@�\�͎g�p�ł��Ȃ����߁A�\�[�X��������ɂ���
   �Ȃ��Ă��镔�������邩������܂���B

���g�p���C�u����
OggVorbis�`���̃t�@�C���ǂݏ����ɂ͉��L�I�[�v���\�[�X���C�u�������g�p���܂��B
 >THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2001
 >by the XIPHOPHORUS Company http://www.xiph.org/

mp4�̓ǂݏ����ɂ�mp4v2���C�u�������g�p���Ă��܂�
http://code.google.com/p/mp4v2/
���\�[�X���ꕔ���ς��Ă��܂�(���ωӏ��ɂ� "K.Takata" �̏������L�ڂ��Ă��܂�)

�����C�Z���X
���̃\�t�g�E�F�A��LGPL���C�Z���X�Ŕz�z���Ă��܂��B
�����ɕt���Ă͓Y�t��LGPL�����_��(lgpl.txt)���Q�Ƃ��Ă��������B

----------------------------------
mp3infp/u
https://github.com/k-takata/mp3infp

Win32�H�쏬��
http://win32lab.com/
