#include <stdlib.h>
#include <stdio.h>
#include "aimbrain/aimbrain.h"

int main(int argc, char const *argv[]) {
  AimbrainContext* ctx = Aimbrain_Init("test", "secre");
  if(ctx == NULL) {
    goto cleanup_ctx;
  }

  AimbrainSessionsInput input = {"user", "device", "system", 200, 200};
  AimbrainSessionsOutput* output = NULL;

  AimbrainError err = Aimbrain_Sessions(ctx, input, &output);
  if(err.code != AIMBRAIN_NO_ERROR) {
    printf("%d\n", err.code);
    printf("%s\n", ctx->error);
    goto cleanup_sessions;
  }

  printf("%s\n", output->session);
  printf("%d\n", output->face);
  printf("%d\n", output->voice);
  printf("%d\n", output->behaviour);

cleanup_sessions:
  Aimbrain_DisposeAimbrainSessionsOutput(output);
cleanup_ctx:
  Aimbrain_Dispose(ctx);
  return 0;
}
