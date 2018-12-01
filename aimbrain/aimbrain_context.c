#include <stdlib.h>

#include "aimbrain.h"


AimbrainContext* Aimbrain_Init(const char* api_key, const char* secret) {
  AimbrainContext* ctx = malloc(sizeof(AimbrainContext));

  if(ctx == NULL) {
    return NULL;
  }

  ctx->api_key = api_key;
  ctx->secret = secret;
  return ctx;
}

void Aimbrain_Dispose(AimbrainContext* ctx) {
  free(ctx);
}
