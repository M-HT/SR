@echo off
cd /D "%~dp0"
goto Intro

:Albion
SR-Main.exe
IF %ERRORLEVEL% GTR 3 goto Exit
IF %ERRORLEVEL% GTR 2 goto Credits
IF %ERRORLEVEL% GTR 1 goto Intro
goto Exit

:Intro
IF EXIST ALBIONCD\VIDEO\INTRO.SMK (smk_play.exe ALBIONCD\VIDEO\INTRO.SMK)
goto Albion

:Credits
IF EXIST ALBIONCD\VIDEO\CREDITS.SMK (smk_play.exe ALBIONCD\VIDEO\CREDITS.SMK)
goto Albion

:Exit
