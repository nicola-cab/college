#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

#include "compose.h"
#include "logger.h"
#include "db.h"
#include "message.h"

int flag_execution_query =  NO_EXEC;


//nome del database
char * db_name = "../DB/server.db";


//variabili per memorizzare id,username,password e session_id letti da db

char global_id[DIM];

char id_db[DIM];
char username_db[DIM];
char password_db[DIM];
char session_db[DIM];
char socket_db[DIM];
char filename_db[DIM];
char path_db[DIM];

//vettore di stringhe per memorizzare nomi delle categorie
char nome_categoria[NUM_MAX_CATEGORIE][50];
char nome_files[NUM_MAX_FILES_CATEGORIA][50];

#define NO_OP -1
//memorizza il tipo di query da eseguire e permette di gestire i risultati in maniera appropriata
int select_op = NO_OP;
int select_type_query = NO_OP;

int num_categorie = 0;
int num_files = 0;

int index_elements = 0;
int index_elements_file = 0;


//puntatore a funzione (utilizzato per assegnarlo alla routine di callback)

int (*callback_ptr) (void*,int,char**,char**) = NULL;


/*
	Funzione generica per gestire le query su db
*/
int executeQuery(char * sql, int type_query, int type_op){

	logger("ESEGUO QUERY");

	clear_strings();

	sqlite3 *db;
  	char *zErrMsg = 0;
  	int rc;
	char error[60];

	flag_execution_query =  NO_EXEC;


	//loggata della query
	char log[300];
	sprintf(log,"%s%s","Richiesta query: ",sql);
	logger(log);


	logger("Apro connessione verso il database");

	//apro connessione verso db server.db
	rc = sqlite3_open(db_name, &db);

 	if(rc){
 	   sprintf(error, "Impossibile connettersi al db: %s", sqlite3_errmsg(db));
		logger(error);
		logger("Chiusura connessione al database");
 	   sqlite3_close(db);
	   return ERROR;
  	}

	//memorizzo il tipo di operazione richiesta per fare lo switch dentro la callback
	select_type_query = type_query;
	select_op = type_op; 

	//setto routine di callback da richiamare
	callback_ptr = callback;

	//debug
	if(!callback_ptr)
		logger("Routine di callback non invocata");

	rc = sqlite3_exec(db, sql, callback_ptr, 0, &zErrMsg);

	if(rc!=SQLITE_OK){
		sprintf(error, "SQL error: %s", zErrMsg);
		logger(error);
		return ERROR;
	}
			
	logger("Chiusura connessione al database");
	sqlite3_close(db);

	//setto a NULL il puntatore
	callback_ptr  = NULL;

	//verifico che la query sia stata eseguita, altrimenti restituisco ERROR
	//if(flag_excution_query == NO_EXEC)
	//	return ERROR;
	
	return QUERY_OK;
}

/*
	Funzione di gestione di callback 
*/
int callback(void *NotUsed, int argc, char **argv, char **azColName){
	//logger("Dentro1");
  	int i;
  	NotUsed=0;

	flag_execution_query = EXEC;

	compose_mess_to_log("Operazione richiesta : ",&select_type_query,INTEGER_TYPE);
	compose_mess_to_log("Tipo richiesta : ",&select_op,INTEGER_TYPE);

	switch(select_type_query){
		case SELECT:
			switch(select_op){
				case REQUEST_LOGIN:
					logger("Leggo credenziali d'accesso SELECT, RICHIESTA LOGGIN");
					return read_values_to_login(argc,argv);
				break;
				
				case SESSION_ID_CLIENT:
					//entro solo se la query ha successo (ossia ottiene dei risultati)
					logger("Leggo id di sessione SELECT, LEGGI SESSION_ID");
					return read_session_id(argc,argv);
				break;

				case SOCKET_ID_CLIENT:
					//entro solo se la query ha successo (ossia ottiene dei risultati)
					logger("Leggo id di sessione SELECT, LEGGI SESSION_ID");
					return read_socket_id(argc,argv);
				break;
	
				case ALL_CATEGORIES_DB:
					logger("Leggo le categorie dei files memorizzate su db");

					int ret_category =  read_categories(
						argc,
						argv,
						nome_categoria[index_elements],
						nome_categoria[index_elements+1]);

					index_elements+=2;
					num_categorie+=2;
					return ret_category;
				break;

				case ALL_ITEMS_CATEGORY:
					logger("Leggo files per categoria");

					//Leggo e memorizzo per ogni file richiesto:
					//1) ID, da passare al client per poter richiedere il contenuto
					//2) NOME, da passare al client per poter visualizzare il nome del file
					//3) PATH, DA NON PASSARE AL CLIENT, utilizzo il PATH+NOME per aprire il file e leggerne la dimenzione (da passare al client)

					int ret_items = read_files_category(
						argc,
						argv,
						nome_files[index_elements_file],
						nome_files[index_elements_file+1],
						nome_files[index_elements_file+2]);

					//ogni 3 elementi scrivo info per un nuovo file
					index_elements_file+=3;
					num_files+=3;
					return ret_items;
				break;

				case FILE_INFOS:
					logger("Leggo info file");
					int ret_file_info = read_info_file(argc,argv);
					return ret_file_info;
				break;	

				default:
					//nothing todo
					logger("Nessun select possibile");
				break;
			}
		break;

		case UPDATE:
			switch(select_op){
				case REQUEST_LOGIN:
					logger("Eseguo update per inserire un nuovo session_id, UPDATE INSERISCO SESSION_ID");
					return update_nothing_todo();
				break;
				case REQUEST_CLOSE:
					logger("Eseguo update per inserire un nuovo socket id");
					return update_nothing_todo();
					break;
				default:
					//nothing todo
					logger("Nessun update possibile");
				break;
			}
		break;

		default:
			logger("Operazione di query non valida!, verifica i parametri passati alla executeQuery");
			return ERROR;
		break;
	}

return QUERY_OK;
}

/*
	Verifico se il client possiede le credenziali per accedere al catalogo di contenuti
*/
int read_values_to_login(int argc, char ** argv){
	
	int i=0;

	for(i=0; i<argc; i++){
		if(i==0){
			strcpy(id_db,argv[i] ? argv[i] : "NULL");
			strcpy(global_id,argv[i] ? argv[i] : "NULL");
		}
		if(i==1)
			strcpy(username_db,argv[i]?argv[i]:"NULL");
		if(i==2)
			strcpy(password_db,argv[i]?argv[i]:"NULL");
		else
			strcpy(session_db,argv[i]?argv[i]:"NULL");
  	}

	char loggata [300];
	sprintf(loggata,"%s%s%s%s%s%s%s","Ho letto da DB le seguenti credenziali d'accesso, id: '",id_db,"',username: '",username_db,"',password: '",password_db,"',session_id: '",session_db,"'");
	logger(loggata);
  	
  	return QUERY_OK;
}

/*
	Verifica se il client è loggato
*/
int read_session_id(int argc, char ** argv){

	int i=0;
	
  	for(i=0; i<argc; i++){
		//conta numero di utenti con quel session_id
		//memorizza session_id
		if(i == 1) strcpy(session_db,argv[i] ? argv[i] : "NULL");
		else strcpy(id_db,argv[i] ? argv[i] : "NULL");
	}

	compose_mess_to_log("Ho letto da DB il seguente id: ",id_db,STRING_TYPE);
	compose_mess_to_log("Ho letto da DB il seguente session_id: ",session_db,STRING_TYPE);

	return QUERY_OK;
}

/*
	Legge socket id da db
*/

int read_socket_id(int argc, char **argv){

	int i=0;
	
  	for(i=0; i<argc; i++){
		//conta numero di utenti con quel session_id
		//memorizza session_id
		if(i == 1) strcpy(socket_db,argv[i] ? argv[i] : "NULL");
		else strcpy(id_db,argv[i] ? argv[i] : "NULL");
	}

	compose_mess_to_log("Ho letto da DB il seguente id: ",id_db,STRING_TYPE);
	compose_mess_to_log("Ho letto da DB il seguente socket_id: ",socket_db,STRING_TYPE);

	return QUERY_OK;
}

int update_nothing_todo(){
	return QUERY_OK;
}

/* 
	Leggo tutte le categorie di files presenti nel db
*/

int read_categories(int argc,char ** argv, char category_name[], char id_category[]){

	int i = 0;

	bzero(category_name,strlen(category_name)+1);

	for(i; i<argc; i++){

		if(i == 0){
			sprintf(id_category,"%s",(argv[i] ? argv[i] : "NULL"));
			compose_mess_to_log("id categoria ",id_category, STRING_TYPE);
		}

		if(i == 1){
			sprintf(category_name,"%s",(argv[i] ? argv[i] : "NULL"));
			compose_mess_to_log("Nome Categoria: ",category_name,STRING_TYPE);
		}		
	}

	return QUERY_OK;
}

/*
	Leggo tutti i files per una determinata categoria
*/

int read_files_category(int argc, char ** argv,char file_name[], char id_file[], char path_file[]){
	
	int i = 0;

	bzero(file_name,strlen(file_name)+1);
	bzero(id_file,strlen(id_file)+1);
	bzero(path_file,strlen(path_file)+1);


	for(i;i<argc;i++){
		//memorizzo categoria
		//debug
		compose_mess_to_log("VALORE LETTO: ",argv[i],STRING_TYPE);
		compose_mess_to_log("INDEX: ",&i,INTEGER_TYPE);

		if(i == 0){
			sprintf(id_file,"%s",(argv[i] ? argv[i] : "NULL"));
			compose_mess_to_log("VALORE SCRITTO: ",id_file,STRING_TYPE);
		}
		if(i == 1){
			sprintf(path_file,"%s",(argv[i] ? argv[i] : "NULL"));
			compose_mess_to_log("VALORE SCRITTO: ",path_file,STRING_TYPE);
		}
		if(i == 2){
			sprintf(file_name,"%s",(argv[i] ? argv[i] : "NULL"));
			compose_mess_to_log("VALORE SCRITTO: ",file_name,STRING_TYPE);
		}
	} 

	//debug
	logger("Fine lettura valori");
	compose_mess_to_log("ID File: ",  id_file,  STRING_TYPE);
	compose_mess_to_log("Path File: ",path_file,STRING_TYPE);
	compose_mess_to_log("Nome File: ",file_name,STRING_TYPE);
	
	//Apro file in lettura
	//compongo path di caricamento
	char path[100];
	strcpy(path,path_file);
	strcat(path,file_name);

	compose_mess_to_log("Path file per determinare dimensione: ",path,STRING_TYPE);

	FILE * fp = fopen(path,"rb");
	if(!fp){
		logger("Dimensione del file 0Bytes, non esiste file nel path di load");
		//errore file non presente, ci metto 0B
		bzero(path_file,50);
		strcpy(path_file,"0");
	}
	else{
		logger("Determino dimensione");
		bzero(path_file,50);
		//determina dimensione file
		fseek(fp, 0, SEEK_END);
		long size=ftell(fp);
		//scrivo dimensione
		char buf_size[20];
		sprintf(buf_size,"%ld",size);
		strcpy(path_file,buf_size);
	}

	fclose(fp);

	compose_mess_to_log("Dimensione file: ",path_file,STRING_TYPE);
}

int read_info_file(int argc,char ** argv){

	int i=0;
	
  	for(i=0; i<argc; i++){
		if(i == 0) strcpy(filename_db,argv[i] ? argv[i] : "NULL");
		else strcpy(path_db,argv[i] ? argv[i] : "NULL");
	}

	compose_mess_to_log("Ho letto da DB il seguente filename: ",filename_db,STRING_TYPE);
	compose_mess_to_log("Ho letto da DB il seguente path: ",path_db,STRING_TYPE);

	return QUERY_OK;
	
}


/*
	Cancella tutte le stringhe utilizzate per memorizzare 
*/
void clear_strings(){
	//cancello preventivamente username e password e session_id già salvati

	logger("Pulisco stringhe");

	strcpy(id_db,"");
	strcpy(username_db,"");
	strcpy(password_db,"");
	strcpy(session_db,"");
	strcpy(socket_db,"");
	strcpy(filename_db,"");
	strcpy(path_db,"");
		
}

