// MIT license, full license at end of file
// Minor mods by Deano, for Wyrd
#pragma once
#ifndef WYRD_SYOYO_TINY_OBJLOADER_H
#define WYRD_SYOYO_TINY_OBJLOADER_H

/* @todo { Remove stddef dependency. size_t? } */
#include <stddef.h>

typedef struct {
  char *name;

  float ambient[3];
  float diffuse[3];
  float specular[3];
  float transmittance[3];
  float emission[3];
  float shininess;
  float ior;      /* index of refraction */
  float dissolve; /* 1 == opaque; 0 == fully transparent */
  /* illumination model (see http://www.fileformat.info/format/material/) */
  int illum;

  int pad0;

  char *ambient_texname;            /* map_Ka */
  char *diffuse_texname;            /* map_Kd */
  char *specular_texname;           /* map_Ks */
  char *specular_highlight_texname; /* map_Ns */
  char *bump_texname;               /* map_bump, bump */
  char *displacement_texname;       /* disp */
  char *alpha_texname;              /* map_d */
} tinyobj_material_t;

typedef struct {
  char *name; /* group name or object name. */
  unsigned int face_offset;
  unsigned int length;
} tinyobj_shape_t;

typedef struct { int v_idx, vt_idx, vn_idx; } tinyobj_vertex_index_t;

typedef struct {
  unsigned int num_vertices;
  unsigned int num_normals;
  unsigned int num_texcoords;
  unsigned int num_faces;
  unsigned int num_face_num_verts;

  int pad0;

  float *vertices;
  float *normals;
  float *texcoords;
  tinyobj_vertex_index_t *faces;
  int *face_num_verts;
  int *material_ids;
} tinyobj_attrib_t;


#define TINYOBJ_FLAG_TRIANGULATE (1 << 0)

#define TINYOBJ_INVALID_INDEX (0x80000000)

#define TINYOBJ_SUCCESS (0)
#define TINYOBJ_ERROR_EMPTY (-1)
#define TINYOBJ_ERROR_INVALID_PARAMETER (-2)
#define TINYOBJ_ERROR_FILE_OPERATION (-3)

/* Parse wavefront .obj(.obj string data is expanded to linear char array `buf')
 * flags are combination of TINYOBJ_FLAG_***
 * Returns TINYOBJ_SUCCESS if things goes well.
 * Returns TINYOBJ_ERR_*** when there is an error.
 */
EXTERN_C int tinyobj_parse_obj(tinyobj_attrib_t *attrib, tinyobj_shape_t **shapes,
                             size_t *num_shapes, tinyobj_material_t **materials,
                             size_t *num_materials, const char *buf, size_t len,
                             unsigned int flags);
EXTERN_C int tinyobj_parse_mtl_file(tinyobj_material_t **materials_out,
                                  size_t *num_materials_out,
                                  const char *filename);

EXTERN_C void tinyobj_attrib_init(tinyobj_attrib_t *attrib);
EXTERN_C void tinyobj_attrib_free(tinyobj_attrib_t *attrib);
EXTERN_C void tinyobj_shapes_free(tinyobj_shape_t *shapes, size_t num_shapes);
EXTERN_C void tinyobj_materials_free(tinyobj_material_t *materials,
                                   size_t num_materials);
#endif //WYRD_TINY_OBJLOADER_H

/*
   The MIT License (MIT)

   Copyright (c) 2016 - 2019 Syoyo Fujita and many contributors.

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
   */
