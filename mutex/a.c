#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/delay.h>


static struct task_struct *kthread_1;
static struct task_struct *kthread_2;
static int t1 = 1, t2 = 2;
int delay[] = { 0, 1000, 500};

//mutex
static struct mutex lock;

int work_function(void *data){
	
	int index = *(int *)data;

	mutex_lock(&lock);
	pr_info("thread %d enter critical section\n",index);
	msleep(delay[index]);
	pr_info("thread %d leave critical section\n",index);
	mutex_unlock(&lock);
	return 0;
}



static int __init my_init(void){

	kthread_1 = kthread_create(work_function,&t1,"kthread_1");

	if(kthread_1 == NULL){

		pr_info("Counld not create thread\n");
		return -1;
	}

	kthread_2 = kthread_create(work_function,&t2,"kthread_2");

	if(kthread_2 == NULL){

		pr_info("Counld not create thread\n");
		return -1;
	}
	mutex_init(&lock);
	wake_up_process(kthread_1);
	wake_up_process(kthread_2);
	return 0;
}

static void __exit my_exit(void){
	pr_info("module unload\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("thread create");