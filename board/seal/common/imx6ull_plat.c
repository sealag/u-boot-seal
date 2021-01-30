// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2020 SEAL AG
 */

#include <common.h>
#include <env.h>
#include <led.h>
#include <asm/arch/clock.h>
#include <asm/arch/sys_proto.h>
#include <asm/gpio.h>
#include <asm/mach-imx/boot_mode.h>
#include <linux/delay.h>

DECLARE_GLOBAL_DATA_PTR;

int dram_init(void)
{
	gd->ram_size = imx_ddr_size();

	return 0;
}

/*
 * Ethernet
 */
static int setup_fec(void)
{
	struct iomuxc *const iomuxc_regs = (struct iomuxc *)IOMUXC_BASE_ADDR;
	int ret;

	clrsetbits_le32(&iomuxc_regs->gpr[1], IOMUX_GPR1_FEC1_MASK,
			IOMUX_GPR1_FEC1_CLOCK_MUX1_SEL_MASK);

	ret = enable_fec_anatop_clock(0, ENET_50MHZ);
	if (ret)
		return ret;

	enable_enet_clk(1);

	return 0;
}

static int set_leds(char *leds_string, enum led_state_t cmd)
{
	const char *list, *end;
	int node;
	int len;
	struct udevice *dev;

	node = fdt_path_offset(gd->fdt_blob, "/config");
	if (node < 0) {
		debug("%s: no /config node?\n", __func__);
		return -ENOENT;
	}

	list = fdt_getprop(gd->fdt_blob, node, leds_string, &len);
	if (!list)
		return -ENOENT;

	end = list + len;
	while (list < end) {
		len = strlen(list);
		if (!led_get_by_label(list, &dev)) {
			debug("%s: set %s to %d\n", __func__, list, cmd);
			led_set_state(dev, cmd);
		}

		list += (len + 1);
	}

	return 0;
}

enum boot_state {
	BOOT,
	FASTBOOT,
	USB_MASS_STORAGE,
};

static void led_indicate_boot_state(enum boot_state state)
{
	if (!IS_ENABLED(CONFIG_LED))
		return;

	switch (state) {
	case BOOT:
		set_leds("u-boot,fastboot-leds", LEDST_OFF);
		set_leds("u-boot,usb-mass-storage-leds", LEDST_OFF);
		set_leds("u-boot,boot-leds", LEDST_ON);
		break;
	case FASTBOOT:
		set_leds("u-boot,boot-leds", LEDST_OFF);
		set_leds("u-boot,usb-mass-storage-leds", LEDST_OFF);
		set_leds("u-boot,fastboot-leds", LEDST_ON);
		break;
	case USB_MASS_STORAGE:
		set_leds("u-boot,boot-leds", LEDST_OFF);
		set_leds("u-boot,fastboot-leds", LEDST_OFF);
		set_leds("u-boot,usb-mass-storage-leds", LEDST_ON);
		break;
	default:
		break;
	}
}

static void board_key_check(void)
{
	ofnode node;
	struct gpio_desc gpio;
	uint32_t key_hold_duration = 0; /* in ms */
	enum boot_state boot_selection = BOOT;

	if (!IS_ENABLED(CONFIG_FASTBOOT))
		return;

	node = ofnode_path("/config");
	if (!ofnode_valid(node)) {
		debug("%s: no /config node?\n", __func__);
		return;
	}

	if (gpio_request_by_name_nodev(node, "seal,boot-selection-gpios", 0,
				       &gpio, GPIOD_IS_IN)) {
		debug("%s: could not find a /config/seal,boot-selection-gpios\n",
		      __func__);
	} else {
		while (dm_gpio_get_value(&gpio)) {
			if (key_hold_duration == 0) {
				puts("detected key press, waiting for release to determine boot selection\n");
			}

			mdelay(10);
			key_hold_duration += 10;

			if (key_hold_duration < 1000) {
				boot_selection = BOOT;
			} else if (key_hold_duration < 2000) {
				boot_selection = FASTBOOT;
			} else {
				boot_selection = USB_MASS_STORAGE;
			}
			led_indicate_boot_state(boot_selection);
		}

		dm_gpio_free(NULL, &gpio);
	}

	switch (boot_selection) {
	case FASTBOOT:
		puts("entering fastboot mode...\n");
		env_set("preboot", "setenv preboot; fastboot usb 0");
		break;
	case USB_MASS_STORAGE:
		puts("entering USB mass storage mode...\n");
		env_set("preboot", "setenv preboot; ums 0 mmc 1");
		break;
	default:
		break;
	}
}

int board_init(void)
{
	/* Address of boot parameters */
	gd->bd->bi_boot_params = PHYS_SDRAM + 0x100;

	setup_fec();

	return 0;
}

#ifdef CONFIG_CMD_BMODE
static const struct boot_mode board_boot_modes[] = {
	/* 8 bit bus width */
	{"emmc", MAKE_CFGVAL(0x62, 0x48, 0x00, 0x00)},
	{NULL,	 0},
};
#endif

int board_late_init(void)
{
#ifdef CONFIG_CMD_BMODE
	add_board_boot_modes(board_boot_modes);
#endif

	led_indicate_boot_state(BOOT);
	board_key_check();

	return 0;
}
