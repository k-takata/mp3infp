@echo off
rem Create a tags file using Universal Ctags.
ctags -R --input-encoding=cp932 --languages=C,C++,WindRes
