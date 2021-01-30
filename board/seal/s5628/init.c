// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2020 SEAL AG
 */

#include <common.h>
#include <env.h>
#include <fuse.h>
#include <asm/gpio.h>
#include <power/regulator.h>

#define POWER_ON_N_GPIO IMX_GPIO_NR(1, 20)
#define M2_POWER_OFF	IMX_GPIO_NR(1, 3)
#define M2_RESET	IMX_GPIO_NR(1, 5)
#define M_SUPPLY_PRESENT IMX_GPIO_NR(4, 22)

int board_early_init_f(void)
{
	gpio_request(POWER_ON_N_GPIO, "power_on_n");
	gpio_direction_output(POWER_ON_N_GPIO, 0);

	return 0;
}

int board_early_init_r(void)
{
	struct udevice *regulator;
	int ret;

	ret = regulator_get_by_platname("m2_vcc", &regulator);
	if (ret) {
		printf("%s: failed to get m2_vcc regulator (%d)\n", __func__, ret);
		goto out;
	}

	ret = regulator_set_enable(regulator, true);
	if (ret)
		printf("%s: failed to enable m2_vcc regulator (%d)\n", __func__, ret);

	gpio_request(M_SUPPLY_PRESENT, "m_supply_present");
	gpio_direction_input(M_SUPPLY_PRESENT);
	if (!gpio_get_value(M_SUPPLY_PRESENT)) {
		gpio_request(M2_POWER_OFF, "m2_power_off");
		gpio_direction_output(M2_POWER_OFF, 0);
		gpio_request(M2_RESET, "m2_reset");
		gpio_direction_output(M2_RESET, 0);
	}

out:
	return 0;
}

int misc_init_r(void)
{
	char sn[9] = {0};

	fuse_sense(4, 6, (uint32_t *)sn);
	fuse_sense(4, 7, (uint32_t *)(sn + 4));

	printf("S/N:   %s\n", sn);
	env_set("serial#", sn);

	return 0;
}
