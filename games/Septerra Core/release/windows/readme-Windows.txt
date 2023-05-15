Septerra Core for Windows (x86)
Version 1.04.0.8

Original Septerra Core version 1.04 is required for playing.
(version from GOG.com can be used for playing)

Installation
------------

Install game.
Put files from this archive into the installed game's directory.
Run SR-Septerra.exe in the installed game's directory.

Configuration
-------------

Configuration is stored in the file Septerra.cfg.


Misc
----

The game uses following libraries:
SDL2 (https://www.libsdl.org/)
mpg123 (https://www.mpg123.de/)
quicktime2 (http://libquicktime.sourceforge.net/)
dlfcn-win32 (https://github.com/dlfcn-win32/dlfcn-win32)
ffmpeg (https://www.ffmpeg.org/)
intl from gettext (https://www.gnu.org/software/gettext/)

Source code is available on GitHub: https://github.com/M-HT/SR

Using mouse polling rate higher than default 125Hz can result in mouse stuttering.
Setting CPU_SleepMode (in configuration file) to "reduced" can help. If not, then set CPU_SleepMode to "nosleep".


Changes
-------
v1.04.0.8 (2023-05-15)
* add option to use right mouse click to preselect characters in combat (based on unofficial patch)
* change mouse cursor position handling
* fix screen shake when loading new area
* don't add sleep (to prevent too much CPU utilization) when not necessary
* minor bug fixes

v1.04.0.7 (2022-01-08)
* add option to only use integer scaling

v1.04.0.6 (2021-11-04)
* add option to reduce cpu sleep, to help with higher mouse poll rates

v1.04.0.5 (2021-01-21)
* add scaling option without bilinear filtering
* add options to set command line parameters
* add option to set delay after image flip
* add options to enable cheats
* add options to switch WSAD and arrow keys

v1.04.0.4 (2020-04-13)
* fix playing .avi versions of movies from GOG Windows installer
* allow setting size of audio buffer

v1.04.0.3 (2020-03-30)
* fix audio in Windows Vista and higher

v1.04.0.2 (2019-10-11)
first Windows (x86) version
