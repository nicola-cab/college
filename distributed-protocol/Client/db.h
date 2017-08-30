//#include "op.h"

/*
	Gestione database per il download di contenuti
*/

//definisco le macro per definire se la query è andata bene o meno
#define QUERY_OK 0
#define QUERY_ERROR -1

//definisco il tipo di query che devo eseguire (per il momento solo select o update)
#define SELECT 0
#define INSERT 1
#define SELECT_ALL_ITEMS_SAVED 2
#define SELECT_COUNT 3

extern int num_same_item_db;

//flag per determinare se la query è stata eseguita o meno
#define NO_EXEC -1
#define EXEC 0
extern int flag_execution_query;

/*
	Header da importare per utilizzare le routine d'accesso a sqlite db
*/

//nome del database (client.db)
extern char *db_name;

//soluzione poco pulita, si dovrebbe allocare la memoria dinamicamente
//#define MAX_DOWNLOAD 1000

/***************************************ESEGUE QUERY GENERICA*************************************/
int executeQuery(char * sql, int type_query);
int callback(void *NotUsed, int argc, char **argv, char **azColName);

int execute_insert_new_item(char *, char *, char *);
void * execute_select_items(void * );
int update_perc(char *,int);
int count_num_download(char *);
//int all_downloads();
