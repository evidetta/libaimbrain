#include "error.h"

#include <stdlib.h>
#include <string.h>

AimbrainError GetNewAimbrainError(AimbrainContext* ctx, AimbrainErrorCode code, char* msg) {
  SetErrorMessage(ctx, msg);

  AimbrainError err = {code, msg};
  return err;
}

void SetErrorMessage(AimbrainContext* ctx, char* msg) {
  free(ctx->error);
  ctx->error = (char *)malloc(sizeof(msg));
  if(ctx->error != NULL) {
    printf("0x%x\n", ctx->error);
    strncpy(ctx->error, msg, strlen(msg));
  }
}
