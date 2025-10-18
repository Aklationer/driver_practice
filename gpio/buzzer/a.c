#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio/consumer.h>
#include <linux/delay.h>
static struct gpio_desc *buzzer;

#define IO_BUZZER 158  // Pin 37 = GPIO4_C1

static int __init buzzer_init(void)
{
    int status;

    buzzer = gpio_to_desc(IO_BUZZER);
    if (!buzzer) {
        pr_err("buzzer: failed to get GPIO %d\n", IO_BUZZER);
        return -ENODEV;
    }

    // 設為輸出，預設高電平 -> 不鳴
    status = gpiod_direction_output(buzzer, 1);
    if (status)
        return status;

    pr_info("buzzer: initialized, HIGH (silent)\n");

    // 測試鳴叫 1 秒
    gpiod_set_value(buzzer, 0);
    msleep(1000);
    gpiod_set_value(buzzer, 1);

    return 0;
}

static void __exit buzzer_exit(void)
{
    gpiod_set_value(buzzer, 1);  // 關閉蜂鳴器
    pr_info("buzzer: unloaded\n");
}

module_init(buzzer_init);
module_exit(buzzer_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Joe");
MODULE_DESCRIPTION("Rock Pi 4B Buzzer (LOW trigger @3.3V)");