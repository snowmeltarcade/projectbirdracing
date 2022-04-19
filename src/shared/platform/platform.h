#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define PLATFORM_WINDOWS
#elif __APPLE__
#include <TargetConditionals.h>
#if TARGET_OS_SIMULATOR == 1 || TARGET_OS_IOS == 1
#define PLATFORM_IOS
#elif defined(TARGET_OS_OSX)
#define PLATFORM_MAC
#endif
#elif defined(__linux__) || defined(__unix__)
#define PLATFORM_LINUX
#endif
