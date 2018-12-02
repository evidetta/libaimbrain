#ifndef REQUEST_H
#define REQUEST_H

#include <cjson/cJSON.h>
#include "../aimbrain/aimbrain.h"

#define SHA256_BLOCK_SIZE 64

typedef struct Request {
  char* host;
  char* endpoint;
  cJSON* body;
} Request;

typedef struct Response {
  long status_code;
  cJSON* body;
  AimbrainError error;
} Response;

Response MakeRequest(AimbrainContext* ctx, Request request);

#endif
