/*
 * Very simple but very effective user-space memory tester.
 * Originally by Simon Kirby <sim@stormix.com> <sim@neato.org>
 * Version 2 by Charles Cazabon <charlesc-memtester@pyropus.ca>
 * Version 3 not publicly released.
 * Version 4 rewrite:
 * Copyright (C) 2004-2020 Charles Cazabon <charlesc-memtester@pyropus.ca>
 * Licensed under the terms of the GNU General Public License version 2 (only).
 * See the file COPYING for details.
 *
 * This file contains typedefs, structure, and union definitions.
 *
 */

#ifndef __TYPES_H__
#define __TYPES_H__

#include "fsl_common.h"

typedef unsigned int ul;
typedef unsigned int ull;
typedef unsigned int volatile ulv;
typedef unsigned char volatile u8v;
typedef unsigned short volatile u16v;
typedef unsigned int off_t;

struct test {
    char *name;
    int (*fp)();
};

#endif // __TYPES_H__
