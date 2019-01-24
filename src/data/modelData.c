#include "data/modelData.h"

ModelData* lovrModelDataInit(ModelData* model, Blob* source, ModelDataIO io) {
  if (lovrModelDataInitGltf(model, source, io)) {
    return model;
  } else if (lovrModelDataInitObj(model, source, io)) {
    return model;
  }

  lovrThrow("Unable to load model from '%s'", source->name);
  return NULL;
}

void lovrModelDataDestroy(void* ref) {
  ModelData* model = ref;
  for (int i = 0; i < model->blobCount; i++) {
    lovrRelease(model->blobs[i]);
  }
  for (int i = 0; i < model->imageCount; i++) {
    lovrRelease(model->images[i]);
  }
  free(model->blobs);
  free(model->images);
  free(model->animations);
  free(model->attributes);
  free(model->buffers);
  free(model->textures);
  free(model->materials);
  free(model->primitives);
  free(model->nodes);
  free(model->skins);
  free(model->misc);
}
