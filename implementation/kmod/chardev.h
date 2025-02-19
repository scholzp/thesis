#pragma once
#include <linux/fs.h>
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/cdev.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pascal Scholz <pascal.scholz@cyberus-technology.de>");

#define TEE_DEVICE_NAME "tee_dev"
#define TEE_DEVICE_BUFFER_SIZE (1024)

int init_tee_chardev(void);
void purge_tee_chardev(void);
extern int device_open(struct inode *inode, struct file *file);
extern int device_release(struct inode *inode, struct file *file);
extern ssize_t device_read(struct file *file, char __user *buffer, 
    size_t length, loff_t *offset);
extern ssize_t device_write(struct file *file, const char __user *buffer,
    size_t length, loff_t *offset);
extern char *tee_device_devnode_mode(const struct device *dev, umode_t *mode);
