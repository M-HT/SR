@echo off
cd /D "%~dp0"

rem Uncomment and set SDI_INSTALL_PATH to contain path to the directory where the game is installed (in case this file is not inside the directory)
rem set SDI_INSTALL_PATH=?:\path\to\game\install\directory

rem Uncomment and set SDI_CD_PATH to contain path to the directory where the game CDs are copied (in case it's not in the DATA subdirectory of the directory where the game is installed)
rem set SDI_CD_PATH=?:\path\to\game\cd\directory

rem Set SDI_LANG to change the language in game (text only)
rem 0 = german (deutsch)
rem 1 = english
rem 2 = french (français) - use only when your CD contains french resources (i.e. FRA directory)
set SDI_LANG=1

start SR-BI3.exe -%SDI_LANG%
