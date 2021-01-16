/**
 *
 *  Copyright (C) 2019-2021 Roman Pauer
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy of
 *  this software and associated documentation files (the "Software"), to deal in
 *  the Software without restriction, including without limitation the rights to
 *  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 *  of the Software, and to permit persons to whom the Software is furnished to do
 *  so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 */

#if !defined(_GAME_CONFIG_H_INCLUDED_)
#define _GAME_CONFIG_H_INCLUDED_

#ifdef GAME_CONFIG_DEFINE_VARIABLES
#define GAME_CONFIG_STORAGE
#else
#define GAME_CONFIG_STORAGE extern
#endif

GAME_CONFIG_STORAGE int Intro_Play;             /* play intro movies ? */

GAME_CONFIG_STORAGE int Display_Mode;           /* display mode - 0 = window; 1 = desktop; 2 = fullscreen */
GAME_CONFIG_STORAGE int Display_VSync;          /* vsync enabled ? */
GAME_CONFIG_STORAGE int Display_Width;          /* horizontal size */
GAME_CONFIG_STORAGE int Display_Height;         /* vertical size */
GAME_CONFIG_STORAGE int Display_Resizable;      /* resizable window ? */
GAME_CONFIG_STORAGE int Display_ScalingQuality; /* scaling quality: 0 = nearest neighbour, 1 = bilinear */

GAME_CONFIG_STORAGE int Audio_BufferSize;       /* audio buffer size (in samples) */

void ReadConfiguration(void);

#endif

