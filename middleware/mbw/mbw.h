
#ifndef __MBW_H__
#define __MBW_H__

#include "fsl_common.h"
#include "fsl_debug_console.h"

typedef struct _timeval
{
    uint32_t tv_sec;
    uint32_t tv_usec;
} timeval_t;

typedef struct _timezone {
   uint32_t tz_minuteswest;
   uint32_t tz_dsttime;
} timezone_t;

int gettimeofday(timeval_t *tv, timezone_t *tz);
void *mempcpy(void *restrict dest, const void *restrict src, size_t n);

#endif // __MBW_H__
