#include <cjson/cJSON.h>
#include <string.h>

#include "../request/request.h"
#include "aimbrain.h"


void Aimbrain_Sessions(AimbrainContext* ctx, AimbrainSessionsInput input, AimbrainSessionsOutput* output) {

  cJSON* obj = cJSON_CreateObject();
  if(obj == NULL) {
    AimbrainError err = {AIMBRAIN_MEMORY_ERROR, "Memory allocation for obj failed."};
    output->error = err;
    goto cleanup_obj;
  }

  cJSON* user_id = cJSON_CreateString(input.user_id);
  if(user_id == NULL) {
    AimbrainError err = {AIMBRAIN_MEMORY_ERROR, "Memory allocation for user_id failed."};
    output->error = err;
    goto cleanup_obj;
  }

  cJSON_AddItemToObject(obj, "userID", user_id);

  cJSON* device = cJSON_CreateString(input.device);
  if(device == NULL) {
    AimbrainError err = {AIMBRAIN_MEMORY_ERROR, "Memory allocation for device failed."};
    output->error = err;
    goto cleanup_obj;
  }

  cJSON_AddItemToObject(obj, "device", device);

  cJSON* system = cJSON_CreateString(input.system);
  if(system == NULL) {
    AimbrainError err = {AIMBRAIN_MEMORY_ERROR, "Memory allocation for system failed."};
    output->error = err;
    goto cleanup_obj;
  }

  cJSON_AddItemToObject(obj, "system", system);

  cJSON* screen_height = cJSON_CreateNumber(input.screen_height);
  if(screen_height == NULL) {
    AimbrainError err = {AIMBRAIN_MEMORY_ERROR, "Memory allocation for screen_height failed."};
    output->error = err;
    goto cleanup_obj;
  }

  cJSON_AddItemToObject(obj, "screenHeight", screen_height);

  cJSON* screen_width = cJSON_CreateNumber(input.screen_width);
  if(screen_width == NULL) {
    AimbrainError err = {AIMBRAIN_MEMORY_ERROR, "Memory allocation for screen_width failed."};
    output->error = err;
    goto cleanup_obj;
  }

  cJSON_AddItemToObject(obj, "screenWidth", screen_width);

  Request request = {AIMBRAIN_HOST, "/v1/sessions", obj};
  Response response = MakeRequest(ctx, request);

  if(response.error.code != AIMBRAIN_NO_ERROR) {
    output->error = response.error;
  }

  cJSON* error = cJSON_GetObjectItemCaseSensitive(response.body, "error");
  if (cJSON_IsString(error) && (error->valuestring != NULL)) {
    AimbrainError err = {AIMBRAIN_SERVICE_ERROR, error->valuestring};
    output->error = err;
    goto service_error;
  }

  cJSON* session = cJSON_GetObjectItemCaseSensitive(response.body, "session");
  if(cJSON_IsString(session) && (session->valuestring != NULL)) {
    strncpy(output->session, session->valuestring, strlen(session->valuestring));
  }

  cJSON* face = cJSON_GetObjectItemCaseSensitive(response.body, "face");
  if(cJSON_IsNumber(face)) {
    output->face = face->valueint;
  }

  cJSON* voice = cJSON_GetObjectItemCaseSensitive(response.body, "voice");
  if(cJSON_IsNumber(voice)) {
    output->voice = voice->valueint;
  }

  cJSON* behaviour = cJSON_GetObjectItemCaseSensitive(response.body, "behaviour");
  if(cJSON_IsNumber(behaviour)) {
    output->behaviour = behaviour->valueint;
  }

service_error:
  cJSON_Delete(response.body);
cleanup_obj:
  cJSON_Delete(obj);
}
