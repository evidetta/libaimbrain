#include "aimbrain.h"

#include <stdlib.h>


AimbrainContext* Aimbrain_Init(char const * api_key, char const * secret) {
  AimbrainContext* ctx = (AimbrainContext *)malloc(sizeof(AimbrainContext));
  if(ctx == NULL) {
    return NULL;
  }

  ctx->api_key = api_key;
  ctx->secret = secret;
  ctx->error = NULL;
  return ctx;
}

void Aimbrain_Dispose(AimbrainContext* ctx) {
  if(ctx != NULL) {
    if(ctx->error != NULL) {
      printf("0x%x\n", ctx->error);
      free(ctx->error);
    }
    free(ctx);
  }
}
