#include "core/core.h"
#include "image/format.h"
#include "image/format_cracker.h"

static uint8_t s_Image_Swizzle_RGBA[4] = {0, 1, 2, 3};
static uint8_t s_Image_Swizzle_ARGB[4] = {3, 0, 1, 2};
static uint8_t s_Image_Swizzle_BGRA[4] = {2, 1, 0, 3};
static uint8_t s_Image_Swizzle_ABGR[4] = {3, 2, 1, 0};
EXTERN_C Image_Swizzle_t Image_Format_Swizzle_RGBA = s_Image_Swizzle_RGBA;
EXTERN_C Image_Swizzle_t Image_Format_Swizzle_ARGB = s_Image_Swizzle_ARGB;
EXTERN_C Image_Swizzle_t Image_Format_Swizzle_BGRA = s_Image_Swizzle_BGRA;
EXTERN_C Image_Swizzle_t Image_Format_Swizzle_ABGR = s_Image_Swizzle_ABGR;

EXTERN_C char const *Image_Format_Name(enum Image_Format_t const fmt) {
#define IF_START_MACRO switch(fmt) {
#define IF_MOD_MACRO(x) case Image_Format_##x: return #x;
#define IF_END_MACRO };

#include "image/format.h"

  return "Unknown Format";
}
