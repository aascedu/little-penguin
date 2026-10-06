#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/miscdevice.h>

static ssize_t ft_misc_write(struct file *file, const char __user *buf,
               size_t len, loff_t *ppos)
{
    pr_info("ft misc device write\n");
    
    /* We are not doing anything with this data now */
    
    return len; 
}
 
/*
** This function will be called when we read the Misc Device file
*/
static ssize_t ft_misc_read(struct file *filp, char __user *buf,
                    size_t count, loff_t *f_pos)
{
    pr_info("ft misc device read\n");
 
    return 0;
}

static const struct file_operations fops = {
  .owner = THIS_MODULE,
  .write = ft_misc_write,
  .read = ft_misc_read,
  // .open = ft_misc_open,
  // .close = ft_misc_close,
  // .llseek = no_llseek,
};

struct miscdevice ft_misc_device = {
  .minor = MISC_DYNAMIC_MINOR,
  .name = "fortytwo",
  .fops = &fops,
};

static int __init misc_init(void)
{
  int error;

  error = misc_register(&ft_misc_device);
  if (error) {
    pr_err("misc_register failed!\n");
  }
  pr_info("misc_register init done!\n");
  return 0;
}

static void __exit misc_exit(void)
{
  misc_deregister(&ft_misc_device);
  pr_info("misc_register exit done!\n");
}

module_init(misc_init);
module_exit(misc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("misc device Module");
MODULE_AUTHOR("aascedu");
