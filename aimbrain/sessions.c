#include "aimbrain.h"

#include <cjson/cJSON.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "../request/request.h"


AimbrainError Aimbrain_Sessions(AimbrainContext* ctx, AimbrainSessionsInput input, AimbrainSessionsOutput** output) {
  AimbrainError err = {AIMBRAIN_NO_ERROR, ""};

  cJSON* obj = cJSON_CreateObject();
  if(obj == NULL) {
    err.code = AIMBRAIN_MEMORY_ERROR;
    err.msg = "Memory allocation for obj failed.";
    goto cleanup_obj;
  }

  cJSON* user_id = cJSON_CreateString(input.user_id);
  if(user_id == NULL) {
    err.code = AIMBRAIN_MEMORY_ERROR;
    err.msg = "Memory allocation for user_id failed.";
    goto cleanup_obj;
  }

  cJSON_AddItemToObject(obj, "userID", user_id);

  cJSON* device = cJSON_CreateString(input.device);
  if(device == NULL) {  
    err.code = AIMBRAIN_MEMORY_ERROR;
    err.msg = "Memory allocation for device failed.";
    goto cleanup_obj;
  }

  cJSON_AddItemToObject(obj, "device", device);

  cJSON* system = cJSON_CreateString(input.system);
  if(system == NULL) {
    err.code = AIMBRAIN_MEMORY_ERROR;
    err.msg = "Memory allocation for system failed.";
    goto cleanup_obj;
  }

  cJSON_AddItemToObject(obj, "system", system);

  cJSON* screen_height = cJSON_CreateNumber(input.screen_height);
  if(screen_height == NULL) {
    err.code = AIMBRAIN_MEMORY_ERROR;
    err.msg = "Memory allocation for screen_height failed.";
    goto cleanup_obj;
  }

  cJSON_AddItemToObject(obj, "screenHeight", screen_height);

  cJSON* screen_width = cJSON_CreateNumber(input.screen_width);
  if(screen_width == NULL) {
    err.code = AIMBRAIN_MEMORY_ERROR;
    err.msg = "Memory allocation for screen_width failed.";
    goto cleanup_obj;
  }

  cJSON_AddItemToObject(obj, "screenWidth", screen_width);

  Request request = {AIMBRAIN_HOST, "/v1/sessions", obj};
  Response response = MakeRequest(ctx, request);

  if(response.error.code != AIMBRAIN_NO_ERROR) {
    err = response.error;
    goto internal_error;
  }

  //
  cJSON* error = cJSON_GetObjectItemCaseSensitive(response.body, "error");
  if (cJSON_IsString(error) && (error->valuestring != NULL)) {
    err.code = AIMBRAIN_SERVICE_ERROR;
    err.msg = error->valuestring;
    goto service_error;
  }

  //Allocate new memory off the heap for AimbrainSessionsOutput
  *output = (AimbrainSessionsOutput *)malloc(sizeof(AimbrainSessionsOutput));
  if(*output == NULL) {
    err.code = AIMBRAIN_MEMORY_ERROR;
    err.msg = "Memory allocation for output failed.";
    goto cleanup_output;
  }

  cJSON* session = cJSON_GetObjectItemCaseSensitive(response.body, "session");
  if(cJSON_IsString(session) && (session->valuestring != NULL)) {
    (*output)->session = (char *)malloc(strlen(session->valuestring));
    if((*output)->session == NULL) {
      free(*output);
      err.code = AIMBRAIN_MEMORY_ERROR;
      err.msg = "Memory allocation for output.session failed.";
      goto cleanup_output_session;
    }
    strncpy(((*output)->session), session->valuestring, strlen(session->valuestring));
  }

  cJSON* face = cJSON_GetObjectItemCaseSensitive(response.body, "face");
  if(cJSON_IsNumber(face)) {
    (*output)->face = face->valueint;
  }

  cJSON* voice = cJSON_GetObjectItemCaseSensitive(response.body, "voice");
  if(cJSON_IsNumber(voice)) {
    (*output)->voice = voice->valueint;
  }

  cJSON* behaviour = cJSON_GetObjectItemCaseSensitive(response.body, "behaviour");
  if(cJSON_IsNumber(behaviour)) {
    (*output)->behaviour = behaviour->valueint;
  }

cleanup_output_session:
cleanup_output:
service_error:
  cJSON_Delete(response.body);
internal_error:
cleanup_obj:
  cJSON_Delete(obj);

  return err;
}

void Aimbrain_DisposeAimbrainSessionsOutput(AimbrainSessionsOutput* output) {
  if(output != NULL) {
    free(output->session);
    free(output);
  }
}
