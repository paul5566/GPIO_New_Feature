/*
 * gpio-hammer - example swiss army knife to shake GPIO lines on a system
 *
 * Copyright (C) 2016 Linus Walleij
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * Usage:
 *	gpio-hammer -n <device-name> -o <offset1> -o <offset2>
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

/**
 * doc: Operation of gpio
 *
 * Provide the api of gpiochip for chardev interface. There are two
 * types of api.  The first one provide as same function as each
 * ioctl, including request and release for lines of gpio, read/write
 * the value of gpio. If the user want to do lots of read and write of
 * lines of gpio, user should use this type of api.
 *
 * The second one provide the easy to use api for user. Each of the
 * following api will request gpio lines, do the operation and then
 * release these lines.
 */
/**
 * gpiotools_request_linehandle() - request gpio lines in a gpiochip
 * @device_name:	The name of gpiochip without prefix "/dev/",
 *			such as "gpiochip0"
 * @lines:		An array desired lines, specified by offset
 *			index for the associated GPIO device.
 * @nline:		The number of lines to request.
 * @flag:		The new flag for requsted gpio. Reference
 *			"linux/gpio.h" for the meaning of flag.
 * @data:		Default value will be set to gpio when flag is
 *			GPIOHANDLE_REQUEST_OUTPUT.
 * @consumer_label:	The name of consumer, such as "sysfs",
 *			"powerkey". This is useful for other users to
 *			know who is using.
 *
 * Request gpio lines through the ioctl provided by chardev. User
 * could call gpiotools_set_values() and gpiotools_get_values() to
 * read and write respectively through the returned fd. Call
 * gpiotools_release_linehandle() to release these lines after that.
 *
 * Return:		On success return the fd;
 *			On failure return the errno.
 */
/*
 * Define the Data Type of the node
*/
struct node {
	const char *gpio_chip_name;
	struct node *next;
	int gpio_line[32];
};

static void remove_all(struct node *head)
{
	struct node *current = head->next;
	struct node *next = current->next;

	while(current && next)//current->value != NULL the last point would not print out
	{
		next = current->next;
		free(current);
		current = next;
	}
}


static void print_all_status(struct node *head)
{
	struct node *current;

	current = head;
	while(current)//current->value != NULL the last point would not print out
	{
		//skip the head node
		if(current-> gpio_chip_name){
			printf("device name:%s\n",current-> gpio_chip_name);
			for(int i = 0; i < 32; i++)
				printf("%s:line[%d]= %d\n",current->gpio_chip_name,i,current->gpio_line[i]);
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


static int list_and_store_device(const char *device_name, struct node *head)
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
exit_close_error:
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
		printf("Open /dev dir: %s\n", strerror(errno));
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





int gpiotools_request_linehandle(const char *device_name, unsigned int *lines,
				 unsigned int nlines, unsigned int flag,
				 struct gpiohandle_data *data,
				 const char *consumer_label)
{
	struct gpiohandle_request req;
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

	for (i = 0; i < nlines; i++)
		req.lineoffsets[i] = lines[i];

	req.flags = flag;
	strcpy(req.consumer_label, consumer_label);
	req.lines = nlines;
	if (flag & GPIOHANDLE_REQUEST_OUTPUT)
		memcpy(req.default_values, data, sizeof(req.default_values));

	ret = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &req);
	if (ret == -1) {
		ret = -errno;
		fprintf(stderr, "Failed to issue GET LINEHANDLE IOCTL (%d)\n",
			ret);
	}

exit_close_error:
	if (close(fd) == -1)
		perror("Failed to close GPIO character device file");
	free(chrdev_name);
	return ret < 0 ? ret : req.fd;
}

/**
 * gpiotools_set_values(): Set the value of gpio(s)
 * @fd:			The fd returned by
 *			gpiotools_request_linehandle().
 * @data:		The array of values want to set.
 *
 * Return:		On success return 0;
 *			On failure return the errno.
 */
int gpiotools_set_values(const int fd, struct gpiohandle_data *data)
{
	int ret;

	ret = ioctl(fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, data);
	if (ret == -1) {
		ret = -errno;
		fprintf(stderr, "Failed to issue %s (%d)\n",
			"GPIOHANDLE_SET_LINE_VALUES_IOCTL", ret);
	}

	return ret;
}
/**
 * gpiotools_get_values(): Get the value of gpio(s)
 * @fd:			The fd returned by
 *			gpiotools_request_linehandle().
 * @data:		The array of values get from hardware.
 *
 * Return:		On success return 0;
 *			On failure return the errno.
 */
int gpiotools_get_values(const int fd, struct gpiohandle_data *data)
{
	int ret;

	ret = ioctl(fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, data);
	if (ret == -1) {
		ret = -errno;
		fprintf(stderr, "Failed to issue %s (%d)\n",
			"GPIOHANDLE_GET_LINE_VALUES_IOCTL", ret);
	}

	return ret;
}
/**
 * gpiotools_release_linehandle(): Release the line(s) of gpiochip
 * @fd:			The fd returned by
 *			gpiotools_request_linehandle().
 *
 * Return:		On success return 0;
 *			On failure return the errno.
 */
int gpiotools_release_linehandle(const int fd)
{
	int ret;

	ret = close(fd);
	if (ret == -1) {
		perror("Failed to close GPIO LINEHANDLE device file");
		ret = -errno;
	}

	return ret;
}


int hammer_device(const char *device_name, unsigned int *lines, int nlines,
		  unsigned int loops)
{
	struct gpiohandle_data data;
	char swirr[] = "-\\|/";
	int fd;
	int ret;
	int i, j;
	unsigned int iteration = 0;

	memset(&data.values, 0, sizeof(data.values));
	ret = gpiotools_request_linehandle(device_name, lines, nlines,
					   GPIOHANDLE_REQUEST_OUTPUT, &data,
					   "gpio-hammer");
	if (ret < 0)
		goto exit_error;
	else
		fd = ret;

	ret = gpiotools_get_values(fd, &data);
	if (ret < 0)
		goto exit_close_error;

	fprintf(stdout, "Hammer lines [");
	for (i = 0; i < nlines; i++) {
		fprintf(stdout, "%d", lines[i]);
		if (i != (nlines - 1))
			fprintf(stdout, ", ");
	}
	fprintf(stdout, "] on %s, initial states: [", device_name);
	for (i = 0; i < nlines; i++) {
		fprintf(stdout, "%d", data.values[i]);
		if (i != (nlines - 1))
			fprintf(stdout, ", ");
	}
	fprintf(stdout, "]\n");

	/* Hammertime! */
	j = 0;
	while (1) {
		/* Invert all lines so we blink */
		for (i = 0; i < nlines; i++)
			data.values[i] = !data.values[i];

		ret = gpiotools_set_values(fd, &data);
		if (ret < 0)
			goto exit_close_error;

		/* Re-read values to get status */
		ret = gpiotools_get_values(fd, &data);
		if (ret < 0)
			goto exit_close_error;

		fprintf(stdout, "[%c] ", swirr[j]);
		j++;
		if (j == sizeof(swirr)-1)
			j = 0;

		fprintf(stdout, "[");
		for (i = 0; i < nlines; i++) {
			fprintf(stdout, "%d: %d", lines[i], data.values[i]);
			if (i != (nlines - 1))
				fprintf(stdout, ", ");
		}
		fprintf(stdout, "]\r");
		fflush(stdout);
		sleep(1);
		iteration++;
		if (loops && iteration == loops)
			break;
	}
	fprintf(stdout, "\n");
	ret = 0;

exit_close_error:
	gpiotools_release_linehandle(fd);
exit_error:
	return ret;
}

void print_usage(void)
{
	fprintf(stderr, "Usage: gpio-hammer [options]...\n"
		"Hammer GPIO lines, 0->1->0->1...\n"
		"  -n <name>  Hammer GPIOs on a named device (must be stated)\n"
		"  -o <n>     Offset[s] to hammer, at least one, several can be stated\n"
		" [-c <n>]    Do <n> loops (optional, infinite loop if not stated)\n"
		"  -?         This helptext\n"
		"\n"
		"Example:\n"
		"gpio-hammer -n gpiochip0 -o 4\n"
	);
}

int main(int argc, char **argv)
{



	int ret;
	struct node head;

	/* First, stoed all the GPIO chip/line status in linkedlist */
	ret = store_status(&head);
	if (ret)
		return -1;

	const char *device_name = NULL;
	unsigned int lines[GPIOHANDLES_MAX];
	unsigned int loops = 0;
	int nlines;
	int c;
	int i;

	i = 0;
	while ((c = getopt(argc, argv, "c:n:o:?")) != -1) {
		switch (c) {
		case 'c':
			loops = strtoul(optarg, NULL, 10);
			break;
		case 'n':
			device_name = optarg;
			break;
		case 'o':
			lines[i] = strtoul(optarg, NULL, 10);
			i++;
			break;
		case '?':
			print_usage();
			return -1;
		}
	}
	nlines = i;

	if (!device_name || !nlines) {
		print_usage();
		return -1;
	}
	return hammer_device(device_name, lines, nlines, loops);
	remove_all(&head);
}
