#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

int main(){

    int fd = open("/dev/hello0", O_RDWR);
    printf("%d",fd);
    close(fd);
}