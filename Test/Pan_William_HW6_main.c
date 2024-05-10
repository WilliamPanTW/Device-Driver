/**************************************************************
* Class::  CSC-415-01 Spring 2024
* Name:: Pan William
* Student ID:: 922867228
* GitHub-Name:: WilliamPanTW
* Project:: Assignment 6 – Device Driver
*
* File:: Pan_William_HW6_main.c
*
* Description:: test user applicatio for Device Driver 
*
**************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

//unique number for I/O control 
#define IOCTL_SET_MODE_ENCRYPT _IOR('h', 1, unsigned long)
#define IOCTL_SET_MODE_DECRYPT _IOR('h', 2, unsigned long)

int main(int argc , char * argv[]){
    int fd;             //file descriptor 
    long command;       //store user command for I/O control
    char text[500];     //store string from user 
    char buffer[1024];  //store string from kernel

    // open our kernel module 
    fd=open("/dev/Cat",O_RDWR);
    if(fd<0){
        printf("Device open error\n");
        perror("Device file open error");
        return -1;
    }else{
        printf("Device Open success\n");
    }
    
    printf("Welcome to cat Caesar Cipher\n");
    printf("1. Encrypt\n2. Decrypt\n3. exit\n");
    printf("Please enter number(1-3) of your service: ");
    scanf("%ld", &command);
    //decide what command user enter and assign the unique number for I/O control
    switch (command) {
        case 1:            
            printf("Enter a message to encrypt: ");
            scanf("%s", text);
            command = IOCTL_SET_MODE_ENCRYPT;        
            break;
        case 2:
            printf("Enter a message to decrypt: ");
            scanf("%s", text);
            command = IOCTL_SET_MODE_DECRYPT;        
            break;  
        case 3:
            printf("bye , Meow~ \n");
            return 0;  
        default:
            printf("unrecognized service. Bye Meow~ \n");
            return 0;
    }   

    // Write data from user space to kernel space 
    ssize_t bytes_written = write(fd, text, strlen(text));
    if (bytes_written < 0) {
        perror("Failed to write to the kernel space");
        close(fd);
        return -1;
    }else{
        // printf("write %ld to device success\n",bytes_written);//success
    }
    
    //call I/O control from file operation 
    long control = ioctl(fd, command, 0);
    //if it success meaning it already encrypt/decrypt data
    if ( control!= 0) {
        perror("Failed to set encrypt/decrypt mode");
        close(fd);
        return -1;
    }else{
        //Read data form kernel space to user space 
        ssize_t bytes_read = read(fd, buffer,sizeof(buffer));
        if (bytes_read < 0) {
            perror("Failed to read from the kernel space");
            close(fd);
            return -1;
        }else{
            // printf("Device read success\n");
            // printf("Bytes read: %d\n", (int)bytes_read);
            printf("Your output: %s\n", buffer);
        }
    }


    printf("Service charge $1000, Meow! \n");
    close(fd); //close file discriptor and triggers the kernel to call the .release

    return 0;
}