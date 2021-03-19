/*
 * Very simple (yet, for some reason, very effective) memory tester.
 * Originally by Simon Kirby <sim@stormix.com> <sim@neato.org>
 * Version 2 by Charles Cazabon <charlesc-memtester@pyropus.ca>
 * Version 3 not publicly released.
 * Version 4 rewrite:
 * Copyright (C) 2004-2020 Charles Cazabon <charlesc-memtester@pyropus.ca>
 * Licensed under the terms of the GNU General Public License version 2 (only).
 * See the file COPYING for details.
 *
 * This file contains the declarations for external variables from the main file.
 * See other comments in that file.
 *
 */

#ifndef __MEMTESTER_H__
#define __MEMTESTER_H__
#include "types.h"
#include "sizes.h"
#include "fsl_debug_console.h"

/* extern declarations. */

extern int use_phys;
extern off_t physaddrbase;

int memtester_main(ul physaddrbase, ul wantraw, char * memsuffix, ul loops,ul pagesize);

#endif // __MEMTESTER_H__
