@echo off
cd /D "%~dp0"
SR-Intro.exe
SR-Geoscape.exe "0"
if errorlevel 2 goto begin
goto end
:begin
SR-Tactical.exe "1"
:geo
SR-Geoscape.exe "1"
if errorlevel 2 goto begin
:end
