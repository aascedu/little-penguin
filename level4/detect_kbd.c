#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h> 

static int __init keyboard_init(void)
{
  printk(KERN_INFO "keyboard plugged!\n");
  return 0;
}

static void __exit keyboard_exit(void)
{
  printk(KERN_INFO "keyboard unplugged!\n");
}

module_init(keyboard_init);
module_exit(keyboard_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("keyboard detection Module");
MODULE_AUTHOR("aascedu");
