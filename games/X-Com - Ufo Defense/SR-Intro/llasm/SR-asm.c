#include "llasm_cpu.h"

extern volatile uint32_t Game_SDLTicks;
extern volatile uint32_t Game_LastAudio;

#ifdef __cplusplus
extern "C" {
#endif

extern void Game_RunTimer(void);
extern void Game_RunTimerDelay(void);

#ifdef __cplusplus
}
#endif


EXTERNC void SR_CheckTimer(void)
{
    if (Game_SDLTicks != Game_LastAudio) Game_RunTimer();
}

EXTERNC void SR_RunTimerDelay(void)
{
    Game_RunTimerDelay();
}

