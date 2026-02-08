/**
 *
 *  Copyright (C) 2020-2026 Roman Pauer
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

#include "BBDBG.h"
#include "BBDSA.h"
#include "BBMEM.h"
#include <stdio.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>


typedef struct {
    const char *module_name;
    const char *function_name;
    uint8_t quiet_function;
} DBG_struct;


static int DBG_initialized = 0;
static int DBG_stackPos1 = -1;
static HANDLE DBG_hLogFile = INVALID_HANDLE_VALUE;
static int DBG_stackPos2 = -1;
static int DBG_newLine = 0;
static int DBG_level = 0;
static int DBG_writeToLogFile = 1;
static char DBG_printBuffer[300];
static unsigned int DBG_logBufferPosition;
static char DBG_moduleName[104];
static char DBG_logBuffer[104];
static DBG_struct DBG_stack[128];


static void DBG_Action(unsigned int type, const char *module_name, const char *function_name, int line, const char *msg);

static int DBG_OpenLogFile_c(void)
{
    DBG_hLogFile = CreateFileA("LOGFILE.TXT", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (DBG_hLogFile != INVALID_HANDLE_VALUE) return 1;

    MessageBoxA((HWND)DSAWIN_GetMainWindowHandle_c(), "COULD NOT CREATE DEBUG-OUTPUTFILE ( 1 ). This is no severe Error and you may proceed normally.", "INFORMATION", MB_OK | MB_ICONINFORMATION);
    return 0;
}

static void DBG_WriteToLogFile_c(const char *str)
{
    unsigned int index;
    DWORD NumberOfBytesWritten;

    if (DBG_level > 0) return;

    for (index = 0; str[index] != 0; index++)
    {
        DBG_logBuffer[DBG_logBufferPosition] = str[index];
        if ((DBG_logBufferPosition != 100) && (str[index] != '\n'))
        {
            DBG_logBufferPosition++;
        }
        else
        {
            if (DBG_hLogFile != INVALID_HANDLE_VALUE)
            {
                if ((DBG_logBuffer[DBG_logBufferPosition] == '\n') || (DBG_logBuffer[DBG_logBufferPosition] == '\r'))
                {
                    DBG_logBuffer[DBG_logBufferPosition] = '\r';
                    DBG_logBufferPosition++;
                    DBG_logBuffer[DBG_logBufferPosition] = '\n';
                }
                DBG_logBuffer[DBG_logBufferPosition + 1] = 0;
                WriteFile(DBG_hLogFile, DBG_logBuffer, DBG_logBufferPosition + 1, &NumberOfBytesWritten, NULL);
                FlushFileBuffers(DBG_hLogFile);
            }

            DBG_logBufferPosition = 0;
        }
    }
}

static void DBG_CloseOutputFile_c(void)
{
    if (DBG_hLogFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(DBG_hLogFile);
        DBG_hLogFile = INVALID_HANDLE_VALUE;
    }
}

void CCALL DBG_Panic_c(const char *module, int line)
{
    int closeLogFile;

    closeLogFile = 0;
    sprintf(DBG_printBuffer, "A severe error has occured!\nPlease refere to your manual for further information.\nPlease write down the following information:\nMODULE: %s\nLINE:%d\n", module, line);
    MessageBoxA((HWND)DSAWIN_GetMainWindowHandle_c(), DBG_printBuffer, "MAJOR ERROR", MB_OK);

    if (!DBG_writeToLogFile)
    {
        return;
    }

    DBG_writeToLogFile = 0;
    if (DBG_hLogFile == INVALID_HANDLE_VALUE)
    {
        if (!DBG_OpenLogFile_c())
        {
            return;
        }
        closeLogFile = 1;
    }

    MEM_Dump_c(DBG_hLogFile);

    if (DBG_hLogFile != INVALID_HANDLE_VALUE)
    {
        if (closeLogFile)
        {
            CloseHandle(DBG_hLogFile);
            DBG_hLogFile = INVALID_HANDLE_VALUE;
        }
    }
}

int CCALL DBG_Init_c(void)
{
    if (!DBG_initialized)
    {
        DBG_stackPos1 = -1;
        DBG_stackPos2 = -1;
        DBG_newLine = 0;
        DBG_logBufferPosition = 0;
        DBG_logBuffer[101] = 0;
        DBG_level = 0;
    }
    DBG_initialized++;
    DBG_Action(2, "BBSYSTEM", "MAIN", 0, "SYSTEMENTRY\n");
    return 1;
}

void CCALL DBG_Exit_c(void)
{
    DBG_initialized--;
    if (DBG_initialized == 0)
    {
        DBG_CloseOutputFile_c();
    }
}

static void DBG_PrintString_c(unsigned int type, const char *msg, int line)
{
    if (type == 0x100)
    {
        DBG_WriteToLogFile_c("\n");
        DBG_newLine = 1;
        return;
    }

    DBG_moduleName[0] = 0;

    if ( DBG_stackPos2 != DBG_stackPos1 )
    {
        if (DBG_stackPos1 >= 0)
        {
            if ((DBG_stackPos2 == -1) || 0 != strcmp(DBG_stack[DBG_stackPos2].module_name, DBG_stack[DBG_stackPos1].module_name))
            {
                sprintf(DBG_moduleName, " %s ", DBG_stack[DBG_stackPos1].module_name);
            }
        }

        switch (type)
        {
        case 2:
            if (DBG_moduleName[0] != 0)
            {
                sprintf(
                    DBG_printBuffer,
                    "  >>> DOWN TO  FUNCTION : %s  IN MODULE : %s  -----\n",
                    DBG_stack[DBG_stackPos1].function_name,
                    DBG_moduleName
                );
            }
            else
            {
                sprintf(
                    DBG_printBuffer,
                    "  >>> DOWN TO  FUNCTION : %s  -----\n",
                    DBG_stack[DBG_stackPos1].function_name
                );
            }
            break;

        case 4:
            if (DBG_moduleName[0] != 0)
            {
                sprintf(
                    DBG_printBuffer,
                    "  <<< RETURN TO FUNCTION: %s IN MODULE : %s\n      --- FROM LINE:  %ld  IN FUNCTION: %s  -----\n",
                    DBG_stack[DBG_stackPos1].function_name,
                    DBG_moduleName,
                    (long int)line,
                    DBG_stack[DBG_stackPos1 + 1].function_name
                );
            }
            else
            {
                sprintf(
                    DBG_printBuffer,
                    "  <<< RETURN TO FUNCTION: %s FROM LINE:  %ld  IN FUNCTION: %s  -----\n",
                    DBG_stack[DBG_stackPos1].function_name,
                    (long int)line,
                    DBG_stack[DBG_stackPos1 + 1].function_name
                );
            }
            break;

        case 8:
            sprintf(
                DBG_printBuffer,
                "  ### ERROR !! ### RETURN TO FUNCTION: %s FROM LINE:  %ld  IN FUNCTION: %s  -----\n",
                DBG_stack[DBG_stackPos1].function_name,
                (long int)line,
                DBG_stack[DBG_stackPos1 + 1].function_name
            );
            break;

        case 0x10:
            sprintf(DBG_printBuffer, "    --- LOOP - ENTRY \n");
            break;

        case 0x20:
            sprintf(DBG_printBuffer, "    --- LOOP - EXIT \n");
            break;

        case 0x40:
            sprintf(DBG_printBuffer, "    --- LOOP - BREAK \n");
            break;

        case 0x80:
            sprintf(DBG_printBuffer, "    --- BRANCH SWITCH\n");
            break;

        default:
            DBG_printBuffer[0] = 0;
            break;
        }

        DBG_WriteToLogFile_c(DBG_printBuffer);
        DBG_stackPos2 = DBG_stackPos1;
    }

    if (DBG_newLine)
    {
        if (msg != NULL)
        {
            if ((DBG_stackPos1 != -1) && DBG_stack[DBG_stackPos1].quiet_function)
            {
                sprintf(DBG_printBuffer, "        LINE : %06ld -- QUIETFUNCTION: %s -- MSG : ", (long int)line, DBG_stack[DBG_stackPos1].function_name);
            }
            else
            {
                sprintf(DBG_printBuffer, "        LINE : %06ld -- MSG : ", (long int)line);
            }

            DBG_WriteToLogFile_c(DBG_printBuffer);
            sprintf(DBG_printBuffer, "%s", msg);
        }
        else
        {
            DBG_printBuffer[0] = 0;
        }
    }
    else if (msg != NULL)
    {
        sprintf(DBG_printBuffer, "%s", msg);
    }
    else
    {
        DBG_printBuffer[0] = 0;
    }

    DBG_WriteToLogFile_c(DBG_printBuffer);
    DBG_newLine = (strrchr(DBG_printBuffer, '\n') != NULL)?1:0;

    switch (type)
    {
    case 2:
    case 4:
    case 8:
    case 0x10:
    case 0x20:
    case 0x40:
    case 0x80:
        if (!DBG_newLine)
        {
            sprintf(DBG_printBuffer, "\n");
            DBG_newLine = 1;
            DBG_WriteToLogFile_c(DBG_printBuffer);
        }
        break;

    default:
        break;
    }

    DBG_stackPos2 = DBG_stackPos1;
}

static void DBG_Action(unsigned int type, const char *module_name, const char *function_name, int line, const char *msg)
{
    uint8_t panic;
    uint8_t quiet_function;

    quiet_function = 0;
    panic = 0;

    if (type & 0x80000000)
    {
        type &= 0x7FFFFFFF;
        quiet_function = 1;
    }

    if (type & 0x40000000)
    {
        type &= 0xBFFFFFFF;
        panic = 1;
    }

    switch (type)
    {
    case 2u:
        if (DBG_stackPos1 >= 0x7f)
        {
            DBG_PrintString_c(type, "ERROR : DEBUG-STACK OVERFLOW!!!\n", line);
        }
        else
        {
            DBG_stackPos1++;
            DBG_stack[DBG_stackPos1].module_name = module_name;
            DBG_stack[DBG_stackPos1].function_name = function_name;
            DBG_stack[DBG_stackPos1].quiet_function = quiet_function;

            if ((DBG_stackPos1 != 0) && (!DBG_stack[DBG_stackPos1].quiet_function))
            {
                if (!DBG_newLine)
                {
                    DBG_PrintString_c(0x100, NULL, 0);
                }

                DBG_PrintString_c(type, msg, line);

                if (!DBG_newLine)
                {
                    DBG_PrintString_c(0x100, NULL, line);
                }
            }
        }

        DBG_writeToLogFile = 1;
        break;

    case 4u:
    case 8u:
        if ( DBG_stackPos1 < 0 )
        {
            DBG_PrintString_c(type, "ERROR : DEBUG-STACK UNDERFLOW!!!\n", line);
        }
        else
        {
            DBG_stackPos1--;

            if ((DBG_stackPos1 != -1) && (!DBG_stack[DBG_stackPos1 + 1].quiet_function))
            {
                if (!DBG_newLine)
                {
                    DBG_PrintString_c(0x100, NULL, 0);
                }

                DBG_PrintString_c(type, msg, line);

                if (!DBG_newLine)
                {
                    DBG_PrintString_c(0x100, NULL, line);
                }
            }
        }

        if (panic)
        {
            DBG_Panic_c(module_name, line);
        }
        break;

    case 0x10u:
    case 0x20u:
    case 0x40u:
    case 0x80u:
        DBG_PrintString_c(type, msg, line);
        // @fallthrough@
    case 0:
        DBG_PrintString_c(0x100, NULL, line);
        break;

    default:
        break;
    }
}

