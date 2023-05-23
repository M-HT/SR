;;
;;  Copyright (C) 2019-2023 Roman Pauer
;;
;;  Permission is hereby granted, free of charge, to any person obtaining a copy of
;;  this software and associated documentation files (the "Software"), to deal in
;;  the Software without restriction, including without limitation the rights to
;;  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
;;  of the Software, and to permit persons to whom the Software is furnished to do
;;  so, subject to the following conditions:
;;
;;  The above copyright notice and this permission notice shall be included in all
;;  copies or substantial portions of the Software.
;;
;;  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
;;  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
;;  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
;;  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
;;  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
;;  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
;;  SOFTWARE.
;;

%ifidn __OUTPUT_FORMAT__, win32
    %define Intro_Play _Intro_Play
    %define Outro_Play _Outro_Play
    %define Patch_IncreaseMaxWindowSize _Patch_IncreaseMaxWindowSize
%endif

extern ms_strlen_asm2c
extern ms_strcat_asm2c
extern ms_strcpy_asm2c
extern ms_atol_asm2c
extern ms_atoi_asm2c
extern ms_printf_asm2c
extern ms_strncpy_asm2c
extern ms_strcmp_asm2c
extern ms_memcpy_asm2c
extern ms__ftol_asm2c
extern ms_memset_asm2c
extern ms_memcmp_asm2c
extern ms_srand_asm2c
extern ms_rand_asm2c
extern ms__alloca_probe_asm2c
extern ms_strstr_asm2c
extern ms_isalnum_asm2c
extern ms__strnicmp_asm2c
extern ms__ltoa_asm2c

extern ms__except_handler3_asm2c

extern SDI_CheckTicksDelay_asm2c

extern midi_OpenSDIMusic_asm2c
extern midi_GetSDIMusicID_asm2c
extern midi_PlaySDIMusic_asm2c
extern midi_CloseSDIMusic_asm2c
extern midi_IsPlaying_asm2c
extern midi_OpenTestMusic_asm2c
extern midi_PlayTestMusic_asm2c
extern midi_CloseTestMusic_asm2c
extern midi_GetErrorString_asm2c

extern cmdline_ContainsOption_asm2c
extern cmdline_ReadLanguageOption_asm2c

extern video_RegisterClass_PRE_Video_asm2c
extern video_Open_PRE_Video_asm2c
extern video_Close_PRE_Video_asm2c
extern video_Play_PRE_Video_asm2c
extern video_RegisterClass_POST_Video_asm2c
extern video_Open_POST_Video_asm2c
extern video_Close_POST_Video_asm2c
extern video_Play_POST_Video_asm2c
extern video_RegisterClass_SS_Video_asm2c

extern sync_asm2c

extern Intro_Play
extern Outro_Play
extern Patch_IncreaseMaxWindowSize


; gdi32.dll
%define CreateFontIndirectA CreateFontIndirectA_asm2c
%define DeleteObject DeleteObject_asm2c
%define GdiFlush GdiFlush_asm2c
%define GetDeviceCaps GetDeviceCaps_asm2c
%define GetTextExtentPointA GetTextExtentPointA_asm2c
%define SelectObject SelectObject_asm2c
%define SetBkMode SetBkMode_asm2c
%define TextOutA TextOutA_asm2c

; kernel32.dll
%define CloseHandle CloseHandle_asm2c
%define CreateFileA CreateFileA_asm2c
%define FindClose FindClose_asm2c
%define FindFirstFileA FindFirstFileA_asm2c
%define FindNextFileA FindNextFileA_asm2c
%define GetCurrentDirectoryA GetCurrentDirectoryA_asm2c
%define GetPrivateProfileIntA GetPrivateProfileIntA_asm2c
%define GetPrivateProfileStringA GetPrivateProfileStringA_asm2c
%define GetTickCount GetTickCount_asm2c
%define GlobalMemoryStatus GlobalMemoryStatus_asm2c
%define ReadFile ReadFile_asm2c
%define SetCurrentDirectoryA SetCurrentDirectoryA_asm2c
%define SetFilePointer SetFilePointer_asm2c
%define WriteFile WriteFile_asm2c
%define WritePrivateProfileStringA WritePrivateProfileStringA_asm2c

; user32.dll
%define ClientToScreen ClientToScreen_asm2c
%define ClipCursor ClipCursor_asm2c
%define CreateWindowExA CreateWindowExA_asm2c
%define GetActiveWindow GetActiveWindow_asm2c
%define GetCursorPos GetCursorPos_asm2c
%define GetDC GetDC_asm2c
%define GetWindowRect GetWindowRect_asm2c
%define IsIconic IsIconic_asm2c
%define IsWindowVisible IsWindowVisible_asm2c
%define LoadCursorA LoadCursorA_asm2c
%define LoadStringA LoadStringA_asm2c
%define MessageBoxA MessageBoxA_asm2c
%define MoveWindow MoveWindow_asm2c
%define ReleaseCapture ReleaseCapture_asm2c
%define ReleaseDC ReleaseDC_asm2c
%define ScreenToClient ScreenToClient_asm2c
%define SendMessageA SendMessageA_asm2c
%define SetActiveWindow SetActiveWindow_asm2c
%define SetCursor SetCursor_asm2c
%define SetCursorPos SetCursorPos_asm2c
%define SetRect SetRect_asm2c
%define SetRectEmpty SetRectEmpty_asm2c
%define SetWindowPos SetWindowPos_asm2c
%define ShowCursor ShowCursor_asm2c
%define ShowWindow ShowWindow_asm2c
%define UnregisterClassA UnregisterClassA_asm2c
%define wsprintfA wsprintfA_asm2c

; winmm.dll
%define mciGetErrorStringA mciGetErrorStringA_asm2c
%define mciSendCommandA mciSendCommandA_asm2c

