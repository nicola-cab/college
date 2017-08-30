

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <malloc.h>

#include <pthread.h>

#include <signal.h>


#include "monitor.h"
#include "time_monitor.h"
#include "server.h"
#include "menu.h"
#include "tx_rx.h"


#define TIME_MONITOR_DEVICE_FILE_NAME  "/dev/time_monitor"
unsigned short int track_options = 0x0000;

#define OFF 0
#define ON 1
#define DEFAULT 0
#define USER_DEFINE 1
int track_state = OFF;
int mod_tracking = DEFAULT;

struct time_stats
{
   int id;
   unsigned long long timestamp;
   char name[16];
	int type;
	unsigned int num;
};  

struct sonde_monitor sonde[NUM_SONDE]={
	SCHED,
	-1,
	EXCEPTION,

	
	HARD_INT,
	-1,
	SOFT_INT,

	
	EXCEPTION,
	SCHED,
	-1,

	SOFT_INT,
	HARD_INT,
	-1,
} ;                          

struct time_stats buffer[MAX_STATS];



// file descriptor
int fd = 0;

// file pointer of the statistics file
FILE *file_stats = NULL, *mb0_log = NULL;


// Dumping thread
pthread_t dump_thread;

// Synchronization flag
int flag = 0;
int num_data = 0;

int flag_execute = 0;

int num_dati = 0;


unsigned long long old_timestamp = 0;
int j = 0;
int old_pos = 0;
unsigned int n_write = 0;
void print_statistics_local(void)
{
   int i = 0;
        
   // It writes the buffer into the file    
   for(i = 0; i < num_dati; ++i)
   {  
      
      if(old_timestamp != buffer[i].timestamp)
         j = 0;
     
		if(n_write != buffer[i].num || old_timestamp > buffer[i].timestamp){
		fprintf(mb0_log, "%llu %d %s %d %d *****\n", ((buffer[i].timestamp * 100) + j++), buffer[i].id,buffer[i].name,buffer[i].type,buffer[i].num);
		}
		else
		fprintf(mb0_log, "%llu %d %s %d \n", ((buffer[i].timestamp * 100) + j++), buffer[i].id,buffer[i].name,buffer[i].type);         
      
      
      
      old_timestamp = buffer[i].timestamp;
		n_write ++;
      
   }
}



void print_statistics_remote(void)
{
   int num_bytes = 0;
   char temp = 0;

   
   num_bytes = sizeof(struct time_stats) * (num_dati);   

   temp = (char) (num_dati);   
   
  
   if((fullwrite(conn_fd_thread, &temp, 1)) < 0)
   {
      perror("write error");
      return;
   } 
   
   if((fullwrite(conn_fd_thread, buffer, num_bytes)) < 0)
   {
      perror("write error");
      return;
   }   
}

void print_statistics(void)
{
   if(CONNECTION == LOCAL)
      print_statistics_local();
   else
      print_statistics_remote();   
}


void clear_screen(void)
{
   fprintf(stdout, "\033[2J");
	fprintf(stdout, "\033[1;1H");
	fflush(stdout);
}

void set_mask_driver(int choose , int mode){

	mod_tracking = USER_DEFINE; 

	switch(choose){
		case (SCHED) : 
							if(mode)
								track_options |= SCHED_ACTIVE;
							else
								track_options &= SCHED_DEACTIVE;
							break;

		case (HARD_INT) :
							if(mode)
								track_options |= INT_ACTIVE;
							else
								track_options &= INT_DEACTIVE;
			 				break;
		
		case (EXCEPTION) :
							if(mode)
								track_options |= EXCEPTION_ACTIVE;
							else
								track_options &= EXCEPTION_DEACTIVE;									
							break;
		case (SOFT_INT) :
							if(mode)
								track_options |= SOFTIRQ_ACTIVE;
							else
								track_options &= SOFTIRQ_DEACTIVE;
							break;
		case (BUFFER):
							break;
		case (WAKE):
							break;
							
		default :	
					break;
	}
	if(mod_tracking == DEFAULT)
		track_options |= SCHED_ACTIVE;

}

// signal handler for signal SIGUSR1 (User defined signal 1)
void thread_signal_handler(int sigNumber)
{
   pthread_exit(NULL);
}


// Thread
void * statistics_dump()
{
   int ret = 0;
	unsigned int delta =0;
   
   
   // It installs the signal handler for signal SIGUSR1 (User defined signal 1)
   signal(SIGUSR1, thread_signal_handler); 

   
   // It is for communicate the task_struct pointer to the module
   ret = ioctl(fd, TIME_MONITOR_IOCTL_REGISTER_THREAD, NULL);

   if(ret == -EINVAL)
   {
      printf("Error: it is not possible to register the thread into the module!!\n");
      printf("Program terminated!!\\n");
      exit(-1);
   }
   
   // It unlocks the main thread
   flag = 1;
   
   
   // This causes the thread to sleep indefinitely
   ret = ioctl(fd, TIME_MONITOR_IOCTL_THREAD_SLEEP, NULL);

   if(ret == -EINVAL)
   {
      printf("Error: it is not possible to put to sleep the thread!!\n");
      printf("Program terminated!!\\n");
      exit(-1);
   }  

	while(1){
		ioctl(fd, TIME_MONITOR_IOCTL_READ_DELTA, &delta);
      

		if(delta >= MAX_STATS/4)
		{
			num_dati = (int)delta;
			buffer[0].num = delta ;
			ioctl(fd, TIME_MONITOR_IOCTL_DUMP, &buffer);
			print_statistics();
			ioctl(fd, TIME_MONITOR_IOCTL_SET_DELTA, delta);
		}
		else
			ioctl(fd, TIME_MONITOR_IOCTL_THREAD_SLEEP, NULL);
	} 
   
}


int main(int argc, char *argv[])
{
   int ret = 0, service_num = -1;
   
   int rc = 0;
   
   int choice = 0;
   
   CONNECTION = LOCAL;
   
   clear_screen();
   

   fd = open(TIME_MONITOR_DEVICE_FILE_NAME, O_RDONLY);
   
   if(fd == -1)
   {
      printf("\nErrore di apertura del file %s\n", TIME_MONITOR_DEVICE_FILE_NAME);
      printf("Se il file esiste, assicurarsi di avere i permessi di root\n");
      printf("Se il file non esiste, assicurarsi di avere caricato il modulo\n\n");
      return 0;
   }

   
   mb0_log = fopen("./mb0_local.log", "w");
   if(!mb0_log)
   {
      printf("Impossibile aprire il file!\n");
      return 1;
   }   

   file_stats = fopen("./statistics.log", "w");
   if(!file_stats)
   {
      printf("Impossibile aprire il file!\n");
      return 1;
   }

   rc = pthread_create(&dump_thread, NULL, statistics_dump, NULL);
  
   if(rc != 0)
   {
      printf("ERROR: it is impossible to create the dumping thread\n");
      printf("Return code from pthread_create() is %d\n", rc);
      printf("Program terminated!\n\n");
      exit(-1);
   }
            
   
   while(!flag)
      sched_yield( );
      

   create_menu();   
   
   while(1)
   {
      print_menu();
      choice = get_user_choice();
      launch_menu(choice);
   }
      
   return 0;
}


void monitor_on(void)
{
   int ret = 0;

	if(mod_tracking == DEFAULT)
		set_mask_driver(SCHED,1);
   
   ret = ioctl(fd, TIME_MONITOR_IOCTL_ON, track_options);
   
   if(ret == -EINVAL)
   {
      printline(conn_fd_client, "Errore: impossibile attivare il monitor!\n");
      return;
   }
   
	track_state = ON;
   printline(conn_fd_client, "MONITOR ON\n");
}

void monitor_off(void)
{
   int ret = 0;
   
   ret = ioctl(fd, TIME_MONITOR_IOCTL_OFF, track_options);
   
   if(ret == -EINVAL)
   {
      printline(conn_fd_client, "Errore: impossibile disattivare il monitor!\n");
      return;
   }
   
	track_state = OFF;
   printline(conn_fd_client, "MONITOR OFF\n");
}


void monitor_info(void)
{
	int i=0;
	printline(conn_fd_client, "\n####################################\n");

   printline(conn_fd_client, "\n- Info tracking kernel\n\t");

	if(track_state == ON)
		printline(conn_fd_client,"\n- Tracking enabled\t");
	if(track_state == OFF)
		printline(conn_fd_client,"\n- Tracking disabled\t");
	
	if(CONNECTION == LOCAL)
		printline(conn_fd_client,"\n- Local connection\t");
	if(CONNECTION == REMOTE)
		printline(conn_fd_client,"\n- Remote connection\t");

	printline(conn_fd_client,"\n- Buffer size: %d\t",MAX_STATS);

	if(mod_tracking == USER_DEFINE)
		printline(conn_fd_client,"\n- User define tracking\t");
	if(mod_tracking == DEFAULT)
		printline(conn_fd_client,"\n- Default tracking (only scheduler)\t");		
	
	printf("\n");
				
	if(track_options & SCHED_ACTIVE)
		print_menu_options_screen(SCHED);
	if(track_options & INT_ACTIVE)
		print_menu_options_screen(HARD_INT);
	if(track_options & EXCEPTION_ACTIVE)
		print_menu_options_screen(EXCEPTION);
	if(track_options & SOFTIRQ_ACTIVE)
		print_menu_options_screen(SOFT_INT);
	
	printline(conn_fd_client,"\n\n- End info\t");
	printline(conn_fd_client, "\n####################################\n\n");
}

void monitor_options(void)
{
	int choice;
	int sub_choice;
	int n_arg;
	create_menu_options();
   
	do
   {
		print_menu_options();
      choice = get_user_choice_options();
		
		if(choice >= SCHED && choice <= SOFT_INT) {
			do{
				if(n_arg == 0) 
					scanf("%*[^\n]");

				printline(conn_fd_client,"\n 1) Active option \n 0) Deactive option \n Choice --> ");
				n_arg = scanf("%d",&sub_choice);
				printf("\n");
			}while(n_arg != 1 || sub_choice < 0 || sub_choice > 1);

			launch_menu_options(choice,sub_choice);
		}
		if(choice == WAKE || choice == BUFFER){	
			printline(conn_fd_client,"Not yet implemented\n");
		} 
   }while(choice != EXIT);
}


void monitor_socket(void)
{
   clear_screen();
   printf("I'm opening the socket...\n");
   
   open_socket();
}

void monitor_exit(void)
{
   int ret = 0;
   
   char c = 0;
   
   ret = ioctl(fd, TIME_MONITOR_IOCTL_OFF, NULL);

   if(ret == -EINVAL)
   {
      printline(conn_fd_client, "Errore: impossibile disattivare il monitor!\n"); // da gestire meglio!!
      printline(conn_fd_client, "Attenzione: il monitor e' ancora attivo!!!\n");
   }
   else
      printline(conn_fd_client, "MONITOR OFF\n");
   

   c = (char) 254;
   
   if(CONNECTION == REMOTE)
   {
      if((fullwrite(conn_fd_client, &c, 1)) < 0)
      {
         perror("write error");
      }
   }

   printf("I'm closing the thread...");
   fflush(stdout);

   if(pthread_kill(dump_thread, SIGUSR1) != 0)
      printf("Error: failed to signal thread!!!\n\n");
   
   pthread_join(dump_thread, NULL);
   
   fflush(stdout);
   
   ret = ioctl(fd, TIME_MONITOR_IOCTL_UNREGISTER_THREAD, NULL);

   if(ret == -EINVAL)
   {
      printline(conn_fd_client, "Error: it is not possible to unregister the thread into the module!!\n");
      printline(conn_fd_client, "Please remove the module from the kernel as soon as possible!!\n\n");
   }
   else
      printline(conn_fd_client, "Thread unregistered from module\n");
   
   close(fd);
   fclose(file_stats);
   
   fclose(mb0_log);
   close(conn_fd_client);
   close(conn_fd_thread);
   close(sock_fd);

   printf("Program Monitor terminated!\n\n");
   exit(0);
}

void set_option(int options, int mode){
	int sonda_dipendente=-1;

	if(track_state == ON && !mode && options == SCHED){
		printline(conn_fd_client, "\nImpossibile disattivare la sonda dello scheduler a runtime, devi riavviare il programma per cambiare lo stato di tale sonda nel sistema\n");
		return ;
	}
	else{
		if(mode){
			if(sonde[options].dipendenza_w != -1)
				sonda_dipendente = sonde[options].dipendenza_w;
		}
		else
			if(sonde[options].dipendenza_r != -1)
				sonda_dipendente = sonde[options].dipendenza_r;	
		
		set_mask_driver(sonda_dipendente,mode);
		set_mask_driver(options,mode);

		printline(conn_fd_client,"Operazione completata\n\n");
	}

	if(!(track_options & 0x001f))
		mod_tracking = DEFAULT;
	return;
}

void print_line(int fd, const char *buffer)
{
   printf("%s", buffer);   
   if(CONNECTION == REMOTE)
      send_data(fd, buffer, strlen(buffer));
}
