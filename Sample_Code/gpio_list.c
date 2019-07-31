/*
 * lsgpio - example on how to list the GPIO lines on a system
 *
 * Copyright (C) 2015 Linus Walleij
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * Usage:
 *	lsgpio <-n device-name>
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <poll.h>
#include <fcntl.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include <linux/gpio.h>

#include "gpio-utils.h"

struct gpio_flag {
	char *name;
	unsigned long mask;
};
const char *gpio_chiplist[8];
int main(int argc, char **argv)
{
	const char *device_name = NULL;
	int ret;
	int count_chip = 0;
	const struct dirent *ent;
	DIR *dp;

	/* List all GPIO devices one at a time */
	dp = opendir("/dev");
	if (!dp) {
			ret = -errno;
			goto error_out;
		}
		ret = -ENOENT;
		//error handling???
	printf("opendir sucessfully\n");

	while (ent = readdir(dp), ent) {
		printf("readdir sucessfully\n");
		if (check_prefix(ent->d_name, "gpiochip")) {
			device_name = ent->d_name;
			gpio_chiplist[count_chip] = device_name;
			count_chip++;
		}
		else{
			printf("No Chip is matched\n");
			break;
		}
	}
	for(int i = 0;i < count_chip;i++)	{
		printf("%s\n",gpio_chiplist[i]);
	}
	ret = 0;
	if (closedir(dp) == -1) {
		perror("scanning devices: Failed to close directory");
		ret = -errno;
	}
error_out:
	return ret;
}
