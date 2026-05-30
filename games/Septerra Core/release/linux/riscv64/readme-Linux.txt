Septerra Core for Linux (riscv64)
Version 1.04.0.13

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

The game requires following 64-bit libraries: SDL2, mpg123, zlib
On debian based distributions these libraries are in following packages: libsdl2-2.0-0 libmpg123-0, zlib1g

The game uses following libraries for playing video:
quicktime2 (http://libquicktime.sourceforge.net/)
ffmpeg (https://www.ffmpeg.org/)

The game optionally uses SpeexDSP library for audio resampling (https://github.com/xiph/speexdsp).

Source code is available on GitHub: https://github.com/M-HT/SR


Changes
-------

v1.04.0.13 (2026-05-30)
first Linux (riscv64) version
