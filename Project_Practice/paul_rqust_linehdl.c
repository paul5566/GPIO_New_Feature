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
#include "gpio.h"
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
