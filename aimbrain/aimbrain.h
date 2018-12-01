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
  const char* msg;
} AimbrainError;

//Sessions
typedef struct AimbrainSessionsInput {
  const char* user_id;
  const char* device;
  const char* system;
  int screen_height;
  int screen_width;
} AimbrainSessionsInput;

typedef struct AimbrainSessionsOutput {
  char* session;
  int face;
  int voice;
  int behaviour;
  AimbrainError error;
} AimbrainSessionsOutput;

AimbrainContext* Aimbrain_Init(const char* apiKey, const char* secret);
void Aimbrain_Dispose(AimbrainContext* ctx);

void Aimbrain_Sessions(AimbrainContext* ctx, AimbrainSessionsInput input, AimbrainSessionsOutput* output);

#endif
