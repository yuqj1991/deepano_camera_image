#ifndef __LOGD_H__
#define __LOGD_H__
#include <stdio.h>

#ifndef DEBUG_FLAG
#define DEBUG_FLAG                            1
#endif

#ifdef _MSC_VER

#if DEBUG_FLAG
#define LOGE(fmt, ...)  printf(__FILE__"(%d): "##fmt, __LINE__, __VA_ARGS__)
#define LOGD(fmt, ...)  printf(fmt,  __VA_ARGS__)
#define LOGW(fmt, ...)  printf(fmt,  __VA_ARGS__)
#else
#define LOGD(fmt, ...)
#define LOGE(fmt, ...)
#define LOGW(fmt, ...)
#endif //DEBUG_FLAG

#else // _MSC_VER

#if DEBUG_FLAG
#define LOGE(fmt, args...)  printf(__FILE__"(%d): "fmt, __LINE__, ##args)
#define LOGD(fmt, args...)  printf(fmt, ##args)
#define LOGW(fmt, args...)  printf(fmt, ##args)
#else
#define LOGD(fmt, args...)
#define LOGE(fmt, args...)
#define LOGW(fmt, args...)
#endif // DEBUG_FLAG
#endif

#endif