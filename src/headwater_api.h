#ifndef _HEADWATER_API_H_
#define _HEADWATER_API_H_

#include "headwater_state.h"
#define API_STATE HeadwaterState
#include "api.h"

// This comment is here so the commands start on line 10
typedef enum {
  HEADWATER_API_GET_OUTPUT_ENABLED,
  HEADWATER_API_GET_MODE,
  HEADWATER_API_GET_BPM,
  HEADWATER_API_GET_MULTIPLIER_A,
  HEADWATER_API_GET_MULTIPLIER_B,
  HEADWATER_API_GET_SAMPLES_PER_BPM,
  HEADWATER_API_GET_SAMPLES_PER_MULTIPLIER_A,
  HEADWATER_API_GET_SAMPLES_PER_MULTIPLIER_B,
  HEADWATER_API_GET_COUNT_MULTIPLIER_A,
  HEADWATER_API_GET_COUNT_MULTIPLIER_B,
  HEADWATER_API_GET_SAMPLE_COUNT_BPM,
  HEADWATER_API_GET_SAMPLE_COUNT_MULTIPLIER_A,
  HEADWATER_API_GET_SAMPLE_COUNT_MULTIPLIER_B,
  HEADWATER_API_GET_CHANGE_FLAGS,
  HEADWATER_API_UPDATE_PLAY,
  HEADWATER_API_UPDATE_RESET,
  HEADWATER_API_UPDATE_STOP,
  HEADWATER_API_UPDATE_MODE,
  HEADWATER_API_UPDATE_BPM,
  HEADWATER_API_UPDATE_MULTIPLIER_A,
  HEADWATER_API_UPDATE_MULTIPLIER_B
} HEADWATER_API_CMD;

void headwater_api_payload_preprocessor(API *api);
void headwater_api_payload_postprocessor(API *api);

#endif
