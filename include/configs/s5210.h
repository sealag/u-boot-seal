/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2018 SEAL AG
 *
 * Configuration settings for the SEAL S5210
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include "mx6_common.h"
#include "imx6_spl.h"

#undef CONFIG_SYS_MONITOR_LEN
#define CONFIG_SYS_MONITOR_LEN		(3936 * 1024)

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		SZ_64M

/* UART */
#define CONFIG_MXC_UART_BASE		UART4_BASE
#undef CONFIG_CONS_INDEX
#define CONFIG_CONS_INDEX		1
#define CONFIG_SYS_BAUDRATE_TABLE	{ 115200, 230400, 460800, 921600 }

/* Memory test */
#define CONFIG_SYS_MEMTEST_START	0x10000000
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_MEMTEST_START + 240 * SZ_1M)

/* MMC */
#define CONFIG_SUPPORT_EMMC_BOOT

/* USB */
#define CONFIG_USB_MAX_CONTROLLER_COUNT	2
#define CONFIG_MXC_USB_PORTSC		(PORT_PTS_UTMI | PORT_PTS_PTW)
#define CONFIG_MXC_USB_FLAGS		0
#define CONFIG_USBD_HS

/* Ethernet */
#define CONFIG_MII
#define IMX_FEC_BASE			ENET_BASE_ADDR
#define CONFIG_FEC_XCV_TYPE		RMII
#define CONFIG_ETHPRIME			"FEC"
#define CONFIG_FEC_MXC_PHYADDR		0

#define CONFIG_ENV_VARS_UBOOT_RUNTIME_CONFIG
#define CONFIG_EXTRA_ENV_SETTINGS \
	"fdt_high=0xffffffff\0" \
	"initrd_high=0xffffffff\0" \
	"ip_dyn=yes\0" \
	"dfu_alt_info=" \
		"spl raw 0x2 0xfe mmcpart 1;" \
		"u-boot raw 0x100 0x1ec0 mmcpart 1;" \
		"u-boot-env raw 0x1fc0 0x40 mmcpart 1\0" \
	"fdt_addr_r=0x83000000\0" \
	"kernel_addr_r=0x82000000\0" \
	"pxefile_addr_r=0x81f00000\0" \
	"ramdisk_addr_r=0x84000000\0" \
	"scriptaddr=0x81f00000\0" \
	BOOTENV

#define BOOT_TARGET_DEVICES(func) \
	func(MMC, mmc, 1) \
	func(PXE, pxe, na) \
	func(DHCP, dhcp, na)

#include <config_distro_bootcmd.h>

/* Physical Memory Map */
#define PHYS_SDRAM			MMDC0_ARB_BASE_ADDR

#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM
#define CONFIG_SYS_INIT_RAM_ADDR	IRAM_BASE_ADDR
#define CONFIG_SYS_INIT_RAM_SIZE	IRAM_SIZE

#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

/* Environment organization */
#define CONFIG_SYS_MMC_ENV_DEV		1
#define CONFIG_SYS_MMC_ENV_PART		1
#define CONFIG_ENV_OFFSET		(4064 * 1024)
#define CONFIG_ENV_SIZE			(32 * 1024)

#endif			       /* __CONFIG_H * */
