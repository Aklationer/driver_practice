#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>

#define DEVNAME "/dev/mydev"

int main(int argc, char **argv){

    char text[4096];
	void *ptr;
    int status;

    if(argc < 2)
        return 0;

    int fd = open(DEVNAME,O_RDWR);

    if(fd < 0) {
		perror("open");
		return 1;
	}

    switch(argv[1][0]){
        case 'r':
            status = read(fd, text, 4096);
			printf("READ: I got %d bytes with '%s'\n", status, text);
			break;
        case 'w':
            if(argc < 3)
                return 1;
            memset(text,0,4096);
            strcpy(text,argv[2]);
            status = write(fd,text,4096);
            printf("Wrote %d bytes\n", status);
            break;
        case 'm':
            ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            if(argc < 3)
                    return;
            memset(text, 0, 4096);
            strcpy(text, argv[2]);
            memcpy(ptr, text, 4096);
            printf("MMAP: I got '%s'\n",  (char*)ptr);
            break;
        default:
            break;
    }
}