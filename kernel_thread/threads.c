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
#define NUMINTS  (1000)
#define FILESIZE (NUMINTS * sizeof(int))

#define MAX_COOKIE_LENGTH       PAGE_SIZE

static struct proc_dir_entry *proc_entry;
 
static char *cookie_pot;  // Space for fortune strings
 
static int cookie_index;  // Index to write next fortune
 
static int next_fortune;  // Index to read next fortune



 struct task_struct *thread1;
static int umh_test( int);

int thread_fn(void); 
int thread_fn() {

unsigned long j0,j1;
int delay = 10*HZ;
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
  

    int ret = 0 ;  
    char  our_thread[8]="thread1";
    printk(KERN_INFO "in init");
    thread1 = kthread_create(thread_fn,NULL,our_thread);
    if((thread1))
        {
        printk(KERN_INFO "in if");
        wake_up_process(thread1);
        }

	cookie_pot = (char *)vmalloc( MAX_COOKIE_LENGTH );

	if (!cookie_pot) {
 
    		ret = -ENOMEM;
 
  	} else {
		 memset( cookie_pot, 0, MAX_COOKIE_LENGTH );

		 proc_entry=proc_create( "fortune", 0644, NULL );
		
		 if (proc_entry == NULL) {
			 ret = -ENOMEM;
 
     			 vfree(cookie_pot);
 
      			printk(KERN_INFO "fortune: Couldn't create proc entry\n");
		}
		else{
			cookie_index = 0;
 
     			 next_fortune = 0;

			proc_entry->read_proc = fortune_read;
			proc_entry->write_proc = fortune_write;
			proc_entry->owner = THIS_MODULE;
			printk(KERN_INFO "fortune: Module loaded.\n");
 
		    }
	}

   

    return ret ;
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

vfree(cookie_pot);

}

ssize_t fortune_write( struct file *filp, const char __user *buff,
 
                        unsigned long len, void *data )
 
{
 
  int space_available = (MAX_COOKIE_LENGTH-cookie_index)+1;
 
 
 
  if (len > space_available) {
 
 
 
    printk(KERN_INFO "fortune: cookie pot is full!\n");
 
    return -ENOSPC;
 
 
 
  }
 
 
 
  if (copy_from_user( &cookie_pot[cookie_index], buff, len )) {
 
    return -EFAULT;
 
  }
 
 
 
  cookie_index += len;
 
  cookie_pot[cookie_index-1] = 0;
 
 
 
  return len;
 
}




int fortune_read( char *page, char **start, off_t off,
 
                   int count, int *eof, void *data )
 
{
 
  int len;
 
 
 
  if (off > 0) {
 
    *eof = 1;
 
    return 0;
 
  }
 
 
 
  /* Wrap-around */
 
  if (next_fortune >= cookie_index) next_fortune = 0;
 
 
 
  len = sprintf(page, "%s\n", &cookie_pot[next_fortune]);
 
 
 
  next_fortune += len;
 
 
 
  return len;
 
}
MODULE_LICENSE("GPL");   
module_init(thread_init);
module_exit(thread_cleanup);

