/**
 *
 *  Copyright (C) 2021-2025 Roman Pauer
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

#include "SDI.h"
#include "SDImidi.h"
#include "Game-Config.h"
#include "Game-Display.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>


void X86_InitializeExceptions(void);


#ifdef __cplusplus
extern "C" {
#endif
extern int WinMain_asm(
  void *hInstance,
  void *hPrevInstance,
  char *lpCmdLine,
  int   nCmdShow
);
#ifdef __cplusplus
}
#endif


int CALLBACK WinMain(
  HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPSTR     lpCmdLine,
  int       nCmdShow
)
{
    int return_value;

    if (!SDI_LocatePaths())
    {
        MessageBoxA(NULL, "Unable to find game paths", "Error", MB_OK);
        return 0;
    }

    ReadConfiguration();

    midi_PluginStartup();

    X86_InitializeExceptions();

    SetResolution();

    return_value = WinMain_asm(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

    RestoreResolution();

    midi_PluginShutdown();

    return return_value;
}

