#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/poll.h>

static struct gpio_desc *button;

#define IO_BUTTON 158   /* Rock Pi 4B 實體腳位 37 */
#define IO_OFFSET 0

unsigned int irq_number;

static int irq_ready = 0;
static wait_queue_head_t wq;

static int major;

// class
static struct class *cls;

static irqreturn_t gpio_irq_handler(int irq, void *dev_id)
{
	printk("Button Press!\n");
    irq_ready  = 1;
    wake_up(&wq);
	return IRQ_HANDLED;
}


static unsigned int my_poll(struct file *file, poll_table *wait){
    printk("Enter poll\n");
    poll_wait(file,&wq,wait);
    if(irq_ready  == 1){
        irq_ready = 0;
        return POLLIN;
    }
        
    return 0;

}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.poll = my_poll
};


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

	irq_number = gpio_to_irq(IO_BUTTON + IO_OFFSET);

	if(request_irq(irq_number, gpio_irq_handler, IRQF_TRIGGER_RISING, "my_gpio_irq", NULL) != 0){
		printk("Error!\nCan not request interrupt nr.: %d\n", irq_number);
		gpio_free(IO_BUTTON + IO_OFFSET);
		return -1;
	}

	printk("GPIO 17 is mapped to IRQ Nr.: %d\n", irq_number);
	
    init_waitqueue_head(&wq);

    major = register_chrdev(0, "gpio_pull", &fops);
	if(major < 0) {
		printk("Error!\n Can't register device Number!\n");
		free_irq(irq_number, NULL);
		gpio_free(IO_BUTTON + IO_OFFSET);
	}

	cls = class_create(THIS_MODULE, "gpio_pull_class");
	if(!cls){
		printk("Error!\n Can't create class!\n");
		free_irq(irq_number, NULL);
		gpio_free(IO_BUTTON + IO_OFFSET);
		unregister_chrdev(major,"gpio_pull");
	}

	if(!device_create(cls, NULL, MKDEV(major, 0), NULL, "gpio_pull")){
		printk("Error!\n Can't create device file!\n");
		free_irq(irq_number, NULL);
		gpio_free(IO_BUTTON + IO_OFFSET);
		class_unregister(cls);
		class_destroy(cls);
		unregister_chrdev(major,"gpio_pull");
	}


	return 0;
}

static void __exit my_exit(void)
{
	printk("gpioctrl - Module unloaded\n");
	free_irq(irq_number, NULL);
	gpio_free(IO_BUTTON + IO_OFFSET);
    device_destroy(cls, MKDEV(major, 0));
    class_unregister(cls);
    class_destroy(cls);
    unregister_chrdev(major,"gpio_pull");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Joe (Rock Pi 4B)");
MODULE_DESCRIPTION("Example: read GPIO button input on Rock Pi 4B");