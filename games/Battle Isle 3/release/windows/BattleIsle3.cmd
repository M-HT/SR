@echo off
cd /D "%~dp0"

rem Uncomment and set SDI_INSTALL_PATH to contain path to the directory where the game is installed (in case this file is not inside the directory)
rem set SDI_INSTALL_PATH=?:\path\to\game\install\directory

rem Uncomment and set SDI_CD_PATH to contain path to the directory where the game CDs are copied (in case it's not in the DATA subdirectory of the directory where the game is installed)
rem set SDI_CD_PATH=?:\path\to\game\cd\directory

start SR-BI3.exe
