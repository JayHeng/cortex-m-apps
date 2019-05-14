/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DHRY_PORTME_H
#define DHRY_PORTME_H

#include <stdint.h>

/*******************************************************************************
 * Externs
 ******************************************************************************/

extern double secs;

/*******************************************************************************
 * API
 ******************************************************************************/

void start_time(void);
void end_time(void);

#endif /* DHRY_PORTME_H */
