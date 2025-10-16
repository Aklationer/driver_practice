#include <linux/module.h>
#include <linux/init.h>


/* Kernel Module's Paramerters*/
static unsigned int number = 12;
static char *device_name = "test device";

module_param(number, uint, S_IRUGO);   
module_param(device_name, charp, S_IRUGO); 

MODULE_PARM_DESC(number, "number to use in this LKM");   // modinfo時顯示
MODULE_PARM_DESC(device_name, "Device name to use in this LKM"); // modinfo時顯示


static int __init ModuleInit(void) {
	printk("gpio_nr = %u\n", number);
	printk("device_name = %s\n", device_name);
	printk("Hello, Kernel!\n");
	return 0;
}


static void __exit ModuleExit(void) {
	printk("Goodbye, Kernel\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A simple LKM to demonstrate the use of parameters");
