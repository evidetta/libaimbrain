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
  int len = sizeof(char) * (strlen(msg) + 1);
  ctx->error = (char *)malloc(len);
  if(ctx->error != NULL) {
    memset(ctx->error, 0, len);
    strncpy(ctx->error, msg, len);
  }
}
