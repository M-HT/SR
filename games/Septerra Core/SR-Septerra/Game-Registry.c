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

#include "Game-Registry.h"

#if (defined(__WIN32__) || defined(__WINDOWS__)) && !defined(_WIN32)
#define _WIN32
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "CLIB.h"
#endif

#define REGISTRY_BASE "SOFTWARE\\Valkyrie Studios\\Septerra Core"
#define CONF_NAME "septerra.conf"

#if !defined(_WIN32)
static int SetValue(const char *ValueName, char *Value)
{
    FILE *file, *fnew;
    int buf_length;
    char buf[8192];
    char *value_start, *name_start, *conf_name;
    int value_written, hasnewline;
    char old_char;

    file = fopen(CONF_NAME, "rb");
    if (file == NULL)
    {
        if (CLIB_FindFile(CONF_NAME, buf))
        {
            conf_name = buf;
            file = fopen(buf, "rb");
        }
    } else conf_name = NULL;

    if (file == NULL)
    {
        fnew = fopen(buf, "wb");
        if (fnew == NULL)
        {
            return 0;
        }
        else
        {
            conf_name = NULL;
        }
    }
    else
    {
        fnew = fopen(CONF_NAME "new", "wb");
        if (fnew == NULL)
        {
            fclose(file);
            return 0;
        }

        if (conf_name != NULL)
        {
            conf_name = strdup(conf_name);
        };
    }

    value_written = 0;
    hasnewline = 1;
    if (file != NULL)
    {
        while (!feof(file))
        {
            buf[0] = 0;
            buf_length = fscanf(file, "%8192[\r\n]", buf);
            if (buf_length > 0)
            {
                hasnewline = 1;
                fputs(&(buf[0]), fnew);
            }

            buf[0] = 0;
            buf_length = fscanf(file, "%8192[^\r\n]", buf);

            if (buf_length <= 0) continue;

            if (value_written)
            {
                fputs(&(buf[0]), fnew);
                continue;
            }

            hasnewline = 0;

            value_start = strchr(&(buf[0]), '=');
            if (value_start == NULL)
            {
                fputs(&(buf[0]), fnew);
                continue;
            }

            name_start = &(buf[0]);

            while (*name_start == ' ') name_start++;

            buf_length = (uintptr_t)value_start - (uintptr_t)name_start;
            while ((buf_length != 0) && (name_start[buf_length - 1] == ' '))
            {
                buf_length--;
            }
            old_char = name_start[buf_length];
            name_start[buf_length] = 0;

            if (0 != strcasecmp(ValueName, name_start))
            {
                name_start[buf_length] = old_char;
                fputs(&(buf[0]), fnew);
                continue;
            }

            value_written = 1;
            fputs(&(buf[0]), fnew);
            fputc('=', fnew);
            fputs(Value, fnew);
        }

        fclose(file);
    }

    if (!value_written)
    {
        if (!hasnewline)
        {
            fputc('\n', fnew);
        }
        fputs(ValueName, fnew);
        fputc('=', fnew);
        fputs(Value, fnew);
    }

    fclose(fnew);

    if (file != NULL)
    {
        remove((conf_name != NULL)?conf_name:CONF_NAME);
        if (0 != rename(CONF_NAME "new", (conf_name != NULL)?conf_name:CONF_NAME))
        {
            remove(CONF_NAME "new");
            if (conf_name != NULL) free(conf_name);
            return 0;
        }

        if (conf_name != NULL) free(conf_name);
    }

    return 1;
}

static int GetValue(const char *ValueName, char *Value, unsigned int Length)
{
    FILE *file;
    unsigned int buf_length;
    char buf[8192];
    char *value_start, *name_start;

    file = fopen(CONF_NAME, "rb");
    if (file == NULL)
    {
        if (CLIB_FindFile(CONF_NAME, buf))
        {
            file = fopen(buf, "rb");
        }
    }
    if (file == NULL) return 0;

    while (!feof(file))
    {
        buf_length = fscanf(file, "%8192[\r\n]", buf);
        buf[0] = 0;
        buf_length = fscanf(file, "%8192[^\r\n]", buf);

        if (buf_length <= 0) continue;

        value_start = strchr(&(buf[0]), '=');
        if (value_start == NULL) continue;

        name_start = &(buf[0]);

        while (*name_start == ' ') name_start++;

        buf_length = (uintptr_t)value_start - (uintptr_t)name_start;
        while ((buf_length != 0) && (name_start[buf_length - 1] == ' '))
        {
            buf_length--;
        }
        name_start[buf_length] = 0;

        if (0 != strcasecmp(ValueName, name_start)) continue;

        value_start++;
        while (*value_start == ' ') value_start++;

        buf_length = strlen(value_start);
        while ((buf_length != 0) && (value_start[buf_length - 1] == ' '))
        {
            buf_length--;
        }
        value_start[buf_length] = 0;

        if (buf_length >= Length) break;

        memcpy(Value, value_start, buf_length + 1);

        fclose(file);
        return 1;
    }

    fclose(file);
    return 0;
}
#endif

int32_t Registry_SetValueDword(const char *ValueName, uint32_t Value)
{
#ifdef _WIN32
    HKEY hKey;
    LSTATUS result;

    if (ERROR_SUCCESS != RegOpenKeyExA(HKEY_LOCAL_MACHINE, REGISTRY_BASE, 0, KEY_QUERY_VALUE, &hKey))
    {
        return 0;
    }

    result = RegSetValueExA(hKey, ValueName, 0, REG_DWORD, (const BYTE *)&Value, 4);

    RegCloseKey(hKey);

    return (result == ERROR_SUCCESS)?1:0;
#else
    char StringValue[20];
    uint32_t OldValue;

    if (Registry_GetValueDword(ValueName, &OldValue))
    {
        if (OldValue == Value) return 1;
    }

    snprintf(&(StringValue[0]), 20, "0x%x", (unsigned int) Value);
    return SetValue(ValueName, &(StringValue[0]));
#endif
}

/*int32_t Registry_SetValueString(const char *ValueName, const char *Value)
{
#ifdef _WIN32
    HKEY hKey;
    LSTATUS result;

    if (ERROR_SUCCESS != RegOpenKeyExA(HKEY_LOCAL_MACHINE, REGISTRY_BASE, 0, KEY_QUERY_VALUE, &hKey))
    {
        return 0;
    }

    result = RegSetValueExA(hKey, ValueName, 0, REG_SZ, (const BYTE *)Value, strlen(Value) + 1);

    RegCloseKey(hKey);

    return (result == ERROR_SUCCESS)?1:0;
#else
    return SetValue(ValueName, Value);
#endif
}*/

int32_t Registry_GetValueDword(const char *ValueName, uint32_t *Value)
{
#ifdef _WIN32
    HKEY hKey;
    LSTATUS result;
    DWORD type, cbData;

    *Value = -1;
    if (ERROR_SUCCESS == RegOpenKeyExA(HKEY_LOCAL_MACHINE, REGISTRY_BASE, 0, KEY_QUERY_VALUE, &hKey))
    {
        cbData = 4;
        result = RegQueryValueExA(hKey, ValueName, 0, &type, (LPBYTE)Value, &cbData);
        RegCloseKey(hKey);

        if (result == ERROR_SUCCESS) return 1;

        *Value = -1;
    }

    return 0;
#else
    char StringValue[20];
    unsigned long IntValue;

    if (GetValue(ValueName, &(StringValue[0]), 20))
    {
        IntValue = strtoul(&(StringValue[0]), NULL, 0);
        if (errno == ERANGE)
        {
            *Value = 0;
            return 0;
        }

        *Value = IntValue;
        return 1;
    }
    else
    {
        *Value = 0;
        return 0;
    }
#endif
}

int32_t Registry_GetValueString(const char *ValueName, char *Value)
{
#ifdef _WIN32
    HKEY hKey;
    LSTATUS result;
    DWORD type, cbData;

    *Value = 0;
    if (ERROR_SUCCESS == RegOpenKeyExA(HKEY_LOCAL_MACHINE, REGISTRY_BASE, 0, KEY_QUERY_VALUE, &hKey))
    {
        cbData = 254;
        result = RegQueryValueExA(hKey, ValueName, 0, &type, (LPBYTE)Value, &cbData);
        RegCloseKey(hKey);

        if (result == ERROR_SUCCESS) return 1;

        *Value = 0;
    }

    return 0;
#else
    if (0 == strcasecmp(ValueName, "InstallPath"))
    {
        Value[0] = '.';
        Value[1] = 0;
        return 1;
    }
    else if (0 == strcasecmp(ValueName, "SourcePath"))
    {
        Value[0] = '.';
        Value[1] = '/';
        Value[2] = 0;
        return 1;
    }
    else if (GetValue(ValueName, Value, 255))
    {
        return 1;
    }
    else
    {
        *Value = 0;
        return 0;
    }
#endif
}

