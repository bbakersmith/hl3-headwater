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

uint8_t api_handle_request(APIRequest *request, uint8_t incoming_value) {
  uint8_t outgoing_value = request->payload[request->index];
  if(0 < request->index) {
    request->payload[request->index - 1] = incoming_value;
  }
  request->index++;
  return outgoing_value;
}

uint8_t api_handle_interrupt(API *api, uint8_t incoming_value) {
  if(api->request->command == API_CMD_NEW_REQUEST) {
    api_parse_header(api->request, incoming_value);
    api->payload_preprocessor(api);
  }

  uint8_t outgoing_value = api_handle_request(api->request, incoming_value);

  if(api->request->size < api->request->index) {
    api->payload_postprocessor(api);
    *api->request = api_new_request();
    outgoing_value = 0;
  }

  return outgoing_value;
}
