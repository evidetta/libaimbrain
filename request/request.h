#ifndef REQUEST_H
#define REQUEST_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/buffer.h>

#include "../aimbrain/aimbrain.h"

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
