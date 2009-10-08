@echo off
cd src\x86
for /r %%f in (*regist.exe *.dll *.cpl *.lng) do verpatch "%%f"
pause
