// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2020 SEAL AG
 */

#include <common.h>
#include <fuse.h>

int checkboard(void)
{
	char sn[9] = {0};

	fuse_sense(4, 6, (uint32_t *)sn);
	fuse_sense(4, 7, (uint32_t *)(sn + 4));

	printf("S/N:   %s\n", sn);

	return 0;
}
