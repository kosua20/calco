#pragma once

#ifdef _WIN32
#	define NOMINMAX
#endif

#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <glm/gtc/constants.hpp>

typedef unsigned char uchar;
typedef unsigned int  uint;
typedef unsigned long ulong;

#ifdef _WIN32
#	undef near
#	undef far
#	undef ERROR
#endif

#if defined(__APPLE__) && defined(__MACH__)
#	define _MACOS
#endif

#include "system/Logger.hpp"
