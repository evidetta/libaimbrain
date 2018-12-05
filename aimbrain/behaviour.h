#ifndef AIMBRAIN_BEHAVIOUR_H
#define AIMBRAIN_BEHAVIOUR_H

#include "aimbrain.h"

typedef enum AimbrainBehaviouralStatus {
  AIMBRAIN_USER_NOT_ENROLLED,
  AIMBRAIN_USER_ENROLLED
} AimbrainBehaviouralStatus;

typedef struct AimbrainTouches {
  int tid;
  int p;
  int t;
  int x;
  int y;
  int rx;
  int ry;
  float r;
  float f;
  char** ids;
} AimbrainTouches;

typedef struct AimbrainAccelerations {
  int t;
  float x;
  float y;
  float z;
} AimbrainAccelerations;

typedef struct AimbrainTextEvents {
  int t;
  char * const tx;
  char** ids;
} AimbrainTextEvents;

typedef struct AimbrainBehaviourInput {
  char const * session;
  AimbrainTouches** touches;
  AimbrainAccelerations** accelerations;
  AimbrainTextEvents** text_events;
} AimbrainBehaviourInput;

typedef struct AimbrainBehaviourOutput {
  float score;
  AimbrainBehaviouralStatus status;
} AimbrainBehaviourOutput;

AimbrainError Aimbrain_Behaviour(AimbrainContext ctx, AimbrainBehaviourInput input, AimbrainBehaviourOutput** output);
void Aimbrain_DisposeAimbrainBehaviourOutput(AimbrainBehaviourOutput* output);

#endif
