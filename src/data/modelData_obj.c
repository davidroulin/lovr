#include "data/modelData.h"
#include <stdio.h>

#define STARTS_WITH(a, b) !strncmp(a, b, strlen(b))

ModelData* lovrModelDataInitObj(ModelData* model, Blob* source, ModelDataIO io) {
  char* data = (char*) source->data;
  size_t length = source->size;

  vec_float_t vertexBuffer;
  vec_int_t indexBuffer;
  map_int_t vertexMap;
  vec_float_t vertices;
  vec_float_t normals;
  vec_float_t uvs;

  vec_init(&vertexBuffer);
  vec_init(&indexBuffer);
  map_init(&vertexMap);
  vec_init(&vertices);
  vec_init(&normals);
  vec_init(&uvs);

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
              vec_pusharr(&vertexBuffer, vertices.data + 3 * v, 3);
              vec_pusharr(&vertexBuffer, normals.data + 3 * vn, 3);
              vec_pusharr(&vertexBuffer, uvs.data + 2 * vn, 2);
            } else if (sscanf(s, "%d//%d", &v, &vn) == 2) {
              vec_pusharr(&vertexBuffer, vertices.data + 3 * v, 3);
              vec_pusharr(&vertexBuffer, normals.data + 3 * vn, 3);
              vec_pusharr(&vertexBuffer, ((float[2]) { 0 }), 2);
            } else if (sscanf(s, "%d", &v) == 1) {
              vec_pusharr(&vertexBuffer, vertices.data + 3 * v, 3);
              vec_pusharr(&vertexBuffer, ((float[5]) { 0 }), 5);
            } else {
              lovrThrow("Bad OBJ: Unknown face format");
            }
          }
          *space = terminator;
          s = space + 1;
        }
      }
      lineLength = s - data + 1;
    } else {
      char* newline = memchr(data, '\n', length);
      lineLength = newline - data + 1;
    }

    data += lineLength;
    length -= lineLength;
  }

  model->blobCount = 2;
  model->blobs = calloc(model->blobCount, sizeof(Blob*));
  model->blobs[0] = lovrBlobCreate(vertexBuffer.data, vertexBuffer.capacity, "obj vertex data");
  model->blobs[1] = lovrBlobCreate(indexBuffer.data, indexBuffer.capacity, "obj index data");

  map_deinit(&vertexMap);
  vec_deinit(&vertices);
  vec_deinit(&normals);
  vec_deinit(&uvs);
  return NULL;
}
