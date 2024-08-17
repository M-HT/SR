/**
 *
 *  Copyright (C) 2016-2024 Roman Pauer
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

#include <string.h>
#include "Game_defs.h"
#include "Game_vars.h"
#include "Xcom-proc-events.h"
#include "input.h"

#if !SDL_VERSION_ATLEAST(2,0,0)
//senquack
/* Convert GP2X touchscreen coordinates to keyboard cursor position */
static void Game_VK_Convert_Mouse(int *posX, int *posY)
{
    int lower_bound_x, upper_bound_x, lower_bound_y, upper_bound_y;

    lower_bound_x = ( Game_Screen->w / 2 ) - ((4*15) << Font_Size_Shift);
    upper_bound_x = lower_bound_x + ((16*8) << Font_Size_Shift);
    lower_bound_y = ( Game_Screen->h / 2 ) - ((4*11) << Font_Size_Shift);
    upper_bound_y = lower_bound_y + ((16*5) << Font_Size_Shift);

    // Shift everything slightly to the left and up since characters lack left & top borders
    *posX += 2;
    *posY += 2;

    if ((*posX >= lower_bound_x) && (*posX <= upper_bound_x) &&
        (*posY >= lower_bound_y) && (*posY <= upper_bound_y)
       )
    {
        // Touchscreen pressed inside bounds of onscreen keyboard, convert
        // 	touchscreen coordinates to key coordinates on screen
        *posX = *posX - lower_bound_x;
        *posX = *posX >> (4 + Font_Size_Shift);

        *posY = *posY - lower_bound_y;
        *posY = *posY >> (4 + Font_Size_Shift);

    }
    else
    {
        // Outside bounds of onscreen keyboard
        *posX = -1;
        *posY = -1;
    }
}

/*static void Game_VK_normal2x(const uint8_t *src, uint8_t *dst, int srcwidth, int srcheight)
{
    int x, y;

    for (y = 0; y < srcheight; y++)
    {
        for (x = 0; x < srcwidth; x++)
        {
            dst[0] = dst[1] = dst[2 * srcwidth] = dst[2 * srcwidth + 1] = src[x];
            dst += 2;
        }
        src += srcwidth;
        dst += 2 * srcwidth;
    }
}*/

static void Game_VK_scale2x(const uint8_t *src, uint8_t *dst, int srcwidth, int srcheight)
{
    int x, y;
    unsigned int /*A,*/ B, C, D, E, F, /*G,*/ H, I;

    // first line
    {
        // first column
        B = 0;
        E = 0;
        H = 0;
        C = 0;
        F = src[0];
        I = src[srcwidth];

        for (x = 1; x < srcwidth; x++)
        {
            //A = B;
            D = E;
            //G = H;
            B = C;
            E = F;
            H = I;
            C = 0;
            F = src[1];
            I = src[1 + srcwidth];
            src++;

            if (B != H && D != F)
            {
                dst[0] = (D == B) ? D : E; // E0
                dst[1] = (B == F) ? F : E; // E1
                dst[2 * srcwidth] = (D == H) ? D : E; // E2
                dst[2 * srcwidth + 1] = (H == F) ? F : E; // E3
                dst += 2;
            }
            else
            {
                dst[0] = E; // E0
                dst[1] = E; // E1
                dst[2 * srcwidth] = E; // E2
                dst[2 * srcwidth + 1] = E; // E3
                dst += 2;
            }
        }

        // last column
        //A = B;
        D = E;
        //G = H;
        B = C;
        E = F;
        H = I;
        C = 0;
        F = 0;
        I = 0;
        src++;

        if (B != H && D != F)
        {
            dst[0] = (D == B) ? D : E; // E0
            dst[1] = (B == F) ? F : E; // E1
            dst[2 * srcwidth] = (D == H) ? D : E; // E2
            dst[2 * srcwidth + 1] = (H == F) ? F : E; // E3
            dst += 2;
        }
        else
        {
            dst[0] = E; // E0
            dst[1] = E; // E1
            dst[2 * srcwidth] = E; // E2
            dst[2 * srcwidth + 1] = E; // E3
            dst += 2;
        }

        dst += 2 * srcwidth;
    }

    for (y = 2; y < srcheight; y++)
    {
        // first column
        B = 0;
        E = 0;
        H = 0;
        C = src[-srcwidth];
        F = src[0];
        I = src[srcwidth];

        for (x = 1; x < srcwidth; x++)
        {
            //A = B;
            D = E;
            //G = H;
            B = C;
            E = F;
            H = I;
            C = src[1 - srcwidth];
            F = src[1];
            I = src[1 + srcwidth];
            src++;

            if (B != H && D != F)
            {
                dst[0] = (D == B) ? D : E; // E0
                dst[1] = (B == F) ? F : E; // E1
                dst[2 * srcwidth] = (D == H) ? D : E; // E2
                dst[2 * srcwidth + 1] = (H == F) ? F : E; // E3
                dst += 2;
            }
            else
            {
                dst[0] = E; // E0
                dst[1] = E; // E1
                dst[2 * srcwidth] = E; // E2
                dst[2 * srcwidth + 1] = E; // E3
                dst += 2;
            }
        }

        // last column
        //A = B;
        D = E;
        //G = H;
        B = C;
        E = F;
        H = I;
        C = 0;
        F = 0;
        I = 0;
        src++;

        if (B != H && D != F)
        {
            dst[0] = (D == B) ? D : E; // E0
            dst[1] = (B == F) ? F : E; // E1
            dst[2 * srcwidth] = (D == H) ? D : E; // E2
            dst[2 * srcwidth + 1] = (H == F) ? F : E; // E3
            dst += 2;
        }
        else
        {
            dst[0] = E; // E0
            dst[1] = E; // E1
            dst[2 * srcwidth] = E; // E2
            dst[2 * srcwidth + 1] = E; // E3
            dst += 2;
        }

        dst += 2 * srcwidth;
    }

    // last line
    {
        // first column
        B = 0;
        E = 0;
        H = 0;
        C = src[-srcwidth];
        F = src[0];
        I = 0;

        for (x = 1; x < srcwidth; x++)
        {
            //A = B;
            D = E;
            //G = H;
            B = C;
            E = F;
            H = I;
            C = src[1 - srcwidth];
            F = src[1];
            I = 0;
            src++;

            if (B != H && D != F)
            {
                dst[0] = (D == B) ? D : E; // E0
                dst[1] = (B == F) ? F : E; // E1
                dst[2 * srcwidth] = (D == H) ? D : E; // E2
                dst[2 * srcwidth + 1] = (H == F) ? F : E; // E3
                dst += 2;
            }
            else
            {
                dst[0] = E; // E0
                dst[1] = E; // E1
                dst[2 * srcwidth] = E; // E2
                dst[2 * srcwidth + 1] = E; // E3
                dst += 2;
            }
        }

        // last column
        //A = B;
        D = E;
        //G = H;
        B = C;
        E = F;
        H = I;
        C = 0;
        F = 0;
        I = 0;
        src++;

        if (B != H && D != F)
        {
            dst[0] = (D == B) ? D : E; // E0
            dst[1] = (B == F) ? F : E; // E1
            dst[2 * srcwidth] = (D == H) ? D : E; // E2
            dst[2 * srcwidth + 1] = (H == F) ? F : E; // E3
            //dst += 2;
        }
        else
        {
            dst[0] = E; // E0
            dst[1] = E; // E1
            dst[2 * srcwidth] = E; // E2
            dst[2 * srcwidth + 1] = E; // E3
            //dst += 2;
        }

        //dst += 2 * srcwidth;
    }
}

/* draw char on screen */
static void Game_DisplayVK_Char(int pos, int column, int row, int invert)
{
    uint8_t *fontchar, *tempfontchar, *screen_line, *screen_char;
    uint32_t fgcolor, bgcolor;
    int x, y, i, j, k, pitch, bpp, pos2;


    if (UFO_Font == NULL) return;

    x = ( Game_Screen->w / 2 ) + ((column * 8 - 4) << Font_Size_Shift);
    y = ( Game_Screen->h / 2 ) + ((row * 8 - 4) << Font_Size_Shift);

/*
!"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~...
*/


    if (pos == 38) pos2 = 27;
    else if (pos == 37) pos2 = 0;
    else if (pos == 36) pos2 = 12;
    else if (pos == 35) pos2 = 15;
    else if (pos < 26) pos2 = pos + 32;
    else pos2 = pos - 26 + 16;

    fontchar = &(UFO_Font[pos2 * 8 * 9]);
    if (Font_Size_Shift != 0)
    {
        tempfontchar = &(Temp_Font_Data[(8*9) << (2*Font_Size_Shift)]);
        for (i = 0; i < 8*9; i++)
        {
            tempfontchar[i] = (fontchar[i] == 1)?1:0;
        }
        Game_VK_scale2x(tempfontchar, Temp_Font_Data, 8, 9);
        fontchar = Temp_Font_Data;

        for (i=1,j=16,k=18; i<Font_Size_Shift; i++,j*=2,k*=2)
        {
            uint8_t *tempptr;

            Game_VK_scale2x(fontchar, tempfontchar, j, k);

            tempptr = fontchar;
            fontchar = tempfontchar;
            tempfontchar = tempptr;
        }
    }

    if (invert)
    {

        fgcolor = (pos == 37)?0xffffffff:0;
        bgcolor = 0xffffffff;
    }
    else
    {
        fgcolor = (pos == 37)?0:0xffffffff;
        bgcolor = 0;
    }

    pitch = Game_Screen->pitch;
    bpp = Game_Screen->format->BytesPerPixel;
    screen_line = &(((uint8_t *) Game_Screen->pixels)[y * pitch + x * bpp]);

    if (bpp == 4)
    {
        for (j = 0; j < (9 << Font_Size_Shift); j++)
        {
            screen_char = screen_line;
            for (i = 0; i < (8 << Font_Size_Shift); i++)
            {
                *((uint32_t *) screen_char) = (*fontchar == 1)?fgcolor:bgcolor;

                fontchar++;
                screen_char+=bpp;
            }

            screen_line+=pitch;
        }
    }
    else if (bpp == 2)
    {
        for (j = 0; j < (9 << Font_Size_Shift); j++)
        {
            screen_char = screen_line;
            for (i = 0; i < (8 << Font_Size_Shift); i++)
            {
                *((uint16_t *) screen_char) = (*fontchar == 1)?fgcolor:bgcolor;

                fontchar++;
                screen_char+=bpp;
            }

            screen_line+=pitch;
        }
    }

}

/* draw virtual keyboard on screen */
static void Game_DisplayVirtualKeyboard(char *str, int xpos, int ypos)
{
    int cpos, pos, col, row, i;

    cpos = ypos * 8 + xpos;

    SDL_LockSurface(Game_Screen);

    memset(Game_Screen->pixels, 0, Game_Screen->pitch * Game_Screen->h);

    pos = 0;
    for (row = -5; row < 5; row+=2)
    {
        for (col = -7; col <= 7; col+=2)
        {
            if (pos < 39)
            {
                Game_DisplayVK_Char(pos, col, row, (cpos == pos)?1:0);
            }
            else
            {
                Game_DisplayVK_Char(14, col, row, (cpos == pos)?1:0);
                Game_DisplayVK_Char(10, col + 1, row, (cpos == pos)?1:0);
            }

            pos++;
        }
    }

    i = 0;
    while (str[i])
    {
        if (str[i] == ' ') pos = 37;
        else if (str[i] == '-') pos = 36;
        else if (str[i] == '0') pos = 35;
        else if (str[i] < 'A') pos = str[i] - '1' + 26;
        else pos = str[i] - 'A';

        Game_DisplayVK_Char(pos, i - 7, 5, 0);
        i++;
    }

    SDL_UnlockSurface(Game_Screen);
    SDL_Flip(Game_Screen);
}

static void Game_VirtualKeyboard(void)
{
    SDL_Event *cevent, event;
    int MaxStrLen, Cont, PosX, PosY;
    char VirtStr[16];

    //senquack - new touchscreen code
    int current_x, current_y, converted_x, converted_y, orig_x, orig_y;
    unsigned int current_buttons;

    Game_KQueueRead = (Game_KQueueRead + 1) & (GAME_KQUEUE_LENGTH - 1);

    if (Game_Screen == NULL) return;

    Game_Paused = 1;

    Cont = 1;
    MaxStrLen = 15;
    VirtStr[0] = 0;
    PosX = 0;
    PosY = 0;


    current_x = Game_MouseX;
    current_y = Game_MouseY;
    current_buttons = Game_MouseButtons;

    SDL_GetMouseState(&orig_x, &orig_y);

    //SDL_ShowCursor(SDL_DISABLE);

    /* wait for existing flip to finish */
    SDL_Delay(100);

    Game_DisplayVirtualKeyboard(&(VirtStr[0]), PosX, PosY);

    while (Cont)
    {
        SDL_Delay(10);

        if (Thread_Exit)
        {
            Game_Paused = 0;
            return;
        }

        // process mouse
        {
            // mouse motion
            if ((Game_MouseX != current_x) || (Game_MouseY != current_y))
            {
                current_x = Game_MouseX;
                current_y = Game_MouseY;
                converted_x = current_x;
                converted_y = current_y;
                Game_VK_Convert_Mouse(&converted_x, &converted_y);
                if ((converted_x >= 0) && (converted_y >= 0))
                {
                    PosX = converted_x;
                    PosY = converted_y;
                    Game_DisplayVirtualKeyboard(&(VirtStr[0]), PosX, PosY);
                }
            }
            // mouse buttons
            if (Game_MouseButtons != current_buttons)
            {
                // left mouse button is released
                if (((Game_MouseButtons & 1) == 0) && ((current_buttons & 1) != 0))
                {
                    converted_x = current_x;
                    converted_y = current_y;
                    Game_VK_Convert_Mouse(&converted_x, &converted_y);
                    if ((converted_x >= 0) && (converted_y >= 0))
                    {
                        int Pos, Len;

                        PosX = converted_x;
                        PosY = converted_y;
                        Pos = PosY * 8 + PosX;

/*
ABCDEFGH
IJKLMNOP
QRSTUVWX
YZ123456
7890- <OK
*/
                        if (Pos == 39)
                        {
                            Cont = 0;
                        }
                        else if (Pos == 38)
                        {
                            if (VirtStr[0] != 0)
                            {
                                VirtStr[strlen(VirtStr) - 1] = 0;
                            }
                        }
                        else
                        {
                            Len = strlen(VirtStr);
                            if (Len < MaxStrLen)
                            {
                                if (Pos == 37) VirtStr[Len] = ' ';
                                else if (Pos == 36) VirtStr[Len] = '-';
                                else if (Pos == 35) VirtStr[Len] = '0';
                                else if (Pos >= 26) VirtStr[Len] = Pos - 26 + '1';
                                else VirtStr[Len] = Pos + 'A';
                                VirtStr[Len + 1] = 0;
                            }
                        }

                        Game_DisplayVirtualKeyboard(&(VirtStr[0]), PosX, PosY);
                    }
                }

                current_buttons = Game_MouseButtons;
            }
        }

        // process keyboard events
        while (Cont && Game_KQueueWrite != Game_KQueueRead)
        {
            if (Thread_Exit)
            {
                Game_Paused = 0;
                return;
            }

            cevent = &(Game_EventKQueue[Game_KQueueRead]);

            switch(cevent->type)
            {
                case SDL_KEYUP:
                    switch((int) cevent->key.keysym.sym)
                    {
                        case SDLK_UP:
                            if (PosY == 0)
                            {
                                PosY = 4;
                            }
                            else
                            {
                                PosY--;
                            }
                            Game_DisplayVirtualKeyboard(&(VirtStr[0]), PosX, PosY);
                            break;

                        case SDLK_DOWN:
                            if (PosY == 4)
                            {
                                PosY = 0;
                            }
                            else
                            {
                                PosY++;
                            }
                            Game_DisplayVirtualKeyboard(&(VirtStr[0]), PosX, PosY);
                            break;

                        case SDLK_LEFT:
                            if (PosX == 0)
                            {
                                PosX = 7;
                            }
                            else
                            {
                                PosX--;
                            }
                            Game_DisplayVirtualKeyboard(&(VirtStr[0]), PosX, PosY);
                            break;

                        case SDLK_RIGHT:
                            if (PosX == 7)
                            {
                                PosX = 0;
                            }
                            else
                            {
                                PosX++;
                            }
                            Game_DisplayVirtualKeyboard(&(VirtStr[0]), PosX, PosY);
                            break;

                        case SDLK_KP_ENTER:
                        case SDLK_RETURN:
                            {
                                int Pos, Len;

                                Pos = PosY * 8 + PosX;

/*
ABCDEFGH
IJKLMNOP
QRSTUVWX
YZ123456
7890- <OK
*/
                                if (Pos == 39)
                                {
                                    Cont = 0;
                                }
                                else if (Pos == 38)
                                {
                                    if (VirtStr[0] != 0)
                                    {
                                        VirtStr[strlen(VirtStr) - 1] = 0;
                                        Game_DisplayVirtualKeyboard(&(VirtStr[0]), PosX, PosY);
                                    }
                                }
                                else
                                {
                                    Len = strlen(VirtStr);
                                    if (Len < MaxStrLen)
                                    {
                                        if (Pos == 37) VirtStr[Len] = ' ';
                                        else if (Pos == 36) VirtStr[Len] = '-';
                                        else if (Pos == 35) VirtStr[Len] = '0';
                                        else if (Pos >= 26) VirtStr[Len] = Pos - 26 + '1';
                                        else VirtStr[Len] = Pos + 'A';
                                        VirtStr[Len + 1] = 0;
                                        Game_DisplayVirtualKeyboard(&(VirtStr[0]), PosX, PosY);
                                    }
                                }
                            }
                            break;
                    } // switch((int) cevent->key.keysym.sym)

                    break;
            } // switch(cevent->type)

            Game_KQueueRead = (Game_KQueueRead + 1) & (GAME_KQUEUE_LENGTH - 1);
        }

    }

    if (VirtStr[0] != 0)
    {
        char *str;

        str = &(VirtStr[0]);

        /* clear keyboard events */
        if (Game_KQueueWrite != Game_KQueueRead)
        {
            Game_KQueueRead = Game_KQueueWrite;
        }

        while (*str != 0)
        {
            EmulateKey(SDL_KEYDOWN, (int) *str);
            EmulateKey(SDL_KEYUP, (int) *str);

            str++;
        }
    }

    // Restore original cursor position
    event.type = SDL_USEREVENT;
    event.user.code = EC_MOUSE_SET;
    event.user.data1 = (void *)(intptr_t) orig_x;
    event.user.data2 = (void *)(intptr_t) orig_y;

    SDL_PushEvent(&event);

    if (!Display_MouseLocked)
    {
        //SDL_ShowCursor(SDL_ENABLE);
    }

    event.type = SDL_USEREVENT;
    event.user.code = EC_DISPLAY_FLIP_START;
    event.user.data1 = NULL;
    event.user.data2 = NULL;

    SDL_PushEvent(&event);

    Game_MouseButtons = 0;
    Game_MousePressedButtons = 0;

    Game_Paused = 0;
}
#endif

void Game_ProcessKEvents()
{
    int finish;
    uint32_t VSyncTick, key_code, ascii_code, scancode;
    SDL_Event *cevent;

    const static uint8_t scancode_table[128] = {
           0,    0,    0,    0,    0,    0,    0,    0, 0x0e, 0x0f,    0,    0,    0, 0x1c,    0,    0, /*   0- 15 */
           0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0x01,    0,    0,    0,    0, /*  16- 31 */
        0x39, 0x02, 0x28, 0x04, 0x05, 0x06, 0x08, 0x28, 0x0a, 0x0b, 0x09, 0x0d, 0x33, 0x0c, 0x34, 0x35, /*  32- 47 */
        0x0b, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x27, 0x27, 0x33, 0x0d, 0x34, 0x35, /*  48- 63 */
        0x03, 0x1e, 0x30, 0x2e, 0x20, 0x12, 0x21, 0x22, 0x23, 0x17, 0x24, 0x25, 0x26, 0x32, 0x31, 0x18, /*  64- 79 */
        0x19, 0x10, 0x13, 0x1f, 0x14, 0x16, 0x2f, 0x11, 0x2d, 0x15, 0x2c, 0x1a, 0x2b, 0x1b, 0x07, 0x0c, /*  80- 95 */
        0x29, 0x1e, 0x30, 0x2e, 0x20, 0x12, 0x21, 0x22, 0x23, 0x17, 0x24, 0x25, 0x26, 0x32, 0x31, 0x18, /*  96-111 */
        0x19, 0x10, 0x13, 0x1f, 0x14, 0x16, 0x2f, 0x11, 0x2d, 0x15, 0x2c, 0x1a, 0x2b, 0x1b, 0x29, 0x53, /* 112-127 */
    };

    const static uint8_t bios_scancode_table[128] = {
           0, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, /*   0- 15 */
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, /*  16- 31 */
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, /*  32- 47 */
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, /*  48- 63 */
        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, /*  64- 79 */
        0x50, 0x51, 0x52, 0x53,    0,    0,    0, 0x85, 0x86,    0,    0, 0xec, 0xed, 0xee,    0,    0, /*  80- 95 */
           0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, /*  96-111 */
           0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, /* 112-127 */
    };

    VSyncTick = Game_VSyncTick;
    finish = 0;

    while (!finish && Game_KQueueWrite != Game_KQueueRead)
    {
        cevent = &(Game_EventKQueue[Game_KQueueRead]);

        switch(cevent->type)
        {
            //senquack
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                if (cevent->key.keysym.sym == SDLK_PAUSE)
                {
                    goto _after_switch1;
                }
            #if !SDL_VERSION_ATLEAST(2,0,0)
                else if ((cevent->key.keysym.unicode > 0) && (cevent->key.keysym.unicode < 128))
                {
                    scancode = scancode_table[cevent->key.keysym.unicode];
                    ascii_code = cevent->key.keysym.unicode;
                }
            #endif
                else if (cevent->key.keysym.sym < 128)
                {
                    scancode = scancode_table[cevent->key.keysym.sym];

                    if (((cevent->key.keysym.mod & KMOD_SHIFT) == 0) != ((cevent->key.keysym.mod & KMOD_CAPS) == 0))
                    {
                        if (cevent->key.keysym.sym >= SDLK_a && cevent->key.keysym.sym <= SDLK_z)
                        {
                            ascii_code = cevent->key.keysym.sym - 32;
                        }
                        else
                        {
                            ascii_code = cevent->key.keysym.sym;
                        }
                    }
                    else
                    {
                        ascii_code = cevent->key.keysym.sym;
                    }
                }
                else
                {
                    ascii_code = 0;
                    scancode = 0;

                    switch((int) cevent->key.keysym.sym)
                    {
                    #if SDL_VERSION_ATLEAST(2,0,0)
                        case SDLK_KP_0:
                    #else
                        case SDLK_KP0:
                    #endif
                            scancode = 0x52;
                            if (cevent->key.keysym.mod & KMOD_NUM) ascii_code = '0';

                            break;
                    #if SDL_VERSION_ATLEAST(2,0,0)
                        case SDLK_KP_1:
                    #else
                        case SDLK_KP1:
                    #endif
                            scancode = 0x4f;
                            if (cevent->key.keysym.mod & KMOD_NUM) ascii_code = '1';

                            break;
                    #if SDL_VERSION_ATLEAST(2,0,0)
                        case SDLK_KP_2:
                    #else
                        case SDLK_KP2:
                    #endif
                            scancode = 0x50;
                            if (cevent->key.keysym.mod & KMOD_NUM) ascii_code = '2';

                            break;
                    #if SDL_VERSION_ATLEAST(2,0,0)
                        case SDLK_KP_3:
                    #else
                        case SDLK_KP3:
                    #endif
                            scancode = 0x51;
                            if (cevent->key.keysym.mod & KMOD_NUM) ascii_code = '3';

                            break;
                    #if SDL_VERSION_ATLEAST(2,0,0)
                        case SDLK_KP_4:
                    #else
                        case SDLK_KP4:
                    #endif
                            scancode = 0x4b;
                            if (cevent->key.keysym.mod & KMOD_NUM) ascii_code = '4';

                            break;
                    #if SDL_VERSION_ATLEAST(2,0,0)
                        case SDLK_KP_5:
                    #else
                        case SDLK_KP5:
                    #endif
                            scancode = 0x4c;
                            if (cevent->key.keysym.mod & KMOD_NUM) ascii_code = '5';

                            break;
                    #if SDL_VERSION_ATLEAST(2,0,0)
                        case SDLK_KP_6:
                    #else
                        case SDLK_KP6:
                    #endif
                            scancode = 0x4d;
                            if (cevent->key.keysym.mod & KMOD_NUM) ascii_code = '6';

                            break;
                    #if SDL_VERSION_ATLEAST(2,0,0)
                        case SDLK_KP_7:
                    #else
                        case SDLK_KP7:
                    #endif
                            scancode = 0x47;
                            if (cevent->key.keysym.mod & KMOD_NUM) ascii_code = '7';

                            break;
                    #if SDL_VERSION_ATLEAST(2,0,0)
                        case SDLK_KP_8:
                    #else
                        case SDLK_KP8:
                    #endif
                            scancode = 0x48;
                            if (cevent->key.keysym.mod & KMOD_NUM) ascii_code = '8';

                            break;
                    #if SDL_VERSION_ATLEAST(2,0,0)
                        case SDLK_KP_9:
                    #else
                        case SDLK_KP9:
                    #endif
                            scancode = 0x49;
                            if (cevent->key.keysym.mod & KMOD_NUM) ascii_code = '9';

                            break;
                        case SDLK_KP_PERIOD:
                            scancode = 0x53;
                            if (cevent->key.keysym.mod & KMOD_NUM) ascii_code = '.';

                            break;
                        case SDLK_KP_DIVIDE:
                            scancode = 0x35;
                            ascii_code = '/';

                            break;
                        case SDLK_KP_MULTIPLY:
                            scancode = 0x37;
                            ascii_code = '*';

                            break;
                        case SDLK_KP_MINUS:
                            scancode = 0x4a;
                            ascii_code = '-';

                            break;
                        case SDLK_KP_PLUS:
                            scancode = 0x4e;
                            ascii_code = '+';

                            break;
                        case SDLK_KP_ENTER:
                            scancode = 0x1c;
                            ascii_code = 0x1c;

                            break;
                        case SDLK_KP_EQUALS:
                            scancode = 0x0d;
                            ascii_code = '=';

                            break;
                        case SDLK_UP:
                            scancode = 0x48;

                            break;
                        case SDLK_DOWN:
                            scancode = 0x50;

                            break;
                        case SDLK_RIGHT:
                            scancode = 0x4d;

                            break;
                        case SDLK_LEFT:
                            scancode = 0x4b;

                            break;
                        case SDLK_INSERT:
                            scancode = 0x52;

                            break;
                        case SDLK_HOME:
                            scancode = 0x47;

                            break;
                        case SDLK_END:
                            scancode = 0x4f;

                            break;
                        case SDLK_PAGEUP:
                            scancode = 0x49;

                            break;
                        case SDLK_PAGEDOWN:
                            scancode = 0x51;

                            break;
                        case SDLK_F1:
                            scancode = 0x3b;

                            break;
                        case SDLK_F2:
                            scancode = 0x3c;

                            break;
                        case SDLK_F3:
                            scancode = 0x3d;

                            break;
                        case SDLK_F4:
                            scancode = 0x3e;

                            break;
                        case SDLK_F5:
                            scancode = 0x3f;

                            break;
                        case SDLK_F6:
                            scancode = 0x40;

                            break;
                        case SDLK_F7:
                            scancode = 0x41;

                            break;
                        case SDLK_F8:
                            scancode = 0x42;

                            break;
                        case SDLK_F9:
                            scancode = 0x43;

                            break;
                        case SDLK_F10:
                            scancode = 0x44;

                            break;
                        case SDLK_F11:
                            scancode = 0x57;

                            break;
                        case SDLK_F12:
                            scancode = 0x58;

                            break;
                        case SDLK_F15:
                    #if SDL_VERSION_ATLEAST(2,0,0)
                            goto _after_switch1;
                    #else
                        #ifdef ALLOW_OPENGL
                            if (Game_UseOpenGL)
                            {
                                goto _after_switch1;
                            }
                        #endif
                            if (cevent->type == SDL_KEYUP)
                            {
                                Game_VirtualKeyboard();
                                goto _after_switch2;
                            }
                            else goto _after_switch1;
                    #endif
                    #if SDL_VERSION_ATLEAST(2,0,0)
                        case SDLK_NUMLOCKCLEAR:
                    #else
                        case SDLK_NUMLOCK:
                    #endif
                            scancode = 0x45;

                            break;
                        case SDLK_CAPSLOCK:
                            scancode = 0x3a;

                            break;
                    #if SDL_VERSION_ATLEAST(2,0,0)
                        case SDLK_SCROLLLOCK:
                    #else
                        case SDLK_SCROLLOCK:
                    #endif
                            scancode = 0x46;

                            break;
                        case SDLK_RSHIFT:
                            scancode = 0x36;

                            break;
                        case SDLK_LSHIFT:
                            scancode = 0x2a;

                            break;
                        case SDLK_RCTRL:
                            scancode = 0x1d;

                            break;
                        case SDLK_LCTRL:
                            scancode = 0x1d;

                            break;
                        case SDLK_RALT:
                            scancode = 0x38;

                            break;
                        case SDLK_LALT:
                            scancode = 0x38;

                            break;
                    #if SDL_VERSION_ATLEAST(2,0,0)
                        case SDLK_LGUI:
                    #else
                        case SDLK_LSUPER:
                    #endif
                            scancode = 0x5b;

                            break;
                    #if SDL_VERSION_ATLEAST(2,0,0)
                        case SDLK_RGUI:
                    #else
                        case SDLK_RSUPER:
                    #endif
                            scancode = 0x5c;

                            break;
                        case SDLK_MODE:
                            scancode = 0x5d;

                            break;
                        case SDLK_MENU:
                            scancode = 0x38;

                            break;

                        default:
                            goto _after_switch1;
                    }
                }
                if (cevent->key.state == SDL_RELEASED)
                {
                    scancode |= 0x80;
                }

                if (cevent->key.state == SDL_PRESSED)
                {
                    key_code = (scancode << 24) | (bios_scancode_table[scancode & 0x7f] << 8) | ascii_code;

                    if ( ( (Game_KBufferWrite + 1) & (GAME_KBUFFER_LENGTH - 1) ) == Game_KBufferRead )
                    {
#if defined(__DEBUG__)
                        fprintf(stderr, "keyboard buffer overflow\n");
#endif
                    }
                    else
                    {
                        Game_KBuffer[Game_KBufferWrite] = key_code;

                        Game_KBufferWrite = (Game_KBufferWrite + 1) & (GAME_KBUFFER_LENGTH - 1);
                    }


                }

                break;
                // case SDL_KEYDOWN, SDL_KEYUP:
        } // switch(event.type)

    _after_switch1:
        Game_KQueueRead = (Game_KQueueRead + 1) & (GAME_KQUEUE_LENGTH - 1);
#if !SDL_VERSION_ATLEAST(2,0,0)
    _after_switch2:
#endif
        if (VSyncTick != Game_VSyncTick) finish = 1;
    }

}

// convert device coordinates to picture coordinates
int32_t Game_Device2PictureX(int32_t devicex)
{
    if (devicex < Picture_Position_UL_X) devicex = Picture_Position_UL_X;
    else if (devicex > Picture_Position_BR_X) devicex = Picture_Position_BR_X;

    return devicex - Picture_Position_UL_X;
}

int32_t Game_Device2PictureY(int32_t devicey)
{
    if (devicey < Picture_Position_UL_Y) devicey = Picture_Position_UL_Y;
    else if (devicey > Picture_Position_BR_Y) devicey = Picture_Position_BR_Y;

    return devicey - Picture_Position_UL_Y;
}

// convert picture coordinates to device coordinates
int32_t Game_Picture2DeviceX(int32_t picturex)
{
    picturex += Picture_Position_UL_X;

    if (picturex < 0) picturex = 0;
    else if (picturex >= (int32_t) Display_Width) picturex = Display_Width - 1;

    return picturex;
}

int32_t Game_Picture2DeviceY(int32_t picturey)
{
    picturey += Picture_Position_UL_Y;

    if (picturey < 0) picturey = 0;
    else if (picturey >= (int32_t) Display_Height) picturey = Display_Height - 1;

    return picturey;
}

void Game_GetGameMouse(int *mousex, int *mousey)
{
    int mx, my;

    SDL_GetMouseState(&mx, &my);
    *mousex = (Game_Device2PictureX(mx) * Game_VideoAspectX + 32767) >> 16;
    *mousey = (Game_Device2PictureY(my) * Game_VideoAspectY + 32767) >> 16;
}

void Game_RepositionMouse(int mousex, int mousey)
{
    SDL_Event event;

    event.type = SDL_USEREVENT;
    event.user.code = EC_MOUSE_SET;
    event.user.data1 = (void *)(intptr_t) Game_Picture2DeviceX((mousex * Game_VideoAspectXR + 32767) >> 16);
    event.user.data2 = (void *)(intptr_t) Game_Picture2DeviceY((mousey * Game_VideoAspectYR + 32767) >> 16);

    SDL_PushEvent(&event);
}

