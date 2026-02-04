;;
;;  Copyright (C) 2019-2026 Roman Pauer
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
    %define Patch_PreselectCharacters _Patch_PreselectCharacters
    %define Patch_IsPreselected _Patch_IsPreselected
    %define Patch_PreselectTime _Patch_PreselectTime
    %define Patch_IsBattle _Patch_IsBattle
%endif

extern ASI_startup_asm2c
extern ASI_shutdown_asm2c
extern ASI_stream_close_asm2c
extern ASI_stream_process_asm2c


extern InitializeQTML_asm2c
extern TerminateQTML_asm2c

extern CreatePortAssociation_asm2c
extern DestroyPortAssociation_asm2c

extern SetGWorld_asm2c
extern c2pstr_asm2c
extern PtrToHand_asm2c

extern FSMakeFSSpec_asm2c
extern QTSetDDPrimarySurface_asm2c
extern NativeEventToMacEvent_asm2c

extern EnterMovies_asm2c
extern ExitMovies_asm2c
extern StartMovie_asm2c
extern StopMovie_asm2c
extern IsMovieDone_asm2c
extern DisposeMovie_asm2c
extern OpenMovieFile_asm2c
extern CloseMovieFile_asm2c
extern NewMovieFromFile_asm2c
extern GetMovieBox_asm2c
extern NewMovieController_asm2c
extern DisposeMovieController_asm2c
extern QTRegisterAccessKey_asm2c
extern QTUnregisterAccessKey_asm2c
extern MCIsPlayerEvent_asm2c
extern MCDoAction_asm2c


extern memset_asm2c
extern memcpy_asm2c

extern _stricmp_asm2c
extern strncpy_asm2c
extern strncmp_asm2c
extern strncat_asm2c

extern malloc_asm2c
extern free_asm2c
extern calloc_asm2c
extern _alloca_probe_asm2c

extern atol_asm2c
extern toupper_asm2c

extern sprintf_asm2c
extern sscanf_asm2c

extern system_asm2c
extern srand_asm2c
extern rand_asm2c

extern _aulldiv_asm2c
extern _alldiv_asm2c
extern _time64_asm2c

extern _check_security_cookie_asm2c

extern _except_handler4_asm2c

extern _beginthread_asm2c

extern sync_asm2c

extern _ftol2_sse_asm2c
extern _ftol2_asm2c
extern _ftol_asm2c

extern _CIcos_asm2c
extern _CIsin_asm2c
extern _CIatan2_asm2c
extern _CIsqrt_asm2c
extern _CIfmod_asm2c
extern _CItan_asm2c
extern _CIpow_asm2c


extern OpenGameDataFiles_asm2c
extern CloseGameDataFiles_asm2c
extern RecordOpen_asm2c
extern RecordTryOpen_asm2c
extern RecordGetDataFilePathAndOffset_asm2c
extern RecordSeek_asm2c
extern RecordRead_asm2c
extern RecordClose_asm2c
extern RecordGetSize_asm2c
extern GetFirstLevelRecordKey_asm2c
extern GetNextLevelRecordKey_asm2c


extern Registry_SetValueDword_asm2c
extern Registry_GetValueDword_asm2c
extern Registry_GetValueString_asm2c


extern SoundEngine_StartTimer_asm2c
extern SoundEngine_StopTimer_asm2c
extern SoundEngine_DecodeMP3Stream_asm2c
extern SoundEngine_OpenMP3Stream_asm2c


extern Patch_PreselectCharacters
extern Patch_IsPreselected
extern Patch_PreselectTime
extern Patch_IsBattle
extern Patch_PushKeyEvent_asm2c


; DDRAW.dll
%define DirectDrawCreate DirectDrawCreate_asm2c

; WINMM.dll
%define timeGetTime   timeGetTime_asm2c

; DSOUND.dll
%define DirectSoundCreate DirectSoundCreate_asm2c

; KERNEL32.dll
%define FindNextFileA             FindNextFileA_asm2c
%define FindFirstFileA            FindFirstFileA_asm2c
%define ReadFile                  ReadFile_asm2c
%define SetFilePointer            SetFilePointer_asm2c
%define CreateDirectoryA          CreateDirectoryA_asm2c
%define GetPrivateProfileStringA  GetPrivateProfileStringA_asm2c
%define QueryPerformanceCounter   QueryPerformanceCounter_asm2c
%define QueryPerformanceFrequency QueryPerformanceFrequency_asm2c
%define InitializeCriticalSection InitializeCriticalSection_asm2c
%define DeleteCriticalSection     DeleteCriticalSection_asm2c
%define LeaveCriticalSection      LeaveCriticalSection_asm2c
%define EnterCriticalSection      EnterCriticalSection_asm2c
%define CreatePipe                CreatePipe_asm2c
%define SetThreadPriority         SetThreadPriority_asm2c
%define GetCurrentThread          GetCurrentThread_asm2c
%define SetPriorityClass          SetPriorityClass_asm2c
%define GetCurrentProcess         GetCurrentProcess_asm2c
%define FindClose                 FindClose_asm2c
%define FileTimeToLocalFileTime   FileTimeToLocalFileTime_asm2c
%define FileTimeToSystemTime      FileTimeToSystemTime_asm2c
%define CreateMutexA              CreateMutexA_asm2c
%define GetLastError              GetLastError_asm2c
%define SetErrorMode              SetErrorMode_asm2c
%define ExitProcess               ExitProcess_asm2c
%define Sleep                     Sleep_asm2c
%define GetFullPathNameA          GetFullPathNameA_asm2c
%define Beep                      Beep_asm2c
%define DeleteFileA               DeleteFileA_asm2c
%define CreateFileA               CreateFileA_asm2c
%define WriteFile                 WriteFile_asm2c
%define CloseHandle               CloseHandle_asm2c

; USER32.dll
%define DestroyWindow      DestroyWindow_asm2c
%define SetCursorPos       SetCursorPos_asm2c
%define GetAsyncKeyState   GetAsyncKeyState_asm2c
%define PostMessageA       PostMessageA_asm2c
%define GetMessageTime     GetMessageTime_asm2c
%define GetMessagePos      GetMessagePos_asm2c
%define OffsetRect         OffsetRect_asm2c
%define LoadImageA         LoadImageA_asm2c
%define GetSystemMetrics   GetSystemMetrics_asm2c
%define LoadIconA          LoadIconA_asm2c
%define LoadCursorA        LoadCursorA_asm2c
%define RegisterClassA     RegisterClassA_asm2c
%define AdjustWindowRectEx AdjustWindowRectEx_asm2c
%define CreateWindowExA    CreateWindowExA_asm2c
%define ShowWindow         ShowWindow_asm2c
%define UpdateWindow       UpdateWindow_asm2c
%define SetFocus           SetFocus_asm2c
%define PtInRect           PtInRect_asm2c
%define PeekMessageA       PeekMessageA_asm2c
%define GetMessageA        GetMessageA_asm2c
%define TranslateMessage   TranslateMessage_asm2c
%define DispatchMessageA   DispatchMessageA_asm2c
%define WaitMessage        WaitMessage_asm2c
%define SetCursor          SetCursor_asm2c
%define PostQuitMessage    PostQuitMessage_asm2c
%define ValidateRect       ValidateRect_asm2c
%define GetKeyState        GetKeyState_asm2c
%define GetCursorPos       GetCursorPos_asm2c
%define ClipCursor         ClipCursor_asm2c
%define DefWindowProcA     DefWindowProcA_asm2c
%define ShowCursor         ShowCursor_asm2c
%define MessageBoxA        MessageBoxA_asm2c

; GDI32.dll
%define DeleteDC            DeleteDC_asm2c
%define CreateCompatibleDC  CreateCompatibleDC_asm2c
%define BitBlt              BitBlt_asm2c
%define GetStockObject      GetStockObject_asm2c
%define CreatePolygonRgn    CreatePolygonRgn_asm2c
%define OffsetRgn           OffsetRgn_asm2c
%define FillRgn             FillRgn_asm2c
%define CreateSolidBrush    CreateSolidBrush_asm2c
%define CreateFontIndirectA CreateFontIndirectA_asm2c
%define SelectObject        SelectObject_asm2c
%define SetTextColor        SetTextColor_asm2c
%define SetBkMode           SetBkMode_asm2c
%define TextOutA            TextOutA_asm2c
%define DeleteObject        DeleteObject_asm2c

