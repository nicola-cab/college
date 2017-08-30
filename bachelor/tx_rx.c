#include "tx_rx.h"

#include <unistd.h>
#include <errno.h>
#include <stdio.h>


int fullwrite(int fd, const void *buf, int count) 
{
   int nleft;
   int nwritten;
   
   nleft = count;
   
   while(nleft > 0)
   {             
      if((nwritten = write(fd, buf, nleft)) < 0)
      {
         if(errno == EINTR)
         {   
            continue;
         }
         else
         {
            return(nwritten);
         }
      }
      
      nleft -= nwritten;
      buf += nwritten;
   }
   
   return (nleft);
}

int fullread(int fd, void *buf, int count) 
{
   int nleft;
   int nread;
   
   nleft = count;
   
   while(nleft > 0)
   {             
      if((nread = read(fd, buf, nleft)) < 0)
      {
         if(errno == EINTR)
         {    
            continue;
         }
         else
         {
            return(nread);
         }
      }
      else if(nread == 0)
      {    
         break;
      }
      
      nleft -= nread;
      buf += nread;
   }
   
   return (nleft);
}

int send_data(int fd, const char *buffer, unsigned int num_bytes)
{
   unsigned char c;

    
   c = (unsigned char) num_bytes;      
         
   if((fullwrite(fd, &c, 1)) < 0)
   {
      perror("write error");
      return -1;
   } 
      
   if((fullwrite(fd, buffer, num_bytes)) < 0)
   {
      perror("write error");
      return -1;
   }   
}

int receive_data(int fd, char *buffer)
{
   unsigned int i = 0, string_length = 0;
   unsigned char c;
   
   if((i = fullread(fd, &c, 1)) < 0)
   {
      perror("read error");
      return -1;
   }
   
   if(i > 0)
   {
      printf("EOF!!!!!!!!!!!!\n\n");
      return -1;
   }
   
   string_length = (unsigned int) c;   
   
 
   if(!((0 < string_length) && (string_length <= 255)))
   {
      return -1;
   }
   
   if((string_length == 254) || (string_length == 255))
      return string_length;
   
   
   if((i = fullread(fd, buffer, string_length)) < 0)
   {
      perror("read error");
      return -1;
   }
   
   if(i > 0)
   {
      printf("EOF!!!!!!!!!!!!\n\n");
      return -1; 
   }
   
   return string_length;
}
