/*** MACRO ***/

//definisco massima grandezza di un messaggio per il protocollo su socket
#define MAX_MESS_SOCKET_LENGHT 1000 
//Definisco la macro per specificare la porta d'ascolto
#define PORT 9000

#define BYTE_FILE 0
#define PROTOCOL 1

/******/

int create_client(char *, unsigned int);      //creo ed inizializza il client
int write_to_socket(int sd,char * buffer);  //scrive sul socket
int read_from_socket(int sd,char *buffer, int type_read); //legge dal socket  


