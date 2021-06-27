/**
 *
 *  Copyright (C) 2020-2021 Roman Pauer
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

#if !defined(_BBBLEV_H_INCLUDED_)
#define _BBBLEV_H_INCLUDED_

#include <stdint.h>

#define BBBLEV_TYPE_KEYDOWN 1
#define BBBLEV_TYPE_KEYUP 2
#define BBBLEV_TYPE_MOUSELEFTDOWN 4
#define BBBLEV_TYPE_MOUSELEFTUP 8
#define BBBLEV_TYPE_MOUSELEFTDOUBLECLICK 16
#define BBBLEV_TYPE_MOUSERIGHTDOWN 32
#define BBBLEV_TYPE_MOUSERIGHTUP 64
#define BBBLEV_TYPE_MOUSERIGHTDOUBLECLICK 128
#define BBBLEV_TYPE_MOUSEMOVE 256

#define BBBLEV_BUTTON_MOUSELEFT 1
#define BBBLEV_BUTTON_MOUSERIGHT 2
#define BBBLEV_BUTTON_SHIFT 8
#define BBBLEV_BUTTON_CONTROL 0x10
#define BBBLEV_BUTTON_ALT 0x20

#define BBBLEV_KEY_ESCAPE 0x100
#define BBBLEV_KEY_F1 0x101
#define BBBLEV_KEY_F2 0x102
#define BBBLEV_KEY_F3 0x103
#define BBBLEV_KEY_F4 0x104
#define BBBLEV_KEY_F5 0x105
#define BBBLEV_KEY_F6 0x106
#define BBBLEV_KEY_F7 0x107
#define BBBLEV_KEY_F8 0x108
#define BBBLEV_KEY_F9 0x109
#define BBBLEV_KEY_F10 0x10A
#define BBBLEV_KEY_F11 0x10B
#define BBBLEV_KEY_F12 0x10C
#define BBBLEV_KEY_TAB 0x10D
#define BBBLEV_KEY_INSERT 0x10E
#define BBBLEV_KEY_DELETE 0x10F
#define BBBLEV_KEY_BACKSPACE 0x110
#define BBBLEV_KEY_HOME 0x111
#define BBBLEV_KEY_END 0x112
#define BBBLEV_KEY_PAGEUP 0x113
#define BBBLEV_KEY_PAGEDOWN 0x114
#define BBBLEV_KEY_LEFT 0x115
#define BBBLEV_KEY_UP 0x116
#define BBBLEV_KEY_RIGHT 0x117
#define BBBLEV_KEY_DOWN 0x118
#define BBBLEV_KEY_ENTER 0x119


#pragma pack(1)

typedef struct __attribute__ ((__packed__)) _BLEV_Event {
    uint32_t type;
    void *screen;
    uint32_t buttonState;
    int32_t key;
    uint32_t x;
    uint32_t y;
} BLEV_Event;

#pragma pack()


#ifdef __cplusplus
extern "C" {
#endif

int BLEV_Init_c(void);
void BLEV_Exit_c(void);
void BLEV_ClearAllEvents_c(void);
int BLEV_PutEvent_c(const BLEV_Event *event);
int BLEV_GetEvent_c(BLEV_Event *event);
int BLEV_PeekEvent_c(BLEV_Event *event);

#ifdef __cplusplus
}
#endif

#endif /* _BBBLEV_H_INCLUDED_ */
