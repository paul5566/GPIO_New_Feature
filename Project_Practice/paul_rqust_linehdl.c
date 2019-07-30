//asprintf function
//#define _GNU_SOURCE

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
#include <asm/types.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <errno.h>

//define GPIO device
#define GPIO0 "gpiochip0"
#define GPIO1 "gpiochip1"
#define GPIO2 "gpiochip2"
#define GPIO3 "gpiochip3"
#define GPIO4 "gpiochip4"
#define GPIO5 "gpiochip5"
/*define COMSUMER*/
#define COMSUMER "PAUL_TEST"

enum direction {
	OUT = 0,
	IN = 1
};


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

	for (i = 0; i < nlines; i++)	{
		req.lineoffsets[i] = lines[i];
		printf("req.lineoffsets[%d] is %d\n",i,lines[i]);
	}
	req.flags = flag;
	strcpy(req.consumer_label, consumer_label);
	req.lines = nlines;
	if (flag & GPIOHANDLE_REQUEST_OUTPUT){
		memcpy(req.default_values, data, sizeof(req.default_values));
		//printf("memcopy sucessdully\n");
		//printf("the req.default_values is %hhn\n",req.default_values);
	}
	//printf("***executed the IOCTL in fuction:%s***\n",__func__);
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




int main(void){

	struct gpiohandle_data data;
	/*lines stored the line of the chip to request*/
	unsigned int lines[32] = {15};
	int fd;
	int rv;
	//gpiochip target:
	char *targetchip = "gpiochip2";
	memset(&data.values, 0, sizeof(data.values));
	//gpiotools_requeshandle("chip name,line of the chip to request...")
	rv = gpiotools_request_linehandle(targetchip, lines, 1,
				GPIOHANDLE_REQUEST_OUTPUT,&data,COMSUMER);
	if (rv < 0){
		printf("REQUEST LINEHANDLE FAILED\n");
		//close(fd); it close in the gpiotools_request_linehandle
	}
	else{
		fd = rv;
		printf("REQUEST LINEHANDLE SUCESSFULLY\n");
	}
	/*Using lsgpio to check*/
	system("./lsgpio");
	//sleep(15);

	//get value
	rv = gpiotools_get_values(fd, &data);
	//error handling of gpiotools_get_values
	if(rv == 0){
		printf("GET VALUES SUCESSFULLY\n");
		fd = rv;
	}
	else {
		printf("Failed to GET VALYES to %s with errno %d\n",targetchip,rv);
		close(fd);
	}
	//gpiotools_release_linehandle
	rv = gpiotools_release_linehandle(fd);
	//error handling of release_linehandle
	if(rv == 0)
		printf("RELEASE LINEHANDLE SUCESSFULLY\n");
	else {
		printf("Failed to Release Linehandle of the %s with errno %d\n",targetchip,rv);
		close(fd);
	}
	return 0;
}