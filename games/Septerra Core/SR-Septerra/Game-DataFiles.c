/**
 *
 *  Copyright (C) 2019-2020 Roman Pauer
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

#define _XOPEN_SOURCE 700
#include "Game-DataFiles.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <malloc.h>

#if (defined(__WIN32__) || defined(__WINDOWS__)) && !defined(_WIN32)
#define _WIN32
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define fopen_c fopen
#else
#include <pthread.h>
#include "CLIB.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif
extern const char *GetRecordName_asm(uint32_t RecordKey);
extern uint32_t MessageProc_asm(const char *MessageText, uint32_t MessageType, uint32_t MessageCode, uint32_t (*MessageProc)(const char *, uint32_t, uint32_t));
#ifdef __cplusplus
}
#endif


#define eprintf(...) fprintf(stderr,__VA_ARGS__)


typedef struct
{
    const char *Name;   // file name
    FILE *File;         // file handle
    uint32_t Offset;    // current offset in file
    uint32_t Flags;     // file flags - 1 = optional file
    char Path[256];     // file path
} DatabaseFileInfo;

typedef struct
{
    int32_t IndexNumber;    // number in index array
    uint32_t Offset;        // current offset in record
    int32_t NewOffset;      // new offset
    int32_t NewWhence;      // whence of new offset
} RecordInfo;

typedef struct
{
    uint32_t RecordKey;          // record ID
    uint32_t DatabaseNumber;        // number in file array
    uint32_t FileOffset;        // record offset in file
    uint32_t UncompressedSize;  // record uncompressed size
    uint8_t CompressionType;    // record compression type - 0 = uncompressed ???, 1 = compressed ???
    uint8_t ii_f4_1;
    uint8_t ii_f4_2;
    uint8_t ii_f4_3;
    uint32_t RecordSize;        // record size in file
    uint32_t ii_f6;
    uint32_t ii_f7;
} IndexInfo;


static uint8_t DecompressionBuffer[16384]; // byte_4B2544
static IndexInfo *CurrentIndex; // dword_4B6544
static uint32_t IsRecordTryOpen = 0; // dword_4B6548
static FILE *ManifestFILE; // dword_4B654C
static FILE *IndexFILE; // dword_4B6550
static uint8_t *DecompressionBufPtr; // dword_4B6554
static uint32_t DecompressionRemaining; // dword_4B6558
static uint32_t DecompressionBufSize; // dword_4B655C
static FILE *DecompressionFILE; // dword_4B6560

static char LocalDirectory[256]; // byte_4DFF40
static char *ManifestFileBuffer; // dword_4E0040
static DatabaseFileInfo DatabaseFiles[255]; // dword_4E0060
static uint32_t NumberOfRecords; // dword_4F0F50
static uint32_t (*MessageProc)(const char *MessageText, uint32_t MessageType, uint32_t MessageCode); // proc_4F0F54
static uint32_t NumberOfIndices; // dword_4F0F58
static char ErrorString[256]; // byte_4F0F60
static RecordInfo *RecordBuffer; // dword_4F1060
#ifdef _WIN32
static CRITICAL_SECTION CriticalSection; // loc_4F1080
#else
static pthread_mutex_t Mutex;
#endif
static uint32_t NumberOfDatabaseFiles; // dword_4F1098
static char SourcePath[256]; // byte_4F10A0
//static uint32_t dword_4F11A0;
static IndexInfo *IndexFileBuffer; // dword_4F11A4


static void CloseDatabaseFiles(void);
static int GetDataFilePath(const char *FileName, char *FilePath);
static void RequestFileOnCDPresence(FILE *f);
static void RequestCDPresence(void);
static void ExitWithMessage(const char *MessageText, uint32_t MessageCode);
static void MessageWithPossibleExit(const char *MessageText, uint32_t MessageCode);
static RecordInfo *AllocateRecordBuffer(unsigned int NumRecords);
static uint32_t LZO1X_DecompressRecord(uint8_t *TmpBuffer, uint32_t TmpBufferSize, uint32_t RecordSize, uint8_t *DstBuffer, FILE *DataFILE);
static uint32_t LZO1X_DecompressRead(void);
static int32_t LZO1X_DoDecompress(uint8_t *TmpBuffer, uint32_t TmpBufRemaining, uint8_t *DstBuffer, uint32_t *ReadSizePtr, uint32_t (*ReadProc)(void));


// sub_445460
void OpenGameDataFiles(const char *ManifestFilePath, const char *_SourcePath, uint32_t (*_MessageProc)(const char *, uint32_t, uint32_t))
{
    char FilePath[256];

#ifdef DEBUG_DATABASE
    eprintf("OpenGameFiles - ");
#endif

    strcpy(&(SourcePath[0]), _SourcePath);
    MessageProc = _MessageProc;
    memset(&(DatabaseFiles[0]), 0, 255 * sizeof(DatabaseFileInfo));
    //dword_4F11A0 = 0;

    // get local directory from manifest file path
    strcpy(LocalDirectory, ManifestFilePath);
    int len = strlen(LocalDirectory);
    char *findslash = &LocalDirectory[len];
    for ( ; len != 0; len-- )
    {
        if ( *findslash == '\\' ) break;
        findslash--;
    }
    *findslash = 0;

    // open manifest file
    ManifestFILE = (FILE *)fopen_c(ManifestFilePath, "rt");
    if ( ManifestFILE == NULL )
    {
        snprintf(ErrorString, 256, "Unable to open %s", ManifestFilePath);
        ExitWithMessage(ErrorString, 101);
    }

    fseek(ManifestFILE, 0, SEEK_END);
    long ManifestSize = ftell(ManifestFILE);
    fseek(ManifestFILE, 0, SEEK_SET);

    ManifestFileBuffer = (char *) calloc(ManifestSize + 1, 1);
    if ( ManifestFileBuffer == NULL )
    {
        ExitWithMessage("Unable to allocate memory for manifest file buffer", 103);
    }
    fread(ManifestFileBuffer, 1, ManifestSize, ManifestFILE);

    // replace returns with zeros
    char *buf = ManifestFileBuffer;
    for ( ; ManifestSize != 0; ManifestSize-- )
    {
        if (*buf == '\n') *buf = 0;
        buf++;
    }

    // get index file path
    GetDataFilePath(&(ManifestFileBuffer[strlen(ManifestFileBuffer) + 1]), &(FilePath[0]));

    // open index file
    IndexFILE = (FILE *)fopen_c(&(FilePath[0]), "rb");
    if ( IndexFILE == NULL )
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wformat-truncation"
        snprintf(ErrorString, 256, "Unable to open %s", &(FilePath[0]));
#pragma GCC diagnostic pop
        ExitWithMessage(ErrorString, 101);
    }
    fseek(IndexFILE, 0, SEEK_END);
    long IndexSize = ftell(IndexFILE);
    fseek(IndexFILE, 0, SEEK_SET);

    IndexFileBuffer = (IndexInfo *) calloc(IndexSize, 1);
    if ( IndexFileBuffer == NULL )
    {
        ExitWithMessage("Unable to allocate memory for index file buffer", 103);
    }
    fread(IndexFileBuffer, 1, IndexSize, IndexFILE);

    NumberOfIndices = IndexSize >> 5;
    NumberOfRecords = 256;
    RecordBuffer = AllocateRecordBuffer(256);

    const char *FileName = &(ManifestFileBuffer[strlen(ManifestFileBuffer) + 1]);
    FileName = &(FileName[strlen(FileName) + 1]);
    NumberOfDatabaseFiles = 0;
    for ( ; *FileName != 0; NumberOfDatabaseFiles++, FileName = &(FileName[strlen(FileName) + 1]) )
    {
        DatabaseFiles[NumberOfDatabaseFiles].Name = FileName;
        DatabaseFiles[NumberOfDatabaseFiles].Flags = 0;

        // get data file path
        int DataFileRes = GetDataFilePath(FileName, &(FilePath[0]));
        if ( DataFileRes == -1 )
        {
            DatabaseFiles[NumberOfDatabaseFiles].Flags |= 1;
        }
        else
        {
            if ( DataFileRes > 1 )
            {
                ExitWithMessage("Invalid Manifest File Data", 101);
            }
        }
        strcpy(&(DatabaseFiles[NumberOfDatabaseFiles].Path[0]), &(FilePath[0]));

        DatabaseFiles[NumberOfDatabaseFiles].File = (FILE *)fopen_c(&(FilePath[0]), "rb");
        for ( ; DatabaseFiles[NumberOfDatabaseFiles].File == NULL; )
        {
            if (DatabaseFiles[NumberOfDatabaseFiles].Flags & 1) break;

            int fopenerrno = errno;
            if ((fopenerrno != ENOENT) && (fopenerrno != EACCES))
            {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wformat-truncation"
                snprintf(ErrorString, 256, "Unable to open %s\n(error %d: %s)", &(FilePath[0]), fopenerrno, strerror(fopenerrno));
#pragma GCC diagnostic pop
                ExitWithMessage(ErrorString, 101);
            }

            RequestCDPresence();
            DatabaseFiles[NumberOfDatabaseFiles].File = (FILE *)fopen_c(&(FilePath[0]), "rb");
        }

        DatabaseFiles[NumberOfDatabaseFiles].Offset = 0;
    }

#ifdef _WIN32
    InitializeCriticalSection(&CriticalSection);
#else
    pthread_mutexattr_t attr;
    if (0 != pthread_mutexattr_init(&attr))
    {
        ExitWithMessage("Unable to initialize mutex", 103);
    }
    if (0 != pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE))
    {
        pthread_mutexattr_destroy(&attr);
        ExitWithMessage("Unable to initialize mutex", 103);
    }
    if (0 != pthread_mutex_init(&Mutex, &attr))
    {
        pthread_mutexattr_destroy(&attr);
        ExitWithMessage("Unable to initialize mutex", 103);
    }
    pthread_mutexattr_destroy(&attr);
#endif

#ifdef DEBUG_DATABASE
    eprintf("OK\n");
#endif
}

// sub_445880
void CloseGameDataFiles(void)
{
#ifdef DEBUG_DATABASE
    eprintf("CloseGameFiles - ");
#endif

    CloseDatabaseFiles();

    fclose(ManifestFILE);
    ManifestFILE = NULL;

    free(ManifestFileBuffer);
    ManifestFileBuffer = NULL;

    fclose(IndexFILE);
    IndexFILE = NULL;

    free(IndexFileBuffer);
    IndexFileBuffer = NULL;

    free(RecordBuffer);
    RecordBuffer = NULL;

    NumberOfDatabaseFiles = 0;

#ifdef _WIN32
    DeleteCriticalSection(&CriticalSection);
#else
    pthread_mutex_destroy(&Mutex);
#endif

#ifdef DEBUG_DATABASE
    eprintf("OK\n");
#endif
}

// sub_445900
int32_t RecordOpen(uint32_t RecordKey)
{
#ifdef DEBUG_DATABASE
    eprintf("RecordOpen - 0x%x - ", RecordKey);
#endif

    //try
#ifdef _WIN32
    EnterCriticalSection(&CriticalSection);
#else
    pthread_mutex_lock(&Mutex);
#endif

    int32_t IndexNumber;

    // binary search for RecordKey in IndexFileBuffer
    if (RecordKey == IndexFileBuffer[0].RecordKey)
    {
        IndexNumber = 0;
    }
    else if (RecordKey < IndexFileBuffer[0].RecordKey)
    {
        IndexNumber = -1;
    }
    else if (RecordKey > IndexFileBuffer[NumberOfIndices - 1].RecordKey)
    {
        IndexNumber = -1;
    }
    else
    {
        IndexNumber = NumberOfIndices - 1;

        unsigned int SearchSize = NumberOfIndices;

        while (RecordKey != IndexFileBuffer[IndexNumber].RecordKey)
        {
            if (RecordKey <= IndexFileBuffer[IndexNumber].RecordKey)
            {
                IndexNumber -= (SearchSize >> 1);
            }
            else
            {
                IndexNumber += (SearchSize >> 1);
            }

            if (SearchSize == 1)
            {
                IndexNumber = -1;
                break;
            }

            SearchSize -= (SearchSize >> 1);
        }
    }
    if (IndexNumber == -1)
    {
        if ( !IsRecordTryOpen )
        {
            snprintf(ErrorString, 256, "Attempt to open invalid database record %d (%s)", RecordKey, GetRecordName_asm(RecordKey));
            ExitWithMessage(ErrorString, 101);
        }

#ifdef _WIN32
        LeaveCriticalSection(&CriticalSection);
#else
        pthread_mutex_unlock(&Mutex);
#endif
#ifdef DEBUG_DATABASE
        eprintf("%i\n", -1);
#endif
        return -1;
    }

    uint32_t result = 0;
    uint32_t RecordHandle;
    for (RecordHandle = 1; RecordHandle < NumberOfRecords; RecordHandle++)
    {
        if (RecordBuffer[RecordHandle].IndexNumber == -1)
        {
            result = RecordHandle;
            break;
        }
    }

    if (result == 0)
    {
        // allocate larger RecordBuffer
        RecordInfo *NewRecordBuffer = AllocateRecordBuffer(NumberOfRecords + 256);
        for (RecordHandle = 0; RecordHandle < NumberOfRecords; RecordHandle++)
        {
            NewRecordBuffer[RecordHandle] = RecordBuffer[RecordHandle];
        }

        free(RecordBuffer);
        result = NumberOfRecords;
        NumberOfRecords += 256;
        RecordBuffer = NewRecordBuffer;
    }

    RecordBuffer[result].IndexNumber = IndexNumber;
    RecordBuffer[result].Offset = 0;
    RecordBuffer[result].NewOffset = -1;
    RecordBuffer[result].NewWhence = -1;

    // finally
#ifdef _WIN32
    LeaveCriticalSection(&CriticalSection);
#else
    pthread_mutex_unlock(&Mutex);
#endif

#ifdef DEBUG_DATABASE
    eprintf("%i\n", result);
#endif
    return result;
}

// sub_445ae0
int32_t RecordTryOpen(uint32_t RecordKey)
{
#ifdef DEBUG_DATABASE
    eprintf("RecordTryOpen - 0x%x - ", RecordKey);
#endif
    // try
#ifdef _WIN32
    EnterCriticalSection(&CriticalSection);
#else
    pthread_mutex_lock(&Mutex);
#endif

    IsRecordTryOpen = 1;
    int32_t RecordHandle = RecordOpen(RecordKey);
    IsRecordTryOpen = 0;

    // finally
#ifdef _WIN32
    LeaveCriticalSection(&CriticalSection);
#else
    pthread_mutex_unlock(&Mutex);
#endif

#ifdef DEBUG_DATABASE
    eprintf(" - %i\n", RecordHandle);
#endif
    return RecordHandle;
}

// sub_445b60
const char *RecordGetDataFilePathAndOffset(uint32_t RecordKey, uint32_t *Offset)
{
#ifdef DEBUG_DATABASE
    eprintf("RecordGetDataFilePathAndOffset - 0x%x - 0x%x - ", RecordKey, (int)Offset);
#endif

    *Offset = 0;
    int32_t RecordHandle = RecordTryOpen(RecordKey);
    if ( RecordHandle == -1 )
    {
#ifdef DEBUG_DATABASE
        eprintf("NULL\n");
#endif
        return NULL;
    }
    else
    {
        IndexInfo *Index = &(IndexFileBuffer[RecordBuffer[RecordHandle].IndexNumber]);
        uint32_t DatabaseNumber = Index->DatabaseNumber;
        *Offset = Index->FileOffset;
#ifdef DEBUG_DATABASE
        eprintf("(offset = %i) - ", Index->FileOffset);
#endif

        if (DatabaseFiles[DatabaseNumber].File == NULL)
        {
            RecordClose(RecordHandle);
#ifdef DEBUG_DATABASE
            eprintf("NULL\n");
#endif
            return NULL;
        }
        else
        {
            RecordClose(RecordHandle);
#ifdef DEBUG_DATABASE
            eprintf("%s\n", DatabaseFiles[DatabaseNumber].Path);
#endif
            return &(DatabaseFiles[DatabaseNumber].Path[0]);
        }
    }
}

//sub_445be0
void RecordSeek(int32_t RecordHandle, int32_t Offset, int32_t Whence)
{
#ifdef DEBUG_DATABASE
    eprintf("RecordSeek - %i - %i - %i - ", RecordHandle, Offset, Whence);
#endif

    // try
    RecordInfo *Record = &(RecordBuffer[RecordHandle]);

#ifdef _WIN32
    EnterCriticalSection(&CriticalSection);
#else
    pthread_mutex_lock(&Mutex);
#endif
    if ( Record->IndexNumber == -1 )
    {
        snprintf(ErrorString, 256, "Attempt to seek in unopened RecordHandle %d", RecordHandle);
        ExitWithMessage(ErrorString, 101);
    }
    Record->NewOffset = Offset;
    Record->NewWhence = Whence;

    // finally
#ifdef _WIN32
    LeaveCriticalSection(&CriticalSection);
#else
    pthread_mutex_unlock(&Mutex);
#endif

#ifdef DEBUG_DATABASE
    eprintf("OK\n");
#endif
}

// sub_445c80
uint32_t RecordRead(int32_t RecordHandle, uint8_t *ReadBuffer, uint32_t NumberOfBytes)
{
#ifdef DEBUG_DATABASE
    eprintf("RecordRead - %i - 0x%x - %i - ", RecordHandle, (int)ReadBuffer, NumberOfBytes);
#endif

    // try
    RecordInfo *Record = &(RecordBuffer[RecordHandle]);
    uint32_t result = 0;

#ifdef _WIN32
    EnterCriticalSection(&CriticalSection);
#else
    pthread_mutex_lock(&Mutex);
#endif

    if (Record->IndexNumber == -1)
    {
        snprintf(ErrorString, 256, "Attempt to read in unopened RecordHandle %d", RecordHandle);
        ExitWithMessage(ErrorString, 101);
    }

    IndexInfo *Index = &(IndexFileBuffer[Record->IndexNumber]);
    uint32_t DatabaseNumber = Index->DatabaseNumber;
    FILE *DataFILE = DatabaseFiles[DatabaseNumber].File;

    uint32_t FileOffset = Record->Offset;
    switch (Record->NewWhence)
    {
        case 0:
            FileOffset = Record->NewOffset;
            break;
        case 1:
            FileOffset += Record->NewOffset;
            break;
        case 2:
            FileOffset = Index->RecordSize + Record->NewOffset;
            break;
    }
    Record->NewWhence = -1;
    Record->Offset = FileOffset;
    FileOffset += Index->FileOffset;

    if (FileOffset != DatabaseFiles[DatabaseNumber].Offset)
    {
        while ( fseek(DataFILE, FileOffset - DatabaseFiles[DatabaseNumber].Offset, SEEK_CUR) )
        {
            RequestFileOnCDPresence(DataFILE);
        }
        DatabaseFiles[DatabaseNumber].Offset = FileOffset;
    }

    if ( Record->Offset < Index->RecordSize )
    {
#ifdef DEBUG_DATABASE
        eprintf("(Compression: %i) - ", Index->CompressionType);
#endif
        if (Index->CompressionType)
        {
            if (Index->CompressionType != 1)
            {
                snprintf(ErrorString, 256, "Unsupported compression type in database %s\n record key %d", DatabaseFiles[DatabaseNumber].Name, Index->RecordKey);
                ExitWithMessage(ErrorString, 101);
            }

            result = LZO1X_DecompressRecord(DecompressionBuffer, 16384, Index->RecordSize, ReadBuffer, DataFILE);
            if (result < Index->UncompressedSize)
            {
                snprintf(ErrorString, 256, "Error decompressing from database file %s", DatabaseFiles[DatabaseNumber].Name);
                ExitWithMessage(ErrorString, 101);
            }

            DatabaseFiles[DatabaseNumber].Offset += Index->RecordSize;
            Record->Offset += Index->RecordSize;
        }
        else
        {
            if (Record->Offset + NumberOfBytes > Index->RecordSize)
            {
                NumberOfBytes = Index->RecordSize - Record->Offset;
            }

            while (1)
            {
                result = fread(ReadBuffer, 1, NumberOfBytes, DataFILE);

                if ( !feof(DataFILE) ) break;

                snprintf(ErrorString, 256, "Error reading database file %s", &(DatabaseFiles[DatabaseNumber].Path[0]));
                MessageWithPossibleExit(ErrorString, 101);
            }

            DatabaseFiles[DatabaseNumber].Offset += result;
            Record->Offset += result;
        }
    }

    // finally
#ifdef _WIN32
    LeaveCriticalSection(&CriticalSection);
#else
    pthread_mutex_unlock(&Mutex);
#endif

#ifdef DEBUG_DATABASE
    eprintf("%i\n", result);
#endif

    return result;
}

// sub_445ec0
void RecordClose(int32_t RecordHandle)
{
#ifdef DEBUG_DATABASE
    eprintf("RecordClose - %i - ", RecordHandle);
#endif

    // try
    RecordInfo *Record = &(RecordBuffer[RecordHandle]);

#ifdef _WIN32
    EnterCriticalSection(&CriticalSection);
#else
    pthread_mutex_lock(&Mutex);
#endif

    Record->IndexNumber = -1;

    // finally
#ifdef _WIN32
    LeaveCriticalSection(&CriticalSection);
#else
    pthread_mutex_unlock(&Mutex);
#endif

#ifdef DEBUG_DATABASE
    eprintf("OK\n");
#endif
}

// sub_445f30
uint32_t RecordGetSize(int32_t RecordHandle)
{
#ifdef DEBUG_DATABASE
    eprintf("RecordGetSize - %i ", RecordHandle);
#endif

    // try
    RecordInfo *Record = &(RecordBuffer[RecordHandle]);
    uint32_t result = 0;

#ifdef _WIN32
    EnterCriticalSection(&CriticalSection);
#else
    pthread_mutex_lock(&Mutex);
#endif

    if ( Record->IndexNumber == -1 )
    {
        snprintf(ErrorString, 256, "Attempt to get the size of unopened RecordHandle %d", RecordHandle);
        ExitWithMessage(ErrorString, 101);
    }

    IndexInfo *Index = &(IndexFileBuffer[Record->IndexNumber]);
#ifdef DEBUG_DATABASE
    eprintf("(Compression: %i) - ", Index->CompressionType);
#endif
    if (Index->CompressionType == 1)
    {
        result = 3;
    }

    result += Index->UncompressedSize;

    // finally
#ifdef _WIN32
    LeaveCriticalSection(&CriticalSection);
#else
    pthread_mutex_unlock(&Mutex);
#endif

#ifdef DEBUG_DATABASE
    eprintf("%i\n", result);
#endif
    return result;
}

// sub_445ff0
static void CloseDatabaseFiles(void)
{
    uint32_t DatabaseNumber;

    for (DatabaseNumber = 0; DatabaseNumber < NumberOfDatabaseFiles; DatabaseNumber++)
    {
        if (DatabaseFiles[DatabaseNumber].File != NULL)
        {
            fclose(DatabaseFiles[DatabaseNumber].File);
            DatabaseFiles[DatabaseNumber].File = NULL;
            DatabaseFiles[DatabaseNumber].Offset = 0;
        }
    }
}

// sub_446040
static int GetDataFilePath(const char *FileName, char *FilePath)
{
#if !defined(_WIN32)
    size_t len;
#endif

    if ( 0 == strncasecmp(FileName, "[OPTION]", 8) )
    {
        strcpy(FilePath, LocalDirectory);
        strcat(FilePath, &(FileName[8]));
#if !defined(_WIN32)
        len = strlen(FilePath);
        if ((len > 0) && (FilePath[len - 1] == '\r'))
        {
            FilePath[len - 1] = 0;
        }
#endif
        return -1;
    }

    if ( 0 == strncasecmp(FileName, "[LOCAL]", 7) )
    {
        strcpy(FilePath, LocalDirectory);
        strcat(FilePath, &(FileName[7]));
#if !defined(_WIN32)
        len = strlen(FilePath);
        if ((len > 0) && (FilePath[len - 1] == '\r'))
        {
            FilePath[len - 1] = 0;
        }
#endif
        return 0;
    }

    strcpy(FilePath, SourcePath);
    strcat(FilePath, &(FileName[5]));
#if !defined(_WIN32)
    len = strlen(FilePath);
    if ((len > 0) && (FilePath[len - 1] == '\r'))
    {
        FilePath[len - 1] = 0;
    }
#endif

    return FileName[3] - '0';
}

// sub_446150
static void RequestFileOnCDPresence(FILE *f)
{
    int err;

    err = fseek(f, 0, SEEK_CUR);
    for ( ; err != 0; err = fseek(f, 0, SEEK_CUR) )
    {
        RequestCDPresence();
    }
}

// sub_446180
static void RequestCDPresence(void)
{
    MessageWithPossibleExit("Please insure that the CD is in the drive", 107);
}

// sub_446190
static void ExitWithMessage(const char *MessageText, uint32_t MessageCode)
{
    MessageProc_asm(MessageText, 0, MessageCode, MessageProc); // 0 = MB_OK | MB_ICONERROR
    exit(-1);
}

// sub_4461b0
static void MessageWithPossibleExit(const char *MessageText, uint32_t MessageCode)
{
    if ( 3 == MessageProc_asm(MessageText, 2, MessageCode, MessageProc) ) // 3 = CANCEL; 2 = MB_RETRYCANCEL | MB_ICONWARNING
    {
        exit(-2);
    }
}

//sub_4461e0
uint32_t GetFirstLevelRecordKey(void)
{
#ifdef DEBUG_DATABASE
    eprintf("GetFirstLevelRecordKey - ");
#endif

    CurrentIndex = IndexFileBuffer;
    return GetNextLevelRecordKey();
}

//sub_4461f0
uint32_t GetNextLevelRecordKey(void)
{
#ifdef DEBUG_DATABASE
    eprintf("GetNextLevelRecordKey - ");
#endif

    uint32_t RecordKey = CurrentIndex->RecordKey;

    if (RecordKey & 0x1000000)
    {
#ifdef DEBUG_DATABASE
        eprintf("%i\n", 0);
#endif
        return 0;
    }
    else
    {
        CurrentIndex++;
#ifdef DEBUG_DATABASE
        eprintf("0x%x\n", RecordKey);
#endif
        return RecordKey;
    }
}

// sub_446210
static RecordInfo *AllocateRecordBuffer(unsigned int NumRecords)
{
    RecordInfo *res = (RecordInfo *) malloc(NumRecords * sizeof(RecordInfo));
    if (res == NULL)
    {
        ExitWithMessage("Unable to (re)allocate memory for record buffer", 103);
    }

    RecordInfo *item = res;
    for ( ; NumRecords != 0; NumRecords--)
    {
        item->IndexNumber = -1;
        item++;
    }

    return res;
}

// sub_446260
static uint32_t LZO1X_DecompressRecord(uint8_t *TmpBuffer, uint32_t TmpBufferSize, uint32_t RecordSize, uint8_t *DstBuffer, FILE *DataFILE)
{
    DecompressionBufSize = TmpBufferSize;
    DecompressionRemaining = RecordSize;
    uint32_t result = 0;
    DecompressionBufPtr = TmpBuffer;
    DecompressionFILE = DataFILE;
    uint32_t ReadBytes = LZO1X_DecompressRead();
    LZO1X_DoDecompress(TmpBuffer, ReadBytes, DstBuffer, &result, &LZO1X_DecompressRead);

    return result;
}

// sub_4462c0
static uint32_t LZO1X_DecompressRead(void)
{
    uint32_t ReadSize = DecompressionBufSize;

    if ( DecompressionBufSize > DecompressionRemaining )
    {
        ReadSize = DecompressionRemaining;
        DecompressionBufSize = DecompressionRemaining;
    }

    DecompressionRemaining -= ReadSize;

    if (ReadSize != 0)
    {
        ReadSize = fread(DecompressionBufPtr, 1, ReadSize, DecompressionFILE);
    }

    return ReadSize;
}

// LZO1X compression
// sub_446300
/*static int32_t LZO1X_DecompressBuffer(uint8_t *SrcBuffer, int32_t unused, uint8_t *DstBuffer, uint32_t *DecompressedSize)
{
    unsigned int NumCopy;
    uint8_t *Src2;
    uint8_t *Src3;
    unsigned int Value;


    *DecompressedSize = 0;

    uint8_t *DstPtr = DstBuffer;

    if (*SrcBuffer > 0x11)
    {
        NumCopy = *SrcBuffer - 0x11;
        SrcBuffer++;
        for (; NumCopy != 0; NumCopy--)
        {
            *DstPtr = *SrcBuffer;
            DstPtr++;
            SrcBuffer++;
        }

        goto loc_44639C;
    }

loc_446333:

    Value = *SrcBuffer;
    SrcBuffer++;

    if (Value >= 0x10) goto loc_4463EE;

    if (Value == 0)
    {
        for (; *SrcBuffer == 0; SrcBuffer++ )
        {
            Value += 255;
        }

        Value = Value + *SrcBuffer + 0x0f;
        SrcBuffer++;
    }

    for (NumCopy = Value + 3; NumCopy != 0; NumCopy--)
    {
        *DstPtr = *SrcBuffer;
        DstPtr++;
        SrcBuffer++;
    }

loc_44639C:

    Value = *SrcBuffer;
    SrcBuffer++;

    if (Value >= 0x10) goto loc_4463EE;

    Src2 = (((DstPtr - (4 * *SrcBuffer)) - (Value >> 2)) - 2049);
    SrcBuffer++;

    *DstPtr = *Src2;
    DstPtr++;
    Src2++;

loc_4463C9: // needs: Src2

    DstPtr[0] = Src2[0];
    DstPtr[1] = Src2[1];
    DstPtr += 2;

loc_4463D4:

    NumCopy = SrcBuffer[-2] & 3;

    if (NumCopy == 0) goto loc_446333;

    for (; NumCopy != 0; NumCopy--)
    {
        *DstPtr = *SrcBuffer;
        DstPtr++;
        SrcBuffer++;
    }

    Value = *SrcBuffer;
    SrcBuffer++;

loc_4463EE: // needs: Value

    if (Value < 0x40) goto loc_446426;

    Src3 = ((DstPtr - ((Value >> 2) & 7)) - (8 * *SrcBuffer)) - 1;
    SrcBuffer++;
    Value = (Value >> 5) - 1;

loc_44640E: // needs Value, Src3

    for (NumCopy = Value + 2; NumCopy != 0; NumCopy--)
    {
        *DstPtr = *Src3;
        DstPtr++;
        Src3++;
    }

    goto loc_4463D4;

loc_446426: // needs: Value

    if (Value < 0x20) goto loc_44645D;

    Value = Value & 0x1f;
    if (Value == 0)
    {
        for (; *SrcBuffer == 0; SrcBuffer++ )
        {
            Value += 255;
        }

        Value = Value + *SrcBuffer + 0x1f;
        SrcBuffer++;
    }

    Src3 = (DstPtr - ((SrcBuffer[0] + 256 * SrcBuffer[1]) >> 2)) - 1;
    SrcBuffer += 2;

    goto loc_4464A9;

loc_44645D: // needs: Value

    if (Value < 0x10) goto loc_4464F4;

    Src3 = DstPtr - ((Value & 8) << 11);

    Value = Value & 7;
    if (Value == 0)
    {
        for (; *SrcBuffer == 0; SrcBuffer++ )
        {
            Value += 255;
        }

        Value = Value + *SrcBuffer + 7;
        SrcBuffer++;
    }

    Src3 = Src3 - ((SrcBuffer[0] + 256 * SrcBuffer[1]) >> 2);
    SrcBuffer += 2;

    if (Src3 == DstPtr) goto loc_44650D;

    Src3 = Src3 - 0x4000;

loc_4464A9: // needs: Value, Src3

    if (Value < 6) goto loc_44640E;
    if (DstPtr < 4 + Src3) goto loc_44640E;

    for (NumCopy = Value + 2; NumCopy != 0; NumCopy--)
    {
        *DstPtr = *Src3;
        DstPtr++;
        Src3++;
    }

    goto loc_4463D4;

loc_4464F4: // needs: Value

    Src2 = ((DstPtr - (4 * *SrcBuffer)) - (Value >> 2)) - 1;
    SrcBuffer++;

    goto loc_4463C9;

loc_44650D:

    *DecompressedSize = DstPtr - DstBuffer;

    return 0;
}*/


// LZO1X compression
// sub_446520
static int32_t LZO1X_DoDecompress(uint8_t *TmpBuffer, uint32_t TmpBufRemaining, uint8_t *DstBuffer, uint32_t *DecompressedSize, uint32_t (*ReadProc)(void))
{
    unsigned int SrcVal2 = 0;
    unsigned int SrcVal1 = 0;
    unsigned int NumCopy;
    uint8_t *Src2;
    uint8_t *Src3;
    unsigned int Value;

#define READ_MORE_DATA { \
    Src1++; \
    if (Src1 >= TmpBufEnd) \
    { \
        SrcVal2 = Src1[-2]; \
        SrcVal1 = Src1[-1]; \
        TmpBufEnd = ReadProc() + TmpBuffer; \
        Src1 = TmpBuffer; \
    } \
}

    *DecompressedSize = 0;

    uint8_t *DstPtr = DstBuffer;
    uint8_t *TmpBufEnd = TmpBufRemaining + TmpBuffer;
    uint8_t *Src1 = TmpBuffer;

    if (*Src1 > 0x11)
    {
        NumCopy = *Src1 - 0x11;
        Src1++;
        for (; NumCopy != 0; NumCopy--)
        {
            *DstPtr = *Src1;
            DstPtr++;
            Src1++;
        }

        goto loc_44668A;
    }

loc_44656F:

    Value = *Src1;

    READ_MORE_DATA

    if (Value >= 0x10) goto loc_44679A;

    if (Value == 0)
    {
        while (*Src1 == 0)
        {
            Value += 255;

            READ_MORE_DATA
        }

        Value = Value + *Src1 + 0x0f;

        READ_MORE_DATA
    }

    for (NumCopy = Value + 3; NumCopy != 0; NumCopy--)
    {
        *DstPtr = *Src1;
        DstPtr++;

        READ_MORE_DATA
    }

loc_44668A:

    Value = *Src1;

    READ_MORE_DATA

    if (Value >= 0x10) goto loc_44679A;

    Src2 = ((DstPtr - (Value >> 2)) - (4 * *Src1)) - 2049;

    READ_MORE_DATA

    DstPtr[0] = Src2[0];
    DstPtr[1] = Src2[1];
    DstPtr[2] = Src2[2];
    DstPtr += 3;

loc_44671B:

    if (Src1 == TmpBuffer)
    {
        Value = SrcVal2;
    }
    else if (Src1 == (TmpBuffer+1))
    {
        Value = SrcVal1;
    }
    else
    {
        Value = Src1[-2];
    }

    NumCopy = Value & 3;

    if (NumCopy == 0) goto loc_44656F;

    for (; NumCopy != 0; NumCopy--)
    {
        *DstPtr = *Src1;
        DstPtr++;

        READ_MORE_DATA
    }

    Value = *Src1;

    READ_MORE_DATA

loc_44679A:

    if (Value < 0x40) goto loc_4467F9;

    Src3 = ((DstPtr - ((Value >> 2) & 7)) - (8 * *Src1)) - 1;

    READ_MORE_DATA

    Value = (Value >> 5) - 1;

loc_4467DA:

    for (NumCopy = Value+2; NumCopy != 0; NumCopy--)
    {
        *DstPtr = *Src3;
        DstPtr++;
        Src3++;
    }

    goto loc_44671B;

loc_4467F9:

    if (Value < 0x20) goto loc_4468F9;

    Value = Value & 0x1f;

    if (Value == 0)
    {
        while (*Src1 == 0)
        {
            Value += 255;

            READ_MORE_DATA
        }

        Value = Value + *Src1 + 0x1f;

        READ_MORE_DATA
    }

    Src3 = (DstPtr - (*Src1 >> 2)) - 1;

    READ_MORE_DATA

    Src3 = Src3 - (*Src1 << 6);

    READ_MORE_DATA

loc_4468A2:

    if (Value < 6) goto loc_4467DA;
    if (DstPtr < 4 + Src3) goto loc_4467DA;

    for (NumCopy = Value+2; NumCopy != 0; NumCopy--)
    {
        *DstPtr = *Src3;
        DstPtr++;
        Src3++;
    }

    goto loc_44671B;

loc_4468F9:

    if (Value < 0x10) goto loc_4469BE;

    Src3 = DstPtr - ((Value & 8) << 11);
    Value = Value & 7;
    if (Value == 0)
    {
        while (*Src1 == 0)
        {
            Value += 255;

            READ_MORE_DATA
        }

        Value = Value + *Src1 + 7;

        READ_MORE_DATA
    }

    Src3 = Src3 - (*Src1 >> 2);

    READ_MORE_DATA

    Src3 = Src3 - (*Src1 << 6);

    READ_MORE_DATA

    if (Src3 == DstPtr) goto loc_446A06;

    Src3 -= 0x4000;

    goto loc_4468A2;

loc_4469BE:

    Src3 = ((DstPtr - (Value >> 2)) - (4 * *Src1)) - 1;

    READ_MORE_DATA

    DstPtr[0] = Src3[0];
    DstPtr[1] = Src3[1];
    DstPtr += 2;

    goto loc_44671B;

loc_446A06:

    *DecompressedSize = DstPtr - DstBuffer;

    return 0;

#undef READ_MORE_DATA
}

