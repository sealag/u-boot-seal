/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2020 SEAL AG
 */

#ifndef __S5628_H
#define __S5628_H

#include <configs/seal_imx6ull_common.h>

#undef CONFIG_EXTRA_ENV_SETTINGS
#define CONFIG_EXTRA_ENV_SETTINGS \
	"fdt_high=0xffffffff\0" \
	"initrd_high=0xffffffff\0" \
	"ip_dyn=yes\0" \
	"dfu_alt_info=mmc 1=" \
		"boot raw 0x2 0x1fbe mmcpart 1;" \
		"spl raw 0x2 0xfe mmcpart 1;" \
		"u-boot raw 0x100 0x1ec0 mmcpart 1;" \
		"u-boot-env raw 0x1fc0 0x40 mmcpart 1\0" \
	"fastboot_raw_partition_boot=0x2 0x1fbe mmcpart 1\0" \
	"fastboot_raw_partition_spl=0x2 0xfe mmcpart 1\0" \
	"fastboot_raw_partition_uboot=0x100 0x1ec0 mmcpart 1\0" \
	"fastboot_raw_partition_ubootenv=0x1fc0 0x40 mmcpart 1\0" \
	"partitions=name=rootfs_a,size=128MiB,bootable;name=rootfs_b,size=128MiB;name=config,size=128MiB;name=data,size=-\0" \
	"fdt_addr_r=0x83000000\0" \
	"kernel_addr_r=0x82000000\0" \
	"pxefile_addr_r=0x81f00000\0" \
	"ramdisk_addr_r=0x84000000\0" \
	"scriptaddr=0x81f00000\0" \
	BOOTENV

#endif
