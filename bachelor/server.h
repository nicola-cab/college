#ifndef __server_h__
#define __server_h__


#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int sock_fd, conn_fd_client, conn_fd_thread;
void open_socket(void);


#endif // __server_h__
