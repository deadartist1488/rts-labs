@echo off
rem Запускает программу и печатает её код возврата (нужно для п.1.1 лаб.1)
%*
echo.
echo Код возврата: %errorlevel% (0x%=ExitCode%)
