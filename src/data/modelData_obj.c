#include "data/modelData.h"
#include "filesystem/filesystem.h"
#include "lib/math.h"
#include "lib/map/map.h"
#include "lib/vec/vec.h"
#include <stdio.h>
#include <ctype.h>

typedef vec_t(ModelMaterial) vec_material_t;

#define STARTS_WITH(a, b) !strncmp(a, b, strlen(b))

static void parseMtl(char* path, vec_material_t* materials, char* base) {
  size_t length = 0;
  char* data = lovrFilesystemRead(path, &length);
  lovrAssert(data && length > 0, "Unable to read mtl from '%s'", path);
  char* s = data;

  while (length > 0) {
    int lineLength = 0;

    if (STARTS_WITH(s, "newmtl ")) {
      char name[128];
      bool hasName = sscanf(s + 7, "%s\n%n", name, &lineLength);
      lovrAssert(hasName, "Bad OBJ: Expected a material name");
      vec_push(materials, ((ModelMaterial) {
        .scalars[SCALAR_METALNESS] = 1.f,
        .scalars[SCALAR_ROUGHNESS] = 1.f,
        .colors[COLOR_DIFFUSE] = { 1.f, 1.f, 1.f, 1.f },
        .colors[COLOR_EMISSIVE] = { 0.f, 0.f, 0.f, 0.f }
      }));
      memset(&vec_last(materials).textures, 0xff, MAX_MATERIAL_TEXTURES * sizeof(int));
    } else if (STARTS_WITH(s, "map_Kd")) {
      char filename[128];
      bool hasFilename = sscanf(s + 7, "%s\n%n", filename, &lineLength);
      lovrAssert(hasFilename, "Bad OBJ: Expected a texture filename");
      char path[1024];
      snprintf(path, 1023, "%s/%s", base, filename);
      size_t size = 0;
      void* data = lovrFilesystemRead(path, &size);
      lovrAssert(data && size > 0, "Unable to read texture from %s", path);
      Blob* blob = lovrBlobCreate(data, size, NULL);
      TextureData* image = lovrTextureDataCreateFromBlob(blob, false);
      // TODO what do I do with the image?  ew image + texture + material
      lovrRelease(blob);
    } else {
      char* newline = memchr(s, '\n', length);
      lineLength = newline - s + 1;
    }

    s += lineLength;
    length -= lineLength;
    while (length && isspace(*s)) length--, s++;
  }

  free(data);
}

ModelData* lovrModelDataInitObj(ModelData* model, Blob* source) {
  char* data = (char*) source->data;
  size_t length = source->size;

  vec_material_t materials;
  map_int_t materialNames;
  vec_float_t vertexBuffer;
  vec_int_t indexBuffer;
  map_int_t vertexMap;
  vec_float_t vertices;
  vec_float_t normals;
  vec_float_t uvs;

  vec_init(&materials);
  map_init(&materialNames);
  vec_init(&vertexBuffer);
  vec_init(&indexBuffer);
  map_init(&vertexMap);
  vec_init(&vertices);
  vec_init(&normals);
  vec_init(&uvs);

  char base[1024];
  strncpy(base, source->name, 1023);
  char* slash = strrchr(base, '/');
  if (slash) *slash = 0;

  while (length > 0) {
    int lineLength = 0;

    if (STARTS_WITH(data, "v ")) {
      float x, y, z;
      int count = sscanf(data + 2, "%f %f %f\n%n", &x, &y, &z, &lineLength);
      lovrAssert(count == 3, "Bad OBJ: Expected 3 coordinates for vertex position");
      vec_pusharr(&vertices, ((float[3]) { x, y, z }), 3);
    } else if (STARTS_WITH(data, "vn ")) {
      float x, y, z;
      int count = sscanf(data + 3, "%f %f %f\n%n", &x, &y, &z, &lineLength);
      lovrAssert(count == 3, "Bad OBJ: Expected 3 coordinates for vertex normal");
      vec_pusharr(&normals, ((float[3]) { x, y, z }), 3);
    } else if (STARTS_WITH(data, "vt ")) {
      float u, v;
      int count = sscanf(data + 3, "%f %f\n%n", &u, &v, &lineLength);
      lovrAssert(count == 2, "Bad OBJ: Expected 2 coordinates for texture coordinate");
      vec_pusharr(&uvs, ((float[2]) { u, v }), 2);
    } else if (STARTS_WITH(data, "f ")) {
      char* s = data + 2;
      for (int i = 0; i < 3; i++) {
        char terminator = i == 2 ? '\n' : ' ';
        char* space = strchr(s, terminator);
        if (space) {
          *space = '\0'; // I'll be back
          int* index = map_get(&vertexMap, s);
          if (index) {
            vec_push(&indexBuffer, *index);
          } else {
            int v, vt, vn;
            int newIndex = vertexBuffer.length / 8;
            vec_push(&indexBuffer, newIndex);
            map_set(&vertexMap, s, newIndex);

            // Can be improved
            if (sscanf(s, "%d/%d/%d", &v, &vt, &vn) == 3) {
              vec_pusharr(&vertexBuffer, vertices.data + 3 * (v - 1), 3);
              vec_pusharr(&vertexBuffer, normals.data + 3 * (vn - 1), 3);
              vec_pusharr(&vertexBuffer, uvs.data + 2 * (vt - 1), 2);
            } else if (sscanf(s, "%d//%d", &v, &vn) == 2) {
              vec_pusharr(&vertexBuffer, vertices.data + 3 * (v - 1), 3);
              vec_pusharr(&vertexBuffer, normals.data + 3 * (vn - 1), 3);
              vec_pusharr(&vertexBuffer, ((float[2]) { 0 }), 2);
            } else if (sscanf(s, "%d", &v) == 1) {
              vec_pusharr(&vertexBuffer, vertices.data + 3 * (v - 1), 3);
              vec_pusharr(&vertexBuffer, ((float[5]) { 0 }), 5);
            } else {
              lovrThrow("Bad OBJ: Unknown face format");
            }
          }
          *space = terminator;
          s = space + 1;
        }
      }
      lineLength = s - data;
    } else if (STARTS_WITH(data, "mtllib ")) {
      char filename[1024];
      bool hasName = sscanf(data + 7, "%1024s\n%n", filename, &lineLength);
      lovrAssert(hasName, "Bad OBJ: Expected filename after mtllib");
      char path[1024];
      snprintf(path, 1023, "%s/%s", base, filename);
      parseMtl(path, &materials, base);
    } else {
      char* newline = memchr(data, '\n', length);
      lineLength = newline - data + 1;
    }

    data += lineLength;
    length -= lineLength;
    while (length && isspace(*data)) length--, data++;
  }

  model->blobCount = 2;
  model->bufferCount = 2;
  model->attributeCount = 4;
  model->primitiveCount = 1;
  model->nodeCount = 1;
  lovrModelDataAllocate(model);

  model->blobs[0] = lovrBlobCreate(vertexBuffer.data, vertexBuffer.length * sizeof(float), "obj vertex data");
  model->blobs[1] = lovrBlobCreate(indexBuffer.data, indexBuffer.length * sizeof(int), "obj index data");

  model->buffers[0] = (ModelBuffer) {
    .data = model->blobs[0]->data,
    .size = model->blobs[0]->size,
    .stride = 8 * sizeof(float)
  };

  model->buffers[1] = (ModelBuffer) {
    .data = model->blobs[1]->data,
    .size = model->blobs[1]->size,
    .stride = sizeof(int)
  };

  model->attributes[0] = (ModelAttribute) {
    .buffer = 0,
    .offset = 0,
    .count = vertexBuffer.length / 8,
    .type = F32,
    .components = 3
  };

  model->attributes[1] = (ModelAttribute) {
    .buffer = 0,
    .offset = 3 * sizeof(float),
    .count = vertexBuffer.length / 8,
    .type = F32,
    .components = 3
  };

  model->attributes[2] = (ModelAttribute) {
    .buffer = 0,
    .offset = 6 * sizeof(float),
    .count = vertexBuffer.length / 8,
    .type = F32,
    .components = 2
  };

  model->attributes[3] = (ModelAttribute) {
    .buffer = 1,
    .offset = 0,
    .count = indexBuffer.length,
    .type = U32,
    .components = 1
  };

  model->primitives[0] = (ModelPrimitive) {
    .mode = DRAW_TRIANGLES,
    .attributes = {
      [ATTR_POSITION] = &model->attributes[0],
      [ATTR_NORMAL] = &model->attributes[1],
      [ATTR_TEXCOORD] = &model->attributes[2]
    },
    .indices = &model->attributes[3],
    .material = -1
  };

  model->nodes[0] = (ModelNode) {
    .transform = MAT4_IDENTITY,
    .primitiveIndex = 0,
    .primitiveCount = 1
  };

  vec_deinit(&materials);
  map_deinit(&materialNames);
  map_deinit(&vertexMap);
  vec_deinit(&vertices);
  vec_deinit(&normals);
  vec_deinit(&uvs);
  return model;
}
