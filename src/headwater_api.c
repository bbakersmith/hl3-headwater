#include "bytes.h"
#include "headwater_api.h"

void headwater_api_16bit_payload(volatile uint8_t payload[8], uint16_t value) {
  payload[0] = bytes_16bit_to_high(value);
  payload[1] = bytes_16bit_to_low(value);
}

void headwater_api_payload_preprocessor(API *api) {
  HEADWATER_API_CMD command = api->request.command;
  switch(command) {
    case HEADWATER_API_GET_OUTPUT_ENABLED:
      api->request.payload[0] = api->state.output_enabled;
      break;
    case HEADWATER_API_GET_MODE:
      api->request.payload[0] = api->state.mode;
      break;
    case HEADWATER_API_GET_BPM:
      headwater_api_16bit_payload(
        api->request.payload,
        api->state.bpm
      );
      break;
    case HEADWATER_API_GET_MULTIPLIER_A:
      api->request.payload[0] = api->state.multiplier_a_channel.multiplier;
      break;
    case HEADWATER_API_GET_MULTIPLIER_B:
      api->request.payload[0] = api->state.multiplier_b_channel.multiplier;
      break;
    case HEADWATER_API_GET_SAMPLES_PER_BPM:
      headwater_api_16bit_payload(
        api->request.payload,
        api->state.bpm_channel.samples_per_beat
      );
      break;
    case HEADWATER_API_GET_SAMPLES_PER_MULTIPLIER_A:
      headwater_api_16bit_payload(
        api->request.payload,
        api->state.multiplier_a_channel.samples_per_beat
      );
      break;
    case HEADWATER_API_GET_SAMPLES_PER_MULTIPLIER_B:
      headwater_api_16bit_payload(
        api->request.payload,
        api->state.multiplier_b_channel.samples_per_beat
      );
      break;
    case HEADWATER_API_GET_COUNT_MULTIPLIER_A:
      headwater_api_16bit_payload(
        api->request.payload,
        api->state.multiplier_a_channel.beats
      );
      break;
    case HEADWATER_API_GET_COUNT_MULTIPLIER_B:
      headwater_api_16bit_payload(
        api->request.payload,
        api->state.multiplier_b_channel.beats
      );
      break;
    case HEADWATER_API_GET_SAMPLE_COUNT_BPM:
      headwater_api_16bit_payload(
        api->request.payload,
        api->state.bpm_channel.samples
      );
      break;
    case HEADWATER_API_GET_SAMPLE_COUNT_MULTIPLIER_A:
      headwater_api_16bit_payload(
        api->request.payload,
        api->state.multiplier_a_channel.samples
      );
      break;
    case HEADWATER_API_GET_SAMPLE_COUNT_MULTIPLIER_B:
      headwater_api_16bit_payload(
        api->request.payload,
        api->state.multiplier_b_channel.samples
      );
      break;
    case HEADWATER_API_GET_CV_BPM:
      api->request.payload[0] = api->state.bpm_channel.modifier;
      break;
    case HEADWATER_API_GET_CV_MULTIPLIER_A:
      api->request.payload[0] = api->state.multiplier_a_channel.modifier;
      break;
    case HEADWATER_API_GET_CV_MULTIPLIER_B:
      api->request.payload[0] = api->state.multiplier_b_channel.modifier;
      break;

    // TODO test
    case HEADWATER_API_GET_CHANGE_FLAGS:
      api->request.payload[0] = api->state.change_flags;
      break;
  }

}

void headwater_api_payload_postprocessor(API *api) {
  HEADWATER_API_CMD command = api->request.command;
  switch(command) {
    case HEADWATER_API_UPDATE_PLAY:
      api->state.change_flags |= (1 << HEADWATER_STATE_CHANGE_PLAY);
      break;
    case HEADWATER_API_UPDATE_RESET:
      api->state.change_flags |= (1 << HEADWATER_STATE_CHANGE_RESET);
      break;
    case HEADWATER_API_UPDATE_STOP:
      api->state.change_flags |= (1 << HEADWATER_STATE_CHANGE_STOP);
      break;
    case HEADWATER_API_UPDATE_MODE:
      api->state.mode = api->request.payload[0];
      api->state.change_flags |= (1 << HEADWATER_STATE_CHANGE_MODE);
      break;
    case HEADWATER_API_UPDATE_BPM:
      api->state.bpm = bytes_high_low_to_16bit(
        api->request.payload[0],
        api->request.payload[1]
      );
      api->state.change_flags |= (1 << HEADWATER_STATE_CHANGE_BPM);
      break;
    case HEADWATER_API_UPDATE_MULTIPLIER_A:
      api->state.multiplier_a_channel.multiplier = api->request.payload[0];
      api->state.change_flags |= (1 << HEADWATER_STATE_CHANGE_MULTIPLIER_A);
      break;
    case HEADWATER_API_UPDATE_MULTIPLIER_B:
      api->state.multiplier_b_channel.multiplier = api->request.payload[0];
      api->state.change_flags |= (1 << HEADWATER_STATE_CHANGE_MULTIPLIER_B);
      break;
  }
}
