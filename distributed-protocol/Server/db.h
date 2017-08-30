//definisco le macro per definire se la query è andata bene o meno
#define QUERY_OK 0
#define QUERY_ERROR -1

//definisco il tipo di query che devo eseguire (per il momento solo select o update)
#define SELECT 0
#define UPDATE 1

//macro particolari utilizzate per operazioni di select quali ad esempio la lettura di un session_id
#define SESSION_ID_CLIENT 1000
#define SOCKET_ID_CLIENT 2000
#define ALL_CATEGORIES_DB 3000
#define ALL_ITEMS_CATEGORY 4000
#define FILE_INFOS 5000

//definisce il numero delle categorie di files presenti sul server (categorie attualmente inserire sul db)
#define NUM_CATEGORIE 2

//define numero massimo di categorie che posso gestire sul server
#define NUM_MAX_CATEGORIE 20
#define NUM_MAX_FILES_CATEGORIA 300

//flag per determinare se la query è stata eseguita o meno
#define NO_EXEC -1
#define EXEC 0
extern int flag_execution_query;

/*
	Header da importare per utilizzare le routine d'accesso a sqlite db
*/

//nome del database (server.db)
extern char *db_name;

/****************************************************************************************/
							/*stringhe valorizzate con campi letti dal db*/

//variabili per memorizzare username,password,session_id,ids,etc. letti da db

#define DIM 30

extern char global_id[DIM];

extern char id_db[DIM];
extern char username_db[DIM];
extern char password_db[DIM];
extern char session_db[DIM];
extern char socket_db[DIM];
extern char filename_db[DIM];
extern char path_db[DIM];

//vettore di stringhe per memorizzare nomi delle categorie
extern char nome_categoria[NUM_MAX_CATEGORIE][50];
extern char nome_files[NUM_MAX_FILES_CATEGORIA][50];
extern int num_categorie;
extern int num_files;
extern int index_elements;
extern int index_elements_file;
/***************************************************************************************/



/***************************************ESEGUE QUERY GENERICA*************************************/

//Esegue query aprendo connessione verso il db e chiudendo la connessione quando termina la query
int executeQuery(char * sql, int type_query, int type_op);

/*****************************************Routine gestione callback********************************/
//Funzione per la gestione la select riguardante il login sul server 
int callback(void *NotUsed, int argc, char **argv, char **azColName);

//legge valori per verificare autenticazione del client
int read_values_to_login(int, char**);
//legge session_id
int read_session_id(int,char**);
//legge socket id
int read_socket_id(int,char**);
//leggo categorie
int read_categories(int, char**,char *, char *);
//leggo file per categoria
int read_files_category(int , char **,char *, char *, char *);
//leggo info file
int read_info_file(int,char**);

//update
int update_nothing_todo();
//cancella strighe
void clear_strings();





