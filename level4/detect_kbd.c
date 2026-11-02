// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init keyboard_init(void)
{
	pr_info("keyboard plugged!\n");
	return 0;
}

static void __exit keyboard_exit(void)
{
	pr_info("keyboard unplugged!\n");
}

module_init(keyboard_init);
module_exit(keyboard_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("keyboard detection Module");
MODULE_AUTHOR("aascedu");
