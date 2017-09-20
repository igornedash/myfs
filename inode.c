#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>

MODULE_AUTHOR ("Igor Nedashkivskyi");
MODULE_LICENSE("Dual BSD/GPL");

static int myfs_init(void)
{
    printk(KERN_INFO "My First Virtual File System Driver\n");

    return 0;
}

static void myfs_clean(void)
{
}
module_init(myfs_init);
module_exit(myfs_clean);
