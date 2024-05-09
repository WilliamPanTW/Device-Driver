/**************************************************************
* Class::  CSC-415-01 Spring 2024
* Name:: Pan William
* Student ID:: 922867228
* GitHub-Name:: WilliamPanTW
* Project:: Assignment 6 – Device Driver
*
* File:: Cat.c
*
* Description:: Sample Device Driver 
*
**************************************************************/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/vmalloc.h>


#include <linux/slab.h>
#define MY_MAJOR 415
#define MY_MINOR 0
#define DEVICE_NAME "Cat"

int major ,minor;
char *kernel_buffer;

struct cdev my_cdev;
int actual_rx_size=0;

MODULE_AUTHOR("Pan William");
MODULE_DESCRIPTION("Cat meow meow");
MODULE_LICENSE("GPL");

// this write function increment the data structure count every time it is call 
// return how many bytes were passed in 
static ssize_t myWrite(struct file *fs , const char __user * buff , size_t hsize , loff_t * off){
    printk(KERN_INFO "Writing myWrite Meow! Meow!\n");
    printk(KERN_INFO "we wrote: %lu on write number\n",hsize);
    return hsize;
}

//copy data to user buffer
static ssize_t myRead(struct file *fs , char __user * buff , size_t hsize , loff_t * off){
    printk(KERN_INFO "Reading myRead Meow! Meow!\n");
    printk(KERN_INFO "we read: %lu on read number\n",hsize);
    return hsize;
}

//inode:linux directory entry 
//fs : file control block to track file when open 
static int myOpen(struct inode *inode , struct file *fs){
    //vmalloc allocat kernel heap 
    printk(KERN_INFO "Opening myOpen Meow! Meow!\n");

    return 0; 
}

static int myClose(struct inode * inode,struct file *file){
    //vfree(ds);
    printk(KERN_INFO "Closing myClose Meow! Meow!\n");
    return 0;
}


// this is a way to deal with device file where there may not 
//basically count how many time write was called 
static long myIoCtl(struct file *fs , unsigned int command,unsigned long arg){
    printk(KERN_INFO "I/O control myIoctl Meow! Meow!\n");

    //only support command 3
    if(command !=3){
        printk(KERN_INFO "fail in myIoctl\n");
        return -1;
    }

    //copy kernel memory to user space 
    // copy_to_user(count, &(ds->count),size)
    int bytesNotCopied=0;

    return bytesNotCopied;
}

//File operation core linux system support 
static struct file_operations fops = {
    .open = myOpen,
    .release = myClose,
    .write = myWrite,
    .read = myRead, 
    .unlocked_ioctl = myIoCtl, //back doork speciifc other command 
    .owner = THIS_MODULE
};


//creates a device mode in /dev, reutrn error if not made 
int init_module(void){
    int result,registers;
    dev_t devno;

    devno = MKDEV(MY_MAJOR , MY_MINOR);//make a development number 

    registers = register_chrdev_region(devno, 1 , DEVICE_NAME);//register this device 
    printk(KERN_INFO "Register chardev succeeded :%d\n",registers);

    cdev_init(&my_cdev,&fops);
    my_cdev.owner = THIS_MODULE;

    result = cdev_add(&my_cdev, devno,1 );

    if(result <0){
        printk(KERN_INFO "register chardev failed\n");
    }

    printk(KERN_INFO "loaded cat module from kernel meow meow\n");

    return result;
}

//unregistering and removing device from kernel 
void cleanup_module(){
    dev_t devno;
    devno = MKDEV(MY_MAJOR,MY_MINOR);
    unregister_chrdev_region(devno,1);
    cdev_del(&my_cdev);

    printk(KERN_INFO "unloaded cat module from kernel meow");
}