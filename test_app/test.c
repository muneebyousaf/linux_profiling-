#include <stdio.h>
#include <stdlib.h>
int main(void){

 FILE *fptr;
    char name[20];
    int age;
    float salary;
 
    /*  open for writing */
    fptr = fopen("emp.txt", "w");
 
    if (fptr == NULL)
    {
        printf("File does not exists \n");
        return;
    }
    printf("Enter the name \n");
   // scanf("%s", name);
    fprintf(fptr, " my name is written \n ");
    
    fprintf(fptr, "My age is ok\n");
    
    fprintf(fptr, "Salary is good \n");
 	system("logger this is my first program"); 
    fclose(fptr);

}
