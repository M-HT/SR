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

#if !defined(_ALBION_MUSIC_XMIPLAYER_H_INCLUDED_)
#define _ALBION_MUSIC_XMIPLAYER_H_INCLUDED_

struct _xmi_player;

struct _xmi_player *xmi_player_create(unsigned int rate, const void *wave_synthetizer);
void xmi_player_destroy(struct _xmi_player *player);
int xmi_player_open(struct _xmi_player *player, const uint8_t *xmi, int seq_num);
void xmi_player_close(struct _xmi_player *player);
void xmi_player_set_volume(struct _xmi_player *player, int volume);
void xmi_player_set_loop_count(struct _xmi_player *player, int loop_count);
void xmi_player_get_data(struct _xmi_player *player, void *buffer, uint32_t size);
int xmi_player_is_playing(struct _xmi_player *player);

#endif /* _ALBION_MUSIC_XMIPLAYER_H_INCLUDED_ */

