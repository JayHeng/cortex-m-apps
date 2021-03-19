/*
 * The Clear BSD License
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 *  that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "app.h"
#include "memtester.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
typedef struct _semc_test_config {
  uint32_t base_addr;
  uint32_t test_size;
  uint32_t loop_num;
  uint32_t dram_freq;
  uint32_t enable_cache;
} semc_test_config;

semc_test_config test_config;

int fail_stop;
   
/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */
int main(void)
{
    uint32_t semcclk = 0;
    char memsuffix = 'B';
    /* Init board hardware. */
    BOARD_InitHardware();
    semcclk = CLOCK_GetFreq(kCLOCK_SemcClk);

    /* --------------- stress test --------------- */
    test_config.base_addr = 0x80000000;
    test_config.test_size = 64 * 1024;
    test_config.loop_num = 1;
    test_config.dram_freq = semcclk;
    test_config.enable_cache = 0;
    
    if (!test_config.enable_cache) {
        /* Disable D cache */
        SCB_DisableDCache();
    }

    PRINTF("\r\n########## Print out from target board ##########\r\n");
    PRINTF("\r\nDRAM test setting:\r\n");
    PRINTF("                  Base Address: 0x%x;\r\n", test_config.base_addr);
    PRINTF("                  Test Size: %d Bytes;\r\n", test_config.test_size);
    PRINTF("                  Test Loop: %d;\r\n", test_config.loop_num);    
    PRINTF("                  DRAM Freq: %d;\r\n",test_config.dram_freq);
    PRINTF("                  Enable Cache: %d;\r\n", test_config.enable_cache);

    fail_stop = 1;

    /* Run memory stress test: 64KByte,loop=1,page_size = 1kbyte */
    memtester_main(test_config.base_addr, test_config.test_size, &memsuffix, test_config.loop_num, (1*1024));

    while (1)
    {
    }
}
