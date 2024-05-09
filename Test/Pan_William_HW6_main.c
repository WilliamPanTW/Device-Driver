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
    long n1,n2,n3;  
    char buffer[1024];

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

    // Read our kernel module 
    // ssize_t bytes_read = read(fd, buffer, 1024);
    // if (bytes_read < 0) {
    //     perror("Failed to read from the device");
    //     close(fd);
    //     return -1;
    // }

    // n1=write(fd,"testing",10);
    // n2=ioctl(fd,3,&info);
    // printf("wrote to %d that return %ld count %d - Rel val from ioctl:%ld\n",fd,n1,info,n2);

    close(fd);

    return 0;
}