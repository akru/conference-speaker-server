#ifndef HOWLING_SUPPRESSION_H
#define HOWLING_SUPPRESSION_H

#define PI_F 3.1415926535897932384626433832795f

#define HS_BLOCKL_MAX       1024  // In bins
#define HS_IP_LENGTH        (HS_BLOCKL_MAX >> 1) // must be at least ceil(2 + sqrt(ANAL_BLOCKL_MAX/2))
#define HS_W_LENGTH         (HS_BLOCKL_MAX >> 1)
#define HS_BIQUAD_COUNT     20
#define HS_FREQ_DEVIATION   50 // Howling ignor area in Hz
#define HS_FREQ_MIN         10
#define HS_FREQ_MAX         10000
#define HS_GAIN_STEP        3     // Gain step in dB
#define HS_GAIN_MIN         -30   // Gain minimal value
#define HS_SAMPLE_RATE      8000.0
#define HS_BLOCKL           256   // In bins
#define HS_M                HS_BLOCKL_MAX / 2.0
#define HS_INDEX_TO_HZ      HS_SAMPLE_RATE / HS_M
#define HS_INDEX_TO_RAD     2 * PI_F * HS_SAMPLE_RATE / HS_M
#define HS_HZ_TO_INDEX      HS_M / HS_SAMPLE_RATE
#define HS_RAD_TO_INDEX     HS_M / 2 / PI_F / HS_SAMPLE_RATE

#include "biquad.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    // Analyzer data
    int   ip[HS_IP_LENGTH];
    float wfft[HS_W_LENGTH],
          dataBuf[HS_BLOCKL_MAX];
    // Frame data
    int   blockLen;
    int   howlingFreq[HS_BIQUAD_COUNT];
    // Filters data
    HsBiquadParams filter[HS_BIQUAD_COUNT];
    int   filterCount;
} HsHandle;

/*
 * Howling suppression handle cration function
 */
int Hs_Create(HsHandle **);

/*
 * Init handle
 */
int Hs_Init(HsHandle *, int);

/*
 * Howling suppression handle free function
 */
void Hs_Free(HsHandle *);

/*
 * Process buffer
 */
void Hs_Process(HsHandle *, const short *, short *);

#ifdef __cplusplus
}
#endif

#endif // HOWLING_SUPPRESSION_H
