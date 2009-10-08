del /F /S /Q *.ncb
del /F /S /Q *.plg
del /F /S /Q *.opt
del /F /S /Q *.aps
del /F /S /Q *.clw
del /F /S /Q *.cache
del /F /S /Q *.suo
del /F /S /Q *.obj
del /F /S /Q *.pch
del /F /S /Q *.idb
del /F /S /Q *.sbr
del /F /S /Q *.pdb

rmdir /S /Q lib\mpeg4ip-1.4.1\lib\gnu\libmissing60___Win32_Debug
rmdir /S /Q lib\mpeg4ip-1.4.1\lib\gnu\Release

rmdir /S /Q lib\mpeg4ip-1.4.1\lib\mp4v2\Debug
rmdir /S /Q lib\mpeg4ip-1.4.1\lib\mp4v2\Release

rmdir /S /Q Debug
rmdir /S /Q Release

rmdir /S /Q mp3infp_regist\Debug
rmdir /S /Q mp3infp_regist\Release

rmdir /S /Q mp3infp_cpl\Debug
rmdir /S /Q mp3infp_cpl\Release

rmdir /S /Q mp3infp_res\Debug
rmdir /S /Q mp3infp_res\Release

rmdir /S /Q Installer\Debug
rmdir /S /Q Installer\Release

rmdir /S /Q lang_japanese\Debug
rmdir /S /Q lang_japanese\Release

rmdir /S /Q Installer\Output
del /F /S /Q Installer\*.exe
del /F /S /Q Installer\Japanese.chm
