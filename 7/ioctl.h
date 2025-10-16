#ifndef IOCTL_TEST_H
#define IOCTL_TEST_H

struct mystruct{
	int repeat;
	char name[64];
};

#define WR_VALUE _IOW('a', 0, int32_t *)
#define RD_VALUE _IOR('a', 1, int32_t *)
#define GREETER  _IOW('a', 2, struct mystruct *)

#endif