#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include <string.h>
#if !defined(__cplusplus)
#if defined(_MSC_VER) && _MSC_VER < 1800
typedef int bool;
#define false 0
#define true 1
#else
#include <stdbool.h>
#endif
#endif

#include "Game_defs.h"
#include "Game_vars.h"

#if defined(_MSC_VER)
#define VAR_ALIGN(a)
#else
#define VAR_ALIGN(a) __attribute__ ((aligned (a)))
#endif


/*
 * Mapgrid is a rectangle area of the map which can be "seen" in the viewport
 *
 * View angle is the angle of the direction where the player is looking (when looking on the map from the top)
 *
 * Mapobject type == 4 - draw ceiling tile, when ceiling is not displayed (outside)
 *
 * There are two types of viewport coordinates
 *
 * 1) final, display type - point 0,0 is in the upper left corner of the viewport
 * 2) work type - point 0,0 is in the "middle" of the viewport  (x=0 is in the middle of the viewport, y=0 is in the middle of the viewport - on the horizont ? )
 *
*/


#define D3_MAXIMUM_NUMBER_OF_MAPOBJECTS 500
#define D3_MAXIMUM_NUMBER_OF_MAPGRID_POINTS 2400

// original value = 640
#define D3_MAXIMUM_VIEWPORT_WIDTH 800
// original value = 400
#define D3_MAXIMUM_VIEWPORT_HEIGHT 400

#pragma pack(1)

struct struc_1 {
    PTR32(uint8_t) viewport_first_pixel_addr;
    int32_t texture_y1_fp16;
    int32_t texture_y2_fp16;
    int32_t texture_height_in_viewport;
};

struct struc_2 {
    uint8_t mapobject_type_flags;
    uint8_t clip_flags;
    uint8_t draw_mapobject;
    uint8_t without_transparent_pixels;
    uint8_t mapdata_x;
    uint8_t mapdata_y;
    uint16_t field_06;
    uint16_t field_08;
    uint16_t field_0A;
    uint16_t field_0C;
    uint16_t field_0E;
    int32_t viewport_clip_x1;
    int32_t viewport_clip_x2;
    int32_t distance; // sortkey, distance ?
    union {
        struct {
            int32_t mapgrid_x1;
            int32_t mapgrid_y1;
            int32_t mapgrid_x2;
            int32_t mapgrid_y2;
        } t3;
        struct {
            int32_t mapdata_x;
            int32_t mapdata_y;
            PTR32(int32_t) mapgrid_point_ptr;
            PTR32(int32_t) mapgrid_next_line_point_ptr;
            PTR32(uint8_t) mapdata_ptr;
        } t4;
    };
    int32_t field_30;
    int32_t field_34;
    int32_t field_38;
    int32_t field_3C;
    int32_t field_40;
    int32_t viewport_x1;
    int32_t viewport_y11;
    int32_t viewport_y12;
    int32_t viewport_x2;
    int32_t viewport_y21;
    int32_t viewport_y22;
};

struct PACKED struc_4 {
    uint8_t check_value;
    uint8_t field_1;
    uint16_t xpos;
    uint16_t ypos;
    uint8_t mapobject_type;
    uint8_t field_7;
    uint8_t mapdata_x_plus1;
    uint8_t mapdata_y_plus1;
};

struct PACKED struc_5 {
    int32_t field_0;
    int32_t field_4;
    uint16_t field_8;
    uint16_t field_A;
};


struct PACKED struc_6 {
    int32_t field_0;
    uint8_t field_4;
    uint8_t field_5;
    uint8_t field_6;
    uint8_t field_7;
    uint16_t field_8;
};

struct PACKED struc_7 {
    uint16_t viewport_column;
    int32_t texture_ypos_fp8;
    int32_t texture_xpos_fp8;
};

struct struc_8 {
    int32_t sortkey; // ascending
    PTR32(struct struc_2) mapobject_ptr;
};

struct PACKED struc_9 {
    int16_t viewport_x1;
    int16_t viewport_x2;
    int16_t viewport_y1;
    int16_t viewport_y2;
};

#pragma pack()


static uint32_t Engine_RenderWidth;
static uint32_t Engine_RenderHeight;
static uint32_t Engine_MaximumViewportWidth;
static uint32_t Engine_MaximumViewportHeight;


static uint32_t Game_ScreenWidth;
static uint32_t Game_ViewportWidth, Game_ViewportHeight;
static uint8_t *Game_ViewportPtr;

static int32_t Game_ViewportZeroYRow, Game_ViewportZeroXColumn;
static int32_t Game_ViewportMinimumX, Game_ViewportMaximumX, Game_ViewportMinimumY, Game_ViewportMaximumY;
static uint16_t Game_ResizeWidthMult, Game_ResizeWidthDiv, Game_ResizeHeightMult, Game_ResizeHeightDiv;
static int32_t Game_mul_dword_140004_ResizeWidthMult, Game_mul_dword_140008_ResizeHeightMult;

//static struct struc_1 Game_stru_1414AC[D3_MAXIMUM_VIEWPORT_WIDTH + 3];
//static struct struc_7 Game_unk_144F50[2 * (D3_MAXIMUM_VIEWPORT_HEIGHT + 1)];
static struct struc_1 *Game_stru_1414AC = NULL;
static void *Game_unk_144F50 = NULL;

#define g_viewport_offsetx loc_14A8E8
#define g_viewport_offsety loc_14A8EA
// draw_3dscene
#define g_word_14A4C8  loc_14A4C8
#define d3_param_word_196D0C  loc_196D0C
#define g_dword_14A47C loc_14A47C
#define d3_without_ceiling  loc_143D59
#define d3_byte_13FFAB  loc_13FFAB
#define g_byte_14A4DE   loc_14A4DE
#define d3_param_palette_depth_index loc_196D00
#define g_dword_13FFC4 loc_13FFC4
#define d3_dword_1A5DF8 loc_1A5DF8
#define g_mapgrid_point_distance loc_13FFF8
#define d3_dword_1A5E0C loc_1A5E0C
#define d3_mapgrid_point_distance_3_4 loc_1A5DFC
#define g_byte_13FFAE  loc_13FFAE
#define g_word_14A480  loc_14A480
#define d3_dword_196CA8 loc_196CA8
#define g_word_14A486  loc_14A486
#define d3_dword_196CA4 loc_196CA4
#define d3_scene_counter loc_143D90
#define g_word_14A490  loc_14A490
#define g_dword_196CEC loc_196CEC
#define d3_word_196D0A  loc_196D0A
#define d3_word_196D08  loc_196D08
#define g_select_mapobject loc_140010
#define d3_time_counter_1 loc_143D84
#define d3_time_counter_2 loc_143D88
#define d3_time_counter_3 loc_143D8C
#define g_screen_width loc_196D04
#define g_viewport_zero_y_row loc_196CE0
// prepare_mapgrid
#define d3_stru_194A80  loc_194A80
#define g_sin_view_angle_fp14 loc_196CA0
#define g_dword_13FFDC loc_13FFDC
#define g_dword_13FFE0 loc_13FFE0
#define g_cos_view_angle_fp14 loc_196CD4
#define g_dword_13FFFC loc_13FFFC
#define g_mapdata_width loc_13FFE8
#define g_mapdata_height loc_13FFEC
#define d3_mapgrid_width loc_1A5E3C
#define d3_mapgrid_start_position_y loc_199A48
#define d3_mapgrid_start_position_x loc_199A44
#define d3_skip_draw_list_sm1234  loc_143D58
#define d3_mapgrid_number_of_points loc_143D5C
#define d3_mapgrid_height loc_1A5E38
#define d3_mapgrid_points loc_18E358
// fill_mapgrid
// initialize_mapvalues
#define d3_list_of_mapobjects  loc_19AA48
#define d3_next_mapobject_ptr loc_1A5E28
#define d3_dword_143D64 loc_143D64
#define d3_number_of_mapobjects loc_143D60
#define d3_dword_143D68 loc_143D68
// d3_sub_BF05C
#define g_mapdata_ptr loc_14A4B8
#define g_viewport_maximum_x loc_196CF8
#define g_dword_140004 loc_140004
#define g_dword_14A4AE loc_14A4AE
#define g_dword_14000C loc_14000C
#define g_word_14A4CA  loc_14A4CA
// insert_into_list_sm1234
#define d3_mapobjects_list_index loc_199AA8
#define d3_stru_1999E8  loc_1999E8
// d3_sub_BED08
#define g_dword_14A4A2 loc_14A4A2
#define g_dword_14A4AA loc_14A4AA
#define g_dword_140000 loc_140000
#define g_dword_140008 loc_140008
#define g_viewport_minimum_y loc_196CE8
// fill_stru_1999E8_2
#define g_viewport_minimum_x loc_196CE4
// fill_stru_1999E8_1
// d3_sub_BF4D4
#define g_dword_14A4B2 loc_14A4B2
#define g_word_14A4B6  loc_14A4B6
// d3_sub_BF9BC
#define d3_param_dword_143D74 loc_143D74
#define g_viewport_maximum_y loc_196CF4
#define d3_param_dword_143D78 loc_143D78
// d3_sub_BF644
// draw_background
#define g_sky_texture_ptr    loc_14A4D2
#define g_word_14A496     loc_14A496
#define g_sky_texture_width     loc_14A4D6
#define g_fillvalue1      loc_14A498
#define g_viewport_height_0    loc_196CF0
#define g_fillvalue2      loc_14A499
#define g_viewport_height loc_13FFF4
#define g_word_14A494     loc_14A494
#define g_sky_texture_height     loc_14A4D8
#define g_viewport_ptr    loc_13FFB0
// viewport_fillchar
#define g_viewport_width loc_13FFF0
// draw_lores_sky
#define g_view_angle_fp14 loc_196D10
#define g_word_1966B0 loc_1966B0
// draw_hires_sky
// draw_floor_and_ceiling
#define g_dword_143D6C loc_143D6C
#define g_dword_143D70 loc_143D70
#define d3_param_current_mapdata_ptr loc_1A5E08
#define d3_param_dword_1A5E00 loc_1A5E00
#define g_dword_14A4A6 loc_14A4A6
#define d3_param_dword_1A5E04 loc_1A5E04
// draw_list_sm1234
// sm1234_hyperproc
#define d3_palette_type   loc_13FFAC
#define d3_word_196C28    loc_196C28
#define g_byte_13FFAA    loc_13FFAA
#define d3_texture_column loc_13FFB8
#define d3_dword_1A5E34   loc_1A5E34
#define d3_dword_196CD0   loc_196CD0
#define d3_param_texture_height loc_196CCC
#define g_word_14A4BC    loc_14A4BC
#define d3_param_tile_color    loc_144E90
#define d3_dword_1A5E1C   loc_1A5E1C
#define d3_param_dword_1A5E2C   loc_1A5E2C
#define d3_param_dword_1A5E14   loc_1A5E14
#define d3_param_dword_1A5E30   loc_1A5E30
#define d3_dword_1A5E20   loc_1A5E20
#define d3_dword_1A5E24   loc_1A5E24
#define d3_param_dword_13FFC0   loc_13FFC0
// d3_sub_1CE04
#define g_dword_14FFF0 loc_14FFF0
// d3_sub_22BC1
#define g_word_151254 loc_151254
#define g_word_151374 loc_151374
// d3_sub_1CD21
#define g_word_1511C2  loc_1511C2
#define g_byte_151173  loc_151173
#define g_dword_14A8EC loc_14A8EC
// d3_sub_1CABA
#define g_word_1511CA  loc_1511CA
#define g_dword_14EBD4 loc_14EBD4
#define g_byte_159C71  loc_159C71
#define g_word_159C75  loc_159C75
// d3_sub_1CC6A
#define g_word_1511C4 loc_1511C4
#define g_word_1501D4 loc_1501D4
// sm1_subproc
#define d3_sm_dword_143D08_fp8 loc_143D08
#define d3_sm_dword_143CFC_fp8 loc_143CFC
#define d3_sm_dword_143D00_fp8 loc_143D00
#define d3_sm_dword_143CF4_fp8 loc_143CF4
#define d3_sm_viewport_y     loc_1414A0
#define d3_sm_texture_height_fp16 loc_143D14
#define d3_sm_dword_143CF8_fp8 loc_143CF8
// sm123_proc
#define g_viewport_zero_x_column       loc_196CDC
#define g_palettes           loc_196CFC
// sm4_proc
#define g_dword_13FFBC loc_13FFBC
// draw_solid_tile
#define d3_word_196BB0 loc_196BB0
#define d3_param_byte_144E8D loc_144E8D
// d3_sub_C4F7C
// d3_sub_C5B99
// d3_sub_C5342
#define d3_param_byte_144E92  loc_144E92
// draw_textured_tile
#define d3_word_199A58  loc_199A58
#define d3_param_dword_144EDE_fp8 loc_144EDE
#define d3_param_dword_144EE2_fp8 loc_144EE2
#define d3_dword_144F30 loc_144F30
#define d3_param_byte_144E8E  loc_144E8E
// d3_sub_C61A7
#define d3_param_texture_y1_144EA6  loc_144EA6
#define d3_param_texture_x1_144EA8  loc_144EA8
#define d3_param_texture_y2_144EAA  loc_144EAA
#define d3_param_texture_x2_144EAC  loc_144EAC
#define g_word_144EA0  loc_144EA0
// d3_sub_C5ED3
#define d3_param_dword_1A5E10 loc_1A5E10
#define d3_param_dword_1A5E18 loc_1A5E18
#define g_dword_13FFC8 loc_13FFC8
// d3_sub_BFBA4
// draw_floor_ceiling_tile
#define g_word_196D0E loc_196D0E


extern uint16_t g_viewport_offsetx;
extern uint16_t g_viewport_offsety;
// draw_3dscene
extern int16_t g_word_14A4C8;
extern int16_t d3_param_word_196D0C;
extern uint16_t g_dword_14A47C;
extern uint8_t d3_without_ceiling;
extern uint8_t d3_byte_13FFAB;
extern int8_t g_byte_14A4DE[1024];
extern PTR32(int8_t) d3_param_palette_depth_index;
extern uint8_t g_dword_13FFC4;
extern uint32_t d3_dword_1A5DF8;
extern int32_t g_mapgrid_point_distance;
extern int32_t d3_dword_1A5E0C;
extern int32_t d3_mapgrid_point_distance_3_4;
extern uint8_t g_byte_13FFAE;
extern uint16_t g_word_14A480[3];
extern int32_t d3_dword_196CA8;
extern uint16_t g_word_14A486[3];
extern int32_t d3_dword_196CA4;
extern uint32_t d3_scene_counter;
extern uint16_t g_word_14A490;
extern int32_t g_dword_196CEC;
extern uint16_t d3_word_196D0A;
extern uint16_t d3_word_196D08;
extern PTR32(struct struc_4) g_select_mapobject;
extern uint32_t d3_time_counter_1;
extern uint32_t d3_time_counter_2;
extern uint32_t d3_time_counter_3;
extern int32_t g_screen_width;
extern int32_t g_viewport_zero_y_row;
// prepare_mapgrid
extern struct struc_9 d3_stru_194A80[D3_MAXIMUM_NUMBER_OF_MAPOBJECTS + 1];
extern int32_t g_sin_view_angle_fp14;
extern int32_t g_dword_13FFDC;
extern int32_t g_dword_13FFE0;
extern int32_t g_cos_view_angle_fp14;
extern uint8_t g_dword_13FFFC;
extern int32_t g_mapdata_width;
extern int32_t g_mapdata_height;
extern int32_t d3_mapgrid_width;
extern int32_t d3_mapgrid_start_position_y;
extern int32_t d3_mapgrid_start_position_x;
extern uint8_t d3_skip_draw_list_sm1234;
extern int32_t d3_mapgrid_number_of_points;
extern int32_t d3_mapgrid_height;
extern int32_t d3_mapgrid_points[2*D3_MAXIMUM_NUMBER_OF_MAPGRID_POINTS];
// fill_mapgrid
// initialize_mapvalues
extern struct struc_2 d3_list_of_mapobjects[D3_MAXIMUM_NUMBER_OF_MAPOBJECTS];
extern PTR32(struct struc_2) d3_next_mapobject_ptr;
extern uint32_t d3_dword_143D64;
extern int32_t d3_number_of_mapobjects;
extern int32_t d3_dword_143D68;
// d3_sub_BF05C
extern PTR32(uint8_t) g_mapdata_ptr;
extern int32_t g_viewport_maximum_x;
extern int32_t g_dword_140004;
extern PTR32_ALIGN(PTR32(void),2) g_dword_14A4AE;
extern int32_t g_dword_14000C;
extern uint16_t g_word_14A4CA[4];
// insert_into_list_sm1234
extern struct struc_8 d3_mapobjects_list_index[D3_MAXIMUM_NUMBER_OF_MAPOBJECTS];
extern struct struc_2 d3_stru_1999E8;
// d3_sub_BED08
extern PTR32_ALIGN(int16_t,2) g_dword_14A4A2; // struct ??? (i16, i16, i16, u16)
extern PTR32_ALIGN(uint8_t,2) g_dword_14A4AA; // struct ???
extern int32_t g_dword_140000;
extern int32_t g_dword_140008;
extern int32_t g_viewport_minimum_y;
// fill_stru_1999E8_2
extern int32_t g_viewport_minimum_x;
// fill_stru_1999E8_1
// d3_sub_BF4D4
extern PTR32_ALIGN(struct struc_5,2) g_dword_14A4B2;
extern uint16_t g_word_14A4B6;
// d3_sub_BF9BC
extern int32_t d3_param_dword_143D74;
extern int32_t g_viewport_maximum_y;
extern int32_t d3_param_dword_143D78;
// d3_sub_BF644
// draw_background
extern PTR32_ALIGN(uint8_t,2) g_sky_texture_ptr;
extern uint16_t g_word_14A496;
extern uint16_t g_sky_texture_width;
extern uint8_t g_fillvalue1;
extern int32_t g_viewport_height_0;
extern uint8_t g_fillvalue2;
extern int32_t g_viewport_height;
extern int16_t g_word_14A494;
extern uint16_t g_sky_texture_height;
extern PTR32(uint8_t) g_viewport_ptr;
// viewport_fillchar
extern int32_t g_viewport_width;
// draw_lores_sky
extern int16_t g_view_angle_fp14; // player view angle 0..16383
extern uint16_t g_word_1966B0[640];
// draw_hires_sky
// draw_floor_and_ceiling
extern int32_t g_dword_143D6C;
extern int32_t g_dword_143D70;
extern PTR32(uint8_t) d3_param_current_mapdata_ptr;
extern int32_t d3_param_dword_1A5E00;
extern PTR32_ALIGN(struct struc_6,2) g_dword_14A4A6;
extern int32_t d3_param_dword_1A5E04;
// draw_list_sm1234
// sm1234_hyperproc
extern uint8_t d3_palette_type;
extern uint16_t d3_word_196C28[60];
extern uint8_t g_byte_13FFAA; // ??? - read only (0)
extern PTR32(uint8_t) d3_texture_column;
extern int32_t d3_dword_1A5E34;
extern uint32_t d3_dword_196CD0;
extern uint32_t d3_param_texture_height;
extern uint16_t g_word_14A4BC;
extern uint8_t d3_param_tile_color;
extern int32_t d3_dword_1A5E1C;
extern int32_t d3_param_dword_1A5E2C;
extern int32_t d3_param_dword_1A5E14;
extern int32_t d3_param_dword_1A5E30;
extern int32_t d3_dword_1A5E20;
extern int32_t d3_dword_1A5E24;
extern PTR32(uint8_t) d3_param_dword_13FFC0;
// d3_sub_1CE04
extern PTR32(void) g_dword_14FFF0[1];
// d3_sub_22BC1
extern uint16_t g_word_151254[144];
extern uint16_t g_word_151374[168];
// d3_sub_1CD21
extern uint16_t g_word_1511C2;
extern uint8_t g_byte_151173[1];
extern PTR32(void) g_dword_14A8EC[1];
// d3_sub_1CABA
extern uint16_t g_word_1511CA;
extern PTR32(void) g_dword_14EBD4[1];
extern uint8_t g_byte_159C71[1];
extern uint16_t g_word_159C75[1];
// d3_sub_1CC6A
extern uint16_t g_word_1511C4;
extern uint8_t g_word_1501D4[4000]; // ??? struct
// sm1_subproc
extern int32_t d3_sm_dword_143D08_fp8; // sm1_subproc parameter
extern int32_t d3_sm_dword_143CFC_fp8; // sm1_subproc parameter
extern int32_t d3_sm_dword_143D00_fp8; // sm1_subproc
extern int32_t d3_sm_dword_143CF4_fp8; // sm1_subproc parameter
extern int32_t d3_sm_viewport_y; // sm1_subproc parameter
extern uint32_t d3_sm_texture_height_fp16; // sm1_subproc parameter
extern int32_t d3_sm_dword_143CF8_fp8; // sm1_subproc parameter
// sm123_proc
extern int32_t g_viewport_zero_x_column;
extern PTR32(uint8_t) g_palettes;
// sm4_proc
extern PTR32(uint16_t) g_dword_13FFBC; // ??? - read only (0)
// draw_solid_tile
extern int16_t d3_word_196BB0[60];
extern uint8_t d3_param_byte_144E8D;
// d3_sub_C4F7C
// d3_sub_C5B99
// d3_sub_C5342
extern uint8_t d3_param_byte_144E92;
// draw_textured_tile
extern int16_t d3_word_199A58[40];
extern int32_t d3_param_dword_144EDE_fp8 VAR_ALIGN(2);
extern int32_t d3_param_dword_144EE2_fp8 VAR_ALIGN(2);
extern int32_t d3_dword_144F30;
extern uint8_t d3_param_byte_144E8E;
// d3_sub_C61A7
extern int16_t d3_param_texture_y1_144EA6;
extern int16_t d3_param_texture_x1_144EA8;
extern int16_t d3_param_texture_y2_144EAA;
extern int16_t d3_param_texture_x2_144EAC;
extern int16_t g_word_144EA0;
// d3_sub_C5ED3
extern int32_t d3_param_dword_1A5E10;
extern int32_t d3_param_dword_1A5E18;
extern uint32_t g_dword_13FFC8;
// d3_sub_BFBA4
// draw_floor_ceiling_tile
extern int16_t g_word_196D0E;


#ifdef __cplusplus
extern "C"
#else
extern
#endif
void *sub_8B6BB(void *handle);


static int32_t INLINE convert_horizontal(int32_t xpos)
{
    return (xpos * (int32_t)Engine_RenderWidth) / 360;
}

static int32_t INLINE convert_vertical(int32_t ypos)
{
    return (ypos * (int32_t)Engine_RenderHeight) / 240;
}


static int32_t INLINE HIDWORD_S(int64_t i64)
{
    return (int32_t) (i64 >> 32);
}

static uint32_t INLINE HIDWORD_U(uint64_t u64)
{
    return (uint32_t) (u64 >> 32);
}

static int64_t INLINE __PAIR_S__(int32_t d1, uint32_t d0)
{
    return (((int64_t) d1) << 32) | ((uint64_t) d0);
}

static int64_t INLINE __PAIR_U__(uint32_t d1, uint32_t d0)
{
    return (((uint64_t) d1) << 32) | ((uint64_t) d0);
}


static void INLINE memorycopy(void *dst, const void *src, size_t count)
{
    memcpy(dst, src, count);
}


static struct struc_7 *d3_sub_C5ED3(int32_t y1, int32_t x1, int32_t y2, int32_t x2, struct struc_7 *a5)
{
    int32_t viewport_column; // ecx@3
    int32_t var08; // ecx@7
    int32_t var09; // ebx@7
    int32_t var10; // eax@7
    int32_t texture_ydiff_fp8; // esi@17
    int32_t texture_xdiff_fp8; // ebp@17
    int32_t viewport_column_fp12; // ebx@17 // fp12
    int32_t texture_xpos_fp8; // eax@17
    int32_t texture_ypos_fp8; // ecx@17

    int32_t invresized_dword_144EDE_fp8;
    int32_t invresized_dword_144EE2_fp8;

    int32_t l_vertical_counter_144F2C;
    int32_t l_viewport_xdiff_fp12; // fp12
    int32_t l_viewport_mincolumn;
    int32_t l_viewport_maxcolumn;
    int32_t l_dword_144EC6_fp8; // fp8
    int32_t l_dword_144ECA_fp8; // fp8
    int32_t l_dword_144ECE;
    int32_t l_dword_144ED2_fp8; // fp8
    int32_t l_dword_144ED6_fp8; // fp8
    int32_t l_dword_144EDA;
    int32_t l_dword_144EF6_fp8;
    int32_t l_dword_144EE6;
    int32_t l_dword_144EEA;
    int32_t l_dword_144EEE;

    l_vertical_counter_144F2C = y2 - y1;
    viewport_column_fp12 = (Game_ViewportZeroXColumn + x1) << 12;
    l_viewport_xdiff_fp12 = ((x2 - x1) << 12) / (y2 - y1);

    if ( d3_param_byte_144E8E )
    {
        invresized_dword_144EDE_fp8 = (d3_param_dword_144EDE_fp8 * Game_ResizeWidthDiv) / Game_ResizeWidthMult;
        invresized_dword_144EE2_fp8 = (d3_param_dword_144EE2_fp8 * Game_ResizeWidthDiv) / Game_ResizeWidthMult;

        l_viewport_mincolumn = Game_ViewportZeroXColumn + Game_ViewportMinimumX;
        l_viewport_maxcolumn = Game_ViewportZeroXColumn + Game_ViewportMaximumX;
        l_dword_144EC6_fp8 = (((int64_t)((Game_mul_dword_140008_ResizeHeightMult * d3_param_dword_1A5E2C) + y1 * Game_ResizeHeightDiv * d3_param_dword_1A5E10)) << 8) / Game_mul_dword_140008_ResizeHeightMult;
        l_dword_144ECA_fp8 = (((int64_t)((Game_mul_dword_140008_ResizeHeightMult * d3_param_dword_1A5E30) + y1 * Game_ResizeHeightDiv * d3_param_dword_1A5E14)) << 8) / Game_mul_dword_140008_ResizeHeightMult;
        l_dword_144ECE = ((y1 << 8) * (int64_t)(d3_param_dword_1A5E18 * Game_ResizeHeightDiv)) / (Game_mul_dword_140008_ResizeHeightMult << 8);
        l_dword_144ED2_fp8 = (((int64_t)(d3_param_dword_1A5E10 * Game_ResizeHeightDiv)) << 8) / Game_mul_dword_140008_ResizeHeightMult;
        l_dword_144ED6_fp8 = (((int64_t)(d3_param_dword_1A5E14 * Game_ResizeHeightDiv)) << 8) / Game_mul_dword_140008_ResizeHeightMult;
        l_dword_144EDA = (((int64_t)(d3_param_dword_1A5E18 * Game_ResizeHeightDiv)) << 8) / (Game_mul_dword_140008_ResizeHeightMult << 8);
        l_dword_144EF6_fp8 = (1 << (g_dword_13FFC8 + 8)) - 1;
        do
        {
            viewport_column = viewport_column_fp12 >> 12;
            viewport_column_fp12 += l_viewport_xdiff_fp12;

            if ( viewport_column < l_viewport_mincolumn )
            {
                viewport_column = l_viewport_mincolumn;
            }
            else
            {
                if ( viewport_column > l_viewport_maxcolumn )
                    viewport_column = l_viewport_maxcolumn;
            }


            a5->viewport_column = (uint16_t)viewport_column;
            l_dword_144EE6 = l_dword_144EC6_fp8 >> 8;
            l_dword_144EC6_fp8 += l_dword_144ED2_fp8;
            l_dword_144EEA = l_dword_144ECA_fp8 >> 8;
            l_dword_144ECA_fp8 += l_dword_144ED6_fp8;
            l_dword_144EEE = (l_dword_144ECE >> 8) | 1;
            l_dword_144ECE += l_dword_144EDA;
            var08 = viewport_column - Game_ViewportZeroXColumn;
            l_dword_144EE6 += (invresized_dword_144EDE_fp8 * (int64_t)var08) >> 8;
            l_dword_144EEA += (invresized_dword_144EE2_fp8 * (int64_t)var08) >> 8;
            var09 = (l_dword_144EE6 << g_dword_13FFC8) / l_dword_144EEE;
            var10 = (l_dword_144EEA << g_dword_13FFC8) / l_dword_144EEE;

            if ( var09 >= 0 )
            {
                if ( var09 > l_dword_144EF6_fp8 )
                    var09 = l_dword_144EF6_fp8;
            }
            else
            {
                var09 = 0;
            }

            if ( var10 >= 0 )
            {
                if ( var10 > l_dword_144EF6_fp8 )
                    var10 = l_dword_144EF6_fp8;
            }
            else
            {
                var10 = 0;
            }

            a5->texture_ypos_fp8 = var09;
            a5->texture_xpos_fp8 = var10;

            ++a5;
            --l_vertical_counter_144F2C;
        }
        while ( l_vertical_counter_144F2C >= 0 );

        return a5 - 1;
    }
    else
    {
        texture_ydiff_fp8 = (((int32_t)(d3_param_texture_y2_144EAA - d3_param_texture_y1_144EA6)) << 8) / l_vertical_counter_144F2C;
        texture_xdiff_fp8 = (((int32_t)(d3_param_texture_x2_144EAC - d3_param_texture_x1_144EA8)) << 8) / l_vertical_counter_144F2C;
        texture_xpos_fp8 = ((uint32_t)(uint16_t)d3_param_texture_x1_144EA8) << 8;
        texture_ypos_fp8 = ((uint32_t)(uint16_t)d3_param_texture_y1_144EA6) << 8;

        do
        {
            a5->texture_xpos_fp8 = texture_xpos_fp8;
            a5->texture_ypos_fp8 = texture_ypos_fp8;
            texture_xpos_fp8 += texture_xdiff_fp8;
            texture_ypos_fp8 += texture_ydiff_fp8;
            a5->viewport_column = (uint16_t)(viewport_column_fp12 >> 12);
            viewport_column_fp12 += l_viewport_xdiff_fp12;

            ++a5;
            --l_vertical_counter_144F2C;
        }
        while ( l_vertical_counter_144F2C >= 0 );

        return a5 - 1;
    }
}


static void d3_sub_C61A7(int32_t number_of_coordinates, int16_t *list_of_coordinates)
{
    struct struc_7 *var02; // edi@1
    int32_t x1; // ebx@3
    int32_t y1; // eax@3
    int32_t x2; // edx@3
    int32_t y2; // ecx@3
    int32_t viewport_column2; // ecx@27
    int32_t viewport_column1; // edi@27
    int32_t horizontal_counter; // ecx@29

    uint8_t *viewport_line_ptr;
    uint8_t *pal;
    int32_t vertical_counter;
    int32_t texture_xpos_fp16, texture_ypos_fp16;
    int32_t texture_xdiff_fp16, texture_ydiff_fp16;
    uint8_t *dst, *src;

    int16_t *l_list_of_coordinates;
    int32_t l_x_min;
    int32_t l_x_max;
    int16_t l_viewport_y2;
    int16_t l_viewport_y1;
    int32_t l_counter_144F28;

    l_list_of_coordinates = list_of_coordinates;
    l_x_min = 32767;
    l_x_max = -32767;
    l_viewport_y2 = l_viewport_y1 = list_of_coordinates[1];
    l_counter_144F28 = number_of_coordinates;
    var02 = (struct struc_7 *) Game_unk_144F50;
    if ( !d3_param_byte_144E8D )
        var02 += Engine_MaximumViewportHeight;

    do
    {
        d3_param_texture_y1_144EA6 = list_of_coordinates[2];
        d3_param_texture_x1_144EA8 = list_of_coordinates[3];
        x1 = (int32_t)list_of_coordinates[0];
        y1 = (int32_t)list_of_coordinates[1];
        list_of_coordinates += 4;
        d3_param_texture_y2_144EAA = list_of_coordinates[2];
        d3_param_texture_x2_144EAC = list_of_coordinates[3];
        x2 = (int32_t)list_of_coordinates[0];
        y2 = (int32_t)list_of_coordinates[1];

        if ( x1 < l_x_min )
            l_x_min = x1;
        if ( x1 > l_x_max )
            l_x_max = x1;
        if ( y2 < y1 )
            break;

        if ( y2 != y1 )
        {
            l_viewport_y2 = y2;
            var02 = d3_sub_C5ED3(y1, x1, y2, x2, var02);
        }

        --l_counter_144F28;
    }
    while ( l_counter_144F28 );

    l_counter_144F28 = number_of_coordinates;
    list_of_coordinates = &(l_list_of_coordinates[4 * number_of_coordinates]);
    var02 = (struct struc_7 *) Game_unk_144F50;
    if ( d3_param_byte_144E8D )
        var02 += Engine_MaximumViewportHeight;

    do
    {
        d3_param_texture_y1_144EA6 = list_of_coordinates[2];
        d3_param_texture_x1_144EA8 = list_of_coordinates[3];
        x1 = list_of_coordinates[0];
        y1 = list_of_coordinates[1];
        list_of_coordinates -= 4;
        d3_param_texture_y2_144EAA = list_of_coordinates[2];
        d3_param_texture_x2_144EAC = list_of_coordinates[3];
        x2 = list_of_coordinates[0];
        y2 = list_of_coordinates[1];

        if ( x1 < l_x_min )
            l_x_min = x1;
        if ( x1 > l_x_max )
            l_x_max = x1;
        if ( y2 < y1 )
            break;

        if ( y2 != y1 )
            var02 = d3_sub_C5ED3(y1, x1, y2, x2, var02);

        --l_counter_144F28;
    }
    while ( l_counter_144F28 );

    var02 = (struct struc_7 *) Game_unk_144F50;
    if ( l_viewport_y1 == l_viewport_y2 )
    {
        int32_t column_min; // bx@22
        int32_t column_max; // ax@24

        column_min = Game_ViewportZeroXColumn + l_x_min;
        if ( column_min < 0 )
            column_min = 0;
        var02[0].viewport_column = (uint16_t)column_min;

        column_max = Game_ViewportZeroXColumn + l_x_max;
        if ( column_max > (int32_t)Game_ViewportWidth )
            column_max = Game_ViewportWidth;
        var02[Engine_MaximumViewportHeight].viewport_column = (uint16_t)column_max;
    }

    g_word_144EA0 = Game_ViewportZeroYRow + (int32_t)l_viewport_y1;
    viewport_line_ptr = &(Game_ViewportPtr[(Game_ViewportZeroYRow + (int32_t)l_viewport_y1) * Game_ScreenWidth]);
    vertical_counter = (uint16_t)(l_viewport_y2 - l_viewport_y1);
    pal = &(g_palettes[256 * (uint32_t)d3_palette_type]);

    do
    {
        viewport_column2 = (uint32_t)var02[Engine_MaximumViewportHeight].viewport_column;
        viewport_column1 = (uint32_t)var02[0].viewport_column;

        if ( viewport_column2 <= viewport_column1 )
        {
            viewport_column2 = (uint32_t)var02[0].viewport_column;
            viewport_column1 = (uint32_t)var02[Engine_MaximumViewportHeight].viewport_column;
        }

        horizontal_counter = (viewport_column2 - viewport_column1) + 1;
        dst = viewport_line_ptr + viewport_column1;

        texture_xdiff_fp16 = ((var02[Engine_MaximumViewportHeight].texture_xpos_fp8 - var02[0].texture_xpos_fp8) << 8) / horizontal_counter;
        texture_ydiff_fp16 = ((var02[Engine_MaximumViewportHeight].texture_ypos_fp8 - var02[0].texture_ypos_fp8) << 8) / horizontal_counter;

        texture_xpos_fp16 = var02[0].texture_xpos_fp8 << 8;
        texture_ypos_fp16 = var02[0].texture_ypos_fp8 << 8;

        src = &(d3_texture_column[((texture_ypos_fp16 >> 16) & 0xff) * 256 + ((texture_xpos_fp16 >> 16) & 0xff)]);

        for (; horizontal_counter > 0; horizontal_counter--)
        {
            uint8_t tmp;

            tmp = pal[*src];
            texture_xpos_fp16 += texture_xdiff_fp16;
            src = &(d3_texture_column[((texture_ypos_fp16 >> 16) & 0xff) * 256 + ((texture_xpos_fp16 >> 16) & 0xff)]);
            texture_ypos_fp16 += texture_ydiff_fp16;
            *dst = tmp;

            dst++;
        }

        viewport_line_ptr += Game_ScreenWidth;
        var02++;
        vertical_counter--;
    }
    while ( vertical_counter >= 0 );
}


static void draw_textured_tile(int32_t number_of_coordinates, int32_t a2, int32_t a3)
{
    int32_t var_14;
    int32_t var_10;
    int32_t y_max;
    int32_t y_min;
    int16_t x_max;
    int16_t x_min;


    int32_t var03; // edx@0
    int32_t coordinate_counter; // ecx@1
    int16_t *current_coordinate; // esi@2
    int32_t var09; // ebx@22
    int16_t *var10; // edi@22
    int32_t var11; // eax@22
    int16_t *var12; // esi@22
    int32_t var13; // edx@23
    int32_t var14; // ecx@23

    int8_t l_partial_tile;

    if ( number_of_coordinates < 3 ) return;

    // silence warning
    var03 = 0;

    coordinate_counter = number_of_coordinates;
    current_coordinate = &(d3_word_199A58[0]);
    y_min = 32767;
    x_min = 32767;
    y_max = -32767;
    x_max = -32767;
    do
    {
        if ( current_coordinate[0] > x_max )
            x_max = current_coordinate[0];
        if ( current_coordinate[0] < x_min )
            x_min = current_coordinate[0];
        if ( (int32_t)current_coordinate[1] > y_max )
            y_max = (int32_t)current_coordinate[1];
        if ( (int32_t)current_coordinate[1] < y_min )
        {
            y_min = (int32_t)current_coordinate[1];
            var03 = coordinate_counter;
        }
        current_coordinate += 4;
        --coordinate_counter;
    }
    while ( coordinate_counter );

    l_partial_tile = 0;
    if ( y_min >= Game_ViewportMinimumY )
    {
        if ( y_max > Game_ViewportMaximumY )
        {
            l_partial_tile = 1;
            if ( y_min > Game_ViewportMaximumY )
                return;
        }
    }
    else
    {
        l_partial_tile = 1;
        if ( y_max < Game_ViewportMinimumY )
            return;
    }

    if ( x_min >= (int16_t)Game_ViewportMinimumX )
    {
      if ( (x_max > (int16_t)Game_ViewportMaximumX) && (x_min > (int16_t)Game_ViewportMaximumX) )
        return;
    }
    else
    {
        if ( x_max < (int16_t)Game_ViewportMinimumX )
            return;
    }

    // rotate the list of coordinates, so that the coordinate with lowest y is first
    memorycopy(&(d3_word_196BB0[0]), &(d3_word_199A58[4 * (number_of_coordinates - var03)]), 8 * var03);
    memorycopy(&(d3_word_196BB0[4 * var03]), &(d3_word_199A58[0]), 8 * (number_of_coordinates - var03));
    memorycopy(&(d3_word_196BB0[4 * number_of_coordinates]), &(d3_word_196BB0[0]), 8);
    var10 = &(d3_word_196BB0[4 * number_of_coordinates + 4]);
    var09 = (int32_t)d3_word_196BB0[0];
    var11 = (int32_t)d3_word_196BB0[1];
    var12 = &(d3_word_196BB0[4]);

    // var09,var11 = first coordinate (with lowest y)
    do
    {
        var13 = (int32_t)var12[0];
        var14 = (int32_t)var12[1];
        var12 += 4;
    }
    while ( (var09 == var13) && (var11 == var14) );
    // var13,var14 = next coordinate (different from first)

    var_10 = var13 - var09;
    var_14 = (var14 + 1) - var11;
    var12 = var10 - 4;

    do
    {
        var13 = (int32_t)var12[0];
        var14 = (int32_t)var12[1];
        var12 -= 4;
    }
    while ( (var09 == var13) && (var11 == var14) );
    // var13,var14 = previous coordinate (different from first)

    d3_param_byte_144E8D = ((int32_t) (var_10 * ((var14 + 1) - var11) - var_14 * (var13 - var09)) ) >> 31;
    d3_param_dword_144EDE_fp8 = (((int64_t)d3_dword_1A5E1C) << 8) / g_dword_140004;
    d3_param_dword_144EE2_fp8 = (((int64_t)d3_dword_1A5E20) << 8) / g_dword_140004;

    if ( !l_partial_tile )
    {
        d3_dword_144F30 = g_dword_140008 * a2;
        d3_param_byte_144E8E = (uint8_t)a3;
        d3_sub_C61A7(number_of_coordinates, &(d3_word_196BB0[0]));
    }
}


static PTR32(uint8_t) *d3_sub_C5342(int32_t y1, int32_t x1, int32_t y2, int32_t x2, PTR32(uint8_t) *a5)
{
    uint32_t height;
    int32_t width;
    int32_t vertical_counter;
    int64_t diff_fp32;
    uint64_t value_fp32;

    uint8_t l_byte_144E8F;
    uint8_t *l_dword_144F44;

    l_byte_144E8F = 0;
    height = y2 - y1;
    width = x2 - x1;

    if ( width < 0 )
    {
        width = -width;
        l_byte_144E8F = 1;
    }

    l_dword_144F44 = &(Game_ViewportPtr[Game_ScreenWidth * (Game_ViewportZeroYRow + y1) + Game_ViewportZeroXColumn + x1]);
    vertical_counter = (height & 0xffff) + 1;
    diff_fp32 = (((uint64_t)width) << 32) / height;

    if ( l_byte_144E8F )
        diff_fp32 = -diff_fp32;

    value_fp32 = __PAIR_U__((uintptr_t)l_dword_144F44, 0x8000);
    diff_fp32 += ((int64_t)Game_ScreenWidth) << 32;

    if ( d3_param_byte_144E92 )
        value_fp32 += ((uint64_t)1) << 32;

    for (; vertical_counter > 0; vertical_counter--)
    {
        a5[0] = (uint8_t *)(uintptr_t)HIDWORD_U(value_fp32);
        a5++;
        value_fp32 += diff_fp32;
    }

    return a5-1;
}


static void d3_sub_C5B99(int32_t number_of_coordinates, int16_t *list_of_coordinates)
{
    PTR32(uint8_t) *var02; // edi@1
    int32_t x1; // ebx@3
    int32_t y1; // ax@3
    int32_t x2; // edx@3
    int32_t y2; // ecx@3
    int16_t vertical_counter; // dx@23
    uint8_t *dst; // edi@24
    int32_t var22; // ecx@24

    int16_t *l_list_of_coordinates;
    int32_t l_x_min;
    int32_t l_x_max;
    int16_t l_viewport_y2;
    int16_t l_viewport_y1;
    int32_t l_counter_144F28;

    l_list_of_coordinates = list_of_coordinates;
    l_x_min = 32767;
    l_x_max = -32767;
    l_viewport_y2 = l_viewport_y1 = list_of_coordinates[1];
    l_counter_144F28 = number_of_coordinates;
    d3_param_byte_144E92 = 0;
    var02 = (PTR32(uint8_t) *) Game_unk_144F50;
    if ( !d3_param_byte_144E8D )
    {
        d3_param_byte_144E92 = ~d3_param_byte_144E92;
        var02 += Engine_MaximumViewportHeight;
    }

    do
    {
        x1 = (int32_t)list_of_coordinates[0];
        y1 = (int32_t)list_of_coordinates[1];
        list_of_coordinates += 2;
        x2 = list_of_coordinates[0];
        y2 = list_of_coordinates[1];

        if ( x1 < l_x_min )
            l_x_min = x1;
        if ( x1 > l_x_max )
            l_x_max = x1;
        if ( y2 < y1 )
            break;

        if ( y2 != y1 )
        {
            l_viewport_y2 = (int16_t)y2;
            var02 = d3_sub_C5342(y1, x1, y2, x2, var02);
        }

        --l_counter_144F28;
    }
    while ( l_counter_144F28 );

    d3_param_byte_144E92 = ~d3_param_byte_144E92;
    l_counter_144F28 = number_of_coordinates;
    list_of_coordinates = &(l_list_of_coordinates[2 * number_of_coordinates]);
    var02 = (PTR32(uint8_t) *) Game_unk_144F50;
    if ( d3_param_byte_144E8D )
        var02 += Engine_MaximumViewportHeight;

    do
    {
        x1 = (int32_t)list_of_coordinates[0];
        y1 = (int32_t)list_of_coordinates[1];
        list_of_coordinates -= 2;
        x2 = (int32_t)list_of_coordinates[0];
        y2 = (int32_t)list_of_coordinates[1];

        if ( x1 < l_x_min )
            l_x_min = x1;
        if ( x1 > l_x_max )
            l_x_max = x1;
        if ( y2 < y1 )
            break;

        if ( y2 != y1 )
            var02 = d3_sub_C5342(y1, x1, y2, x2, var02);

        --l_counter_144F28;
    }
    while ( l_counter_144F28 );

    var02 = (PTR32(uint8_t) *) Game_unk_144F50;
    if ( l_viewport_y1 == l_viewport_y2 )
    {
        uint8_t *var16; // eax@22

        var16 = &(Game_ViewportPtr[Game_ViewportZeroXColumn + Game_ScreenWidth * (Game_ViewportZeroYRow + (int32_t)l_viewport_y1)]);
        var02[0] = &(var16[l_x_min]);
        var02[Engine_MaximumViewportHeight] = &(var16[l_x_max]);
    }

    vertical_counter = l_viewport_y2 - l_viewport_y1;

    do
    {
        dst = var02[0];
        var22 = var02[Engine_MaximumViewportHeight] - dst;
        ++var02;

        if ( var22 < 0 )
        {
            var22 = -var22;
            dst -= var22;
        }

        memset(dst, d3_param_tile_color, var22);

        --vertical_counter;
    }
    while ( vertical_counter >= 0 );
}


static int32_t d3_sub_C4F7C(int32_t a1)
{
    int32_t var_A;
    int16_t var_6;
    int32_t var_4;


    int16_t *var01; // esi@1
    int16_t *var02; // edi@1
    uint16_t var03; // cx@1
    uint16_t var04; // bx@2


    var01 = (int16_t *)&(d3_word_196C28[0]);
    var01[a1] = var01[0];
    var01[a1 + 1] = var01[1];
    var02 = &(d3_word_196BB0[0]);
    var03 = ((int16_t)Game_ViewportMinimumX - var01[0]) & 0x8000;
    var_4 = a1 + 1;
    a1 = 1;
    for ( --var_4; var_4 != 0; --var_4 )
    {
        var04 = var03;
        var03 = ((int16_t)Game_ViewportMinimumX - var01[2]) & 0x8000;
        if ( var04 )
        {
            var02[0] = var01[0];
            var02[1] = var01[1];
            var02 += 2;
            ++a1;
        }

        if ( var03 ^ var04 ) // different sign
        {
            if ( var01[0] >= var01[2] )
            {
                var02[0] = (int16_t)Game_ViewportMinimumX;
                var02[1] = var01[3] + ( (((int16_t)Game_ViewportMinimumX - var01[2]) * (int32_t)(var01[3] - var01[1])) / (var01[2] - var01[0]) );
                var02 += 2;
            }
            else
            {
                var02[0] = (int16_t)Game_ViewportMinimumX;
                var02[1] = var01[1] + ( (((int16_t)Game_ViewportMinimumX - var01[0]) * (int32_t)(var01[3] - var01[1])) / (var01[2] - var01[0]) );
                var02 += 2;
            }

            ++a1;
        }

        var01 += 2;
    }

    var01 = &(d3_word_196BB0[0]);
    var02[0] = var01[0];
    var02[1] = var01[1];
    var02 = (int16_t *)&(d3_word_196C28[0]);
    var03 = (var01[0] - (int16_t)Game_ViewportMaximumX) & 0x8000;
    var_4 = a1;
    a1 = 1;
    for ( --var_4; var_4 != 0; --var_4 )
    {
        var04 = var03;
        var03 = (var01[2] - (int16_t)Game_ViewportMaximumX) & 0x8000;
        if ( var04 )
        {
            var02[0] = var01[0];
            var02[1] = var01[1];
            var02 += 2;
            ++a1;
        }

        if ( var03 ^ var04 ) // different sign
        {
            if ( var01[0] >= var01[2] )
            {
                var02[0] = (int16_t)Game_ViewportMaximumX;
                var02[1] = var01[3] + ( (((int16_t)Game_ViewportMaximumX - var01[2]) * (int32_t)(var01[3] - var01[1])) / (var01[2] - var01[0]) );
                var02 += 2;
            }
            else
            {
                var02[0] = (int16_t)Game_ViewportMaximumX;
                var02[1] = var01[1] + ( (((int16_t)Game_ViewportMaximumX - var01[0]) * (int32_t)(var01[3] - var01[1])) / (var01[2] - var01[0]) );
                var02 += 2;
            }

            ++a1;
        }

        var01 += 2;
    }

    var01 = (int16_t *)&(d3_word_196C28[0]);
    var02[0] = var01[0];
    var02[1] = var01[1];
    var02 = &(d3_word_196BB0[0]);
    var03 = ((int16_t)Game_ViewportMinimumY - var01[1]) & 0x8000;
    var_4 = a1;
    a1 = 1;
    for ( --var_4; var_4 != 0; --var_4 )
    {
        var04 = var03;
        var03 = ((int16_t)Game_ViewportMinimumY - var01[3]) & 0x8000;
        if ( var04 )
        {
            var02[0] = var01[0];
            var02[1] = var01[1];
            var02 += 2;
            ++a1;
        }

        if ( var03 ^ var04 ) // different sign
        {
            if ( var01[1] >= var01[3] )
            {
                var02[0] = var01[2] + ( (((int16_t)Game_ViewportMinimumY - var01[3]) * (int32_t)(var01[2] - var01[0])) / (var01[3] - var01[1]) );
                var02[1] = (int16_t)Game_ViewportMinimumY;
                var02 += 2;
            }
            else
            {
                var02[0] = var01[0] + ( (((int16_t)Game_ViewportMinimumY - var01[1]) * (int32_t)(var01[2] - var01[0])) / (var01[3] - var01[1]) );
                var02[1] = (int16_t)Game_ViewportMinimumY;
                var02 += 2;
            }

            ++a1;
        }

        var01 += 2;
    }

    // silence warning
    var_A = 0;

    var01 = &(d3_word_196BB0[0]);
    var02[0] = var01[0];
    var02[1] = var01[1];
    var02 = (int16_t *)&(d3_word_196C28[0]);
    var03 = (var01[1] - (int16_t)Game_ViewportMaximumY) & 0x8000;
    var_6 = 32767;
    var_4 = a1;
    a1 = 0;
    for ( --var_4; var_4 != 0; --var_4 )
    {
        var04 = var03;
        var03 = (var01[3] - (int16_t)Game_ViewportMaximumY) & 0x8000;
        if ( var04 )
        {
            var02[0] = var01[0];
            var02[1] = var01[1];
            var02 += 2;

            if ( var01[1] < var_6 )
            {
                var_6 = var01[1];
                var_A = a1;
            }

            ++a1;
        }

        if ( var03 ^ var04 ) // different sign
        {
            if ( var01[1] >= var01[3] )
            {
                var02[0] = var01[2] + ( (((int16_t)Game_ViewportMaximumY - var01[3]) * (int32_t)(var01[2] - var01[0])) / (var01[3] - var01[1]) );
                var02[1] = (int16_t)Game_ViewportMaximumY;
                var02 += 2;
            }
            else
            {
                var02[0] = var01[0] + ( (((int16_t)Game_ViewportMaximumY - var01[1]) * (int32_t)(var01[2] - var01[0])) / (var01[3] - var01[1]) );
                var02[1] = (int16_t)Game_ViewportMaximumY;
                var02 += 2;
            }

          ++a1;
        }

        var01 += 2;
    }

    if ( a1 )
    {
        memorycopy(&(d3_word_196BB0[0]), ((uint8_t *)&(d3_word_196C28[0])) + 4 * var_A, 4 * (a1 - var_A));
        memorycopy(((uint8_t *)&(d3_word_196BB0[0])) + 4 * (a1 - var_A), &(d3_word_196C28[0]), 4 * var_A);
        memorycopy(&(d3_word_196BB0[2 * a1]), &(d3_word_196BB0[0]), 4);
    }

    return a1;
}


static void draw_solid_tile(int32_t number_of_coordinates)
{
//    int16_t var_E;
    int16_t var_C;
    int16_t var_A;
    int16_t y_max;
    int16_t y_min;
    int16_t x_max;
    int16_t x_min;

    int32_t var01; // edx@0
    int32_t coordinate_counter; // ecx@1
    int16_t *current_coordinate; // esi@2
    int8_t partial_tile; // bl@12
    int16_t var10; // bx@22
    int16_t var11; // ax@22
    int16_t *var12; // esi@22
    int16_t var13; // dx@23
    int16_t var14; // cx@23
    uint32_t var17;
    uint32_t var18; // kr00_4@28
    int32_t final_number_of_coordinates; // eax@28


    if ( number_of_coordinates < 3 ) return;

    // silence warning
    var01 = 0;

    coordinate_counter = number_of_coordinates;
    y_min = 32767;
    x_min = 32767;
    y_max = -32767;
    x_max = -32767;
    current_coordinate = (int16_t *)&(d3_word_196C28[0]);

    do
    {
        if ( current_coordinate[0] > x_max )
            x_max = current_coordinate[0];
        if ( current_coordinate[0] < x_min )
            x_min = current_coordinate[0];

        if ( current_coordinate[1] > y_max )
            y_max = current_coordinate[1];
        if ( current_coordinate[1] < y_min )
        {
            y_min = current_coordinate[1];
            var01 = coordinate_counter;
        }

        current_coordinate += 2;
        --coordinate_counter;
    }
    while ( coordinate_counter );

    partial_tile = 0;
    if ( y_min >= (int16_t)Game_ViewportMinimumY )
    {
        if ( y_max > (int16_t)Game_ViewportMaximumY )
        {
            partial_tile = 1;
            if ( y_min > (int16_t)Game_ViewportMaximumY )
                return;
        }
    }
    else
    {
        partial_tile = -1;
        if ( y_max < (int16_t)Game_ViewportMinimumY )
            return;
    }

    if ( x_min >= (int16_t)Game_ViewportMinimumX )
    {
        if ( x_max > (int16_t)Game_ViewportMaximumX )
        {
            partial_tile = 1;
            if ( x_min > (int16_t)Game_ViewportMaximumX )
                return;
        }
    }
    else
    {
        partial_tile = 1;
        if ( x_max < (int16_t)Game_ViewportMinimumX )
            return;
    }

    // rotate the list of coordinates, so that the coordinate with lowest y is first
    memorycopy(&(d3_word_196BB0[0]), ((uint8_t *)&(d3_word_196C28[0])) + 4 * (number_of_coordinates - var01), 4 * var01);
    memorycopy(((uint8_t *)&(d3_word_196BB0[0])) + 4 * var01, &(d3_word_196C28[0]), 4 * (number_of_coordinates - var01));
    memorycopy(&(d3_word_196BB0[2 * number_of_coordinates]), &(d3_word_196BB0[0]), 4);
    var10 = d3_word_196BB0[0];
    var11 = d3_word_196BB0[1];
    var12 = &(d3_word_196BB0[2]);

    // var10,var11 = first coordinate (with lowest y)
    do
    {
        var13 = var12[0];
        var14 = var12[1];
        var12 += 2;
    }
    while ( (var10 == var13) && (var11 == var14) );
    // var13,var14 = next coordinate (different from first)

    var_A = var10 - var13; // horizontal(x) difference between first and next coordinate
    var_C = var11 - var14; // vertical(y) difference between first and next coordinate
    var12 = &(d3_word_196BB0[2 * (number_of_coordinates - 1)]);

    do
    {
        var13 = var12[0];
        var14 = var12[1];
        var12 -= 2;
    }
    while ( (var10 == var13) && (var11 == var14) );
    // var13,var14 = previous coordinate (different from first)

    var17 = (var11 - var14) * (int32_t)var_A;
    var18 = (var10 - var13) * (int32_t)var_C;

    d3_param_byte_144E8D = ( ((var18 & 0xffff0000) | (var17 & 0x0000ffff)) < ((var17 & 0xffff0000) | (var18 & 0x0000ffff)) )?1:0;
    final_number_of_coordinates = number_of_coordinates;

    if ( partial_tile )
    {
        final_number_of_coordinates = d3_sub_C4F7C(number_of_coordinates);
        if ( final_number_of_coordinates < 3 ) return;
    }
    d3_sub_C5B99(final_number_of_coordinates, &(d3_word_196BB0[0]));
}


static void sm4_proc(struct struc_2 *tex_info, uint32_t arg2)
{
    int32_t var_28;
    int32_t var_24;
    int32_t var_20;
    int32_t var_1C;
    int32_t var_18;
    int32_t var_14;
    int32_t var_10;
    int32_t var_C;
    int32_t var_8;
    int32_t var_4;

    int32_t var11; // eax@13
    int32_t var13; // eax@15
    uint8_t *var16; // esi@22
    int32_t var19; // ebx@24
    int32_t var23; // ecx@26
    int32_t var24; // eax@26
    int32_t var27; // eax@29
    int32_t var34; // eax@38
    int8_t *var35; // ebx@38
    uint32_t var36; // ecx@38
    uint32_t var40; // eax@53
    uint32_t var42; // edx@57

    uint8_t *addr;
    uint8_t *pal;
    int32_t counter;
    uint64_t base;

    uint32_t l_dword_143D10_fp16;
    uint32_t l_dword_143D3C_fp16; // fp16
    uint8_t *l_dword_143D44;
    uint64_t l_qword_143D4C_fp32; // fp32
    uint32_t l_dword_143D34[2]; // local variable - must be initialized to 0 !!!
    uint32_t l_dword_143D54;
    uint32_t l_dword_143D48;
    int32_t l_draw_loop_counter; // local variable
    uint32_t l_dword_141498_fp16; // local variable
    uint32_t l_dword_141494_fp16; // local variable
    uint64_t l_qword_14147C_fp32; // local variable


    var_4 = tex_info->viewport_clip_x1;
    var_8 = tex_info->viewport_clip_x2;
    var_C = tex_info->viewport_x1;
    var_10 = tex_info->viewport_x2;

    if (var_10 <= var_C) return;
    if (var_10 < var_4) return;
    if (var_C > var_8) return;

    var_14 = tex_info->viewport_y11;
    var_20 = tex_info->viewport_y12;

    if (var_14 >= var_20) return;
    if (Game_ViewportMaximumY < var_14) return;
    if (Game_ViewportMinimumY > var_20) return;

    l_dword_141494_fp16 = 0;
    l_dword_143D10_fp16 = d3_dword_196CD0 << 16;
    l_dword_141498_fp16 = l_dword_143D10_fp16 / (uint32_t)((var_10 - var_C) + 1);
    if ( var_C < var_4 )
    {
        l_dword_141494_fp16 += (var_4 - var_C) * l_dword_141498_fp16;
        var_C = var_4;
    }
    if ( var_10 > var_8 )
        var_10 = var_8;
    l_draw_loop_counter = var_10 - var_C;

    if ( d3_dword_1A5E34 )
    {
        while ( 1 )
        {
            var11 = (l_dword_143D10_fp16 * (int64_t)((d3_param_dword_1A5E2C * Game_ResizeWidthMult) + ((d3_dword_1A5E1C * (int64_t)(var_C * Game_ResizeWidthDiv)) / g_dword_140004))) / (d3_dword_1A5E34 * Game_ResizeWidthMult);
            if ( var11 >= 0 ) break;
            ++var_C;
        }

        l_dword_141494_fp16 = var11;

        var13 = (l_dword_143D10_fp16 * (int64_t)((d3_param_dword_1A5E2C * Game_ResizeWidthMult) + ((d3_dword_1A5E1C * (int64_t)(var_10 * Game_ResizeWidthDiv)) / g_dword_140004))) / (d3_dword_1A5E34 * Game_ResizeWidthMult);
        if ( (uint32_t) var13 >= l_dword_143D10_fp16 )
            var13 = l_dword_143D10_fp16 - 1;
        l_dword_141498_fp16 = (var13 - var11) / (l_draw_loop_counter + 1);

        if ( var_10 - var_C < 0 ) return;
        l_draw_loop_counter = var_10 - var_C;
    }

    if ( var_20 > Game_ViewportMaximumY )
    {
        var_24 = Game_ViewportMaximumY;
        var_28 = var_20 - Game_ViewportMaximumY;
        var_1C = Game_ViewportMaximumY;
    }
    else
    {
        var_24 = var_20;
        var_28 = 0;
        var_1C = var_20;
    }

    var16 = &(Game_ViewportPtr[(Game_ScreenWidth * (Game_ViewportZeroYRow + Game_ViewportMinimumY)) + var_C + Game_ViewportZeroXColumn]);
    if ( var_14 < Game_ViewportMinimumY )
    {
        var_24 -= Game_ViewportMinimumY;
        var_18 = Game_ViewportMinimumY;
        var19 = Game_ViewportMinimumY - var_14;
        var_28 += var19;

        Game_stru_1414AC[0].texture_y1_fp16 = (((int64_t)(var19 * (int32_t)d3_param_texture_height)) << 16) / (var_24 + var_28 + 1);

        Game_stru_1414AC[0].viewport_first_pixel_addr = var16;
    }
    else
    {
        var_24 -= var_14;
        Game_stru_1414AC[0].texture_y1_fp16 = 0;
        var_18 = var_14;
        Game_stru_1414AC[0].viewport_first_pixel_addr = var16 + (Game_ScreenWidth * (var_14 - Game_ViewportMinimumY));
    }

    if ( d3_dword_1A5E34 )
    {
        var23 = d3_param_texture_height << 16;
        while ( 1 )
        {
            var24 = (var23 * (int64_t)((d3_param_dword_1A5E30 * Game_ResizeHeightMult) + ((d3_param_dword_1A5E14 * (int64_t)(var_18 * Game_ResizeHeightDiv)) / g_dword_140008))) / (d3_dword_1A5E34 * Game_ResizeHeightMult);
            if ( var24 >= 0 ) break;
            if ( var_18 >= Game_ViewportMaximumY ) return;
            ++var_18;
            --var_24;
            Game_stru_1414AC[0].viewport_first_pixel_addr += Game_ScreenWidth;
        }

        Game_stru_1414AC[0].texture_y1_fp16 = var24;

        var27 = (var23 * (int64_t)((d3_param_dword_1A5E30 * Game_ResizeHeightMult) + ((d3_param_dword_1A5E14 * (int64_t)(var_1C * Game_ResizeHeightDiv)) / g_dword_140008))) / (d3_dword_1A5E34 * Game_ResizeHeightMult);
        if ( var27 >= var23 )
            var27 = var23 - 1;
        var_28 = 0;
        l_dword_143D3C_fp16 = var27 - var24;
    }
    else
    {
        l_dword_143D3C_fp16 = d3_param_texture_height << 16;
    }

    if ( !arg2 || (g_dword_13FFBC == NULL) )
    {
// sm4_draw_loop_init:

        l_qword_14147C_fp32 = ((uint64_t) (l_dword_143D3C_fp16 / ((uint32_t)((var_24 + var_28) & 0xffff) + 1))) << 16;

        pal = &(g_palettes[256 * (uint32_t)d3_palette_type]);

// sm4_draw_loop:
        do
        {
            addr = Game_stru_1414AC[0].viewport_first_pixel_addr;
            counter = var_24;
            base = __PAIR_U__((uintptr_t)&d3_texture_column[(((l_dword_141494_fp16 >> 16) & 0xff) << 8) + ((Game_stru_1414AC[0].texture_y1_fp16 >> 16) & 0xff)], Game_stru_1414AC[0].texture_y1_fp16 << 16);

            Game_stru_1414AC[0].viewport_first_pixel_addr++;

            for (; counter >= 0; counter--)
            {
                uint8_t tmp;

                tmp = *((uint8_t *)(uintptr_t) HIDWORD_U(base));
                if (tmp) *addr = pal[tmp];

                addr += Game_ScreenWidth;
                base += l_qword_14147C_fp32;
            }

            l_dword_141494_fp16 += l_dword_141498_fp16;
            l_draw_loop_counter--;
        }
        while (l_draw_loop_counter >= 0);

        return;
    }

    // this code seems to be unreachable, because g_dword_13FFBC is always NULL

    //sm_jump_target = (int (__fastcall *)(_DWORD, _DWORD, _DWORD, _DWORD))((char *)loc_BCC42 - 18 * Game_stru_1414AC[0].texture_height_in_viewport);
    l_dword_143D3C_fp16 = (uint32_t)(d3_param_texture_height << 16) / ((uint16_t)(Game_stru_1414AC[0].texture_height_in_viewport + 1));
    l_qword_143D4C_fp32 = ((uint64_t) l_dword_143D3C_fp16) << 16;

    l_dword_143D34[0] = 0;
    l_dword_143D34[1] = 0;

    do
    {
        l_dword_143D44 = Game_stru_1414AC[0].viewport_first_pixel_addr;
        ++Game_stru_1414AC[0].viewport_first_pixel_addr;
        *(uint32_t *)((uintptr_t)(&(l_dword_143D34[0])) + 2) = Game_stru_1414AC[0].texture_y1_fp16;
        var34 = g_dword_13FFBC[(l_dword_141494_fp16 >> 16) & 0xff];
        var35 = var34 + (int8_t *)(uint16_t *)g_dword_13FFBC;
        var36 = 0;
        l_dword_143D54 = 0;
        l_dword_143D48 = 0;

        while ( var36 != l_dword_143D34[1] )
        {
            if ( var36 > l_dword_143D34[1] )
            {
                var36 -= l_dword_143D34[1];
                l_dword_143D54 -= var36;
                if ( var34 < 0 )
                    var36 = -(int32_t)var36;
                var34 = var36;
                --var35;
                goto LABEL_49;
            }

            var34 = (int32_t)var35[0];
            if ( var34 == 0 )
                goto LABEL_62;

            if ( var34 >= 0 )
            {
                var36 += var34;
                l_dword_143D54 += var34;
                ++var35;
            }
            else
            {
                var36 -= var34;
                l_dword_143D54 -= var34;
                ++var35;
            }
        }

        while ( 1 )
        {
            var34 = (int32_t)var35[0];
LABEL_49:
            if ( var34 == 0 )
                break;

            if ( var34 >= 0 )
            {
                l_dword_143D54 += var34;
                if ( l_dword_143D54 > l_dword_143D34[1] )
                {
                    var40 = ((l_dword_143D3C_fp16 + ((l_dword_143D34[1] + var34) << 16) - *(uint32_t *)((uintptr_t)(&(l_dword_143D34[0])) + 2)) - 1) / l_dword_143D3C_fp16;
                    var42 = var40 + l_dword_143D48;
                    if ( var40 + l_dword_143D48 > (uint32_t)Game_stru_1414AC[0].texture_height_in_viewport )
                    {
                      var40 = (var40 - (var42 - Game_stru_1414AC[0].texture_height_in_viewport)) + 1;
                      var42 = Game_stru_1414AC[0].texture_height_in_viewport;
                    }
                    l_dword_143D48 = var42;
                    counter = var40;
                    base = __PAIR_U__((uintptr_t)&d3_texture_column[(((l_dword_141494_fp16 >> 16) & 0xff) << 8) + (l_dword_143D34[1] & 0xff)], l_dword_143D34[0]);
                    pal = &(g_palettes[256 * (uint32_t)d3_palette_type]);
                    addr = l_dword_143D44;

                    do
                    {
                        *addr = pal[*((uint8_t *)(uintptr_t) HIDWORD_U(base))];
                        base += l_qword_143D4C_fp32;
                        addr += Game_ScreenWidth;
                        --counter;
                    }
                    while ( counter );

                    l_dword_143D44 = addr;
                    l_dword_143D34[0] = (uint32_t)base;
                    l_dword_143D34[1] = (HIDWORD_U(base) - (uint32_t)d3_texture_column) & 0xff;
                    ++var35;
                }
                else
                {
                    ++var35;
                }
            }
            else
            {
                var34 = -var34;
                l_dword_143D54 += var34;
                if ( l_dword_143D54 > l_dword_143D34[1] )
                {
                    var40 = ((l_dword_143D3C_fp16 + ((l_dword_143D34[1] + var34) << 16) - *(uint32_t *)((uintptr_t)(&(l_dword_143D34[0])) + 2)) - 1) / l_dword_143D3C_fp16;

                    if ( var40 + l_dword_143D48 >= (uint32_t)Game_stru_1414AC[0].texture_height_in_viewport )
                        break;

                    l_dword_143D48 += var40;
                    l_dword_143D44 += var40 * Game_ScreenWidth;
                    *(uint32_t *)((uintptr_t)(&(l_dword_143D34[0])) + 2) += l_dword_143D3C_fp16 * var40;
                    ++var35;
                }
                else
                {
                    ++var35;
                }
            }
        }

LABEL_62:
        l_dword_141494_fp16 += l_dword_141498_fp16;
        --l_draw_loop_counter;
    }
    while ( l_draw_loop_counter >= 0 );
}


static uint32_t sm1_subproc(void)
{
    int32_t var01; // eax@1
    uint32_t result_fp16; // eax@3
    uint64_t var03_fp16; // qax@6

    int32_t l_dword_143D1C; // local variable

    l_dword_143D1C = d3_sm_dword_143D08_fp8 >> 8;
    d3_sm_dword_143D08_fp8 += d3_sm_dword_143CFC_fp8;

    var01 = (d3_sm_dword_143D00_fp8 >> 8) + ((int64_t)(d3_sm_dword_143CF4_fp8 * (int64_t)d3_sm_viewport_y) >> 8);
    if ( var01 >= 0 )
    {
        if ( l_dword_143D1C < 0 )
        {
            result_fp16 = 0;
            goto LABEL_9;
        }
    }
    else
    {
        var01 = -var01;
        l_dword_143D1C = -l_dword_143D1C;
        if ( l_dword_143D1C < 0 )
        {
            result_fp16 = 0;
            goto LABEL_9;
        }
    }

    var03_fp16 = d3_sm_texture_height_fp16 * (uint64_t)(uint32_t)var01;
    if ( HIDWORD_U(var03_fp16) < (uint32_t)l_dword_143D1C )
    {
        result_fp16 = (uint32_t)(var03_fp16 / (uint32_t)l_dword_143D1C);
    }
    else
    {
        result_fp16 = d3_sm_texture_height_fp16 - 1;
    }

LABEL_9:
    d3_sm_dword_143D00_fp8 += d3_sm_dword_143CF8_fp8;
    return result_fp16;
}


static void sm123_proc(struct struc_2 *tex_info, int32_t sm123_draw_type)
{
    int32_t var_30;
    int32_t var_2C;
    int32_t var_28;
    int32_t var_24;
    int32_t var_20;
    int32_t var_1C;
    int32_t var_18;
    int32_t var_14;
    int32_t var_10;
    int32_t var_C;
    int32_t var_8;
    int32_t var_4;

    int32_t var08; // ebx@9
    struct struc_1 *var12; // edi@12
    int32_t var14; // eax@14
    int64_t var19; // kr00_8@18
    int32_t var20; // ecx@18
    uint8_t *var24; // esi@21
    int32_t var33; // ecx@28
    int32_t var34; // edx@30

    uint8_t *addr;
    uint8_t *pal;
    int32_t counter;
    uint64_t base;
    uint64_t diff;

    int32_t l_draw_loop_counter; // local variable
    //uint32_t l_dword_141498; // local variable
    uint32_t l_dword_141494; // local variable
    int64_t l_qword_141484_fp32; // fp32 // local variable
    int32_t l_dword_143D04_fp8; // fp8 // local variable
    int32_t l_dword_143D0C_fp8; // fp8 // local variable
    int32_t l_dword_143D28; // local variable
    int32_t l_dword_143D2C; // local variable
    int32_t l_dword_143CE0; // local variable
    int32_t l_dword_143CDC; // local variable
    int64_t l_qword_143CE4; // local variable
    int64_t l_qword_143CEC; // local variable


    var_4 = tex_info->viewport_clip_x1;
    var_8 = tex_info->viewport_clip_x2;
    var_24 = var_C = tex_info->viewport_x1;
    var_28 = var_10 = tex_info->viewport_x2;

    if (var_28 <= var_24) return;
    if (var_28 < var_4) return;
    if (var_24 > var_8) return;

    var_14 = tex_info->viewport_y11;
    var_18 = tex_info->viewport_y21;
    var_1C = tex_info->viewport_y22;
    var_20 = tex_info->viewport_y12;

    if (var_14 >= var_20) return;
    if (var_18 >= var_1C) return;
    if (Game_ViewportMaximumY < var_14) return;
    if (Game_ViewportMinimumY > var_20) return;

// sm123_process_input_data:

    var_2C = var_18 - var_14;
    var_30 = var_1C - var_20;
    l_draw_loop_counter = var_10 - var_C;
    //l_dword_141498 = (d3_dword_196CD0 << 16) / (uint32_t)l_draw_loop_counter;
    //l_dword_141494 = 0;

    if (var_C < var_4)
    {
        var08 = var_4 - var_C;
        //l_dword_141494 += var08 * l_dword_141498;
        var_14 += (var08 * (int64_t)var_2C) / l_draw_loop_counter;
        var_20 += (var08 * (int64_t)var_30) / l_draw_loop_counter;
        var_C = var_4;
    }

    if (var_10 > var_8)
    {
        var08 = var_8 - var_10;
        var_18 += (var08 * (int64_t)var_2C) / l_draw_loop_counter;
        var_1C += (var08 * (int64_t)var_30) / l_draw_loop_counter;
        var_10 = var_8;
    }

    var12 = &(Game_stru_1414AC[0]);
    l_draw_loop_counter = var_10 - var_C;
    var14 = ((var_1C - var_20) << 16) / ((l_draw_loop_counter)?l_draw_loop_counter:1);

    l_qword_141484_fp32 = ((int64_t)var14) << 16;
    d3_sm_dword_143CF4_fp8 = (((int64_t)(d3_param_dword_1A5E14 * Game_ResizeHeightDiv)) << 8) / Game_mul_dword_140008_ResizeHeightMult;
    d3_sm_dword_143CF8_fp8 = (((int64_t)(d3_dword_1A5E20 * Game_ResizeWidthDiv) << 8)) / Game_mul_dword_140004_ResizeWidthMult;
    l_dword_143D04_fp8 = d3_sm_dword_143D00_fp8 = (d3_param_dword_1A5E30 << 8) + var_C * d3_sm_dword_143CF8_fp8;
    d3_sm_dword_143CFC_fp8 = (((int64_t)(d3_dword_1A5E24 * Game_ResizeWidthDiv) << 8)) / Game_mul_dword_140004_ResizeWidthMult;
    l_dword_143D0C_fp8 = d3_sm_dword_143D08_fp8 = (d3_dword_1A5E34 << 8) + var_C * d3_sm_dword_143CFC_fp8;
    d3_sm_texture_height_fp16 = d3_param_texture_height << 16;

    var19 = __PAIR_S__(var_20 + 1, 0);
    var20 = l_draw_loop_counter;
    do
    {
        if ( HIDWORD_S(var19) > Game_ViewportMaximumY )
        {
            d3_sm_viewport_y = Game_ViewportMaximumY;
            var12->texture_height_in_viewport = Game_ViewportMaximumY;
        }
        else
        {
            d3_sm_viewport_y = HIDWORD_S(var19);
            var12->texture_height_in_viewport = HIDWORD_S(var19);
        }
        d3_sm_viewport_y -= 2;
        var12->texture_y2_fp16 = sm1_subproc();
        var19 += l_qword_141484_fp32;
        ++var12;
        var20--;
    }
    while ( var20 >= 0 );

    var14 = ((var_18 - var_14) << 16) / ((l_draw_loop_counter)?l_draw_loop_counter:1);
    l_qword_141484_fp32 = ((int64_t)var14) << 16;
    var24 = &(Game_ViewportPtr[var_C + Game_ViewportZeroXColumn]);
    var12 = &(Game_stru_1414AC[0]);
    var20 = l_draw_loop_counter;
    var24 += Game_ScreenWidth * (Game_ViewportZeroYRow + Game_ViewportMinimumY);
    d3_sm_dword_143D00_fp8 = l_dword_143D04_fp8;
    d3_sm_dword_143D08_fp8 = l_dword_143D0C_fp8;

    var19 = __PAIR_S__(var_14, 0);
    do
    {
        if ( HIDWORD_S(var19) < Game_ViewportMinimumY )
        {
            d3_sm_viewport_y = Game_ViewportMinimumY;
            var12->texture_height_in_viewport -= Game_ViewportMinimumY;
            var12->viewport_first_pixel_addr = var24;
        }
        else
        {
            d3_sm_viewport_y = HIDWORD_S(var19);
            var12->texture_height_in_viewport -= HIDWORD_S(var19);
            var12->viewport_first_pixel_addr = var24 + Game_ScreenWidth * (HIDWORD_S(var19) - Game_ViewportMinimumY);
        }
        var12->texture_y1_fp16 = sm1_subproc();
        ++var12;

        var19 += l_qword_141484_fp32;
        ++var24;
        --var20;
    }
    while ( var20 >= 0 );

    l_dword_143D28 = tex_info->t3.mapgrid_y1;
    l_dword_143D2C = tex_info->t3.mapgrid_y2 - tex_info->t3.mapgrid_y1;

    if ( (tex_info->clip_flags & 1) || ((var_4 - var_24) > 1) )
        var33 = ((Game_mul_dword_140004_ResizeWidthMult * tex_info->t3.mapgrid_x1) / Game_ResizeWidthDiv) - (var_C * tex_info->t3.mapgrid_y1);
    else
        var33 = 0;

    var34 = (var_C * l_dword_143D2C) - ((Game_mul_dword_140004_ResizeWidthMult * (tex_info->t3.mapgrid_x2 - tex_info->t3.mapgrid_x1)) / Game_ResizeWidthDiv);

    if ( var34 < 0 )
    {
        l_dword_143D28 = -l_dword_143D28;
        l_dword_143D2C = -l_dword_143D2C;
        var34 = -var34;
        var33 = -var33;
    }
    l_dword_143CE0 = var34;

    if ( var33 >= 0 )
    {
        if ( var33 >= l_dword_143CE0 )
            var33 = l_dword_143CE0 - 1;
    }
    else
    {
        var33 = 0;
    }
    l_dword_143CDC = var33;

    if ( ((tex_info->clip_flags & 2) == 0) && ((var_28 - var_8) <= 1) )
        l_dword_143D2C = ((l_dword_143CDC + ((l_draw_loop_counter + 1) * -l_dword_143D28)) - l_dword_143CE0) / (l_draw_loop_counter + 1);
    if ( l_dword_143CE0 == 0 ) l_dword_143CE0 = 1;

// sm1_draw_loop_init:

    l_qword_143CE4 = l_dword_143CDC * (int64_t)d3_dword_196CD0;
    l_qword_143CEC = d3_dword_196CD0 * (int64_t)l_dword_143D28;
    var12 = &(Game_stru_1414AC[0]);

    pal = &(g_palettes[256 * (uint32_t)d3_palette_type]);

// sm1_draw_loop:
    switch (sm123_draw_type)
    {
        case 1:
            do
            {
                l_dword_141494 = (int32_t)(l_qword_143CE4 / l_dword_143CE0);
                l_qword_143CE4 -= l_qword_143CEC;
                l_dword_143CE0 += l_dword_143D2C;


                addr = var12->viewport_first_pixel_addr;
                counter = var12->texture_height_in_viewport;
                base = __PAIR_U__((uintptr_t)&d3_texture_column[((l_dword_141494 & 0xff) << 8) + ((var12->texture_y1_fp16 >> 16) & 0xff)], var12->texture_y1_fp16 << 16);
                diff = ((uint64_t) ((var12->texture_y2_fp16 - var12->texture_y1_fp16) / var12->texture_height_in_viewport)) << 16;
                var12++;

                for (; counter >= 0; counter--)
                {
                    *addr = pal[*((uint8_t *)(uintptr_t) HIDWORD_U(base))];
                    addr += Game_ScreenWidth;
                    base += diff;
                }

                l_draw_loop_counter--;
            }
            while (l_draw_loop_counter >= 0);
            return;

        case 2:
            do
            {
                l_dword_141494 = (int32_t)(l_qword_143CE4 / l_dword_143CE0);
                l_qword_143CE4 -= l_qword_143CEC;
                l_dword_143CE0 += l_dword_143D2C;


                addr = var12->viewport_first_pixel_addr;
                counter = var12->texture_height_in_viewport;
                base = __PAIR_U__((uintptr_t)&d3_texture_column[((l_dword_141494 & 0xff) << 8) + ((var12->texture_y1_fp16 >> 16) & 0xff)], var12->texture_y1_fp16 << 16);
                diff = ((uint64_t) ((var12->texture_y2_fp16 - var12->texture_y1_fp16) / var12->texture_height_in_viewport)) << 16;
                var12++;

                for (; counter >= 0; counter--)
                {
                    uint8_t tmp;

                    tmp = *((uint8_t *)(uintptr_t) HIDWORD_U(base));
                    if (tmp) *addr = pal[tmp];

                    addr += Game_ScreenWidth;
                    base += diff;
                }

                l_draw_loop_counter--;
            }
            while (l_draw_loop_counter >= 0);
            return;

        case 3:
            do
            {
                l_dword_141494 = (int32_t)(l_qword_143CE4 / l_dword_143CE0);
                l_qword_143CE4 -= l_qword_143CEC;
                l_dword_143CE0 += l_dword_143D2C;


                addr = var12->viewport_first_pixel_addr;
                counter = var12->texture_height_in_viewport;
                base = __PAIR_U__((uintptr_t)&d3_texture_column[((l_dword_141494 & 0xff) << 8) + ((var12->texture_y1_fp16 >> 16) & 0xff)], var12->texture_y1_fp16 << 16);
                diff = ((uint64_t) ((var12->texture_y2_fp16 - var12->texture_y1_fp16) / var12->texture_height_in_viewport)) << 16;
                var12++;

                for (; counter >= 0; counter--)
                {
                    uint8_t tmp;

                    tmp = *((uint8_t *)(uintptr_t) HIDWORD_U(base));
                    if (tmp)
                    {
                        *addr = pal[tmp];
                    }
                    else
                    {
                        *addr = d3_param_dword_13FFC0[*addr];
                    }

                    addr += Game_ScreenWidth;
                    base += diff;
                }

                l_draw_loop_counter--;
            }
            while (l_draw_loop_counter >= 0);
            return;
    }
}

static void INLINE sm1_proc(struct struc_2 *tex_info)
{
    sm123_proc(tex_info, 1);
}

static void INLINE sm2_proc(struct struc_2 *tex_info)
{
    sm123_proc(tex_info, 2);
}

static void INLINE sm3_proc(struct struc_2 *tex_info)
{
    sm123_proc(tex_info, 3);
}


static uint16_t d3_sub_22BC1(uint32_t a1, uint16_t a2, uint16_t a3)
{
//    uint32_t var_1C;
    uint16_t *var_18;
//    uint16_t var_14;
//    uint16_t var_10;
//    uint32_t var_C;
    uint16_t var_8;
//  uint8_t var_4;


    //var_1C = a1;
    //var_10 = a2;
    //var_14 = a3;
    var_8 = 0;

    if ( ((int32_t)a2 < 2) || ((int32_t)a2 > 8) )
        return 0;

    if ( (a1 & 1) && ((int32_t)a2 > 2) )
        var_18 = &(g_word_151254[24 * ((uint32_t)a2 - 3)]);
    else
        var_18 = &(g_word_151374[24 * ((uint32_t)a2 - 2)]);

    if ( a1 & 8 )
        var_8 = a3 & 7;

    if ( ((a1 & 0x10) == 0) ||
         ( (uint32_t)var_18[(3 * (uint32_t)var_8) + 2] & (1 << ((a3 + (var_18[(3 * (uint32_t)var_8) + 1] >> 8)) & 0xF)) )
       )
        return var_18[3 * var_8] & 7;
    else
        return 0;
}


static uint8_t *d3_sub_1CC6A(uint16_t a1, uint16_t a2, uint32_t a3)
{
    struct struc_6 *var_1C;
    uint8_t *var_18;
//    uint8_t *var_14;
//    uint32_t var_10;
//    uint16_t var_C;
    uint16_t var_8;
//    uint16_t var_4;

    uintptr_t result;


    //var_4 = a1;
    //var_C = a2;
    //var_10 = a3;
    if ( a1 > g_word_1511C4 ) return NULL;

    var_1C = &(g_dword_14A4A6[a1 - 1]);
    var_8 = d3_sub_22BC1(var_1C->field_0, (uint16_t)var_1C->field_4, a2);
    var_18 = (uint8_t *)&(g_word_1501D4[80 * ((uint32_t)a1 - 1) + 10 * (uint32_t)var_8]);
    result = (uintptr_t)sub_8B6BB(*(void **)(var_18 + 6));
    result = (result + 256) & ~(uintptr_t)0xff;
    result += (uintptr_t) *(uint16_t *)var_18;

    return (uint8_t *)result;
}


static uint8_t *d3_sub_1CABA(uint16_t a1, uint16_t a2, uint16_t a3)
{
    uint8_t *var_1C;
    uint8_t *var_18;
//    uint8_t *var_14;
//    uint16_t var_10;
//    uint16_t var_C;
    uint16_t var_8;
//    uint16_t var_4;

    uintptr_t result;


    //var_4 = a1;
    //var_C = a2;
    //var_10 = a3;
    if ( a2 > g_word_1511CA ) return NULL;

    var_1C = &(g_dword_14A4AA[16 * (uint32_t)a2 - 16]);
    if ( a1 == 65535 )
    {
        var_8 = d3_sub_22BC1(*((uint32_t *)var_1C), (uint16_t)var_1C[6], a3);
        var_18 = (uint8_t *)(void *)g_dword_14EBD4[8 * (uint32_t)a2 + (uint32_t)var_8];
        result = (uintptr_t)sub_8B6BB(*(void **)(var_18 + 6));
        result += (uintptr_t) *(uint16_t *)var_18;

        return (uint8_t *)result;
    }

    a1--;
    if ( g_byte_159C71[128 * (uint32_t)a1] & 0x10 )
    {
        var_8 = d3_sub_22BC1(*((uint32_t *)var_1C), (uint16_t)var_1C[6], a3);
    }
    else
    {
        if ( g_byte_159C71[128 * (uint32_t)a1] & 4 )
        {
            var_8 = d3_sub_22BC1(*((uint32_t *)var_1C), (uint16_t)var_1C[6], a3);
        }
        else
        {
            var_8 = ((uint32_t)var_1C[6]) / 2;
            if ( (int32_t)(uint32_t)var_8 >= (int32_t)(uint32_t)var_1C[6] )
                var_8 = 0;
        }
    }

    g_word_159C75[64 * (uint32_t)a1] = var_8;
    var_18 = (uint8_t *)(void *)g_dword_14EBD4[8 * (uint32_t)a2 + (uint32_t)var_8];
    result = (uintptr_t)sub_8B6BB(*(void **)(var_18 + 6));
    result += (uintptr_t) *(uint16_t *)var_18;

    return (uint8_t *)result;
}


static uint8_t *d3_sub_1CD21(uint16_t a1, uint16_t a2)
{
    uint8_t *var_1C;
//    uint8_t * var_18;
    uint8_t *var_14;
//    uint16_t var_10;
    uint16_t var_C;
    uint16_t var_8;
//    uint16_t var_4;

    uintptr_t result;

    //var_4 = a1;
    //var_10 = a2;
    if ( a1 > g_word_1511C2 ) return NULL;

    var_14 = (uint8_t *)(void *)g_dword_14A4AE[(uint32_t)a1 - 1];
    var_8 = (uint16_t)g_byte_151173[a1];

    if ( (var_8 != 2) && ((int32_t)var_8 < 4) )
    {
        var_C = d3_sub_22BC1(*((uint32_t *)var_14), (uint16_t)var_14[6], a2);
    }
    else
    {
        var_C = 8;
    }

    var_1C = (uint8_t *)(void *)g_dword_14A8EC[9 * ((uint32_t)a1 - 1) + (uint32_t)var_C];
    result = (uintptr_t)sub_8B6BB(*(void **)(var_1C + 6));
    result += (uintptr_t) *(uint16_t *)var_1C;

    return (uint8_t *)result;
}


static uint8_t *d3_sub_1CE04(uint16_t a1)
{
    uintptr_t result;

    result = (uintptr_t)sub_8B6BB(g_dword_14FFF0[a1]);
    result = (result + 256) & ~(uintptr_t)0xff;
    return (uint8_t *)result;
}


static int32_t d3_sub_BFBA4(int32_t orig_number_of_coordinates)
{
    int16_t var_11C[120];
    int32_t var_2C;
    int32_t var_28;
    int32_t var_24;
    int16_t var_20;
    int16_t var_1C;


    int16_t *var03; // ebp@1
    int16_t *var04; // ecx@1
    int16_t var07; // dx@2
    int16_t *var14; // ebp@11
    int16_t *var15; // ecx@11
    int16_t var17; // dx@12


    var_24 = orig_number_of_coordinates;
    d3_word_199A58[4 * orig_number_of_coordinates] = d3_word_199A58[0];
    d3_word_199A58[4 * orig_number_of_coordinates + 1] = d3_word_199A58[1];
    d3_word_199A58[4 * orig_number_of_coordinates + 2] = d3_word_199A58[2];
    d3_word_199A58[4 * orig_number_of_coordinates + 3] = d3_word_199A58[3];
    var03 = &(d3_word_199A58[0]);
    var04 = &(var_11C[0]);
    var_1C = ((int16_t)Game_ViewportMinimumX - d3_word_199A58[0]) & 0x8000;
    var_28 = var_24 + 1;
    var_24 = 1;

    for ( --var_28; var_28 != 0; --var_28 )
    {
        var07 = var_1C;
        var_1C = ((int16_t)Game_ViewportMinimumX - var03[4]) & 0x8000;
        if ( var07 )
        {
            var04[0] = var03[0];
            var04[1] = var03[1];
            var04[2] = var03[2];
            var04[3] = var03[3];
            var04 += 4;
            var_24++;
        }

        if ( (int32_t)var07 != (int32_t)var_1C )
        {
            if ( var03[1] >= var03[5] )
            {
                var04[1] = var03[5] + (int16_t)( ((Game_ViewportMinimumX - (int32_t)var03[4]) * (int64_t)((int32_t)var03[1] - (int32_t)var03[5])) / ((int32_t)var03[0] - (int32_t)var03[4]) );
            }
            else
            {
                var04[1] = var03[1] + (int16_t)( ((Game_ViewportMinimumX - (int32_t)var03[0]) * (int64_t)((int32_t)var03[5] - (int32_t)var03[1])) / ((int32_t)var03[4] - (int32_t)var03[0]) );
            }

            var04[0] = (int16_t)Game_ViewportMinimumX;
            var04 += 4;
            var_24++;
        }

        var03 += 4;
    }

    var04[0] = var_11C[0];
    var04[1] = var_11C[1];
    var04[2] = var_11C[2];
    var04[3] = var_11C[3];
    var14 = &(var_11C[0]);
    var15 = &(d3_word_199A58[0]);
    var_20 = (var_11C[0] - (int16_t)Game_ViewportMaximumX) & 0x8000;
    var_2C = var_24;
    var_24 = 0;

    for ( --var_2C; var_2C != 0; --var_2C )
    {
        var17 = var_20;
        var_20 = (var14[4] - (int16_t)Game_ViewportMaximumX) & 0x8000;

        if ( var17 )
        {
            var15[0] = var14[0];
            var15[1] = var14[1];
            var15[2] = var14[2];
            var15[3] = var14[3];
            var15 += 4;
            ++var_24;
        }

        if ( (int32_t)var17 != (int32_t)var_20 )
        {
            if ( var14[1] >= var14[5] )
            {
                var15[1] = var14[5] + (int16_t)( ((Game_ViewportMaximumX - (int32_t)var14[4]) * (int64_t)((int32_t)var14[1] - (int32_t)var14[5])) / ((int32_t)var14[0] - (int32_t)var14[4]) );
            }
            else
            {
                var15[1] = var14[1] + (int16_t)( ((Game_ViewportMaximumX - (int32_t)var14[0]) * (int64_t)((int32_t)var14[5] - (int32_t)var14[1])) / ((int32_t)var14[4] - (int32_t)var14[0]) );
            }

            var15[0] = (int16_t)Game_ViewportMaximumX;
            var15 += 4;
            ++var_24;
        }

        var14 += 4;
    }

    return var_24;
}


static int32_t d3_mul_div_roundup(int32_t a1, int32_t a2, int32_t a3)
{
    int64_t var03; // qax@1
    int32_t var04; // ecx@1


    var03 = a2 * (int64_t)a1;
    var04 = a3 >> 1;
    if ( (HIDWORD_S(var03) ^ a3) < 0 )    // different sign
        var04 = -var04;
    var03 += (int64_t)var04;

    return (int32_t)(var03 / a3);
}


static void draw_floor_ceiling_tile(int32_t _mapobject_distance, int32_t *_firstline_mapgrid_point, int32_t *_nextline_mapgrid_point)
{
    int32_t var_9_[10]; // mapgrid coordinates
    int16_t var_6_[10]; // texture coordinates
    int32_t var_54;
    int32_t var_50;
    uint32_t var_4C;
    int32_t var_48;
    int32_t var_44;
    int32_t var_40;
//    int32_t var_3C;
    int32_t mapobject_distance;
    int32_t partial_tile;
    int32_t *nextline_mapgrid_point;
    int32_t *firstline_mapgrid_point;
    int32_t var_28;
    int32_t var_24;
    int32_t var_20;
    int32_t number_of_coordinates;
    int32_t var_18;
    uint8_t var_14;


    int16_t *current_texture_point; // edi@12
    int16_t *var06; // esi@12
    int32_t *current_mapgrid_point; // ecx@12
    uint32_t var08; // ebp@13
    int32_t var12; // ebp@17
    int16_t *var23; // eax@25
    int16_t x_min; // cx@25
    int16_t y_min; // dx@25
    int16_t x_max; // si@25
    int16_t y_max; // di@25
    int32_t var28; // ebp@25
    struct struc_9 *iii; // eax@54
    int32_t var32; // eax@69
    int32_t var40; // ebx@83
    int32_t var41; // eax@83
    int32_t var42; // esi@83
    int32_t var43; // ecx@83


    mapobject_distance = _mapobject_distance;
    firstline_mapgrid_point = _firstline_mapgrid_point;
    nextline_mapgrid_point = _nextline_mapgrid_point;

    if ( d3_param_dword_1A5E00 >= 0 )
    {
        // floor
        var_14 = d3_param_current_mapdata_ptr[1];
        if ( (var_14 == 0) || (d3_param_dword_1A5E00 == 0) )
            return;
        var_44 = g_dword_143D70;
        var_28 = Game_ViewportMaximumY;
    }
    else
    {
        // ceiling
        var_14 = d3_param_current_mapdata_ptr[2];
        if ( var_14 == 0 )
            return;
        if ( (g_dword_196CEC - g_dword_14000C) == d3_param_dword_1A5E00 )
            var_44 = g_dword_143D6C;
        else
            var_44 = d3_mul_div_roundup(g_dword_140008, d3_param_dword_1A5E00, g_viewport_minimum_y);
        var_28 = Game_ViewportMinimumY;
    }

    if ( var_44 - d3_mapgrid_point_distance_3_4 > mapobject_distance )
        return;

    partial_tile = 0;
    if ( var_44 + d3_mapgrid_point_distance_3_4 <= mapobject_distance )
    {
        // texture coordinates
        d3_word_199A58[2] = 0;
        d3_word_199A58[3] = 0;
        d3_word_199A58[6] = g_word_196D0E;
        d3_word_199A58[7] = 0;
        d3_word_199A58[10] = g_word_196D0E;
        d3_word_199A58[11] = g_word_196D0E;
        d3_word_199A58[14] = 0;
        d3_word_199A58[15] = g_word_196D0E;
        // viewport coordinates
        d3_word_199A58[0] = (int16_t)((firstline_mapgrid_point[0] * (int64_t)Game_mul_dword_140004_ResizeWidthMult) / (firstline_mapgrid_point[1] * Game_ResizeWidthDiv));
        d3_word_199A58[1] = (int16_t)((d3_param_dword_1A5E00 * (int64_t)Game_mul_dword_140008_ResizeHeightMult) / (firstline_mapgrid_point[1] * Game_ResizeHeightDiv));
        d3_word_199A58[4] = (int16_t)((firstline_mapgrid_point[2] * (int64_t)Game_mul_dword_140004_ResizeWidthMult) / (firstline_mapgrid_point[3] * Game_ResizeWidthDiv));
        d3_word_199A58[5] = (int16_t)((d3_param_dword_1A5E00 * (int64_t)Game_mul_dword_140008_ResizeHeightMult) / (firstline_mapgrid_point[3] * Game_ResizeHeightDiv));
        d3_word_199A58[8] = (int16_t)((nextline_mapgrid_point[2] * (int64_t)Game_mul_dword_140004_ResizeWidthMult) / (nextline_mapgrid_point[3] * Game_ResizeWidthDiv));
        d3_word_199A58[9] = (int16_t)((d3_param_dword_1A5E00 * (int64_t)Game_mul_dword_140008_ResizeHeightMult) / (nextline_mapgrid_point[3] * Game_ResizeHeightDiv));
        d3_word_199A58[12] = (int16_t)((nextline_mapgrid_point[0] * (int64_t)Game_mul_dword_140004_ResizeWidthMult) / (nextline_mapgrid_point[1] * Game_ResizeWidthDiv));
        number_of_coordinates = 4;
        d3_word_199A58[13] = (int16_t)((d3_param_dword_1A5E00 * (int64_t)Game_mul_dword_140008_ResizeHeightMult) / (nextline_mapgrid_point[1] * Game_ResizeHeightDiv));
    }
    else
    {
        var_9_[0] = firstline_mapgrid_point[0];
        var_9_[1] = firstline_mapgrid_point[1];
        var_9_[2] = firstline_mapgrid_point[2];
        var_9_[3] = firstline_mapgrid_point[3];
        var_9_[4] = nextline_mapgrid_point[2];
        var_9_[5] = nextline_mapgrid_point[3];
        var_9_[6] = nextline_mapgrid_point[0];
        var_9_[7] = nextline_mapgrid_point[1];
        current_texture_point = &(var_6_[0]);
        var06 = &(d3_word_199A58[0]);
        current_mapgrid_point = &(var_9_[0]);
        var_9_[8] = firstline_mapgrid_point[0];
        var_9_[9] = firstline_mapgrid_point[1];
        var_6_[3] = 0;
        var_6_[6] = 0;
        var_6_[0] = 0;
        var_6_[1] = 0;
        var_6_[8] = 0;
        var_6_[2] = g_word_196D0E;
        var_6_[4] = g_word_196D0E;
        var_6_[5] = g_word_196D0E;
        var_6_[7] = g_word_196D0E;
        var_6_[9] = 0;
        var_4C = (var_44 < var_9_[1])?1:0;
        var_48 = 5;
        number_of_coordinates = 0;

        for (--var_48; var_48 != 0; --var_48)
        {
            var08 = var_4C;
            var_4C = (var_44 < current_mapgrid_point[3])?1:0;
            if ( var08 )
            {
                // texture coordinates
                var06[2] = current_texture_point[0];
                var06[3] = current_texture_point[1];
                // viewport coordinates
                var06[0] = (int16_t)((current_mapgrid_point[0] * (int64_t)Game_mul_dword_140004_ResizeWidthMult) / (current_mapgrid_point[1] * Game_ResizeWidthDiv));
                var06[1] = (int16_t)((d3_param_dword_1A5E00 * (int64_t)Game_mul_dword_140008_ResizeHeightMult) / (current_mapgrid_point[1] * Game_ResizeHeightDiv));
                var06 += 4;
                ++number_of_coordinates;
            }

            if ( var_4C ^ var08 ) // var_4C != var08
            {
                if ( current_mapgrid_point[1] >= current_mapgrid_point[3] )
                {
                    var12 = var_44 - current_mapgrid_point[3];
                    var_18 = current_mapgrid_point[1] - current_mapgrid_point[3];
                    var_20 = current_mapgrid_point[2] + ( (var12 * (int64_t)(current_mapgrid_point[0] - current_mapgrid_point[2])) / var_18 );
                }
                else
                {
                    var12 = var_44 - current_mapgrid_point[1];
                    var_18 = current_mapgrid_point[3] - current_mapgrid_point[1];
                    var_20 = current_mapgrid_point[0] + ( (var12 * (int64_t)(current_mapgrid_point[2] - current_mapgrid_point[0])) / var_18 );
                }

                // texture coordinates
                var06[2] = (int16_t)(current_texture_point[0] + ( (var12 * (int64_t)((int32_t)current_texture_point[2] - (int32_t)current_texture_point[0])) / var_18 ));
                var06[3] = (int16_t)(current_texture_point[1] + ( (var12 * (int64_t)((int32_t)current_texture_point[3] - (int32_t)current_texture_point[1])) / var_18 ));
                // viewport coordinates
                var06[0] = (int16_t)((var_20 * (int64_t)Game_mul_dword_140004_ResizeWidthMult) / (var_44 * Game_ResizeWidthDiv));
                var06[1] = var_28;
                var06 += 4;
                number_of_coordinates++;
            }

            current_texture_point += 2;
            current_mapgrid_point += 2;
        }

        if ( number_of_coordinates < 3 )
            return;
        partial_tile = 1;
    }

    // find minimal/maximal x/y
    var23 = &(d3_word_199A58[0]);
    x_min = d3_word_199A58[0];
    y_min = d3_word_199A58[1];
    x_max = x_min;
    y_max = y_min;
    for (var28 = number_of_coordinates; var28 != 0; --var28)
    {
        if ( x_min <= var23[0] )
        {
            if ( x_max < var23[0] )
                x_max = var23[0];
        }
        else
        {
            x_min = var23[0];
        }

        if ( y_min <= var23[1] )
        {
            if ( y_max < var23[1] )
                y_max = var23[1];
        }
        else
        {
            y_min = var23[1];
        }

        var23 += 4;
    }

    if ((int32_t)x_min > Game_ViewportMaximumX) return;
    if ((int32_t)x_max < Game_ViewportMinimumX) return;
    if ((int32_t)y_min > Game_ViewportMaximumY) return;
    if ((int32_t)y_max < Game_ViewportMinimumY) return;

    if ( d3_param_dword_1A5E00 >= 0 )
    {
        if ( (int32_t)y_max < d3_param_dword_143D78 ) return;
    }
    else
    {
        if ( (int32_t)y_min > d3_param_dword_143D74 ) return;
    }

    if ( (int32_t)x_min < Game_ViewportMinimumX )
    {
        x_min = (int16_t)Game_ViewportMinimumX;
        partial_tile = 1;
    }

    if ( (int32_t)x_max > Game_ViewportMaximumX )
    {
        x_max = (int16_t)Game_ViewportMaximumX;
        partial_tile = 1;
    }

    if ( (int32_t)y_min < Game_ViewportMinimumY )
    {
        y_min = (int16_t)Game_ViewportMinimumY;
        partial_tile = 1;
    }

    if ( (int32_t)y_max > Game_ViewportMaximumY )
    {
        y_max = (int16_t)Game_ViewportMaximumY;
        partial_tile = 1;
    }

    if ( (d3_stru_194A80[0].viewport_x1 != -32700) && (x_min >= d3_stru_194A80[0].viewport_x1) )
    {
        var_24 = Game_ViewportMinimumX;
        for ( iii = &(d3_stru_194A80[0]); iii->viewport_x1 != -32700; iii++ )
        {
            if ( x_min <= iii->viewport_x2 )
            {
                if ( (x_max < iii->viewport_x1) || ((x_min < iii->viewport_x1) && ((int32_t)iii->viewport_x1 > var_24 + 1)) || (y_max > iii->viewport_y2) || (y_min < iii->viewport_y1) )
                    break;
                if ( x_max <= iii->viewport_x2 )
                    return;
                var_24 = (int32_t)iii->viewport_x2;
            }
        }
    }

    if ( partial_tile )
    {
        number_of_coordinates = d3_sub_BFBA4(number_of_coordinates);
        if ( number_of_coordinates < 3 )
            return;
    }

    if ( d3_param_dword_1A5E04 & 2 )
    {
        d3_palette_type = 0;
        goto LABEL_82;
    }

    var32 = (int32_t)(mapobject_distance * (int32_t)d3_param_word_196D0C) >> g_dword_13FFC4;
    if ( var32 >= 0 )
    {
        if ( var32 > 1023 )
            var32 = 1023;
    }
    else
    {
        var32 = 0;
    }

    d3_palette_type = (d3_param_palette_depth_index[var32] < 0)?0:d3_param_palette_depth_index[var32];
    if ( d3_palette_type < 64 )
        goto LABEL_82;

    if ( (d3_param_dword_1A5E04 & 0x80) == 0 )
    {
        if ( number_of_coordinates > 0 )
        {
            int16_t *var34; // eax@77
            int16_t *var35; // edx@77
            int32_t var36; // ebx@77

            // copy coordinates from d3_word_199A58 to d3_word_196C28
            var34 = (int16_t *)&(d3_word_196C28[0]);
            var35 = &(d3_word_199A58[0]);
            var36 = 0;

            do
            {
                ++var36;
                var34[0] = var35[0];
                var34[1] = var35[1];
                var34 += 2;
                var35 += 4;
            }
            while ( var36 < number_of_coordinates );
        }

        //set_tile_color(g_word_14A4BC);
        d3_param_tile_color = (uint8_t)g_word_14A4BC; // inlined set_tile_color
        draw_solid_tile(number_of_coordinates);

        return;
    }

    d3_palette_type = 63;

LABEL_82:
    d3_texture_column = d3_sub_1CC6A((uint16_t)var_14, (uint16_t)(uint32_t)d3_param_current_mapdata_ptr, (mapobject_distance > 4000)?1:0);
    if ( d3_texture_column == NULL ) return;

    var40 = firstline_mapgrid_point[1];
    var41 = firstline_mapgrid_point[0];
    var42 = nextline_mapgrid_point[0] - firstline_mapgrid_point[0];
    var_54 = nextline_mapgrid_point[1] - firstline_mapgrid_point[1];
    d3_param_dword_1A5E2C = d3_param_dword_1A5E00 * var42;
    d3_dword_1A5E1C = d3_param_dword_1A5E00 * var_54;
    var_40 = var40 * var42;
    var_50 = firstline_mapgrid_point[2] - var41;
    var43 = firstline_mapgrid_point[3] - var40;
    d3_param_dword_1A5E2C = -d3_param_dword_1A5E2C;
    d3_param_dword_1A5E30 = d3_param_dword_1A5E00 * var_50;
    d3_param_dword_1A5E10 = var_40 - var41 * var_54;
    d3_param_dword_1A5E14 = var43 * var41 - var_50 * var40;
    d3_dword_1A5E20 = var43 * d3_param_dword_1A5E00;
    d3_param_dword_1A5E18 = var_54 * var_50 - var42 * var43;
    d3_dword_1A5E20 = -d3_dword_1A5E20;

    draw_textured_tile(number_of_coordinates, d3_param_dword_1A5E00, (partial_tile || (6 * g_mapgrid_point_distance > mapobject_distance))?1:0);
}

static void sm1234_hyperproc(struct struc_2 *tex_info)
{
    int32_t var_20;
    //int32_t var_1C;

    int32_t var05; // ebx@19
    int32_t var08; // edx@27
    int32_t var10; // eax@27
    uint16_t *var11; // eax@29
    uint16_t *var17; // eax@34
    int32_t var20; // edx@35
    int32_t var22; // eax@35


    if ( tex_info->mapobject_type_flags & 8 )
    {
        d3_palette_type = 0;
    }
    else
    {
        int32_t var03; // edx@1
        int8_t var04; // dl@7

        var03 = ((int32_t)(tex_info->distance * (int32_t)d3_param_word_196D0C)) >> g_dword_13FFC4;
        if ( var03 >= 0 )
        {
            if ( var03 > 1023 ) var03 = 1023;
        }
        else
        {
            var03 = 0;
        }

        var04 = d3_param_palette_depth_index[var03];
        if ( (tex_info->mapobject_type_flags & 7) == 3 )
            var04 += tex_info->field_0E;
        if ( var04 < 0 )
            var04 = 0;
        d3_palette_type = var04;
    }

    switch(tex_info->mapobject_type_flags & 7)
    {
        case 1:
        case 2:
            if ( d3_palette_type >= 64 )
            {
                if ( (tex_info->mapobject_type_flags & 0x60) == 0 ) return;
                d3_palette_type = 63;
            }

            d3_texture_column = d3_sub_1CABA((uint32_t)(tex_info->field_0A), (uint32_t)(tex_info->field_0C), (uint32_t)(tex_info->field_08));
            if (d3_texture_column == NULL) return;

            // silence warning
            var_20 = 0;

            if ( tex_info->field_3C == tex_info->field_40 )
            {
                d3_dword_1A5E34 = 0;
            }
            else
            {
                var08 = tex_info->field_40 - tex_info->field_3C;
                var_20 = g_dword_196CEC - tex_info->field_40;
                var10 = tex_info->t3.mapgrid_x2 - tex_info->t3.mapgrid_x1;
                d3_dword_1A5E1C = -(tex_info->t3.mapgrid_y1 * var08);
                d3_param_dword_1A5E2C = tex_info->t3.mapgrid_x1 * var08;
                d3_param_dword_1A5E14 = -(var10 * tex_info->t3.mapgrid_y1);
                d3_param_dword_1A5E30 = var10 * var_20;
                d3_dword_1A5E34 = -(var08 * var10);
            }

            var11 = (uint16_t *)&(g_dword_14A4AA[16 * (uint32_t)(tex_info->field_0C) - 16]);
            d3_dword_196CD0 = var11[4];
            d3_param_texture_height = var11[5];
            sm4_proc(tex_info, var_20);
            break;

        case 3:
            if ( d3_palette_type >= 64 )
            {
                if ( (tex_info->mapobject_type_flags & 0x40) == 0 )
                {
                    //set_tile_color(g_word_14A4BC);
                    d3_param_tile_color = (uint8_t)g_word_14A4BC; // inlined set_tile_color

                    d3_word_196C28[0] = (int16_t)tex_info->viewport_x1;
                    d3_word_196C28[1] = (int16_t)tex_info->viewport_y11 - 1;
                    d3_word_196C28[2] = (int16_t)tex_info->viewport_x2;
                    d3_word_196C28[3] = (int16_t)tex_info->viewport_y21 - 1;
                    d3_word_196C28[4] = (int16_t)tex_info->viewport_x2;
                    d3_word_196C28[5] = (int16_t)tex_info->viewport_y22 + 1;
                    d3_word_196C28[6] = (int16_t)tex_info->viewport_x1;
                    d3_word_196C28[7] = (int16_t)tex_info->viewport_y12 + 1;

                    draw_solid_tile(4);
                    return;
                }
                d3_palette_type = 63;
            }

            var17 = (uint16_t *)(void *) g_dword_14A4AE[tex_info->field_0C];
            if (((uintptr_t)var17) < 1024) return; // fix reading from NULL pointer
            d3_dword_196CD0 = var17[5];
            d3_param_texture_height = var17[6];
            d3_texture_column = d3_sub_1CD21((uint16_t)(tex_info->field_0C + 1), (uint16_t)(tex_info->field_08));
            if (d3_texture_column == NULL) return;

            var_20 = g_dword_196CEC - g_dword_14000C;
            var20 = tex_info->t3.mapgrid_x2 - tex_info->t3.mapgrid_x1;
            var22 = tex_info->t3.mapgrid_y2 - tex_info->t3.mapgrid_y1;
            d3_param_dword_1A5E30 = var20 * var_20;
            d3_param_dword_1A5E14 = var22 * tex_info->t3.mapgrid_x1 - var20 * tex_info->t3.mapgrid_y1;
            d3_dword_1A5E34 = -(g_dword_14000C * var20);
            d3_dword_1A5E20 = -(var22 * var_20);
            d3_dword_1A5E24 = g_dword_14000C * var22;

            if ( tex_info->without_transparent_pixels )
            {
                sm1_proc(tex_info);
            }
            else
            {
                if ( tex_info->mapobject_type_flags & 0x10 )
                {
                    d3_param_dword_13FFC0 = d3_sub_1CE04((uint16_t)(tex_info->field_0C + 1));
                    if ( d3_param_dword_13FFC0 != NULL )
                        sm3_proc(tex_info);
                }
                else
                {
                    sm2_proc(tex_info);
                }
            }
            break;

        case 4:
            d3_param_current_mapdata_ptr = tex_info->t4.mapdata_ptr;
            if ( g_byte_13FFAA )
                var05 = g_dword_14000C;
            else
                var05 = ((uint32_t) g_dword_14A4A6[(uint32_t)tex_info->field_0C - 1].field_8) << g_byte_13FFAE;

            d3_param_dword_1A5E04 = g_dword_14A4A6[(uint32_t)tex_info->field_0C - 1].field_0;
            d3_param_dword_1A5E00 = g_dword_196CEC - var05;

            draw_floor_ceiling_tile(tex_info->distance, tex_info->t4.mapgrid_point_ptr, tex_info->t4.mapgrid_next_line_point_ptr);
        default:
            break;
    }
}


static void draw_list_sm1234(void)
{
    uint8_t guard_pixel_check_value;
    uint8_t guard_pixel_last_value;

    uint8_t *guard_pixel_ptr; // ebx@0
    struct struc_2 *guard_nearest_mapobject; // edi@0 // nearest map object which overwrites the guard check pixel
    int32_t index_position;
    struct struc_2 *current_mapobject; // edx@6

    if ( d3_skip_draw_list_sm1234 ) return;

    // silence warning
    guard_pixel_check_value = guard_pixel_last_value = 0;
    guard_pixel_ptr = NULL;

    if ( g_select_mapobject != NULL )
    {
        guard_pixel_check_value = g_select_mapobject->check_value;
        guard_pixel_ptr = &(Game_ViewportPtr[Game_ScreenWidth * convert_vertical(g_select_mapobject->ypos) + convert_horizontal(g_select_mapobject->xpos)]);
        // remember the last value at guard position
        guard_pixel_last_value = *guard_pixel_ptr;
        guard_nearest_mapobject = NULL;
        // write check value to guard position
        *guard_pixel_ptr = guard_pixel_check_value;
    }

    for ( index_position = d3_number_of_mapobjects - 1; index_position >= 0; index_position-- )
    {
        current_mapobject = d3_mapobjects_list_index[index_position].mapobject_ptr;
        if ( (current_mapobject != NULL) && current_mapobject->draw_mapobject )
        {
            sm1234_hyperproc(current_mapobject);

            if ( g_select_mapobject != NULL )
            {
                // if check value was overwritten at guard position ...
                if ( *guard_pixel_ptr != guard_pixel_check_value )
                {
                    // remember the last value at guard position
                    guard_pixel_last_value = *guard_pixel_ptr;
                    // remember the map object which overwrote the guard pixel
                    guard_nearest_mapobject = current_mapobject;
                    // write check value to guard position
                    *guard_pixel_ptr = guard_pixel_check_value;
                }
            }
        }
    }

    if ( g_select_mapobject != NULL )
    {
        // restore last value at guard position
        *guard_pixel_ptr = guard_pixel_last_value;
        if ( guard_nearest_mapobject != NULL )
        {
            g_select_mapobject->mapobject_type = guard_nearest_mapobject->mapobject_type_flags & 7;
            g_select_mapobject->mapdata_x_plus1 = guard_nearest_mapobject->mapdata_x + 1;
            g_select_mapobject->mapdata_y_plus1 = guard_nearest_mapobject->mapdata_y + 1;
            g_select_mapobject->field_7 = (uint8_t)guard_nearest_mapobject->field_06;
        }
        g_select_mapobject = NULL;
    }
}


static void draw_floor_and_ceiling(void)
{
    int32_t var_58;
    int32_t *var_54;
    int32_t mapdata_current_y;
    int32_t nextline_offset;
    int32_t mapdata_end_y;
    uint8_t *currentline_mapdata_ptr;
    int32_t mapdata_start_x;
    int32_t var_3C;
    int32_t mapdata_end_x;
    uint8_t *guard_pixel_ptr;
    int32_t mapdata_current_x;
    uint8_t guard_pixel_check_value;
    uint8_t guard_pixel_last_value;
    uint8_t var_1C;


    int32_t *firstline_mapgrid_point; // ecx@4
    int32_t var08; // ebp@9
    int32_t *nextline_mapgrid_point; // esi@9
    int32_t distance; // edi@12


    if ( d3_mapgrid_number_of_points <= 0 ) return;

    // silence warning
    guard_pixel_ptr = NULL;
    guard_pixel_check_value = guard_pixel_last_value = 0;

    if ( g_select_mapobject != NULL )
    {
        guard_pixel_check_value = g_select_mapobject->check_value;
        guard_pixel_ptr = &(Game_ViewportPtr[convert_vertical(g_select_mapobject->ypos) * Game_ScreenWidth + convert_horizontal(g_select_mapobject->xpos)]);
        // remember the last value at guard position
        guard_pixel_last_value = *guard_pixel_ptr;
        // write check value to guard position
        *guard_pixel_ptr = guard_pixel_check_value;
    }

    mapdata_current_y = g_mapdata_height - (d3_mapgrid_start_position_y + 1);
    mapdata_end_y = mapdata_current_y - d3_mapgrid_height;
    mapdata_start_x = d3_mapgrid_start_position_x;
    mapdata_end_x = d3_mapgrid_width + d3_mapgrid_start_position_x;
    currentline_mapdata_ptr = &(g_mapdata_ptr[3 * d3_mapgrid_start_position_x + 3 * mapdata_current_y * g_mapdata_width]);
    var_58 = (3 * g_mapgrid_point_distance) >> 2;
    firstline_mapgrid_point = &(d3_mapgrid_points[0]);

    if ( g_dword_143D6C < g_dword_143D70 )
    {
        var_3C = g_dword_143D6C - d3_mapgrid_point_distance_3_4;
    }
    else
    {
        var_3C = g_dword_143D70 - d3_mapgrid_point_distance_3_4;
    }

    nextline_offset = 2 * (d3_mapgrid_width + 1);

    while ( mapdata_current_y > mapdata_end_y )
    {
        d3_param_current_mapdata_ptr = currentline_mapdata_ptr;
        var_1C = 0;
        mapdata_current_x = mapdata_start_x;

        if ( mapdata_start_x < mapdata_end_x )
        {
            var08 = 2 * mapdata_end_x - 2 * mapdata_start_x;
            nextline_mapgrid_point = &firstline_mapgrid_point[nextline_offset];

            do
            {
                if ( (d3_param_current_mapdata_ptr[1] == 0) && (d3_param_current_mapdata_ptr[2] == 0) )
                    goto LABEL_24;

                distance = (firstline_mapgrid_point[1] >> 1) + (nextline_mapgrid_point[3] >> 1);
                var_54 = &(firstline_mapgrid_point[var08]);
                if ( distance < var_3C )
                {
                    if ( var_1C )
                    {
                        firstline_mapgrid_point = var_54;
                        break;
                    }
                    goto LABEL_24;
                }

                if ( (int32_t)((abs((nextline_mapgrid_point[2] >> 1) + (firstline_mapgrid_point[0] >> 1)) - var_58) * g_dword_140004) > distance * g_viewport_maximum_x )
                {
                    if ( !var_1C )
                        goto LABEL_24;

                    firstline_mapgrid_point = var_54;
                    break;
                }

                d3_param_dword_1A5E00 = g_dword_196CEC;
                var_1C = 1;
                if (d3_param_current_mapdata_ptr[1]) // fix bug in original code
                {
                    d3_param_dword_1A5E04 = g_dword_14A4A6[(uint32_t)d3_param_current_mapdata_ptr[1] - 1].field_0;
                    draw_floor_ceiling_tile(distance, firstline_mapgrid_point, nextline_mapgrid_point); // floor
                }

                if ( !d3_without_ceiling && d3_param_current_mapdata_ptr[2] )
                {
                    d3_param_dword_1A5E00 -= g_dword_14000C;
                    d3_param_dword_1A5E04 = g_dword_14A4A6[(uint32_t)d3_param_current_mapdata_ptr[2] - 1].field_0;
                    draw_floor_ceiling_tile(distance, firstline_mapgrid_point, nextline_mapgrid_point); // ceiling
                }

                // if check value was overwritten at guard position ...
                if ( (g_select_mapobject != NULL) && (guard_pixel_check_value != *guard_pixel_ptr) )
                {
                    g_select_mapobject->mapobject_type = 5;
                    g_select_mapobject->mapdata_x_plus1 = mapdata_current_x + 1;
                    g_select_mapobject->mapdata_y_plus1 = mapdata_current_y + 1;
                    // remember the last value at guard position
                    guard_pixel_last_value = *guard_pixel_ptr;
                    // write check value to guard position
                    *guard_pixel_ptr = guard_pixel_check_value;
                }

            LABEL_24: // loc_C0C50
                var08 -= 2;
                nextline_mapgrid_point += 2;
                firstline_mapgrid_point += 2;
                ++mapdata_current_x;
                d3_param_current_mapdata_ptr += 3;
            }
            while ( mapdata_current_x < mapdata_end_x );
        }

        firstline_mapgrid_point += 2;
        --mapdata_current_y;
        currentline_mapdata_ptr -= 3 * g_mapdata_width;
    }

    if ( g_select_mapobject != NULL )
    {
        // restore last value at guard position
        *guard_pixel_ptr = guard_pixel_last_value;
    }
}


static void draw_scaledres_sky(uint8_t *texture_ptr, uint8_t *column_ptr, int32_t starty, int32_t column_height, int32_t texture_width, int32_t a6, uint32_t a7, uint32_t factor)
{
    int32_t var_4;

    uint32_t counter1;
    uint8_t *dst, *src;
    uint32_t counter2;
    uint32_t fpcounter, fpdiff;

    uint32_t jjj; // esi@5
    uint32_t var11; // ecx@8

    uint32_t counter3;


    if ( column_height <= 0 ) return;

    column_ptr += starty * Game_ScreenWidth;

    for ( var_4 = ((texture_width * a6) * (int64_t)(-32 * (int32_t)g_view_angle_fp14)) / 16384; var_4 < 0; var_4 += 32 * texture_width ) ;

    counter1 = 0;
    fpcounter = 0;
    fpdiff = ((g_viewport_width - 1) << 16) / (Game_ViewportWidth - 1);
    do
    {
        jjj = (((uint32_t)g_word_1966B0[fpcounter >> 16]) * (0x10000 - (fpcounter & 0xffff)) + ((uint32_t)g_word_1966B0[(fpcounter >> 16) + 1]) * (fpcounter & 0xffff)) >> 16;
        for ( jjj = ((int32_t) (var_4 + jjj)) >> 5; jjj >= (uint32_t)texture_width; jjj -= texture_width ) ;

        dst = column_ptr;
        var11 = column_height;
        src = &(texture_ptr[jjj]);
        column_ptr++;

        if ( a7 )
        {
            for (counter3 = a7; counter3 != 0; counter3--)
            {
                *dst = *src;
                dst += Game_ScreenWidth;
                var11--;
                if ( var11 == 0 ) break;
            }

            src += texture_width;
        }

        for (counter2 = var11; counter2 >= factor; counter2 -= factor)
        {
            for (counter3 = factor; counter3 != 0; counter3--)
            {
                *dst = *src;
                dst += Game_ScreenWidth;
            }
            src += texture_width;
        }

        for (; counter2 > 0; counter2--)
        {
            *dst = *src;
            dst += Game_ScreenWidth;
        }

        fpcounter+=fpdiff;
        counter1++;
    }
    while ( counter1 < (uint32_t)Game_ViewportWidth );
}


static void draw_hires_sky(uint8_t *texture_ptr, uint8_t *column_ptr, int32_t starty, int32_t column_height, int32_t texture_width, int32_t a6, uint32_t a7)
{
    int32_t var_4;

    uint32_t counter1;
    uint8_t *dst, *src;
    bool doloop;
    uint32_t counter2;
    uint32_t fpcounter, fpdiff;

    uint32_t jjj; // esi@5
    uint32_t var11; // ecx@8


    if ( column_height <= 0 ) return;

    column_ptr += starty * Game_ScreenWidth;

    for ( var_4 = ((texture_width * a6) * (int64_t)(-32 * (int32_t)g_view_angle_fp14)) / 16384; var_4 < 0; var_4 += 32 * texture_width ) ;

    counter1 = 0;
    fpcounter = 0;
    fpdiff = ((g_viewport_width - 1) << 16) / (Game_ViewportWidth - 1);
    do
    {
        jjj = (((uint32_t)g_word_1966B0[fpcounter >> 16]) * (0x10000 - (fpcounter & 0xffff)) + ((uint32_t)g_word_1966B0[(fpcounter >> 16) + 1]) * (fpcounter & 0xffff)) >> 16;
        for ( jjj = ((int32_t) (var_4 + jjj)) >> 5; jjj >= (uint32_t)texture_width; jjj -= texture_width ) ;

        dst = column_ptr;
        var11 = column_height;
        src = &(texture_ptr[jjj]);
        column_ptr++;

        doloop = true;
        if ( a7 )
        {
            *dst = *src;
            src += texture_width;
            dst += Game_ScreenWidth;
            var11--;
            if ( var11 == 0 ) doloop = false;
        }

        if ( doloop )
        {
            for (counter2 = var11 >> 1; counter2 > 0; counter2--)
            {
                dst[0] = *src;
                dst[Game_ScreenWidth] = *src;
                src += texture_width;
                dst += 2 * Game_ScreenWidth;
            }

            if ( var11 & 1 )
            {
                *dst = *src;
            }
        }

        fpcounter+=fpdiff;
        counter1++;
    }
    while ( counter1 < (uint32_t)Game_ViewportWidth );
}


static void draw_lores_sky(uint8_t *texture_ptr, uint8_t *column_ptr, int32_t starty, int32_t column_height, int32_t texture_width, int32_t a6)
{
    int32_t var_4; // 0 .. (32 * texture_width - 1)

    uint32_t counter1;
    uint8_t *dst, *src;
    uint32_t counter2;
    uint32_t fpcounter, fpdiff;

    uint32_t jjj; // esi@4

    if ( column_height <= 0 ) return;

    column_ptr += starty * Game_ScreenWidth;

    for ( var_4 = ((texture_width * a6) * (int64_t)(-32 * (int32_t)g_view_angle_fp14)) / 16384; var_4 < 0; var_4 += 32 * texture_width ) ;

    counter1 = 0;
    fpcounter = 0;
    fpdiff = ((g_viewport_width - 1) << 16) / (Game_ViewportWidth - 1);
    do
    {
        jjj = (((uint32_t)g_word_1966B0[fpcounter >> 16]) * (0x10000 - (fpcounter & 0xffff)) + ((uint32_t)g_word_1966B0[(fpcounter >> 16) + 1]) * (fpcounter & 0xffff)) >> 16;
        for ( jjj = ((int32_t) (var_4 + jjj)) >> 5; jjj >= (uint32_t)texture_width; jjj -= texture_width ) ;

        dst = column_ptr;
        src = &(texture_ptr[jjj]);
        column_ptr++;

        for (counter2 = column_height; counter2 > 0; counter2--)
        {
            *dst = *src;
            src += texture_width;
            dst += Game_ScreenWidth;
        }

        fpcounter+=fpdiff;
        counter1++;
    }
    while ( counter1 < (uint32_t)Game_ViewportWidth );
}


static void viewport_fillchar(int32_t startlineoffset, int32_t endlineoffset, uint8_t fillvalue)
{
    uint8_t *dstptr; // edi@1
    uint8_t *endptr; // esi@1

    dstptr = &(Game_ViewportPtr[startlineoffset]);
    endptr = &(Game_ViewportPtr[endlineoffset]);

    while ( dstptr < endptr )
    {
        memset(dstptr, fillvalue, Game_ViewportWidth);
        dstptr += Game_ScreenWidth;
    };
}


static void draw_background(int32_t endlineoffset) // endlineoffset is allways screen width * viewport sky height
{
    uint32_t var_28;
    uint32_t sky_hires;
    int32_t factor;
    int32_t var_20;
    int32_t var_1C;


    uint8_t *var02; // ebp@1
    int32_t var06; // edi@6
    int32_t var07; // esi@8
    int32_t var08; // ebx@10
    uint8_t var12; // bl@15
    int32_t var13; // eax@15


    var02 = g_sky_texture_ptr;
    if ( (var02 == NULL) || (g_word_14A496 == 0) || (g_sky_texture_width == 0) )
    {
        viewport_fillchar(0, endlineoffset, g_fillvalue1);
        viewport_fillchar(endlineoffset, Game_ViewportHeight * Game_ScreenWidth, g_fillvalue2);
        return;
    }

    // silence warning
    var_28 = 0;

    sky_hires = (Game_ViewportHeight > 200)?1:0;
    if ( !sky_hires )
    {
        factor = 1;
        var07 = Game_ViewportZeroYRow - (int32_t)g_word_14A494;
        var06 = (uint32_t)g_sky_texture_height;
    }
    else
    {
        factor = 2;
        while ((int32_t)Game_ViewportHeight > factor * 200) factor++;

        var07 = Game_ViewportZeroYRow - factor * (int32_t)g_word_14A494;
        var06 = factor * (uint32_t)g_sky_texture_height;
    }

    var_1C = var07 + var06;
    if ( var07 > 0 )
    {
        var_20 = var07 * Game_ScreenWidth;
        if ( var07 > Game_ViewportZeroYRow )
        {
            viewport_fillchar(0, endlineoffset, g_fillvalue1);

            if ( (uint32_t)var07 >= (uint32_t)Game_ViewportHeight )
            {
                viewport_fillchar(endlineoffset, Game_ViewportHeight * Game_ScreenWidth, g_fillvalue2);
                return;
            }

            var13 = endlineoffset;
            var12 = g_fillvalue2;
        }
        else
        {
            var12 = g_fillvalue1;
            var13 = 0;
        }

        viewport_fillchar(var13, var_20, var12);
    }
    else
    {
        if ( sky_hires )
        {
            if (factor == 2)
            {
                var08 = -((var07 >> 1) * (int32_t)(uint32_t)g_sky_texture_width);
                var_28 = var07 & 1;
            }
            else
            {
                var08 = -((var07 / factor) * (int32_t)(uint32_t)g_sky_texture_width);
                var_28 = var07 % factor;
            }
        }
        else
        {
            var08 = -(var07 * (int32_t)(uint32_t)g_sky_texture_width);
        }

        var06 += var07;
        var02 += var08;
        var07 = 0;

        if ( var06 <= 0 )
        {
            viewport_fillchar(0, endlineoffset, g_fillvalue1);
            viewport_fillchar(endlineoffset, Game_ViewportHeight * Game_ScreenWidth, g_fillvalue1);
            return;
        }
    }

    if ( (uint32_t)var_1C < (uint32_t)Game_ViewportHeight )
    {
        viewport_fillchar(var_1C * Game_ScreenWidth, Game_ViewportHeight * Game_ScreenWidth, g_fillvalue1);
    }
    else
    {
        var06 = Game_ViewportHeight - var07;
        if ( var06 <= 0 )
            return;
    }

    if ( sky_hires )
    {
        if (factor == 2)
            draw_hires_sky(var02, Game_ViewportPtr, var07, var06, (uint32_t)g_sky_texture_width, (uint32_t)g_word_14A496, var_28);
        else
            draw_scaledres_sky(var02, Game_ViewportPtr, var07, var06, (uint32_t)g_sky_texture_width, (uint32_t)g_word_14A496, var_28, factor);
    }
    else
        draw_lores_sky(var02, Game_ViewportPtr, var07, var06, (uint32_t)g_sky_texture_width, (uint32_t)g_word_14A496);
}


static void INLINE memorymove_words(const void *src, void *dst, int32_t words_count)
{
    if (words_count > 0) memmove(dst, src, 2 * words_count);
}


static void d3_sub_BF644(struct struc_2 *tex_info)
{
    struct struc_9 var_48;
    struct struc_9 var_40;
//int32_t var_38;
//int32_t var_34;
//int32_t var_30;
    int32_t words_count;
    int32_t var_28;
    int32_t var_24;
    int32_t var_20;
    uint8_t var_1C;


    int32_t var04; // ecx@1
    struct struc_9 *var06; // edi@5
    struct struc_9 *var07; // esi@5
    struct struc_9 *iii; // eax@9
    int32_t var12; // edx@19
    uint8_t lll; // bl@31
    struct struc_9 *var26; // esi@63
    struct struc_9 *var27; // ebx@63


    tex_info->draw_mapobject = 0;
    var_20 = tex_info->viewport_x1;
    var_24 = tex_info->viewport_y11;
    var04 = tex_info->viewport_x2;
    if ( var_24 <= tex_info->viewport_y21 )
        var_24 = tex_info->viewport_y21;
    var_28 = tex_info->viewport_y22;
    if ( var_28 >= tex_info->viewport_y12 )
        var_28 = tex_info->viewport_y12;
    var06 = NULL;
    var07 = NULL;

    if ( var_20 < Game_ViewportMinimumX )
    {
        var_20 = Game_ViewportMinimumX;
        tex_info->clip_flags |= 1;
    }

    if ( var04 > Game_ViewportMaximumX )
    {
        var04 = Game_ViewportMaximumX;
        tex_info->clip_flags |= 2u;
    }

    tex_info->viewport_clip_x1 = Game_ViewportMinimumX;
    tex_info->viewport_clip_x2 = Game_ViewportMaximumX;

    for ( iii = &(d3_stru_194A80[0]); iii->viewport_x1 != -32700 ; iii++ )
    {
        if ( (((int32_t) iii->viewport_x1) <= var_20) && (((int32_t) iii->viewport_x2) >= var_20) )
            var06 = iii;

        if ( (((int32_t) iii->viewport_x1) <= var04) && (var04 <= ((int32_t) iii->viewport_x2)) )
            var07 = iii;
    }

    if ( var06 == var07 )
    {
        if ( var07 )
        {
            if ( g_dword_14000C >= tex_info->field_40 ) return;
            if ( ((int32_t) var07->viewport_y1) <= tex_info->viewport_y11 ) return;
            if ( ((int32_t) var07->viewport_y1) <= Game_ViewportMinimumY ) return;

            tex_info->draw_mapobject = 1;
            return;
        }

        if ( !tex_info->without_transparent_pixels )
        {
            tex_info->draw_mapobject = 1;
            return;
        }

        for ( iii = &(d3_stru_194A80[0]); iii->viewport_x1 != -32700; iii++ )
        {
            if ( ((int32_t) iii->viewport_x1) > var04 )
                break;
        }

        var26 = iii;
        var27 = iii;

        while ( 1 )
        {
            iii++;
            memorycopy(&var_48, var26, sizeof(struct struc_9));

            if ( var_48.viewport_x1 == -32700 )
                break;

            memorycopy(&var_40, iii, sizeof(struct struc_9));
            memorycopy(iii, &var_48, sizeof(struct struc_9));

            var26 = &var_40;
        }

        memorycopy(iii, &var_48, sizeof(struct struc_9));
        var27->viewport_x1 = (int16_t) var_20;
        var27->viewport_x2 = (int16_t) var04;
        var27->viewport_y1 = (int16_t) var_24;
        var27->viewport_y2 = (int16_t) var_28;
        tex_info->draw_mapobject = 1;
        d3_dword_143D68++;

        return;
    }


    var12 = tex_info->viewport_y21;
    if ( tex_info->viewport_y11 < var12 )
        var12 = tex_info->viewport_y11;

    for ( var_1C = 1; var06 != NULL; var06++ )
    {
        if ( var_1C && ((var12 >= (int32_t)var06->viewport_y1) || ((int32_t)var06->viewport_y1 <= Game_ViewportMinimumY)) )
            tex_info->viewport_clip_x1 = ((int32_t)var06->viewport_x2) + 1;
        else
            var_1C = 0;

        if ( var06[1].viewport_x1 == -32700 ) break;
        if ( (int32_t)var06[1].viewport_x1 > ((int32_t)var06->viewport_x2) + 1 ) break;
        if ( (int32_t)var06[1].viewport_x2 >= var04 ) break;
    }

    for ( lll = 1; var07 != NULL; var07-- )
    {
        if ( lll && ((var12 >= (int32_t)var07->viewport_y1) || ((int32_t)var07->viewport_y1 <= Game_ViewportMinimumY)) )
            tex_info->viewport_clip_x2 = ((int32_t)var07->viewport_x1) - 1;
        else
            lll = 0;

        if ( var07 <= &(d3_stru_194A80[0]) ) break;
        if ( (int32_t)var07[-1].viewport_x2 < ((int32_t)var07->viewport_x1) - 1 ) break;
        if ( (int32_t)var07[-1].viewport_x1 <= var_20 ) break;
    }

    words_count = 4 * ((d3_dword_143D68 + 1) - (((intptr_t)var07 - (intptr_t)&(d3_stru_194A80[0])) >> 3));

    if ( var06 == NULL )
    {
        if ( tex_info->without_transparent_pixels )
        {
            ++d3_dword_143D68;
            memorymove_words(var07, var07 + 1, words_count);
            var07->viewport_x1 = (int16_t)var_20;
            var07->viewport_x2 = var07[1].viewport_x1 - 1;
            var07->viewport_y1 = (int16_t)var_24;
            var07->viewport_y2 = (int16_t)var_28;
        }
        tex_info->draw_mapobject = 1;

        return;
    }

    if ( var07 == NULL )
    {
        if ( tex_info->without_transparent_pixels )
        {
            ++d3_dword_143D68;
            memorymove_words(var06, var06 + 1, 4 * (d3_dword_143D68 - (((intptr_t)var06 - (intptr_t)&(d3_stru_194A80[0])) >> 3)));
            var06[1].viewport_x2 = (int16_t)var04;
            var06[1].viewport_x1 = var06->viewport_x2 + 1;
            var06[1].viewport_y1 = (int16_t)var_24;
            var06[1].viewport_y2 = (int16_t)var_28;
        }
        tex_info->draw_mapobject = 1;

        return;
    }

    if ( tex_info->viewport_clip_x2 < tex_info->viewport_clip_x1 ) return;

    if ( tex_info->without_transparent_pixels )
    {
        ++d3_dword_143D68;
        memorymove_words(var07, var07 + 1, words_count);
        var07->viewport_x1 = var06->viewport_x2 + 1;
        var07->viewport_x2 = var07[1].viewport_x1 - 1;
        var07->viewport_y1 = (int16_t)var_24;
        var07->viewport_y2 = (int16_t)var_28;
    }
    tex_info->draw_mapobject = 1;
}


static void d3_sub_BF9BC(void)
{
    int32_t iii; // ebx@1
    struct struc_2 *var02; // eax@2
    int32_t var03; // ebx@6
    struct struc_9 *var04; // eax@6 // struct ??? (i16, i16, i16, i16)


    for ( iii = 0; iii < d3_number_of_mapobjects; ++iii )
    {
        var02 = d3_mapobjects_list_index[iii].mapobject_ptr;
        if ( (var02 != NULL) && var02->draw_mapobject && ((var02->mapobject_type_flags & 7) != 4) )
            d3_sub_BF644(var02);
    }

    d3_param_dword_143D74 = Game_ViewportMinimumY;
    var03 = Game_ViewportMinimumX;
    d3_param_dword_143D78 = Game_ViewportMaximumY;
    var04 = &(d3_stru_194A80[0]);

    while ( var04->viewport_x1 != -32700 )
    {
        if ( ((int32_t)var04->viewport_x1) - var03 > 1 )
        {
            d3_param_dword_143D74 = 0;
            d3_param_dword_143D78 = 0;
            break;
        }

        if ( ((int32_t) var04->viewport_y2) < d3_param_dword_143D78 )
            d3_param_dword_143D78 = (int32_t) var04->viewport_y2;
        if ( ((int32_t) var04->viewport_y1) > d3_param_dword_143D74 )
            d3_param_dword_143D74 = (int32_t) var04->viewport_y1;

        var03 = (int32_t) var04->viewport_x2;
        var04++;
    }

    if ( var03 < Game_ViewportMaximumX )
    {
        d3_param_dword_143D74 = 0;
        d3_param_dword_143D78 = 0;
    }
}


static void insert_into_list_sm1234(void)
{
    struct struc_2 *current_mapobject; // edi@2
    int32_t minindex; // edx@4
    int32_t maxindex; // ebx@4

    if ( d3_number_of_mapobjects >= D3_MAXIMUM_NUMBER_OF_MAPOBJECTS )
    {
        current_mapobject = d3_mapobjects_list_index[D3_MAXIMUM_NUMBER_OF_MAPOBJECTS - 1].mapobject_ptr;
        if ( d3_stru_1999E8.distance >= current_mapobject->distance ) return;
    }
    else
    {
        d3_mapobjects_list_index[d3_number_of_mapobjects].sortkey = 0x7fffffff;
        current_mapobject = d3_next_mapobject_ptr;
        d3_number_of_mapobjects++;
        d3_next_mapobject_ptr++;
    }

    minindex = 0;
    maxindex = d3_number_of_mapobjects - 1;
    while ( maxindex - minindex >= 2 )
    {
        if ( d3_stru_1999E8.distance > d3_mapobjects_list_index[(minindex + maxindex) >> 1].sortkey )
            minindex = (minindex + maxindex) >> 1;
        else
            maxindex = (minindex + maxindex) >> 1;
    }

    if ( d3_stru_1999E8.distance > d3_mapobjects_list_index[minindex].sortkey )
        minindex = maxindex;

    memorymove_words(&(d3_mapobjects_list_index[minindex]), &(d3_mapobjects_list_index[minindex + 1]), 4 * (d3_number_of_mapobjects - minindex - 1));
    d3_mapobjects_list_index[minindex].mapobject_ptr = current_mapobject;
    d3_mapobjects_list_index[minindex].sortkey = d3_stru_1999E8.distance;
    *current_mapobject = d3_stru_1999E8;
}


static void d3_sub_BED08(uint16_t a1, uint16_t a2, int32_t a3, int32_t a4, uint8_t mapobject_type)
{
    int32_t var_38;
//int32_t var_34;
//    int32_t var_30;
    uint32_t var_2C;
//    int32_t var_28;
    int32_t var_24;
    int32_t var_20;
    int32_t var_1C;
    int32_t var_18;
    uint8_t *var_14; // struct ???
//    uint16_t var_10;


    int16_t *var05; // edi@1
    int32_t var06; // ebx@3
    int32_t var07; // esi@3
    int32_t var08; // ebp@3
    int32_t var09; // esi@3
    int32_t var10; // ebx@4
    int32_t var12; // ecx@7
    int32_t var14; // ebx@8


//    var_10 = a1;
//    var_28 = a3;
//    var_30 = a4
    var_2C = 8 * (uint32_t) d3_stru_1999E8.field_08;
    d3_stru_1999E8.draw_mapobject = 1;
    d3_stru_1999E8.without_transparent_pixels = 0;
    d3_stru_1999E8.mapobject_type_flags = mapobject_type;
    d3_stru_1999E8.field_0A = a1;
    ++d3_dword_143D64;
    var_18 = 0;
    var05 = &(g_dword_14A4A2[(33 * (uint32_t)a2) + 1]);

    do
    {
        if ( var05[3] )
        {
            var_14 = &(g_dword_14A4AA[(16 * (uint32_t) ((uint16_t *)var05)[3]) - 16]);
            var06 = ((int32_t)var05[1]) << g_byte_13FFAE;
            var07 = ((int32_t)var05[0]) << g_byte_13FFAE;
            var08 = ((int32_t)var05[2]) << g_byte_13FFAE;
            var_20 = ((var06 * g_sin_view_angle_fp14 + var07 * g_cos_view_angle_fp14) >> 14) + a3;
            var09 = ((g_cos_view_angle_fp14 * var06 - g_sin_view_angle_fp14 * var07) >> 14) + a4;

            if ( var09 >= g_dword_140000 )
            {
                var10 = ((int32_t) *((uint16_t *) &(var_14[12]))) << g_byte_13FFAE;
                var_1C = ((int32_t) *((uint16_t *) &(var_14[14]))) << g_byte_13FFAE;

                if ( g_dword_14000C + 200 < var_1C )
                    d3_stru_1999E8.mapobject_type_flags |= 0x20;

                var10 >>= 1;
                d3_stru_1999E8.t3.mapgrid_x1 = var_20 - var10;
                d3_stru_1999E8.t3.mapgrid_y1 = var09;
                d3_stru_1999E8.t3.mapgrid_y2 = var09;
                d3_stru_1999E8.t3.mapgrid_x2 = var_20 + var10;
                d3_stru_1999E8.distance = var09;

                if ( g_dword_140004 * (abs(var_20) - var10) <= var09 * g_viewport_maximum_x )
                {
                    var12 = g_dword_196CEC - var08;
                    var_38 = var12 - var_1C;
                    d3_stru_1999E8.viewport_x1 = (Game_mul_dword_140004_ResizeWidthMult * (int64_t)d3_stru_1999E8.t3.mapgrid_x1) / (var09 * Game_ResizeWidthDiv);
                    d3_stru_1999E8.viewport_x2 = (Game_mul_dword_140004_ResizeWidthMult * (int64_t)d3_stru_1999E8.t3.mapgrid_x2) / (var09 * Game_ResizeWidthDiv);

                    if ( var_14[0] & 4 )
                    {
                        var_24 = (Game_mul_dword_140008_ResizeHeightMult * (int64_t)var12) / (var09 * Game_ResizeHeightDiv);
                        var14 = abs( (int32_t)(var_24 - ( (Game_mul_dword_140008_ResizeHeightMult * (int64_t)var12) / ((var09 + (var_1C >> 1)) * Game_ResizeHeightDiv) )) );
                        if ( var14 < 2 )
                            var14 = 2;
                        d3_stru_1999E8.viewport_y11 = var_24 - var14;
                        d3_stru_1999E8.field_3C = var08;
                        d3_stru_1999E8.field_40 = var08;
                        d3_stru_1999E8.viewport_y22 = var_24 + var14;
                    }
                    else
                    {
                        d3_stru_1999E8.viewport_y11 = (Game_mul_dword_140008_ResizeHeightMult * (int64_t)var_38) / (var09 * Game_ResizeHeightDiv);
                        d3_stru_1999E8.viewport_y22 = (Game_mul_dword_140008_ResizeHeightMult * (int64_t)var12) / (var09 * Game_ResizeHeightDiv);
                        d3_stru_1999E8.field_3C = var08;
                        d3_stru_1999E8.field_40 = var08 + var_1C;
                    }

                    d3_stru_1999E8.viewport_y12 = d3_stru_1999E8.viewport_y22;
                    d3_stru_1999E8.viewport_y21 = d3_stru_1999E8.viewport_y11;

                    if ( Game_ViewportMinimumY <= d3_stru_1999E8.viewport_y22 )
                    {
                        d3_stru_1999E8.field_0C = var05[3];
                        d3_stru_1999E8.mapobject_type_flags &= 0xB7;
                        if ( var_14[0] & 2 )
                            d3_stru_1999E8.mapobject_type_flags |= 8;
                        if ( var_14[0] & 0x80 )
                            d3_stru_1999E8.mapobject_type_flags |= 0x40;
                        if ( (int32_t)(int8_t)mapobject_type == 2 )
                            d3_stru_1999E8.field_06 = a1;
                        else
                            d3_stru_1999E8.field_06 = var_18;
                        d3_stru_1999E8.field_08 = var_18 + var_2C;
                        insert_into_list_sm1234();
                    }
                }
            }
        }

        var05 += 4;
        ++var_18;
    }
    while ( var_18 < 8 );
}


static void d3_sub_BF4D4(void)
{
    int32_t mapgrid_end_position_y;
    int32_t mapgrid_end_position_x;
    int32_t mapgrid_start_position_y;
    int32_t mapgrid_start_position_x;
    int32_t var_20;
    uint32_t var_1C;


    struct struc_5 *var00; // edi@1
    int32_t var01; // ebp@3
    int32_t var02; // ecx@3
    int32_t var03; // ebx@7
    int32_t var04; // esi@7
    int32_t var05; // ecx@7
    int32_t var06; // ebp


    var00 = g_dword_14A4B2;
    if ( var00 == NULL ) return;

    mapgrid_start_position_x = d3_mapgrid_start_position_x;
    mapgrid_end_position_x = d3_mapgrid_width + d3_mapgrid_start_position_x;
    mapgrid_start_position_y = d3_mapgrid_start_position_y;
    mapgrid_end_position_y = d3_mapgrid_height + d3_mapgrid_start_position_y;
    d3_stru_1999E8.without_transparent_pixels = 0;
    var_1C = 0;
    var_20 = 2 * g_mapgrid_point_distance;

    if ( !g_word_14A4B6 ) return;

    do
    {
        var01 = (var00->field_0 << g_byte_13FFAE) >> g_dword_13FFFC;
        var02 = (var00->field_4 << g_byte_13FFAE) >> g_dword_13FFFC;

        if ( (var01 >= mapgrid_start_position_x) && (var01 < mapgrid_end_position_x) && (var02 >= mapgrid_start_position_y) && (var02 < mapgrid_end_position_y) )
        {
            var03 = (var00->field_4 << g_byte_13FFAE) - d3_dword_196CA4;
            var04 = (var00->field_0 << g_byte_13FFAE) - d3_dword_196CA8;
            var05 = (var03 * g_cos_view_angle_fp14 - var04 * g_sin_view_angle_fp14) >> 14;
            var06 = (var04 * g_cos_view_angle_fp14 + var03 * g_sin_view_angle_fp14) >> 14;

            if ( var05 + var_20 >= g_dword_140000 )
            {
                if ( (var05 * g_viewport_maximum_x) >= ((abs(var06) - var_20) * g_dword_140004) )
                {
                    d3_stru_1999E8.field_08 = (uint16_t) ((var04 + var03) << 8) + (var_1C & 0xff);
                    d3_sub_BED08(var00->field_A, var00->field_8, var06, var05, 2);
                }
            }
        }

        ++var00;
        ++var_1C;
    }
    while ( (uint16_t)var_1C < g_word_14A4B6 );
}


static void fill_stru_1999E8_1(int32_t a1, int32_t a2, int32_t a3, int32_t a4)
{
    d3_stru_1999E8.viewport_x1 = (Game_mul_dword_140004_ResizeWidthMult * (int64_t)a1) / (a2 * Game_ResizeWidthDiv);
    d3_stru_1999E8.viewport_x2 = (Game_mul_dword_140004_ResizeWidthMult * (int64_t)a3) / (a4 * Game_ResizeWidthDiv);
    d3_stru_1999E8.viewport_y11 = (Game_mul_dword_140008_ResizeHeightMult * (int64_t)(g_dword_196CEC - g_dword_14000C)) / (a2 * Game_ResizeHeightDiv);
    d3_stru_1999E8.viewport_y21 = (Game_mul_dword_140008_ResizeHeightMult * (int64_t)(g_dword_196CEC - g_dword_14000C)) / (a4 * Game_ResizeHeightDiv);
    d3_stru_1999E8.viewport_y12 = (Game_mul_dword_140008_ResizeHeightMult * (int64_t)g_dword_196CEC) / (a2 * Game_ResizeHeightDiv);
    d3_stru_1999E8.viewport_y22 = (Game_mul_dword_140008_ResizeHeightMult * (int64_t)g_dword_196CEC) / (a4 * Game_ResizeHeightDiv);
}


static int32_t fill_stru_1999E8_2(void)
{
//int32_t var_68;
//int32_t var_64;
//int32_t var_60;
//int32_t var_5C;
//int32_t var_58;
//int32_t var_54;
//int32_t var_50;
//int32_t var_4C;
//int32_t var_48;
    int32_t var_44;
    int32_t var_40;
    int32_t var_3C;
    int32_t var_38;
    int32_t var_34;
    int32_t var_30;
    int32_t var_2C;
    int32_t var_28;
    uint8_t var_24;
    uint8_t var_20;
    uint8_t var_1C;


    int32_t var03; // edi@9
    int32_t var04; // ebp@9
    uint8_t var05; // bl@9
    int32_t var06; // esi@17
    int32_t var07; // ecx@17
    int32_t var08; // ebx@19
    int32_t var36; // eax@71


    if ( d3_stru_1999E8.t3.mapgrid_x2 * d3_stru_1999E8.t3.mapgrid_y1 < d3_stru_1999E8.t3.mapgrid_y2 * d3_stru_1999E8.t3.mapgrid_x1 )
    {
        int32_t var01; // edx@5

        if ( (d3_stru_1999E8.mapobject_type_flags & 7) != 3 || d3_stru_1999E8.without_transparent_pixels )
            return 0;

        // exchange x1 and x2
        var01 = d3_stru_1999E8.t3.mapgrid_x1;
        d3_stru_1999E8.t3.mapgrid_x1 = d3_stru_1999E8.t3.mapgrid_x2;
        d3_stru_1999E8.t3.mapgrid_x2 = var01;

        // exchange y1 and y2
        var01 = d3_stru_1999E8.t3.mapgrid_y1;
        d3_stru_1999E8.t3.mapgrid_y1 = d3_stru_1999E8.t3.mapgrid_y2;
        d3_stru_1999E8.t3.mapgrid_y2 = var01;
    }

    if ( (g_dword_140000 > d3_stru_1999E8.t3.mapgrid_y1) && (g_dword_140000 > d3_stru_1999E8.t3.mapgrid_y2) )
        return 0;

    var_30 = 0;
    var_34 = 0;
    var03 = d3_stru_1999E8.t3.mapgrid_y1;
    d3_stru_1999E8.clip_flags = 0;
    var_2C = d3_stru_1999E8.t3.mapgrid_x1;
    var_28 = d3_stru_1999E8.t3.mapgrid_x2;
    var04 = d3_stru_1999E8.t3.mapgrid_y2;
    var05 = (d3_stru_1999E8.t3.mapgrid_y1 * g_viewport_minimum_x < g_dword_140004 * d3_stru_1999E8.t3.mapgrid_x1)?1:0;
    var_24 = (d3_stru_1999E8.t3.mapgrid_y1 * g_viewport_maximum_x > g_dword_140004 * d3_stru_1999E8.t3.mapgrid_x1)?1:0;
    var_1C = (d3_stru_1999E8.t3.mapgrid_y2 * g_viewport_minimum_x < g_dword_140004 * d3_stru_1999E8.t3.mapgrid_x2)?1:0;
    var_20 = (d3_stru_1999E8.t3.mapgrid_y2 * g_viewport_maximum_x > g_dword_140004 * d3_stru_1999E8.t3.mapgrid_x2)?1:0;
    if ( !var05 )
        var_24 = 1;
    if ( !var_20 )
        var_1C = 1;

    if ((var_1C | var05) == 0) return 0;
    if ((var_24 | var_20) == 0) return 0;

    if (((var05 & var_24 & var_1C) & var_20) == 0)
    {
        var06 = d3_stru_1999E8.t3.mapgrid_x1 - d3_stru_1999E8.t3.mapgrid_x2;
        var07 = d3_stru_1999E8.t3.mapgrid_y1 - d3_stru_1999E8.t3.mapgrid_y2;

        if ((var05 & var_24) == 0)
        {
            if ( !var05 )
            {
                d3_stru_1999E8.clip_flags |= 1u;
                var08 = var07 * g_viewport_minimum_x - var06 * g_dword_140004;

                if ( var08 )
                {
                    var_44 = d3_stru_1999E8.t3.mapgrid_x1 * g_dword_140004 - d3_stru_1999E8.t3.mapgrid_y1 * g_viewport_minimum_x;

                    var03 = var06 * var_44;
                    if ( var03 >= 0 )
                        var03 += (var08 / 2);
                    else
                        var03 -= (var08 / 2);
                    var_2C = (var03 / var08) + d3_stru_1999E8.t3.mapgrid_x1;

                    var03 = var07 * var_44;
                    if ( var03 >= 0 )
                        var03 = var03 + (var08 / 2);
                    else
                        var03 = var03 - (var08 / 2);
                    var03 = (var03 / var08) + d3_stru_1999E8.t3.mapgrid_y1;

                    var_30 = Game_ViewportMinimumX;
                }
            }

            if ( !var_24 )
            {
                d3_stru_1999E8.clip_flags |= 2u;
                var08 = var07 * g_viewport_maximum_x - var06 * g_dword_140004;

                if ( var08 )
                {
                    var_40 = d3_stru_1999E8.t3.mapgrid_x1 * g_dword_140004 - d3_stru_1999E8.t3.mapgrid_y1 * g_viewport_maximum_x;

                    var03 = var06 * var_40;
                    if ( var03 >= 0 )
                        var03 += (var08 / 2);
                    else
                        var03 -= (var08 / 2);
                    var_2C = (var03 / var08) + d3_stru_1999E8.t3.mapgrid_x1;

                    var03 = var07 * var_40;
                    if ( var03 >= 0 )
                        var03 += (var08 / 2);
                    else
                        var03 -= (var08 / 2);
                    var03 = (var03 / var08) + d3_stru_1999E8.t3.mapgrid_y1;

                    var_30 = Game_ViewportMaximumX;
                }
            }

            if ( var03 < g_dword_140000 )
            {
                var_2C = d3_stru_1999E8.t3.mapgrid_x2 + (int32_t)((var06 * (int64_t)(g_dword_140000 - d3_stru_1999E8.t3.mapgrid_y2)) / var07);
                var03 = g_dword_140000;
                var_30 = 0;
            }
        }

        if ((var_1C & var_20) == 0)
        {
            if ( !var_1C )
            {
                d3_stru_1999E8.clip_flags |= 1u;
                var08 = var07 * g_viewport_minimum_x - var06 * g_dword_140004;

                if ( var08 )
                {
                    var_38 = d3_stru_1999E8.t3.mapgrid_x2 * g_dword_140004 - d3_stru_1999E8.t3.mapgrid_y2 * g_viewport_minimum_x;

                    var04 = var06 * var_38;
                    if ( var04 >= 0 )
                        var04 += (var08 / 2);
                    else
                        var04 -= (var08 / 2);
                    var_28 = (var04 / var08) + d3_stru_1999E8.t3.mapgrid_x2;

                    var04 = var07 * var_38;
                    if ( var04 >= 0 )
                        var04 += (var08 / 2);
                    else
                        var04 -= (var08 / 2);
                    var04 = (var04 / var08) + d3_stru_1999E8.t3.mapgrid_y2;

                    var_34 = Game_ViewportMinimumX;
                }
            }

            if ( !var_20 )
            {
                d3_stru_1999E8.clip_flags |= 2u;
                var08 = var07 * g_viewport_maximum_x - var06 * g_dword_140004;

                if ( var08 )
                {
                    var_3C = d3_stru_1999E8.t3.mapgrid_x2 * g_dword_140004 - d3_stru_1999E8.t3.mapgrid_y2 * g_viewport_maximum_x;

                    var04 = var06 * var_3C;
                    if ( var04 >= 0 )
                        var04 += (var08 / 2);
                    else
                        var04 -= (var08 / 2);
                    var_28 = (var04 / var08) + d3_stru_1999E8.t3.mapgrid_x2;

                    var04 = var07 * var_3C;
                    if ( var04 >= 0 )
                        var04 += (var08 / 2);
                    else
                        var04 -= (var08 / 2);
                    var04 = (var04 / var08) + d3_stru_1999E8.t3.mapgrid_y2;

                    var_34 = Game_ViewportMaximumX;
                }
            }

            if ( var04 < g_dword_140000 )
            {
                var_28 = var_2C + (int32_t)((var06 * (int64_t)(g_dword_140000 - var03)) / var07);
                var04 = g_dword_140000;
                var_34 = 0;
            }
        }

        //abs(d3_stru_1999E8.t3.mapgrid_x2 - d3_stru_1999E8.t3.mapgrid_x1);
        //abs(d3_stru_1999E8.t3.mapgrid_y2 - d3_stru_1999E8.t3.mapgrid_y1);
    }

    fill_stru_1999E8_1(var_2C, var03, var_28, var04);

    if ( var_30 )
        d3_stru_1999E8.viewport_x1 = var_30;
    if ( var_34 )
        d3_stru_1999E8.viewport_x2 = var_34;

    if ( d3_stru_1999E8.viewport_x2 < d3_stru_1999E8.viewport_x1 ) return 0;
    if ( d3_stru_1999E8.viewport_x2 < Game_ViewportMinimumX ) return 0;
    if ( d3_stru_1999E8.viewport_x1 > Game_ViewportMaximumX ) return 0;

    if ( ((d3_stru_1999E8.mapobject_type_flags & 7) == 3) && d3_stru_1999E8.without_transparent_pixels )
    {
        var36 = d3_stru_1999E8.t3.mapgrid_y1;
        if ( d3_stru_1999E8.t3.mapgrid_y1 <= d3_stru_1999E8.t3.mapgrid_y2 )
            var36 = d3_stru_1999E8.t3.mapgrid_y2;
    }
    else
    {
        var36 = (d3_stru_1999E8.t3.mapgrid_y2 + d3_stru_1999E8.t3.mapgrid_y1) >> 1;
    }

    d3_stru_1999E8.distance = var36;
    d3_stru_1999E8.draw_mapobject = 1;

    return 1;
}


static void d3_sub_BF05C(void)
{
    int32_t mapdata_end_y;
    int32_t mapdata_start_x;
    int32_t var_38;
//    int32_t var_34;
//    int32_t var_30;
    int32_t mapdata_current_y;
    int32_t nextline_offset;
    uint8_t *var_24;
    int32_t mapdata_end_x;
    int32_t mapdata_current_x;

    bool inserted;

    int32_t *current_mapgrid_point; // esi@2
    uint8_t *current_mapdata; // edi@4
    int32_t *nextline_mapgrid_point; // ebp@5
    uint8_t var03; // bx@6
    bool var06; // eax@6
    uint8_t *var08; // edx@16


    if ( d3_mapgrid_number_of_points <= 0 ) return;

    mapdata_end_y = g_mapdata_height - (d3_mapgrid_start_position_y + 1) - d3_mapgrid_height;
    mapdata_start_x = d3_mapgrid_start_position_x;
    mapdata_end_x = d3_mapgrid_width + d3_mapgrid_start_position_x;
    mapdata_current_y = g_mapdata_height - (d3_mapgrid_start_position_y + 1);
    var_24 = &(g_mapdata_ptr[3 * mapdata_start_x + 3 * mapdata_current_y * g_mapdata_width]);
    current_mapgrid_point = &(d3_mapgrid_points[0]);

    if ( mapdata_current_y <= mapdata_end_y ) return; // d3_mapgrid_height < 0 ???

    nextline_offset = 2 * (d3_mapgrid_width + 1);

    do
    {
        d3_stru_1999E8.mapdata_y = mapdata_current_y;
        current_mapdata = var_24;
        mapdata_current_x = mapdata_start_x;

        if ( mapdata_start_x < mapdata_end_x ) // d3_mapgrid_width > 0 ???
        {
            nextline_mapgrid_point = &current_mapgrid_point[nextline_offset];
            do
            {
                var03 = current_mapdata[0];
                var_38 = (nextline_mapgrid_point[3] >> 1) + (current_mapgrid_point[1] >> 1);
                var06 = ( (g_viewport_maximum_x * var_38) < (int32_t)(( abs((current_mapgrid_point[0] >> 1) + (nextline_mapgrid_point[2] >> 1)) - g_mapgrid_point_distance) * g_dword_140004) )?1:0;

                if ( (var03 && (int32_t)var03 < 101) || !var06 )
                {
                    d3_stru_1999E8.mapdata_x = mapdata_current_x;
                    d3_stru_1999E8.field_08 = (uint16_t)(uintptr_t)current_mapdata;

                    if ( d3_without_ceiling && current_mapdata[2] && !var06 )
                    {
                        d3_stru_1999E8.field_0C = current_mapdata[2];
                        d3_stru_1999E8.without_transparent_pixels = 0;
                        d3_stru_1999E8.t4.mapgrid_point_ptr = current_mapgrid_point;
                        d3_stru_1999E8.draw_mapobject = 1;
                        d3_stru_1999E8.t4.mapgrid_next_line_point_ptr = nextline_mapgrid_point;
                        d3_stru_1999E8.t4.mapdata_x = mapdata_current_x;
                        d3_stru_1999E8.t4.mapdata_ptr = current_mapdata;
                        d3_stru_1999E8.t4.mapdata_y = mapdata_current_y;
                        d3_stru_1999E8.mapobject_type_flags = 4;
                        d3_stru_1999E8.distance = var_38;
                        insert_into_list_sm1234();
                    }

                    if ( var03 )
                    {
                        if ( (int32_t)var03 >= 101 )
                        {
                            var03 -= 101;
                            var08 = (uint8_t *)(void *) g_dword_14A4AE[var03];
                            d3_stru_1999E8.field_0C = var03;
                            d3_stru_1999E8.mapobject_type_flags = 3;
                            d3_stru_1999E8.without_transparent_pixels = ((((uintptr_t)var08) >= 1024) && (var08[0] & 0x60))?0:1; // fix reading from NULL pointer
                            if ( ((uintptr_t)var08) >= 1024 ) // fix reading from NULL pointer
                            {
                                if ( var08[0] & 2 ) d3_stru_1999E8.mapobject_type_flags = 0x0b;
                                if ( var08[0] & 0x80 ) d3_stru_1999E8.mapobject_type_flags |= 0x40;
                                if ( var08[0] & 0x40 ) d3_stru_1999E8.mapobject_type_flags |= 0x10;
                            }
                            d3_stru_1999E8.field_40 = g_dword_14000C;

                            inserted = false;
                            if ((mapdata_current_y != 0) &&
                                (((int32_t) current_mapdata[-3 * g_mapdata_width] < 101) ||
                                 ((d3_stru_1999E8.without_transparent_pixels != 0) &&
                                  (((uintptr_t)(g_dword_14A4AE[current_mapdata[-3 * g_mapdata_width] - 101])) >= 1024) && // fix reading from NULL pointer
                                  (*((uint8_t *)(void *) g_dword_14A4AE[current_mapdata[-3 * g_mapdata_width] - 101]) & 0x60)
                                 )
                                )
                               )
                            {
                                d3_stru_1999E8.t3.mapgrid_x1 = nextline_mapgrid_point[2];
                                d3_stru_1999E8.t3.mapgrid_y1 = nextline_mapgrid_point[3];
                                d3_stru_1999E8.t3.mapgrid_x2 = nextline_mapgrid_point[0];
                                d3_stru_1999E8.t3.mapgrid_y2 = nextline_mapgrid_point[1];

                                if ( fill_stru_1999E8_2() )
                                {
                                    d3_stru_1999E8.field_0E = g_word_14A4CA[0];
                                    insert_into_list_sm1234();
                                    if (d3_stru_1999E8.without_transparent_pixels) inserted = true;
                                }
                            }

                            if (!inserted &&
                                (g_mapdata_height - 1 > mapdata_current_y) &&
                                (((int32_t) current_mapdata[3 * g_mapdata_width] < 101) ||
                                 ((d3_stru_1999E8.without_transparent_pixels != 0) &&
                                  (((uintptr_t)(g_dword_14A4AE[current_mapdata[3 * g_mapdata_width] - 101])) >= 1024) && // fix reading from NULL pointer
                                  (*((uint8_t *)(void *) g_dword_14A4AE[current_mapdata[3 * g_mapdata_width] - 101]) & 0x60)
                                 )
                                )
                               )
                            {
                                d3_stru_1999E8.t3.mapgrid_x1 = current_mapgrid_point[0];
                                d3_stru_1999E8.t3.mapgrid_y1 = current_mapgrid_point[1];
                                d3_stru_1999E8.t3.mapgrid_x2 = current_mapgrid_point[2];
                                d3_stru_1999E8.t3.mapgrid_y2 = current_mapgrid_point[3];

                                if ( fill_stru_1999E8_2() )
                                {
                                    d3_stru_1999E8.field_0E = g_word_14A4CA[2];
                                    insert_into_list_sm1234();
                                }
                            }

                            inserted = false;
                            if ((mapdata_current_x != 0) &&
                                (((int32_t) current_mapdata[-3] < 101) ||
                                 ((d3_stru_1999E8.without_transparent_pixels != 0) &&
                                  (((uintptr_t)(g_dword_14A4AE[current_mapdata[-3] - 101])) >= 1024) && // fix reading from NULL pointer
                                  (*((uint8_t *)(void *) g_dword_14A4AE[current_mapdata[-3] - 101]) & 0x60)
                                 )
                                )
                               )
                            {
                                d3_stru_1999E8.t3.mapgrid_x1 = nextline_mapgrid_point[0];
                                d3_stru_1999E8.t3.mapgrid_y1 = nextline_mapgrid_point[1];
                                d3_stru_1999E8.t3.mapgrid_x2 = current_mapgrid_point[0];
                                d3_stru_1999E8.t3.mapgrid_y2 = current_mapgrid_point[1];

                                if ( fill_stru_1999E8_2() )
                                {
                                    d3_stru_1999E8.field_0E = g_word_14A4CA[1];
                                    insert_into_list_sm1234();
                                    if (d3_stru_1999E8.without_transparent_pixels) inserted = true;
                                }
                            }

                            if (!inserted &&
                                (g_mapdata_width - 1 > mapdata_current_x) &&
                                (((int32_t) current_mapdata[3] < 101) ||
                                 ((d3_stru_1999E8.without_transparent_pixels != 0) &&
                                  (((uintptr_t)(g_dword_14A4AE[current_mapdata[3] - 101])) >= 1024) && // fix reading from NULL pointer
                                  (*((uint8_t *)(void *) g_dword_14A4AE[current_mapdata[3] - 101]) & 0x60)
                                 )
                                )
                               )
                            {
                                d3_stru_1999E8.t3.mapgrid_x1 = current_mapgrid_point[2];
                                d3_stru_1999E8.t3.mapgrid_y1 = current_mapgrid_point[3];
                                d3_stru_1999E8.t3.mapgrid_x2 = nextline_mapgrid_point[2];
                                d3_stru_1999E8.t3.mapgrid_y2 = nextline_mapgrid_point[3];

                                if ( fill_stru_1999E8_2() )
                                {
                                    d3_stru_1999E8.field_0E = g_word_14A4CA[3];
                                    insert_into_list_sm1234();
                                }
                            }
                        }
                        else
                        {
                            d3_sub_BED08(0xffff, ((uint32_t)var03) - 1, current_mapgrid_point[0], current_mapgrid_point[1], 1);
                        }
                    }
                }

                current_mapdata += 3;
                nextline_mapgrid_point += 2;
                current_mapgrid_point += 2;
                ++mapdata_current_x;
            }
            while ( mapdata_current_x < mapdata_end_x );
        }

        current_mapgrid_point += 2;
        var_24 -= 3 * g_mapdata_width;
        --mapdata_current_y;
    }
    while ( mapdata_current_y > mapdata_end_y );
}


static void initialize_mapvalues(void)
{
    d3_next_mapobject_ptr = &(d3_list_of_mapobjects[0]);
    d3_dword_143D64 = 0;
    d3_number_of_mapobjects = 0;
    d3_dword_143D68 = 0;
    d3_stru_194A80[0].viewport_x1 = -32700;
}


static void fill_mapgrid(int32_t start_position_x, int32_t start_position_y, int32_t mapgrid_width, int32_t mapgrid_height, int32_t mapgrid_number_of_points, int32_t *mapgrid_points)
{
    int32_t ydiff_fp14;
    int32_t xdiff_fp14;
    int32_t line_start_y_fp14;
    int32_t line_start_x_fp14;

    int32_t *current_mapgrid_point; // edi@2
    int32_t counter1; // esi@2
    int32_t current_x_fp14; // ecx@3
    int32_t current_y_fp14; // ebx@3
    int32_t counter2; // esi@3

    if ( mapgrid_number_of_points > 0 )
    {
        xdiff_fp14 = g_mapgrid_point_distance * g_cos_view_angle_fp14;
        ydiff_fp14 = g_mapgrid_point_distance * (-g_sin_view_angle_fp14);
        line_start_x_fp14 = (start_position_x * g_cos_view_angle_fp14) - (start_position_y * (-g_sin_view_angle_fp14));
        line_start_y_fp14 = (start_position_y * g_cos_view_angle_fp14) + (start_position_x * (-g_sin_view_angle_fp14));
        current_mapgrid_point = mapgrid_points;
        counter1 = mapgrid_height;

        do
        {
            current_x_fp14 = line_start_x_fp14;
            current_y_fp14 = line_start_y_fp14;
            counter2 = mapgrid_width;

            do
            {
                current_mapgrid_point[0] = current_x_fp14 >> 14;
                current_mapgrid_point[1] = current_y_fp14 >> 14;
                current_x_fp14 += xdiff_fp14;
                current_y_fp14 += ydiff_fp14;
                current_mapgrid_point += 2;
                --counter2;
            }
            while ( counter2 >= 0 );

            line_start_x_fp14 -= ydiff_fp14;
            line_start_y_fp14 += xdiff_fp14;
            counter1--;
        }
        while ( counter1 >= 0 );
    }
}


static void prepare_mapgrid(void)
{
    int32_t var_[6];
    int32_t var_20;
    int32_t var_1C;

#define var_38 var_[0]
#define var_34 var_[1]
#define var_30 var_[2]
#define var_2C var_[3]
#define var_28 var_[4]
#define var_24 var_[5]


    int32_t var00; // ebp@1
    int32_t iii; // edx@1
    int32_t var06; // esi@1
    int32_t var07; // edi@1
    int32_t var08; // ebx@1
    int32_t *var09; // eax@1


    d3_stru_194A80[0].viewport_x1 = -32700;
    var_38 = 0;
    var_34 = 0;
    var00 = -2147483647;
    var_30 = (((int32_t) ((-g_sin_view_angle_fp14) * g_dword_13FFDC)) >> 14) + (((int32_t) (g_dword_13FFE0 * g_cos_view_angle_fp14)) >> 14);
    var_28 = (((int32_t) ((-g_sin_view_angle_fp14) * g_dword_13FFDC)) >> 14) - (((int32_t) (g_dword_13FFE0 * g_cos_view_angle_fp14)) >> 14);

    var_24 = (((int32_t) (g_dword_13FFDC * g_cos_view_angle_fp14)) >> 14) - (((int32_t) (g_dword_13FFE0 * g_sin_view_angle_fp14)) >> 14);
    var_2C = (((int32_t) (g_dword_13FFDC * g_cos_view_angle_fp14)) >> 14) + (((int32_t) (g_dword_13FFE0 * g_sin_view_angle_fp14)) >> 14);

    iii = 0;
    var06 = 2147483647;
    var_20 = d3_dword_196CA8 >> g_dword_13FFFC;
    var07 = -2147483647;
    var08 = 2147483647;
    var_1C = d3_dword_196CA4 >> g_dword_13FFFC;
    var09 = &(var_38);

    do
    {
        var09[0] += var_20;
        var09[1] += var_1C;
        if ( var08 > var09[0] ) var08 = var09[0];
        if ( var07 < var09[0] ) var07 = var09[0];
        if ( var06 > var09[1] ) var06 = var09[1];
        if ( var00 < var09[1] ) var00 = var09[1];
        ++iii;
        var09 += 2;
    }
    while ( iii < 3 );

    var06--;
    var07++;
    var08--;
    var00++;

    if ( var08 >= 0 )
    {
        if ( var08 > g_mapdata_width ) var08 = g_mapdata_width;
    }
    else
    {
        var08 = 0;
    }

    if ( var07 >= 0 )
    {
        if ( var07 > g_mapdata_width ) var07 = g_mapdata_width;
    }
    else
    {
        var07 = 0;
    }

    if ( var06 >= 0 )
    {
        if ( var06 > g_mapdata_height ) var06 = g_mapdata_height;
    }
    else
    {
        var06 = 0;
    }

    if ( var00 >= 0 )
    {
        if ( var00 > g_mapdata_height ) var00 = g_mapdata_height;
    }
    else
    {
        var00 = 0;
    }

    var07 -= var08;
    var00 -= var06;
    d3_mapgrid_width = var07;

    if ( var07 && var00 )
    {
        var07 = (var07 + 1) * (var00 + 1);
        d3_mapgrid_start_position_y = var06;
        d3_mapgrid_start_position_x = var08;

        if ( var07 <= D3_MAXIMUM_NUMBER_OF_MAPGRID_POINTS )
        {
            d3_skip_draw_list_sm1234 = 0;
        }
        else
        {
            var07 = D3_MAXIMUM_NUMBER_OF_MAPGRID_POINTS;
            d3_skip_draw_list_sm1234 = 1;
        }

        d3_mapgrid_number_of_points = var07;
        d3_mapgrid_height = var00;

        fill_mapgrid(
          (var08 << g_dword_13FFFC) - d3_dword_196CA8,
          (var06 << g_dword_13FFFC) - d3_dword_196CA4,
          d3_mapgrid_width,
          var00,
          var07,
          &(d3_mapgrid_points[0])
        );

        var00 = d3_mapgrid_height;
        var07 = d3_mapgrid_number_of_points;
    }
    else
    {
        var07 = 0;
    }

    d3_mapgrid_height = var00;
    d3_mapgrid_number_of_points = var07;

#undef var_38
#undef var_34
#undef var_30
#undef var_2C
#undef var_28
#undef var_24
}


#ifdef __cplusplus
extern "C"
#endif
void draw_3dscene(void)
{
#if defined(ALLOW_OPENGL) || SDL_VERSION_ATLEAST(2,0,0)
    if (Game_AdvancedScaling)
    {
        // using factor greater than 6 introduces visible artifacts (oveflow or loss of precision in fixed point calculations)
        Engine_RenderWidth = Scaler_ScaleFactor * Render_Width;
        Engine_RenderHeight = Scaler_ScaleFactor * Render_Height;
        Engine_MaximumViewportWidth = Scaler_ScaleFactor * Render_Width;
        Engine_MaximumViewportHeight = (Scaler_ScaleFactor * Render_Height * 5) / 6; // = ... * 200 / 240
        Game_ScreenWidth = Scaler_ScaleFactor * Render_Width;
    }
    else
#endif
    {
        Engine_RenderWidth = Render_Width;
        Engine_RenderHeight = Render_Height;
        Engine_MaximumViewportWidth = D3_MAXIMUM_VIEWPORT_WIDTH;
        Engine_MaximumViewportHeight = D3_MAXIMUM_VIEWPORT_HEIGHT;
        Game_ScreenWidth = 800;
    }

    if (Game_stru_1414AC == NULL)
    {
        Game_stru_1414AC = (struct struc_1 *) malloc((Engine_MaximumViewportWidth + 3) * sizeof(struct struc_1));
        if (Game_stru_1414AC == NULL) return;
    }

    if (Game_unk_144F50 == NULL)
    {
        Game_unk_144F50 = malloc((2 * (Engine_MaximumViewportHeight + 1)) * sizeof(struct struc_7));
        if (Game_unk_144F50 == NULL) return;
    }


    Game_OverlayDraw.Enhanced3DEngineUsed = 1;
    Game_OverlayDraw.ViewportX = g_viewport_offsetx;
    Game_OverlayDraw.ViewportY = g_viewport_offsety;
    Game_OverlayDraw.ViewportWidth = g_viewport_width;
    Game_OverlayDraw.ViewportHeight = g_viewport_height;
    Game_OverlayDraw.OverlayX = convert_horizontal(g_viewport_offsetx);
    Game_OverlayDraw.OverlayY = convert_vertical(g_viewport_offsety);
    Game_OverlayDraw.OverlayWidth = (Engine_RenderWidth - Game_OverlayDraw.OverlayX) - convert_horizontal((360 - g_viewport_offsetx) - g_viewport_width);
    Game_OverlayDraw.OverlayHeight = (Engine_RenderHeight - Game_OverlayDraw.OverlayY) - convert_vertical((240 - g_viewport_offsety) - g_viewport_height);
    Game_ViewportWidth = Game_OverlayDraw.OverlayWidth;
    Game_ViewportHeight = Game_OverlayDraw.OverlayHeight;
    Game_ViewportPtr = &(Game_OverlayDraw.ScreenViewpartOverlay[Game_OverlayDraw.OverlayY * Game_ScreenWidth + Game_OverlayDraw.OverlayX]);
    // clean original viewport
    /*{
        uint8_t *ptr;
        uint32_t counter;

        ptr = g_viewport_ptr;
        for (counter = g_viewport_height; counter != 0; counter--)
        {
            memset(ptr, 0, g_viewport_width);
            ptr += g_screen_width;
        }

        ptr = g_viewport_ptr + (g_viewport_height - 2) * g_screen_width + 1;
        for (counter = 0; counter < 8; counter++)
        {
            *ptr = counter + 19;
            ptr++;
        }

        ptr = g_viewport_ptr + (g_viewport_height - 2) * g_screen_width + g_viewport_width - 10;
        for (counter = 0; counter < 8; counter++)
        {
            *ptr = counter + 31;
            ptr++;
        }
    }*/

    Game_ViewportZeroYRow = convert_vertical(g_viewport_zero_y_row);
    Game_ViewportZeroXColumn = convert_horizontal(g_viewport_zero_x_column);
    Game_ViewportMinimumX = - Game_ViewportZeroXColumn;
    Game_ViewportMaximumX = Game_ViewportMinimumX + (int32_t)Game_ViewportWidth - 1;
    Game_ViewportMinimumY = - Game_ViewportZeroYRow;
    Game_ViewportMaximumY = Game_ViewportMinimumY + (int32_t)Game_ViewportHeight - 1;

    Game_ResizeWidthMult = Engine_RenderWidth;
    Game_ResizeWidthDiv = 360; // = 2 * 2 * 2 * 3 * 3 * 5
    Game_ResizeHeightMult = Engine_RenderHeight;
    Game_ResizeHeightDiv = 240; // = 2 * 2 * 2 * 2 * 3 * 5

    // minimize mult and div values
    {
        if ((Game_ResizeWidthMult % 2) == 0)
        {
            Game_ResizeWidthMult /= 2;
            Game_ResizeWidthDiv /= 2;
            if ((Game_ResizeWidthMult % 2) == 0)
            {
                Game_ResizeWidthMult /= 2;
                Game_ResizeWidthDiv /= 2;
                if ((Game_ResizeWidthMult % 2) == 0)
                {
                    Game_ResizeWidthMult /= 2;
                    Game_ResizeWidthDiv /= 2;
                }
            }
        }
        if ((Game_ResizeWidthMult % 3) == 0)
        {
            Game_ResizeWidthMult /= 3;
            Game_ResizeWidthDiv /= 3;
            if ((Game_ResizeWidthMult % 3) == 0)
            {
                Game_ResizeWidthMult /= 3;
                Game_ResizeWidthDiv /= 3;
            }
        }
        if ((Game_ResizeWidthMult % 5) == 0)
        {
            Game_ResizeWidthMult /= 5;
            Game_ResizeWidthDiv /= 5;
        }

        if ((Game_ResizeHeightMult % 2) == 0)
        {
            Game_ResizeHeightMult /= 2;
            Game_ResizeHeightDiv /= 2;
            if ((Game_ResizeHeightMult % 2) == 0)
            {
                Game_ResizeHeightMult /= 2;
                Game_ResizeHeightDiv /= 2;
                if ((Game_ResizeHeightMult % 2) == 0)
                {
                    Game_ResizeHeightMult /= 2;
                    Game_ResizeHeightDiv /= 2;
                    if ((Game_ResizeHeightMult % 2) == 0)
                    {
                        Game_ResizeHeightMult /= 2;
                        Game_ResizeHeightDiv /= 2;
                    }
                }
            }
        }
        if ((Game_ResizeHeightMult % 3) == 0)
        {
            Game_ResizeHeightMult /= 3;
            Game_ResizeHeightDiv /= 3;
        }
        if ((Game_ResizeHeightMult % 5) == 0)
        {
            Game_ResizeHeightMult /= 5;
            Game_ResizeHeightDiv /= 5;
        }
    }

    Game_mul_dword_140004_ResizeWidthMult = g_dword_140004 * Game_ResizeWidthMult;
    Game_mul_dword_140008_ResizeHeightMult = g_dword_140008 * Game_ResizeHeightMult;

    d3_param_word_196D0C = g_word_14A4C8;
    d3_without_ceiling = g_dword_14A47C & 1;
    d3_byte_13FFAB = 1;
    d3_param_palette_depth_index = &(g_byte_14A4DE[0]);
    d3_dword_1A5DF8 = 2 * (1 << g_dword_13FFC4);
    d3_dword_1A5E0C = g_mapgrid_point_distance - 1;
    d3_mapgrid_point_distance_3_4 = g_mapgrid_point_distance - (g_mapgrid_point_distance >> 2);

    d3_dword_196CA8 = ((uint32_t)(g_word_14A480[0] | (g_word_14A480[1] << 16))) >> (16 - g_byte_13FFAE);
    d3_dword_196CA4 = ((uint32_t)(g_word_14A486[0] | (g_word_14A486[1] << 16))) >> (16 - g_byte_13FFAE);
    d3_dword_196CA8 += ((uint32_t)(g_word_14A480[1] | (g_word_14A480[2] << 16))) >> (16 - g_byte_13FFAE);
    ++d3_scene_counter;
    d3_dword_196CA4 += ((uint32_t)(g_word_14A486[1] | (g_word_14A486[2] << 16))) >> (16 - g_byte_13FFAE);
    g_dword_196CEC = ((uint32_t)g_word_14A490) << g_byte_13FFAE;
    d3_word_196D0A = g_word_14A480[0] << g_byte_13FFAE;
    d3_word_196D08 = g_word_14A486[0] << g_byte_13FFAE;

    if ( g_select_mapobject != NULL )
    {
        g_select_mapobject->mapobject_type = 0;
        g_select_mapobject->mapdata_x_plus1 = 0;
        g_select_mapobject->mapdata_y_plus1 = 0;
        g_select_mapobject->field_7 = 0;
    }

    d3_time_counter_1 -= Game_VSyncTick;
    prepare_mapgrid();
    initialize_mapvalues();
    d3_sub_BF05C();
    d3_sub_BF4D4();
    d3_sub_BF9BC();
    d3_time_counter_1 += Game_VSyncTick;

    d3_time_counter_2 -= Game_VSyncTick;
    draw_background(Game_ScreenWidth * Game_ViewportZeroYRow);
    draw_floor_and_ceiling();
    d3_time_counter_2 += Game_VSyncTick;

    d3_time_counter_3 -= Game_VSyncTick;
    draw_list_sm1234();
    d3_time_counter_3 += Game_VSyncTick;

    {
        uint32_t src_ydelta, src_ypos, src_xdelta, src_xpos, height, width;
        uint8_t *ptr, *ptr2;

        if (Game_OverlayDraw.OverlayY > 0)
        {
            if (Game_OverlayDraw.OverlayX > 0)
            {
                Game_ViewportPtr[-(int32_t)(Game_ScreenWidth + 1)] = Game_ViewportPtr[0];
            }
            memcpy(&(Game_ViewportPtr[-(int32_t)Game_ScreenWidth]), Game_ViewportPtr, Game_ViewportWidth);
            if (Game_OverlayDraw.OverlayX + Game_ViewportWidth < Engine_RenderWidth)
            {
                Game_ViewportPtr[-(int32_t)(Game_ScreenWidth - Game_ViewportWidth)] = Game_ViewportPtr[Game_ViewportWidth - 1];
            }
        }

        if (Game_OverlayDraw.OverlayX > 0)
        {
            for (height = 0; height < Game_ViewportHeight; height++)
            {
                Game_ViewportPtr[height * Game_ScreenWidth - 1] = Game_ViewportPtr[height * Game_ScreenWidth];
            }
        }

        if (Game_OverlayDraw.OverlayX + Game_ViewportWidth < Engine_RenderWidth)
        {
            for (height = 0; height < Game_ViewportHeight; height++)
            {
                Game_ViewportPtr[height * Game_ScreenWidth + Game_ViewportWidth] = Game_ViewportPtr[height * Game_ScreenWidth + Game_ViewportWidth - 1];
            }
        }

        if (Game_OverlayDraw.OverlayY + Game_ViewportHeight < (Engine_RenderHeight - convert_vertical(240-192)))
        {
            if (Game_OverlayDraw.OverlayX > 0)
            {
                Game_ViewportPtr[Game_ViewportHeight * Game_ScreenWidth - 1] = Game_ViewportPtr[(Game_ViewportHeight - 1) * Game_ScreenWidth];
            }
            memcpy(&(Game_ViewportPtr[Game_ViewportHeight * Game_ScreenWidth]), &(Game_ViewportPtr[(Game_ViewportHeight - 1) * Game_ScreenWidth]), Game_ViewportWidth);
            if (Game_OverlayDraw.OverlayX + Game_ViewportWidth < Engine_RenderWidth)
            {
                Game_ViewportPtr[Game_ViewportHeight * Game_ScreenWidth + Game_ViewportWidth] = Game_ViewportPtr[(Game_ViewportHeight - 1) * Game_ScreenWidth + Game_ViewportWidth - 1];
            }
        }


        // fill original viewport
        src_xdelta = (Game_ViewportWidth << 16) / g_viewport_width;

        src_ydelta = (Game_ViewportHeight << 16) / g_viewport_height;
        src_ypos = 0;


        ptr = g_viewport_ptr;
        for (height = 0; (int32_t)height < g_viewport_height; height++)
        {
            ptr2 = Game_ViewportPtr + (src_ypos >> 16) * Game_ScreenWidth;
            src_xpos = 0;

            for (width = 0; (int32_t)width < g_viewport_width; width++)
            {
                ptr[width] = *ptr2;

                src_xpos += src_xdelta;
                ptr2 += (src_xpos >> 16);
                src_xpos &= 0xffff;
            }

            ptr += g_screen_width;
            src_ypos += src_ydelta;
        }

        // mark use of enhanced 3d engine
        ptr = g_viewport_ptr + (g_viewport_height - 2) * g_screen_width + 1;
        for (width = 0; width < 8; width++)
        {
            *ptr = width + 19;
            ptr++;
        }

        ptr = g_viewport_ptr + (g_viewport_height - 2) * g_screen_width + g_viewport_width - 10;
        for (width = 0; width < 8; width++)
        {
            *ptr = width + 31;
            ptr++;
        }


        // copy original screen
        ptr = g_viewport_ptr - g_viewport_offsetx;
        height = g_viewport_height;
        ptr2 = Game_OverlayDraw.ScreenViewpartOriginal;
        if (g_viewport_offsety > 0)
        {
            ptr -= 360;
            height++;
            ptr2 += 360*(g_viewport_offsety - 1);
        }
        if (height < 192)
        {
            height++;
        }
        memcpy(ptr2, ptr, 360*height);
    }
}

