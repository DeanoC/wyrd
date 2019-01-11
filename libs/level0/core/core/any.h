#pragma once
#ifndef WYRD_ANY_H
#define WYRD_ANY_H

#include "core.h"

#if PLATFORM_OS == OSX
#include "macos/any"
#else
#include <any>
#endif

#endif //WYRD_ANY_H
