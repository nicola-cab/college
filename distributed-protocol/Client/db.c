#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

#include "db.h"
#include "compose.h"
#include "op.h"
#include "message.h"

//flag query execute
int flag_execution_query =  NO_EXEC;
//nome del database
char * db_name = "../DB/client.db";
//puntatore a funzione (utilizzato per assegnarlo alla routine di callback)
int (*callback_ptr) (void*,int,char**,char**) = NULL;
/*memorizza operazione da eseguire*/
#define NO_OP -1
int select_type_query = NO_OP;
//num same download
int num_same_item_db = 0;

struct download_item *  inizio = NULL;

/********************************************ROUTINES**************************************************/

/*
	Esegue query
*/
int executeQuery(char * sql, int type_query){

	logger("ESEGUO QUERY");

	//clear_strings();

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

	//logger("Connessione aperta");

 	if(rc){
 	   sprintf(error, "Impossibile connettersi al db: %s", sqlite3_errmsg(db));
		logger(error);
		logger("Chiusura connessione al database");
 	   sqlite3_close(db);
	   return QUERY_ERROR;
  	}

	//memorizzo il tipo di operazione richiesta per fare lo switch dentro la callback
	select_type_query = type_query;

	//setto routine di callback da richiamare
	callback_ptr = callback;

	//debug
	if(!callback_ptr)
		logger("Routine di callback non invocata");

	rc = sqlite3_exec(db, sql, callback_ptr, 0, &zErrMsg);

	if(rc!=SQLITE_OK){
		sprintf(error, "SQL error: %s", zErrMsg);
		logger(error);
		return QUERY_ERROR;
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
	Gestore record letti dalla query
*/

int callback(void *NotUsed, int argc, char **argv, char **azColName){

	switch(select_type_query){
		case INSERT:
			flag_execution_query = EXEC;
			//nothing to do
		break;
		case SELECT:
			flag_execution_query = EXEC;
			//costruisco lista di items scaricati o in download
		break;
		case SELECT_ALL_ITEMS_SAVED:

			logger("Carico download vecchi da db");

			flag_execution_query = EXEC;
			//leggo tutti i download effettuati e memorizzati sul db

			int i = 0;

			char name[30];
			char path[60];
			char percentuale[5];
			int status = IN_DOWNLOAD;

			for(i;i<argc;i++){
				//scrivo in memoria tutti i download effettuati e memorizzati nel db
				if(i == 1){
					strcpy(name,argv[i]?argv[i]:"NULL");
					compose_mess_to_log("NOME: ",name,STRING_TYPE);
				}
				if(i == 2){
					char tmp[30];
					strcpy(tmp,argv[i]?argv[i]:"NULL");
					if(!strcmp(tmp,"downloaded"))
						status = DOWNLOADED;
				}
				if(i == 3){
					strcpy(percentuale,argv[i]?argv[i]:"NULL");
					compose_mess_to_log("DOWNLOAD PERC: ",percentuale,STRING_TYPE);
				}
				if(i == 4){
					strcpy(path,argv[i]?argv[i]:"NULL");
					compose_mess_to_log("PATH: ",path,STRING_TYPE);
				}
			}
	
			logger("Punto critico creo PILA");
			//alloco memoria per avere tutti i download eseguiti precedentemente
			inizio = write_new_download(inizio,name, status , path);
			inizio->percentuale = atoi(percentuale);

		break;
		case SELECT_COUNT:
			flag_execution_query = EXEC;
			int j = 0;
			for(j;j<argc;j++){
				num_same_item_db = argv[i]?atoi(argv[i]):atoi("NULL");
			}
		break;
		default:
		break;
	}

	return QUERY_OK;
	
}

/*
	Inserisco nuovo download nel db
*/
int execute_insert_new_item(char * nome_elemento, char * status ,char * path){

	//verifico che il file non sia già stato memorizzato su db

	logger("Eseguo inserimento nuovo contenuto nel db");

	char sql[150];
	strcpy(sql,"SELECT * from items where descrizione = '");
	strcat(sql,nome_elemento);	
	strcat(sql,"'");

	executeQuery(sql,SELECT);

	logger("SELECT eseguita");

	if(flag_execution_query == NO_EXEC){
		//preparo insert normalmente (non ci sono vecchi download) */

		//costruisco query di insert per nuovo contenuto scaricato

		compose_mess_to_log("NOME FILE:",nome_elemento,STRING_TYPE);
		compose_mess_to_log("STATUS:",status,STRING_TYPE);
		compose_mess_to_log("PATH:",path,STRING_TYPE);

		bzero(sql,strlen(sql)+1);
		strcpy(sql,"INSERT into items(descrizione,status,path,percentuale) values('");
		strcat(sql,nome_elemento);
		strcat(sql,"','");
		strcat(sql,status);
		strcat(sql,"','");
		strcat(sql,path);
		strcat(sql,"','0')");

		executeQuery(sql,INSERT);
	} 
	else{
		//eseguo solo update per dello status, perchè sto riscaricando il contenuto
		bzero(sql,strlen(sql)+1);
		strcpy(sql,"UPDATE items set status ='in_download' where descrizione='");
		strcat(sql,nome_elemento);
		strcat(sql,"'");
		
		executeQuery(sql,INSERT);
	}
	//non inserisco un ramo else poichè lato db non mi interessa mantenere un ordine cronologico

	return QUERY_OK;
}

/*
	Leggo tutti i download effettuati
*/
void * execute_select_items(void * pointer){
	//costruisco query per leggere contenuti scaricati

	char sql[100];
	strcpy(sql,"SELECT * from items");

	inizio = (struct download_item *) pointer;

	executeQuery(sql,SELECT_ALL_ITEMS_SAVED);
	
	return inizio;
}

/*
	Aggiorno percentuale download contenuto
*/
int update_perc(char * name_file,int perc){

	logger("Eseguo update della percentuale di scaricamento");

	char perc_s[4];
	sprintf(perc_s,"%d",perc);
	
	char sql[150];
	strcpy(sql,"UPDATE items set percentuale = '");
	strcat(sql,perc_s);
	strcat(sql,"'");
	strcat(sql," WHERE descrizione = '");
	strcat(sql,name_file);
	strcat(sql,"'");

	executeQuery(sql,INSERT);

	if(perc == 100){
		//download completato
		bzero(sql,strlen(sql)+1);
		strcpy(sql,"UPDATE items set status = 'downloaded' where percentuale = 100 and descrizione ='");
		strcat(sql,name_file);
		strcat(sql,"'");
		executeQuery(sql,INSERT);
	}
	return QUERY_OK;	
}

int count_num_download(char * name){

	char sql[100];
	strcpy(sql,"SELECT COUNT(*) FROM ITEMS WHERE DESCRIZIONE LIKE'%");
	strcat(sql,name);
	strcat(sql,"'");
	
	executeQuery(sql,SELECT_COUNT);

	return num_same_item_db;	
}


/*int all_downloads(){

	char sql[100];
	strcpy(sql,"SELECT COUNT(*) FROM ITEMS");
	strcat(sql,name);
	strcat(sql,"'");
	
	executeQuery(sql,SELECT_COUNT);

	return num_same_item_db;	

}*/
