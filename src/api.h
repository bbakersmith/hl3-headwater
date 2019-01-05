#ifndef _API_H_
#define _API_H_

#include "stdint.h"

typedef enum {
  API_HEADER_CMD0,
  API_HEADER_CMD1,
  API_HEADER_CMD2,
  API_HEADER_CMD3,
  API_HEADER_CMD4,
  API_HEADER_SIZE0,
  API_HEADER_SIZE1,
  API_HEADER_SIZE2
} API_HEADER;

typedef enum {
  API_CMD_NEW_REQUEST
} API_CMD;

typedef struct APIRequest {
  API_CMD command; // TODO enum?
  uint8_t index;
  uint8_t payload[8];
  uint8_t size;
} APIRequest;

void api_parse_header(APIRequest *request, uint8_t header);
void api_new_payload(APIRequest *request, uint8_t payload[8]);
void api_handle_request(APIRequest *request, uint8_t *serial_register);

APIRequest api_new_request();

#endif

// typedef enum {
//   HEADWATER_API_REQUEST_HEADER_CMD0,
//   HEADWATER_API_REQUEST_HEADER_CMD1,
//   HEADWATER_API_REQUEST_HEADER_CMD2,
//   HEADWATER_API_REQUEST_HEADER_CMD3,
//   HEADWATER_API_REQUEST_HEADER_CMD4,
//   HEADWATER_API_REQUEST_HEADER_TYPE,
//   HEADWATER_API_REQUEST_HEADER_SIZE0,
//   HEADWATER_API_REQUEST_HEADER_SIZE1
// } HEADWATER_API_REQUEST_HEADER;
//
// typedef enum {
//   HEADWATER_API_REQUEST_TYPE_GET,
//   HEADWATER_API_REQUEST_TYPE_UPDATE
// } HEADWATER_API_REQUEST_TYPE;
//
// typedef enum {
//   HEADWATER_API_GET_BPM,
//   HEADWATER_API_GET_MULT_A,
//   HEADWATER_API_GET_MULT_B,
//   HEADWATER_API_GET_SAMPLES_PER_BEAT,
//   HEADWATER_API_GET_SAMPLES_PER_MULT_A,
//   HEADWATER_API_GET_SAMPLES_PER_MULT_B,
//   HEADWATER_API_GET_RUNNING,
//   HEADWATER_API_UPDATE_BPM = 32,
//   HEADWATER_API_UPDATE_MULT_A,
//   HEADWATER_API_UPDATE_MULT_B,
//   HEADWATER_API_UPDATE_STOP,
//   HEADWATER_API_UPDATE_RESET,
//   HEADWATER_API_UPDATE_PLAY,
//   HEADWATER_API_NEW_COMMAND = 63
// } HEADWATER_API_COMMAND;
//
// typedef enum {
//   HEADWATER_API_PAYLOAD_SIZE_NONE,
//   HEADWATER_API_PAYLOAD_SIZE_1BYTE,
//   HEADWATER_API_PAYLOAD_SIZE_2BYTES,
//   HEADWATER_API_PAYLOAD_SIZE_3BYTES,
//   HEADWATER_API_PAYLOAD_SIZE_4BYTES,
// } HEADWATER_API_PAYLOAD_SIZE;
//
// typedef struct HeadwaterAPIRequest {
//   HEADWATER_API_COMMAND      command;
//   uint8_t                    header;
//   HEADWATER_API_PAYLOAD_SIZE index;
//   uint8_t                    payload[4];
//   HEADWATER_API_PAYLOAD_SIZE size;
//   HEADWATER_API_REQUEST_TYPE type;
// } HeadwaterAPIRequest;
//
// typedef struct HeadwaterAPI {
//   HeadwaterAPIRequest *request;
//   uint8_t             *serial_register;
//   HeadwaterAPIState   *state;
//   void (*get)(struct HeadwaterAPI *api);
//   void (*update)(struct HeadwaterAPI *api);
// } HeadwaterAPI;
//
// void headwater_api_handle_interrupt(HeadwaterAPI *api);
//
// #endif
