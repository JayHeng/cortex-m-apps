/* 
	File : core_portme.c
*/
/*
	Author : Shay Gal-On, EEMBC
	Legal : TODO!
*/ 
#include "coremark.h"
#include "core_portme.h"
#include "app.h"
#include "clock_config.h"
#include "fsl_pit.h"

#if VALIDATION_RUN
	volatile ee_s32 seed1_volatile=0x3415;
	volatile ee_s32 seed2_volatile=0x3415;
	volatile ee_s32 seed3_volatile=0x66;
#endif
#if PERFORMANCE_RUN
	volatile ee_s32 seed1_volatile=0x0;
	volatile ee_s32 seed2_volatile=0x0;
	volatile ee_s32 seed3_volatile=0x66;
#endif
#if PROFILE_RUN
	volatile ee_s32 seed1_volatile=0x8;
	volatile ee_s32 seed2_volatile=0x8;
	volatile ee_s32 seed3_volatile=0x8;
#endif
	volatile ee_s32 seed4_volatile=ITERATIONS;
	volatile ee_s32 seed5_volatile=0;

/*
volatile uint32_t s_timerHighCounter = 0;
void PIT_IRQ_HANDLER(void)
{
    // Clear interrupt flag.
    PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag);
    s_timerHighCounter++;
}

void timer_pit_init(void)
{
    s_timerHighCounter = 0;
    // Structure of initialize PIT 
    pit_config_t pitConfig;
    PIT_GetDefaultConfig(&pitConfig);
    // Init pit module
    PIT_Init(PIT, &pitConfig);
    // Set max timer period for channel 0 
    PIT_SetTimerPeriod(PIT, kPIT_Chnl_0, (uint32_t)~0);
    // Enable timer interrupts for channel 0
    PIT_EnableInterrupts(PIT, kPIT_Chnl_0, kPIT_TimerInterruptEnable);
    // Enable at the NVIC
    EnableIRQ(PIT_IRQ_ID);
    // Start channel 0
    PIT_StartTimer(PIT, kPIT_Chnl_0);
}
*/

void timer_pit_init(void)
{
    // PIT clock gate control ON
    CLOCK_EnableClock(kCLOCK_Pit);

    // Turn on PIT: MDIS = 0, FRZ = 0
    PIT->MCR = 0x00;

    // Set up timer 1 to max value
    PIT->CHANNEL[1].LDVAL = 0xFFFFFFFF;          // setup timer 1 for maximum counting period
    PIT->CHANNEL[1].TCTRL = 0;                   // Disable timer 1 interrupts
    PIT->CHANNEL[1].TFLG = 1;                    // clear the timer 1 flag
    PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_CHN_MASK; // chain timer 1 to timer 0
    PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TEN_MASK; // start timer 1

    // Set up timer 0 to max value
    PIT->CHANNEL[0].LDVAL = 0xFFFFFFFF;         // setup timer 0 for maximum counting period
    PIT->CHANNEL[0].TFLG = 1;                   // clear the timer 0 flag
    PIT->CHANNEL[0].TCTRL = PIT_TCTRL_TEN_MASK; // start timer 0
}

void timer_pit_deinit(void)
{
    // Turn off PIT: MDIS = 1, FRZ = 0
    PIT->CHANNEL[1].TCTRL = 0; // stop timer 1
    PIT->CHANNEL[0].TCTRL = 0; // stop timer 1
    PIT->CHANNEL[1].LDVAL = 0;
    PIT->CHANNEL[0].LDVAL = 0;
    PIT->MCR |= PIT_MCR_MDIS_MASK;
}

/* Porting : Timing functions
	How to capture time and convert to seconds must be ported to whatever is supported by the platform.
	e.g. Read value from on board RTC, read value from cpu clock cycles performance counter etc. 
	Sample implementation for standard time.h and windows.h definitions included.
*/
/*
CORETIMETYPE barebones_clock() {
    uint64_t retVal;
    uint32_t high;
    uint32_t low;
    do
    {
        high = s_timerHighCounter;
        low = ~PIT_GetCurrentTimerCount(PIT, kPIT_Chnl_0);
    } while (high != s_timerHighCounter);
    retVal = ((uint64_t)high << 32U) + low;

    return retVal;
}
*/

CORETIMETYPE barebones_clock() {
    uint64_t valueH;
    volatile uint32_t valueL;

#if defined(FSL_FEATURE_PIT_HAS_LIFETIME_TIMER) && (FSL_FEATURE_PIT_HAS_LIFETIME_TIMER == 1)
    // Note: first read LTMR64H and then LTMR64L. LTMR64H will have the value
    //  of CVAL1 at the time of the first access, LTMR64L will have the value of CVAL0 at the
    //  time of the first access, therefore the application does not need to worry about carry-over
    //  effects of the running counter.
    valueH = PIT->LTMR64H;
    valueL = PIT->LTMR64L;
#else
    // Make sure that there are no rollover of valueL.
    // Because the valueL always decreases, so, if the formal valueL is greater than
    // current value, that means the valueH is updated during read valueL.
    // In this case, we need to re-update valueH and valueL.
    do
    {
        valueL = PIT->CHANNEL[0].CVAL;
        valueH = PIT->CHANNEL[1].CVAL;
    } while (valueL < PIT->CHANNEL[0].CVAL);
#endif // FSL_FEATURE_PIT_HAS_LIFETIME_TIMER

    // Invert to turn into an up counter
    return ~((valueH << 32) | valueL);
}

/* Define : TIMER_RES_DIVIDER
	Divider to trade off timer resolution and total time that can be measured.

	Use lower values to increase resolution, but make sure that overflow does not occur.
	If there are issues with the return value overflowing, increase this value.
	*/
#define CLOCKS_PER_SEC (CLOCK_GetPerClkFreq())
#define GETMYTIME(_t) (*_t=barebones_clock())
#define MYTIMEDIFF(fin,ini) ((fin)-(ini))
#define TIMER_RES_DIVIDER 1
#define SAMPLE_TIME_IMPLEMENTATION 1
#define EE_TICKS_PER_SEC (CLOCKS_PER_SEC / TIMER_RES_DIVIDER)

/** Define Host specific (POSIX), or target specific global time variables. */
static CORETIMETYPE start_time_val, stop_time_val;

/* Function : start_time
	This function will be called right before starting the timed portion of the benchmark.

	Implementation may be capturing a system timer (as implemented in the example code) 
	or zeroing some system parameters - e.g. setting the cpu clocks cycles to 0.
*/
void start_time(void) {
	GETMYTIME(&start_time_val );      
}
/* Function : stop_time
	This function will be called right after ending the timed portion of the benchmark.

	Implementation may be capturing a system timer (as implemented in the example code) 
	or other system parameters - e.g. reading the current value of cpu cycles counter.
*/
void stop_time(void) {
	GETMYTIME(&stop_time_val );      
}
/* Function : get_time
	Return an abstract "ticks" number that signifies time on the system.
	
	Actual value returned may be cpu cycles, milliseconds or any other value,
	as long as it can be converted to seconds by <time_in_secs>.
	This methodology is taken to accomodate any hardware or simulated platform.
	The sample implementation returns millisecs by default, 
	and the resolution is controlled by <TIMER_RES_DIVIDER>
*/
CORE_TICKS get_time(void) {
	CORE_TICKS elapsed=(CORE_TICKS)(MYTIMEDIFF(stop_time_val, start_time_val));
	return elapsed;
}
/* Function : time_in_secs
	Convert the value returned by get_time to seconds.

	The <secs_ret> type is used to accomodate systems with no support for floating point.
	Default implementation implemented by the EE_TICKS_PER_SEC macro above.
*/
secs_ret time_in_secs(CORE_TICKS ticks) {
	secs_ret retval=((secs_ret)ticks) / (secs_ret)EE_TICKS_PER_SEC;
	return retval;
}

ee_u32 default_num_contexts=1;

/* Function : portable_init
	Target specific initialization code 
	Test for some common mistakes.
*/
void portable_init(core_portable *p, int *argc, char *argv[])
{
    /* Init board hardware. */
    BOARD_InitHardware();
    /* Init timer for microsecond function. */
    timer_pit_init();
    
	if (sizeof(ee_ptr_int) != sizeof(ee_u8 *)) {
		ee_printf("ERROR! Please define ee_ptr_int to a type that holds a pointer!\n");
	}
	if (sizeof(ee_u32) != 4) {
		ee_printf("ERROR! Please define ee_u32 to a 32b unsigned type!\n");
	}
	p->portable_id=1;
}
/* Function : portable_fini
	Target specific final code 
*/
void portable_fini(core_portable *p)
{
	p->portable_id=0;
    
    timer_pit_deinit();
}


