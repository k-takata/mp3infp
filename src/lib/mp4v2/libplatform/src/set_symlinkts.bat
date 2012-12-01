@echo off

for %%i in (*.cpp) do (
	..\..\..\..\tools\symlinkts\symlinkts.exe %%i
)
