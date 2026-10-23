@echo off
chcp 65001 >nul
cd "%~dp0"

gcc "main.c" -o "output\main.exe"

echo test.txt | "output\main.exe"

pause