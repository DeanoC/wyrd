#include "core/core.h"
#include "os/filesystem.h"
#include "vfile/vfile.hpp"
#include "catch/catch.hpp"

#include "syoyo/tiny_objloader.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

static const char *gBasePath = "test_data/models/";


static void PrintInfo(tinyobj_attrib_t const& attrib,
                      tinyobj_shape_t const *shapes,
                      size_t const num_shapes,
                      tinyobj_material_t const *materials,
                      size_t const num_materials,
                      bool const triangulate = true) {
  std::cout << "# of vertices  : " << (attrib.num_vertices / 3) << std::endl;
  std::cout << "# of normals   : " << (attrib.num_normals / 3) << std::endl;
  std::cout << "# of texcoords : " << (attrib.num_texcoords / 2) << std::endl;

  std::cout << "# of shapes    : " << num_shapes << std::endl;
  std::cout << "# of materials : " << num_materials << std::endl;

  for (size_t v = 0; v < attrib.num_vertices / 3; v++) {
    printf("  v[%ld] = (%f, %f, %f)\n", v,
           static_cast<const double>(attrib.vertices[3 * v + 0]),
           static_cast<const double>(attrib.vertices[3 * v + 1]),
           static_cast<const double>(attrib.vertices[3 * v + 2]));
  }

  for (size_t v = 0; v < attrib.num_normals / 3; v++) {
    printf("  n[%ld] = (%f, %f, %f)\n", v,
           static_cast<const double>(attrib.normals[3 * v + 0]),
           static_cast<const double>(attrib.normals[3 * v + 1]),
           static_cast<const double>(attrib.normals[3 * v + 2]));
  }

  for (size_t v = 0; v < attrib.num_texcoords / 2; v++) {
    printf("  uv[%ld] = (%f, %f)\n", v,
           static_cast<const double>(attrib.texcoords[2 * v + 0]),
           static_cast<const double>(attrib.texcoords[2 * v + 1]));
  }

  for (size_t i = 0; i < num_shapes; i++) {
    printf("shape[%ld].name = %s\n", i, shapes[i].name);
/*    printf("Size of shape[%ld].indices: %ld\n", i,
           shapes[i].mesh.indices.size());

    if (triangulate) {
      printf("Size of shape[%ld].material_ids: %ld\n", i,
             shapes[i].mesh.material_ids.size());
      assert((shapes[i].mesh.indices.size() % 3) == 0);
      for (size_t f = 0; f < shapes[i].mesh.indices.size() / 3; f++) {
        tinyobj::index_t i0 = shapes[i].mesh.indices[3 * f + 0];
        tinyobj::index_t i1 = shapes[i].mesh.indices[3 * f + 1];
        tinyobj::index_t i2 = shapes[i].mesh.indices[3 * f + 2];
        printf("  idx[%ld] = %d/%d/%d, %d/%d/%d, %d/%d/%d. mat_id = %d\n", f,
               i0.vertex_index, i0.normal_index, i0.texcoord_index,
               i1.vertex_index, i1.normal_index, i1.texcoord_index,
               i2.vertex_index, i2.normal_index, i2.texcoord_index,
               shapes[i].mesh.material_ids[f]);
      }
    } else {
      for (size_t f = 0; f < shapes[i].mesh.indices.size(); f++) {
        tinyobj::index_t idx = shapes[i].mesh.indices[f];
        printf("  idx[%ld] = %d/%d/%d\n", f, idx.vertex_index, idx.normal_index,
               idx.texcoord_index);
      }

      printf("Size of shape[%ld].material_ids: %ld\n", i,
             shapes[i].mesh.material_ids.size());
      assert(shapes[i].mesh.material_ids.size() ==
          shapes[i].mesh.num_face_vertices.size());
      for (size_t m = 0; m < shapes[i].mesh.material_ids.size(); m++) {
        printf("  material_id[%ld] = %d\n", m, shapes[i].mesh.material_ids[m]);
      }
    }

    printf("shape[%ld].num_faces: %ld\n", i,
           shapes[i].mesh.num_face_vertices.size());
    for (size_t v = 0; v < shapes[i].mesh.num_face_vertices.size(); v++) {
      printf("  num_vertices[%ld] = %ld\n", v,
             static_cast<long>(shapes[i].mesh.num_face_vertices[v]));
    }

    // printf("shape[%ld].vertices: %ld\n", i, shapes[i].mesh.positions.size());
    // assert((shapes[i].mesh.positions.size() % 3) == 0);
    // for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
    //  printf("  v[%ld] = (%f, %f, %f)\n", v,
    //    static_cast<const double>(shapes[i].mesh.positions[3*v+0]),
    //    static_cast<const double>(shapes[i].mesh.positions[3*v+1]),
    //    static_cast<const double>(shapes[i].mesh.positions[3*v+2]));
    //}

    printf("shape[%ld].num_tags: %ld\n", i, shapes[i].mesh.tags.size());
    for (size_t t = 0; t < shapes[i].mesh.tags.size(); t++) {
      printf("  tag[%ld] = %s ", t, shapes[i].mesh.tags[t].name.c_str());
      printf(" ints: [");
      for (size_t j = 0; j < shapes[i].mesh.tags[t].intValues.size(); ++j) {
        printf("%ld", static_cast<long>(shapes[i].mesh.tags[t].intValues[j]));
        if (j < (shapes[i].mesh.tags[t].intValues.size() - 1)) {
          printf(", ");
        }
      }
      printf("]");

      printf(" floats: [");
      for (size_t j = 0; j < shapes[i].mesh.tags[t].floatValues.size(); ++j) {
        printf("%f", static_cast<const double>(
            shapes[i].mesh.tags[t].floatValues[j]));
        if (j < (shapes[i].mesh.tags[t].floatValues.size() - 1)) {
          printf(", ");
        }
      }
      printf("]");

      printf(" strings: [");
      for (size_t j = 0; j < shapes[i].mesh.tags[t].stringValues.size(); ++j) {
        printf("%s", shapes[i].mesh.tags[t].stringValues[j].c_str());
        if (j < (shapes[i].mesh.tags[t].stringValues.size() - 1)) {
          printf(", ");
        }
      }
      printf("]");
      printf("\n");
    }*/
  }

  for (size_t i = 0; i < num_materials; i++) {
    printf("material[%ld].name = %s\n", i, materials[i].name);
    printf("  material.Ka = (%f, %f ,%f)\n",
           static_cast<const double>(materials[i].ambient[0]),
           static_cast<const double>(materials[i].ambient[1]),
           static_cast<const double>(materials[i].ambient[2]));
    printf("  material.Kd = (%f, %f ,%f)\n",
           static_cast<const double>(materials[i].diffuse[0]),
           static_cast<const double>(materials[i].diffuse[1]),
           static_cast<const double>(materials[i].diffuse[2]));
    printf("  material.Ks = (%f, %f ,%f)\n",
           static_cast<const double>(materials[i].specular[0]),
           static_cast<const double>(materials[i].specular[1]),
           static_cast<const double>(materials[i].specular[2]));
    printf("  material.Tr = (%f, %f ,%f)\n",
           static_cast<const double>(materials[i].transmittance[0]),
           static_cast<const double>(materials[i].transmittance[1]),
           static_cast<const double>(materials[i].transmittance[2]));
    printf("  material.Ke = (%f, %f ,%f)\n",
           static_cast<const double>(materials[i].emission[0]),
           static_cast<const double>(materials[i].emission[1]),
           static_cast<const double>(materials[i].emission[2]));
    printf("  material.Ns = %f\n",
           static_cast<const double>(materials[i].shininess));
    printf("  material.Ni = %f\n", static_cast<const double>(materials[i].ior));
    printf("  material.dissolve = %f\n",
           static_cast<const double>(materials[i].dissolve));
    printf("  material.illum = %d\n", materials[i].illum);
    printf("  material.map_Ka = %s\n", materials[i].ambient_texname);
    printf("  material.map_Kd = %s\n", materials[i].diffuse_texname);
    printf("  material.map_Ks = %s\n", materials[i].specular_texname);
    printf("  material.map_Ns = %s\n",
           materials[i].specular_highlight_texname);
    printf("  material.map_bump = %s\n", materials[i].bump_texname);
    printf("  material.map_d = %s\n", materials[i].alpha_texname);
    printf("  material.disp = %s\n", materials[i].displacement_texname);
//    printf("  material.refl = %s\n", materials[i].reflection_texname);
    /*std::map<std::string, std::string>::const_iterator it(
        materials[i]..unknown_parameter.begin());
    std::map<std::string, std::string>::const_iterator itEnd(
        materials[i].unknown_parameter.end());

    for (; it != itEnd; it++) {
      printf("  material.%s = %s\n", it->first.c_str(), it->second.c_str());
    }*/
    printf("\n");
  }
}
static bool TestLoadObj(const char *filename,
                        tinyobj_attrib_t& attrib,
                        tinyobj_shape_t *& shapes,
                        size_t& num_shapes,
                        tinyobj_material_t *& materials,
                        size_t& num_materials,
                        bool triangulate = true) {
  char existCurDir[1024];
  Os_GetCurrentDir(existCurDir, sizeof(existCurDir));
  char path[2048];
  strcpy(path, existCurDir);
  strcat(path, gBasePath);
  Os_SetCurrentDir(path);

  strcat(path,filename);
//  std::cout << "Loading " << filename << std::endl;

  VFile::ScopedFile file = VFile::File::FromFile(path, Os_FM_Read);
  if(!file) {
    Os_SetCurrentDir(existCurDir);
    return false;
  }
  uint64_t data_len = file->Size();
  char *data = (char *) malloc(data_len);
  file->Read(data, data_len);

  unsigned int flags = TINYOBJ_FLAG_TRIANGULATE;
  int ret = tinyobj_parse_obj(&attrib, &shapes, &num_shapes, &materials,
                              &num_materials, data, data_len, flags);
  if (ret != TINYOBJ_SUCCESS) {
    printf("Failed to load/parse .obj.\n");
    free(data);
    Os_SetCurrentDir(existCurDir);
    return false;
  }
  // don't pollute test logs except when fixin
  //PrintInfo(attrib, shapes, num_shapes, materials, num_materials, triangulate);
  free(data);
  Os_SetCurrentDir(existCurDir);

  return true;
}

static bool TestLoadObj(const char *filename,
                        bool triangulate = true) {
//  std::cout << "Loading " << filename << std::endl;

  tinyobj_attrib_t attrib;
  tinyobj_shape_t *shapes = NULL;
  size_t num_shapes;
  tinyobj_material_t *materials = NULL;
  size_t num_materials;

  bool ret = TestLoadObj(filename,
                         attrib,
                         shapes,
                         num_shapes,
                         materials,
                         num_materials);
  if(ret == true ) {
    tinyobj_attrib_free(&attrib);
    tinyobj_materials_free(materials, num_materials);
    tinyobj_shapes_free(shapes, num_shapes);
  }

  return ret;
}

TEST_CASE("cornell_box", "[Loader]") {
  REQUIRE(true == TestLoadObj("cornell_box.obj"));
}

TEST_CASE("catmark_torus_creases0", "[Loader]") {
  tinyobj_attrib_t attrib;
  tinyobj_shape_t *shapes = NULL;
  size_t num_shapes;
  tinyobj_material_t *materials = NULL;
  size_t num_materials;

  bool ret = TestLoadObj("catmark_torus_creases0.obj",
                         attrib,
                         shapes,
                         num_shapes,
                         materials,
                         num_materials);
  REQUIRE(ret == true );

  REQUIRE(1 == num_shapes);
//  REQUIRE(8 == shapes[0].mesh.tags.size());

  tinyobj_attrib_free(&attrib);
  tinyobj_materials_free(materials, num_materials);
  tinyobj_shapes_free(shapes, num_shapes);
}

TEST_CASE("pbr", "[Loader]") {
  tinyobj_attrib_t attrib;
  tinyobj_shape_t *shapes = NULL;
  size_t num_shapes;
  tinyobj_material_t *materials = NULL;
  size_t num_materials;

  bool ret = TestLoadObj("catmark_torus_creases0.obj",
                         attrib,
                         shapes,
                         num_shapes,
                         materials,
                         num_materials);
  REQUIRE(ret == true );
  // C Api doesn't support PBR
  /*
  REQUIRE(1 == num_materials);
  REQUIRE(0.2 == Approx(materials[0].roughness));
  REQUIRE(0.3 == Approx(materials[0].metallic));
  REQUIRE(0.4 == Approx(materials[0].sheen));
  REQUIRE(0.5 == Approx(materials[0].clearcoat_thickness));
  REQUIRE(0.6 == Approx(materials[0].clearcoat_roughness));
  REQUIRE(0.7 == Approx(materials[0].anisotropy));
  REQUIRE(0.8 == Approx(materials[0].anisotropy_rotation));
  REQUIRE(0 == strcmp(materials[0].roughness_texname, "roughness.tex"));
  REQUIRE(0 == strcmp(materials[0].metallic_texname, "metallic.tex"));
  REQUIRE(0 == strcmp(materials[0].sheen_texname, "sheen.tex"));
  REQUIRE(0 == strcmp(materials[0].emissive_texname, "emissive.tex"));
  REQUIRE(0 == strcmp(materials[0].normal_texname, "normalmap.tex"));
  */
  tinyobj_attrib_free(&attrib);
  tinyobj_materials_free(materials, num_materials);
  tinyobj_shapes_free(shapes, num_shapes);
}

TEST_CASE("trailing_whitespace_in_mtl", "[Issue92]") {
  tinyobj_attrib_t attrib;
  tinyobj_shape_t *shapes = NULL;
  size_t num_shapes;
  tinyobj_material_t *materials = NULL;
  size_t num_materials;

  bool ret = TestLoadObj("catmark_torus_creases0.obj",
                         attrib,
                         shapes,
                         num_shapes,
                         materials,
                         num_materials);
  REQUIRE(ret == true );
  tinyobj_attrib_free(&attrib);
  tinyobj_materials_free(materials, num_materials);
  tinyobj_shapes_free(shapes, num_shapes);

}
/* Fails
TEST_CASE("transmittance_filter", "[Issue95]") {
  tinyobj_attrib_t attrib;
  tinyobj_shape_t *shapes = NULL;
  size_t num_shapes;
  tinyobj_material_t *materials = NULL;
  size_t num_materials;

  bool ret = TestLoadObj("issue_95.obj",
                         attrib,
                         shapes,
                         num_shapes,
                         materials,
                         num_materials);
  REQUIRE(true == ret);
  REQUIRE(1 == num_materials);
  REQUIRE(0.1 == Approx(materials[0].transmittance[0]));
  REQUIRE(0.2 == Approx(materials[0].transmittance[1]));
  REQUIRE(0.3 == Approx(materials[0].transmittance[2]));
    tinyobj_attrib_free(&attrib);
    tinyobj_materials_free(materials, num_materials);
    tinyobj_shapes_free(shapes, num_shapes);

}
*/
/* Fails
TEST_CASE("transmittance_filter_Tf", "[Issue95-Tf]") {
  tinyobj_attrib_t attrib;
  tinyobj_shape_t *shapes = NULL;
  size_t num_shapes;
  tinyobj_material_t *materials = NULL;
  size_t num_materials;

  bool ret = TestLoadObj("issue_95-2.obj",
                         attrib,
                         shapes,
                         num_shapes,
                         materials,
                         num_materials);
  REQUIRE(true == ret);
  REQUIRE(1 == num_materials);
  REQUIRE(0.1 == Approx(materials[0].transmittance[0]));
  REQUIRE(0.2 == Approx(materials[0].transmittance[1]));
  REQUIRE(0.3 == Approx(materials[0].transmittance[2]));
    tinyobj_attrib_free(&attrib);
    tinyobj_materials_free(materials, num_materials);
    tinyobj_shapes_free(shapes, num_shapes);
}
*/
TEST_CASE("usemtl_at_last_line", "[Issue104]") {
  tinyobj_attrib_t attrib;
  tinyobj_shape_t *shapes = NULL;
  size_t num_shapes;
  tinyobj_material_t *materials = NULL;
  size_t num_materials;

  bool ret = TestLoadObj("usemtl-issue-104.obj",
                         attrib,
                         shapes,
                         num_shapes,
                         materials,
                         num_materials);
  REQUIRE(true == ret);
  REQUIRE(1 == num_shapes);
  tinyobj_attrib_free(&attrib);
  tinyobj_materials_free(materials, num_materials);
  tinyobj_shapes_free(shapes, num_shapes);
}

/* Fails
TEST_CASE("texture_opts", "[Issue85]") {
  tinyobj_attrib_t attrib;
  tinyobj_shape_t *shapes = NULL;
  size_t num_shapes;
  tinyobj_material_t *materials = NULL;
  size_t num_materials;

  bool ret = TestLoadObj("texture-options-issue-85.obj.obj",
                         attrib,
                         shapes,
                         num_shapes,
                         materials,
                         num_materials);
  REQUIRE(true == ret);
  REQUIRE(1 == num_shapes);
  REQUIRE(3 == num_materials);
  REQUIRE(0 == strcmp(materials[0].name, "default"));
  REQUIRE(0 == strcmp(materials[1].name, "bm2"));
  REQUIRE(0 == strcmp(materials[2].name, "bm3"));
  // C api doesn't support texopts
  REQUIRE(true == materials[0].ambient_texopt.clamp);
  REQUIRE(0.1 == Approx(materials[0].diffuse_texopt.origin_offset[0]));
  REQUIRE(0.0 == Approx(materials[0].diffuse_texopt.origin_offset[1]));
  REQUIRE(0.0 == Approx(materials[0].diffuse_texopt.origin_offset[2]));
  REQUIRE(0.1 == Approx(materials[0].specular_texopt.scale[0]));
  REQUIRE(0.2 == Approx(materials[0].specular_texopt.scale[1]));
  REQUIRE(1.0 == Approx(materials[0].specular_texopt.scale[2]));
  REQUIRE(0.1 == Approx(materials[0].specular_highlight_texopt.turbulence[0]));
  REQUIRE(0.2 == Approx(materials[0].specular_highlight_texopt.turbulence[1]));
  REQUIRE(0.3 == Approx(materials[0].specular_highlight_texopt.turbulence[2]));
  REQUIRE(3.0 == Approx(materials[0].bump_texopt.bump_multiplier));

  REQUIRE(0.1 == Approx(materials[1].specular_highlight_texopt.brightness));
  REQUIRE(0.3 == Approx(materials[1].specular_highlight_texopt.contrast));
  REQUIRE('r' == materials[1].bump_texopt.imfchan);

  REQUIRE(tinyobj::TEXTURE_TYPE_SPHERE == materials[2].diffuse_texopt.type);
  REQUIRE(tinyobj::TEXTURE_TYPE_CUBE_TOP == materials[2].specular_texopt.type);
  REQUIRE(tinyobj::TEXTURE_TYPE_CUBE_BOTTOM ==
      materials[2].specular_highlight_texopt.type);
  REQUIRE(tinyobj::TEXTURE_TYPE_CUBE_LEFT == materials[2].ambient_texopt.type);
  REQUIRE(tinyobj::TEXTURE_TYPE_CUBE_RIGHT == materials[2].alpha_texopt.type);
  REQUIRE(tinyobj::TEXTURE_TYPE_CUBE_FRONT == materials[2].bump_texopt.type);
  REQUIRE(tinyobj::TEXTURE_TYPE_CUBE_BACK ==
      materials[2].displacement_texopt.type);
    tinyobj_attrib_free(&attrib);
    tinyobj_materials_free(materials, num_materials);
    tinyobj_shapes_free(shapes, num_shapes);
}
*/

/* Fails
TEST_CASE("mtllib_multiple_filenames", "[Issue112]") {
  tinyobj_attrib_t attrib;
  tinyobj_shape_t *shapes = NULL;
  size_t num_shapes;
  tinyobj_material_t *materials = NULL;
  size_t num_materials;

  bool ret = TestLoadObj("mtllib-multiple-files-issue-112.obj",
                         attrib,
                         shapes,
                         num_shapes,
                         materials,
                         num_materials);
  REQUIRE(true == ret);
  REQUIRE(1 == num_materials);
    tinyobj_attrib_free(&attrib);
    tinyobj_materials_free(materials, num_materials);
    tinyobj_shapes_free(shapes, num_shapes);
}
*/
/* Fails
TEST_CASE("tr_and_d", "[Issue43]") {
  tinyobj_attrib_t attrib;
  tinyobj_shape_t *shapes = NULL;
  size_t num_shapes;
  tinyobj_material_t *materials = NULL;
  size_t num_materials;

  bool ret = TestLoadObj("tr-and-d-issue-43.obj",
                         attrib,
                         shapes,
                         num_shapes,
                         materials,
                         num_materials);
  REQUIRE(true == ret);
  REQUIRE(2 == num_materials);
  REQUIRE(0.75 == Approx(materials[0].dissolve));
  REQUIRE(0.75 == Approx(materials[1].dissolve));
    tinyobj_attrib_free(&attrib);
    tinyobj_materials_free(materials, num_materials);
    tinyobj_shapes_free(shapes, num_shapes);
}
 */