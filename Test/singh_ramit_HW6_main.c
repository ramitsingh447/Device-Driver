/**************************************************************
* Class: CSC-415-spring2022
* Name: Ramit Singh
* Student ID:918213925  
* GitHub ID:ramitsingh447
* Project: Assignment 6 - Device Driver
*
* File: singh_ramit_HW6_main.c
*
* Description: A device driver that consist of finding the greater number than 0 
*
**************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define BUFFER_LENGTH 256
#define RD_VALUE _IOR('a', 'b', int)
#define WR_VALUE _IOW('a', 'a', int)

static char receive[BUFFER_LENGTH];

char *suffix(int n)
{
   switch (n % 100)
   {
   case 11:
   case 12:
   case 13:
      return "th";
   default:
      switch (n % 10)
      {
      case 1:
         return "st";
      case 2:
         return "nd";
      case 3:
         return "rd";
      default:
         return "th";
      }
   }
}

int main()
{
   int ret, fd; // integer return
   char stringToSend[BUFFER_LENGTH];
   int value, number; // integer value and number

   printf("*****Start Program*****\n"); // start of program

   fd = open("/dev/kernelmod", O_RDWR);
   if (fd < 0)
   {
      perror("ERROR: Failed to open the device."); // error to open to device
      return errno; // return error
   }

   printf("Get the nth number in the Fibonacci Sequence\n"); // prints out getting nth number of Fibonacci sequence
   printf("Type in an integer greater than 0: "); // prints out to type an integer bigger than 0
   scanf("%s", stringToSend);

   ret = write(fd, stringToSend, strlen(stringToSend));
   if (ret < 0)
   {
      perror("ERROR: Failed to write to the device."); // prints error failed to write the device
      return errno; // return error
   }

   ret = read(fd, receive, BUFFER_LENGTH); // returns read and buffer length
   if (ret < 0)
   {
      perror("ERROR: Failed to read from the device."); // prints out error and failed to read device
      return errno; // returns error
   }

   printf("The value you entered: %s\n", receive); // prints out value entered and string

   number = atoi(receive); // represents certain integer number

   if (number == 0)
   {
      printf("ERROR: Invalid entry of '%s'.\n", receive); // prints error invalid entry of string
      printf("Try again with an integer greater than 0.\n"); // prints to try again with bigger integer
      printf("*****End Program*****\n"); // prints end of program
      close(fd);
      return 0;
   }

   ret = ioctl(fd, WR_VALUE, &number);
   if (ret < 0)
   {
      perror("ERROR: Failed to write to the device."); // prints out error failed to write device
      return errno; // returns error
   }

   ret = ioctl(fd, RD_VALUE, &value);
   if (ret < 0)
   {
      perror("ERROR: Failed to read from the device."); // prints out error and failed to read device
      return errno; // return error
   }

   printf("The %d%s number in the Fibonacci Sequence is: %d\n", number, suffix(number), value); // prints out decimal and string of number in Fibonacci sequence
   printf("*****End Program*****\n"); // prints end of program
   close(fd);

   return 0;
}
