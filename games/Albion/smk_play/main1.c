/**
 *
 *  Copyright (C) 2016-2025 Roman Pauer
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include "smack.h"
#include "smk_defs.h"

#if defined(_MSC_VER)
    #define strncasecmp _strnicmp
#endif

typedef void (*change_vol_func)(uint8_t *dst, uint8_t *src, int len);

#if defined(GP2X) && defined(__GNUC__)
static void __attribute__ ((noinline, naked)) BufferToScreen320x240x16Asm(uint16_t *dst, uint8_t *src, uint32_t *pal, uint32_t Height, uint32_t Width, uint32_t WidthDiff)
{
//input:
// r0 - dst
// r1 - src
// r2 - palette
// r3 - Height
// [sp] - Width
// [sp+4] - screen->w
//
//temp:
// ip - Width
// lr - screen->w - Width
// v1 - horizontal counter
// v2 - *src
// v3 - *pal
// v4 - *dst
// v5 - *dst2
	asm(
		"stmfd sp!, {v1-v5, lr}" "\n"
		"ldr ip, [sp, #(6*4)]" "\n"
		"ldr lr, [sp, #(7*4)]" "\n"

		"rsb v1, r3, #240" "\n"
		"mul v2, v1, lr" "\n"
		"add r0, r0, v2" "\n"
		"rsb v1, ip, #320" "\n"
		"add r0, r0, v1" "\n"

		"sub lr, lr, ip" "\n"

		"1:" "\n"
		"mov v1, ip, lsr #2" "\n"

		"2:" "\n"
		"ldr v2, [r1], #4" "\n"
		"and v4, v2, #0x00ff" "\n"
		"ldr v4, [r2, v4, lsl #2]" "\n"

		"and v3, v2, #0x00ff00" "\n"
		"ldr v3, [r2, v3, lsr #6]" "\n"
		"orr v4, v4, v3, lsl #16" "\n"

		"and v5, v2, #0x00ff0000" "\n"
		"ldr v5, [r2, v5, lsr #14]" "\n"

		"mov v3, v2, lsr #24" "\n"
		"ldr v3, [r2, v3, lsl #2]" "\n"
		"orr v5, v5, v3, lsl #16" "\n"

		"stmia r0!, {v4, v5}" "\n"

		"subS v1, v1, #1" "\n"
		"bne 2b" "\n"

		"add r0, r0, lr" "\n"

		"subS r3, r3, #1" "\n"
		"bne 1b" "\n"

		//exit
		"ldmfd sp!, {v1-v5, pc}" "\n"
	);
}

static void __attribute__ ((noinline, naked)) BufferToScreen320x240x16DoubleAsm(uint16_t *dst, uint8_t *src, uint32_t *pal, uint32_t Height, uint32_t Width, uint32_t WidthDiff)
{
//input:
// r0 - dst
// r1 - src
// r2 - palette
// r3 - Height
// [sp] - Width
// [sp+4] - screen->w
//
//temp:
// ip - Width
// lr - screen->w - Width
// v1 - horizontal counter
// v2 - *src
// v3 - *pal
// v4 - *dst
// v5 - *dst2
// v6 - dst+w
	asm(
		"stmfd sp!, {v1-v6, lr}" "\n"
		"ldr ip, [sp, #(7*4)]" "\n"
		"ldr lr, [sp, #(8*4)]" "\n"

		"rsb v1, r3, #120" "\n"
		"mul v2, v1, lr" "\n"
		"add r0, r0, v2, lsl #1" "\n"
		"rsb v1, ip, #320" "\n"
		"add r0, r0, v1" "\n"

		"add v6, r0, lr" "\n"

		"rsb lr, ip, lr, lsl #1" "\n"

		"1:" "\n"
		"mov v1, ip, lsr #2" "\n"

		"2:" "\n"
		"ldr v2, [r1], #4" "\n"
		"and v4, v2, #0x00ff" "\n"
		"ldr v4, [r2, v4, lsl #2]" "\n"

		"and v3, v2, #0x00ff00" "\n"
		"ldr v3, [r2, v3, lsr #6]" "\n"
		"orr v4, v4, v3, lsl #16" "\n"

		"and v5, v2, #0x00ff0000" "\n"
		"ldr v5, [r2, v5, lsr #14]" "\n"

		"mov v3, v2, lsr #24" "\n"
		"ldr v3, [r2, v3, lsl #2]" "\n"
		"orr v5, v5, v3, lsl #16" "\n"

		"stmia r0!, {v4, v5}" "\n"
		"stmia v6!, {v4, v5}" "\n"

		"subS v1, v1, #1" "\n"
		"bne 2b" "\n"

		"add r0, r0, lr" "\n"
		"add v6, v6, lr" "\n"

		"subS r3, r3, #1" "\n"
		"bne 1b" "\n"

		//exit
		"ldmfd sp!, {v1-v6, pc}" "\n"
	);
}
#endif

SmackStruct *Smack;
SmackFrame *Frame;

int audio_track;					/* audio track to be played */
int frame_delay;					/* frame delay (in ms) */
int quit;
int audio_volume;					/* volume 0-128 */
change_vol_func change_volume;

SDL_Surface *screen;

pixel_format palette_out[256];

uint8_t *audio_buf;
uint32_t audio_len, audio_read, audio_write;

static int initialize(void)
{

	/* clear values */
	Frame = 0;
	audio_buf = NULL;

	/* set initial values */
	quit = 0;

	/* fill values */

	/* allocate framebuffer with sound */
	Frame = SmackAllocateFrame(Smack, NULL, 0, 0, (audio_track == -1)?0:1, 0, 0, 0);

	if (Frame == NULL) return 11;

	/* open display */
#if (DISPLAY_RES == DR_640x480x32)

	#if (DISPLAY_FULLSCREEN == 0)
	screen = SDL_SetVideoMode (640, 480, 32, SDL_SWSURFACE);
	#else
	screen = SDL_SetVideoMode (640, 480, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN | SDL_NOFRAME);
	#endif
#elif (DISPLAY_RES == DR_320x240x16)
	#if (DISPLAY_FULLSCREEN == 0)
	screen = SDL_SetVideoMode (320, 240, 16, SDL_SWSURFACE);
	#else
	screen = SDL_SetVideoMode (320, 240, 16, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN | SDL_NOFRAME);
	#endif
#elif (DISPLAY_RES == DR_800x480x16)
	#if (DISPLAY_FULLSCREEN == 0)
	screen = SDL_SetVideoMode (800, 480, 16, SDL_SWSURFACE);
	#else
	screen = SDL_SetVideoMode (800, 480, 16, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN | SDL_NOFRAME);
	#endif

#else
	#error no display defined
#endif

	if (screen == NULL) return 12;
	SDL_WM_SetCaption ("SDL Smackply", NULL);
	SDL_ShowCursor(0);

	/* allocate audio buffer */
	if (audio_track != -1)
	{
		audio_len = 3 * Smack->AudioSize[audio_track];
		audio_buf = (uint8_t *) malloc(audio_len);

		if (audio_buf == NULL) return 13;
	}

	return 0;
}

static void deinitialize(void)
{
	/* stop audio */
	if (audio_track != -1)
	{
		if (audio_buf != NULL)
		{
			SDL_PauseAudio(1);
		}
		SDL_CloseAudio();
	}

	/* deallocate audio buffer */
	if (audio_buf != NULL)
	{
		free(audio_buf);
	}

	/* close display */
	if (screen != NULL)
	{
		SDL_LockSurface(screen);
#if (DISPLAY_RES == DR_640x480x32)
		memset(screen->pixels, 0, 640*480*4);
#elif (DISPLAY_RES == DR_320x240x16)
		memset(screen->pixels, 0, 320*240*2);
#elif (DISPLAY_RES == DR_800x480x16)
		memset(screen->pixels, 0, 800*480*2);
#endif
		SDL_UnlockSurface(screen);
		SDL_Flip(screen);
		SDL_FreeSurface(screen);
	}

	/* deallocate framebuffer */
	if (Frame != NULL)
	{
		SmackDeallocateFrame(Frame);
	}

	SDL_Quit();
	fclose(Smack->File);
	SmackClose(Smack);
}

static void BufferToScreen(void)
{
	/* set palette */
	{
		int j;

		if (Smack->FrameTypes[Smack->CurrentFrame] & 1)
		{
			for (j = 0; j < 256; j++)
			{
#if (DISPLAY_RES == DR_640x480x32)
				palette_out[j].s.b = (*(Frame->Palette))[j].b;
				palette_out[j].s.g = (*(Frame->Palette))[j].g;
				palette_out[j].s.r = (*(Frame->Palette))[j].r;
				palette_out[j].s.a = 0;
#elif (DISPLAY_RES == DR_320x240x16) || (DISPLAY_RES == DR_800x480x16)
				palette_out[j].s.b = (*(Frame->Palette))[j].b >> 3;
				palette_out[j].s.g = (*(Frame->Palette))[j].g >> 2;
				palette_out[j].s.r = (*(Frame->Palette))[j].r >> 3;
				palette_out[j].s.a = 0;
#endif
			}
		}
	}

	/* display frame */
#if (DISPLAY_RES == DR_640x480x32)
	{
		unsigned int x, y, w;
		uint8_t *psrc;
		uint32_t *pdst;
		uint32_t Width, Height;

		Width = Smack->PaddedWidth;
		Height = Smack->PaddedHeight;

		SDL_LockSurface(screen);
		w = screen->w;

		pdst = (uint32_t *) &(( (uint32_t *) screen->pixels )[ (240 - ((Smack->Flags & 4)?2:1) * Height) * w + 320 - Width ]);
		psrc = Frame->Video;

		if (Smack->Flags & 4)
		{
			for (y = 0; y < Height; y++)
			{
				for (x = 0; x < Width; x++)
				{
					uint32_t pixel;

					pixel = palette_out[*psrc].pix;

					pdst[0] = pixel;
					pdst[1] = pixel;
					pdst[w] = pixel;
					pdst[w+1] = pixel;

					pdst[2*w] = pixel;
					pdst[2*w+1] = pixel;
					pdst[3*w] = pixel;
					pdst[3*w+1] = pixel;

					psrc++;
					pdst+=2;
				}
				pdst+=(2*w - Width) * 2;
			}
		}
		else
		{
			for (y = 0; y < Height; y++)
			{
				for (x = 0; x < Width; x++)
				{
					uint32_t pixel;

					pixel = palette_out[*psrc].pix;

					pdst[0] = pixel;
					pdst[1] = pixel;
					pdst[w] = pixel;
					pdst[w+1] = pixel;

					psrc++;
					pdst+=2;
				}
				pdst+=(w - Width) * 2;
			}
		}
		SDL_UnlockSurface(screen);
	}
#elif (DISPLAY_RES == DR_320x240x16)
	{
		SDL_LockSurface(screen);
		if (Smack->Flags & 4)
		{
#if defined(GP2X) && defined(__GNUC__)
			BufferToScreen320x240x16DoubleAsm
			(
				(uint16_t *) screen->pixels,
				Frame->Video,
				(uint32_t *) palette_out,
				Smack->PaddedHeight,
				Smack->PaddedWidth,
				screen->w
			);
#else
			unsigned int x, y, w;
			uint8_t *psrc;
			uint16_t *pdst;
			uint32_t Width, Height;

			Width = Smack->PaddedWidth;
			Height = Smack->PaddedHeight;

			SDL_LockSurface(screen);
			w = screen->w;

			pdst = (uint16_t *) &(( (uint16_t *) screen->pixels )[ ( (240 - 2 * Height) * w + 320 - Width ) / 2 ]);
			psrc = Frame->Video;


			for (y = 0; y < Height; y++)
			{
				for (x = 0; x < Width; x++)
				{
					uint32_t pixel;

					pixel = palette_out[*psrc].pix;

					pdst[0] = pixel;
					pdst[w] = pixel;

					psrc++;
					pdst++;
				}
				pdst+=(2*w - Width);
			}
#endif
		}
		else
		{
#if defined(GP2X) && defined(__GNUC__)
			BufferToScreen320x240x16Asm
			(
				(uint16_t *) screen->pixels,
				Frame->Video,
				(uint32_t *) palette_out,
				Smack->PaddedHeight,
				Smack->PaddedWidth,
				screen->w
			);
#else
			unsigned int x, y, w;
			uint8_t *psrc;
			uint16_t *pdst;
			uint32_t Width, Height;

			Width = Smack->PaddedWidth;
			Height = Smack->PaddedHeight;

			w = screen->w;

			pdst = (uint16_t *) &(( (uint16_t *) screen->pixels )[ ( (240 - Height) * w + 320 - Width ) / 2 ]);
			psrc = Frame->Video;


			for (y = 0; y < Height; y++)
			{
				for (x = 0; x < Width; x++)
				{
					*pdst = palette_out[*psrc].pix;

					psrc++;
					pdst++;
				}
				pdst+=(w - Width);
			}
#endif
		}
		SDL_UnlockSurface(screen);
	}
#elif (DISPLAY_RES == DR_800x480x16)
	{
		unsigned int x, y, w;
		uint8_t *psrc;
		uint16_t *pdst;
		uint32_t Width, Height;

		Width = Smack->PaddedWidth;
		Height = Smack->PaddedHeight;

		SDL_LockSurface(screen);
		w = screen->w;

		pdst = (uint16_t *) &(( (uint16_t *) screen->pixels )[ (240 - ((Smack->Flags & 4)?2:1) * Height) * w + 400 - Width ]);
		psrc = Frame->Video;

		if (Smack->Flags & 4)
		{
			for (y = 0; y < Height; y++)
			{
				for (x = 0; x < Width; x++)
				{
					uint32_t pixel;

					pixel = palette_out[*psrc].pix;

					pdst[0] = pixel;
					pdst[1] = pixel;
					pdst[w] = pixel;
					pdst[w+1] = pixel;

					pdst[2*w] = pixel;
					pdst[2*w+1] = pixel;
					pdst[3*w] = pixel;
					pdst[3*w+1] = pixel;

					psrc++;
					pdst+=2;
				}
				pdst+=(2*w - Width) * 2;
			}
		}
		else
		{
			for (y = 0; y < Height; y++)
			{
				for (x = 0; x < Width; x++)
				{
					uint32_t pixel;

					pixel = palette_out[*psrc].pix;

					pdst[0] = pixel;
					pdst[1] = pixel;
					pdst[w] = pixel;
					pdst[w+1] = pixel;

					psrc++;
					pdst+=2;
				}
				pdst+=(w - Width) * 2;
			}
		}
		SDL_UnlockSurface(screen);
	}
#endif
}

static void change_volume_u8(uint8_t *dst, uint8_t *src, int len)
{
	int volume;

	volume = (audio_volume << 17);
	for (; len != 0; len--)
	{
		*dst = (uint8_t) ((((((int) (*src)) - 128) * volume) + 0x80000000) >> 24);
		src++;
		dst++;
	}
}

#if (SDL_BYTEORDER == SDL_LIL_ENDIAN)

static void change_volume_s16lsb_nat(int16_t *dst, int16_t *src, int len)
{
	int volume;

	volume = (audio_volume << 9);
	for (len &=(~1); len != 0; len-=2)
	{
		*dst = (int16_t) (((int) (*src) * volume) >> 16);
		src++;
		dst++;
	}
}

#else

static void change_volume_s16lsb_nonnat(int8_t *dst, int8_t *src, int len)
{
	int volume;

	volume = (audio_volume << 9);
	for (len &=(~1); len != 0; len-=2)
	{
		int val;
		val = ((((int) src[0]) << 8) | ((int) (((uint8_t *) src)[1]))) * volume;
		dst[0] = (int8_t) (val >> 24);
		dst[1] = (int8_t) (val >> 16);
		src+=2;
		dst+=2;
	}
}

#endif

/* callback procedure for filling audio data */
static void fill_audio(void *udata, Uint8 *stream, int len)
{
	int len2;

	if (audio_volume == 128)
	{
		if (audio_read + len > audio_len)
		{
			len2 = audio_len - audio_read;

			memcpy(stream, &(audio_buf[audio_read]), len2);

			audio_read = len - len2;

			memcpy(&(stream[len2]), audio_buf, audio_read);
		}
		else
		{
			memcpy(stream, &(audio_buf[audio_read]), len);
			audio_read+=len;
			if (audio_read >= audio_len) audio_read-=audio_len;
		}
	}
	else
	{
		if (audio_read + len > audio_len)
		{
			len2 = audio_len - audio_read;

			change_volume(stream, &(audio_buf[audio_read]), len2);

			audio_read = len - len2;

			change_volume(&(stream[len2]), audio_buf, audio_read);
		}
		else
		{
			change_volume(stream, &(audio_buf[audio_read]), len);
			audio_read+=len;
			if (audio_read >= audio_len) audio_read-=audio_len;
		}
	}
}

int main(int argc, char *argv[])
{
	Uint32 lastticks, ticks;
	int delay;
	SDL_Event event;

	if (argc < 2) return 1;

	audio_volume = 128;

	/* check input file */
	{
		char *filename;
		FILE *file;

		filename = NULL;
		argv++;
		argc--;
		while (argc)
		{
			if ((*argv)[0] == '-')
			{
				if (strncasecmp(*argv, "--volume=", 9) == 0)
				{
					audio_volume = atoi(&((*argv)[9]));

					if (audio_volume < 0) audio_volume = 0;
					else if (audio_volume > 128) audio_volume = 128;
				}
			}
			else
			{
				if (filename == NULL) filename = *argv;
			}
			argv++;
			argc--;
		} ;

		/* open file */
		file = fopen(filename, "rb");
		if (file == NULL) return 2;

		/* open Smacker file */
		Smack = SmackOpen(file);
		if (Smack == NULL)
		{
			fclose(file);
			return 3;
		}
	}

	/* check smack parameters */
	if (Smack->PaddedWidth == 0 ||
		Smack->PaddedWidth > 320 ||
		Smack->PaddedHeight == 0 ||
		Smack->PaddedHeight > ( (Smack->Flags & 4)?120u:240u ) ||
		Smack->Frames == 0)
	{
		fclose(Smack->File);
		SmackClose(Smack);
		return 4;
	}

	/* choose audio track */
	{
		int i;

		audio_track = -1;

		for (i = 0; i < 7; i++)
		{
			if (Smack->AudioSize[i] != 0 &&
				(Smack->AudioRate[i] & 0x40000000) != 0) /* data present */
			{
				audio_track = i;
				break;
			}
		}
	}

	/* Initialize SDL */
	{
		Uint32 flags;

		flags = (audio_track == -1)?
				(SDL_INIT_VIDEO | SDL_INIT_TIMER
#ifdef GP2X
				| SDL_INIT_JOYSTICK
#endif
				):
				(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO
#ifdef GP2X
				| SDL_INIT_JOYSTICK
#endif
				);

		if (SDL_Init (flags) < 0)
		{
			fclose(Smack->File);
			SmackClose(Smack);
			return 5;
		}

#ifdef GP2X
		SDL_JoystickOpen(0);
#endif
	}

	/* Initialize SDL Audio */
	if (audio_track != -1)
	{
	    SDL_AudioSpec wanted;

		/* Set the audio format*/
	    wanted.freq = Smack->AudioRate[audio_track] & 0x00ffffff;
	    wanted.format = (Smack->AudioRate[audio_track] & 0x20000000)?AUDIO_S16LSB:AUDIO_U8;
	    wanted.channels = (Smack->AudioRate[audio_track] & 0x10000000)?2:1;
	    wanted.samples = 2048;
	    wanted.callback = fill_audio;
	    wanted.userdata = Smack;

	    if (Smack->AudioRate[audio_track] & 0x20000000)
	    {
#if (SDL_BYTEORDER == SDL_LIL_ENDIAN)
			change_volume = (change_vol_func) change_volume_s16lsb_nat;
#else
			change_volume = (change_vol_func) change_volume_s16lsb_nonnat;
#endif
	    }
	    else
	    {
	    	change_volume = change_volume_u8;
	    }

	    if ( SDL_OpenAudio(&wanted, NULL) < 0 )
	    {
	    	audio_track = -1;
		}
	}

	/* more initialization */
	{
		int return_value;

		return_value = initialize();
		if (return_value)
		{
			deinitialize();
			return return_value;
		}
	}

	/* calculate frame delay (in ms) */
	{
		if (Smack->FrameRate > 0) frame_delay = Smack->FrameRate;
		else if (Smack->FrameRate < 0) frame_delay = Smack->FrameRate / -100;
		else frame_delay = 100;
	}

	/* prebuffer frames */
	{
		int error;

		SmackDecodeFrame(Smack, Frame, Frame, audio_track);

		error = SmackError();
		if ( error )
		{
			deinitialize();
			return error;
		}

		BufferToScreen();

		if (audio_track != -1)
		{
			audio_read = 0;
			audio_write = Frame->AudioLength;

			memcpy(audio_buf, Frame->Audio, audio_write);
		}

		SmackNextFrame(Smack);
	}

	/* start playing */
	if (audio_track != -1)
	{
		SDL_PauseAudio(0);
	}
	lastticks = SDL_GetTicks();
	delay = 0;

	while (!quit)
	{
		/* check events */
		if (SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_KEYUP:
					if (event.key.keysym.sym == SDLK_ESCAPE)
					{
						quit = 1;
					}
					else if (event.key.keysym.sym == SDLK_KP_MINUS)
					{
						audio_volume -= (audio_volume > 24)?8:4;
						if (audio_volume < 0) audio_volume = 0;
					}
					else if (event.key.keysym.sym == SDLK_KP_PLUS)
					{
						audio_volume += (audio_volume < 24)?4:8;
						if (audio_volume > 128) audio_volume = 128;
					}
#ifdef PANDORA
					else if ((event.key.keysym.sym == SDLK_LALT) ||   // PANDORA_BUTTON_START
                             (event.key.keysym.sym == SDLK_PAGEDOWN)) // PANDORA_BUTTON_X
					{
						quit = 1;
					}
#endif

					break;
					// case SDL_KEYUP: ...
#ifdef GP2X
				case SDL_JOYBUTTONUP:
					if (event.jbutton.button == GP2X_BUTTON_START ||
						event.jbutton.button == GP2X_BUTTON_X)
					{
						quit = 1;
					}
					else if (event.jbutton.button == GP2X_BUTTON_VOLUP)
					{
						audio_volume += (audio_volume < 24)?4:8;
						if (audio_volume > 128) audio_volume = 128;
					}
					else if (event.jbutton.button == GP2X_BUTTON_VOLDOWN)
					{
						audio_volume -= (audio_volume > 24)?8:4;
						if (audio_volume < 0) audio_volume = 0;
					}

					break;
					// case SDL_JOYBUTTONUP: ...
#endif
				case SDL_QUIT:
					// todo: question
					quit = 1;

					break;
			} // switch(event.type)

			if (quit) break;
		}

		ticks = SDL_GetTicks();
		delay -= (ticks - lastticks);
		lastticks = ticks;

		if (delay > 0)
		{
			SDL_Delay(1);
		}
		else
		{
			delay += frame_delay;

			if (Smack->CurrentFrame == 0)
			{
				quit = 1;
			}
			else
			{
				int error;

				SDL_Flip(screen);

				SmackDecodeFrame(Smack, Frame, Frame, audio_track);

				error = SmackError();
				if ( error )
				{
					deinitialize();
					return error;
				}

				BufferToScreen();

				if (audio_track != -1)
				{
					unsigned int frame_audio_len, frame_audio_len2;

					frame_audio_len = Frame->AudioLength;

					if (frame_audio_len != 0)
					{
						if (audio_write + frame_audio_len > audio_len)
						{
							frame_audio_len2 = audio_len - audio_write;

							memcpy(&(audio_buf[audio_write]), Frame->Audio, frame_audio_len2);

							audio_write = frame_audio_len - frame_audio_len2;

							memcpy(audio_buf, &(Frame->Audio[frame_audio_len2]), audio_write);
						}
						else
						{
							memcpy(&(audio_buf[audio_write]), Frame->Audio, frame_audio_len);
							audio_write+=frame_audio_len;
							if (audio_write >= audio_len) audio_write-=audio_len;
						}

					}
				}


				SmackNextFrame(Smack);
			}

		}

	};

	deinitialize();

	return 0;
}
