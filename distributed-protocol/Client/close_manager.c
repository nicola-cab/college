#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include "close_manager.h"
#include "message.h"

/***
Gestisco terminazione task, per evitare che si creino degli ZOMBIE.
Utilizzo la waitpid.
***/
void _manage_exit_child(int signo){
	int status;
	int pid_waiting = waitpid(WAIT_ANY,&status,WNOHANG); //wait nn bloccante su tutti i pid dei figli.  
}

/***
Chiudo canale socket, utilizzando il socket descriptor passato come parametro
***/
void _manage_close_socket(int sd){

	//debug
	/*char b[30] = "Canale da chiudere : ";
	char b1[3];
	sprintf(b1,"%d",sd);
	strcat(b,b1);
	logger(b);*/

	compose_mess_to_log(close_socket, &sd, INTEGER_TYPE);
	//logger(close_socket);
	close(sd);
}

