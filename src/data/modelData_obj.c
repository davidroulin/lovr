#include "data/modelData.h"
#include <stdio.h>

#define STARTS_WITH(a, b) !strncmp(a, b, strlen(b))

ModelData* lovrModelDataInitObj(ModelData* model, Blob* source, ModelDataIO io) {
  char* data = (char*) source->data;
  size_t length = source->size;
  vec_int_t indices;
  vec_float_t vertices;
  vec_float_t normals;
  vec_float_t uvs;
  vec_init(&indices);
  vec_init(&vertices);
  vec_init(&normals);
  vec_init(&uvs);

  while (length > 0) {
    int lineLength;

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
      int v1, v2, v3;
      int vt1, vt2, vt3;
      int vn1, vn2, vn3;
      if (sscanf(data + 2, "%d %d %d\n%n", &v1, &v2, &v3, &lineLength) == 3) {
        vec_pusharr(&indices, ((int[9]) { v1, -1, -1, v2, -1, -1, v3, -1, -1 }), 9);
      } else if (sscanf(data + 2, "%d//%d %d//%d %d//%d\n%n", &v1, &vn1, &v2, &vn2, &v3, &vn3, &lineLength) == 6) {
        vec_pusharr(&indices, ((int[9]) { v1, vn1, -1, v2, vn2, -1, v3, vn3, -1 }), 9);
      } else if (sscanf(data + 2, "%d/%d/%d %d/%d/%d %d/%d/%d\n%n", &v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3, &lineLength) == 9) {
        vec_pusharr(&indices, ((int[9]) { v1, vn1, vt1, v2, vn2, vt2, v3, vn3, vt3 }), 9);
      } else {
        lovrThrow("Bad OBJ: Unknown face format");
      }
    } else {
      char* newline = memchr(data, '\n', length);
      lineLength = newline - data + 1;
    }

    data += lineLength;
    length -= lineLength;
  }

  vec_deinit(&indices);
  vec_deinit(&vertices);
  vec_deinit(&normals);
  vec_deinit(&uvs);
  return NULL;
}
