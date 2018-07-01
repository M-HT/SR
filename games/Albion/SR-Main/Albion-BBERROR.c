/**
 *
 *  Copyright (C) 2018 Roman Pauer
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

#include "Albion-BBERROR.h"
#include <stdio.h>
#include <string.h>


typedef struct {
    ERROR_PrintDataProc PrintData;
    // todo: remove
    ERROR_PrintDataProc PrintDataDOS;
    const char *prefix;
    uint8_t data[16];
} ERROR_MessageStruct;

static ERROR_MessageStruct ERROR_messages[20];
static char ERROR_message_buffer[400];
static ERROR_LogPrintProc ERROR_LogPrint;

// todo: make static
extern uint16_t ERROR_num_messages;

// todo: remove
#ifdef __cplusplus
extern "C" {
#endif
extern uint32_t Game_RunProcReg1_Asm(void *proc_addr, const char *proc_param1);
extern uint32_t Game_RunProcReg2_Asm(void *proc_addr, const char *proc_param1, const uint8_t *proc_param2);
#ifdef __cplusplus
}
#endif


static void ERROR_SetLogprintProc(ERROR_LogPrintProc logprint_proc);
static void ERROR_PrintData(char *buffer, const uint8_t *data);

void ERROR_Init(ERROR_LogPrintProc logprint_proc)
{
    ERROR_ClearMessages();
    ERROR_SetLogprintProc(logprint_proc);
}

void ERROR_ClearMessages(void)
{
    ERROR_num_messages = 0;
}

static void ERROR_SetLogprintProc(ERROR_LogPrintProc logprint_proc)
{
    ERROR_LogPrint = logprint_proc;
}

int ERROR_AddMessage(ERROR_PrintDataProc msg_printdata_proc, const char *msg_prefix, int msg_data_len, const uint8_t *msg_data)
{
    int data_index;

    if (ERROR_num_messages == 20)
    {
        ERROR_num_messages--;
        ERROR_AddMessage(ERROR_PrintData, "BBERROR Library", strlen("STACK FULL") + 1, (const uint8_t *) "STACK FULL");

        return 1;
    }

    if (msg_data_len > 16)
    {
        ERROR_AddMessage(ERROR_PrintData, "BBERROR Library", strlen("TOO MUCH DATA") + 1, (const uint8_t *) "TOO MUCH DATA");

        return 2;
    }

    ERROR_messages[ERROR_num_messages].PrintData = msg_printdata_proc;
    ERROR_messages[ERROR_num_messages].PrintDataDOS = NULL;
    ERROR_messages[ERROR_num_messages].prefix = msg_prefix;
    for (data_index = 0; data_index < msg_data_len; data_index++)
    {
        ERROR_messages[ERROR_num_messages].data[data_index] = msg_data[data_index];
    }
    ERROR_num_messages++;

    return 0;
}

// todo: remove
int ERROR_AddMessageDOS(ERROR_PrintDataProc msg_printdata_proc, const char *msg_prefix, int msg_data_len, const uint8_t *msg_data)
{
    int data_index;

    if (ERROR_num_messages == 20)
    {
        ERROR_num_messages--;
        ERROR_AddMessage(ERROR_PrintData, "BBERROR Library", strlen("STACK FULL") + 1, (const uint8_t *) "STACK FULL");

        return 1;
    }

    if (msg_data_len > 16)
    {
        ERROR_AddMessage(ERROR_PrintData, "BBERROR Library", strlen("TOO MUCH DATA") + 1, (const uint8_t *) "TOO MUCH DATA");

        return 2;
    }

    ERROR_messages[ERROR_num_messages].PrintData = NULL;
    ERROR_messages[ERROR_num_messages].PrintDataDOS = msg_printdata_proc;
    ERROR_messages[ERROR_num_messages].prefix = msg_prefix;
    for (data_index = 0; data_index < msg_data_len; data_index++)
    {
        ERROR_messages[ERROR_num_messages].data[data_index] = msg_data[data_index];
    }
    ERROR_num_messages++;

    return 0;
}

void ERROR_RemoveMessage(void)
{
    if (ERROR_num_messages)
    {
        ERROR_num_messages--;
    }
}

int ERROR_NumMessagesIsZero(void)
{
    return (ERROR_num_messages == 0)?1:0;
}

void ERROR_PrintAndClearMessages(unsigned int flags)
{
    int index;
    int buflen;

    if (ERROR_num_messages == 0) return;

    if ((flags & 0x20) && (flags & 0x08))
    {
        if (ERROR_LogPrint != NULL)
        {
            //ERROR_LogPrint("BBERROR: ERRORSTACK START:--------------\n");
            // todo: remove
            Game_RunProcReg1_Asm(ERROR_LogPrint, "BBERROR: ERRORSTACK START:--------------\n");
        }
    }

    for (index = 0; index < ERROR_num_messages; index++)
    {
        ERROR_message_buffer[0] = 0;
        buflen = 0;

        if (flags & 0x02)
        {
            if (ERROR_messages[index].prefix != NULL)
            {
                sprintf(ERROR_message_buffer + buflen, "%s: ", ERROR_messages[index].prefix);
                buflen = strlen(ERROR_message_buffer);
            }
        }

        if (flags & 0x01)
        {
            if (ERROR_messages[index].PrintData != NULL)
            {
                ERROR_messages[index].PrintData(ERROR_message_buffer + buflen, ERROR_messages[index].data);
                buflen = strlen(ERROR_message_buffer);
            }
            // todo: remove
            else if (ERROR_messages[index].PrintDataDOS != NULL)
            {
                Game_RunProcReg2_Asm(ERROR_messages[index].PrintDataDOS, ERROR_message_buffer + buflen, ERROR_messages[index].data);
                buflen = strlen(ERROR_message_buffer);
            }
        }

        if (flags & 0x04)
        {
            sprintf(ERROR_message_buffer + buflen, "\n");
            buflen = strlen(ERROR_message_buffer);
        }

        if (flags & 0x20)
        {
            if (ERROR_LogPrint != NULL)
            {
                //ERROR_LogPrint(ERROR_message_buffer);
                // todo: remove
                Game_RunProcReg1_Asm(ERROR_LogPrint, ERROR_message_buffer);
            }
        }
    }

    if ((flags & 0x20) && (flags & 0x10))
    {
        if (ERROR_LogPrint != NULL)
        {
            //ERROR_LogPrint("BBERROR: ERRORSTACK END-----------------\n");
            // todo: remove
            Game_RunProcReg1_Asm(ERROR_LogPrint, "BBERROR: ERRORSTACK END-----------------\n");
        }
    }

    ERROR_num_messages = 0;
}

static void ERROR_PrintData(char *buffer, const uint8_t *data)
{
    sprintf(buffer, "INTERNAL ERROR: %s", (const char *)data);
}

