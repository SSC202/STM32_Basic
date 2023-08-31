@echo off
ver | find "6.2" > nul
if %ERRORLEVEL% == 0 REG.EXE ADD "HKCU\SOFTWARE\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers" /v "%CD%\STM32 ST-LINK Utility.exe" /t REG_SZ /d "WIN7RTM" /f