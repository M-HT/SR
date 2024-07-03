/**
 *
 *  Copyright (C) 2019-2024 Roman Pauer
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

#if ( \
    defined(__aarch64__) || \
    defined(_M_ARM64) || \
    defined(_M_ARM64EC) \
)
    #define ARMV8 1
#else
    #undef ARMV8
#endif

#if (!defined(ARMV8)) && ( \
    (defined(__ARM_ARCH) && (__ARM_ARCH >= 6)) || \
    (defined(_M_ARM) && (_M_ARM >= 6)) || \
    (defined(__TARGET_ARCH_ARM) && (__TARGET_ARCH_ARM >= 6)) || \
    (defined(__TARGET_ARCH_THUMB) && (__TARGET_ARCH_THUMB >= 3)) \
)
    #define ARMV6 1
#else
    #undef ARMV6
#endif

#if defined(ARMV8)
#include <arm_neon.h>
#elif defined(ARMV6) && defined(__ARM_ACLE)
#include <arm_acle.h>
#endif


#if defined(ARMV6) && !defined(SDL_Swap16)
#   if defined(__ARM_ACLE)
#       define SDL_Swap16(x) __revsh(x)
#   else
#       define SDL_Swap16(x) arm_swap16(x)
static inline int32_t arm_swap16(int16_t x)
{
    int32_t res;
    asm ( "revsh %[result], %[value]" : [result] "=r" (res) : [value] "r" (x) :  );
    return res;
}
#   endif
#endif

#if defined(ARMV8)
#define accum_2_mono(dstvalue) \
for (; num_samples != 0; num_samples--) \
{ \
    int32_t val1, val2; \
    int16_t value; \
    val1 = src[0]; \
    val2 = src[1]; \
    src += 2; \
    value = vqmovns_s32((val1 + val2) >> 1); \
    *dst = dstvalue; \
    dst++; \
}
#elif defined(ARMV6)
#   if defined(__ARM_ACLE) && __ARM_FEATURE_SAT
#       define ARM_SSAT(dst,src) dst = __ssat(src, 16);
#   else
#       define ARM_SSAT(dst,src) asm ( "ssat %[result], #16, %[value]" : [result] "=r" (dst) : [value] "r" (src) : "cc" );
#   endif
#define accum_2_mono(dstvalue) \
for (; num_samples != 0; num_samples--) \
{ \
    int32_t val1, val2, value; \
    val1 = src[0]; \
    val2 = src[1]; \
    src += 2; \
    ARM_SSAT(value,(val1 + val2) >> 1) \
    *dst = dstvalue; \
    dst++; \
}
#else
#define accum_2_mono(dstvalue) \
for (; num_samples != 0; num_samples--) \
{ \
    int value; \
    value = (src[0] + src[1]) >> 1; \
    src += 2; \
    if (value >= 0x8000) value = 0x7fff; \
    else if (value < -0x8000) value = -0x8000; \
    *dst = dstvalue; \
    dst++; \
}
#endif

static void accum_2_s8_mono(int32_t *src, int8_t *dst, int num_samples)
{
    accum_2_mono(value >> 8)
}

static void accum_2_u8_mono(int32_t *src, uint8_t *dst, int num_samples)
{
    accum_2_mono((value + 0x8000) >> 8)
}

static void accum_2_s16_mono(int32_t *src, int16_t *dst, int num_samples)
{
    accum_2_mono(value)
}

static void accum_2_u16_mono(int32_t *src, uint16_t *dst, int num_samples)
{
    accum_2_mono(value + 0x8000)
}

static void accum_2_s16swap_mono(int32_t *src, int16_t *dst, int num_samples)
{
    accum_2_mono(SDL_Swap16(value))
}

static void accum_2_u16swap_mono(int32_t *src, uint16_t *dst, int num_samples)
{
    accum_2_mono(SDL_Swap16(value + 0x8000))
}


#if defined(ARMV8)
for (; num_samples != 0; num_samples--) \
{ \
    int32x4_t val1; \
    int16x4_t val2; \
    int16_t value, value2;
    val1 = vreinterpret_s32_u64(vld1q_dup_u64((uint64_t *)src)); \
    src += 2; \
    val2 = vqmovn_s32(val1); \
    value = vget_lane_s16(val2, 0); \
    value2 = vget_lane_s16(val2, 1); \
    dst[0] = dstvalue; \
    value = value2; \
    dst[1] = dstvalue; \
    dst += 2; \
}
#elif defined(ARMV6)
#define accum_2_stereo(dstvalue) \
for (; num_samples != 0; num_samples--) \
{ \
    int32_t val1, val2, value, value2; \
    val1 = src[0]; \
    val2 = src[1]; \
    src += 2; \
    ARM_SSAT(value,val1) \
    ARM_SSAT(value2,val2) \
    dst[0] = dstvalue; \
    value = value2; \
    dst[1] = dstvalue; \
    dst += 2; \
}
#else
#define accum_2_stereo(dstvalue) \
for (num_samples *= 2; num_samples != 0; num_samples--) \
{ \
    int value; \
    value = *src; \
    src++; \
    if (value >= 0x8000) value = 0x7fff; \
    else if (value < -0x8000) value = -0x8000; \
    *dst = dstvalue; \
    dst++; \
}
#endif

static void accum_2_s8_stereo(int32_t *src, int8_t *dst, int num_samples)
{
    accum_2_stereo(value >> 8)
}

static void accum_2_u8_stereo(int32_t *src, uint8_t *dst, int num_samples)
{
    accum_2_stereo((value + 0x8000) >> 8)
}

static void accum_2_s16_stereo(int32_t *src, int16_t *dst, int num_samples)
{
    accum_2_stereo(value)
}

static void accum_2_u16_stereo(int32_t *src, uint16_t *dst, int num_samples)
{
    accum_2_stereo(value + 0x8000)
}

static void accum_2_s16swap_stereo(int32_t *src, int16_t *dst, int num_samples)
{
    accum_2_stereo(SDL_Swap16(value))
}

static void accum_2_u16swap_stereo(int32_t *src, uint16_t *dst, int num_samples)
{
    accum_2_stereo(SDL_Swap16(value + 0x8000))
}


#define conv_mono(srcvalue) \
for (; num_samples != 0; num_samples--) \
{ \
    *dst = srcvalue; \
    src++; \
    dst++; \
}

static void conv_s8_mono(int8_t *src, int16_t *dst, int num_samples)
{
    conv_mono((*src) << 8)
}

static void conv_u8_mono(uint8_t *src, int16_t *dst, int num_samples)
{
    conv_mono(((*src) << 8) - 0x8000)
}

static void conv_s16_mono(int16_t *src, int16_t *dst, int num_samples)
{
    conv_mono(*src)
}

static void conv_u16_mono(uint16_t *src, int16_t *dst, int num_samples)
{
    conv_mono((*src) - 0x8000)
}

static void conv_s16swap_mono(int16_t *src, int16_t *dst, int num_samples)
{
    conv_mono(SDL_Swap16(*src))
}

static void conv_u16swap_mono(uint16_t *src, int16_t *dst, int num_samples)
{
    conv_mono(SDL_Swap16(*src) - 0x8000)
}


#define conv_stereo(srcvalue) \
for (; num_samples != 0; num_samples--) \
{ \
    dst[0] = srcvalue; \
    src++; \
    dst[1] = srcvalue; \
    src++; \
    dst += 2; \
}

static void conv_s8_stereo(int8_t *src, int16_t *dst, int num_samples)
{
    conv_stereo((*src) << 8)
}

static void conv_u8_stereo(uint8_t *src, int16_t *dst, int num_samples)
{
    conv_stereo(((*src) << 8) - 0x8000)
}

static void conv_s16_stereo(int16_t *src, int16_t *dst, int num_samples)
{
    conv_stereo(*src)
}

static void conv_u16_stereo(uint16_t *src, int16_t *dst, int num_samples)
{
    conv_stereo((*src) - 0x8000)
}

static void conv_s16swap_stereo(int16_t *src, int16_t *dst, int num_samples)
{
    conv_stereo(SDL_Swap16(*src))
}

static void conv_u16swap_stereo(uint16_t *src, int16_t *dst, int num_samples)
{
    conv_stereo(SDL_Swap16(*src) - 0x8000)
}


#define downrate_mono(srcvalue) \
for (; num_samples != 0; num_samples--) \
{ \
    *dst = srcvalue; \
    src += (1 << freq_diff_shift); \
    dst++; \
}

static void downrate_s8_mono(int8_t *src, int16_t *dst, int num_samples, int freq_diff_shift)
{
    downrate_mono((*src) << 8)
}

static void downrate_u8_mono(uint8_t *src, int16_t *dst, int num_samples, int freq_diff_shift)
{
    downrate_mono(((*src) << 8) - 0x8000)
}

static void downrate_s16_mono(int16_t *src, int16_t *dst, int num_samples, int freq_diff_shift)
{
    downrate_mono(*src)
}

static void downrate_u16_mono(uint16_t *src, int16_t *dst, int num_samples, int freq_diff_shift)
{
    downrate_mono((*src) - 0x8000)
}

static void downrate_s16swap_mono(int16_t *src, int16_t *dst, int num_samples, int freq_diff_shift)
{
    downrate_mono(SDL_Swap16(*src))
}

static void downrate_u16swap_mono(uint16_t *src, int16_t *dst, int num_samples, int freq_diff_shift)
{
    downrate_mono(SDL_Swap16(*src) - 0x8000)
}


#define downrate_stereo(srcvalue) \
for (; num_samples != 0; num_samples--) \
{ \
    dst[0] = srcvalue; \
    src++; \
    dst[1] = srcvalue; \
    src--; \
    src += (2 << freq_diff_shift); \
    dst += 2; \
}

static void downrate_s8_stereo(int8_t *src, int16_t *dst, int num_samples, int freq_diff_shift)
{
    downrate_stereo((*src) << 8)
}

static void downrate_u8_stereo(uint8_t *src, int16_t *dst, int num_samples, int freq_diff_shift)
{
    downrate_stereo(((*src) << 8) - 0x8000)
}

static void downrate_s16_stereo(int16_t *src, int16_t *dst, int num_samples, int freq_diff_shift)
{
    downrate_stereo(*src)
}

static void downrate_u16_stereo(uint16_t *src, int16_t *dst, int num_samples, int freq_diff_shift)
{
    downrate_stereo((*src) - 0x8000)
}

static void downrate_s16swap_stereo(int16_t *src, int16_t *dst, int num_samples, int freq_diff_shift)
{
    downrate_stereo(SDL_Swap16(*src))
}

static void downrate_u16swap_stereo(uint16_t *src, int16_t *dst, int num_samples, int freq_diff_shift)
{
    downrate_stereo(SDL_Swap16(*src) - 0x8000)
}


#define uprate_mono(srcvalue) \
int position, diff; \
int16_t prev_value, next_value; \
 \
diff = 0x100 >> freq_diff_shift; \
position = diff; \
 \
prev_value = *last_sample; \
next_value = srcvalue; \
for (; num_samples != 0; num_samples--) \
{ \
    *dst = ((position * next_value) + ((0x100 - position) * prev_value)) >> 8; \
    dst++; \
 \
    position += diff; \
 \
    if (position >= 0x100) \
    { \
        src += position >> 8; \
        position &= 0xff; \
        prev_value = next_value; \
        next_value = srcvalue; \
    } \
} \
 \
*last_sample = prev_value;

static void uprate_s8_mono(int8_t *src, int16_t *dst, int num_samples, int freq_diff_shift, int16_t *last_sample)
{
    uprate_mono((*src) << 8)
}

static void uprate_u8_mono(uint8_t *src, int16_t *dst, int num_samples, int freq_diff_shift, int16_t *last_sample)
{
    uprate_mono(((*src) << 8) - 0x8000)
}

static void uprate_s16_mono(int16_t *src, int16_t *dst, int num_samples, int freq_diff_shift, int16_t *last_sample)
{
    uprate_mono(*src)
}

static void uprate_u16_mono(uint16_t *src, int16_t *dst, int num_samples, int freq_diff_shift, int16_t *last_sample)
{
    uprate_mono((*src) - 0x8000)
}

static void uprate_s16swap_mono(int16_t *src, int16_t *dst, int num_samples, int freq_diff_shift, int16_t *last_sample)
{
    uprate_mono(SDL_Swap16(*src))
}

static void uprate_u16swap_mono(uint16_t *src, int16_t *dst, int num_samples, int freq_diff_shift, int16_t *last_sample)
{
    uprate_mono(SDL_Swap16(*src) - 0x8000)
}


#define uprate_stereo(srcvalue) \
int position, diff; \
int16_t prev_value[2], next_value[2]; \
 \
diff = 0x100 >> freq_diff_shift; \
position = diff; \
 \
prev_value[0] = last_sample[0]; \
prev_value[1] = last_sample[1]; \
next_value[0] = srcvalue; \
src++; \
next_value[1] = srcvalue; \
src--; \
for (; num_samples != 0; num_samples--) \
{ \
    dst[0] = ((position * next_value[0]) + ((0x100 - position) * prev_value[0])) >> 8; \
    dst[1] = ((position * next_value[1]) + ((0x100 - position) * prev_value[1])) >> 8; \
    dst += 2; \
 \
    position += diff; \
 \
    if (position >= 0x100) \
    { \
        src += 2 * (position >> 8); \
        position &= 0xff; \
        prev_value[0] = next_value[0]; \
        prev_value[1] = next_value[1]; \
        next_value[0] = srcvalue; \
        src++; \
        next_value[1] = srcvalue; \
        src--; \
    } \
} \
 \
last_sample[0] = prev_value[0]; \
last_sample[1] = prev_value[1];

static void uprate_s8_stereo(int8_t *src, int16_t *dst, int num_samples, int freq_diff_shift, int16_t *last_sample)
{
    uprate_stereo((*src) << 8)
}

static void uprate_u8_stereo(uint8_t *src, int16_t *dst, int num_samples, int freq_diff_shift, int16_t *last_sample)
{
    uprate_stereo(((*src) << 8) - 0x8000)
}

static void uprate_s16_stereo(int16_t *src, int16_t *dst, int num_samples, int freq_diff_shift, int16_t *last_sample)
{
    uprate_stereo(*src)
}

static void uprate_u16_stereo(uint16_t *src, int16_t *dst, int num_samples, int freq_diff_shift, int16_t *last_sample)
{
    uprate_stereo((*src) - 0x8000)
}

static void uprate_s16swap_stereo(int16_t *src, int16_t *dst, int num_samples, int freq_diff_shift, int16_t *last_sample)
{
    uprate_stereo(SDL_Swap16(*src))
}

static void uprate_u16swap_stereo(uint16_t *src, int16_t *dst, int num_samples, int freq_diff_shift, int16_t *last_sample)
{
    uprate_stereo(SDL_Swap16(*src) - 0x8000)
}

