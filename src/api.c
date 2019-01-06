#include "api.h"

APIRequest api_new_request() {
  APIRequest request = {
    .command = API_CMD_NEW_REQUEST
  };
  return request;
}

void api_parse_header(APIRequest *request, uint8_t header) {
  request->size = header >> 5;
  request->command = header & ~(
   (1 << API_HEADER_SIZE2)
   | (1 << API_HEADER_SIZE1)
   | (1 << API_HEADER_SIZE0)
  );
}

void api_new_payload(APIRequest *request, uint8_t payload[8]) {
  for(uint8_t i = 0; i < request->size; i++) {
    request->payload[i] = payload[i];
  }
  request->index = 0;
}

void api_handle_request(APIRequest *request, uint8_t *serial_register) {
  uint8_t incoming_value = *serial_register;
  *serial_register = request->payload[request->index];
  if(0 < request->index) {
    request->payload[request->index - 1] = incoming_value;
  }
  request->index++;
}

void api_payload_preprocessor(API *api) {
  api->payload_preprocessor(api);
  *api->serial_register = api->request->payload[api->request->index]; // ?
}

void api_handle_interrupt(API *api) {
  if(api->request->command == API_CMD_NEW_REQUEST) {
    api_parse_header(api->request, *api->serial_register);
    api->payload_preprocessor(api);
  }

  api_handle_request(api->request, api->serial_register);

  if(api->request->size < api->request->index) {
    api->payload_postprocessor(api);
    *api->request = api_new_request();
  }
}
