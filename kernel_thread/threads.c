#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>  // for threads
#include <linux/sched.h>  // for task_struct
#include <linux/time.h>   // for using jiffies 
#include <linux/timer.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <linux/device.h>
#define  DEVICE_NAME "UTK_Buffer"    ///UTK: User To Kernel<The device will appear at /dev/ebbchar using this value
#define  CLASS_NAME  "UTK"       ///< The device class -- this is a character device driver

MODULE_LICENSE("GPL");            ///< The license type -- this affects available functionality
MODULE_AUTHOR("Muhammad muneeb yousaf");    ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("This module is being created for communicatin between kernel and user");  ///< The description -- see modinfo
MODULE_VERSION("0.1");            ///< A version number to inform users

unsigned int t2_counter= 0; 
struct task_struct *thread1;

struct task_struct *thread2;
static int Hook_User_App(void);

int thread_fn(void); 
int thread_fn2(void); 
/* all these data sturcture are for communication with file*/

static int    majorNumber;                  ///< Stores the device number -- determined automatically
unsigned long long int message[6]= {0};
static short  size_of_message;              ///< Used to remember the size of the string stored
static int    numberOpens = 0;              ///< Counts the number of times the device is opened
static struct class*  ebbcharClass  = NULL; ///< The device-driver class struct pointer
static struct device* ebbcharDevice = NULL; ///< The device-driver device struct pointer


// The prototype functions for the character driver -- must come before the struct definition
static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const unsigned long long  *, size_t, loff_t *);


/** @brief Devices are represented as file structure in the kernel. The file_operations structure from
 *  /linux/fs.h lists the callback functions that you wish to associated with your file operations
 *  using a C99 syntax structure. char devices usually implement open, read, write and release calls
 */
static struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};

/***********************************************************/
/* **************** function related to first thread*******/
/**********************************************************/


int thread_fn2() {

 unsigned long j0,j1;
 int delay = 20*HZ;
int a =10;
char buffer[20];
char mythread[20]="thread";

while(1){
	
		
	sprintf(buffer,"%d",a);

	strcat(mythread,buffer);
	printk(KERN_INFO " Result of cancatination%s\n",mythread);
	printk(KERN_INFO "In thread2\n");
	j0 = jiffies;
	j1 = j0 + delay;

	while (time_before(jiffies, j1))
        	schedule();


	if ( kthread_should_stop() == true)
	{
	  break;
	}
	t2_counter++;
	printk(KERN_INFO " It is from thread2 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

 
 }
 return 0; 
}
int thread_fn() {

unsigned long j0,j1;
int delay = 20*HZ;
unsigned char ab ;
char our_thread2[8]="Thread2";

unsigned char thread2_running_flag = 0;

int ret2;
 
while(1){
	printk(KERN_INFO "In thread1\n");
	j0 = jiffies;
	j1 = j0 + delay;

	while (time_before(jiffies, j1))
        	schedule();

	if ((message[0] == 3)&& (thread2_running_flag == 2)){

		thread2_running_flag = 0;
	} 
	
	/**********Create Thread2**********************/
	if(thread2_running_flag == 0U ){
	 	thread2= kthread_create(thread_fn2,NULL,our_thread2);
    		if((thread2)){
        		wake_up_process(thread2);
			thread2_running_flag = 1; 
        		printk(KERN_INFO "Thread 1 has started running\n");
		}
    	}
	else
        printk(KERN_INFO "Thread 2 not created \n");

	/**************************************************/
	/****Stop thread2********************************/

	if(t2_counter >= 2){

		ret2 = kthread_stop(thread2);
		thread2_running_flag = 2; 
 		if(!ret2)
 		 printk(KERN_INFO "Thread2 stopped: Good by from thread2 \n");
		t2_counter = 0 ;
	}


	if ( kthread_should_stop() == true)
	{
	  break;
	}

	printk(KERN_INFO" The message receive from user: ");
	for (ab = 0 ; ab < 6 ; ab++){ 
		printk(KERN_INFO"%llx\n",message[ab]);
	}
	
	/* Call user Space APP*/
	Hook_User_App(); 

 
	printk(KERN_INFO "Return from the user space \n");
 }

 return 0;
}



int thread_init (void) {
     
    char  our_thread[8]="thread1";
    printk(KERN_INFO "Init has started \n");


    thread1 = kthread_create(thread_fn,NULL,our_thread);
    if((thread1)){
        wake_up_process(thread1);
        printk(KERN_INFO "Thread 1 has started running\n");
    }

/* This is being added for communication through file*/
/*****************************************************/

  printk(KERN_INFO "UTK_Buffer: Initializing the UTK_Buffer parameters \n");
 
   // Try to dynamically allocate a major number for the device -- more difficult but worth it
   majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
   if (majorNumber<0){
      printk(KERN_ALERT "UTK_Buffer failed to register a major number\n");
      return majorNumber;
   }

 

  printk(KERN_INFO "UTK_Buffer: registered correctly with major number %d\n", majorNumber);
 
   // Register the device class
   ebbcharClass = class_create(THIS_MODULE, CLASS_NAME);
   if (IS_ERR(ebbcharClass)){                // Check for error and clean up if there is
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to register device class\n");
      return PTR_ERR(ebbcharClass);          // Correct way to return an error on a pointer
   }
   printk(KERN_INFO "UTK_Buffer: device class registered correctly\n"); 


   // Register the device driver
   ebbcharDevice = device_create(ebbcharClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
   if (IS_ERR(ebbcharDevice)){               // Clean up if there is an error
      class_destroy(ebbcharClass);           // Repeated code but the alternative is goto statements
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to create the device\n");
      return PTR_ERR(ebbcharDevice);
   }
   printk(KERN_INFO "UTK_Buffer: device class created correctly\n"); // Made it! device was initialized

/*****************************************************/
   

    return 0;
}


/**********************************************************/
/**********User Space Caller function *********************/
/*********************************************************/

static int Hook_User_App() 
{
  char *argv[] = { "/home/obc/github/linux_profiling-/test_app/test"," I am feeling Great !!!",NULL };

  static char *envp[] = {
        "HOME=/",
        "TERM=linux",
	"SHELL=/bin/bash"
        "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL };
 
 return call_usermodehelper( argv[0], argv, envp, UMH_WAIT_PROC );
}



/*******************************************************************/
/******* Remove module, threads and inits stuff from the kernel*****/
/******************************************************************/

void thread_cleanup(void) {
 int ret;
 ret = kthread_stop(thread1);
 if(!ret)
  printk(KERN_INFO "Thread stopped");
  /*********************************************************/
  /****FILE Removal*****************************************/
  /********************************************************/

  device_destroy(ebbcharClass, MKDEV(majorNumber, 0));     // remove the device
   class_unregister(ebbcharClass);                          // unregister the device class
   class_destroy(ebbcharClass);                             // remove the device class
   unregister_chrdev(majorNumber, DEVICE_NAME);             // unregister the major number
   printk(KERN_INFO "UTK_buffer: Goodbye from the from ITU test bench!\n");

}
/*********************************************/
/* File communication related function    */
/******************************************/

/** @brief The device open function that is called each time the device is opened
 *  This will only increment the numberOpens counter in this case.
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_open(struct inode *inodep, struct file *filep){
   numberOpens++;
   printk(KERN_INFO "UTK_Buffer: Buffer has been opened %d time(s)\n", numberOpens);
   return 0;
}


/** @brief This function is called whenever device is being read from user space i.e. data is
 *  being sent from the device to the user. In this case is uses the copy_to_user() function to
 *  send the buffer string to the user and captures any errors.
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 *  @param buffer The pointer to the buffer to which this function writes the data
 *  @param len The length of the b
 *  @param offset The offset if required
 */
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
   int error_count = 0;
   // copy_to_user has the format ( * to, *from, size) and returns 0 on success
   error_count = copy_to_user(buffer, message, size_of_message);
 
   if (error_count==0){            // if true then have success
      printk(KERN_INFO "EBBChar: Sent %d characters to the user\n", size_of_message);
      return (size_of_message=0);  // clear the position to the start and return 0
   }
   else {
      printk(KERN_INFO "EBBChar: Failed to send %d characters to the user\n", error_count);
      return -EFAULT;              // Failed -- return a bad address message (i.e. -14)
   }
}
 


/** @brief This function is called whenever the device is being written to from user space i.e.
 *  data is sent to the device from the user. The data is copied to the message[] array in this
 *  LKM using the sprintf() function along with the length of the string.
 *  @param filep A pointer to a file object
 *  @param buffer The buffer to that contains the string to write to the device
 *  @param len The length of the array of data that is being passed in the const char buffer
 *  @param offset The offset if required
 */
static ssize_t dev_write(struct file *filep, const unsigned long long  *buffer, size_t len, loff_t *offset){
unsigned int i; 

for ( i = 0; i < len ; i++){

	message[i] = buffer[i]; /* Copying the user data into the kernel buffer*/
}

printk(KERN_INFO "UTK_Buffer: Received %zu characters from the user\n", len);

   return len;
}
 
/** @brief The device release function that is called whenever the device is closed/released by
 *  the userspace program
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_release(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "UTK_Buffer:Buffer successfully closed\n");
   return 0;
}



MODULE_LICENSE("GPL");   
module_init(thread_init);
module_exit(thread_cleanup);

