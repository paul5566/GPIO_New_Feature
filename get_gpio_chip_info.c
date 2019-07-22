#include <linux/gpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <asm/types.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/gpio.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <errno.h>

/* define GPIO node*/
#define GPIO0 "/dev/gpiochip0"
#define GPIO1 "/dev/gpiochip1"
#define GPIO2 "/dev/gpiochip2"
#define GPIO3 "/dev/gpiochip3"
#define GPIO4 "/dev/gpiochip4"
#define GPIO5 "/dev/gpiochip5"


/*
#define GPIOHANDLES_MAX 64
#define GPIOHANDLE_REQUEST_INPUT (1UL << 0)
#define GPIOHANDLE_REQUEST_OUTPUT (1UL << 1)
#define GPIOHANDLE_REQUEST_ACTIVE_LOW (1UL << 2)
#define GPIOHANDLE_REQUEST_OPEN_DRAIN (1UL << 3)
#define GPIOHANDLE_REQUEST_OPEN_SOURCE (1UL << 4)
*/


static void get_chip_info()
{
	struct gpiochip_info chip_info;
	int fd, rv;

	fd = open(GPIO0, O_RDWR);
	if (fd < 0)
	{
		printf("the open is failed\n");
		printf("errno id is %d\n",errno);
		close(fd);
		exit(0);
	}
	rv = ioctl(fd, GPIO_GET_CHIPINFO_IOCTL, &chip_info);
	printf("the fd is %d\n",fd);
	printf("the val of ioctl return is %d\n",rv);
	printf("the chipinfo.name is %s\n",chip_info.name);
	printf("the chipinfo.label is %s\n",chip_info.label);
	printf("the chipinfo.lines is %u\n",chip_info.lines);
	/* Don't forgat close the fd*/
	close(fd);
}




static void get_line_info(void)
{
	struct gpioline_info line_info;
	int fd, rv;

	fd = open(GPIO0, O_RDWR);
	if (fd < 0)
	{
		printf("the open is failed\n");
		printf("errno id is %d\n",errno);
		close(fd);
		exit(0);
	}

	memset(&line_info, 0, sizeof(line_info));
	line_info.line_offset = 5;
	rv = ioctl(fd, GPIO_GET_LINEINFO_IOCTL, &line_info);
	printf("the fd is %d\n",fd);
	printf("the val of ioctl return is %d\n",rv);
	printf("the line_info.offset is %u\n",line_info.line_offset);
	printf("the line_info.flags is %u\n",line_info.flags);
	printf("the line_info.name is %s\n",line_info.name);
	printf("the line_info.consumer is %s\n",line_info.consumer);
	/* Don't forgat close the fd*/
	close(fd);
}







static void request_output(void){
	int fd,rv;
	fd = open(GPIO1, O_RDWR);
	if (fd < 0)
	{
		printf("the open is failed\n");
		printf("errno id is %d\n",errno);
		close(fd);
		exit(0);
	}

	struct gpiohandle_request req; 
	req.flags |=  GPIOHANDLE_REQUEST_OUTPUT;
 	req.lines = 2;
	req.lineoffsets[0] = 3;
 	req.lineoffsets[1] = 5;
 	req.default_values[0] = 1;
 	req.default_values[1] = 0;
 	strcpy(req.consumer_label, "foobar");
	req.fd = fd;
//???? compile
/*
 struct gpiohandle_request req = {
	req.flags |=  GPIOHANDLE_REQUEST_OUTPUT,
 	req.lines = 2,
	req.lineoffsets[0] = 3,
	req.lineoffsets[1] = 5,
	req.default_values[0] = 1,
 	req.default_values[1] = 0
};

 */
 	//strcpy(req.consumer_label, "foobar");
	rv = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &req);
	printf("errno id is %d\n",errno);
	printf("the request output value is %d\n",rv);
	close(fd);
}

static void set_values(void)
{
	int fd,rv;
	fd = open(GPIO1, O_RDWR);
	if (fd < 0)
	{
		printf("the open is failed\n");
		printf("errno id is %d\n",errno);
		close(fd);
		exit(0);
	}

	struct gpiohandle_request req;

	req.flags |=  GPIOHANDLE_REQUEST_OUTPUT;
 	req.lines = 2;
	req.lineoffsets[0] = 3;
 	req.lineoffsets[1] = 5;
 	req.default_values[0] = 1;
 	req.default_values[1] = 0;
 	strcpy(req.consumer_label, "foobar");
	req.fd = fd;

	struct gpiohandle_data data;

	data.values[0] = 0;
	data.values[1] = 1;

	rv = ioctl(req.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);
	printf("errno id is %d\n",errno);
	printf("the set value is %d\n",rv);
	close(fd);
}



static void get_values(void)
{
	int fd,rv;

	fd = open(GPIO1, O_RDWR);
	if (fd < 0)
	{
		printf("the open is failed\n");
		printf("errno id is %d\n",errno);
		close(fd);
		exit(0);
	}

	struct gpiohandle_request req;

	req.flags |=  GPIOHANDLE_REQUEST_OUTPUT;
 	req.lines = 2;
	req.lineoffsets[0] = 3;
 	req.lineoffsets[1] = 5;
 	req.default_values[0] = 1;
 	req.default_values[1] = 0;
 	strcpy(req.consumer_label, "foobar");
	req.fd = fd;

 	struct gpiohandle_data data;
 
	memset(&data, 0, sizeof(data));
 	rv = ioctl(fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data);
	printf("errno id is %d\n",errno);
	printf("the set value is %d\n",rv);
	close(fd);
}



static void request_event(void)
{
	int fd,rv;

	fd = open(GPIO2, O_RDWR);
	if (fd < 0)
	{
		printf("the open is failed\n");
		printf("errno id is %d\n",errno);
		close(fd);
		exit(0);
	}

	struct gpioevent_request req;

	req.lineoffset = 4;
 	req.handleflags = GPIOHANDLE_REQUEST_INPUT;
 	req.eventflags = GPIOEVENT_REQUEST_BOTH_EDGES;
	req.fd = fd;
 	strcpy(req.consumer_label, "foobar");
 	rv = ioctl(fd, GPIO_GET_LINEEVENT_IOCTL, &req);
	printf("errno id is %d\n",errno);
	printf("the set value is %d\n",rv);
	close(fd);
}
/*
static void recv_event(void)
{
	int fd,rv;

	fd = open(GPIO2, O_RDWR);
	if (fd < 0)
	{
		printf("the open is failed\n");
		printf("errno id is %d\n",errno);
		close(fd);
		exit(0);
	}

	struct gpioevent_request req;

	req.lineoffset = 4;
 	req.handleflags = GPIOHANDLE_REQUEST_INPUT;
 	req.eventflags = GPIOEVENT_REQUEST_BOTH_EDGES;
	req.fd = fd;


	struct gpioevent_data event;
	struct pollfd pfd;
	ssize_t rd;
	pfd.fd = req.fd;
	pfd.events = POLLIN | POLLPRI;
	rv = poll(&pfd, 1, 1000);
	if (rv > 0)
 		rd = read(req.fd, &event, sizeof(event));
}
*/

int main(void)
{
	printf("***get gpiochip function***\n\n");
	get_chip_info();
	printf("\n***get line gpiochip function***\n\n");
	get_line_info();
	printf("\n***get line gpiochip function***\n\n");
	request_output();
	printf("\n***set value function***\n\n");
	set_values();
	printf("\n***get value function***\n\n");
	get_values();
	printf("\n***request event function***\n\n");
	request_event();
	return 0;
}
