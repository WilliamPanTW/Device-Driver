/**************************************************************
* Class::  CSC-415-01 Spring 2024
* Name:: Pan William
* Student ID:: 922867228
* GitHub-Name:: WilliamPanTW
* Project:: Assignment 6 – Device Driver
*
* File:: cat.c
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


//another data strucutre 
static struct file_operations fops = {
    // .open = myOpen,
    // .release = myClose,
    // .write = my_Write,
    // .unlocked_ioctl = myIoCtl,
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

    printk(KERN_INFO "loaded cat module rom kernel meow meow\n");

    return result;
}

void cleanup_module(){
    dev_t devno;
    devno = MKDEV(MY_MAJOR,MY_MINOR);
    unregister_chrdev_region(devno,1);
    cdev_del(&my_cdev);

    printk(KERN_INFO "unloaded cat module from kernel meow");
}