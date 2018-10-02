/*
 * Copyright (C) 2018 SEAL AG
 *
 * Author: Filip Brozovic <fbrozovic@gmail.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <asm/arch/clock.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/iomux.h>
#include <asm/arch/mx6-pins.h>
#include <linux/errno.h>
#include <asm/gpio.h>
#include <asm/mach-imx/iomux-v3.h>
#include <asm/mach-imx/video.h>
#include <mmc.h>
#include <fsl_esdhc.h>
#include <asm/arch/crm_regs.h>
#include <asm/io.h>
#include <asm/arch/sys_proto.h>
#include <spl.h>
#include <ddr_spd.h>
#include <i2c.h>

DECLARE_GLOBAL_DATA_PTR;

#if defined(CONFIG_SPL_BUILD)
#include <asm/arch/mx6-ddr.h>

static struct mx6ul_iomux_ddr_regs mx6_ddr_ioregs = {
	.dram_dqm0 = 0x00000030,
	.dram_dqm1 = 0x00000030,
	.dram_ras = 0x00000030,
	.dram_cas = 0x00000030,
	.dram_odt0 = 0x00000030,
	.dram_odt1 = 0x00000030,
	.dram_sdba2 = 0x00000000,
	.dram_sdclk_0 = 0x00000008,
	.dram_sdqs0 = 0x00000038,
	.dram_sdqs1 = 0x00000030,
	.dram_reset = 0x00000030,
};

static struct mx6ul_iomux_grp_regs mx6_grp_ioregs = {
	.grp_addds = 0x00000030,
	.grp_ddrmode_ctl = 0x00020000,
	.grp_b0ds = 0x00000030,
	.grp_ctlds = 0x00000030,
	.grp_b1ds = 0x00000030,
	.grp_ddrpke = 0x00000000,
	.grp_ddrmode = 0x00020000,
	.grp_ddr_type = 0x000c0000,
};

/* MT41K64M16TW-107 */
static struct mx6_ddr3_cfg mt41k64m16tw = {
	.mem_speed = 1866,
	.density = 1,
	.width = 16,
	.banks = 8,
	.rowaddr = 13,
	.coladdr = 10,
	.pagesz = 2,
	.trcd = 1391,
	.trcmin = 4791,
	.trasmin = 3400,
};

/*
 * use reset values here - we perform an MMDC calibration on boot
 * which automatically fills in the correct values
 */
static struct mx6_mmdc_calibration mx6_mmdc_calib = {
	.p0_mpwldectrl0 = 0,
	.p0_mpdgctrl0 = 0,
	.p0_mprddlctl = 0x40404040,
	.p0_mpwrdlctl = 0x40404040,
};

/* 16-bit wide DDR3 bus */
static struct mx6_ddr_sysinfo mem_s = {
	.dsize		= 0,
	.cs1_mirror	= 0,
	.cs_density	= 20,
	.ncs		= 1,
	.bi_on		= 1,
	.rtt_nom	= 1,
	.rtt_wr		= 2,
	.ralat		= 5,
	.walat		= 0,
	.mif3_mode	= 3,
	.rst_to_cke	= 0x23,
	.sde_to_rst	= 0x10,
	.ddr_type	= DDR_TYPE_DDR3,
};

static void ccgr_init(void)
{
	struct mxc_ccm_reg *ccm = (struct mxc_ccm_reg *)CCM_BASE_ADDR;

	writel(0xFFFFFFFF, &ccm->CCGR0);
	writel(0xFFFFFFFF, &ccm->CCGR1);
	writel(0xFFFFFFFF, &ccm->CCGR2);
	writel(0xFFFFFFFF, &ccm->CCGR3);
	writel(0xFFFFFFFF, &ccm->CCGR4);
	writel(0xFFFFFFFF, &ccm->CCGR5);
	writel(0xFFFFFFFF, &ccm->CCGR6);
}

static void spl_dram_init(void)
{
	mx6ul_dram_iocfg(mt41k64m16tw.width, &mx6_ddr_ioregs, &mx6_grp_ioregs);
	mx6_dram_cfg(&mem_s, &mx6_mmdc_calib, &mt41k64m16tw);

	/* Perform DDR DRAM calibration */
	udelay(100);
	mmdc_do_write_level_calibration(&mem_s);
	mmdc_do_dqs_calibration(&mem_s);
}

void board_init_f(ulong dummy)
{
	ccgr_init();

	/* setup AIPS and disable watchdog */
	arch_cpu_init();

	/* iomux */
	board_early_init_f();

	/* setup GP timer */
	timer_init();

	/* UART clocks enabled and gd valid - init serial console */
	preloader_console_init();

	/* DDR initialization */
	spl_dram_init();

	/* Clear the BSS. */
	memset(__bss_start, 0, __bss_end - __bss_start);

	/* load/boot image from boot device */
	board_init_r(NULL, 0);
}
#endif

