/* generated vector source file - do not edit */
#include "bsp_api.h"
/* Do not build these data structures if no interrupts are currently allocated because IAR will have build errors. */
#if VECTOR_DATA_IRQ_COUNT > 0
        BSP_DONT_REMOVE const fsp_vector_t g_vector_table[BSP_ICU_VECTOR_MAX_ENTRIES] BSP_PLACE_IN_SECTION(BSP_SECTION_APPLICATION_VECTORS) =
        {
                        [0] = ulpt_int_isr, /* ULPT0 INT (ULPT0 Underflow) */
            [1] = ulpt_int_isr, /* ULPT1 INT (ULPT1 Underflow) */
        };
        #if BSP_FEATURE_ICU_HAS_IELSR
        const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_MAX_ENTRIES] =
        {
            [0] = BSP_PRV_VECT_ENUM(EVENT_ULPT0_INT,GROUP0), /* ULPT0 INT (ULPT0 Underflow) */
            [1] = BSP_PRV_VECT_ENUM(EVENT_ULPT1_INT,GROUP1), /* ULPT1 INT (ULPT1 Underflow) */
        };
        #endif
        #endif
