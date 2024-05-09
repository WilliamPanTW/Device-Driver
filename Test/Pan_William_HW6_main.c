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
    char text[500]; //store string from user 
    char buffer[1024];//store string from kernel

    int key , command;
    printf("Welcome to cat Caesar Cipher\n");
    printf("1. Encrypt\n2. decrypt\n3. exit\n");
    printf("Please enter number of your service: ");
    scanf("%d", & command);
    switch (command) {
        case 1:            
            printf("Enter a message to encrypt: ");
            scanf("%s", text);
            // printf("Enter the key(1-25): ");
            // scanf("%d", & key);
            break;
        case 2:
            printf("Enter a message to decrypt: ");
            scanf("%s", text);
            // printf("Enter the key(1-25): ");
            // scanf("%d", & key);
            break;  
        case 3:
            printf("bye , Meow~ \n");
            return 0;  
        default:
            printf("unrecognized service. Bye Meow~ \n");
            return 0;
    }   

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
    // if (ioctl(fd, command, 0) < 0) {
    //     perror("Failed to set encrypt mode");
    //     close(fd);
    //     return -1;
    // }else{
    //     printf("Device I/O control success\n");
    // }

    // Write our device driver 
    ssize_t bytes_written = write(fd, text, strlen(text));
    if (bytes_written < 0) {
        perror("Failed to write to the device");
        close(fd);
        return -1;
    }else{
        printf("write %ld to device success\n",bytes_written);
    }

    //Read our kernel module 
    ssize_t bytes_read = read(fd, buffer,sizeof(buffer));
    if (bytes_read < 0) {
        perror("Failed to read from the device");
        close(fd);
        return -1;
    }else{
        printf("Device read success\n");
        printf("Bytes read: %d\n", (int)bytes_read);
        printf("buffer output: %s\n", buffer);
    }





    close(fd);

    return 0;
}