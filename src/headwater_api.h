#ifndef _HEADWATER_API_H_
#define _HEADWATER_API_H_

#define API_STATE_INCLUDE "headwater_state.h"
#define API_STATE HeadwaterState

#include "api.h"

typedef enum {
  HEADWATER_API_GET_BPM,
  HEADWATER_API_GET_MULTIPLIER_A,
  HEADWATER_API_GET_MULTIPLIER_B,
  HEADWATER_API_GET_SAMPLES_PER_BEAT,
  HEADWATER_API_GET_SAMPLES_PER_MULTIPLIER_A,
  HEADWATER_API_GET_SAMPLES_PER_MULTIPLIER_B
} HEADWATER_API_CMD;

void headwater_api_payload_preprocessor(API *api);
void headwater_api_payload_postprocessor(API *api);

#endif
