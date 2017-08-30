#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/kthread.h>
#include <linux/wait.h>
#include <linux/delay.h>

#include <linux/file.h>
#include <linux/syscalls.h>
#include <linux/fcntl.h>
#include <asm/uaccess.h>
#include <linux/interrupt.h>

#include <asm/timex.h>
#include <asm/system.h>


#include "time_monitor.h"

MODULE_LICENSE("Dual BSD/GPL");


struct time_stats
{
   int id;
   unsigned long long timestamp;
   char name[TASK_COMM_LEN];
	int type;
	unsigned int num;
};

unsigned short int flags_pointer = 0x0000;

void attiva_puntatori(void);
void disattiva_puntatori(void);

void time_monitor_tick(int type,int id,char *name);
void time_monitor_dump(void);

struct time_stats *buffer_init = NULL, *buffer_current = NULL, *buffer_dump = NULL;

struct task_struct *dump_mythread = NULL;

int write_count = 0;
int read_count = 0;
unsigned int num=0;
int flag_overflow = OVERFLOW_BUFFER_NO;
unsigned int delta = 0;
int daddy_pid = -1;

static int time_monitor_opened = 0;

extern void (*ptr2time_monitor)          (int type,int id,char * name);
extern void (*ptr2time_monitor_interrupt)(int type,int id,char * name);
extern void (*ptr2time_monitor_exception)(int type,int id,char * name);
extern void (*ptr2time_monitor_softirq)  (int type,int id,char * name);
extern void (*ptr2time_monitor_exception_nmi)	(int type,int id,char * name);

extern void (*ptr2time_monitor_dump)(void);


// ioctl device
static int time_monitor_ioctl(struct inode *inode, struct file *filep, unsigned int ioctl_num, unsigned long ioctl_param)
{
   int rc = -EINVAL;
	struct time_stats * buffer_user_side = NULL;
	unsigned int * data = NULL;
	unsigned int num_elementi = 0;
	unsigned int to_dump_this_side = 0;
	
   switch(ioctl_num)
   {
      case TIME_MONITOR_IOCTL_OFF:
      {	  
			flags_pointer = (unsigned short int) ioctl_param;
			disattiva_puntatori();

         printk("time_monitor OFF\n");
         rc = 0;
         break;      
      }
      
      case TIME_MONITOR_IOCTL_ON:
      {
         if(ptr2time_monitor	|| ptr2time_monitor_interrupt || ptr2time_monitor_exception || ptr2time_monitor_softirq || ptr2time_monitor_exception_nmi ||ptr2time_monitor_dump)
         {
            printk("The monitor is already ON\n");
            break;
         }


			flags_pointer = (unsigned short int) ioctl_param;
			attiva_puntatori();
			
			write_count = 0;
		   read_count = 0;
			delta = 0;

			buffer_current = buffer_dump = buffer_init;

         printk("time_monitor ON\n");
            
         rc = 0;
         break;   
      }      
      
      case TIME_MONITOR_IOCTL_REGISTER_THREAD:
      {
         dump_mythread = current;
			daddy_pid = current->parent->pid;
         printk("dump_mythread has been registered\n\n");
         rc = 0;
         break;
      }      
      
      case TIME_MONITOR_IOCTL_UNREGISTER_THREAD:
      {
         dump_mythread = NULL;
         printk("dump_mythread has been unregistered\n\n");
         rc = 0;
         break;
      }         
      
      case TIME_MONITOR_IOCTL_THREAD_SLEEP:
      {
         //It puts the thread to sleep indefinitely
         set_current_state(TASK_INTERRUPTIBLE);
         schedule();
			rc = 0;
         break;
      }
      
      case TIME_MONITOR_IOCTL_DUMP:
      {
			unsigned long  flags;
			
			buffer_user_side = (struct time_stats *) ioctl_param;
			num_elementi = buffer_user_side -> num;
         
			if(likely(flag_overflow == OVERFLOW_BUFFER_NO)){

				if(read_count + num_elementi == MAX_STATS){
               
					copy_to_user(buffer_user_side, buffer_dump, (sizeof(struct time_stats) * num_elementi));
					local_irq_save(flags);
					buffer_dump = buffer_init;
					read_count = 0;
					local_irq_restore(flags);
				}
				else{

					if(read_count + num_elementi < MAX_STATS){
						copy_to_user(buffer_user_side, buffer_dump, (sizeof(struct time_stats) * num_elementi));
						local_irq_save(flags);
						buffer_dump += num_elementi;
						read_count += num_elementi;
						local_irq_restore(flags);
					}

					else{
						to_dump_this_side = MAX_STATS - read_count;	
						copy_to_user(buffer_user_side, buffer_dump, (sizeof(struct time_stats) * to_dump_this_side));
						local_irq_save(flags);
						buffer_dump = buffer_init;
						read_count = 0;
						local_irq_restore(flags);
                  
						copy_to_user((buffer_user_side + to_dump_this_side), buffer_dump, (sizeof(struct time_stats) * (num_elementi -to_dump_this_side)));


						local_irq_save(flags);

						buffer_dump += (num_elementi -to_dump_this_side);
						read_count += (num_elementi -to_dump_this_side);
						
						local_irq_restore(flags);
					}
				}
			}

			else{
				if(!read_count){
					buffer_dump = buffer_init;
					copy_to_user(buffer_user_side, buffer_dump, (sizeof(struct time_stats) * (MAX_STATS))); 
				}
				else{
					buffer_dump = buffer_init + read_count;
					copy_to_user(buffer_user_side, buffer_dump, (sizeof(struct time_stats) * (MAX_STATS - read_count)));
					buffer_dump = buffer_init;
					copy_to_user(buffer_user_side + (MAX_STATS - read_count), buffer_dump, (sizeof(struct time_stats) * (write_count)));

				printk("overflow resolved \n");

				write_count = read_count = 0;
				buffer_current = buffer_dump = buffer_init ;
				flag_overflow = OVERFLOW_BUFFER_NO;
				ptr2time_monitor_dump = NULL;
				attiva_puntatori();
				}
			
			}
					
         rc = 0;
         break;
      }
      
      case TIME_MONITOR_IOCTL_THREAD_WAKEUP:
      {
         if(dump_mythread)
            wake_up_process(dump_mythread);
         else
            printk("ERROR: the thread to be woken up doesn't exist!\n");
         
         rc = 0;
         break;
      }

		case TIME_MONITOR_IOCTL_SET_DELTA:
		{
			unsigned long flags;

			local_irq_save(flags);

			delta -= (unsigned int) ioctl_param;

			local_irq_restore(flags);

			rc = 0;
			break;
		}

		case TIME_MONITOR_IOCTL_READ_DELTA:
		{
			data = (unsigned int *) ioctl_param;
			*data = delta;
			rc = 0;
			break;
		}

		case TIME_MONITOR_IOCTL_SET_POINTERS:
		{
			flags_pointer = (unsigned short int ) ioctl_param;
			attiva_puntatori();
			break;
		}

      default:
         printk("Undefined ioctl number: %d\n", ioctl_num);
   }
   
   return rc;
}

void time_monitor_tick(int type,int id,char *name)
{ 
	int j = 0;
	int i= 0;	
	char overflow_string[16] = {'o','v','e','r','f','l','o','w','-','b','u','f','f','e','r'};
 
	if((write_count == read_count -1 ) || (write_count  == MAX_STATS-1  && read_count == 0)){
		printk("Buffer overflow %d %d\n",write_count , read_count);
		
		buffer_current  -> type = -1;
		buffer_current  -> num = num++;
		buffer_current  -> id = -1;
		buffer_current  -> timestamp = get_jiffies_64();
		for(i=0;i<TASK_COMM_LEN;i++)
					buffer_current  -> name[i] = (char) overflow_string[i];
		

		flag_overflow = OVERFLOW_BUFFER_YES;
		disattiva_puntatori();
		ptr2time_monitor_dump = time_monitor_dump;
		
	}

	else{

      buffer_current->type = type;
      buffer_current->num = num++;
      buffer_current->id = id;
      buffer_current->timestamp = get_jiffies_64();

   
      for(j=0; j < TASK_COMM_LEN; j++)
         buffer_current->name[j] = (char) name[j];

      
      ++delta;
      ++buffer_current;
      ++write_count ;
      if(write_count == MAX_STATS/2 || write_count == MAX_STATS){
         ptr2time_monitor_dump = time_monitor_dump;

         if(likely(write_count == MAX_STATS))
         {
            buffer_current = buffer_init;
            write_count =  0;
         }
      }
	}
}


void time_monitor_dump(void)
{
	
	unsigned long flags;
   	if(likely(dump_mythread)){

			if(likely(dump_mythread->state == TASK_INTERRUPTIBLE)){
				local_irq_save(flags);

         	wake_up_process(dump_mythread);

				ptr2time_monitor_dump = NULL;
				local_irq_restore(flags);
			}
		}
   	else{
      	printk("ERROR: the thread to be woken up doesn't exist!\n");
			force_sig(SIGKILL,find_task_by_pid(daddy_pid));
		}
return;
}


void attiva_puntatori(){
if((flags_pointer & SCHED_ACTIVE))		ptr2time_monitor = time_monitor_tick;
if((flags_pointer & INT_ACTIVE))			ptr2time_monitor_interrupt = time_monitor_tick;
if((flags_pointer & EXCEPTION_ACTIVE)) {
		ptr2time_monitor_exception = time_monitor_tick;		
}
if((flags_pointer & SOFTIRQ_ACTIVE))   ptr2time_monitor_softirq = time_monitor_tick;

return;
}
void disattiva_puntatori(){
if((flags_pointer & SCHED_ACTIVE))		ptr2time_monitor = NULL;
if((flags_pointer & INT_ACTIVE))			ptr2time_monitor_interrupt = NULL;
if((flags_pointer & EXCEPTION_ACTIVE)) ptr2time_monitor_exception = ptr2time_monitor_exception_nmi = NULL;
if((flags_pointer & SOFTIRQ_ACTIVE))	ptr2time_monitor_softirq = NULL;

return;
}


static int time_monitor_open(struct inode *inode, struct file *file)
{
	num =0;
	delta = 0;

   printk("time_monitor_open\n");
   
   if(time_monitor_opened)
   {
      printk("time_monitor_open: BUSY\n");
      return -EBUSY;
   }
   
   ++time_monitor_opened;
   
   return 0;
}

static int time_monitor_release(struct inode *inode, struct file *file)
{
   printk("time_monitor_release\n");
   
   --time_monitor_opened;
   
   return 0;
}


static struct file_operations time_monitor_fops =
{
   .owner = THIS_MODULE,
   .ioctl = time_monitor_ioctl,
   .open = time_monitor_open,
   .release = time_monitor_release,
};


static struct miscdevice time_monitor_misc =
{
   .minor = TIME_MONITOR_MINOR_NUMBER,
   .name = TIME_MONITOR_DEVICE_NAME,
   .fops = &time_monitor_fops,
};


static int __init time_monitor_init(void)
{
   int ret;
   
   ret = misc_register(&time_monitor_misc);
   
   if(!ret)
   {
      buffer_init = (struct time_stats *) kmalloc(MAX_STATS * sizeof(struct time_stats), GFP_USER);
      
      if(buffer_init == NULL)
      {
         printk(KERN_INFO "Error: it's not possible to allocate memory for the buffer!\n");
      }
   }
   
   if (!ret)
      printk(KERN_INFO "time_monitor_misc registered on minor %d\n", time_monitor_misc.minor);
      
   return ret;
}


static void __exit time_monitor_exit(void)
{
   if(ptr2time_monitor)					ptr2time_monitor = NULL;
	if(ptr2time_monitor_dump) 			ptr2time_monitor_dump = NULL;
	if(ptr2time_monitor_interrupt)	ptr2time_monitor_interrupt = NULL;
	if(ptr2time_monitor_softirq)		ptr2time_monitor_softirq = NULL;
	if(ptr2time_monitor_exception)	ptr2time_monitor_exception = NULL;
	if(ptr2time_monitor_exception_nmi) ptr2time_monitor_exception_nmi = NULL;
   
   kfree(buffer_init);
   
   printk("Removing time_monitor driver\n");
   
   misc_deregister(&time_monitor_misc);
}

module_init(time_monitor_init);
module_exit(time_monitor_exit);
