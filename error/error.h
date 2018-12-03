#ifndef ERROR_H
#define ERROR_H

#include "../aimbrain/aimbrain.h"

AimbrainError GetNewAimbrainError(AimbrainContext* ctx, AimbrainErrorCode code, char* msg);
void SetErrorMessage(AimbrainContext* ctx, char* msg);

#endif
