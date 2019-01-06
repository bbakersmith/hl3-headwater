#include "bytes.h"
#include "headwater_api.h"

void headwater_api_16bit_payload(uint8_t payload[8], uint16_t value) {
  payload[0] = bytes_16bit_to_high(value);
  payload[1] = bytes_16bit_to_low(value);
}

void headwater_api_payload_preprocessor(API *api) {
  HEADWATER_API_CMD command = api->request->command;
  switch(command) {
    case HEADWATER_API_GET_BPM:
      api->request->payload[0] = 0x04;
      api->request->payload[1] = 0xD2;
      break;
    case HEADWATER_API_GET_MULTIPLIER_A:
      api->request->payload[0] = api->state->multiplier_a;
      break;
    case HEADWATER_API_GET_MULTIPLIER_B:
      api->request->payload[0] = api->state->multiplier_b;
      break;
    case HEADWATER_API_GET_SAMPLES_PER_BEAT:
      headwater_api_16bit_payload(
        api->request->payload,
        api->state->samples_per_beat
      );
      break;
    case HEADWATER_API_GET_SAMPLES_PER_MULTIPLIER_A:
      headwater_api_16bit_payload(
        api->request->payload,
        api->state->samples_per_multiplier_a
      );
      break;
    case HEADWATER_API_GET_SAMPLES_PER_MULTIPLIER_B:
      headwater_api_16bit_payload(
        api->request->payload,
        api->state->samples_per_multiplier_b
      );
      break;
  }

}

void headwater_api_payload_postprocessor(API *api) {

}
