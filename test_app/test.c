#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<string.h>

#define BUFFER_LENGTH 256               ///< The buffer length (crude but fine)
static char receive[BUFFER_LENGTH];     ///< The receive buffer from the LKM

#define NUMINTS  (1000)
#define FILESIZE (NUMINTS * sizeof(int))

//#define FILEPATH "/home/obc/github/linux_profiling-/test_app/mylog.txt"

#define FILEPATH "/home/obc/github/linux_profiling-/test_app/emp.txt"

int main(int argc, char **argv){

 FILE *fptr;
    char name[20];
    int age;
    float salary;
    int fd;
    int ret;
 
char stringToSend[BUFFER_LENGTH]= "Muhammad muneeb yousaf";
    /*  open for writing */
  fptr = fopen(FILEPATH, "wr+");

// fptr = fopen(FILEPATH,O_RDWR|O_CREAT);

	strcpy(stringToSend,argv[1]);
	//printf("The given arguments are %s \n", stringToSend);
 
    if (fptr == NULL)
    {
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
   ret = write(fd, stringToSend, strlen(stringToSend)); // Send the string to the LKM
   if (ret < 0){
      fprintf(fptr,"Failed to write the message to the device.");
      return ;
   }
 
 
   fprintf(fptr,"Reading from the device...\n");
   ret = read(fd, receive, BUFFER_LENGTH);        // Read the response from the LKM
   if (ret < 0){
      fprintf(fptr,"Failed to read the message from the device.");
      return 0;
   }
  fprintf(fptr,"The received message is: [%s]\n", receive);
  fprintf(fptr,"End of the program\n");


    fprintf(fptr, " my name is written \n ");
    
    fprintf(fptr, "My age is ok\n");
    
    fprintf(fptr, "Salary is good \n");
    system("logger this is my first program"); 
    fclose(fptr);

}


