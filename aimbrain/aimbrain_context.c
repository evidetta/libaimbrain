#include "aimbrain.h"

#include <stdlib.h>


AimbrainContext* Aimbrain_Init(char const * api_key, char const * secret) {
  AimbrainContext* ctx = (AimbrainContext *)malloc(sizeof(AimbrainContext));
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
