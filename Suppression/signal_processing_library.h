/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */


/*
 * This header file includes all of the fix point signal processing library (SPL) function
 * descriptions and declarations.
 * For specific function calls, see bottom of file.
 */

#ifndef WEBRTC_SPL_SIGNAL_PROCESSING_LIBRARY_H_
#define WEBRTC_SPL_SIGNAL_PROCESSING_LIBRARY_H_

#include <string.h>
#include "typedefs.h"

// Macros specific for the fixed point implementation
#define WEBRTC_SPL_WORD16_MAX       32767
#define WEBRTC_SPL_WORD16_MIN       -32768
#define WEBRTC_SPL_WORD32_MAX       (WebRtc_Word32)0x7fffffff
#define WEBRTC_SPL_WORD32_MIN       (WebRtc_Word32)0x80000000
#define WEBRTC_SPL_MAX_LPC_ORDER    14
#define WEBRTC_SPL_MAX_SEED_USED    0x80000000L
#define WEBRTC_SPL_MIN(A, B)        (A < B ? A : B) // Get min value
#define WEBRTC_SPL_MAX(A, B)        (A > B ? A : B) // Get max value
#define WEBRTC_SPL_ABS_W16(a) \
    (((WebRtc_Word16)a >= 0) ? ((WebRtc_Word16)a) : -((WebRtc_Word16)a))
#define WEBRTC_SPL_ABS_W32(a) \
    (((WebRtc_Word32)a >= 0) ? ((WebRtc_Word32)a) : -((WebRtc_Word32)a))

#if (defined WEBRTC_TARGET_PC)||(defined __TARGET_XSCALE)
#define WEBRTC_SPL_GET_BYTE(a, nr)  (((WebRtc_Word8 *)a)[nr])
#define WEBRTC_SPL_SET_BYTE(d_ptr, val, index) \
    (((WebRtc_Word8 *)d_ptr)[index] = (val))
#elif defined WEBRTC_BIG_ENDIAN
#define WEBRTC_SPL_GET_BYTE(a, nr) \
    ((((WebRtc_Word16 *)a)[nr >> 1]) >> (((nr + 1) & 0x1) * 8) & 0x00ff)
#define WEBRTC_SPL_SET_BYTE(d_ptr, val, index) \
    ((WebRtc_Word16 *)d_ptr)[index >> 1] = \
    ((((WebRtc_Word16 *)d_ptr)[index >> 1]) \
    & (0x00ff << (8 * ((index) & 0x1)))) | (val << (8 * ((index + 1) & 0x1)))
#else
#define WEBRTC_SPL_GET_BYTE(a,nr) \
    ((((WebRtc_Word16 *)(a))[(nr) >> 1]) >> (((nr) & 0x1) * 8) & 0x00ff)
#define WEBRTC_SPL_SET_BYTE(d_ptr, val, index) \
    ((WebRtc_Word16 *)(d_ptr))[(index) >> 1] = \
    ((((WebRtc_Word16 *)(d_ptr))[(index) >> 1]) \
    & (0x00ff << (8 * (((index) + 1) & 0x1)))) | \
    ((val) << (8 * ((index) & 0x1)))
#endif

#define WEBRTC_SPL_MUL(a, b) \
    ((WebRtc_Word32) ((WebRtc_Word32)(a) * (WebRtc_Word32)(b)))
#define WEBRTC_SPL_UMUL(a, b) \
    ((WebRtc_UWord32) ((WebRtc_UWord32)(a) * (WebRtc_UWord32)(b)))
#define WEBRTC_SPL_UMUL_RSFT16(a, b) \
    ((WebRtc_UWord32) ((WebRtc_UWord32)(a) * (WebRtc_UWord32)(b)) >> 16)
#define WEBRTC_SPL_UMUL_16_16(a, b) \
    ((WebRtc_UWord32) (WebRtc_UWord16)(a) * (WebRtc_UWord16)(b))
#define WEBRTC_SPL_UMUL_16_16_RSFT16(a, b) \
    (((WebRtc_UWord32) (WebRtc_UWord16)(a) * (WebRtc_UWord16)(b)) >> 16)
#define WEBRTC_SPL_UMUL_32_16(a, b) \
    ((WebRtc_UWord32) ((WebRtc_UWord32)(a) * (WebRtc_UWord16)(b)))
#define WEBRTC_SPL_UMUL_32_16_RSFT16(a, b) \
    ((WebRtc_UWord32) ((WebRtc_UWord32)(a) * (WebRtc_UWord16)(b)) >> 16)
#define WEBRTC_SPL_MUL_16_U16(a, b) \
    ((WebRtc_Word32)(WebRtc_Word16)(a) * (WebRtc_UWord16)(b))
#define WEBRTC_SPL_DIV(a, b) \
    ((WebRtc_Word32) ((WebRtc_Word32)(a) / (WebRtc_Word32)(b)))
#define WEBRTC_SPL_UDIV(a, b) \
    ((WebRtc_UWord32) ((WebRtc_UWord32)(a) / (WebRtc_UWord32)(b)))

#ifndef WEBRTC_ARCH_ARM_V7A
// For ARMv7 platforms, these are inline functions in spl_inl_armv7.h
#define WEBRTC_SPL_MUL_16_16(a, b) \
    ((WebRtc_Word32) (((WebRtc_Word16)(a)) * ((WebRtc_Word16)(b))))
#define WEBRTC_SPL_MUL_16_32_RSFT16(a, b) \
    (WEBRTC_SPL_MUL_16_16(a, b >> 16) \
     + ((WEBRTC_SPL_MUL_16_16(a, (b & 0xffff) >> 1) + 0x4000) >> 15))
#define WEBRTC_SPL_MUL_32_32_RSFT32(a32a, a32b, b32) \
    ((WebRtc_Word32)(WEBRTC_SPL_MUL_16_32_RSFT16(a32a, b32) \
    + (WEBRTC_SPL_MUL_16_32_RSFT16(a32b, b32) >> 16)))
#define WEBRTC_SPL_MUL_32_32_RSFT32BI(a32, b32) \
    ((WebRtc_Word32)(WEBRTC_SPL_MUL_16_32_RSFT16(( \
    (WebRtc_Word16)(a32 >> 16)), b32) + \
    (WEBRTC_SPL_MUL_16_32_RSFT16(( \
    (WebRtc_Word16)((a32 & 0x0000FFFF) >> 1)), b32) >> 15)))
#endif

#define WEBRTC_SPL_MUL_16_32_RSFT11(a, b) \
    ((WEBRTC_SPL_MUL_16_16(a, (b) >> 16) << 5) \
    + (((WEBRTC_SPL_MUL_16_U16(a, (WebRtc_UWord16)(b)) >> 1) + 0x0200) >> 10))
#define WEBRTC_SPL_MUL_16_32_RSFT14(a, b) \
    ((WEBRTC_SPL_MUL_16_16(a, (b) >> 16) << 2) \
    + (((WEBRTC_SPL_MUL_16_U16(a, (WebRtc_UWord16)(b)) >> 1) + 0x1000) >> 13))
#define WEBRTC_SPL_MUL_16_32_RSFT15(a, b) \
    ((WEBRTC_SPL_MUL_16_16(a, (b) >> 16) << 1) \
    + (((WEBRTC_SPL_MUL_16_U16(a, (WebRtc_UWord16)(b)) >> 1) + 0x2000) >> 14))

#ifdef ARM_WINM
#define WEBRTC_SPL_MUL_16_16(a, b) \
    _SmulLo_SW_SL((WebRtc_Word16)(a), (WebRtc_Word16)(b))
#endif

#define WEBRTC_SPL_MUL_16_16_RSFT(a, b, c) \
    (WEBRTC_SPL_MUL_16_16(a, b) >> (c))

#define WEBRTC_SPL_MUL_16_16_RSFT_WITH_ROUND(a, b, c) \
    ((WEBRTC_SPL_MUL_16_16(a, b) + ((WebRtc_Word32) \
                                  (((WebRtc_Word32)1) << ((c) - 1)))) >> (c))
#define WEBRTC_SPL_MUL_16_16_RSFT_WITH_FIXROUND(a, b) \
    ((WEBRTC_SPL_MUL_16_16(a, b) + ((WebRtc_Word32) (1 << 14))) >> 15)

// C + the 32 most significant bits of A * B
#define WEBRTC_SPL_SCALEDIFF32(A, B, C) \
    (C + (B >> 16) * A + (((WebRtc_UWord32)(0x0000FFFF & B) * A) >> 16))

#define WEBRTC_SPL_ADD_SAT_W32(a, b)    WebRtcSpl_AddSatW32(a, b)
#define WEBRTC_SPL_SAT(a, b, c)         (b > a ? a : b < c ? c : b)
#define WEBRTC_SPL_MUL_32_16(a, b)      ((a) * (b))

#define WEBRTC_SPL_SUB_SAT_W32(a, b)    WebRtcSpl_SubSatW32(a, b)
#define WEBRTC_SPL_ADD_SAT_W16(a, b)    WebRtcSpl_AddSatW16(a, b)
#define WEBRTC_SPL_SUB_SAT_W16(a, b)    WebRtcSpl_SubSatW16(a, b)

// We cannot do casting here due to signed/unsigned problem
#define WEBRTC_SPL_IS_NEG(a)            ((a) & 0x80000000)
// Shifting with negative numbers allowed
// Positive means left shift
#define WEBRTC_SPL_SHIFT_W16(x, c) \
    (((c) >= 0) ? ((x) << (c)) : ((x) >> (-(c))))
#define WEBRTC_SPL_SHIFT_W32(x, c) \
    (((c) >= 0) ? ((x) << (c)) : ((x) >> (-(c))))

// Shifting with negative numbers not allowed
// We cannot do casting here due to signed/unsigned problem
#define WEBRTC_SPL_RSHIFT_W16(x, c)     ((x) >> (c))
#define WEBRTC_SPL_LSHIFT_W16(x, c)     ((x) << (c))
#define WEBRTC_SPL_RSHIFT_W32(x, c)     ((x) >> (c))
#define WEBRTC_SPL_LSHIFT_W32(x, c)     ((x) << (c))

#define WEBRTC_SPL_RSHIFT_U16(x, c)     ((WebRtc_UWord16)(x) >> (c))
#define WEBRTC_SPL_LSHIFT_U16(x, c)     ((WebRtc_UWord16)(x) << (c))
#define WEBRTC_SPL_RSHIFT_U32(x, c)     ((WebRtc_UWord32)(x) >> (c))
#define WEBRTC_SPL_LSHIFT_U32(x, c)     ((WebRtc_UWord32)(x) << (c))

#define WEBRTC_SPL_VNEW(t, n)           (t *) malloc (sizeof (t) * (n))
#define WEBRTC_SPL_FREE                 free

#define WEBRTC_SPL_RAND(a) \
    ((WebRtc_Word16)(WEBRTC_SPL_MUL_16_16_RSFT((a), 18816, 7) & 0x00007fff))

#ifdef __cplusplus
extern "C"
{
#endif

#define WEBRTC_SPL_MEMCPY_W8(v1, v2, length) \
   memcpy(v1, v2, (length) * sizeof(char))
#define WEBRTC_SPL_MEMCPY_W16(v1, v2, length) \
   memcpy(v1, v2, (length) * sizeof(WebRtc_Word16))

#define WEBRTC_SPL_MEMMOVE_W16(v1, v2, length) \
   memmove(v1, v2, (length) * sizeof(WebRtc_Word16))

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // WEBRTC_SPL_SIGNAL_PROCESSING_LIBRARY_H_
