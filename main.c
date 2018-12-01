#include <stdlib.h>
#include <stdio.h>
#include "aimbrain/aimbrain.h"

int main(int argc, char const *argv[]) {
  AimbrainContext* ctx = Aimbrain_Init("test", "secret");
  if(ctx == NULL) {
    goto cleanup_ctx;
  }

  AimbrainSessionsInput input = {"user", "device", "system", 200, 200};
  AimbrainSessionsOutput output;
  char buffer[100];
  output.session = buffer;

  Aimbrain_Sessions(ctx, input, &output);

  printf("%s\n", output.session);
  printf("%d\n", output.face);
  printf("%d\n", output.voice);
  printf("%d\n", output.behaviour);

cleanup_ctx:
  Aimbrain_Dispose(ctx);
  return 0;
}
