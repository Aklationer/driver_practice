#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/sched/signal.h>
#include <linux/ioctl.h>

static struct gpio_desc *button;
static int major;

//gpios
#define IO_BUTTON 158   /* Rock Pi 4B 實體腳位 37 */
#define IO_OFFSET 0

//irq
unsigned int irq_number;

//process
static struct task_struct *task = NULL;

// ioctl
#define REGISTER_UAPP _IO('a', 0)

// signal
#define SIGNR 44
struct kernel_siginfo info;

// class
static struct class *cls;

static long int my_ioctl(struct file *file, unsigned cmd, unsigned long arg){
	switch(cmd){
		case REGISTER_UAPP :
			task = get_current();
			printk("process %d is registered\n",task->pid);
			break;
		default:
			return -EINVAL;
	}
	return 0;
}

static int my_close(struct inode *device_file, struct file *instance) {
	if(task != NULL)
		task = NULL;
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.release = my_close,
	.unlocked_ioctl = my_ioctl,
};




static irqreturn_t irq_handler(int irq, void *dev_id)
{
	if(task != NULL){
		memset(&info,0,sizeof(info));
		info.si_signo = SIGNR;
		info.si_code = SI_QUEUE;

		/* Send the signal */
		if(send_sig_info(SIGNR,&info, task) < 0)
			printk("gpio_irq_signal: Error sending signal\n");
	}
	return IRQ_HANDLED;
}



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
		gpio_free(IO_BUTTON + IO_OFFSET);
		return status;
	}

	irq_number = gpio_to_irq(IO_BUTTON + IO_OFFSET); // 得到virq

	if(request_irq(irq_number,irq_handler,IRQF_TRIGGER_RISING, "my_gpio_irq", NULL) != 0){
		printk("gpioctrl - Error setting GPIO %d interrupt\n", IO_BUTTON);
		gpio_free(IO_BUTTON + IO_OFFSET);
	} 
	major = register_chrdev(0, "gpio_irq_signal", &fops);
	if(major < 0) {
		printk("Error!\n Can't register device Number!\n");
		free_irq(irq_number, NULL);
		gpio_free(IO_BUTTON + IO_OFFSET);
	}

	cls = class_create(THIS_MODULE, "gpio_irq_signal_class");
	if(!cls){
		printk("Error!\n Can't create class!\n");
		free_irq(irq_number, NULL);
		gpio_free(IO_BUTTON + IO_OFFSET);
		unregister_chrdev(major,"gpio_irq_signal");
	}

	if(!device_create(cls, NULL, MKDEV(major, 0), NULL, "gpio_irq_signal")){
		printk("Error!\n Can't create device file!\n");
		free_irq(irq_number, NULL);
		gpio_free(IO_BUTTON + IO_OFFSET);
		class_unregister(cls)
		class_destroy(cls);
		unregister_chrdev(major,"gpio_irq_signal");
	}

	printk("GPIO %d is mapped to IRQ number.: %d\n",IO_BUTTON, irq_number); 
	return 0;
}

static void __exit my_exit(void)
{
	printk("gpioctrl - Module unloaded\n");
	free_irq(irq_number, NULL);
	gpio_free(IO_BUTTON + IO_OFFSET);
	device_destroy(cls, MKDEV(major, 0));
	class_unregister(cls)
	class_destroy(cls);
	unregister_chrdev(major,"gpio_irq_signal");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Joe (Rock Pi 4B)");
MODULE_DESCRIPTION("Example: read GPIO button input on Rock Pi 4B");