#pragma once
#ifndef WYRD_THEFORGE_UTILS_HPP
#define WYRD_THEFORGE_UTILS_HPP

#include "core/core.h"
#include "theforge/image_enums.hpp"

namespace TheForge {

void AllocatorUint32ToStr(char *outStr, size_t strLen, uint32_t num);
void AllocatorUint64ToStr(char *outStr, size_t strLen, uint64_t num);

} // end namespace

#endif //WYRD_THEFORGE_UTILS_HPP
