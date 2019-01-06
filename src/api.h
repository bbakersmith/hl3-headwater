#ifndef _API_H_
#define _API_H_

#ifdef API_STATE_INCLUDE
#include API_STATE_INCLUDE
#endif

#ifdef API_STATE
#define APIState API_STATE
#endif

#include "stdint.h"

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

typedef struct APIRequest {
  uint8_t command;
  uint8_t index;
  uint8_t payload[8];
  uint8_t size;
} APIRequest;

typedef struct API {
  APIRequest *request;
  void (*payload_preprocessor)(struct API *api);
  void (*payload_postprocessor)(struct API *api);
  uint8_t *serial_register;
#ifdef API_STATE
  APIState *state;
#endif
} API;

void api_parse_header(APIRequest *request, uint8_t header);
void api_new_payload(APIRequest *request, uint8_t payload[8]);
void api_handle_request(APIRequest *request, uint8_t *serial_register);
void api_payload_preprocessor(API *api);
void api_handle_interrupt(API *api);
APIRequest api_new_request();

#endif
