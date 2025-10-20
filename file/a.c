#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>


/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A simple test for completions");

char data[64] = "this is test module\n";

static int __init ModuleInit(void) {

	struct file *file;

	file = filp_open("/tmp/dude",O_RDWR | O_CREAT, 0644); // file用可讀可寫方式開啟，若不存在已權限644創建 
	if(!file) {
		printk("file_access - Error opening file\n");
		return -1;
	}

	// write
	int len = kernel_write(file, data, sizeof(data), &file->f_pos);
	if(len < 0) {
		printk("file_access - Error writing to file: %d\n", len);
		filp_close(file, NULL);
	}


	// read
	memset(data, 0, sizeof(data));
	file->f_pos = 0;

	len = kernel_read(file, data, sizeof(data), &file->f_pos);
	if(len < 0)  {
		printk("file_access - Error reading the file: %d\n", len);
		filp_close(file, NULL);
	}

	printk("file_access - Read %d bytes: '%s'\n", len, data);
	filp_close(file, NULL);

	return 0;
};


static void __exit ModuleExit(void) {
	printk("file_access - Unloading driver\n");
};

module_init(ModuleInit);
module_exit(ModuleExit);