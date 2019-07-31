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







int main(void)
{
	printf("***get gpiochip function***\n\n");
	get_chip_info();
	printf("\n***get line gpiochip function***\n\n");
	get_line_info();
	return 0;
}
