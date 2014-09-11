/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MODULES_AUDIO_PROCESSING_NS_MAIN_SOURCE_DEFINES_H_
#define WEBRTC_MODULES_AUDIO_PROCESSING_NS_MAIN_SOURCE_DEFINES_H_

/*
 * Global defines
 */

#define PI_F 3.1415926535897932384626433832795f

#define BLOCK_LEN_INPUT     256

/*
 * Howling suppression
 */

#define HS_DEBUG
#define HS_BUF_COUNT        20
#define HS_BLOCKL           BLOCK_LEN_INPUT             // Input block
#define HS_BLOCKL_A         HS_BLOCKL / 2               // Analyzed block
#define HS_BLOCKL_A_MAX     HS_BLOCKL_A * HS_BUF_COUNT  // Buffer length
#define HS_IP_LENGTH        (HS_BLOCKL * 2 >> 1) // Must be at least ceil(2 + sqrt(HS_BLOCKL/2))
#define HS_W_LENGTH         (HS_BLOCKL * 2 >> 1)
#define HS_BIQUAD_COUNT     20
#define HS_FREQ_DEVIATION2  100
#define HS_FREQ_DEVIATION   50    // Howling freq interpolation range in Hz
#define HS_FREQ_MIN         10    // Minimal freq
#define HS_FREQ_MAX         8000  // Maximal freq
#define HS_GAIN_UP_STEP     1     // Gain raise step in dB
#define HS_GAIN_DOWN_STEP   3     // Gain decrise step in dB
#define HS_GAIN_MIN         -60   // Gain minimal value
#define HS_SAMPLE_RATE      16000.0
#define HS_M                HS_BLOCKL
#define HS_INDEX_TO_HZ      HS_SAMPLE_RATE / HS_M
#define HS_INDEX_TO_RAD     2 * PI_F * HS_SAMPLE_RATE / HS_M
#define HS_HZ_TO_INDEX      HS_M / HS_SAMPLE_RATE
#define HS_RAD_TO_INDEX     HS_M / 2 / PI_F / HS_SAMPLE_RATE
#define HS_GROUP_FREQ_MAX   50    // Max count of freq recs in group
#define HS_PHPR_UNDEF       100

/*
 * Google Noise suppression
 */

//#define PROCESS_FLOW_0    // Use the traditional method.
//#define PROCESS_FLOW_1    // Use traditional with DD estimate of prior SNR.
#define PROCESS_FLOW_2    // Use the new method of speech/noise classification.

#define BLOCKL_MAX          256 // max processing block length: 160
#define ANAL_BLOCKL_MAX     512 // max analysis block length: 256
#define HALF_ANAL_BLOCKL    257 // half max analysis block length + 1

#define QUANTILE            (float)0.25

#define SIMULT              3
#define END_STARTUP_LONG    200
#define END_STARTUP_SHORT   50
#define FACTOR              (float)40.0
#define WIDTH               (float)0.01

#define SMOOTH              (float)0.75 // filter smoothing
// Length of fft work arrays.
#define IP_LENGTH (ANAL_BLOCKL_MAX >> 1) // must be at least ceil(2 + sqrt(ANAL_BLOCKL_MAX/2))
#define W_LENGTH (ANAL_BLOCKL_MAX >> 1)

//PARAMETERS FOR NEW METHOD
#define DD_PR_SNR           (float)0.98 // DD update of prior SNR
#define LRT_TAVG            (float)0.50 // tavg parameter for LRT (previously 0.90)
#define SPECT_FL_TAVG       (float)0.30 // tavg parameter for spectral flatness measure
#define SPECT_DIFF_TAVG     (float)0.30 // tavg parameter for spectral difference measure
#define PRIOR_UPDATE        (float)0.10 // update parameter of prior model
#define NOISE_UPDATE        (float)0.90 // update parameter for noise
#define SPEECH_UPDATE       (float)0.99 // update parameter when likely speech
#define WIDTH_PR_MAP        (float)4.0  // width parameter in sigmoid map for prior model
#define LRT_FEATURE_THR     (float)0.5  // default threshold for LRT feature
#define SF_FEATURE_THR      (float)0.5  // default threshold for Spectral Flatness feature
#define SD_FEATURE_THR      (float)0.5  // default threshold for Spectral Difference feature
#define PROB_RANGE          (float)0.20 // probability threshold for noise state in
                                        // speech/noise likelihood
#define HIST_PAR_EST         1000       // histogram size for estimation of parameters
#define GAMMA_PAUSE         (float)0.05 // update for conservative noise estimate
//
#define B_LIM               (float)0.5  // threshold in final energy gain factor calculation
#endif // WEBRTC_MODULES_AUDIO_PROCESSING_NS_MAIN_SOURCE_DEFINES_H_
