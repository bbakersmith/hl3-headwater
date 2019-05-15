/**
 * @file api.h
 *
 * Framework for creating a serial API.
 */

#ifndef _API_H_
#define _API_H_

#include "stdint.h"

#ifndef APIState
#include "dummy_struct.h"
#define APIState DummyStruct
#endif

#define API_CMD_NEW_REQUEST 31

/**
 * API header bit identifiers.
 */
typedef enum {
  API_HEADER_CMD0,
  API_HEADER_CMD1,
  API_HEADER_CMD2,
  API_HEADER_CMD3,
  API_HEADER_CMD4,
  API_HEADER_SIZE0,
  API_HEADER_SIZE1,
  API_HEADER_SIZE2
} APIHeader;

/**
 * API request header metadata and data payload buffer.
 */
typedef volatile struct APIRequest {
  uint8_t command;
  uint8_t index;
  uint8_t payload[8];
  uint8_t size;
} APIRequest;

/**
 * All necessary functions and metadata for processing the latest API request.
 */
typedef volatile struct API {
  APIRequest request;
  void (*payload_preprocessor)(volatile struct API *api);
  void (*payload_postprocessor)(volatile struct API *api);
  APIState *state;
} API;

/**
 * Set APIRequest command and size from incoming header.
 */
void api_parse_header(APIRequest *request, uint8_t header);

/**
 * Copy payload to APIRequest based on predefined size of request.
 */
void api_new_payload(APIRequest *request, uint8_t payload[8]);

/**
 * Read / write payload value for a request in progress.
 */
uint8_t api_handle_request(APIRequest *request, uint8_t incoming_value);

/**
 * Handle interrupt indicating data successfully read and / or written.
 */
uint8_t api_handle_interrupt(API *api, uint8_t incoming_value);

/**
 * Create new APIRequest.
 */
APIRequest api_request_new(void);

/**
 * Reset APIRequest to initial value to await new request.
 */
void api_request_reset(APIRequest *request);

#endif
