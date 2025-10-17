#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>

static struct proc_dir_entry *proc_folder;
static struct proc_dir_entry *proc_file;
static char buffer[256];
// wait queue
DECLARE_WAIT_QUEUE_HEAD(wq1);

static int my_open(struct inode *inode, struct file *filp)
{
	printk("hello\n");
	return 0;
}

static int my_release(struct inode *inode, struct file *filp)
{	
	printk("bye\n");
	return 0;
}


static ssize_t my_read(struct file *File, char *user_buffer, size_t count, loff_t *offs){
	size_t size = min(count, sizeof(buffer)- *offs);

	if (*offs >= sizeof(buffer))  // 到尾部了
		return 0;


	int  not_copied = copy_to_user(user_buffer,buffer,size);
	int actual_read = size - not_copied;
	*offs+=actual_read;
	return actual_read;
}

static ssize_t my_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs)
{
	size_t size = min(count, sizeof(buffer));
	int  not_copied = copy_from_user(buffer,user_buffer,size);
	return size - not_copied;
}


static struct proc_ops fops = {
	.proc_open = my_open,
	.proc_release = my_release,
	.proc_read = my_read,
	.proc_write = my_write,
};

static int __init my_init(void)
{
	proc_folder = proc_mkdir("hello", NULL);

	if(proc_folder == NULL){
		printk("create dir error\n");
		return -ENOMEM;
	}
		

	proc_file = proc_create("test", 0666, proc_folder, &fops);

	if(proc_file == NULL){
		printk("create files error\n");
		proc_remove(proc_folder);
		return -ENOMEM;
	}
		

	return 0;
}

static void __exit my_exit(void)
{	
	proc_remove(proc_file);
	proc_remove(proc_folder);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A sample driver for manually registering a character device");