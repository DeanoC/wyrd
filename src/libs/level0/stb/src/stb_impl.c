#include "core/core.h"

#define STB_DXT_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#ifdef _WIN32
#define STBI_MSC_SECURE_CRT
#endif
#define STB_DEFINE
#define STB_EXTERN EXTERN_C

#include "stb/stb_dxt.h"
#include "stb/stb_image.h"
#include "stb/stb_image_resize.h"
#include "stb/stb_image_write.h"
#include "stb/stb_truetype.h"
#include "stb/stb_hash.h"
#include "stb/stb_dict.h"