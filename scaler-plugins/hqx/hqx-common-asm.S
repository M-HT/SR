//
// Copyright (C) 2021  Roman Pauer
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//

#include "hqx-platform.h"

#if defined(ARMV7)

.arm

.global calculate_pattern

.align 4
.type calculate_pattern, %function
calculate_pattern:

// r0     = const uint32_t *yuvsrc1  (aligned to 8 bytes (64 bits))
// r1     = const uint32_t *yuvsrc2  (aligned to 8 bytes (64 bits))
// r2     = const uint32_t *yuvsrc3  (aligned to 8 bytes (64 bits))
// r3     = uint8_t *dst             (aligned to 8 bytes (64 bits))
// [sp]   = unsigned int width
// lr     = return address

        ldr ip, [sp]                // ip = width
        push {r4-r8,r10,r11,lr}     // save registers

// ip = width
// lr = 0x300706
// r10 = 0

// q0 = 0x00300706
// q10 = 0x0c080400
// q14,q15 = temporary
// q11 = yuvsrc1[ 4..7]
// q12 = yuvsrc2[ 4..7]
// q13 = yuvsrc3[ 4..7]
// q1 = yuvsrc1[-1..2]
// q2 = yuvsrc1[ 0..3] = b
// q3 = yuvsrc1[ 1..4]
// q4 = yuvsrc2[-1..2]
// q5 = yuvsrc2[ 0..3] = e
// q6 = yuvsrc2[ 1..4]
// q7 = yuvsrc3[-1..2]
// q8 = yuvsrc3[ 0..3] = h
// q9 = yuvsrc3[ 1..4]

        mov r10, #0                 // r10 = 0
        ldr r4, [r1]                // r4 = yuvsrc2[0] = e
        movw lr, #0x0706
        ldr r5, [r0]                // r5 = yuvsrc1[0] = b
        movt lr, #0x30              // lr = 0x300706

        usub8 r7, r4, r5            // r7 = e - b
        usub8 r11, r5, r4           // r11 = b - e
        sel r7, r11, r7             // r7 = (r11 >= 0)r11:r7     // r7 = abs(b - e)
        usub8 r6, lr, r7            // r6 = 0x300706 - abs(b - e)
        sel r7, r10, lr             // r7 = (r6 >= 0)?r10:lr     // r7 = (0x300706 >= abs(b - e))?0:0x300706

        ldr r6, [r2]                // r6 = yuvsrc3[0] = h
        mov r11, #0                 // r11 = 0

        cmp r7, #0
        orrne r11, r11, #7          // r11 |= (r7 != 0)?((1 << 0) | (1 << 1) | (1 << 2)):0

        usub8 r8, r4, r6            // r8 = e - h
        usub8 r7, r6, r4            // r6 = h - e
        sel r8, r7, r8              // r8 = (r7 >= 0)r7:r8       // r8 = abs(h - e)
        usub8 r6, lr, r8            // r6 = 0x300706 - abs(h - e)
        sel r8, r10, lr             // r8 = (r6 >= 0)?r10:lr     // r8 = (0x300706 >= abs(h - e))?0:0x300706

        cmp r8, #0
        orrne r11, r11, #0xe0       // r11 |= (r7 != 0)?((1 << 5) | (1 << 6) | (1 << 7)):0

        cmp ip, #4                  // width >= 4

        strb r11, [r3, #-1]         // dst[-1] = pattern

        blo 205f

        sub r4, sp, #32             // save vector registers
        sub sp, sp, #64
        vst1.32 {d12, d13, d14, d15}, [r4]
        vst1.32 {d8, d9, d10, d11}, [sp]

        movw r5, #0x0400
        movt r5, #0x0c08            // r5 = 0x0c080400

        vdup.32 q0, lr              // q0 = 0x300706
        vdup.32 q10, r5             // q10 = 0x0c080400


        vld1.32 {q11}, [r0:64]!     // q11 = yuvsrc1[0..3]
        vld1.32 {q12}, [r1:64]!     // q12 = yuvsrc2[0..3]
        vld1.32 {q13}, [r2:64]!     // q13 = yuvsrc3[0..3]

        cmp ip, #8

        vdup.32 q2, d22[0]          // b = yuvsrc1[?..0] = yuvsrc1[?..-1]
        vdup.32 q5, d24[0]          // e = yuvsrc2[?..0] = yuvsrc2[?..-1]
        vdup.32 q8, d26[0]          // h = yuvsrc3[?..0] = yuvsrc3[?..-1]

        blo 203f

    201:
        vext.32 q1, q2, q11, #3     // oldb = yuvsrc1[-1..2]
        vext.32 q4, q5, q12, #3     // olde = yuvsrc2[-1..2]
        vmov q2, q11                // b = yuvsrc1[0..3]
        vext.32 q7, q8, q13, #3     // oldh = yuvsrc3[-1..2]

        vld1.32 {q11}, [r0:64]!     // q11 = yuvsrc1[4..7]

        vmov q5, q12                // e = yuvsrc2[0..3]
        vmov q8, q13                // h = yuvsrc3[0..3]

        vld1.32 {q12}, [r1:64]!     // q12 = yuvsrc2[4..7]
        vld1.32 {q13}, [r2:64]!     // q13 = yuvsrc3[4..7]

    202:
        vext.32 q3, q2, q11, #1     // newb = (b >> 32) | (yuvsrc1[4] << 96) = yuvsrc1[1..4]
        vext.32 q6, q5, q12, #1     // newe = (e >> 32) | (yuvsrc2[4] << 96) = yuvsrc2[1..4]
        vext.32 q9, q8, q13, #1     // newh = (b >> 32) | (yuvsrc3[4] << 96) = yuvsrc3[1..4]

        // mam naplnene v21-v29

        vabd.U8 q14, q5, q1         // q14 = abs(e - oldb)
        vabd.U8 q15, q5, q2         // q15 = abs(e -    b)
        vmax.U8 q14, q14, q0        // q14 = max(abs(e - oldb), 0x300706)
        vmax.U8 q15, q15, q0        // q15 = max(abs(e -    b), 0x300706)
        vsub.I8 q14, q14, q0        // q14 = max(abs(e - oldb), 0x300706) - 0x300706
        vsub.I8 q15, q15, q0        // q15 = max(abs(e -    b), 0x300706) - 0x300706
        vcgt.S32 q14, q14, #0       // q14 = (q14 > 0)?-1:0
        vcgt.S32 q15, q15, #0       // q15 = (q15 > 0)?-1:0
        vbic.I32 q14, #(0xff ^ (1 << 0))    // q14 &= 1 << 0
        vbic.I32 q15, #(0xff ^ (1 << 1))    // q15 &= 1 << 1

        vabd.U8 q1, q5, q3          // q1 = abs(e - newb)
        vabd.U8 q4, q5, q4          // q4 = abs(e - olde)
        vmax.U8 q1, q1, q0          // q1 = max(abs(e - newb), 0x300706)
        vmax.U8 q4, q4, q0          // q4 = max(abs(e - olde), 0x300706)
        vsub.I8 q1, q1, q0          // q1 = max(abs(e - newb), 0x300706) - 0x300706
        vsub.I8 q4, q4, q0          // q4 = max(abs(e - olde), 0x300706) - 0x300706
        vorr q14, q14, q15          // q14 |= q15
        vcgt.S32 q1, q1, #0         // q1 = (q1 > 0)?-1:0
        vcgt.S32 q4, q4, #0         // q4 = (q4 > 0)?-1:0
        vbic.I32 q1, #(0xff ^ (1 << 2))     // q1 &= 1 << 2
        vbic.I32 q4, #(0xff ^ (1 << 3))     // q4 &= 1 << 3

        vabd.U8 q15, q5, q6         // q15 = abs(e - newe)
        vabd.U8 q7, q5, q7          // q7 = abs(e - oldh)
        vorr q1, q1, q4             // q1 |= q4
        vmax.U8 q15, q15, q0        // q15 = max(abs(e - newe), 0x300706)
        vmax.U8 q7, q7, q0          // q7 = max(abs(e - oldh), 0x300706)
        vsub.I8 q15, q15, q0        // q15 = max(abs(e - newe), 0x300706) - 0x300706
        vsub.I8 q7, q7, q0          // q7 = max(abs(e - oldh), 0x300706) - 0x300706
        vorr q14, q14, q1           // q14 |= q1
        vcgt.S32 q15, q15, #0       // q15 = (q15 > 0)?-1:0
        vcgt.S32 q7, q7, #0         // q7 = (q7 > 0)?-1:0
        vbic.I32 q15, #(0xff ^ (1 << 4))    // q15 &= 1 << 4
        vbic.I32 q7, #(0xff ^ (1 << 5))     // q7 &= 1 << 5

        vabd.U8 q1, q5, q8          // q1 = abs(e -    h)
        vabd.U8 q4, q5, q9          // q4 = abs(e - newh)
        vmax.U8 q1, q1, q0          // q1 = max(abs(e -    h), 0x300706)
        vmax.U8 q4, q4, q0          // q4 = max(abs(e - newh), 0x300706)
        vsub.I8 q1, q1, q0          // q1 = max(abs(e -    h), 0x300706) - 0x300706
        vsub.I8 q4, q4, q0          // q4 = max(abs(e - newh), 0x300706) - 0x300706
        vcgt.S32 q1, q1, #0         // q1 = (q1 > 0)?-1:0
        vcgt.S32 q4, q4, #0         // q4 = (q4 > 0)?-1:0
        vbic.I32 q1, #(0xff ^ (1 << 6))     // q1 &= 1 << 6
        vbic.I32 q4, #(0xff ^ (1 << 7))     // q4 &= 1 << 7

        vorr q15, q15, q7           // q15 |= q7
        vorr q1, q1, q4             // q1 |= q4
        vorr q14, q14, q15          // q14 |= q15
        vorr q14, q14, q1           // q14 |= q1

        sub ip, ip, #4              // width -= 4

        vtbl.8 d30, {d28, d29}, d20 // d30[0] = q14[0]; d30[1] = q14[4]; d30[2] = q14[8]; d30[3] = q14[12]

        cmp ip, #8

        vst1.32 {d30[0]}, [r3:32]!  // dst[0..3] = pattern[0..3]

        bhs 201b

        cmp ip, #4
        blo 204f

    203:

        vext.32 q1, q2, q11, #3     // oldb = yuvsrc1[-1..2]
        vext.32 q4, q5, q12, #3     // olde = yuvsrc2[-1..2]
        vmov q2, q11                // b = yuvsrc1[0..3]
        vext.32 q7, q8, q13, #3     // oldh = yuvsrc3[-1..2]

        vld1.32 {d22[0]}, [r0:32]   // q11[0] = yuvsrc1[4]

        vmov q5, q12                // e = yuvsrc2[0..3]
        vmov q8, q13                // h = yuvsrc3[0..3]

        vld1.32 {d24[0]}, [r1:32]   // q12[0] = yuvsrc2[4]
        vld1.32 {d26[0]}, [r2:32]   // q13[0] = yuvsrc3[4]

        b 202b

    204:

        add r4, sp, #32             // restore vector registers
        vld1.32 {d8, d9, d10, d11}, [sp]
        vld1.32 {d12, d13, d14, d15}, [r4]
        add sp, sp, #64

    205:

        cmp ip, #0                  // width != 0
        beq 304f

        ldr r4, [r0]                // r4 = yuvsrc1[0] = b
        ldr r5, [r1]                // r5 = yuvsrc2[0] = e
        ldr r6, [r2]                // r6 = yuvsrc3[0] = h

    301:

        ldr r7, [r0, #-4]       // r7 = yuvsrc1[-1] = oldb

        usub8 r8, r5, r7        // r8 = e - oldb
        usub8 r7, r7, r5        // r7 = oldb - e
        sel r8, r7, r8          // r8 = (r7 >= 0)r7:r8       // r8 = abs(oldb - e)
        usub8 r7, lr, r8        // r7 = 0x300706 - abs(oldb - e)
        sel r8, r10, lr         // r8 = (r7 >= 0)?r10:lr     // r8 = (0x300706 >= abs(oldb - e))?0:0x300706

        mov r11, #0             // r11 = 0
        cmp r8, #0
        orrne r11, r11, #1      // r11 |= (r8 != 0)?(1 << 0):0


        usub8 r8, r5, r4        // r8 = e - b
        usub8 r7, r4, r5        // r7 = b - e
        sel r8, r7, r8          // r8 = (r7 >= 0)r7:r8       // r8 = abs(b - e)
        usub8 r7, lr, r8        // r7 = 0x300706 - abs(b - e)
        sel r8, r10, lr         // r8 = (r7 >= 0)?r10:lr     // r8 = (0x300706 >= abs(b - e))?0:0x300706

        cmp r8, #0
        orrne r11, r11, #2      // r11 |= (r8 != 0)?(1 << 1):0


        ldr r4, [r0, #4]!       // r4 = yuvsrc1[1] = newb; yuvsrc1++

        usub8 r8, r5, r4        // r8 = e - newb
        usub8 r7, r4, r5        // r7 = newb - e
        sel r8, r7, r8          // r8 = (r7 >= 0)r7:r8       // r8 = abs(h - e)
        usub8 r7, lr, r8        // r7 = 0x300706 - abs(newb - e)
        sel r8, r10, lr         // r8 = (r7 >= 0)?r10:lr     // r8 = (0x300706 >= abs(newb - e))?0:0x300706

        cmp r8, #0
        orrne r11, r11, #4      // r11 |= (r8 != 0)?(1 << 2):0


        ldr r7, [r2, #-4]       // r7 = yuvsrc3[-1] = oldh

        usub8 r8, r5, r7        // r8 = e - oldh
        usub8 r7, r7, r5        // r7 = oldh - e
        sel r8, r7, r8          // r8 = (r7 >= 0)r7:r8       // r8 = abs(oldh - e)
        usub8 r7, lr, r8        // r7 = 0x300706 - abs(oldh - e)
        sel r8, r10, lr         // r8 = (r7 >= 0)?r10:lr     // r8 = (0x300706 >= abs(oldh - e))?0:0x300706

        cmp r8, #0
        orrne r11, r11, #0x20   // r11 |= (r8 != 0)?(1 << 5):0


        usub8 r8, r5, r6        // r8 = e - h
        usub8 r7, r6, r5        // r7 = h - e
        sel r8, r7, r8          // r8 = (r7 >= 0)r7:r8       // r8 = abs(h - e)
        usub8 r7, lr, r8        // r7 = 0x300706 - abs(h - e)
        sel r8, r10, lr         // r8 = (r7 >= 0)?r10:lr     // r8 = (0x300706 >= abs(h - e))?0:0x300706

        cmp r8, #0
        orrne r11, r11, #0x40   // r11 |= (r8 != 0)?(1 << 6):0


        ldr r6, [r2, #4]!       // r6 = yuvsrc3[1] = newh; yuvsrc3++

        usub8 r8, r5, r6        // r8 = e - newh
        usub8 r7, r6, r5        // r7 = newh - e
        sel r8, r7, r8          // r8 = (r7 >= 0)r7:r8       // r8 = abs(newh - e)
        usub8 r7, lr, r8        // r7 = 0x300706 - abs(newh - e)
        sel r8, r10, lr         // r8 = (r7 >= 0)?r10:lr     // r8 = (0x300706 >= abs(newh - e))?0:0x300706

        cmp r8, #0
        orrne r11, r11, #0x80   // r11 |= (r8 != 0)?(1 << 7):0


        ldr r7, [r1, #-4]       // r7 = yuvsrc2[-1] = olde

        usub8 r8, r5, r7        // r8 = e - olde
        usub8 r7, r7, r5        // r7 = olde - e
        sel r8, r7, r8          // r8 = (r7 >= 0)r7:r8       // r8 = abs(olde - e)
        usub8 r7, lr, r8        // r7 = 0x300706 - abs(olde - e)
        sel r8, r10, lr         // r8 = (r7 >= 0)?r10:lr     // r8 = (0x300706 >= abs(olde - e))?0:0x300706

        cmp r8, #0
        orrne r11, r11, #8      // r11 |= (r8 != 0)?(1 << 3):0


        ldr r7, [r1, #4]!       // r7 = yuvsrc2[1] = newe; yuvsrc3++

        usub8 r8, r5, r7        // r8 = e - newe
        usub8 r5, r7, r5        // r5 = newe - e
        sel r8, r5, r8          // r8 = (r5 >= 0)r5:r8       // r8 = abs(newe - e)
        usub8 r5, lr, r8        // r5 = 0x300706 - abs(newe - e)
        sel r8, r10, lr         // r8 = (r5 >= 0)?r10:lr     // r8 = (0x300706 >= abs(newe - e))?0:0x300706

        cmp r8, #0
        orrne r11, r11, #0x10   // r11 |= (r8 != 0)?(1 << 4):0

        mov r5, r7              // e = newe

        strb r11, [r3], #1      // dst[0] = pattern; dst++

        subS ip, ip, #1
        bne 301b

    304:

        ldr r4, [r1]                // r4 = yuvsrc2[0] = e
        ldr r5, [r0]                // r5 = yuvsrc1[0] = b

        usub8 r7, r4, r5            // r7 = e - b
        usub8 r11, r5, r4           // r11 = b - e
        sel r7, r11, r7             // r7 = (r11 >= 0)r11:r7       // r7 = abs(b - e)
        usub8 r6, lr, r7            // r6 = 0x300706 - abs(b - e)
        sel r7, r10, lr             // r7 = (r6 >= 0)?r10:lr     // r7 = (0x300706 >= abs(b - e))?0:0x300706

        ldr r6, [r2]                // r6 = yuvsrc3[0] = h
        mov r11, #0                 // r11 = 0

        cmp r7, #0
        orrne r11, r11, #7          // r11 |= (r7 != 0)?((1 << 0) | (1 << 1) | (1 << 2)):0

        usub8 r8, r4, r6            // r8 = e - h
        usub8 r7, r6, r4            // r6 = h - e
        sel r8, r7, r8              // r8 = (r7 >= 0)r7:r8       // r8 = abs(h - e)
        usub8 r6, lr, r8            // r6 = 0x300706 - abs(h - e)
        sel r8, r10, lr             // r8 = (r6 >= 0)?r10:lr     // r8 = (0x300706 >= abs(h - e))?0:0x300706

        cmp r8, #0
        orrne r11, r11, #0xe0       // r11 |= (r7 != 0)?((1 << 5) | (1 << 6) | (1 << 7)):0

        strb r11, [r3]              // dst[0] = pattern

        pop {r4-r8,r10,r11,pc}      // restore registers; return

// end procedure calculate_pattern

#elif defined(ARMV8)

.global calculate_pattern

.align 4
.type calculate_pattern, %function
calculate_pattern:

// r0     = const uint32_t *yuvsrc1  (aligned to 8 bytes (64 bits))
// r1     = const uint32_t *yuvsrc2  (aligned to 8 bytes (64 bits))
// r2     = const uint32_t *yuvsrc3  (aligned to 8 bytes (64 bits))
// r3     = uint8_t *dst             (aligned to 8 bytes (64 bits))
// r4     = unsigned int width
// lr     = return address

        stp x29, x30, [sp, #-16]!       // save fp, lr (stack constraint: sp mod 16 = 0)
        movz w7, #0x0706
        movk w7, #0x0030, lsl #16       // w7 = 0x00300706
        mov x29, sp                     // update frame pointer

// free registers: <?r18?>, <?fp(r29)?>, <lr,(r30)>
// free vector registers: v19-v20, <v8-v15>

// r5-r17 = temporary
// v0-v7 = temporary
// v16 = yuvsrc1[ 4..7]
// v17 = yuvsrc2[ 4..7]
// v18 = yuvsrc3[ 4..7]
// v21 = yuvsrc1[-1..2]
// v22 = yuvsrc1[ 0..3] = b
// v23 = yuvsrc1[ 1..4]
// v24 = yuvsrc2[-1..2]
// v25 = yuvsrc2[ 0..3] = e
// v26 = yuvsrc2[ 1..4]
// v27 = yuvsrc3[-1..2]
// v28 = yuvsrc3[ 0..3] = h
// v29 = yuvsrc3[ 1..4]
// v30 = 0x00300706
// v31 = 0x0c080400

        ld1r {v0.2S}, [x1]                  // v0[0..1] = yuvsrc2[0] = e
        ld1 {v1.S}[0], [x0]                 // v1[0] = yuvsrc1[0] = b
        ld1 {v1.S}[1], [x2]                 // v1[1] = yuvsrc3[0] = h

        movz w6, #0x0400
        movk w6, #0x0c08, lsl #16           // w6 = 0x0c080400

        dup v30.4S, w7                      // v30[0..3] = 0x00300706
        dup v31.4S, w6                      // v31[0..3] = 0x0c080400

        uabd v1.8B, v0.8B, v1.8B            // v1 = abs(e - <b/h>)
        umax v1.8B, v1.8B, v30.8B           // v1 = max(abs(e - <b/h>), 0x300706)
        sub v1.8B, v1.8B, v30.8B            // v1 = max(abs(e - <b/h>), 0x300706) - 0x300706

        mov w10, v1.S[0]                    // w10 = max(abs(e - b), 0x300706) - 0x300706
        mov w11, v1.S[1]                    // w11 = max(abs(e - h), 0x300706) - 0x300706

        cmp w10, #0
        mov w6, #7                          // w6 = (1 << 0) | (1 << 1) | (1 << 2)
        csel w7, w6, wzr, NE                // w7 = (w10 != 0) ? w6 : 0

        cmp w11, #0
        orr w6, w7, #0xe0                   // w6 = w7 | (1 << 5) | (1 << 6) | (1 << 7)
        csel w7, w6, w7, NE                 // w7 = (w11 != 0) ? w6 : w7

        cmp w4, #4

        sturb w7, [x3, #-1]                 // dst[-1] = pattern

        b.lo 204f

        ldr q16, [x0], #16                  // v16 = yuvsrc1[0..3]
        ldr q17, [x1], #16                  // v17 = yuvsrc2[0..3]
        ldr q18, [x2], #16                  // v18 = yuvsrc3[0..3]

        cmp w4, #8

        mov v22.S[3], v16.S[0]              // b = yuvsrc1[?..0] = yuvsrc1[?..-1]
        mov v25.S[3], v17.S[0]              // e = yuvsrc2[?..0] = yuvsrc2[?..-1]
        mov v28.S[3], v18.S[0]              // h = yuvsrc3[?..0] = yuvsrc3[?..-1]

        b.lo 203f

    201:
        ext v21.16B, v22.16B, v16.16B, #12  // oldb = yuvsrc1[-1..2]
        ext v24.16B, v25.16B, v17.16B, #12  // olde = yuvsrc2[-1..2]
        mov v22.16B, v16.16B                // b = yuvsrc1[0..3]
        ext v27.16B, v28.16B, v18.16B, #12  // oldh = yuvsrc3[-1..2]

        ldr q16, [x0], #16                  // v16 = yuvsrc1[4..7]

        mov v25.16B, v17.16B                // e = yuvsrc2[0..3]
        mov v28.16B, v18.16B                // h = yuvsrc3[0..3]

        ldr q17, [x1], #16                  // v17 = yuvsrc2[4..7]
        ldr q18, [x2], #16                  // v18 = yuvsrc3[4..7]

    202:
        ext v23.16B, v22.16B, v16.16B, #4   // newb = (b >> 32) | (yuvsrc1[4] << 96) = yuvsrc1[1..4]
        ext v26.16B, v25.16B, v17.16B, #4   // newe = (e >> 32) | (yuvsrc2[4] << 96) = yuvsrc2[1..4]
        ext v29.16B, v28.16B, v18.16B, #4   // newh = (h >> 32) | (yuvsrc3[4] << 96) = yuvsrc3[1..4]

        // v21-v29 contain data

        uabd v0.16B, v25.16B, v21.16B   // v0 = abs(e - oldb)
        uabd v1.16B, v25.16B, v22.16B   // v1 = abs(e -    b)
        umax v0.16B, v0.16B, v30.16B    // v0 = max(abs(e - oldb), 0x300706)
        umax v1.16B, v1.16B, v30.16B    // v1 = max(abs(e -    b), 0x300706)
        sub v0.16B, v0.16B, v30.16B     // v0 = max(abs(e - oldb), 0x300706) - 0x300706
        sub v1.16B, v1.16B, v30.16B     // v1 = max(abs(e -    b), 0x300706) - 0x300706
        cmgt v0.4S, v0.4S, #0           // v0 = (v0 > 0)?-1:0
        cmgt v1.4S, v1.4S, #0           // v1 = (v1 > 0)?-1:0
        bic v0.4S, #(0xff ^ (1 << 0))   // v0 &= 1 << 0
        bic v1.4S, #(0xff ^ (1 << 1))   // v1 &= 1 << 1

        uabd v2.16B, v25.16B, v23.16B   // v2 = abs(e - newb)
        uabd v3.16B, v25.16B, v24.16B   // v3 = abs(e - olde)
        umax v2.16B, v2.16B, v30.16B    // v2 = max(abs(e - newb), 0x300706)
        umax v3.16B, v3.16B, v30.16B    // v3 = max(abs(e - olde), 0x300706)
        sub v2.16B, v2.16B, v30.16B     // v2 = max(abs(e - newb), 0x300706) - 0x300706
        sub v3.16B, v3.16B, v30.16B     // v3 = max(abs(e - olde), 0x300706) - 0x300706
        cmgt v2.4S, v2.4S, #0           // v2 = (v2 > 0)?-1:0
        cmgt v3.4S, v3.4S, #0           // v3 = (v3 > 0)?-1:0
        bic v2.4S, #(0xff ^ (1 << 2))   // v2 &= 1 << 2
        bic v3.4S, #(0xff ^ (1 << 3))   // v3 &= 1 << 3
        orr v0.16B, v0.16B, v1.16B      // v0 |= v1

        uabd v4.16B, v25.16B, v26.16B   // v4 = abs(e - newe)
        uabd v5.16B, v25.16B, v27.16B   // v5 = abs(e - oldh)
        umax v4.16B, v4.16B, v30.16B    // v4 = max(abs(e - newe), 0x300706)
        umax v5.16B, v5.16B, v30.16B    // v5 = max(abs(e - oldh), 0x300706)
        sub v4.16B, v4.16B, v30.16B     // v4 = max(abs(e - newe), 0x300706) - 0x300706
        sub v5.16B, v5.16B, v30.16B     // v5 = max(abs(e - oldh), 0x300706) - 0x300706
        cmgt v4.4S, v4.4S, #0           // v4 = (v4 > 0)?-1:0
        cmgt v5.4S, v5.4S, #0           // v5 = (v5 > 0)?-1:0
        bic v4.4S, #(0xff ^ (1 << 4))   // v4 &= 1 << 4
        bic v5.4S, #(0xff ^ (1 << 5))   // v5 &= 1 << 5
        orr v2.16B, v2.16B, v3.16B      // v2 |= v3

        uabd v6.16B, v25.16B, v28.16B   // v6 = abs(e -    h)
        uabd v7.16B, v25.16B, v29.16B   // v7 = abs(e - newh)
        umax v6.16B, v6.16B, v30.16B    // v6 = max(abs(e -    h), 0x300706)
        umax v7.16B, v7.16B, v30.16B    // v7 = max(abs(e - newh), 0x300706)
        sub v6.16B, v6.16B, v30.16B     // v6 = max(abs(e -    h), 0x300706) - 0x300706
        sub v7.16B, v7.16B, v30.16B     // v7 = max(abs(e - newh), 0x300706) - 0x300706
        cmgt v6.4S, v6.4S, #0           // v6 = (v6 > 0)?-1:0
        cmgt v7.4S, v7.4S, #0           // v7 = (v7 > 0)?-1:0
        bic v6.4S, #(0xff ^ (1 << 6))   // v6 &= 1 << 6
        bic v7.4S, #(0xff ^ (1 << 7))   // v7 &= 1 << 7
        orr v4.16B, v4.16B, v5.16B      // v4 |= v5

        orr v6.16B, v6.16B, v7.16B      // v6 |= v7

        orr v0.16B, v0.16B, v2.16B      // v0 |= v2
        orr v4.16B, v4.16B, v6.16B      // v4 |= v6

        orr v0.16B, v0.16B, v4.16B      // v0 |= v4

        tbl v0.16B, {v0.16B}, v31.16B   // v0[0] = v0[0]; v0[1] = v0[4]; v0[2] = v0[8]; v0[3] = v0[12]

        sub w4, w4, #4          // width -= 4

        st1 {v0.S}[0], [x3], #4         // dst[0..3] = pattern[0..3]

        cmp w4, #8
        b.hs 201b

        cmp w4, #4
        b.lo 204f

    203:
        ext v21.16B, v22.16B, v16.16B, #12  // oldb = yuvsrc1[-1..2]
        ext v24.16B, v25.16B, v17.16B, #12  // olde = yuvsrc2[-1..2]
        mov v22.16B, v16.16B                // b = yuvsrc1[0..3]
        ext v27.16B, v28.16B, v18.16B, #12  // oldh = yuvsrc3[-1..2]

        ld1 {v16.S}[0], [x0]                // v16[0] = yuvsrc1[4]

        mov v25.16B, v17.16B                // e = yuvsrc2[0..3]
        mov v28.16B, v18.16B                // h = yuvsrc3[0..3]

        ld1 {v17.S}[0], [x1]                // v17[0] = yuvsrc2[4]
        ld1 {v18.S}[0], [x2]                // v18[0] = yuvsrc3[4]

        b 202b

    204:

        cbz w4, 304f

        ldp w5, w10, [x0, #-4]              // w5 = yuvsrc1[-1], w10 = yuvsrc1[0] = b
        ldp w6, w11, [x1, #-4]              // w6 = yuvsrc2[-1], w11 = yuvsrc2[0] = e
        ldp w7, w12, [x2, #-4]              // w7 = yuvsrc3[-1], w12 = yuvsrc3[0] = h

    301:
        ldr w13, [x0, #4]!                  // w13 = yuvsrc1[1]
        dup v0.4S, w11                      // v0[0..3] = e
        ldr w14, [x1, #4]!                  // w14 = yuvsrc2[1]
        mov v1.S[0], w5                     // v1[0] = oldb
        ldr w15, [x2, #4]!                  // w15 = yuvsrc3[1]

        mov v1.S[1], w10                    // v1[1] = b
        mov v1.S[2], w13                    // v1[2] = newb
        mov v1.S[3], w6                     // v1[3] = olde

        mov v2.S[0], w14                    // v2[0] = newe
        mov v2.S[1], w7                     // v2[1] = olhd
        mov v2.S[2], w12                    // v2[2] = h
        mov v2.S[3], w15                    // v2[3] = newh

        uabd v3.16B, v0.16B, v1.16B         // v3 = abs(v0 - v1)
        uabd v4.16B, v0.16B, v2.16B         // v4 = abs(v0 - v2)

        umax v3.16B, v3.16B, v30.16B        // v3 = max(0x300706, abs(v0 - v1))
        umax v3.16B, v3.16B, v30.16B        // v4 = max(0x300706, abs(v0 - v2))

        sub v3.16B, v3.16B, v30.16B         // v3 = max(0x300706, abs(v0 - v1)) - 0x300706
        sub v4.16B, v4.16B, v30.16B         // v4 = max(0x300706, abs(v0 - v2)) - 0x300706

        mov w8, v3.S[0]
        mov w9, v3.S[1]
        mov w16, v3.S[2]
        mov w17, v3.S[3]

        cmp w8, #0
        cset w7, NE             // w7 = (w8 <> 0)?1:0

        cmp w9, #0
        orr w6, w7, #(1 << 1)   // w6 = w7 | (1 << 1)
        mov w8, v4.S[0]
        csel w7, w6, w7, NE     // w7 = (w9 != 0) ? w6 : w7

        cmp w16, #0
        orr w6, w7, #(1 << 2)   // w6 = w7 | (1 << 2)
        mov w9, v4.S[1]
        csel w7, w6, w7, NE     // w7 = (w16 != 0) ? w6 : w7

        cmp w17, #0
        orr w6, w7, #(1 << 3)   // w6 = w7 | (1 << 3)
        mov w16, v4.S[2]
        csel w7, w6, w7, NE     // w7 = (w17 != 0) ? w6 : w7

        cmp w8, #0
        orr w6, w7, #(1 << 4)   // w6 = w7 | (1 << 4)
        mov w17, v4.S[3]
        csel w7, w6, w7, NE     // w7 = (w8 != 0) ? w6 : w7

        cmp w9, #0
        orr w6, w7, #(1 << 5)   // w6 = w7 | (1 << 5)
        mov w5, w10
        csel w7, w6, w7, NE     // w7 = (w9 != 0) ? w6 : w7

        cmp w16, #0
        orr w6, w7, #(1 << 6)   // w6 = w7 | (1 << 6)
        sub w4, w4, #1
        csel w7, w6, w7, NE     // w7 = (w16 != 0) ? w6 : w7

        cmp w17, #0
        orr w6, w7, #(1 << 7)   // w6 = w7 | (1 << 7)
        mov w10, w13
        csel w7, w6, w7, NE     // w7 = (w17 != 0) ? w6 : w7

        mov w6, w11

        strb w7, [x3], #1       // dst[0] = pattern

        mov w11, w14
        mov w7, w12
        mov w12, w15

        cbnz w4, 301b

    304:

        ld1r {v0.2S}, [x1]                  // v0[0..1] = yuvsrc2[0] = e
        ld1 {v1.S}[0], [x0]                 // v1[0] = yuvsrc1[0] = b
        ld1 {v1.S}[1], [x2]                 // v1[1] = yuvsrc3[0] = h

        uabd v1.8B, v0.8B, v1.8B            // v1 = abs(e - <b/h>)
        umax v1.8B, v1.8B, v30.8B           // v1 = max(abs(e - <b/h>), 0x300706)
        sub v1.8B, v1.8B, v30.8B            // v1 = max(abs(e - <b/h>), 0x300706) - 0x300706

        mov w10, v1.S[0]                    // w10 = max(abs(e - b), 0x300706) - 0x300706
        mov w11, v1.S[1]                    // w11 = max(abs(e - h), 0x300706) - 0x300706

        cmp w10, #0
        mov w6, #7                          // w6 = (1 << 0) | (1 << 1) | (1 << 2)
        csel w7, w6, wzr, NE                // w7 = (w10 != 0) ? w6 : 0

        cmp w11, #0
        orr w6, w7, #0xe0                   // w6 = w7 | (1 << 5) | (1 << 6) | (1 << 7)
        csel w7, w6, w7, NE                 // w7 = (w11 != 0) ? w6 : w7

        strb w7, [x3]                       // dst[0] = pattern

        ldp x29, x30, [sp], #16             // restore fp, lr
        ret

// end procedure calculate_pattern

#endif

#if !((defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)))
.section .note.GNU-stack,"",%progbits
#endif

