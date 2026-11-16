@echo off
rem Запускает программу 3 раза подряд (Enter на system("pause") подаётся автоматически)
for /L %%i in (1,1,3) do (echo.| %1 | findstr /v /c:"any key")
