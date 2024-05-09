/**************************************************************
* Class::  CSC-415-01 Spring 2024
* Name:: Pan William
* Student ID:: 922867228
* GitHub-Name:: WilliamPanTW
* Project:: Assignment 6 – Device Driver
*
* File:: Pan_William_HW6_main.c
*
* Description:: Sample Device Driver 
*
**************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int main(int argc , char * argv[]){
    int fd , info;
    char text[500], buffer[1024];

    int key , command;
    printf("Welcome to cat Caesar Cipher\n");
    printf("1. Encrypt\n2. decrypt\n3. exit\n");
    printf("Please enter number of your service: ");
    scanf("%d", & command);
    switch (command) {
        case 1:            
            printf("Enter a message to encrypt: ");
            scanf("%s", text);
            printf("Enter the key(0-26): ");
            scanf("%d", & key);
            break;
        case 2:
            printf("Enter a message to decrypt: ");
            scanf("%s", text);
            printf("Enter the key(0-26): ");
            scanf("%d", & key);
            break;  
        case 3:
            printf("bye , Meow~ \n");
            return 0;  
        default:
            printf("unrecognized service. Bye Meow~ \n");
            return 0;
    }   
    printf("-------------text: %s and size: %ld--------------\n",text,sizeof(text));

    // open our kernel module 
    fd=open("/dev/Cat",O_RDWR);
    printf("return from open file %d \n",fd);
    if(fd<0){
        printf("Device open error\n");
        perror("Device file open error");
        return -1;
    }else{
        printf("Device Open success\n");
    }


    // Switch to encrypt decrypt mode
    if (ioctl(fd, command, 0) < 0) {
        perror("Failed to set encrypt mode");
        close(fd);
        return -1;
    }else{
        printf("Device I/O control success\n");
    }


    // // Read our kernel module 
    // ssize_t bytes_read = read(fd, buffer,sizeof(buffer));
    // if (bytes_read < 0) {
    //     perror("Failed to read from the device");
    //     close(fd);
    //     return -1;
    // }else{
    //     printf("Device read success txt\n");
    // }

    // // Write our device driver 
    // const char *message = "Hello, Device!"; // miss paremeter loff_t * off
    // ssize_t bytes_written = write(fd, message, strlen(message));
    // if (bytes_written < 0) {
    //     perror("Failed to write to the device");
    //     close(fd);
    //     return -1;
    // }else{
    //     printf("Device write success\n");
    // }

    close(fd);

    return 0;
}