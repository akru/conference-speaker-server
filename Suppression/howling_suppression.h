#ifndef HOWLING_SUPPRESSION_H
#define HOWLING_SUPPRESSION_H

#define HS_BLOCKL_MAX 1024
#define HS_IP_LENGTH (HS_BLOCKL_MAX >> 1) // must be at least ceil(2 + sqrt(ANAL_BLOCKL_MAX/2))
#define HS_W_LENGTH (HS_BLOCKL_MAX >> 1)
#define HS_BIQUAD_COUNT 20

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    float a0, a1, a2, b1, b2,
          Q, peakGain,
          z1, z2;
} HsBiquadParams;

typedef struct
{
    // Analyzer data
    int   ip[HS_IP_LENGTH];
    float wfft[HS_W_LENGTH],
          dataBuf[HS_BLOCKL_MAX];
    // Frame data
    int   blockLen;
    // Filters data
    HsBiquadParams filter[HS_BIQUAD_COUNT];
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
