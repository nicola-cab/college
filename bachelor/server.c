#include "server.h"
#include "monitor.h"

#define MAXLINE 80

void open_socket(void)
{
   struct sockaddr_in serv_add;
   
   int i, nread;
   char buffer[MAXLINE];

   char temp;
   

   if(CONNECTION == REMOTE)
   {
      printline(conn_fd_client, "Choice not permitted: you are already in a remote connection!!\n");
      return;
   }
   

   do
   {
      printf("Type the port number (0 to abort operation): ");
      scanf("%10s", buffer);
      i = atoi(buffer);
      
      if(i == 0)
         return;      
      
      if(!((1023 < i) && (i <= 65536)))
         printf("Invalid port number!!\n");
      
   }
   while(!((1023 < i) && (i <= 65536)));


   memset((void *) &serv_add, 0, sizeof(serv_add));
      
   serv_add.sin_port = htons(i);
   serv_add.sin_family = AF_INET;
   serv_add.sin_addr.s_addr = htonl(INADDR_ANY);
   
   if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
   {
      perror("Socket creation error");
      return;
   }
   
   if(bind(sock_fd, (struct sockaddr *)&serv_add, sizeof(serv_add)) < 0)
   {
	   perror("bind error");
	   return;
   }
   
   if(listen(sock_fd, 1) < 0)
   {
      perror("listen error");
      return;
   }
   
   printf("I'm listening on port %d...\n", i);
   
   if((conn_fd_client = accept(sock_fd, (struct sockaddr *) NULL, NULL)) < 0)
   {
      perror("accept error");
      return;
   }
   
   printf("Client connected!\n");
   
   
   if((conn_fd_thread = accept(sock_fd, (struct sockaddr *) NULL, NULL)) < 0)
   {
      perror("accept error");
      return;
   }
   
   printf("Thread connected!\n");
   CONNECTION = REMOTE;
}
