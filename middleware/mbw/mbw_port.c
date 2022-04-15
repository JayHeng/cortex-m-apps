
#include "mbw.h"
#include "clock_config.h"
#include "fsl_pit.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define PIT PIT1

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
   
/*******************************************************************************
 * Code
 ******************************************************************************/

void timer_pit_init(void)
{
    // PIT clock gate control ON
    CLOCK_EnableClock(kCLOCK_Pit1);

    pit_config_t pitConfig;
    PIT_GetDefaultConfig(&pitConfig);
    // Init pit module
    PIT_Init(PIT, &pitConfig);
    // Set max timer period for channel 1
    PIT_SetTimerPeriod(PIT, kPIT_Chnl_1, (uint32_t)~0);
    // Disable timer interrupts for channel 1
    PIT_DisableInterrupts(PIT, kPIT_Chnl_1, kPIT_TimerInterruptEnable);
    // Clear timer channel 1 flag
    PIT_ClearStatusFlags(PIT, kPIT_Chnl_1, kPIT_TimerFlag);
    // Chain timer channel 1 to channel 0
    PIT_SetTimerChainMode(PIT, kPIT_Chnl_1, true);
    // Start timer channel 1
    PIT_StartTimer(PIT, kPIT_Chnl_1);

    // Set max timer period for channel 0
    PIT_SetTimerPeriod(PIT, kPIT_Chnl_0, (uint32_t)~0);
    // Clear timer channel 0 flag
    PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag);
    // Start timer channel 0
    PIT_StartTimer(PIT, kPIT_Chnl_0);
}

void timer_pit_deinit(void)
{
    // Stop timer channel 1
    PIT_StopTimer(PIT, kPIT_Chnl_1);
    // Stop timer channel 0
    PIT_StopTimer(PIT, kPIT_Chnl_0);
    // Deinit pit module
    PIT_Deinit(PIT);
}

uint64_t timer_clock() {
#if defined(FSL_FEATURE_PIT_HAS_LIFETIME_TIMER) && (FSL_FEATURE_PIT_HAS_LIFETIME_TIMER == 1)
    // Note: first read LTMR64H and then LTMR64L. LTMR64H will have the value
    //  of CVAL1 at the time of the first access, LTMR64L will have the value of CVAL0 at the
    //  time of the first access, therefore the application does not need to worry about carry-over
    //  effects of the running counter.
    return ~PIT_GetLifetimeTimerCount(PIT);
#else
    uint64_t valueH;
    volatile uint32_t valueL;
    // Make sure that there are no rollover of valueL.
    // Because the valueL always decreases, so, if the formal valueL is greater than
    // current value, that means the valueH is updated during read valueL.
    // In this case, we need to re-update valueH and valueL.
    do
    {
        valueL = PIT_GetCurrentTimerCount(PIT, kPIT_Chnl_0);
        valueH = PIT_GetCurrentTimerCount(PIT, kPIT_Chnl_1);
    } while (valueL < PIT_GetCurrentTimerCount(PIT, kPIT_Chnl_0));

    // Invert to turn into an up counter
    return ~((valueH << 32) | valueL);
#endif // FSL_FEATURE_PIT_HAS_LIFETIME_TIMER
}

#define CLOCKS_PER_SEC  (CLOCK_GetRootClockFreq(kCLOCK_Root_Bus))
#define CLOCKS_PER_USEC (CLOCK_GetRootClockFreq(kCLOCK_Root_Bus)/1000000)

int gettimeofday(timeval_t *tv, timezone_t *tz)
{
    uint64_t ticks = timer_clock();
    tv->tv_sec = ticks / CLOCKS_PER_SEC;
    tv->tv_usec = (ticks % CLOCKS_PER_SEC) / CLOCKS_PER_USEC;
    
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

