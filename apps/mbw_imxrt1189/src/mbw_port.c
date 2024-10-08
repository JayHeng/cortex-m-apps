
#include "mbw.h"
#include "clock_config.h"
#include "fsl_lpit.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define PIT LPIT1

/*******************************************************************************
 * Prototypes
 ******************************************************************************/


/*******************************************************************************
 * Code
 ******************************************************************************/

void timer_init(void)
{
    // PIT clock gate control ON
    CLOCK_EnableClock(kCLOCK_Lpit1);

    lpit_config_t lpitConfig;
    lpit_chnl_params_t lpitChannelConfig;
    LPIT_GetDefaultConfig(&lpitConfig);
    // Init pit module
    LPIT_Init(PIT, &lpitConfig);

    // Set max timer period for channel 1
    LPIT_SetTimerPeriod(PIT, kLPIT_Chnl_1, (uint32_t)~0);
    // Disable timer interrupts for channel 1
    LPIT_DisableInterrupts(PIT, kLPIT_Channel1TimerInterruptEnable);
    // Clear timer channel 1 flag
    LPIT_ClearStatusFlags(PIT, kLPIT_Channel1TimerFlag);
    // Chain timer channel 1 to channel 0
    lpitChannelConfig.chainChannel          = true;
    lpitChannelConfig.enableReloadOnTrigger = false;
    lpitChannelConfig.enableStartOnTrigger  = false;
    lpitChannelConfig.enableStopOnTimeout   = false;
    lpitChannelConfig.timerMode             = kLPIT_PeriodicCounter;
    LPIT_SetupChannel(PIT, kLPIT_Chnl_1, &lpitChannelConfig);
    // Start timer channel 1
    LPIT_StartTimer(PIT, kLPIT_Chnl_1);

    // Set max timer period for channel 0
    LPIT_SetTimerPeriod(PIT, kLPIT_Chnl_0, (uint32_t)~0);
    // Clear timer channel 0 flag
    LPIT_ClearStatusFlags(PIT, kLPIT_Channel0TimerFlag);
    // Start timer channel 0
    LPIT_StartTimer(PIT, kLPIT_Chnl_0);
}

void timer_deinit(void)
{
    // Stop timer channel 1
    LPIT_StopTimer(PIT, kLPIT_Chnl_1);
    // Stop timer channel 0
    LPIT_StopTimer(PIT, kLPIT_Chnl_0);
    // Deinit pit module
    LPIT_Deinit(PIT);
}

uint64_t timer_clock(void)
{
#if defined(FSL_FEATURE_LPIT_HAS_LIFETIME_TIMER) && (FSL_FEATURE_LPIT_HAS_LIFETIME_TIMER == 1)
    // Note: first read LTMR64H and then LTMR64L. LTMR64H will have the value
    //  of CVAL1 at the time of the first access, LTMR64L will have the value of CVAL0 at the
    //  time of the first access, therefore the application does not need to worry about carry-over
    //  effects of the running counter.
    return ~LPIT_GetLifetimeTimerCount(PIT);
#else
    uint64_t valueH;
    volatile uint32_t valueL;
    // Make sure that there are no rollover of valueL.
    // Because the valueL always decreases, so, if the formal valueL is greater than
    // current value, that means the valueH is updated during read valueL.
    // In this case, we need to re-update valueH and valueL.
    do
    {
        valueL = LPIT_GetCurrentTimerCount(PIT, kLPIT_Chnl_0);
        valueH = LPIT_GetCurrentTimerCount(PIT, kLPIT_Chnl_1);
    } while (valueL < LPIT_GetCurrentTimerCount(PIT, kLPIT_Chnl_0));

    // Invert to turn into an up counter
    return ~((valueH << 32) | valueL);
#endif // FSL_FEATURE_LPIT_HAS_LIFETIME_TIMER
}

uint32_t timer_clocks_per_sec(void)
{
    return CLOCK_GetRootClockFreq(kCLOCK_Root_Bus_Aon);
}

