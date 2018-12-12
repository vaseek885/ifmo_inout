#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
 
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>

static dev_t first;
static struct cdev c_dev;
static struct class *cl;
static struct file* open_file = NULL;

static unsigned long long global_offset = 0;

struct file *file_open(const char *path, int flags, int rights)
{
  printk(KERN_INFO "file_open");
  struct file *filp = NULL;
  mm_segment_t oldfs;
  int err = 0;
 
  oldfs = get_fs();
  set_fs(get_ds());
  filp = filp_open(path, flags | O_RDWR, rights);
  set_fs(oldfs);
  if (IS_ERR(filp)) {
    err = PTR_ERR(filp);
    return NULL;
  }
  return filp;
}
 
void file_close(struct file *file)
{
  printk(KERN_INFO "file_close");
  filp_close(file, NULL);
}
 
int file_read(struct file *file, unsigned long long offset, unsigned char *data, unsigned int size)
{
  mm_segment_t oldfs;
  int ret;
 
  oldfs = get_fs();
  set_fs(get_ds());
 
  ret = vfs_read(file, data, size, &offset);
 
  set_fs(oldfs);
  return ret;
}
 
int file_write(struct file *file, unsigned long long offset, unsigned char *data, unsigned int size)
{
  printk(KERN_INFO "file_write");

  mm_segment_t oldfs;
  int ret;
 
  oldfs = get_fs();
  set_fs(get_ds());
 
  ret = vfs_write(file, data, size, &offset);
  set_fs(oldfs);

  if (EBADF == ret) {
    printk(KERN_INFO "EBADF");
    return 0;
  }

  return ret;
}
 
int file_sync(struct file *file)
{
  vfs_fsync(file, 0);
  return 0;
}
 
 
static int my_open(struct inode * i, struct file * f)
{
  printk(KERN_INFO "Driver: open()\n");
  return 0;
}
static int my_close(struct inode *i, struct file *f) {
  printk(KERN_INFO "Driver: close()\n");
  return 0;
}
static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
  printk(KERN_INFO "Driver: read()\n");
  return 0;
}

int is_digit(char ch) {
  return '0' <= ch && ch <= '9';
}

int ch_to_int(char ch) {
  return (int) (ch - '0');
}

int sum(char *string) {
  int sum = 0;
  size_t i = 0;
  while (string[i] != '\0') {
    int acc = 0;

    while (is_digit(string[i])) {
      acc *= 10;
      acc += ch_to_int(string[i]);
      i++;
    }

    sum += acc;

    while (!is_digit(string[i]) && string[i] != '\0')
      i++;
  }
  return sum;
}

static ssize_t my_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
  printk(KERN_INFO "my_write");
  char char_buf[50];
  char path[50];
  if (len > 50)
    return 0;
  copy_from_user(&char_buf, buf, len);
  char_buf[len - 1] = '\0';

  if (sscanf(char_buf, "open %s", path)) {
    printk(KERN_INFO "Driver:%d write(%s)\n", len , path);
    open_file = file_open(path, O_CREAT | O_TRUNC | O_RDWR , S_IRWXO | S_IRWXU);
    return len;
  }
 
  if (strcmp("close", char_buf) == 0) {
    if(open_file != NULL) {
      printk(KERN_INFO "Driver: closing file");
      file_close(open_file);
      open_file = NULL;
    }
    else {
      printk(KERN_INFO "Driver: no open file to close");
    }
    return len;
  }

  if (open_file != NULL) {
    int length = sprintf(char_buf, "%d\n", sum(char_buf));
    file_write(open_file, global_offset, char_buf, length);
    global_offset += length;
  }
  else {
    printk(KERN_INFO "Driver: open_file == NULL ;(");
  }
  return len;
}
static struct file_operations mychdev_fops = {
  .owner = THIS_MODULE,
  .open = my_open,
  .release = my_close,
  .read = my_read,
  .write = my_write
};
static int __init ch_drv_init(void)
{
  printk(KERN_INFO "Hello!\n");
  if (alloc_chrdev_region(&first, 0, 1, "ch_dev") < 0)
  {
    return -1;
  }
  if ((cl = class_create(THIS_MODULE, "chardrv")) == NULL)
  {
    unregister_chrdev_region(first, 1);
    return -1;
  }
  if (device_create(cl, NULL, first, NULL, "var8") == NULL)
  {
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    return -1;
  }
  cdev_init(&c_dev, &mychdev_fops);
  if (cdev_add(&c_dev, first, 1) == -1)
  {
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    return -1;
  } return 0;
}
static void __exit ch_drv_exit(void)
{
  cdev_del(&c_dev);
  device_destroy(cl, first);
  class_destroy(cl);
  unregister_chrdev_region(first, 1);
  printk(KERN_INFO "Bye!!!\n");
}
 
 
 
 
 
module_init(ch_drv_init);
module_exit(ch_drv_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Author");
MODULE_DESCRIPTION("The first kernel module");

