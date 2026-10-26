#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/slab.h>


#define DEVICE_NAME "reverse"


static char str[PAGE_SIZE];
static char reversed[PAGE_SIZE];
static size_t   str_len;
static DEFINE_MUTEX(str_mutex)

ssize_t myfd_read(struct file *fp, char __user *user, size_t size,
                    loff_t *offs)
{
    ssize_t ret;
	size_t i;

	if (mutex_lock_interruptible(&str_mutex))
		return -ERESTARTSYS;
	for (i = 0; i < str_len; i++)
		reversed[i] = str[str_len - 1 - i];
	ret = simple_read_from_buffer(user, size, offs,
				      reversed, str_len);
	mutex_unlock(&str_mutex);
	return ret;
}

ssize_t myfd_write(struct file *fp, const char __user *user, size_t size,
                    loff_t *offs)
{
	size_t count;
	ssize_t ret;

	count = min(size, sizeof(str) - 1);
	if (mutex_lock_interruptible(&str_mutex))
		return -ERESTARTSYS;
	if (copy_from_user(str, user, count)) {
		ret = -EFAULT;
		goto out;
	}
	str[count] = '\0';
	str_len = count;
	*offs = 0;
	ret = count;
out:
	mutex_unlock(&str_mutex);
	return ret;
}

static struct file_operations myfd_fops = {
    .owner = THIS_MODULE,
    .read = &myfd_read,
    .write = &myfd_write
};

static struct miscdevice myfd_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &myfd_fops
};

static int __init myfd_init(void)
{
    int retval;
    retval = misc_register(&(*(&(myfd_device))));
    return 1;
}

static void __exit myfd_cleanup(void)
{
}

module_init(myfd_init);
module_exit(myfd_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Louis Solofrizzo <louis@ne02ptzero.me>");
MODULE_DESCRIPTION("Misc device that reverse written text");