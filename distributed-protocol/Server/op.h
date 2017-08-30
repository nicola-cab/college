#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h> 
#include <sys/wait.h> 
#include <time.h>


#include "message.h"
#include "socket.h"
#include "close_manager.h"
#include "compose.h"
#include "db.h"
#include "parse.h"
#include "logger.h"


//utilizzo le macro per decidere quando scrivere o meno sul socket al rientro della routine di riconoscimento
//del messaggio inviato dal client, se non effettuo alcuna scrittura è perchè ho già scritto sul socket 
//all'interno della routine dedicata alla gestione di una particolare operazione (esempio il login)
#define WRITE_SOCKET 1
#define NO_WRITE_SOCKET 0


#define MIN_ID 1000
#define MAX_ID 9000


/********************************************************************************************/
														/*Riconosce operazione da eseguire*/

/*Riconosce il messaggio inviato dal client e restituisce il codice identificativo*/
int recognize_mess_and_response(char * mess,int socket);


/*****************************************Operazioni Server**********************************/

/*	Esegue login del client sul server*/
int do_login(int socket,char *mess,char *username,char *password);
/* Eseguo richiesta per richiedere catalogo*/
int do_request_catalogo(int socket,char *mess,char *session_id);
/*Richiede tutti gli elementi di un deteminato catalogo (es tutti i file musicali o i documenti)*/
int do_request_catalogo_items(int socket,char *mess,char *session_id,char *category_id);
/*Avvia operazione di download*/
int do_request_download_item(int socket,char *mess,char *session_id,char *category_id,char *file_id);
/*Gestisce op di download*/
int do_manage_download(int socket,char mess[], char filename[], char path_file[]);
/*Chiude sessione per il client*/
int do_close_conn(int socket,char *mess,char *session_id);


/********************************************************************************************/




/*****************************************UTILITY*********************************************/
/*Genera numero casuale fra 1000 e 9000 e lo somma all'ID dell'utente storato nel db*/
int generate_session_id(int id);
/* clear session e socket su db*/
void clear_db_session_socket(int,int);
/*set socket utilizzato su db*/
void set_socket_db(int ,char *);
/*Lettura del socket_id da db*/
void get_id_from_socket(char * id);
/*Lettura categorie da db*/
void get_categorie(void);
/*Lettura file per una detrminata categoria*/
void get_file_categoria(char *);
/*Lettura info associate */
void get_file_infos(char *, char *);
/********************************************************************************************/
