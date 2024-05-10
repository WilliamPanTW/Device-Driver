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
//unique number for I/O control 
#define IOCTL_SET_MODE_ENCRYPT _IOR('h', 1, unsigned long) 
#define IOCTL_SET_MODE_DECRYPT _IOR('h', 2, unsigned long)

int major ,minor;

struct cdev my_cdev;
int actual_rx_size=0;

MODULE_AUTHOR("Pan William");
MODULE_DESCRIPTION("Cat meow meow");
MODULE_LICENSE("GPL");

struct file_data {
	int encryptionKey;//for feature implement for user to set up key to shift 
    char mybuffer[]; // store the encrypt/decrypt data 
};

// Function to encrypt the message by shifting each character by 2 spots
void encrypt(char *message) {
    //loop until message reach null terminator
    while (*message != '\0') {
        //checks if it is a lowercase lette
        if (*message >= 'a' && *message <= 'z') {
            //X: Subtracting the corrisponing ASCII value to start from 0 for the alphabet.
            *message = (*message - 'a' + 2) % 26 + 'a';
        } else if (*message >= 'A' && *message <= 'Z') {
        //otherwise it is upper and Encrypt letter X by a right shift 2 spot : (X+2) mod 26
            *message = (*message - 'A' + 2) % 26 + 'A';
        }
        message++;
    }
}

// Function to decrypt the message by shifting each character back by 2 spots
void decrypt(char *message) {
    //loop until message reach null terminator 
    while (*message != '\0') {
        if (*message >= 'a' && *message <= 'z') {
        //X: Subtracting the corrisponing ASCII value to start from 0 for the alphabet.
            *message = (*message - 'a' - 2 + 26) % 26 + 'a';
        } else if (*message >= 'A' && *message <= 'Z') {
        //otherwise it is upper and decrypt letter X by a left shift 2 spot : (X-2) mod 26
            *message = (*message - 'A' - 2 + 26) % 26 + 'A';
        }
        message++;
    }
}

// Copy data from user space and store it in kernel space
static ssize_t myWrite(struct file *fs , const char __user * buff , size_t hsize , loff_t * off){
    struct file_data * data; //Define a pointer to a struct of type file_data
    data = (struct file_data *)fs->private_data;// assign it to private_data from fs
    char * message;//store the message 

    // Allocate memory. +1 for the null terminator    
    message = kmalloc(hsize + 1, GFP_KERNEL); // Allocate memory. +1 for the null terminator    
    if (copy_from_user(message, buff, hsize)) {
        kfree(message); // Free the allocated memory if fail
        return -1;
    }

    strcpy(data->mybuffer, message);// Copy the message to kernel buffer
    kfree(message);// Free the allocated memory

    printk(KERN_INFO "myWrite store : %s Meow! Meow!\n",data->mybuffer);

    return hsize;//return how many bytes were passed in 
}

//copy data from kernel space to user buffer
static ssize_t myRead(struct file *fs , char __user * buff , size_t hsize , loff_t * off){
    struct file_data * data; //Define a pointer to a struct of type file_data
    data = (struct file_data *)fs->private_data; //assign it to private_data from fs
    //Copy the message to the user
    if (copy_to_user(buff, data->mybuffer, strlen(data->mybuffer)+1)){
        printk(KERN_INFO "fail to copy to user Meow! Meow!\n");
        return -1;
    }else{
        printk(KERN_INFO "MyRead modified: %s Meow! Meow!\n",data->mybuffer);
    }

    return strlen(data->mybuffer) + 1;//Return the copied length with null terminator
}

//inode:linux directory entry 
//fs : file control block to track file when open 
static int myOpen(struct inode *inode , struct file *fs){
    //allocat kernel heap 
    struct file_data * data;
    data = vmalloc(sizeof(struct file_data));

    if(!data){
        printk(KERN_INFO "cannot vmalloc, failed to opened Meow! Meow!\n");    
        return -1;
    }
    
    data->encryptionKey=0;

    //New Device Values Set to void pointer
    fs->private_data=data;

    printk(KERN_INFO "Opening myOpen Meow! Meow!\n");

    return 0; 
}

//Deallocated the associate data in kerne
static int myClose(struct inode * inode,struct file *fs){
    struct file_data * data;
    data = (struct file_data *) fs->private_data;
    vfree(data);
    printk(KERN_INFO "Closing myClose Meow! Meow!\n");
    return 0;
}


//back doork of encrypt and decrypt command 
static long myIoCtl(struct file *fs , unsigned int command,unsigned long arg){
    struct file_data * data; //Define a pointer to a struct of type file_data
    data = (struct file_data *) fs->private_data; // assign it to private_data from fs
    
    printk(KERN_INFO "I/O control myIoctl %d Meow! Meow!\n",command);

    switch (command){
        case IOCTL_SET_MODE_ENCRYPT:     
            //If user choose to encrypt data
            printk(KERN_INFO "----Meow! Meow! encrypt-----\n");
            encrypt(data->mybuffer);
            break;
        case IOCTL_SET_MODE_DECRYPT:
            //If user choose to decrypt data
            printk(KERN_INFO "----Meow! Meow! Decrypt----\n");
            decrypt(data->mybuffer);
            break;
        default:
            printk(KERN_INFO "----fail in myIoctl----\n");
            return -1;
    }   

    return 0;
}

//File operation core linux system support 
static struct file_operations fops = {
    .open = myOpen,
    .release = myClose,
    .write = myWrite,
    .read = myRead, 
    .unlocked_ioctl = myIoCtl, 
    .owner = THIS_MODULE
};


//creates a device mode in /dev, reutrn error if not made 
int init_module(void){
    int result,registers;
    dev_t devno;

    devno = MKDEV(MY_MAJOR , MY_MINOR);//make a development device number 

    registers = register_chrdev_region(devno, 1 , DEVICE_NAME);//register this device 
    printk(KERN_INFO "Register chardev succeeded :%d\n",registers);

    //initialize the character device structure my_cdev with file operations (fops)
    cdev_init(&my_cdev,&fops);
    my_cdev.owner = THIS_MODULE;

    //Add the character device to the system
    result = cdev_add(&my_cdev, devno,1 );

    //Check if adding the character device was successful
    if(result <0){
        printk(KERN_INFO "register chardev failed\n");
    }

    printk(KERN_INFO "loaded cat module from kernel. Meow! Meow!\n");

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