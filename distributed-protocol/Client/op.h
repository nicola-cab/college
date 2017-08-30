#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "close_manager.h"
#include "message.h"
#include "socket.h"
#include "compose.h"
#include "parse.h"
#include "db.h"
#include "gui.h"

//flush del buffer. fflush(stdin) --> svuota buffer input tastiera (elimino tutti i \n ossia gli invio)
#define fflush(stdin) while ((getchar()) != '\n')

//ip server
#define IPSERVER 20
char ip_server_connect[20];
//path nel quale salvare il file
extern char * path_to_save;

//pipe
//extern int fd_pipe[2];

//separatori utilizzati per concatenare le stringhe
extern char sep[1];


#define LOGIN 0
#define CATALOGO 1
#define FILES_SCARICATI 2
//#define FILES_SCARICANDO 3
#define EXIT 3

//varie macro
#define MAX_DIM_USER_PLUS_PASSWORD 23  //numero massimo di caratteri della stringa concatenata per definire username e password
#define NUM_STR 2    //numero delle stringhe da allocare	per concatenare username e password


//buffer dal quale leggere i messaggi spediti dal server
char buff[MAX_MESS_SOCKET_LENGHT];  
//path dei file per definire il template
extern char * path_template_menu;
extern char * path_template_loggin;

/*
	Definisco una struttura per memorizzare i dati della sessione 
*/
struct session_t{
	int id;
};

/*
	Definisco una struttura per memorizzare le informazioni riguardanti una singola cattegoria
*/
#define MAX_LENGHT_CATEGORIA  50
struct categoria_t{
	int id;
	char descrizione[MAX_LENGHT_CATEGORIA];
};

/*
	Definisco una struttura per memorizzare le informazioni riguardanti un singolo file
*/
#define MAX_FILE_NAME 50

#define NO_DOWNLOADED 0
#define DOWNLOADED 1
#define IN_DOWNLOAD 2


extern char * status_downloaded; // = "downloaded";
extern char * status_in_download; // = "in_download";

//definisco una costante che mi serve a specificare quando iniziano le info riguardanti un nuovo file (ogni 3 posizioni c'è un nuovo file)
#define NEW_ITEM_START 1
struct file_t{
	int id;
	int status; //(Scaricato o No)
	char descrizione[MAX_FILE_NAME];
	long dimensione;
};

//Gestisco tutti i download in un FIFO che poi memorizzo su 
struct download_item{
	char name[30];
	int status;
	int percentuale; 
	char path[60];
	struct download_item * next;
};

//definisco puntatore alla struttura
//extern struct download_item * items;




/*Rinomino strutture per essere più chiaro*/
typedef struct session_t session;
typedef struct file_t file;
typedef struct categoria_t categoria;

//definisco un vettore che può gestire al massimo 20 categorie (utilizzato in fase di lettura del catalogo)
#define MAX_CATEGORIE 20
categoria elenco_categorie[MAX_CATEGORIE];

//definisco numero massimo di file che posso gestire
#define MAX_FILES 200
file elenco_file[MAX_FILES];

/*Memorizzo la sessione che identifica la connessione del client*/
session session_id;

/********************** ROUTINE ************************/


/*UTILITIES*/
//legge da stdin controllando possibili errori
void read_stdinput(int * choice, int left , int right );
//disegna sullo schermo un menù e legge opzione scelta dall'utente
void draw_menu(int *);
//disegna sullo shermo leggendo il template da file
void draw_from_file(char *);
//printa a video menu per la selezione della categoria
int draw_items_menu(int * max_choice,int num_val, char ** vs, int tipo);
//gestisce menu files
void manage_files(int ,int , char *, char *, int);
//Creo Stack 
struct download_item * write_new_download(struct download_item *,char *, int ,char *);


/*OPERAZIONI PRINCIPALI*/

/*
	legge username e password per l'utente. 
	Richiama routine per comporre messaggio da spedite al server autenticarsi
*/
int login(int sd, char * , char *);

/*
	Richiede catalogo al server e printa a video i risultati
*/
int catalogo(int sd, char **, int*, int);

/*
	Richiede files per una determinata categoria (es tutti gli mp3 per la categoria Musica) 
	* int -> socket descriptor
	* int -> id categoria 
	* char ** -> elenco nome dei file restituiti dal server per categoria
	* int * -> numero di files restituiti dal server
*/
int files(int, int, char **, int *, int);

/*
	Visualizza tutti i file scaricati.
*/
struct download_item * scaricati(void);


//visualizza tutti i download
void download(void);
//chiude la connessione (socket) e printa sul log
void close_conn(int);

/**********************************************************/
