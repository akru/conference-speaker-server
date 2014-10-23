/* SoX Resampler Library      Copyright (c) 2007-13 robs@users.sourceforge.net
 * Licence for this file: LGPL v2.1                  See LICENCE for details. */

#if !defined soxr_config_included
#define soxr_config_included

#define HAVE_SINGLE_PRECISION 1
#define HAVE_DOUBLE_PRECISION 1
//#define HAVE_AVFFT            1
#define HAVE_SIMD             1
#define HAVE_FENV_H           1
//#define HAVE_LRINT            1
//#define WORDS_BIGENDIAN       1

#include <limits.h>
#include <stdint.h>

#undef bool
#undef false
#undef true
#define bool int
#define false 0
#define true 1

#undef int16_t
#undef int32_t
#undef int64_t
#undef uint32_t
#undef uint64_t
#define int16_t short
#if LONG_MAX > 2147483647L
  #define int32_t int
  #define int64_t long
#elif LONG_MAX < 2147483647L
#error this library requires that 'long int' has at least 32-bits
#else
  #define int32_t long
  #if defined _MSC_VER
    #define int64_t __int64
  #else
    #define int64_t long long
  #endif
#endif
#define uint32_t unsigned int32_t
#define uint64_t unsigned int64_t

#if defined _MSC_VER
#define _USE_MATH_DEFINES
#endif

#endif
