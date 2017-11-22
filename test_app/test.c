#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<string.h>

#define BUFFER_LENGTH 6               ///< The buffer length (crude but fine)

#define FILEPATH "/home/obc/github/linux_profiling-/test_app/emp.txt"

int main(int argc, char **argv){

 FILE *fptr;
 int fd;
 int ret;
 unsigned long long int  stringToSend[BUFFER_LENGTH]={0,1,2,3,4,5};
   
 /*open file for the logging of messages*/
 fptr = fopen(FILEPATH, "wr+");

 if (fptr == NULL) {
        printf( "File does not exists \n");
        return;
  }
  
 fprintf(fptr,"Starting device test code example...\n");
 fd = open("/dev/ebbchar", O_RDWR);             // Open the device with read/write access
  if (fd < 0){
     //error("Failed to open the device...");
     fprintf(fptr," failed to open the device \n");
      return 1 ;
  }
  ret = write(fd, stringToSend, BUFFER_LENGTH); // Send the string to the LKM
  if (ret < 0){
     fprintf(fptr,"Failed to write the message to the device.");
      return 1;
  }
  /**************************************/ 
  /*****Read back from kernel*************/
  /*************************************/

 /*fprintf(fptr,"Reading from the device...\n");
   ret = read(fd, receive, BUFFER_LENGTH);        // Read the response from the LKM
   if (ret < 0){
      fprintf(fptr,"Failed to read the message from the device.");
      return 0;
   }
  fprintf(fptr,"The received message is: [%s]\n", receive);
  fprintf(fptr,"End of the program\n");
 */

    
    
  fclose(fptr);

 return 0 ;

}


