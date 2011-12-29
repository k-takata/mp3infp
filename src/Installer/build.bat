@echo off
set ver=2.54h
set filever=%ver:.=%
"%ProgramFiles%\NSIS\makensis.exe" "/DMUI_VERSION=%ver%" "/XOutFile mp3infp%filever%.exe" mp3infp.nsi
pause
