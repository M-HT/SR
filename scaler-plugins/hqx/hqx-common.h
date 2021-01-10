/*
 * Copyright (C) 2021  Roman Pauer
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __HQX_COMMON_H_
#define __HQX_COMMON_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void convert_yuv_32(const uint32_t *src, uint32_t *dst, unsigned int width); // dst is aligned using ALIGNPTR
void calculate_pattern(const uint32_t *yuvsrc1, const uint32_t *yuvsrc2, const uint32_t *yuvsrc3, uint8_t *dst, unsigned int width);

#ifdef __cplusplus
}
#endif

#endif

