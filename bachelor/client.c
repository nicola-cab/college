#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <malloc.h>
#include <string.h>

#include <sys/types.h>   /* predefined types */
#include <arpa/inet.h>   /* IP addresses conversion utiliites */
#include <sys/socket.h>  /* socket library */


#include <pthread.h>

#include "time_monitor.h"
#include "tx_rx.h"



// #include "server.h"


// Currently 254 bytes is the maximum lenght of a single transmission
// because 255 is used as control
// I put 256 as safety margin
#define MAXBUFFER 256





// Dumping thread
pthread_t dump_thread;


struct sockaddr_in serv_add;




void clear_screen(void)
{
   fprintf(stdout, "\033[2J");
   fprintf(stdout, "\033[1;1H");
   fflush(stdout);
}


void usage(void)
{
   printf("Usage:\n");
   printf("   client server_ip server_port\n");
   printf("      port range: 1024:65536\n\n");
   
   exit(1);
}




// Thread
void * statistics_dump()
{
   int sock_fd_thread;
   
   FILE *file_stats = NULL;
   
   int i = 0;
   int string_length = 0;
   
   char c;
   
   unsigned long long old_timestamp = 0;
   int j = 0;
   
   
   struct time_stats
   {
      int pid;
      unsigned long long timestamp;
      char name[16];
   };


   struct time_stats buffer[MAX_STATS / 2];
   
   
   // It opens the file
   file_stats = fopen("./mb0.log", "w");
   if(!file_stats)
   {
      return;
   }
   
  
   // create socket
   if((sock_fd_thread = socket(AF_INET, SOCK_STREAM, 0)) < 0)
   {
      perror("Socket creation error");
      return;
   }      
   
   
   printf("Thread connecting...\n\n");
   
   
   // extablish connection
   if(connect(sock_fd_thread, (struct sockaddr *)&serv_add, sizeof(serv_add)) < 0)
   {
      perror("\n\nThread connection error");
      printf("\n\n");
      return;
   }   
  

   
   // It receives the data from the thread on the server
   while(1)
   {            
      // It reads the number of struct "time_stats" to be received (for now is fixed a 50, but for future development...)
      if((i = fullread(sock_fd_thread, &c, 1)) < 0)
      {
         perror("read error");
         break;
      }
      
      if(i > 0)   // EOF => socket closed by server
         break;
      
                  
      // It reads the first character as a integer (the length of the following data packet)
      i = (int) c;
      if(!((0 < i) && (i <= 255)))
         break;
      }
      
      // The server signals that the transmission is finished
      if(i == 255)
         break;

      
      string_length = i * sizeof(struct time_stats);

      
      // It reads the data from the server
      if((i = fullread(sock_fd_thread, buffer, string_length)) < 0)
      {
         perror("read error");
         break;
      }
      
      if(i > 0)
         return;
   
      
      
      // It prints the received data in the file
      // It writes the buffer into the file    
      for(i = 0; i < (MAX_STATS / 2); ++i)
      {        
         if(old_timestamp != buffer[i].timestamp)
            j = 0;

         fprintf(file_stats, "%llu %u\n", ((buffer[i].timestamp * 100) + j++), buffer[i].pid, buffer[i].name);
         
         old_timestamp = buffer[i].timestamp;
      }
      
      fflush(file_stats);
   }
   
   
   // It closes the socket
   close(sock_fd_thread);
   
   // It closes the file descriptor
   fclose(file_stats);
   
   pthread_exit(NULL);
}

void exit_routine(int sock_fd_client) {
   
   // It closes the socket
   close(sock_fd_client);
   
   
   //It closes the thread
   pthread_cancel(dump_thread);
   
   
   printf("Program terminated!\n");
   
}

int main(int argc, char *argv[])
{
   int sock_fd_client;
   
   int i, nread;
   char buffer[MAXBUFFER];
   
   int string_length = 0;

   enum state {READ, WRITE} STATE;
   
   int rc = 0;

   char c = 0;

   clear_screen();   
   
   if(argc != 3)
      usage();   

   
    // initialize address
   memset((void *) &serv_add, 0, sizeof(serv_add)); // clear server address  
   
   // build address using inet_pton
   if((inet_pton(AF_INET, argv[1], &serv_add.sin_addr)) <= 0)
   {
      perror("Address creation error");
      usage();
   }
   
   // It extracts port number from user input
   i = atoi(argv[2]);
   if(!((1023 < i) && (i <= 65536)))  // valid port range (to use range 1->1023 the process must have root privileges)
   {
      printf("Port creation error!!\n");
      usage();      
   }
   
   serv_add.sin_port = htons(i);                 // port
   serv_add.sin_family = AF_INET;                // address type is INET
   

     
   // create socket
   if((sock_fd_client = socket(AF_INET, SOCK_STREAM, 0)) < 0)
   {
      perror("Socket creation error");
      return;
   }
   
   
   printf("Client connecting to %s:%d...", argv[1], i);
   
   // extablish connection
   if(connect(sock_fd_client, (struct sockaddr *)&serv_add, sizeof(serv_add)) < 0)
   {
      perror("\n\nClient connection error");
      printf("\n\n");
      return -1;
   }
   
   
   printf("OK\n");
   printf("Client connected to server\n\n");
   
   
   rc = pthread_create(&dump_thread, NULL, statistics_dump, NULL);
  
   if(rc != 0)
   {
      printf("ERROR: it is impossible to create the dumping thread\n");
      printf("Return code from pthread_create() is %d\n", rc);
      printf("Program terminated!\n\n");
      exit(-1);
   }
   
   // Now that the client is connected to the server, basically reads and writes in to the socket
   // It is implemented as state machine
   STATE = READ;
   
   while(1)
   {
      switch(STATE)
      {
         case READ:
         {
            i = receive_data(sock_fd_client, buffer);
            
            if(i < 0)
            {
               printf("Errors on received data (STATE READ)!!\n\n");
               return;
            }            
            
            if(i == 254)
            {
               printf("Exiting...\n\n");
               exit_routine(sock_fd_client);
               break;
            }            
            
            if(i == 255)
            {
               STATE = WRITE;
               break;
            }            
            
            buffer[i] = '\0';
            printf("%s", buffer);
            fflush(stdout);            
               
            break;
         }
         
         case WRITE:
         {
            scanf("%d", &i);
            
            c = (unsigned char) i;
            
            send_data(sock_fd_client, &c, 1);            

            STATE = READ;
            
            break;  
         }               
         
         default:
            printf("Error!! This state is not possible!!\n\n");
            break;
      }
   }
}
