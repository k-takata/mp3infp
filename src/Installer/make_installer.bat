@echo off
setlocal

rem �C���X�g�[���쐬

rem �o�[�W������ ..\classID.h ����擾 (e.g. 2.54h/u4)
for /f tokens^=2^ delims^=^" %%i in ('find ^"SOFT_NAME^" ^< ..\classID.h') do set version=%%i
set version=%version:mp3infp Ver.=%
echo Version: %version%

rem �o�[�W��������
rem set /p version=Input mp3infp version ^(e.g. 2.54g/u7^): 

rem �t�@�C�����p�Ƀo�[�W���������񂩂�L�����폜�E�u��
set filever=%version:/=_%
set filever=%filever:.=%


rem WDK�Ńr���h�����t�@�C����VC2008�Ɠ����f�B���N�g���ɔz�u

if not exist ..\x86 mkdir ..\x86
if not exist ..\x64 mkdir ..\x64

copy ..\dll\objfre_w2k_x86\i386\mp3infp.dll     ..\x86 /y
copy ..\dll\objfre_wnet_amd64\amd64\mp3infp.dll ..\x64 /y

copy ..\lang_japanese\objfre_w2k_x86\i386\Japanese.lng     ..\x86 /y
copy ..\lang_japanese\objfre_wnet_amd64\amd64\Japanese.lng ..\x64 /y

copy ..\mp3infp_cpl\objfre_w2k_x86\i386\mp3infp.cpl     ..\x86 /y
copy ..\mp3infp_cpl\objfre_wnet_amd64\amd64\mp3infp.cpl ..\x64 /y

copy ..\mp3infp_regist\objfre_w2k_x86\i386\mp3infp_regist.exe     ..\x86 /y
copy ..\mp3infp_regist\objfre_wnet_amd64\amd64\mp3infp_regist.exe ..\x64 /y


rem �C���X�g�[���쐬
"%ProgramFiles%\NSIS\makensis.exe" "/DMUI_VERSION=%version%" "/XOutFile mp3infp%filever%.exe" mp3infp.nsi
