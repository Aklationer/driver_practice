#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main() 
{
	int fd;
	char c;

	fd = open("/dev/hello0", O_RDWR);

	if (fd < 0) {
		perror("open");
		return fd;
	}

	const char *msg = "ABCDEF";
	ssize_t w = write(fd, msg, strlen(msg));
    if (w < 0) { 
		perror("write"); 
		return fd; 
	}
	lseek(fd, 0, SEEK_SET);  //把offset設為0
	while (read(fd, &c, 1)) 
		putchar(c);

	close(fd);
	return 0;
}