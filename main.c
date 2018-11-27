#include "aimbrain.h"

int main(int argc, char const *argv[]) {
  AimbrainContext* ctx = Aimbrain_Init("test", "secret");

  SessionsInput si;
  si.userID = "elia";
  si.device = "libaimbrain";
  si.system = "libaimbrain";
  si.screenWidth = 200;
  si.screenHeight = 200;

  Aimbrain_Sessions(ctx, si);
  Aimbrain_Dispose(ctx);
  return 0;
}
