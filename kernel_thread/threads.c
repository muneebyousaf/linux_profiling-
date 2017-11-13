#include <linux/kmod.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>  // for threads
#include <linux/sched.h>  // for task_struct
#include <linux/time.h>   // for using jiffies 
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/gfp.h>
static struct task_struct *thread1;
static int umh_test( void );


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



void thread_cleanup(void) {
 int ret;
 ret = kthread_stop(thread1);
 if(!ret)
  printk(KERN_INFO "Thread stopped");

}


static int __init mod_entry_func( void )
{
  return umh_test();
}

static void __exit mod_exit_func( void )
{
  return;
}

 
static int umh_test( void )
{
char *argv[] = { "/usr/bin/logger", "help!", NULL };
  static char *envp[] = {
        "HOME=/",
        "TERM=bash",
        "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL };
 
  return call_usermodehelper( argv[0], argv, envp, UMH_WAIT_PROC );
}

MODULE_LICENSE("GPL");   
//module_init(thread_init);
//module_exit(thread_cleanup);
module_init( mod_entry_func );
module_exit( mod_exit_func );

