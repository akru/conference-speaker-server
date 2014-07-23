#ifndef HOWLING_SUPPRESSION_H
#define HOWLING_SUPPRESSION_H

#include "biquad.h"
#include "defines.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    char  debug;
    // Analyzer data
    int   ip[HS_IP_LENGTH];
    float wfft[HS_W_LENGTH],
          dataBuf[HS_BLOCKL_MAX],
    // Trasholds
          PAPR_TH, PHPR_TH, PNPR_TH;
    // Filters data
    HsBiquadParams filter[HS_BIQUAD_COUNT];
    int            filterCount;
} HsHandle;

/*
 * Howling suppression handle cration function
 */
int Hs_Create(HsHandle **);

/*
 * Init handle
 */
int Hs_Init(HsHandle *, int, float PAPR_TH, float PHPR_TH, float PNPR_TH);

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
