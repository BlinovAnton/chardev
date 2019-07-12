#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
//#include <asm/boot.h>
#include <asm/uaccess.h>

/*ks - kernel space, us - user space*/

MODULE_LICENSE("GPL");

#define DEVICE_NAME "chrdev"

static size_t ks_len = 0;
static int major_num = 0;
static loff_t g_offset = 0;
static char *end_combo = "end ";
static char *ks_buff = NULL, *ks_buff_temp = NULL;
static ssize_t device_read (struct file *, char *, size_t, loff_t *);
static ssize_t device_write (struct file *, const char *, size_t, loff_t *);
static struct file_operations file_ops = {
    .read = device_read,
    .write = device_write,
};

static ssize_t device_read (struct file *file, char *us_buff, size_t us_len, loff_t *offset) {
    int read_res;
    if (ks_buff == NULL) {
	printk(KERN_INFO "Nothing to read\n");
	return 0;
    } else {
	//printk(KERN_INFO "off %d %d us %d\n", (int)*offset, (int)g_offset, (int)us_len);
	read_res = simple_read_from_buffer(us_buff, us_len, offset, ks_buff, ks_len);
	return read_res;
    }
}

static ssize_t device_write (struct file *file, const char *us_buff, size_t us_len, loff_t *offset) {
    int write_res = 0;
    /* start "refreshing" block */
    if (!strncmp(end_combo, us_buff, us_len - 1)) {
	ks_len = 0;
	g_offset = 0;
	if (!ks_buff) {
	    kfree(ks_buff);
	    ks_buff_temp = NULL;
	    printk(KERN_INFO "file refreshed\n");
	}
	return strlen(end_combo);
    }
    /* end "refreshing" block */

    /* start "write/add" block */
    ks_len = ks_len + us_len;
    ks_buff_temp = krealloc(ks_buff, ks_len, GFP_KERNEL);
    if (ks_buff_temp == NULL) {
	printk(KERN_INFO "kmalloc can't alloc memory, device_write fault\n");
	return -EFAULT;
    } else {
	ks_buff = ks_buff_temp;
	//printk(KERN_INFO "off %d %d us %d\n", (int)*offset, (int)g_offset, (int)us_len);
	write_res = simple_write_to_buffer(ks_buff, ks_len, &g_offset, us_buff, us_len);
	//printk(KERN_INFO "off %d %d us %d\n", (int)*offset, (int)g_offset, (int)us_len);
	return write_res;
    }
    /* end "write/add" block */
}

static int __init chrdev_init (void) {
	major_num = register_chrdev (0, DEVICE_NAME, &file_ops);
	if (major_num <= 0) {
	    printk(KERN_INFO "Can't register any Major num...%d\n", major_num);
	    return -EFAULT;
        } else {
	    printk(KERN_INFO "Successfully registered %d as major num\n", major_num);
	    return 0;
	}
}

static void __exit chrdev_exit (void) {
	if (ks_buff != NULL) {
	    kfree(ks_buff);
	}
	unregister_chrdev (major_num, DEVICE_NAME); //add error handling
	printk(KERN_INFO "Unregisterd %s (%d)\n", DEVICE_NAME, major_num);
}

module_init (chrdev_init);
module_exit (chrdev_exit);