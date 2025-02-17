#include <linux/types.h>
#include "chardev.h"

MODULE_LICENSE("GPL");

// Kernel space buffer of the character device
static char TEE_CHARDEV_BUFFER[TEE_DEVICE_BUFFER_SIZE];
// Offset into kerne space buffer
static size_t TEE_CHARDEV_OFFSET = 0;
// Number of the TEE device, as to be assigned by the kernel
static dev_t TEE_DEV_NUM = -1;
// Major part of the device number
static int TEE_DEV_MAJOR = -1;
// Flag that tracks if the character device was opened
static int TEE_DEV_OPENED = 0;
// Pointer to the kernel character device structure
static struct cdev TEE_CHARDEV;
// Pointer to the kernel device structure
static struct device* TEE_DEVICE = NULL;
// Pointer to the class of the device
static struct class *TEE_DEV_CLASS = NULL;

// Permissions for the interation with the device; allow r/w for all
const umode_t USER_SPACE_PERMISSION = 0666;
// Number of minor numbers to allocate
const int TEE_CHARDEV_MINOR_COUNT = 0;

// File operation struct for the character device. Contains pointer to functions
// defined in this file.
static struct file_operations CHARDEV_OPS = 
{
	/* TODO file ops */
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release,
};

int init_tee_chardev(void)
{
	int rc;

	// Allocate dynamic char devise num from kernel
	rc = alloc_chrdev_region(
		&TEE_DEV_NUM,
		0,
		TEE_CHARDEV_MINOR_COUNT,
		TEE_DEVICE_NAME
	);
	
	if (0 > rc) {
		pr_err("Registering chardev failed with error %d! Aborting...\n", rc);
		return rc;
	}
	// Get the major part of the device number
	TEE_DEV_MAJOR = MAJOR(TEE_DEV_NUM);
	pr_info("Registered chardev %d\n", TEE_DEV_MAJOR);
	
	// Create a class for the device in the kernel
	TEE_DEV_CLASS = class_create(TEE_DEVICE_NAME);
	if (IS_ERR(TEE_DEV_CLASS)) {
		pr_err("Failed registering device class.");
		return PTR_ERR(TEE_DEV_CLASS);
	}

	// Give user space access to the device
	TEE_DEV_CLASS->devnode = tee_device_devnode_mode;
	// Set the function pointers for when interacting with the device
	cdev_init(&TEE_CHARDEV, &CHARDEV_OPS);
	TEE_CHARDEV.owner = THIS_MODULE;

	// Add the device node to /proc/
	rc = cdev_add(&TEE_CHARDEV, TEE_DEV_NUM, 1);
	if (rc) {
		pr_err("Failed to add char dev\n");
		return rc;
	}

	// Create device node /dev/TEE_DEVICE_NAME
	TEE_DEVICE = device_create(
		TEE_DEV_CLASS, 
		NULL, TEE_DEV_NUM, NULL, 
		TEE_DEVICE_NAME
	);

	if (IS_ERR(TEE_DEVICE)) {
		pr_err("Failed to call device_create\n");
		return PTR_ERR(TEE_DEVICE);
	}

	pr_info("Device created on /dev/%s\n", TEE_DEVICE_NAME);
	return TEE_DEV_MAJOR;
}

void purge_tee_chardev(void) {
	device_destroy(TEE_DEV_CLASS, TEE_DEV_NUM);
	class_unregister(TEE_DEV_CLASS);
	class_destroy(TEE_DEV_CLASS);
	cdev_del(&TEE_CHARDEV);
	unregister_chrdev_region(TEE_DEV_NUM, TEE_CHARDEV_MINOR_COUNT);
}

int device_open(struct inode *inode, struct file *file)
{
	int acquired_exclusive_access = 0;
	int expected = 0;
	int new_state = 1;

	pr_info("%s called\n", __FUNCTION__);

	acquired_exclusive_access = __atomic_compare_exchange(
			&TEE_DEV_OPENED,
			&expected,
			&new_state,
			0,
			__ATOMIC_SEQ_CST,
			__ATOMIC_SEQ_CST
	);

	if (!acquired_exclusive_access) {
		pr_info("Another device has the driver file already open\n");
		return -EBUSY;
	} else {
		pr_info("Acquired exclusive access to device\n");
	}

	return 0;
};

int device_release(struct inode *inode, struct file *file)
{
	pr_info("%s called", __FUNCTION__);

	// Non-atomic access is fine as there can only be one thread in this
	// function.
	TEE_DEV_OPENED = 0;
	return 0;
};

ssize_t device_read(struct file *file, char __user *buffer, size_t length,
	loff_t *offset)
{
	size_t bytes_read = 0;
	// Get the maximum of the kernel and user space buffer size; Don't copy 
	// more bytes.
	size_t bytes_to_copy = 
		(length < TEE_CHARDEV_OFFSET) ? length : TEE_CHARDEV_OFFSET;
	// Pointer to the buffer for pointer arithmetic
	char *crdev_buffer_ptr = TEE_CHARDEV_BUFFER + (*offset);

	pr_info("%s called, Offset: %lld\n", __FUNCTION__, (*offset));

	// For continues read we should check if we already read till the end.
	// In this case we return 0 to signal EOF.
	if (0 >= (TEE_CHARDEV_OFFSET - (*offset))) {
		(*offset) = 0;
		return 0;
	}

	while (0 < bytes_to_copy) {
		// We copy bytes between user and kernel space thus we need to do extra
		// work
		put_user(*(crdev_buffer_ptr++), buffer++);
		--bytes_to_copy;
		++bytes_read;
	}

	(*offset) += bytes_read;

	return bytes_read;
};

ssize_t device_write(struct file *file, const char __user *buffer, 
	size_t length, loff_t *offset)
{
	char *buffered = memdup_user(buffer, length);

	pr_info("%s called", __FUNCTION__);
	pr_info("Received %ld; Offset %lld\n", length, *offset);
	
	while (0 < length--) {
		TEE_CHARDEV_OFFSET %= (TEE_DEVICE_BUFFER_SIZE -1);
		TEE_CHARDEV_BUFFER[TEE_CHARDEV_OFFSET++] = *buffered;
		pr_info("%c", *(buffered++));
	}
	kfree(buffered);
	return length;
};

char *tee_device_devnode_mode(const struct device *dev, umode_t *mode)
{
	pr_info("%s called", __FUNCTION__);
	*mode = USER_SPACE_PERMISSION;
	return NULL;
}