#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/debugfs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/jiffies.h>

#define FT_LOGIN	"aascedu"
#define FT_LOGIN_LEN	(sizeof(FT_LOGIN) - 1)

static struct dentry	*ft_dir;

static char		*foo_buf;
static size_t		foo_len;
static DEFINE_MUTEX(foo_lock);

/* ------------------------------------------------------------------ id */

static ssize_t ft_id_read(struct file *filp, char __user *buf,
			  size_t count, loff_t *ppos)
{
	return simple_read_from_buffer(buf, count, ppos,
				       FT_LOGIN "\n", FT_LOGIN_LEN + 1);
}

static ssize_t ft_id_write(struct file *filp, const char __user *buf,
			   size_t count, loff_t *ppos)
{
	char input[32];

	if (count >= sizeof(input))
		return -EINVAL;
	if (copy_from_user(input, buf, count))
		return -EFAULT;
	input[count] = '\0';
	if (strcmp(input, FT_LOGIN) != 0)
		return -EINVAL;
	return count;
}

static const struct file_operations ft_id_fops = {
	.owner	= THIS_MODULE,
	.read	= ft_id_read,
	.write	= ft_id_write,
	.llseek	= default_llseek,
};

/* ------------------------------------------------------------- jiffies */

static ssize_t ft_jiffies_read(struct file *filp, char __user *buf,
			       size_t count, loff_t *ppos)
{
	char tmp[32];
	int len;

	len = scnprintf(tmp, sizeof(tmp), "%lu\n", (unsigned long)jiffies);
	return simple_read_from_buffer(buf, count, ppos, tmp, len);
}

static const struct file_operations ft_jiffies_fops = {
	.owner	= THIS_MODULE,
	.read	= ft_jiffies_read,
	.llseek	= default_llseek,
};

/* ----------------------------------------------------------------- foo */

static ssize_t ft_foo_read(struct file *filp, char __user *buf,
			   size_t count, loff_t *ppos)
{
	ssize_t ret;

	if (mutex_lock_interruptible(&foo_lock))
		return -ERESTARTSYS;
	ret = simple_read_from_buffer(buf, count, ppos, foo_buf, foo_len);
	mutex_unlock(&foo_lock);
	return ret;
}

static ssize_t ft_foo_write(struct file *filp, const char __user *buf,
			    size_t count, loff_t *ppos)
{
	ssize_t ret;

	if (*ppos != 0)
		return -EINVAL;
	if (count > PAGE_SIZE)
		count = PAGE_SIZE;

	if (mutex_lock_interruptible(&foo_lock))
		return -ERESTARTSYS;

	memset(foo_buf, 0, PAGE_SIZE);
	if (copy_from_user(foo_buf, buf, count)) {
		foo_len = 0;
		ret = -EFAULT;
		goto out;
	}
	foo_len = count;
	*ppos = count;
	ret = count;
out:
	mutex_unlock(&foo_lock);
	return ret;
}

static const struct file_operations ft_foo_fops = {
	.owner	= THIS_MODULE,
	.read	= ft_foo_read,
	.write	= ft_foo_write,
	.llseek	= default_llseek,
};

/* ------------------------------------------------------- init / exit */

static int __init ft_init(void)
{
	struct dentry *d;

	foo_buf = kzalloc(PAGE_SIZE, GFP_KERNEL);
	if (!foo_buf)
		return -ENOMEM;
	foo_len = 0;

	ft_dir = debugfs_create_dir("fortytwo", NULL);
	if (IS_ERR_OR_NULL(ft_dir)) {
		kfree(foo_buf);
		foo_buf = NULL;
		return ft_dir ? PTR_ERR(ft_dir) : -ENODEV;
	}

	d = debugfs_create_file("id", 0666, ft_dir, NULL, &ft_id_fops);
	if (IS_ERR_OR_NULL(d))
		goto err;

	d = debugfs_create_file("jiffies", 0444, ft_dir, NULL,
				&ft_jiffies_fops);
	if (IS_ERR_OR_NULL(d))
		goto err;

	d = debugfs_create_file("foo", 0644, ft_dir, NULL, &ft_foo_fops);
	if (IS_ERR_OR_NULL(d))
		goto err;

	pr_info("fortytwo: debugfs entries created\n");
	return 0;

err:
	debugfs_remove(ft_dir);
	ft_dir = NULL;
	kfree(foo_buf);
	foo_buf = NULL;
	return -ENODEV;
}

static void __exit ft_exit(void)
{
	debugfs_remove(ft_dir);
	ft_dir = NULL;
	kfree(foo_buf);
	foo_buf = NULL;
	pr_info("fortytwo: debugfs entries removed\n");
}

module_init(ft_init);
module_exit(ft_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("debugfs fortytwo module");
MODULE_AUTHOR("aascedu");
