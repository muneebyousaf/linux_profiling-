/*****************************************************************************
*  This example code shows how to use most of PAPI's High level functions    *
*  to start,count,read and stop on an event set. We use two preset events    *
*  here:                                                                     *
*     PAPI_TOT_INS: Total instructions executed in a period of time                  *
*     PAPI_TOT_CYC: Total cpu cycles in a period of time                             *
******************************************************************************/ 
#include <stdio.h>
#include <stdlib.h>
#include <papi.h>
#include "F_comm.c"
#include "thread_config.h"
#define NUM_EVENTS 4 
#define THRESHOLD 10000
#define ERROR_RETURN(retval) { fprintf(stderr, "Error %d %s:line %d: \n", retval,__FILE__,__LINE__);  exit(retval); }

#define FILEPATH1 "/home/obc/github/linux_profiling-/papi_example_highlevel/multi_log.txt"

/* stupid codes to be monitored */ 
void computation_mult()
{
   double tmp=1.0;
   int i=1;

	printf(" this multiafkldj \n");
   for( i = 1; i < THRESHOLD; i++ )
   {
      tmp = tmp*i;
   }
}
/* stupid codes to be monitored */ 
void computation_add()
{
   int tmp = 0;
   int i=0;
   for( i = 0; i < THRESHOLD; i++ )
   {
      tmp = tmp + i;
   }
}

unsigned long long counter_buffer[6];
int main(void)
{
   /*Declaring and initializing the event set with the presets*/

   /*************************************************************
    * Event_set1 contains events: Total cycles, Total instructions
    * executed, Total load/store instructions executed and Total 
    * branch instructions executed.
   **************************************************************/ 
     FILE *fptr;
	int i ; 

	fptr = fopen(FILEPATH1, "wr+");

	
   int Events_set1[3] = {PAPI_TOT_INS,PAPI_BR_INS,PAPI_LD_INS};

	/* initilizations of buffer*/
	for ( i = 0;  i< 6; i++){
		counter_buffer[i] = 0xFFFF;
	}
	

       

   /**************************************************************
    * Event_set22 contains events: L1 Data cache misses, L1 instruction 
    * cache misses, L2 Data cache misses and L2 instruction cache misses
   **************************************************************/ 
   int Events_set2[2] = { PAPI_L1_DCM,PAPI_L1_ICM};
				   
   /*The length of the events array should be no longer than the
     value returned by PAPI_num_counters.*/     
       
   /*declaring place holder for no of hardware counters */
   int num_hwcntrs = 0;
   int retval;
   char errstring[PAPI_MAX_STR_LEN];
   /*This is going to store our list of results*/
   long long values[NUM_EVENTS];
	
   /* these variables are used for the timing measurement*/
  long long start_cycles, end_cycles, start_usec, end_usec;
   /***************************************************************************
   *  This part initializes the library and compares the version number of the*
   * header file, to the version of the library, if these don't match then it *
   * is likely that PAPI won't work correctly.If there is an error, retval    *
   * keeps track of the version number.                                       *
   ***************************************************************************/
   if((retval = PAPI_library_init(PAPI_VER_CURRENT)) != PAPI_VER_CURRENT )
   {
      fprintf(stderr, "Error: %d %s\n",retval, errstring);
      exit(1);
   }


		 
   /**************************************************************************
    * PAPI_num_counters returns the number of hardware counters the platform *
    * has or a negative number if there is an error                          *
    **************************************************************************/
   if ((num_hwcntrs = PAPI_num_counters()) < PAPI_OK)
   {
      fprintf(fptr,"There are no counters available. \n");
      exit(1);
   }
   fprintf(fptr,"There are %d counters in this system\n",num_hwcntrs);
               
   /**************************************************************************
    * PAPI_start_counters initializes the PAPI library (if necessary) and    *
    * starts counting the events named in the events array. This function    *
    * implicitly stops and initializes any counters running as a result of   *
    * a previous call to PAPI_start_counters.                                *
    **************************************************************************/
  
   /*start time sampling*/     
   start_cycles =PAPI_get_real_cyc();
   start_usec   = PAPI_get_real_usec();


   if ( (retval = PAPI_start_counters(Events_set1, 3)) != PAPI_OK)
       ERROR_RETURN(retval);
   fprintf(fptr,"\nCounter Started for the for multiplicatio: \n");
   /* Your code goes here*/
       
       
    computation_mult();
 
   /**********************************************************************
    * PAPI_read_counters reads the counter values into values array      *
    **********************************************************************/
   if ( (retval=PAPI_read_counters(values, 3)) != PAPI_OK)
      ERROR_RETURN(retval);
   fprintf(fptr,"Read successfully\n");


   /******************* PAPI_stop_counters **********************************/
   if ((retval=PAPI_stop_counters(values, 3)) != PAPI_OK)
      ERROR_RETURN(retval); 
  
    /* stop time sampling*/ 
   end_cycles =PAPI_get_real_cyc();
   end_usec   = PAPI_get_real_usec();

   fprintf(fptr,"counter are stopped \n");     

    counter_buffer[0] = values[0];
   fprintf(fptr,"The total instructions executed for multiplication are %lld \n",values[0]);
    counter_buffer[1] =  values[1];
   fprintf(fptr,"The total branch instructions executed for multiplication  are %lld \n",values[1]);
    counter_buffer[2] =  values[2];
   fprintf(fptr,"The total Load instructions executed for multiplication  are  %lld \n", values[2] );
   fprintf(fptr,"Wallclock cycles  : %lld\nWallclock time(us): %lld\n",end_cycles-start_cycles,end_usec-start_usec);

   file_handler(counter_buffer); /* Send data to excnage with the kernel*/
	fclose(fptr);

   exit(0);     
}
