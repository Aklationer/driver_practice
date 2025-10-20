#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Demonstration for lists in the kernel");

struct my_data{
	struct list_head list;
	char text[64];
};

LIST_HEAD(my_list);

static int __init my_init(void){

	struct my_data *tmp, *next;
	struct list_head *ptr;

	tmp = kmalloc(sizeof(struct my_data), GFP_KERNEL);
	strcpy(tmp->text, "First Element");
	list_add_tail(&tmp->list, &my_list);

	tmp = kmalloc(sizeof(struct my_data), GFP_KERNEL);
	strcpy(tmp->text, "Second Element");
	list_add_tail(&tmp->list, &my_list);

	tmp = kmalloc(sizeof(struct my_data), GFP_KERNEL);
	strcpy(tmp->text, "Third Element");
	list_add_tail(&tmp->list, &my_list);

	list_for_each(ptr,&my_list){
		tmp = list_entry(ptr,struct my_data, list);
		printk("%s\n",tmp->text);
	}

	list_for_each_entry_safe(tmp, next, &my_list, list) {
		list_del_init(&tmp->list);
		kfree(tmp);
	}

	return 0;
}

static void __exit my_exit(void) {
	printk("Unload Kernel\n");
}

module_init(my_init);
module_exit(my_exit);