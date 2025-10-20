#include <linux/module.h>
#include <linux/init.h>
#include <linux/mod_devicetable.h>
#include <linux/property.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A simple LKM to parse the device tree for a specific device and its properties");

static const struct of_device_id my_driver_ids[] = {
	{.compatible = "brightlight,mydev"},
	{ }
};



static int dt_probe(struct platform_device *pdev){

	struct device *dev = &(pdev->dev);


	if(!device_property_present(dev, "label")){
		printk("Device property 'label' not found!\n");
		return -1;
	}

	if(!device_property_present(dev,"my_value")){
		printk("Device property 'my_value' not found!\n");
		return -1;
	}

	
	//read property
	ret = device_property_read_string(dev, "label", &label);
	if(ret){
		printk("Device property 'label' could not read!\n");
		return -1;
	}

	printk("DTS label is %s\n",label);

	ret = device_property_read_u32(dev, "my_value", &my_value);
	if(ret){
		printk("Device property 'my_value' could not read!\n");
		return -1;
	}


	printk("DTS my_value is %d\n",my_value);

	return 0;
};

static int dt_remove(struct platform_device *pdev){
	return 0;
};

static struct platform_driver my_driver = {
	.probe = dt_probe,
	.remove = dt_remove,
	.driver = {
		.name = "my_device_driver",
		.of_match_table = my_driver_ids,
	},
};

static int __init my_init(void){
	if(platform_driver_register(&my_driver)){
		printk("Error! Could not load driver\n");
		return -1;
	}
	return 0;
}

static void __exit my_exit(void){
	printk("Unload driver");
	platform_driver_unregister(&my_driver);
}