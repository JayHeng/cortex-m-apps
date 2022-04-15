
#include "mbw.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

my_mem_t g_myMem;
   
/*******************************************************************************
 * Code
 ******************************************************************************/

int gettimeofday(timeval_t *tv, timezone_t *tz)
{
    uint64_t ticks = timer_clock();
    uint32_t clocksPerSec = timer_clocks_per_sec();
    tv->tv_sec = ticks / clocksPerSec;
    tv->tv_usec = (ticks % clocksPerSec) / (clocksPerSec / 1000000);
    
    return 0;
}

// From glibc-2.26
/* Copy memory to memory until the specified number of bytes
   has been copied, return pointer to following byte.
   Overlap is NOT handled correctly. */
void *mempcpy(void *restrict dest, const void *restrict src, size_t n)
{
    return (void *)((uint8_t *)memcpy(dest, src, n) + n);
}

void *my_calloc(size_t nitems, size_t size)
{
    for (uint32_t i = 0; i < MAX_MEM_REGIONS; i++)
    {
        if (0 == g_myMem.regionStart[i])
        {
            if (0 == i)
            {
                g_myMem.regionStart[i] = g_myMem.memStart;
            }
            else
            {
                g_myMem.regionStart[i] = g_myMem.regionStart[i - 1] + g_myMem.regionSize[i - 1];
            }
            uint32_t neededSize = nitems * size;
            if (neededSize > g_myMem.memStart + g_myMem.memSize - g_myMem.regionStart[i])
            {
                g_myMem.regionStart[i] = 0;
                break;
            }
            else
            {
                g_myMem.regionSize[i] = neededSize;
                return (void *)g_myMem.regionStart[i];
            }
        }
    }
    
    return 0;
}

void my_free(void *ptr)
{
    uint32_t start = (uint32_t)ptr;
    for (uint32_t i = 0; i < MAX_MEM_REGIONS; i++)
    {
        if ((start == g_myMem.regionStart[i]) &&
            (0 != g_myMem.regionSize[i]))
        {
            g_myMem.regionStart[i] = 0;
            g_myMem.regionSize[i] = 0;
            break;
        }
    }
}

