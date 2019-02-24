#ifndef _API_H_
#define _API_H_

#include "stdint.h"

#ifdef API_STATE
#define APIState API_STATE
#else
typedef struct APIState {} APIState;
#endif

#define API_CMD_NEW_REQUEST 31

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

typedef volatile struct APIRequest {
  uint8_t command;
  uint8_t index;
  uint8_t payload[8];
  uint8_t size;
} APIRequest;

typedef volatile struct API {
  APIRequest request;
  void (*payload_preprocessor)(volatile struct API *api);
  void (*payload_postprocessor)(volatile struct API *api);
  APIState state;
} API;

void api_parse_header(APIRequest *request, uint8_t header);
void api_new_payload(APIRequest *request, uint8_t payload[8]);
uint8_t api_handle_request(APIRequest *request, uint8_t incoming_value);
uint8_t api_handle_interrupt(API *api, uint8_t incoming_value);
APIRequest api_new_request();
void api_request_reset(APIRequest *request);

#endif
