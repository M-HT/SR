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

#if !defined(_HQ2X_H_INCLUDED_)
#define _HQ2X_H_INCLUDED_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void hq2x_32(const uint32_t *src, uint32_t *dst, unsigned int width, unsigned int srcstride, unsigned int dststride, unsigned int height, unsigned int y_first, unsigned int y_last);
extern void hq3x_32(const uint32_t *src, uint32_t *dst, unsigned int width, unsigned int srcstride, unsigned int dststride, unsigned int height, unsigned int y_first, unsigned int y_last);
extern void hq4x_32(const uint32_t *src, uint32_t *dst, unsigned int width, unsigned int srcstride, unsigned int dststride, unsigned int height, unsigned int y_first, unsigned int y_last);

#ifdef __cplusplus
}
#endif


#endif /* _HQ2X_H_INCLUDED_ */

