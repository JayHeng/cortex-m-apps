/* 
	File : core_portme.c
*/
/*
	Author : Shay Gal-On, EEMBC
	Legal : TODO!
*/ 
#include "coremark.h"
#include "core_portme.h"
#include "clock_config.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "board.h"
#include "fsl_mrt.h"
#include "fsl_gpio.h"
#if (defined(CPU_MIMXRT798SGFOA_cm33_core0))
#define MRT             MRT0             /* Timer 0 */
#define MRT_CHANNEL     kMRT_Channel_0
#define MRT_IRQ         MRT0_IRQn
#define MRT_IRQHANDLER  MRT0_IRQHandler
#define MRT_CLK_FREQ CLOCK_GetFreq(kCLOCK_BusClk)
#elif (defined(CPU_MIMXRT798SGFOA_cm33_core1))
#define MRT             MRT1             /* Timer 1 */
#define MRT_CHANNEL     kMRT_Channel_0
#define MRT_IRQ         MRT1_IRQn
#define MRT_IRQHANDLER  MRT1_IRQHandler
#define MRT_CLK_FREQ CLOCK_GetFreq(kCLOCK_BusClk)
#endif

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

/*******************************************************************************
 * Prototypes
 ******************************************************************************/


/*******************************************************************************
 * Variables
 ******************************************************************************/


/*******************************************************************************
 * Code
 ******************************************************************************/

volatile uint32_t s_timerHighCounter = 0;
void MRT_IRQHANDLER(void)
{
    /* Clear interrupt flag.*/
    MRT_ClearStatusFlags(MRT, MRT_CHANNEL, kMRT_TimerInterruptFlag);
    s_timerHighCounter++;
    SDK_ISR_EXIT_BARRIER;
}

void timer_init(void)
{
    /* Structure of initialize MRT */
    mrt_config_t mrtConfig;

    /* mrtConfig.enableMultiTask = false; */
    MRT_GetDefaultConfig(&mrtConfig);

    /* Init mrt module */
    MRT_Init(MRT, &mrtConfig);

    /* Setup Channel 0 to be repeated */
    MRT_SetupChannelMode(MRT, MRT_CHANNEL, kMRT_RepeatMode);

    /* Enable timer interrupts for channel 0 */
    MRT_EnableInterrupts(MRT, MRT_CHANNEL, kMRT_TimerInterruptEnable);

    /* Enable at the NVIC */
    EnableIRQ(MRT_IRQ);
    
    MRT_StartTimer(MRT, MRT_CHANNEL, MRT_CHANNEL_INTVAL_IVALUE_MASK);
}

/* Porting : Timing functions
	How to capture time and convert to seconds must be ported to whatever is supported by the platform.
	e.g. Read value from on board RTC, read value from cpu clock cycles performance counter etc. 
	Sample implementation for standard time.h and windows.h definitions included.
*/
CORETIMETYPE barebones_clock() {
    uint64_t retVal = 0;
    uint32_t high;
    uint32_t low;
    do
    {
        high = s_timerHighCounter;
        low = MRT_GetCurrentTimerCount(MRT, MRT_CHANNEL);
    } while (high != s_timerHighCounter);
    retVal = ((uint64_t)high << 24U) + low;

    return retVal;
}
/* Define : TIMER_RES_DIVIDER
	Divider to trade off timer resolution and total time that can be measured.

	Use lower values to increase resolution, but make sure that overflow does not occur.
	If there are issues with the return value overflowing, increase this value.
	*/
#define CLOCKS_PER_SEC MRT_CLK_FREQ
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
#if (defined(CPU_MIMXRT798SGFOA_cm33_core0))
extern void set_power(void);
extern void APP_BootCore1(void);
extern void APP_CopyCore1Image(void);
#endif
void portable_init(core_portable *p, int *argc, char *argv[])
{
    /* Define the init structure for the output LED pin*/
    gpio_pin_config_t led_config = {
        kGPIO_DigitalOutput,
        0,
    };
    //*(uint32_t*)0x40001010 = (*(uint32_t*)0x40001010) | 0x6;
    //*(uint32_t*)0x40033000 = (*(uint32_t*)0x40033000) | 0x1;
    //*(uint32_t*)0x40034000 = (*(uint32_t*)0x40034000) | 0x1;

    /* Init board hardware. */
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

#if (defined(CPU_MIMXRT798SGFOA_cm33_core0))
    set_power();
    
    SYSCON0->COMP_AUTOGATE_EN = 0x7;

    *(uint32_t*)0x40033000 = 0x85000001;
    *(uint32_t*)0x40034000 = 0x85000001;

    APP_CopyCore1Image();
    APP_BootCore1();

    RESET_ClearPeripheralReset(kGPIO0_RST_SHIFT_RSTn);
    CLOCK_EnableClock(kCLOCK_Gpio0);
    RESET_PeripheralReset(kGPIO0_RST_SHIFT_RSTn);
    /* Init output LED GPIO. */
    GPIO_PinInit(GPIO0, 6, &led_config);
    GPIO_PortToggle(GPIO0, 1u << 6);

    ee_printf("--------------------------------\n");
#if defined(RUN_XIP)
    ee_printf(".text section in XSPI0 Flash\n");
#else
    ee_printf(".text section in SRAM P4\n");
#endif
    ee_printf(".data section in SRAM P16\n");
    ee_printf("STACK section in SRAM P0\n");
    ee_printf("i.MXRT798 core0 clk freq: %dHz\r\n", CLOCK_GetFreq(kCLOCK_CoreSysClk));
#elif (defined(CPU_MIMXRT798SGFOA_cm33_core1))

    RESET_ClearPeripheralReset(kGPIO8_RST_SHIFT_RSTn);
    CLOCK_EnableClock(kCLOCK_Gpio8);
    RESET_PeripheralReset(kGPIO8_RST_SHIFT_RSTn);
    /* Init output LED GPIO. */
    GPIO_PinInit(GPIO8, 6, &led_config);
    GPIO_PortToggle(GPIO8, 1u << 6);

    ee_printf("--------------------------------\n");
    ee_printf(".text section in SRAM P18\n");
    ee_printf(".data section in SRAM P26\n");
    ee_printf("STACK section in SRAM P26\n");
    ee_printf("i.MXRT798 core1 clk freq: %dHz\r\n", CLOCK_GetFreq(kCLOCK_CoreSysClk));
#endif

    /* Init timer for microsecond function. */
    timer_init();
    
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
}


