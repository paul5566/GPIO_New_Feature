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


/*
 * Define the Data Type of the node
*/
struct node {
	const char *gpio_chip_name;
	struct node *next;
	int gpio_line[32];
};


#if 0
struct gpio_flag {
	char *name;
	unsigned long mask;
};
/*
 *	Define The Flag
 */
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
#endif

static void print_all(struct node *head)
{
	struct node *current;

	current = head;
	while(current != NULL)//current->value != NULL the last point would not print out
	{
		//skip the head node
		if(current-> gpio_chip_name){
			printf("device name:%s\n",current-> gpio_chip_name);
			for(int i = 0; i < 32; i++)
			{
				printf("%s:line[%d]= %d\n",current->gpio_chip_name,i,current->gpio_line[i]);
			}
		}
		current = current->next;
	}
}

static struct node *find_tail(struct node *head)
{
	struct node *current;

	current = head;
	while(current->next != NULL)
	{
		current = current->next;
	}
	return current;
}

/*
	create new node

	*****Be careful*****
	Create Node Error Handling
*/

static struct node *create_new_node(struct node *head, const char*device_name)
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
    current_tail = find_tail(head);
    current_tail->next = new_node;
	new_node->next = NULL;
	printf("create new node chipname is %s\n",device_name);
	return new_node;
}

#if 0
/*
 *	Stored The Array[32] to linked list in this program
 */

int INFO_STORE(const char *device_name,struct node *info_store)
{
	struct gpiochip_info cinfo;
	char *chrdev_name;
	int fd;
	int i;
	int ret;
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
		perror("Failed to issue CHIPINFO IOCTL\n");
		goto exit_close_error;
	}
	fprintf(stdout, "GPIO chip: %s, \"%s\", %u GPIO lines\n",
		cinfo.name, cinfo.label, cinfo.lines);

	/* Loop over the lines and print info */
	for (i = 0; i < cinfo.lines; i++) {
		struct gpioline_info linfo;

		memset(&linfo, 0, sizeof(linfo));
		linfo.line_offset = i;

		ret = ioctl(fd, GPIO_GET_LINEINFO_IOCTL, &linfo);
		if (ret == -1) {
			ret = -errno;
	
			perror("Failed to issue LINEINFO IOCTL\n");
			goto exit_close_error;
		}
		fprintf(stdout, "\tline %2d:", linfo.line_offset);
		if (linfo.name[0])
			fprintf(stdout, " \"%s\"", linfo.name);
		else
			fprintf(stdout, " unnamed");
		//line info used/unused
		if (linfo.consumer[0]){
			fprintf(stdout, " \"%s\"", linfo.consumer);
			info_store->gpio_line[i] = 1;
		}
		else{
			fprintf(stdout, " unused");
			info_store->gpio_line[i] = 0;
		}
		if (linfo.flags) {
			fprintf(stdout, " [");
			print_flags(linfo.flags);
			fprintf(stdout, "]");
		}
		fprintf(stdout, "\n");
	}

exit_close_error:
	if (close(fd) == -1)
		perror("Failed to close GPIO character device file");
	free(chrdev_name);
	return ret;
}

/*
 * Search the device under the dev
 *
 * @Sucessfully: return 0
 *
 * @crate new node failed: return -1
 *
 * @No device found in /dev:retirn -2
 */
static int LIST_DEV_NODE(struct node *head)
{
	/*Search Device under /dev*/
	const struct dirent *ent;
	DIR *dp;
	int ret_val;
	int dev_count;
	int gpiochip_count = 0;
	struct node *cur_dev_node;

	dp = opendir("/dev");
	if (!dp) {
		ret_val = -errno;
		printf("opendir failed");
		return ret_val;
	}
	//if the gpiochip is already stored in the array
	printf("opendir sucessfully\n");
	while (ent = readdir(dp),ent) {
		dev_count++;
		if (check_prefix(ent->d_name, "gpiochip")) {
			printf("create an linkedlist for %s\n",ent->d_name);
			//create linkedlist,error handling
			cur_dev_node = create_new_node(head,ent->d_name,gpiochip_count);
			if(NULL == cur_dev_node){
				return -1;
			}
			//fill gpiochip information
			INFO_STORE(ent->d_name,cur_dev_node);
			gpiochip_count++;
		}
	}
	//error handling for if dev_e didn't find any device under /dev
	if (dev_count == 0)
	{
		printf("The searching for device may have the problem\n");
		return -2;
	}
	printf("Total numer of the gpiochip under /dev is %d\n",gpiochip_count);
	return 0;
}
#endif

static int list_and_store_device(const char *name, struct node *head)
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
	n = create_new_node(head, device_name);
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

	if (close(fd) == -1)
		perror("Failed to close GPIO character device file");
	free(chrdev_name);

	return ret;
}

static int store_status(struct node *head)
{
	int ret;
	const struct dirent *ent;
	DIR *dp;

	/* List all GPIO devices one at a time */
	dp = opendir("/dev");
	if (!dp) {
		printf("Open /dev dir: %s\n", strerr(errno));
		return -1;
	}

	while (ent = readdir(dp), ent) {
		if (check_prefix(ent->d_name, "gpiochip")) {
				ret = list_and_store_device(ent->d_name, head);
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
	struct node head;

	/* First, stoed all the GPIO chip/line status in linkedlist */
	ret = store_status(&head);
	if (ret)
		return -1;

	print_all_status(&head);
	remove_all(&head);

	return 0;
}

#if 0
int main(int argc, char **argv)
{
	//create linkedlist
	/*
	   Build up the head node,without malloc
	*/
	int ret;
	struct node *head = malloc(sizeof(struct node));//declare a pointer point to the type struct

	if (head == NULL){
		return -1;
		goto memory_allocate_error;
	}
	ret = LIST_DEV_NODE(head);
	//error handling???
	if (ret < 0){
		if(-1 == ret)
			goto faied_created_node;
		else if(-2 == ret)
			goto no_device_find;
	}
	printf("opendir sucessfully\n");
	print_all(head);
	return 0;

faied_created_node:
	printf("Failed to close GPIO character device file\n");
	return -1;

no_device_find:
	printf("Failed to close GPIO character device file\n");
	return -2;

memory_allocate_error:
	printf("Failed to close GPIO character device file\n");
	return -3;
}
#endif
