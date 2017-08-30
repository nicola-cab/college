/*** MACRO ***/

//definisco massima grandezza di un messaggio per il protocollo su socket
#define MAX_MESS_SOCKET_LENGHT 1000 
//definisco il massimo delle chiamate gestite contemporaneamente
#define MAX_CALL 20
//Definisco la macro per specificare la porta d'ascolto
#define PORT 9000
//Numero massimo di download
#define MAX_DOWNLOAD_SIM 5

//costanti per definire tipo di scrittura
#define FILE_BYTES 0
#define PROTOCOL 1

/******/


char buff[MAX_MESS_SOCKET_LENGHT];  //buffer dal quale leggere i messaggi spediti dal client
struct sockaddr_in server;   //struttura per inizializzare il server
struct sockaddr_in client;   //struttura per leggere info del client

int sd;             //descrittore del socket (utilizzato in prima istanza, chiamate di sistema "socket") 
int client_sd;     //descrittori del socket per ogni singola chiamata, chiamata di sistema "accept"
int address_size;

int create_server(int,int);      //creo ed inizializza il server
int create_client_channel(int);  //eseguo la accept
int write_to_socket(int sd,char * buffer, int, int);  //scrive sul socket
int read_from_socket(int sd,char *buffer); //legge dal socket  
 
//NON USATE NON SERVONO
//Routine utilizzate solo per la lettura e scrittura di bytes sul socket,
//Utilizzate solo per il download/upload di un file
//int write_to_socket_bytes_file(int sd,char * buffer, int bytes);
//int read_to_socket_bytes_file(int sd,char * buffer, int bytes);

