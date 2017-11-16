#include <linux/fcntl.h>
#include <linux/sys/mman.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>  // for threads
#include <linux/sched.h>  // for task_struct
#include <linux/time.h>   // for using jiffies 
#include <linux/timer.h>

#define FILEPATH "/tmp/mmapped.bin"
#define NUMINTS  (1000)
#define FILESIZE (NUMINTS * sizeof(int))



static struct task_struct *thread1;
static int umh_test( int);

int thread_fn(void); 
int thread_fn() {

unsigned long j0,j1;
int delay = 10*HZ;

int fd;

int *map;  /* mmapped array of int's */

fd = open(FILEPATH, O_RDONLY);
    if (fd == -1) {
	printk(KERN_INFO"Error opening file for reading");
	return 0;
    }
map = mmap(0, FILESIZE, PROT_READ, MAP_SHARED, fd, 0);
if(map == MAP_FAILED) {
	close(fd);
	printk(KERN_INFO"Erroe in maping the file");
	return 0; 
   }

/* Read the file int-by-int from the mmap
     */
    for (i = 1; i <=NUMINTS; ++i) {
	printk(KERN_INFO"%d: %d\n", i, map[i]);
    }

    if (munmap(map, FILESIZE) == -1) {
	printk(KERN_INFO"Error un-mmapping the file");
    }
    close(fd);

while(1){
	printk(KERN_INFO "In thread1");
	j0 = jiffies;
	j1 = j0 + delay;

	while (time_before(jiffies, j1))
        	schedule();
	if ( kthread_should_stop() == true)
	{
	  break;
	}

	 umh_test( j0);
	printk(KERN_INFO "after the calling fuction \n");
}

return 0;
}

int thread_init (void) {
   
    char  our_thread[8]="thread1";
    printk(KERN_INFO "in init");
    thread1 = kthread_create(thread_fn,NULL,our_thread);
    if((thread1))
        {
        printk(KERN_INFO "in if");
        wake_up_process(thread1);
        }

    return 0;
}


static int umh_test(int  mynum )
{
  char *argv[] = { "/home/obc/github/linux_profiling-/test_app/test", NULL };
  static char *envp[] = {
        "HOME=/",
        "TERM=linux",
	"SHELL=/bin/bash"
        "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL };
 
 return call_usermodehelper( argv[0], argv, envp, UMH_WAIT_PROC );
}

void thread_cleanup(void) {
 int ret;
 ret = kthread_stop(thread1);
 if(!ret)
  printk(KERN_INFO "Thread stopped");

}
MODULE_LICENSE("GPL");   
module_init(thread_init);
module_exit(thread_cleanup);

