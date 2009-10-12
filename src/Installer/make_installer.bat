@echo off

rem インストーラ作成

rem WDKでビルドしたファイルをVC2008と同じディレクトリに配置

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


rem インストーラ作成
"%ProgramFiles%\NSIS\makensis.exe" mp3infp_x64.nsi
"%ProgramFiles%\NSIS\makensis.exe" mp3infp_x86.nsi

