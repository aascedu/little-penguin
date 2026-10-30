#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/rbtree.h>
#include <linux/nsproxy.h>
#include <../fs/mount.h>

#define PROC_NAME "mymounts"

static struct proc_dir_entry *mymounts_entry;


static int mymounts_show(struct seq_file *m, void *v)
{
	struct mnt_namespace *ns;
	struct rb_node *node;
	struct mount *mnt;
	struct path path;
	char *buf;
	char *pathname;
	const char *devname = "unknown";

	ns = current->nsproxy->mnt_ns;
	buf = (char *)__get_free_page(GFP_KERNEL);
	if (!buf)
		return -ENOMEM;
	for (node = rb_first(&ns->mounts); node; node = rb_next(node)) {
		mnt = rb_entry(node, struct mount, mnt_node);
		path.mnt = &mnt->mnt;
        	path.dentry = mnt->mnt.mnt_root;
        	pathname = d_path(&path, buf, PAGE_SIZE);
        	if (IS_ERR(pathname))
            		continue;
			if (mnt->mnt_devname)
				devname = mnt->mnt_devname;
        	seq_printf(m, "%s %s\n", devname, pathname);
	}
	return 0;
}

static int mymounts_open(struct inode *inode, struct file *file)
{
	return single_open(file, mymounts_show, NULL);
}

static const struct proc_ops mymounts_ops = {
	.proc_open    = mymounts_open,
	.proc_read    = seq_read,
	.proc_lseek   = seq_lseek,
	.proc_release = single_release,
};

static int __init mymounts_init(void)
{
	mymounts_entry = proc_create(
		PROC_NAME, 0444, NULL, &mymounts_ops
    );
    if (!mymounts_entry)
	return -ENOMEM;

return 0;
}

static void __exit mymounts_exit(void)
{
	proc_remove(mymounts_entry);
}

module_init(mymounts_init);
module_exit(mymounts_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("aascedu");
MODULE_DESCRIPTION("List mount points in /proc/mymounts");