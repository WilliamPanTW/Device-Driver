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

struct file_data {
	int encryptionKey;
    char mybuffer[];
};

// Function to encrypt the message by shifting each character by 2 spots
void encrypt(char *message) {
    while (*message != '\0') {
        if (*message >= 'a' && *message <= 'z') {
            *message = (*message - 'a' + 2) % 26 + 'a';
        } else if (*message >= 'A' && *message <= 'Z') {
            *message = (*message - 'A' + 2) % 26 + 'A';
        }
        message++;
    }
}

// Function to decrypt the message by shifting each character back by 2 spots
void decrypt(char *message) {
    while (*message != '\0') {
        if (*message >= 'a' && *message <= 'z') {
            *message = (*message - 'a' - 2 + 26) % 26 + 'a';
        } else if (*message >= 'A' && *message <= 'Z') {
            *message = (*message - 'A' - 2 + 26) % 26 + 'A';
        }
        message++;
    }
}

// this write function increment the data structure count every time it is call 
// return how many bytes were passed in 
static ssize_t myWrite(struct file *fs , const char __user * buff , size_t hsize , loff_t * off){
    struct file_data * data;
    data = (struct file_data *)fs->private_data;
    int encrpytionkey = data->encryptionKey;
    char * message; 

    message = kmalloc(hsize + 1, GFP_KERNEL); // Allocate memory. +1 for the null terminator    
    if (copy_from_user(message, buff, hsize)) {
        kfree(message); // Free the allocated memory
        return -1;
    }

    strcpy(data->mybuffer, message);
    kfree(message);

    printk(KERN_INFO "myWrite %s get shift %d Meow! Meow!\n",data->mybuffer,encrpytionkey);

    return hsize;
}

//copy data to user buffer
static ssize_t myRead(struct file *fs , char __user * buff , size_t hsize , loff_t * off){
    struct file_data * data;
    data = (struct file_data *)fs->private_data;
    // int encrpytionkey = data->encryptionKey;
    encrypt(data->mybuffer);
    //Copy the message to the user
    if (copy_to_user(buff, data->mybuffer, strlen(data->mybuffer)+1)){
        printk(KERN_INFO "fail to copy to user Meow! Meow!\n");
        return -1;
    }else{
        printk(KERN_INFO "MyRead service, text: %s Meow! Meow!\n",data->mybuffer);
    }

    return strlen(data->mybuffer) + 1;
}

//inode:linux directory entry 
//fs : file control block to track file when open 
static int myOpen(struct inode *inode , struct file *fs){
    //vmalloc allocat kernel heap 
    struct file_data * data;
    data = vmalloc(sizeof(struct file_data));

    if(!data){
        printk(KERN_INFO "cannot vmalloc, failed to opened Meow! Meow!\n");    
        return -1;
    }
    
    data->encryptionKey=3;

    //New Device Values Set to void pointer
    fs->private_data=data;

    printk(KERN_INFO "Opening myOpen Meow! Meow!\n");

    return 0; 
}

static int myClose(struct inode * inode,struct file *fs){
    struct file_data * data;
    data = (struct file_data *) fs->private_data;
    vfree(data);
    printk(KERN_INFO "Closing myClose Meow! Meow!\n");
    return 0;
}


// this is a way to deal with device file where there may not 
//basically count how many time write was called 
static long myIoCtl(struct file *fs , unsigned int command,unsigned long arg){
    // int * count;
    struct file_data * data;
    data = (struct file_data *) fs->private_data;
    
    printk(KERN_INFO "I/O control myIoctl %d Meow! Meow!\n",command);

    switch (command){
        case 1:            
            printk(KERN_INFO "----Meow! Meow! encrypt updated-----\n");
            break;
        case 2:
            printk(KERN_INFO "----Meow! Meow! Decrypt updated----\n");
            break;
        default:
            printk(KERN_INFO "----fail in myIoctl updated----\n");
            return -1;
    }   

    // //copy kernel memory to user space 
    // count = (int *)data;
    // int bytesNotCopied=copy_to_user(count,&(data->encryptionKey),sizeof(struct file_data));

    return 0;
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

    devno = MKDEV(MY_MAJOR , MY_MINOR);//make a development device number 

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