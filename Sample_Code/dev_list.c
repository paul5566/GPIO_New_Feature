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

static int LIST_DEV_NODE(DIR *dp,const struct dirent *ent){
	int ret_val;

	dp = opendir("/dev");
	if (!dp) {
		ret_val = -errno;
		printf("opendir failed");
		return ret_val;
	}
	while (ent = readdir(dp)) {
		printf("readdir sucessfully\n\n");
		printf("dev name = %s\n", ent->d_name);
	}
	return 0;
}

int main(int argc, char **argv)
{
	int ret;
	int count_chip = 0;
	const struct dirent *ent;
	DIR *dp;


	ret = LIST_DEV_NODE(dp,ent);
	//error handling???
	if (ret < 0){
		printf(" Value of errno: %d\n ", ret);
	}
	printf("opendir sucessfully\n");
}
