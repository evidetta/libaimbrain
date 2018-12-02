#ifndef AIMBRAIN_H
#define AIMBRAIN_H

//
#define AIMBRAIN_HOST "https://api.aimbrain.com"

//General Structs
typedef struct AimbrainContext {
  const char* api_key;
  const char* secret;
} AimbrainContext;

typedef enum AimbrainErrorCode {
  AIMBRAIN_NO_ERROR,
  AIMBRAIN_MEMORY_ERROR,
  AIMBRAIN_NETWORK_ERROR,
  AIMBRAIN_SERVICE_ERROR
} AimbrainErrorCode;

typedef struct AimbrainError {
  AimbrainErrorCode code;
  char const * msg;
} AimbrainError;

//Sessions
typedef struct AimbrainSessionsInput {
  char const * user_id;
  char const * device;
  char const * system;
  int screen_height;
  int screen_width;
} AimbrainSessionsInput;

typedef struct AimbrainSessionsOutput {
  char* session;
  int face;
  int voice;
  int behaviour;
} AimbrainSessionsOutput;

AimbrainContext* Aimbrain_Init(char const * apiKey, char const * secret);
void Aimbrain_Dispose(AimbrainContext* ctx);

AimbrainError Aimbrain_Sessions(AimbrainContext* ctx, AimbrainSessionsInput input, AimbrainSessionsOutput** output);
void Aimbrain_DisposeAimbrainSessionsOutput(AimbrainSessionsOutput* output);

#endif
