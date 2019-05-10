
/*==================================================================================================

    Module Name:  hapi_irom_clock.c

    General Description: This file defines all functions used to access the clock control registers

====================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
#include "fsl_common.h"
#include "pu_irom_ocotp.h"
/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
#define FREQ_24MHz       (24000000UL)
#define FREQ_396MHz      (396000000UL)
#define FREQ_528MHz      (528000000UL)

enum
{
    kMaxAHBClock = 144000000UL,
};

/*==================================================================================================
                                 LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

static void hapi_gate_clocks(void);

/*==================================================================================================
                                     LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                                      LOCAL VARIABLES
==================================================================================================*/
extern uint32_t SystemCoreClock;

/*=================================================================================================
                                       GLOBAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================

FUNCTION: hapi_clock_init

DESCRIPTION:
    Reads type of boot device, performs basic configuration needed to access the memory,
    and returns the base address of the data the irom will need.

ARGUMENTS PASSED:
    None.

RETURN VALUE:
   uint32_t *flash_base_ptr - A pointer to the base of the flash memory.

PRE-CONDITIONS:
   None

POST-CONDITIONS:
   None

IMPORTANT NOTES:
   None

==================================================================================================*/
void hapi_clock_init(void)
{
    uint32_t clock_divider = 1;
    uint32_t fuse_div = 0;
    uint32_t clock_freq = 0;

    // Get the Boot Up CPU Clock Divider
    // 00b = divide by 1
    // 01b = divide by 2
    // 10b = divide by 4
    // 11b = divide by 8
    fuse_div = ROM_OCOTP_LPB_BOOT_VALUE();
    clock_divider = 1 << fuse_div;

    // Get the Boot up frequency
    // 0 = 396Mhz
    // 1 = 528Mhz
    clock_freq = ROM_OCOTP_BOOT_FREQ_VALUE();

    // gate unused Clocks
    hapi_gate_clocks();

    /* Configure PLL_ARM: Reference clock = 24MHz
     * PLL_ARM = 24MHz * div / 2
     *  Core = PLL_ARM / 2 / clock_divider
     * To get 396MHz clock, PLL_ARM = 24 * 66 / 2 = 792, 792 / 2 = 396
     * To get 528MHz clock, PLL_ARM = 24 * 88 / 2 = 1056, 1056 / 2 = 528
     */
    uint32_t div = (clock_freq == 1) ? 88 : 66;
    CCM_ANALOG->PLL_ARM = CCM_ANALOG_PLL_ARM_BYPASS(1) | CCM_ANALOG_PLL_ARM_ENABLE(1) | \
                          CCM_ANALOG_PLL_ARM_DIV_SELECT(div);
    // Wait Until clock is locked
    while ((CCM_ANALOG->PLL_ARM & CCM_ANALOG_PLL_ARM_LOCK_MASK) == 0)
    {
    }


    /* Configure PLL_SYS */
    CCM_ANALOG->PLL_SYS &= ~CCM_ANALOG_PLL_SYS_POWERDOWN_MASK;
    // Wait Until clock is locked
    while ((CCM_ANALOG->PLL_SYS & CCM_ANALOG_PLL_SYS_LOCK_MASK) == 0)
    {
    }


    // Configure SYS_PLL PFD
    // PFD0 = 396MHz  - uSDHC CLOCK Source
    // PFD1 = 396MHz
    // PFD2 = 500MHz  - SEMC CLOCK Source
    // PFD3 = 396MHz
    CCM_ANALOG->PFD_528 =  (CCM_ANALOG->PFD_528 & (~(CCM_ANALOG_PFD_528_PFD0_FRAC_MASK | CCM_ANALOG_PFD_528_PFD1_FRAC_MASK | \
                            CCM_ANALOG_PFD_528_PFD2_FRAC_MASK | CCM_ANALOG_PFD_528_PFD3_FRAC_MASK))) | \
                            CCM_ANALOG_PFD_528_PFD0_FRAC(24) | CCM_ANALOG_PFD_528_PFD1_FRAC(24) | \
                            CCM_ANALOG_PFD_528_PFD2_FRAC(19)  | CCM_ANALOG_PFD_528_PFD3_FRAC(24);

    // Always configure USB1_PLL
    CCM_ANALOG->PLL_USB1 = CCM_ANALOG_PLL_USB1_DIV_SELECT(0) | CCM_ANALOG_PLL_USB1_POWER(1) | CCM_ANALOG_PLL_USB1_ENABLE(1);
    while ((CCM_ANALOG->PLL_USB1 & CCM_ANALOG_PLL_USB1_LOCK_MASK) == 0)
    {
    }
    CCM_ANALOG->PLL_USB1 &= ~CCM_ANALOG_PLL_USB1_BYPASS_MASK;

    // Configure USB_PLL PFD
    // PFD0 = 247MHz  - FLEXSPI CLOCK Source
    // PFD1 = 247MHz  - LPSPI CLOCK Source
    // PFD2 = 332MHz
    // PFD3 = 576MHz
    CCM_ANALOG->PFD_480 =  (CCM_ANALOG->PFD_480 & (~(CCM_ANALOG_PFD_480_PFD0_FRAC_MASK | CCM_ANALOG_PFD_480_PFD1_FRAC_MASK | \
                            CCM_ANALOG_PFD_480_PFD2_FRAC_MASK | CCM_ANALOG_PFD_480_PFD3_FRAC_MASK))) | \
                            CCM_ANALOG_PFD_480_PFD0_FRAC(35) | CCM_ANALOG_PFD_480_PFD1_FRAC(35) | \
                            CCM_ANALOG_PFD_480_PFD2_FRAC(26)  | CCM_ANALOG_PFD_480_PFD3_FRAC(15);

    // Set up CPU_PODF
    CCM->CACRR = CCM_CACRR_ARM_PODF(1);

    // Calculate the Final System Core Clock, it will be used to calculate the AHB / ARM Core divider later.
    SystemCoreClock = ((clock_freq == 0) ? FREQ_396MHz : FREQ_528MHz)  / clock_divider;

    // Calculate the AHB clock divider
    uint32_t ahb_divider = 1;
    while ((SystemCoreClock / ahb_divider) > kMaxAHBClock)
    {
        ++ahb_divider;
    }

    // Set up AXI_PODF - SEMC clock root
    // Set up AHB_PODF - CORE clock
    // Set up IPG_PODF - BUS clock
    CCM->CBCDR = (CCM->CBCDR & (~(CCM_CBCDR_SEMC_PODF_MASK | CCM_CBCDR_AHB_PODF_MASK | CCM_CBCDR_IPG_PODF_MASK))) \
            | CCM_CBCDR_SEMC_PODF(ahb_divider-1) | CCM_CBCDR_AHB_PODF(clock_divider-1) | CCM_CBCDR_IPG_PODF(ahb_divider-1);

    // LPUART clock configuration, peripheral clock 20MHz
    CCM->CSCDR1 = (CCM->CSCDR1 & (~(CCM_CSCDR1_UART_CLK_SEL_MASK | CCM_CSCDR1_UART_CLK_PODF_MASK))) | CCM_CSCDR1_UART_CLK_PODF(3);

    // Pre-peripheral clock configuration
    CCM->CBCMR = (CCM->CBCMR & (~CCM_CBCMR_PRE_PERIPH_CLK_SEL_MASK)) | CCM_CBCMR_PRE_PERIPH_CLK_SEL(3);

    // LPSPI clock configuration, Peripheral clock: 41MHz
    CCM->CBCMR = (CCM->CBCMR & (~(CCM_CBCMR_LPSPI_CLK_SEL_MASK | CCM_CBCMR_LPSPI_PODF_MASK ))) | CCM_CBCMR_LPSPI_CLK_SEL(0)  | CCM_CBCMR_LPSPI_PODF(5);

    // FLEXSPI clock configuration, safe frequency: 30MHz
    CCM->CSCMR1 = ((CCM->CSCMR1 & \
                    ~(CCM_CSCMR1_FLEXSPI_CLK_SEL_MASK | CCM_CSCMR1_FLEXSPI_PODF_MASK | CCM_CSCMR1_PERCLK_PODF_MASK | CCM_CSCMR1_PERCLK_CLK_SEL_MASK ))
                   | CCM_CSCMR1_FLEXSPI_CLK_SEL(3) | CCM_CSCMR1_FLEXSPI_PODF(7) | CCM_CSCMR1_PERCLK_PODF(1) );

    // NOTE: SEMC clock configuration needs handshake, so it will be handled by SEMC driver itself
    // uSDHC1&2 clock configuration
    // SEL: PULLL2 PFD0; DIV: 1 (PFD/2, freq=200MHz)
    CCM->CSCMR1 = (CCM->CSCMR1 & \
                   (~(CCM_CSCMR1_USDHC1_CLK_SEL_MASK | CCM_CSCMR1_USDHC2_CLK_SEL_MASK))) | \
                  CCM_CSCMR1_USDHC1_CLK_SEL(1) | CCM_CSCMR1_USDHC2_CLK_SEL(1);
    CCM->CSCDR1 = (CCM->CSCDR1 & \
                   (~(CCM_CSCDR1_USDHC1_PODF_MASK | CCM_CSCDR1_USDHC2_PODF_MASK))) | \
                  CCM_CSCDR1_USDHC1_PODF(1) | CCM_CSCDR1_USDHC2_PODF(1);


   // Finally, Enable PLL_ARM, PLL_SYS and PLL_USB1
    CCM_ANALOG->PLL_ARM &= ~CCM_ANALOG_PLL_ARM_BYPASS_MASK;
    CCM_ANALOG->PLL_SYS &= ~CCM_ANALOG_PLL_SYS_BYPASS_MASK;
    CCM_ANALOG->PLL_USB1 &= ~CCM_ANALOG_PLL_USB1_BYPASS_MASK;
}

/*================================================================================================

FUNCTION:
   hapi_gate_clocks

DESCRIPTION:
   This function is called to gate all unused clocks.

ARGUMENTS PASSED:
   None

RETURN VALUE:
   None

PRE-CONDITIONS:
   None

POST-CONDITIONS:
   None

IMPORTANT NOTES:
    None

==================================================================================================*/
static void hapi_gate_clocks(void)
{
    // Disable LPUART2, GPT2, CAN2, CAN1, LPUART3
    CCM->CCGR0 &= ~(CCM_CCGR0_CG14_MASK|CCM_CCGR0_CG13_MASK|CCM_CCGR0_CG12_MASK|CCM_CCGR0_CG10_MASK| \
                    CCM_CCGR0_CG9_MASK|CCM_CCGR0_CG8_MASK|CCM_CCGR0_CG7_MASK|CCM_CCGR0_CG6_MASK);
    // Disable LPUART4, ADC1, PIT, ENET, ADC2, LPSPI4, LPSPI3, LPSPI2, LPSPI1
    CCM->CCGR1 &= ~(CCM_CCGR1_CG12_MASK|CCM_CCGR1_CG8_MASK|CCM_CCGR1_CG6_MASK|CCM_CCGR1_CG5_MASK|CCM_CCGR1_CG4_MASK|\
                    CCM_CCGR1_CG3_MASK|CCM_CCGR1_CG2_MASK|CCM_CCGR1_CG1_MASK|CCM_CCGR1_CG0_MASK);
    // Disable PXP, LCDIF, LPI2C3, LPI2C2, LPI2C1, CSI
    CCM->CCGR2 &= ~(CCM_CCGR2_CG15_MASK|CCM_CCGR2_CG14_MASK|CCM_CCGR2_CG5_MASK|CCM_CCGR2_CG4_MASK|\
                    CCM_CCGR2_CG3_MASK|CCM_CCGR2_CG1_MASK);
    // Disable ACMP4, ACMP3, ACMP2, LCDIF_PIX, LPUART6, LPUART5, FLEXIO2
    CCM->CCGR3 &= ~(CCM_CCGR3_CG13_MASK|CCM_CCGR3_CG12_MASK|CCM_CCGR3_CG11_MASK|CCM_CCGR3_CG10_MASK|\
                    CCM_CCGR3_CG5_MASK|CCM_CCGR3_CG3_MASK|CCM_CCGR3_CG1_MASK|CCM_CCGR3_CG0_MASK);
    // Disable ENC4, ENC3, ENC2, ENC1, PWM4, PWM3, PWM2, PWM1
    CCM->CCGR4 &= ~(CCM_CCGR4_CG15_MASK|CCM_CCGR4_CG14_MASK|CCM_CCGR4_CG13_MASK|CCM_CCGR4_CG12_MASK|CCM_CCGR4_CG11_MASK|\
                    CCM_CCGR4_CG10_MASK|CCM_CCGR4_CG9_MASK|CCM_CCGR4_CG8_MASK);
    // Disable LPUART7, LPUART1, SAI3, SAI2, SAI1, SPDIF, KPP, DMA, FLEXIO1
    CCM->CCGR5 &= ~(CCM_CCGR5_CG13_MASK|CCM_CCGR5_CG12_MASK|CCM_CCGR5_CG11_MASK|CCM_CCGR5_CG10_MASK|CCM_CCGR5_CG9_MASK|\
                    CCM_CCGR5_CG7_MASK|CCM_CCGR5_CG4_MASK|CCM_CCGR5_CG3_MASK|CCM_CCGR5_CG1_MASK);
    // QTIMER3, QTIMER2, QTIMER1, LPI2C4, QTIMER4, LPUART8, FLEXSPI, USDHC2, USDHC1, USB clock
    CCM->CCGR6 &= ~(CCM_CCGR6_CG15_MASK|CCM_CCGR6_CG14_MASK|CCM_CCGR6_CG13_MASK|CCM_CCGR6_CG12_MASK|CCM_CCGR6_CG8_MASK|\
                    CCM_CCGR6_CG7_MASK|CCM_CCGR6_CG6_MASK|CCM_CCGR6_CG5_MASK|CCM_CCGR6_CG2_MASK|CCM_CCGR6_CG1_MASK|\
                    CCM_CCGR6_CG0_MASK);
}

