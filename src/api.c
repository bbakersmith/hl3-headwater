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

void api_new_payload(APIRequest *request, uint8_t payload[8], uint8_t size) {
  for(uint8_t i = 0; i < size; i++) {
    request->payload[i] = payload[i];
  }
  request->size = size;
  request->index = 0;
}

/* HeadwaterAPIRequest headwater_api_create_request(uint8_t header) { */
/*   HEADWATER_API_COMMAND command = header & ~( */
/*    (1 << HEADWATER_API_REQUEST_HEADER_SIZE0) */
/*     | (1 << HEADWATER_API_REQUEST_HEADER_SIZE1) */
/*   ); */
/*  */
/*   HEADWATER_API_PAYLOAD_SIZE size = ( */
/*     (1 << HEADWATER_API_REQUEST_HEADER_SIZE0) */
/*     | (1 << HEADWATER_API_REQUEST_HEADER_SIZE1) */
/*   ); */
/*  */
/*   HEADWATER_API_REQUEST_TYPE type = ( */
/*     (1 << HEADWATER_API_REQUEST_HEADER_TYPE) & header */
/*   ); */
/*  */
/*   HeadwaterAPIRequest req = { */
/*     .command = command, */
/*     .header = header, */
/*     .index = 0, */
/*     .type = type, */
/*     .size = size */
/*   }; */
/*  */
/*   return req; */
/* } */
/*  */
/* void headwater_api_handle_interrupt(HeadwaterAPI *api) { */
/*   if(api->request->command == HEADWATER_API_NEW_COMMAND) { */
/*     uint8_t header = *api->serial_register; */
/*     HeadwaterAPIRequest req = headwater_api_create_request(header); */
/*     api->request = &req; */
/*  */
/*     if(api->request->type == HEADWATER_API_REQUEST_TYPE_GET) { */
/*       // populate payload */
/*       api->get(api); */
/*     } */
/*   } */
/*  */
/*   if(api->request->index < api->request->size) { */
/*     *api->serial_register = api->request->payload[api->request->index]; */
/*     api->request->index++; */
/*   } */
/*  */
/*   #<{(| if(api.request.size == 0) { |)}># */
/*   #<{(|   api.postprocessor(api); |)}># */
/*   #<{(|   api->request->command = HEADWATER_API_NEW_COMMAND; |)}># */
/*   #<{(| } |)}># */
/*  */
/*   #<{(| switch(api.request.type) { |)}># */
/*   #<{(|   case HEADWATER_API_REQUEST_TYPE_GET: |)}># */
/*   #<{(|     if(api.request.index < api.request.size) { |)}># */
/*   #<{(|       api->serial_register = api.request.payload[api.request.index]; |)}># */
/*   #<{(|       api->request->index++; |)}># */
/*   #<{(|     } else { |)}># */
/*   #<{(|       api->request->command = HEADWATER_API_NEW_COMMAND; |)}># */
/*   #<{(|     } |)}># */
/*   #<{(|     break; |)}># */
/*   #<{(|   case HEADWATER_API_REQUEST_TYPE_UPDATE: |)}># */
/*   #<{(|     if(api.request.size == 0) { |)}># */
/*   #<{(|       api.update_handler_fn(api); |)}># */
/*   #<{(|       api->request->command = HEADWATER_API_NEW_COMMAND; |)}># */
/*   #<{(|     } else { |)}># */
/*   #<{(|       api->request->payload[api.request.index - 1] = api.serial_register; |)}># */
/*   #<{(|     } |)}># */
/*   #<{(|  |)}># */
/*   #<{(|     // if size 0 call handler |)}># */
/*   #<{(|     // if size 1 index 0 do nothing |)}># */
/*   #<{(|     // if size 1 index 1 copy it |)}># */
/*   #<{(|     // if size 2 index 1 copy it |)}># */
/*   #<{(|     // if size 2 index 2 copy it |)}># */
/*   #<{(|     if(api.request.index < api.request.size) { |)}># */
/*   #<{(|       if(0 < api.request.index) { |)}># */
/*   #<{(|         api->request->payload[api.request.index - 1] = api.serial_register; |)}># */
/*   #<{(|       } |)}># */
/*   #<{(|       api->request->index++; |)}># */
/*   #<{(|     } else { |)}># */
/*   #<{(|       api.update_handler_fn(api); |)}># */
/*   #<{(|     } |)}># */
/*   #<{(|     break; |)}># */
/*   #<{(| } |)}># */
/*  */
/* } */
