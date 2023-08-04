/**
 *
 *  Copyright (C) 2016-2023 Roman Pauer
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

#if !defined(_XCOM_SOUND_H_INCLUDED_)
#define _XCOM_SOUND_H_INCLUDED_

struct _DIGPAK_SNDSTRUC_;

extern void Game_ChannelFinished(int channel);
extern int16_t Game_ProcessAudio(void);

#ifdef __cplusplus
extern "C" {
#endif

// note: parameters smaller than 32 bits are extended to 32 bits by callers in the original asm code
// note: return values smaller than 32 bits are not expected by callers in the original asm code to have been extended to 32 bits

extern int16_t Game_DigPlay(struct _DIGPAK_SNDSTRUC_ *sndplay);
extern int16_t Game_AudioCapabilities(void);
extern void Game_StopSound(void);
extern int16_t Game_PostAudioPending(struct _DIGPAK_SNDSTRUC_ *sndplay);
extern int16_t Game_SetPlayMode(int16_t playmode);
extern int16_t *Game_PendingAddress(void);
extern int16_t *Game_ReportSemaphoreAddress(void);
extern int16_t Game_SetBackFillMode(int16_t mode);
extern int16_t Game_VerifyDMA(char *data, int16_t length);
extern void Game_SetDPMIMode(int16_t mode);
extern int32_t Game_FillSoundCfg(void *buf, int32_t count);
extern uint32_t Game_RealPtr(uint32_t ptr);

#ifdef __cplusplus
}
#endif

#endif /* _XCOM_SOUND_H_INCLUDED_ */
