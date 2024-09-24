Septerra Core for Linux (armv7-gnueabihf)
Version 1.04.0.12

Original Septerra Core version 1.04 is required for playing.
(version from GOG.com can be used for playing)

Installation
------------

Put files from this archive into the installed game's directory.
Run Septerra.sh in the installed game's directory.

Configuration
-------------

Configuration is stored in the file Septerra.cfg.


Controls
--------

The game can use gamepad or joystick to emulate mouse and keyboard, if it's enabled in the configuration file.

Controller mappings can be added using file gamecontrollerdb.txt.

Gamepad / joystick controls:
Analog stick/pad: mouse movement
Digital stick/pad: up/down/left/right arrow key
A/X / 1st joystick button: left mouse button
B/Y / 2nd joystick button: right mouse button
Start / 3rd joystick button: virtual keyboard
Back / 4th joystick button: tab key


Misc
----

The game requires following 32-bit libraries: SDL2, mpg123
On debian based distributions these libraries are in following packages: libsdl2-2.0-0 libmpg123-0

The game uses following libraries for playing video:
quicktime2 (http://libquicktime.sourceforge.net/)
ffmpeg (https://www.ffmpeg.org/)

The game optionally uses SpeexDSP library for audio resampling (https://github.com/xiph/speexdsp).

Source code is available on GitHub: https://github.com/M-HT/SR


Changes
-------

v1.04.0.12 (2024-09-24)
* fix problem with higher mouse polling rates
* add option to use higher quality audio resampling
* add option to select audio sampling rate
* optimize audio conversion and mixing
* add support for higher sampling rates
* add support for using gamepad or joystick as mouse/keyboard
* minor bug fixes

v1.04.0.11 (2023-11-14)
* use bundled quicktime2 and ffmpeg libraries instead of system libraries
* optimize floating point instructions
* minor bug fixes

v1.04.0.10 (2023-05-23)
* fix repositioning mouse cursor

v1.04.0.9 (2023-05-17)
* fix moving mouse cursor outside the game area in fullscreen

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
* optimize recompiled code

v1.04.0.5 (2021-05-21)
first Linux (armv7-gnueabihf) version
