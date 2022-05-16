@echo off
set make_old_working_directory=%cd%
cd %~dp0
.\sdk\bin\make.exe %*
cd %make_old_working_directory%
