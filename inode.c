#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cred.h>
#include <linux/fs.h>

MODULE_AUTHOR ("Igor Nedashkivskyi");
MODULE_LICENSE("Dual BSD/GPL");

#define MYFS_SUPER_MAGIC 0x6d7f5e43

struct myfs_superblock_impl {};

static const struct super_operations myfs_super_ops = {};

static const struct dentry_operations myfs_dentry_ops = {};

static char mount_options[32];
static ssize_t
myfs_fstatus_read(struct file *file, char __user *buf, size_t nbytes, loff_t *ppos)
{
    const char message[] = "This mount status message sent through VFS framework\n""mount options are: ";
    char s[sizeof(message)+sizeof(mount_options)+1];
    snprintf(s, sizeof(s), "%s%s\n", message, mount_options);

    return simple_read_from_buffer(buf, nbytes, ppos, s, strlen(s));
}

static const struct file_operations myfs_fstatus_ops = {
    .read = myfs_fstatus_read,
};

static int fill_super(struct super_block *sb, void *opts, int nonused)
{
    /* Section that contains data related with internal implementation of file system */
    /* it will be accessible by driver as super_block private section sb->s_fs_info */
    /* during VFS file operations, see assignment below */
    static struct myfs_superblock_impl myfs_impl;
    /* start init files tree from / folder of mountpoint */
    static const struct tree_descr files_always_exist[] = {
        [2] = {"status", &myfs_fstatus_ops, S_IRUSR|S_IRUGO},
        /* last one */ {""}
    };
    struct inode *root_ino;
    int err;

    /* data contains string with options in mount -o parameter */
    memset(mount_options, 0x00, sizeof(mount_options));
    strncpy(mount_options, opts ? opts : "no options", sizeof(mount_options) - 1);

    /* fill super_block fields with default generic operations and values */
    err = simple_fill_super(sb, MYFS_SUPER_MAGIC, files_always_exist);
    if (err) goto fail;

    /* Change root i-node (super_block)  properties for access */
    root_ino = d_inode(sb->s_root);
//    root_ino->i_mode &= ~S_IALLUGO;
//    root_ino->i_uid = current_fsuid();
//    root_ino->i_uid = current_fsuid();

    sb->s_op = &myfs_super_ops;
    sb->s_d_op = &myfs_dentry_ops;
    sb->s_fs_info = &myfs_impl;

    return 0;

fail:
    return err;
}

static struct dentry *myfs_mount(struct file_system_type *fs_type,
            int flags, const char *dev_name, void *opts)
{
    return mount_nodev(fs_type, flags, opts, fill_super);
}

static struct file_system_type myfs_type = {
    .owner =    THIS_MODULE,
    .name =     "myfs",
    .mount =    myfs_mount,
    .kill_sb =  kill_litter_super,
};

/* Register new file system via VFS API */
static int myfs_init(void)
{
    printk(KERN_INFO "My First Virtual File System Driver\n");

    return register_filesystem(&myfs_type);
}

static void myfs_clean(void)
{
    unregister_filesystem(&myfs_type);
}
module_init(myfs_init);
module_exit(myfs_clean);
