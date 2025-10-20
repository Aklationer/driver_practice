#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include <string.h>
#include <errno.h>

int main(void) {
    struct pollfd pfd;

	int fd = open("/dev/gpio_pull");

    pfd.fd = fd;          
    pfd.events = POLLIN;


    printf("wait poll\n");
    poll(&pfd, 1, -1);
	printf("End Bye Bye\n");
    return 0;
}