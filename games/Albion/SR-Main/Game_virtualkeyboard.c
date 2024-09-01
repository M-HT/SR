/**
 *
 *  Copyright (C) 2024 Roman Pauer
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

#include "Game_defs.h"
#include "Game_vars.h"
#include "Game_virtualkeyboard.h"
#include "input.h"
#if SDL_VERSION_ATLEAST(2,0,0)
    #include "VKfont2.h"
    #define VK_INNER_BORDER 2
    #define VK_OUTER_BORDER 4
#else
    #include "VKfont1.h"
    #define VK_INNER_BORDER 1
    #define VK_OUTER_BORDER 2
#endif

static SDL_Surface *virtual_keyboard_surface = NULL;
#if SDL_VERSION_ATLEAST(2,0,0)
static SDL_Texture *virtual_keyboard_texture = NULL;
static SDL_Surface *virtual_keyboard_texture_surface = NULL;
#else
#if defined(ALLOW_OPENGL)
static uint32_t *virtual_keyboard_texture_pixels = NULL;
static GLuint virtual_keyboard_texture = 0;
#endif
static SDL_Surface *virtual_keyboard_scaled_surface = NULL;
#endif
static int virtual_keyboard_pos_x;
static int virtual_keyboard_pos_y;
static int virtual_keyboard_altlayout;

#define VK_LAYOUT_WIDTH 13
static const uint8_t virtual_keyboard_layout[2][4][VK_LAYOUT_WIDTH] = { {
    {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M'},
    {'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'},
    {'!', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '='},
    {',', '.', '/', ';', '\'', '*', '(', ')', ' ', '\b', '\177', '\r', '\n'},
}, {
    {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm'},
    {'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'},
    {'!', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '+'},
    {'<', '>', '?', ':', '"', '#', '%', '&', ' ', '\b', '\177', '\r', '\n'},
}, };

static uint8_t virtual_keyboard_buffer[2 * VK_LAYOUT_WIDTH];
static int virtual_keyboard_buflen;


void VirtualKeyboard_Show(void)
{
    if (!VK_Visible)
    {
        VK_Visible = 1;
        virtual_keyboard_pos_x = 0;
        virtual_keyboard_pos_y = 0;
        virtual_keyboard_altlayout = 0;
        virtual_keyboard_buflen = 0;
    }
}

void VirtualKeyboard_Delete(void)
{
    VK_Visible = 0;
#if SDL_VERSION_ATLEAST(2,0,0)
    if (virtual_keyboard_texture != NULL)
    {
        SDL_DestroyTexture(virtual_keyboard_texture);
        virtual_keyboard_texture = NULL;
    }
    if (virtual_keyboard_texture_surface != NULL)
    {
        SDL_FreeSurface(virtual_keyboard_texture_surface);
        virtual_keyboard_texture_surface = NULL;
    }
#else
#if defined(ALLOW_OPENGL)
    if (virtual_keyboard_texture_pixels != NULL)
    {
        free(virtual_keyboard_texture_pixels);
        virtual_keyboard_texture_pixels = NULL;
    }
    if (virtual_keyboard_texture != 0)
    {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &virtual_keyboard_texture);
        virtual_keyboard_texture = 0;
    }
#endif
    if (virtual_keyboard_scaled_surface != NULL)
    {
        if (virtual_keyboard_scaled_surface != virtual_keyboard_surface)
        {
            SDL_FreeSurface(virtual_keyboard_scaled_surface);
        }
        virtual_keyboard_scaled_surface = NULL;
    }
#endif
    if (virtual_keyboard_surface != NULL)
    {
        SDL_FreeSurface(virtual_keyboard_surface);
        virtual_keyboard_surface = NULL;
    }
}

static void VK_DrawCharacter(SDL_Surface *surface, uint8_t chr, int x, int y)
{
    const uint8_t *src;
    uint8_t *dst, prev_value, value;
    int offset, sx, sy;

    if (vkfont_chr_index[chr] < 0) return;

    src = vkfont_chr_data + vkfont_chr_size * vkfont_chr_index[chr];
    dst = ((uint8_t *)surface->pixels) + y * surface->pitch + (x >> 3);

    offset = x & 7;

    if (offset == 0)
    {
        for (sy = 0; sy < vkfont_chr_height; sy++)
        {
            for (sx = 0; sx < vkfont_chr_width_bytes; sx++)
            {
                dst[sx] |= src[sx];
            }
            src += vkfont_chr_width_bytes;
            dst += surface->pitch;
        }
    }
    else if (((8 - (vkfont_chr_width_pixels & 7)) & 7) >= offset)
    {
        for (sy = 0; sy < vkfont_chr_height; sy++)
        {
            prev_value = 0;
            for (sx = 0; sx < vkfont_chr_width_bytes; sx++)
            {
                value = src[sx];
                dst[sx] |= (value >> offset) | (prev_value << (8 - offset));
                prev_value = value;
            }
            src += vkfont_chr_width_bytes;
            dst += surface->pitch;
        }
    }
    else
    {
        for (sy = 0; sy < vkfont_chr_height; sy++)
        {
            prev_value = 0;
            for (sx = 0; sx < vkfont_chr_width_bytes; sx++)
            {
                value = src[sx];
                dst[sx] |= (value >> offset) | (prev_value << (8 - offset));
                prev_value = value;
            }
            dst[vkfont_chr_width_bytes] |= prev_value << (8 - offset);
            src += vkfont_chr_width_bytes;
            dst += surface->pitch;
        }
    }
}

static void VK_InvertArea(SDL_Surface *surface, int x, int y, int w, int h)
{
    uint8_t *dst, mask1, mask2;
    int sy, sx, bytes;

    dst = ((uint8_t *)surface->pixels) + y * surface->pitch + (x >> 3);

    mask1 = 0xff >> (x & 7);
    mask2 = 0xff << ((8 - ((x + w) & 7)) & 7);

    if (w <= (8 - (x & 7)))
    {
        mask1 &= mask2;

        for (sy = 0; sy < h; sy++)
        {
            *dst ^= mask1;
            dst += surface->pitch;
        }
    }
    else if (w <= (16 - (x & 7)))
    {
        for (sy = 0; sy < h; sy++)
        {
            dst[0] ^= mask1;
            dst[1] ^= mask2;
            dst += surface->pitch;
        }
    }
    else
    {
        bytes = (w + (x & 7) - 1) >> 3;

        for (sy = 0; sy < h; sy++)
        {
            dst[0] ^= mask1;
            for (sx = 1; sx < bytes; sx++)
            {
                dst[sx] ^= 0xff;
            }
            dst[bytes] ^= mask2;
            dst += surface->pitch;
        }
    }
}

#if !SDL_VERSION_ATLEAST(2,0,0)
#if defined(ALLOW_OPENGL)
static void VK_BlitToTexturePixels(void)
{
    uint8_t *src;
    uint32_t *dst;
    int h, w, x;

    src = (uint8_t *)virtual_keyboard_surface->pixels;
    dst = virtual_keyboard_texture_pixels;
    for (h = virtual_keyboard_surface->h; h != 0; h--)
    {
        w = virtual_keyboard_surface->w >> 3;
        for (x = 0; x < w; x++)
        {
            dst[0] = (src[x] & (1 << 7)) ? 0xffffffffUL : 0x7f;
            dst[1] = (src[x] & (1 << 6)) ? 0xffffffffUL : 0x7f;
            dst[2] = (src[x] & (1 << 5)) ? 0xffffffffUL : 0x7f;
            dst[3] = (src[x] & (1 << 4)) ? 0xffffffffUL : 0x7f;
            dst[4] = (src[x] & (1 << 3)) ? 0xffffffffUL : 0x7f;
            dst[5] = (src[x] & (1 << 2)) ? 0xffffffffUL : 0x7f;
            dst[6] = (src[x] & (1 << 1)) ? 0xffffffffUL : 0x7f;
            dst[7] = (src[x] & (1 << 0)) ? 0xffffffffUL : 0x7f;
            dst += 8;
        }
        for (x = 0; x < virtual_keyboard_surface->w - (w << 3); x++)
        {
            *dst = (src[w] & (1 << (7 - x))) ? 0xffffffffUL : 0x7f;
            dst++;
        }

        src += virtual_keyboard_surface->pitch;
    }
}

static void VK_UpdateAndDrawTexture(void)
{
    static const GLfloat QuadTexCoords[2*4] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };
    GLfloat QuadVertices[2*4];

    glBindTexture(GL_TEXTURE_2D, virtual_keyboard_texture);

    QuadVertices[1] = QuadVertices[3] = virtual_keyboard_surface->h / 240.0f;
    QuadVertices[2] = QuadVertices[4] = virtual_keyboard_surface->w / 320.0f;
    QuadVertices[0] = QuadVertices[6] = - QuadVertices[2];
    QuadVertices[5] = QuadVertices[7] = - QuadVertices[1];

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, virtual_keyboard_surface->w, virtual_keyboard_surface->h, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, virtual_keyboard_texture_pixels);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glVertexPointer(2, GL_FLOAT, 0, &(QuadVertices[0]));
    glTexCoordPointer(2, GL_FLOAT, 0, &(QuadTexCoords[0]));
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glDisable(GL_BLEND);
}
#endif
static void VK_ScaleSurface(void)
{
    uint8_t *src, *dst;
    int scale_factor_x, scale_factor_y, h, y, w, x, sbit, dbit, dindex, white_value, scaled_value, cur_value;

    src = (uint8_t *)virtual_keyboard_surface->pixels;
    dst = (uint8_t *)virtual_keyboard_scaled_surface->pixels;

    scale_factor_x = virtual_keyboard_scaled_surface->w / virtual_keyboard_surface->w;
    scale_factor_y = virtual_keyboard_scaled_surface->h / virtual_keyboard_surface->h;

    if (scale_factor_x == 1)
    {
        for (h = virtual_keyboard_surface->h; h != 0; h--)
        {
            for (y = scale_factor_y; y != 0; y--)
            {
                memcpy(dst, src, virtual_keyboard_surface->pitch);
                dst += virtual_keyboard_scaled_surface->pitch;
            }
            src += virtual_keyboard_surface->pitch;
        }
    }
    else
    {
        white_value = (scale_factor_x <= 8) ? ((0xff00 >> scale_factor_x) & 0xff) : 0xff;

        for (h = virtual_keyboard_surface->h; h != 0; h--)
        {
            dbit = 0;
            cur_value = 0;
            dindex = 0;

            w = virtual_keyboard_surface->w >> 3;

            if (scale_factor_x <= 8)
            {
                for (x = 0; x < w; x++)
                {
                    for (sbit = 7; sbit >= 0; sbit--)
                    {
                        scaled_value = (src[x] & (1 << sbit)) ? white_value : 0;

                        cur_value |= scaled_value >> dbit;
                        dbit += scale_factor_x;

                        if (dbit >= 8)
                        {
                            dst[dindex] = cur_value;
                            dindex++;

                            dbit -= 8;
                            cur_value = scaled_value << (scale_factor_x - dbit);
                        }
                    }
                }

                for (sbit = 0, w = virtual_keyboard_surface->w - (w << 3); sbit < w; sbit++)
                {
                    scaled_value = (src[x] & (1 << (7 - sbit))) ? white_value : 0;

                    cur_value |= scaled_value >> dbit;
                    dbit += scale_factor_x;

                    if (dbit >= 8)
                    {
                        dst[dindex] = cur_value;
                        dindex++;

                        dbit -= 8;
                        cur_value = scaled_value << (scale_factor_x - dbit);
                    }
                }
            }
            else
            {
                for (x = 0; x < w; x++)
                {
                    for (sbit = 7; sbit >= 0; sbit--)
                    {
                        scaled_value = (src[x] & (1 << sbit)) ? 0xff : 0;

                        dst[dindex] = cur_value | (scaled_value >> dbit);
                        dindex++;

                        dbit += scale_factor_x - 8;

                        for (; dbit >= 8; dbit -= 8)
                        {
                            dst[dindex] = scaled_value;
                            dindex++;
                        }

                        cur_value = scaled_value << (8 - dbit);
                    }
                }

                for (sbit = 0, w = virtual_keyboard_surface->w - (w << 3); sbit < w; sbit++)
                {
                    scaled_value = (src[x] & (1 << (7 - sbit))) ? 0xff : 0;

                    dst[dindex] = cur_value | (scaled_value >> dbit);
                    dindex++;

                    dbit += scale_factor_x - 8;

                    for (; dbit >= 8; dbit -= 8)
                    {
                        dst[dindex] = scaled_value;
                        dindex++;
                    }

                    cur_value = scaled_value << (8 - dbit);
                }
            }

            if (dbit != 0)
            {
                dst[dindex] = cur_value;
            }

            src += virtual_keyboard_surface->pitch;
            dst += virtual_keyboard_scaled_surface->pitch;

            for (y = scale_factor_y - 1; y != 0; y--)
            {
                memcpy(dst, dst - virtual_keyboard_scaled_surface->pitch, virtual_keyboard_scaled_surface->pitch);
                dst += virtual_keyboard_scaled_surface->pitch;
            }
        }
    }
}
#endif

void VirtualKeyboard_Draw(void)
{
    SDL_Rect vkrect;
    int row, column;

    if (!VK_Visible) return;

    vkrect.w = (vkfont_chr_width_pixels + VK_INNER_BORDER * 2) * VK_LAYOUT_WIDTH + VK_OUTER_BORDER * 2;
    vkrect.h = (vkfont_chr_height + VK_INNER_BORDER * 2) * 6 + VK_OUTER_BORDER * 2;

    if (virtual_keyboard_surface == NULL)
    {
        virtual_keyboard_surface = SDL_CreateRGBSurface(0, vkrect.w, vkrect.h, 1, 0, 0, 0, 0);
        if (virtual_keyboard_surface == NULL)
        {
            return;
        }

        virtual_keyboard_surface->format->palette->colors[0].r = 0;
        virtual_keyboard_surface->format->palette->colors[0].g = 0;
        virtual_keyboard_surface->format->palette->colors[0].b = 0;
        virtual_keyboard_surface->format->palette->colors[1].r = 255;
        virtual_keyboard_surface->format->palette->colors[1].g = 255;
        virtual_keyboard_surface->format->palette->colors[1].b = 255;
#if SDL_VERSION_ATLEAST(2,0,0)
        virtual_keyboard_surface->format->palette->colors[0].a = 128;
        virtual_keyboard_surface->format->palette->colors[1].a = 255;
#endif
    }
#if SDL_VERSION_ATLEAST(2,0,0)
    if (virtual_keyboard_texture == NULL)
    {
        Uint32 format, Rmask, Gmask, Bmask, Amask;
        SDL_RendererInfo info;
        int index, bpp;

        format = SDL_PIXELFORMAT_ABGR8888;
        if (SDL_GetRendererInfo(Game_Renderer, &info) <= 0)
        {
            for (index = 0; index < (int)info.num_texture_formats; ++index)
            {
                if (info.texture_formats[index] == SDL_PIXELFORMAT_ARGB8888 ||
                    info.texture_formats[index] == SDL_PIXELFORMAT_ABGR8888 ||
                    info.texture_formats[index] == SDL_PIXELFORMAT_RGB888 ||
                    info.texture_formats[index] == SDL_PIXELFORMAT_BGR888) {
                    format = info.texture_formats[index];
                    break;
                }
            }
        }

        SDL_PixelFormatEnumToMasks(format, &bpp, &Rmask, &Gmask, &Bmask, &Amask);
        virtual_keyboard_texture_surface = SDL_CreateRGBSurfaceFrom(NULL, vkrect.w, vkrect.h, bpp, 0, Rmask, Gmask, Bmask, Amask);
        if (virtual_keyboard_texture_surface == NULL)
        {
            return;
        }

        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, (Game_ScalingQuality)?"linear":"nearest");

        virtual_keyboard_texture = SDL_CreateTexture(Game_Renderer, format, SDL_TEXTUREACCESS_STREAMING, vkrect.w, vkrect.h);
        if (virtual_keyboard_texture == NULL)
        {
            SDL_FreeSurface(virtual_keyboard_texture_surface);
            virtual_keyboard_texture_surface = NULL;
            return;
        }

        if (Amask)
        {
            SDL_SetTextureBlendMode(virtual_keyboard_texture, SDL_BLENDMODE_BLEND);
        }
    }
#else
#if defined(ALLOW_OPENGL)
    if (Game_UseOpenGL)
    {
        if (virtual_keyboard_texture_pixels == NULL)
        {
            virtual_keyboard_texture_pixels = (uint32_t *)malloc(vkrect.w * vkrect.h * sizeof(uint32_t));
            if (virtual_keyboard_texture_pixels == NULL)
            {
                return;
            }
        }

        if (virtual_keyboard_texture == 0)
        {
            // flush GL errors
            while(glGetError() != GL_NO_ERROR);

            glGenTextures(1, &virtual_keyboard_texture);

            glBindTexture(GL_TEXTURE_2D, virtual_keyboard_texture);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (Game_ScalingQuality)?GL_LINEAR:GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (Game_ScalingQuality)?GL_LINEAR:GL_NEAREST);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, vkrect.w, vkrect.h, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, virtual_keyboard_texture_pixels);

            if (glGetError() != GL_NO_ERROR)
            {
                glBindTexture(GL_TEXTURE_2D, 0);
                glDeleteTextures(1, &virtual_keyboard_texture);
                virtual_keyboard_texture = 0;
                return;
            }
        }
    }
    else
#endif
    {
        if (virtual_keyboard_scaled_surface == NULL)
        {
            int scale_factor_x, scale_factor_y;

            scale_factor_x = Picture_Width / 320;
            if (scale_factor_x == 0) scale_factor_x = 1;
            scale_factor_y = Picture_Height / 240;
            if (scale_factor_y == 0) scale_factor_y = 1;

            if ((scale_factor_x * 320 < Picture_Width) && (scale_factor_y * 240 < Picture_Height))
            {
                if (((scale_factor_x + 1) * vkrect.w < Picture_Width) && ((scale_factor_y + 1) * vkrect.h < Picture_Height))
                {
                    scale_factor_x++;
                    scale_factor_y++;
                }
            }

            if ((scale_factor_x == 1) && (scale_factor_y == 1))
            {
                virtual_keyboard_scaled_surface = virtual_keyboard_surface;
            }
            else
            {
                virtual_keyboard_scaled_surface = SDL_CreateRGBSurface(0, vkrect.w * scale_factor_x, vkrect.h * scale_factor_y, 1, 0, 0, 0, 0);
                if (virtual_keyboard_scaled_surface == NULL)
                {
                    return;
                }

                virtual_keyboard_scaled_surface->format->palette->colors[0] = virtual_keyboard_surface->format->palette->colors[0];
                virtual_keyboard_scaled_surface->format->palette->colors[1] = virtual_keyboard_surface->format->palette->colors[1];
            }
        }
    }
#endif

    memset(virtual_keyboard_surface->pixels, 0, virtual_keyboard_surface->h * virtual_keyboard_surface->pitch);

    for (row = 0; row < 4; row++)
    {
        for (column = 0; column < VK_LAYOUT_WIDTH; column++)
        {
            VK_DrawCharacter(virtual_keyboard_surface, virtual_keyboard_layout[virtual_keyboard_altlayout][row][column], (vkfont_chr_width_pixels + VK_INNER_BORDER * 2) * column + VK_INNER_BORDER + VK_OUTER_BORDER, (vkfont_chr_height + VK_INNER_BORDER * 2) * row + VK_INNER_BORDER + VK_OUTER_BORDER);
        }
    }

    if (virtual_keyboard_buflen <= VK_LAYOUT_WIDTH)
    {
        for (column = 0; column < virtual_keyboard_buflen; column++)
        {
            VK_DrawCharacter(virtual_keyboard_surface, virtual_keyboard_buffer[column], (vkfont_chr_width_pixels + VK_INNER_BORDER * 2) * column + VK_INNER_BORDER + VK_OUTER_BORDER, (vkfont_chr_height + VK_INNER_BORDER * 2) * 5 + VK_INNER_BORDER + VK_OUTER_BORDER);
        }
    }
    else
    {
        for (column = 0; column < VK_LAYOUT_WIDTH; column++)
        {
            VK_DrawCharacter(virtual_keyboard_surface, virtual_keyboard_buffer[column], (vkfont_chr_width_pixels + VK_INNER_BORDER * 2) * column + VK_INNER_BORDER + VK_OUTER_BORDER, (vkfont_chr_height + VK_INNER_BORDER * 2) * 4 + VK_INNER_BORDER + VK_OUTER_BORDER);
        }
        for (column = VK_LAYOUT_WIDTH; column < virtual_keyboard_buflen; column++)
        {
            VK_DrawCharacter(virtual_keyboard_surface, virtual_keyboard_buffer[column], (vkfont_chr_width_pixels + VK_INNER_BORDER * 2) * (column - VK_LAYOUT_WIDTH) + VK_INNER_BORDER + VK_OUTER_BORDER, (vkfont_chr_height + VK_INNER_BORDER * 2) * 5 + VK_INNER_BORDER + VK_OUTER_BORDER);
        }
    }

    if ((virtual_keyboard_pos_y == 3) && (virtual_keyboard_pos_x >= (VK_LAYOUT_WIDTH - 2)))
    {
        VK_InvertArea(virtual_keyboard_surface, (vkfont_chr_width_pixels + VK_INNER_BORDER * 2) * (VK_LAYOUT_WIDTH - 2) + VK_OUTER_BORDER, (vkfont_chr_height + VK_INNER_BORDER * 2) * 3 + VK_OUTER_BORDER, (vkfont_chr_width_pixels + VK_INNER_BORDER * 2) * 2, vkfont_chr_height + VK_INNER_BORDER * 2);
    }
    else
    {
        VK_InvertArea(virtual_keyboard_surface, (vkfont_chr_width_pixels + VK_INNER_BORDER * 2) * virtual_keyboard_pos_x + VK_OUTER_BORDER, (vkfont_chr_height + VK_INNER_BORDER * 2) * virtual_keyboard_pos_y + VK_OUTER_BORDER, vkfont_chr_width_pixels + VK_INNER_BORDER * 2, vkfont_chr_height + VK_INNER_BORDER * 2);
    }

#if SDL_VERSION_ATLEAST(2,0,0)
    if (SDL_LockTexture(virtual_keyboard_texture, NULL, &virtual_keyboard_texture_surface->pixels, &virtual_keyboard_texture_surface->pitch) >= 0)
    {
        SDL_UpperBlit(virtual_keyboard_surface, NULL, virtual_keyboard_texture_surface, NULL);
        SDL_UnlockTexture(virtual_keyboard_texture);
    }

    vkrect.w = (vkrect.w * Picture_Width) / 640;
    vkrect.h = (vkrect.h * Picture_Height) / 480;
    vkrect.x = (Picture_Width - vkrect.w) >> 1;
    vkrect.y = (Picture_Height - vkrect.h) >> 1;

    SDL_RenderCopy(Game_Renderer, virtual_keyboard_texture, NULL, &vkrect);
#else
#if defined(ALLOW_OPENGL)
    if (Game_UseOpenGL)
    {
        VK_BlitToTexturePixels();
        VK_UpdateAndDrawTexture();
    }
    else
#endif
    {
        if (virtual_keyboard_scaled_surface == virtual_keyboard_surface)
        {
            vkrect.x = (Picture_Width - vkrect.w) >> 1;
            vkrect.y = (Picture_Height - vkrect.h) >> 1;

            SDL_UpperBlit(virtual_keyboard_surface, NULL, Game_Screen, &vkrect);
        }
        else
        {
            VK_ScaleSurface();

            vkrect.w = virtual_keyboard_scaled_surface->w;
            vkrect.h = virtual_keyboard_scaled_surface->h;
            vkrect.x = (Picture_Width - vkrect.w) >> 1;
            vkrect.y = (Picture_Height - vkrect.h) >> 1;

            SDL_UpperBlit(virtual_keyboard_scaled_surface, NULL, Game_Screen, &vkrect);
        }
    }
#endif
}

void VirtualKeyboard_Event(SDL_Event *event)
{
    const static uint8_t vk_shift_table[128] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*   0- 15 */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*  16- 31 */
        0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, /*  32- 47 */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, /*  48- 63 */
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*  64- 79 */
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, /*  80- 95 */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*  96-111 */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, /* 112-127 */
    };

    int change_shift, index;

    switch(event->type)
    {
        case SDL_KEYDOWN:
            switch (event->key.keysym.sym)
            {
                case SDLK_UP:
                    virtual_keyboard_pos_y = (virtual_keyboard_pos_y) ? virtual_keyboard_pos_y - 1 : 3;
                    break;
                case SDLK_DOWN:
                    virtual_keyboard_pos_y = (virtual_keyboard_pos_y < 3) ? virtual_keyboard_pos_y + 1 : 0;
                    break;
                case SDLK_LEFT:
                    if ((virtual_keyboard_pos_y == 3) && (virtual_keyboard_pos_x >= (VK_LAYOUT_WIDTH - 2)))
                    {
                        virtual_keyboard_pos_x = (VK_LAYOUT_WIDTH - 3);
                    }
                    else
                    {
                        virtual_keyboard_pos_x = (virtual_keyboard_pos_x) ? virtual_keyboard_pos_x - 1 : (VK_LAYOUT_WIDTH - 1);
                    }
                    break;
                case SDLK_RIGHT:
                    if ((virtual_keyboard_pos_y == 3) && (virtual_keyboard_pos_x >= (VK_LAYOUT_WIDTH - 2)))
                    {
                        virtual_keyboard_pos_x = 0;
                    }
                    else
                    {
                        virtual_keyboard_pos_x = (virtual_keyboard_pos_x < (VK_LAYOUT_WIDTH - 1)) ? virtual_keyboard_pos_x + 1 : 0;
                    }
                    break;

                case SDLK_RETURN:
                    if ((virtual_keyboard_pos_y == 3) && (virtual_keyboard_pos_x >= (VK_LAYOUT_WIDTH - 3)))
                    {
                        if (virtual_keyboard_pos_x >= (VK_LAYOUT_WIDTH - 2))
                        {
                            VK_Visible = 0;

                            if (virtual_keyboard_buflen)
                            {
                                EmulateKey(SDL_KEYUP, SDLK_RSHIFT);
                                change_shift = 1;

                                for (index = 0; index < virtual_keyboard_buflen; index++)
                                {
                                    if (change_shift)
                                    {
                                        EmulateKey((vk_shift_table[virtual_keyboard_buffer[index]]) ? SDL_KEYDOWN : SDL_KEYUP, SDLK_LSHIFT);
                                    }

                                    EmulateKey(SDL_KEYDOWN, virtual_keyboard_buffer[index]);
                                    EmulateKey(SDL_KEYUP, virtual_keyboard_buffer[index]);

                                    if (index + 1 < virtual_keyboard_buflen)
                                    {
                                        change_shift = (vk_shift_table[virtual_keyboard_buffer[index]] != vk_shift_table[virtual_keyboard_buffer[index + 1]]) ? 1 : 0;
                                    }
                                    else
                                    {
                                        if (vk_shift_table[virtual_keyboard_buffer[index]])
                                        {
                                            EmulateKey(SDL_KEYUP, SDLK_LSHIFT);
                                        }
                                    }
                                }
                            }
                        }
                        else
                        {
                            if (virtual_keyboard_buflen)
                            {
                                virtual_keyboard_buflen--;
                            }
                        }
                    }
                    else
                    {
                        if (virtual_keyboard_buflen < 2 * VK_LAYOUT_WIDTH)
                        {
                            virtual_keyboard_buffer[virtual_keyboard_buflen] = virtual_keyboard_layout[virtual_keyboard_altlayout][virtual_keyboard_pos_y][virtual_keyboard_pos_x];
                            virtual_keyboard_buflen++;
                        }
                    }
                    break;
                case SDLK_BACKSPACE:
                    if (virtual_keyboard_buflen)
                    {
                        virtual_keyboard_buflen--;
                    }
                    break;

                case SDLK_TAB:
                    virtual_keyboard_altlayout = 1 - virtual_keyboard_altlayout;
                    break;
                case SDLK_F15:
                    VK_Visible = 0;
                    break;

                default:
                    break;
            }
            break;
        default:
            break;
    }
}

