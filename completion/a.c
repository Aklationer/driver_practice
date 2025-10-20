#include <linux/module.h>
#include <linux/init.h>
#include <linux/completion.h>
#include <linux/jiffies.h>
#include <linux/timer.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes 4 GNU/Linux");
MODULE_DESCRIPTION("A simple test for completions");

// timer
static struct timer_list my_timer;

//comp
static struct completion comp;

void timer_callback(struct timer_list * data) {
	printk("mycompletion - timer expired\n");
	complete(&comp);
}


static int __init ModuleInit(void) {
	int status;

	timer_setup(&my_timer, timer_callback, 0);  //  初始化timer並和callback綁定

	init_completion(&comp); // 初始化completions物件

	printk("mycompletion - Start the timer the first time. Time: 40ms\n");
	mod_timer(&my_timer, jiffies + msecs_to_jiffies(40)); // 將timer設定現在起後40ms觸發
	status = wait_for_completion_timeout(&comp, msecs_to_jiffies(100)); // completion最多等待100秒
	if(!status) 
		printk("mycompletion - Completion timed out!\n");

	reinit_completion(&comp); // 重新init completion物件
	printk("mycompletion - Start the timer the second time. Time: 400ms\n");
	mod_timer(&my_timer, jiffies + msecs_to_jiffies(400));
	status = wait_for_completion_timeout(&comp, msecs_to_jiffies(100));
	if(!status) 
		printk("mycompletion - Completion timed out!\n");

	return 0;
}


static void __exit ModuleExit(void) {
	del_timer(&my_timer);
}

module_init(ModuleInit);
module_exit(ModuleExit);