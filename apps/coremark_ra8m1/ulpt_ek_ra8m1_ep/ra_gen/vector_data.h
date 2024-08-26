/* generated vector header file - do not edit */
#ifndef VECTOR_DATA_H
#define VECTOR_DATA_H
#ifdef __cplusplus
        extern "C" {
        #endif
/* Number of interrupts allocated */
#ifndef VECTOR_DATA_IRQ_COUNT
#define VECTOR_DATA_IRQ_COUNT    (2)
#endif
/* ISR prototypes */
void ulpt_int_isr(void);

/* Vector table allocations */
#define VECTOR_NUMBER_ULPT0_INT ((IRQn_Type) 0) /* ULPT0 INT (ULPT0 Underflow) */
#define ULPT0_INT_IRQn          ((IRQn_Type) 0) /* ULPT0 INT (ULPT0 Underflow) */
#define VECTOR_NUMBER_ULPT1_INT ((IRQn_Type) 1) /* ULPT1 INT (ULPT1 Underflow) */
#define ULPT1_INT_IRQn          ((IRQn_Type) 1) /* ULPT1 INT (ULPT1 Underflow) */
#ifdef __cplusplus
        }
        #endif
#endif /* VECTOR_DATA_H */
