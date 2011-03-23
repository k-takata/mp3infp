@echo off

if "%2"=="" goto usage

copy /y %1\include\ogg\*.h	.\include\ogg
copy /y %1\src\*.c	.\lib\ogg
copy /y %1\COPYING	.\lib\ogg

copy /y %2\include\vorbis\*.h	.\include\vorbis
xcopy /y/s %2\lib	.\lib\vorbis
copy /y %2\COPYING	.\lib\vorbis

goto eof

:usage
echo update_oggvorbis ^<path_to_new_libogg^> ^<path_to_new_libvorbis^>

:eof
