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
#include <string.h>
#include "gpio-utils.h"


/*
 * Define the Data Type of the node
*/
struct node {
	const char *gpio_chip_name;
	struct node *next;
	int gpio_line[32];
};

static void remove_all(struct node *h)
{
	struct node *current = h->next;
	struct node *next = current->next;

	while(current && next)//current->value != NULL the last point would not print out
	{
		next = current->next;
		free(current);
		current = next;
	}
}
/*
 *	Check the gpiochip line is used/unused
 */
static int check_line_used(struct node *h,const char* dev_name,int line_no)
{
	struct node *crnt = h->next;


	while(crnt)
	{
		if( strncmp(dev_name, crnt->gpio_chip_name,9) ){
			crnt = crnt->next;
		}
		else{
			if(crnt->gpio_line[line_no] == 0)
				return 0;
			else
				return -1;
		}
	}
	return -2;//the device_name is not matched
}

static void print_all_status(struct node *h)
{
	struct node *current;

	current = h;
	while(current)//current->value != NULL the last point would not print out
	{
		if(current-> gpio_chip_name){
			printf("device name:%s\n",current-> gpio_chip_name);
			for(int i = 0; i < 32; i++)
				printf("%s:line[%d]= %d\n",current->gpio_chip_name,i,current->gpio_line[i]);
		}
		current = current->next;
	}
}

static struct node *find_tail(struct node *h)
{
	struct node *current;

	current = h;
	while(current->next != NULL)
	{
		current = current->next;
	}
	return current;
}

static struct node *create_new_node(struct node *h, const char*device_name)
{
	struct node *new_node;
    struct node *current_tail;

    new_node = malloc(sizeof(struct node));
	if (new_node == NULL)
	{
		printf("the memory allocation failed\n");
		return NULL;
	}
	new_node->gpio_chip_name = device_name;
    current_tail = find_tail(h);
    current_tail->next = new_node;
	new_node->next = NULL;
	printf("create new node chipname is %s\n",device_name);
	return new_node;
}


static int list_and_store_device(const char *device_name, struct node *h)
{
	struct gpiochip_info cinfo;
	char *chrdev_name;
	int fd;
	int ret;
	int i;
	struct node *n;

	ret = asprintf(&chrdev_name, "/dev/%s", device_name);
	if (ret < 0)
		return -ENOMEM;

	fd = open(chrdev_name, 0);
	if (fd == -1) {
		ret = -errno;
		fprintf(stderr, "Failed to open %s\n", chrdev_name);
		goto exit_close_error;
	}

	/* Inspect this GPIO chip */
	ret = ioctl(fd, GPIO_GET_CHIPINFO_IOCTL, &cinfo);
	if (ret == -1) {
		ret = -errno;
			goto exit_close_error;
		perror("Failed to issue CHIPINFO IOCTL\n");
		goto exit_close_error;
	}

	fprintf(stdout, "GPIO chip: %s, \"%s\", %u GPIO lines\n",
		cinfo.name, cinfo.label, cinfo.lines);

	// Get the GPIO chip info succes here,
	// sure things, malloc the linkedlist node
	n = create_new_node(h, device_name);
	if (!n)
		return -1;

	/* Loop over the lines and print info */
	for (i = 0; i < cinfo.lines; i++) {
		struct gpioline_info linfo;

		memset(&linfo, 0, sizeof(linfo));
		linfo.line_offset = i;

		ret = ioctl(fd, GPIO_GET_LINEINFO_IOCTL, &linfo);
		if (ret == -1) {
			ret = -errno;
			perror("Failed to issue LINEINFO IOCTL\n");
			// assume -1 is invalid, ioctl failed
			n->gpio_line[i] = -1;
			continue;
		}

		if (linfo.consumer[0])
			n->gpio_line[i] = 1;
		else
			n->gpio_line[i] = 0;
	}
exit_close_error:
	if (close(fd) == -1)
		perror("Failed to close GPIO character device file");
	free(chrdev_name);

	return ret;
}

static int store_status(struct node *h)
{
	int ret;
	const struct dirent *ent;
	DIR *dp;

	/* List all GPIO devices one at a time */
	dp = opendir("/dev");
	if (!dp) {
		printf("Open /dev dir: %s\n", strerror(errno));
		return -1;
	}

	while (ent = readdir(dp), ent) {
		if (check_prefix(ent->d_name, "gpiochip")) {
				ret = list_and_store_device(ent->d_name, h);
				if (ret)
						break;
		}
	}
	if (closedir(dp) == -1) {
			perror("scanning devices: Failed to close directory");
			ret = -errno;
	}
	return ret;
}

int main(int argc, char **argv)
{
	int ret;
	int check;
	struct node head;
	//test

	/* First, stoed all the GPIO chip/line status in linkedlist */
	ret = store_status(&head);
	if (ret)
		return -1;

	check = check_line_used(&head,"gpiochip0",3);
	if(check < 0){
		if(-2 == check){
			printf("no such a device\n");
			return -1;
		}
		else{
			printf("the line is used\n");
			return -2;
		}
	}
	printf("line is not used\n");
	//print_all_status(&head);
	remove_all(&head);
	return 0;
}
