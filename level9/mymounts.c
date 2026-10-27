// Phew... I don’t know who wrote that last file, but wow.
// Anyway, let’s get back to writing something more meaningful.
// To Do
// • Create a module that can list mount points on your system, with the associated
// name.
// • Your file must be named /proc/mymounts.
// $> cat /proc/mymounts
// root /
// sys /sys
// proc /proc
// run /run
// dev /dev
// Turn In
// • The module source code and a Makefile.
// This isn’t a hard task, but it can be tricky.
// Check out documentation on mount points, directory listing, and linked list traversal in
// the kernel.
// And above all—have fun :)

// 1/ find how to create a /proc. (I learned how to do a /dev but what about /proc ?)
// 2/ how to init and destroy.

// A note before the implementation

// The seq_file approach is well suited to /proc/mymounts, and the official documentation explains how to create such a file and format its output. 
// The Linux Kernel documentation

// The tricky part is accessing the mount list from an external module. Internal structures such as struct mount and the functions used to traverse mount namespaces aren't a stable external-module API. Rather than give you code that assumes inaccessible kernel internals and may fail to compile on your kernel, I'll show you a practical implementation using the kernel's existing mount information as a starting point, along with the relevant source-level distinction.
// 1. Start with the /proc file implementation

// Here is a complete module skeleton using seq_file. It creates /proc/mymounts, handles reads, and cleans up correctly. The mount traversal is deliberately left as a separate function because that part depends on the kernel version and its internal mount APIs.

// mymounts.c
// c


#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/rbtree.h>
#include <linux/nsproxy.h>
#include <../fs/mount.h>

#define PROC_NAME "mymounts"

// static int show_vfsmnt(struct seq_file *m, struct vfsmount *mnt)
// {
// 	struct proc_mounts *p = m->private;
// 	struct mount *r = real_mount(mnt);
// 	struct path mnt_path = { .dentry = mnt->mnt_root, .mnt = mnt };
// 	struct super_block *sb = mnt_path.dentry->d_sb;
// 	int err;

// 	if (sb->s_op->show_devname) {
// 		err = sb->s_op->show_devname(m, mnt_path.dentry);
// 		if (err)
// 			goto out;
// 	} else {
// 		mangle(m, r->mnt_devname);
// 	}
// 	seq_putc(m, ' ');
// 	err = seq_path_root(m, &mnt_path, &p->root, " \t\n\\");
// 	if (err)
// 		goto out;
// 	seq_putc(m, ' ');
// 	show_type(m, sb);
// 	seq_puts(m, __mnt_is_readonly(mnt) ? " ro" : " rw");
// 	err = show_sb_opts(m, sb);
// 	if (err)
// 		goto out;
// 	show_vfsmnt_opts(m, mnt);
// 	if (sb->s_op->show_options)
// 		err = sb->s_op->show_options(m, mnt_path.dentry);
// 	seq_puts(m, " 0 0\n");
// out:
// 	return err;
// }

static struct proc_dir_entry *mymounts_entry;


static int mymounts_show(struct seq_file *m, void *v)
{
	struct mnt_namespace *ns;
	struct rb_node *node;
	struct mount *mnt;
	struct path path;
	char *buf;
	char *pathname;

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
        	seq_printf(m, "%s %s\n",
                   mnt->mnt_devname ? mnt->mnt_devname : "unknown",
                   pathname);
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
