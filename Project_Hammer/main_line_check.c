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

static int gpio_chip1_ary[32] = {0};
const char *gpio_chiplist[8];

/*
   build up the function 
*/
struct node {
	int gpio_chip_no;
	struct node *next;
	int gpio_line[32];
};


struct gpio_flag {
	char *name;
	unsigned long mask;
};

struct gpio_flag flagnames[] = {
	{
		.name = "kernel",
		.mask = GPIOLINE_FLAG_KERNEL,
	},
	{
		.name = "output",
		.mask = GPIOLINE_FLAG_IS_OUT,
	},
	{
		.name = "active-low",
		.mask = GPIOLINE_FLAG_ACTIVE_LOW,
	},
	{
		.name = "open-drain",
		.mask = GPIOLINE_FLAG_OPEN_DRAIN,
	},
	{
		.name = "open-source",
		.mask = GPIOLINE_FLAG_OPEN_SOURCE,
	},
};

void print_flags(unsigned long flags)
{
	int i;
	int printed = 0;

	for (i = 0; i < ARRAY_SIZE(flagnames); i++) {
		if (flags & flagnames[i].mask) {
			if (printed)
				fprintf(stdout, " ");
			fprintf(stdout, "%s", flagnames[i].name);
			printed++;
		}
	}
}


/*
	create new node

	*****Be careful*****
	Create Node Error Handling
*/
/*
void create_new_node(struct node *head,int val){

	struct node *new_node;
    struct node *current_tail;

    new_node = malloc(sizeof(struct node));
	if (head == NULL)
	{
		printf("the memory allocation failed\n");
	}
    new_node->gpio_chip_no = val;
    current_tail = find_tail(head);
    current_tail->next = new_node;
    new_node->next = NULL;
}
*/

/*
 * Search the device under the dev
 */
static int LIST_DEV_NODE(DIR *dp,const struct dirent *ent){
	int ret_val;
	int gpiochip_count = 0;
	dp = opendir("/dev");
	if (!dp) {
		ret_val = -errno;
		printf("opendir failed");
		return ret_val;
	}
	//error handling for ent????
	while (ent = readdir(dp)) {
		//printf("readdir sucessfully\n\n");
		if (check_prefix(ent->d_name, "gpiochip")) {
			//device_name = ent->d_name;
			//gpio_chiplist[count_chip] = device_name;
			printf("create an linkedlist for %s\n",ent->d_name);
			gpiochip_count++;
		}
		else{

		}

	}
	printf("Total of the gpiochip is %d\n",gpiochip_count);
	return 0;
}



/*
void print_usage(void)
{
	fprintf(stderr, "Usage: lsgpio [options]...\n"
		"List GPIO chips, lines and states\n"
		"  -n <name>  List GPIOs on a named device\n"
		"  -?         This helptext\n"
	);
}
*/
int main(int argc, char **argv)
{
	/*Search Device under /dev*/
	int ret;
	//int count_chip = 0;
	const struct dirent *ent;
	DIR *dp;


	ret = LIST_DEV_NODE(dp,ent);
	//error handling???
	if (ret < 0){
		printf(" Value of errno: %d\n ", ret);
	}
	printf("opendir sucessfully\n");
}