@echo off
del /AH /F /S /Q *.suo
del /F /S /Q *.ncb *.user

rmdir /S /Q lib\mpeg4ip-1.4.1\lib\gnu\Win32
rmdir /S /Q lib\mpeg4ip-1.4.1\lib\gnu\x64
rmdir /S /Q lib\mpeg4ip-1.4.1\lib\mp4v2\Win32
rmdir /S /Q lib\mpeg4ip-1.4.1\lib\mp4v2\x64

rmdir /S /Q Win32
rmdir /S /Q x64\Release

rem rmdir /S /Q x86
rem rmdir /S /Q x64

rmdir /S /Q mp3infp_regist\Win32
rmdir /S /Q mp3infp_regist\x64

rmdir /S /Q mp3infp_cpl\Win32
rmdir /S /Q mp3infp_cpl\x64

rmdir /S /Q lang_japanese\Win32
rmdir /S /Q lang_japanese\x64
