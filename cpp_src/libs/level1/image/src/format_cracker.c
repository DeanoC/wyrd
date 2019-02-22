#include "core/core.h"
#include "image/format.h"
#include "image/format_cracker.h"

EXTERN_C char const *Image_Format_Name(enum Image_Format_t const fmt) {
#define IF_START_MACRO switch(fmt) {
#define IF_MOD_MACRO(x) case Image_Format_##x: return #x;
#define IF_END_MACRO };

#include "image/format.h"

  return "Unknown Format";
}
