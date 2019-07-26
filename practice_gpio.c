//asprintf funxtion
#define _GNU_SOURCE


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

//define GPIO device
#define GPIO0 "/dev/gpiochip0"
#define GPIO1 "/dev/gpiochip1"
#define GPIO2 "/dev/gpiochip2"
#define GPIO3 "/dev/gpiochip3"
#define GPIO4 "/dev/gpiochip4"
#define GPIO5 "/dev/gpiochip5"
/*define COMSUMER*/
#define COMSUMER "gpio-utils"

enum direction {
	OUT = 0,
	IN = 1
};



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

	ret = asprintf(&chrdev_name, "%s", device_name);
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



int main(void){
	//int val_gpiotools_gets;



	//struct gpiochip_info chip_info;
	//struct gpioline_info line_info;
	struct gpiohandle_data data;
	unsigned int lines[] = {0};
	int fd;
	int debugfs_dir = IN;
	int debugfs_value = 0;
	int ret;

	// Call gpiotools_request_linehandle()
	ret = gpiotools_request_linehandle(GPIO0,  lines, 1, \
			GPIOHANDLE_REQUEST_OUTPUT,&data,COMSUMER);
	if (ret < 0)
		goto fail_out;
	else
		fd = ret;
	printf("the gpiotools_request_linehandle return value is %d\n",ret);
	close(fd);
	return 0;
fail_out:
	if (ret)
		printf("FAILED\n");
	return 0;
}
