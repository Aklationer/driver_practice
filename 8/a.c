#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/delay.h>


static dev_t dev_nr;
static struct cdev my_cdev;
static struct class *my_class;


static struct task_struct *kthread_1;
static struct task_struct *kthread_2;
static int t1 = 1, t2 = 2;


static int my_open(struct inode *inode, struct file *filp)
{
	kthread_stop(kthread_1);
	kthread_stop(kthread_2);

	return 0;
}

static int my_release(struct inode *inode, struct file *filp)
{
	printk("bye\n");
	return 0;
}


int work_function(void *data){
	int i=0;
	int number = *(int *)data;
	while(!kthread_should_stop()){
		printk("kthread - Thread %d is executed! Counter val: %d\n", number, i++);
		msleep(2000);
	}
	printk("kthread - Thread %d finished execution!\n", number);
	return 0;
};



static struct file_operations fops = {
	.open = my_open,
	.release = my_release,
};

static int __init my_init(void)
{
	int status;
	status = alloc_chrdev_region(&dev_nr, 0, MINORMASK + 1, "hello_cdev");   //dev_nr 同是某major minor是0  
	if (status) {
		pr_err("hello_cdev - Error reserving the region of device numbers\n");
		return status;
	}

	cdev_init(&my_cdev, &fops); // cdev跟 fops綁定
	my_cdev.owner = THIS_MODULE;

	status = cdev_add(&my_cdev, dev_nr, MINORMASK + 1);
	if (status) {
		pr_err("hello_cdev - Error adding cdev\n");
		goto free_devnr;
	}

	pr_info("hello_cdev - Registered a character device for Major %d starting with Minor %d\n", MAJOR(dev_nr), MINOR(dev_nr));

	my_class = class_create(THIS_MODULE,"my_class");
	if (!my_class) {
		pr_err("hello_cdev - Could not create class my_class\n");
		status = ENOMEM;
		goto delete_cdev;
	}

	if (!device_create(my_class, NULL, dev_nr, NULL, "hello%d", 0)) {
		pr_err("hello_cdev - Could not create device hello0\n");
		status = ENOMEM;
		goto delete_class;
	}

	pr_info("hello_cdev - Created device under /sys/class/my_class/hello0\n");


	kthread_1 = kthread_create(work_function,&t1,"kthread_1");

	if(kthread_1 == NULL)
		goto thread;

	kthread_2 = kthread_create(work_function,&t2,"kthread_2");
	
	if(kthread_2 == NULL)
		goto thread;
	
	wake_up_process(kthread_1);
	wake_up_process(kthread_2);

	return 0;

thread:
	device_destroy(my_class, dev_nr);
delete_class:
	class_unregister(my_class);
	class_destroy(my_class);
delete_cdev:
	cdev_del(&my_cdev);
free_devnr:
	unregister_chrdev_region(dev_nr, MINORMASK + 1);
	return status;
}

static void __exit my_exit(void)
{
	device_destroy(my_class, dev_nr);
	class_unregister(my_class);
	class_destroy(my_class);
	cdev_del(&my_cdev);
	unregister_chrdev_region(dev_nr, MINORMASK + 1);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A sample driver for manually registering a character device");