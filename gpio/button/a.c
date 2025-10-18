#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio/consumer.h>

static struct gpio_desc *button;

#define IO_BUTTON 158   /* Rock Pi 4B 實體腳位 37 */
#define IO_OFFSET 0

static int __init my_init(void)
{
	int status;

	button = gpio_to_desc(IO_BUTTON + IO_OFFSET);
	if (!button) {
		printk("gpioctrl - Error getting GPIO %d\n", IO_BUTTON);
		return -ENODEV;
	}

	status = gpiod_direction_input(button);  // 該腳位設定input模式
	if (status) {
		printk("gpioctrl - Error setting GPIO %d to input\n", IO_BUTTON);
		return status;
	}

	printk("gpioctrl - Button is %spressed\n",
	       gpiod_get_value(button) ? "" : "not ");   
	return 0;
}

static void __exit my_exit(void)
{
	printk("gpioctrl - Module unloaded\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Joe (Rock Pi 4B)");
MODULE_DESCRIPTION("Example: read GPIO button input on Rock Pi 4B");