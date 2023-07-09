/**
 *
 *  Copyright (C) 2016-2023 Roman Pauer
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

#if !defined(_XCOM_PROC_EVENTS_H_INCLUDED_)
#define _XCOM_PROC_EVENTS_H_INCLUDED_

extern void Game_ProcessKEvents();
extern int32_t Game_Device2PictureX(int32_t devicex);
extern int32_t Game_Device2PictureY(int32_t devicey);
extern int32_t Game_Picture2DeviceX(int32_t picturex);
extern int32_t Game_Picture2DeviceY(int32_t picturey);
extern void Game_GetGameMouse(int *mousex, int *mousey);
extern void Game_RepositionMouse(int mousex, int mousey);

#endif /* _XCOM_PROC_EVENTS_H_INCLUDED_ */
