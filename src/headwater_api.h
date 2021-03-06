/**
 * @file headwater_api.h
 */

#ifndef _HEADWATER_API_H_
#define _HEADWATER_API_H_

#include "headwater_state.h"

#define APIState HeadwaterState

#include "api.h"

/**
 * Headwater API command codes.
 */
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
} HeadwaterAPICommand;

/**
 * API handler to run before the next transfer event. Generally writing
 * outbound responses to "get" requests.
 */
void headwater_api_payload_preprocessor(API *api);

/**
 * API handler to run after the next transfer event. Generally reading
 * inbound "update" requests.
 */
void headwater_api_payload_postprocessor(API *api);

#endif
