#include <linux/module.h>
#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/string.h>


static struct kobject *dummy_kobj;


static ssize_t dummy_show(struct kobject *kobj, struct kobj_attribute *attr, char *buffer) {
	return sprintf(buffer, "You have read from /sys/kernel/%s/%s\n", kobj->name, attr->attr.name);
}


static ssize_t dummy_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count) {
	printk("sysfs_test - You wrote '%s' to /sys/kernel/%s/%s\n", buffer, kobj->name, attr->attr.name);
	return count;
}



static struct kobj_attribute dummy_attr = __ATTR(dummy, 0660, dummy_show, dummy_store);

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

static int __init my_init(void)
{
	dummy_kobj = kobject_create_and_add("hello", kernel_kobj);  // 這裡會對dir 增加一個參考計數
	if(dummy_kobj == NULL){
		printk("ctreate dir error\n");
		return -ENOMEM;
	}

	if(sysfs_create_file(dummy_kobj, &dummy_attr.attr)){
		kobject_put(dummy_kobj);   // 計數減1
		printk("ctreate file error\n");
		return -ENOMEM;
	}

	return 0;
}

static void __exit my_exit(void)
{	
	sysfs_remove_file(dummy_kobj, &dummy_attr.attr);
	kobject_put(dummy_kobj);   // 計數減1
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A sample sysfs example module");