#include "op.h"

/*
	Riconosce il messaggio inviato dal client e prepara il messaggio di risposta
*/

int recognize_mess_and_response(char * mess,int socket){
	
	//eseguo il parse del messaggio
	int num_val = 0;

	/*Non è una soluzione elegante!!! (correggi)*/
	const int CUSTOM_NUM = 100;
	char ** vs = (char**)malloc(CUSTOM_NUM * sizeof(char*));	

	int ret = parse(mess,*(&vs),&num_val);
	char * tmp; //buffer tmp nel quale copiare la stringa per memorizzare la risposta

	//debug	
	if(vs == NULL){
		logger("Vettore di stringhe restituito dalla routine di splitting NULLO, errore grave!!!");
		//componi un messaggio d'errore
		//bzero(mess,strlen(mess));  //cancello i bytes del vecchio messaggio ricevuto
		compose_protocol_mess(ERROR,"errore generale del server",mess);  //compongo messaggio d'errore
		return WRITE_SOCKET;
	}


	//compose_mess_to_log("valore di ritorno split: ",ret,INTEGER_TYPE);
	logger("parse messaggio terminato");
	
	if(ret == ERROR){
		logger("Errore nella sintassi del messaggio spedito dal client");
		//componi un messaggio d'errore
		//bzero(mess,strlen(mess));  //cancello i bytes del vecchio messaggio ricevuto
		compose_protocol_mess(ERROR,"nessuna corrispondenza con la sintassi del protocollo",mess);  //compongo messaggio d'errore
		//libero la memoria dopo aver utilizzato il vettore di stringhe
		free(vs);
		return WRITE_SOCKET;
	}
	else{
		logger("INIZIO A PROCESSARE L'OPERAZIONE RICHIESTA");

		compose_mess_to_log("trasformazione id request in intero, id: ", vs[0], STRING_TYPE);

		//se lo split delle stringhe è stato eseguito correttamente		
		int val = atoi(vs[0]);

		compose_mess_to_log("id request trasformato in intero, id: ", &val, INTEGER_TYPE);

		switch(val){

			/*********************************************************************************************************************************/
																	/*Errore non è possibile convertire la stringe in un intero*/
			case 0:
				logger("Errore nella codifica nell'id dell'operazione");
				compose_mess_to_log("Errore nella sintassi del messaggio spedito dal client, id operazione non valido,ID: ", vs[0], STRING_TYPE);
				//bzero(mess,strlen(mess));  //cancello i bytes del vecchio messaggio ricevuto
				char buf[100];
				bzero(buf,strlen(buf)+1);
				sprintf(buf,"%s%s","Non esistono request utilizzando il seguente id: ",vs[0]);  //compongo messaggio d'errore
				compose_protocol_mess(ERROR,buf,mess);  
				//libero la memoria dopo aver utilizzato il vettore di stringhe
				free(vs);
				return WRITE_SOCKET;
			break;

			/********************************************************************************************************************************/

			/*******************************************************************************************************************************/
																 				/*Ok proveniente dal client*/
				case OK:
					logger("Messaggio d'ok inviato dal client fuori da un flusso di download");
					char buf_new[100];
					bzero(buf_new,strlen(buf_new)+1);
					sprintf(buf_new,"%s","Mess di OK fuori dal flusso di download");
					compose_protocol_mess(ERROR,buf_new,mess);
					free(vs);
				return WRITE_SOCKET;
				break;

			/*******************************************************************************************************************************/

			/*********************************************************************************************************************************/
																			/*Richiesta login client*/
			case REQUEST_LOGIN:
				logger("Richiesta di login");
				int ret_login;

				if(num_val == 3){
					ret_login = do_login(socket,mess,vs[1],vs[2]);
				}

				else{
					//mancano parametri o il messaggio non è ben formato, comunico errore
					compose_protocol_mess(ERROR,"nessuna corrispondenza con la sintassi del protocollo",mess);  //compongo messaggio d'errore					
					free(vs);
					return WRITE_SOCKET;
				}

				//libero la memoria dopo aver utilizzato il vettore di stringhe
				free(vs);
				return ret_login;
			break;

			/**********************************************************************************************************************************/

			/**********************************************************************************************************************************/
																			/*Richiesta catalogo*/
  			case REQUEST_CATALOGO:
				logger("Richiesta catalogo multimediale sever");
				int ret_catalogo;

				if(num_val == 2){
					if(atoi(vs[1]) > MIN_ID)
						ret_catalogo = do_request_catalogo(socket,mess,vs[1]);

					else{
						logger("id di sessione non valido");
						compose_protocol_mess(ERROR,"ID NON VALIDO",mess);  //compongo messaggio d'errore					
						free(vs);
						return WRITE_SOCKET;
					}
				}

				else{
					//mancano parametri o il messaggio non è ben formato, comunico errore
					compose_protocol_mess(ERROR,"nessuna corrispondenza con la sintassi del protocollo",mess);  //compongo messaggio d'errore					
					free(vs);
					return WRITE_SOCKET;
				}

				//libero la memoria dopo aver utilizzato il vettore di stringhe
				free(vs);
				return ret_catalogo;
			break;
	
			/**********************************************************************************************************************************/

			case REQUEST_CATALOGO_ITEMS:
				
				logger("Richiesta files per categoria");
				int ret_catalogo_items = WRITE_SOCKET;

				if(num_val == 3){
					if(atoi(vs[1]) > MIN_ID){
						//prelevo da db tutti i file per la categoria richiesta
						ret_catalogo_items = do_request_catalogo_items(socket,mess,vs[1],vs[2]);						
					}
					else{
						logger("id di sessione non valido");
						compose_protocol_mess(ERROR,"ID NON VALIDO",mess);  //compongo messaggio d'errore					
						free(vs);
						return WRITE_SOCKET;
					}
				}
				else{
					//mancano parametri o il messaggio non è ben formato, comunico errore
					compose_protocol_mess(ERROR,"nessuna corrispondenza con la sintassi del protocollo",mess);  //compongo messaggio d'errore					
					free(vs);
					return WRITE_SOCKET;
				}
				free(vs);
				return ret_catalogo_items;
			break;

			case REQUEST_FILE_ID:
				
			break;

			case REQUEST_FILE:
				//libero la memoria dopo aver utilizzato il vettore di stringhe
				logger("Richiesta download di un file");
				int ret_download_id_file = NO_WRITE_SOCKET;

				if(num_val == 4){

					if(atoi(vs[1]) > MIN_ID){
						ret_download_id_file = do_request_download_item(socket,mess,vs[1],vs[2],vs[3]);
					}
					else{
						logger("id di sessione non valido");
						compose_protocol_mess(ERROR,"ID NON VALIDO",mess);  //compongo messaggio d'errore					
						free(vs);
						return WRITE_SOCKET;
					}
				}
				else{
					//mancano parametri o il messaggio non è ben formato, comunico errore
					compose_protocol_mess(ERROR,"nessuna corrispondenza con la sintassi del protocollo",mess);  //compongo messaggio d'errore					
					free(vs);
					return WRITE_SOCKET;
				}

				free(vs);
				return ret_download_id_file;
			break;	

			case REQUEST_CLOSE:
				logger("Richiesta chiusura connessione");
				int ret_close;

				if(num_val == 2){
					if(atoi(vs[1]) > MIN_ID){
						write_to_socket(socket,"OK",PROTOCOL,strlen("OK")+1);
						ret_close =  do_close_conn(socket,mess,vs[1]);
						//sincronizzo chiusura
					}
					else{
						logger("id di sessione non valido");
						compose_protocol_mess(ERROR,"ID NON VALIDO",mess);  //compongo messaggio d'errore
						return WRITE_SOCKET;
					}
				}

				else{
					compose_protocol_mess(ERROR,"nessuna corrispondenza con la sintassi del protocollo",mess);  //compongo messaggio d'errore
					return WRITE_SOCKET;
				}

				free(vs);
				return ret_close;
			break;

			default:
				logger("Nessuna corrispondenza con la sintassi del protocollo");
				compose_protocol_mess(ERROR,"nessuna corrispondenza con la sintassi del protocollo",mess);  //compongo messaggio d'errore
				free(vs);
				return WRITE_SOCKET;
			break;
		}
	}
}

/*
	Compone l'sql per loggare il client sul server
*/
int do_login(int socket,char * mess,char * username, char * password){

	//3 parametri frutto della split per 
	//if(num_val == 3){


		char log[200];
		char sql[300];
		char * part_one = "SELECT id,username,password,session FROM users WHERE username='";
		char * close = "'";
		char * part_two = " and password='";

		bzero(mess,strlen(mess));
	
		//compongo la query
		strcpy(sql,part_one);
		strcat(sql,username);
		strcat(sql,close);
		strcat(sql,part_two);
		strcat(sql,password);
		strcat(sql,close);


		//richiamo il gestore sqlite per eseguire la query
		int ret = executeQuery(sql, SELECT, REQUEST_LOGIN);

		//debug
		/*compose_mess_to_log("ID:",id_db,STRING_TYPE);
		compose_mess_to_log("User:",username_db,STRING_TYPE);
		compose_mess_to_log("Pass:",password_db,STRING_TYPE);
		compose_mess_to_log("session:",session_db,STRING_TYPE);*/



		if(ret == QUERY_OK){

			//non esiste alcun id nel db per le credenziali inviate....errore non esistono le credenziali d'accesso per il client
			if(flag_execution_query == NO_EXEC){
				//loggata
				bzero(log,strlen(log));
				strcpy(log,"Username e password non corrette");
				logger(log);
				//compongo messaggio protocollo
				compose_protocol_mess(ERROR,"username e/o password non corretta/i",mess);
				return WRITE_SOCKET;
			}

			else{
				compose_mess_to_log("Session ID: ",session_db,STRING_TYPE);
					if(!strcmp(session_db,"NULL") || !strcmp(session_db,"")){
		
						bzero(log,strlen(log));
						strcpy(log,"Genero session_id per il client");
						logger(log);

						/*genero un session_id che il client dovrà utilizzare per tutta la durata della connessione al server, in tutte le richieste che
						farà. Per creare il session_id, utilizzo la seguente logica:
						1) Leggo ID della tabella users da db
						2) Genero un numero casuale fra 1000 e 10000 e ci aggiungo l'ID letto dal db
						*/

						//Genero un session id fino a quando non ne genero uno univoco
						int ret_check_session;
						char session_value[6];

						char *tmp_id = malloc((strlen(id_db)+1)*sizeof(char));
						strcpy(tmp_id,id_db);

						do{
							int session_id = generate_session_id(atoi(session_db));
							//eseguo query per verificare se il session_id è univoco.
							//pulisco vecchie stringhe e riscrivo la mia nuova select
						
							bzero(sql,strlen(sql));						
							part_one = "";
							part_one = "SELECT id,session FROM users WHERE session='";
							strcat(sql,part_one);
	
							//converto da int a stringa
							sprintf(session_value,"%d",session_id);

							//compongo query
							strcat(sql,session_value);
							strcat(sql,close);

							ret_check_session = executeQuery(sql, SELECT, SESSION_ID_CLIENT);
						
							compose_mess_to_log("Eseguito: ",&flag_execution_query,INTEGER_TYPE);
						
						}while(flag_execution_query == EXEC && (strcmp(session_db,"NULL")) && (strcmp(session_db,"")));

						logger("Session id generato univocamente, inserisco nel db..");

						//l'id generato è univoco (compongo query per l'update)

						bzero(sql,strlen(sql));
						part_one = "";
						part_one = "UPDATE users SET session ='";
						strcat(sql,part_one);
						strcat(sql,session_value);
						strcat(sql,close);
						strcat(sql," WHERE id = '");
						strcat(sql, tmp_id);
						strcat(sql,close);

						int ret_update = executeQuery(sql, UPDATE, REQUEST_LOGIN);

						if(ret_update == QUERY_OK){
							//genero messaggio di risposta confermando il login
						
							//Set socket utilizzato dal client su db
							set_socket_db(socket,tmp_id);
							compose_protocol_mess(OK,session_value,mess);
							return WRITE_SOCKET;
						}
						else{
							//loggata
							bzero(log,strlen(log));
							strcpy(log,"Verifica la query, errore nell'esecuzione");
							logger(log);
							//compongo messaggio
							compose_protocol_mess(ERROR,"errore generale del server nell'esecuzione del login",mess);
							return WRITE_SOCKET;
						}

						free(tmp_id);
					}

					else{
						bzero(log,strlen(log));
						strcpy(log,"Tentativo di login di un utente per più di una volta");
						logger(log);
						//compongo messaggio protocollo
						compose_protocol_mess(ERROR,"credenziali già in uso",mess);
						return WRITE_SOCKET;
					}
				}
			}
		else{
			bzero(log,strlen(log));
			strcpy(log,"Verifica la query perchè ci possono essere errori");
			logger(log);
			//compongo messaggio protocollo
			compose_protocol_mess(ERROR,"errore generale del server nella richiesta del login",mess);
			return WRITE_SOCKET;
		}

	/*}

	else{
			//mancano parametri o il messaggio non è ben formato, comunico errore
			//bzero(mess,strlen(mess));  //cancello i bytes del vecchio messaggio ricevuto
			compose_protocol_mess(ERROR,"nessuna corrispondenza con la sintassi del protocollo",mess);  //compongo messaggio d'errore					
			return WRITE_SOCKET;
		}*/
}

/*
	Esegue query per recuperare catalogo dei file multimediali
*/

int do_request_catalogo(int socket,char *mess,char *session_id){

//	char ** catalogo = (char **) malloc(NUM_MAX_CATEGORIE * sizeof(char*));

	get_id_from_socket(session_id);

	if(flag_execution_query == NO_EXEC){
		compose_mess_to_log("Non eseguo alcuna query, l'id passato non è corretto : ",session_id,STRING_TYPE);
		//l'id di sessione passato non esiste!!!
		compose_protocol_mess(ERROR,"ID di sessione non valido",mess);  //compongo messaggio d'errore
	}

	else{
		//passo ad eseguire una query per comporre la risposta del server contenente 
		//tutte le categorie di files presenti sul server
		get_categorie();

		if(flag_execution_query == NO_EXEC){
			logger("Errore generale del server nella richiesta del catalogo");
			//non ho seguito la query
			compose_protocol_mess(ERROR,"Errore generale del server nella richiesta del catalogo",mess);  //compongo messaggio d'errore
		}
		else{
			//buffer nel quale eseguo la concatenzione dei nomi delle varie categorie

			//compose_mess_to_log("Numero di categorie ",&num_categorie,INTEGER_TYPE);

			char tosend [1000];
			cat(nome_categoria, sep, tosend, num_categorie);	
			//compongo messaggio di richiesta autenticazione verso il server
			compose_protocol_mess(OK,tosend,mess);			
		}
	}

//	free(catalogo);
	return WRITE_SOCKET;
}

/*
	Leggo da db tutti i files per la categoria
*/
int do_request_catalogo_items(int socket,char *mess,char * session_id,char *category_id){

	//char ** items = (char **) malloc(NUM_MAX_FILES_CATEGORIA * sizeof(char*));

	get_id_from_socket(session_id);

	if(flag_execution_query == NO_EXEC){
		compose_mess_to_log("Non eseguo alcuna query, l'id passato non è corretto : ",session_id,STRING_TYPE);
		//l'id di sessione passato non esiste!!!
		compose_protocol_mess(ERROR,"ID di sessione non valido",mess);  //compongo messaggio d'errore
	}

	else{
		//passo ad eseguire una query per comporre la risposta del server contenente 
		//tutti i files per categoria presenti sul server
		//char tmp_id_cat[5];
		//sprintf(tmp_id_cat,"%d",atoi(category_id));
		get_file_categoria(category_id);

		if(flag_execution_query == NO_EXEC){
			logger("Errore generale del server nella richiesta del catalogo");
			//non ho seguito la query
			compose_protocol_mess(ERROR,"Nessun file nel catalogo per la categoria selezionata",mess);  //compongo messaggio d'errore
		}
		else{
			//buffer nel quale eseguo la concatenzione dei nomi delle varie categorie
			char tosend [1000];
			cat(nome_files, sep, tosend, num_files);	
			//compongo messaggio di richiesta autenticazione verso il server
			compose_protocol_mess(OK,tosend,mess);			
		}
	}

	//free(items);
	return WRITE_SOCKET;
}

/*
	Legge da db le info riguardanti il file richiesto e prepara i messaggi di sincronizzazione
	da spedire sul socket per poter effettuare il download del contenuto.
*/
int do_request_download_item(int socket, char * mess, char * session_id, char * category_id, char * file_id){

	get_id_from_socket(session_id);
	int ret_download_item = WRITE_SOCKET;

	if(flag_execution_query == NO_EXEC){
		compose_mess_to_log("Non eseguo alcuna query, l'id passato non è corretto : ",session_id,STRING_TYPE);
		//l'id di sessione passato non esiste!!!
		compose_protocol_mess(ERROR,"ID di sessione non valido",mess);  //compongo messaggio d'errore
	}

	else{
		//passo ad eseguire una query per comporre la risposta del server contenente 
		//tutte le categorie di files presenti sul server

		int id = atoi(file_id);
		/*id +=1;
		bzero(file_id,strlen(file_id)+1);*/
		sprintf(file_id,"%d",id);

		get_file_infos(category_id,file_id);

		if(flag_execution_query == NO_EXEC){
			logger("Errore generale del server nella richiesta del catalogo");
			//non ho seguito la query
			compose_protocol_mess(ERROR,"Nessun file selezionabile dal catalogo",mess);  //compongo messaggio d'errore
		}
		else{
			/*
				Gestisco la richiesta di download. 
				compongo stringa per determinare path nel quale trovare il file
				passo PATH file alla routine di gestione download.
				La routine dopo aver utilizzato il canale socket passato
				come parametro per sincronizzare client e server crea un nuovo processo (di download)
				che scrive sul nuovo canale socket effettuatando il download.
				Questa scelta implementativa permette di ottenere un server concorrente con la possibilità
				di più download paralleli (si è scelto 5 MAX)
			*/
			ret_download_item = do_manage_download(socket,mess,filename_db,path_db);
		}
	}

	return ret_download_item;
}


/*
	Gestisco operazioni di download
*/

int num_download = 0;

int do_manage_download(int socket,char mess[], char filename[], char path_file[]){

	//per prima cosa apro il file in lettura per verificare l'esistenza dello stesso

	char path_to_load_file [100];

	strcpy(path_to_load_file,path_file);
	strcat(path_to_load_file,filename);

	compose_mess_to_log("File to load:",path_to_load_file,STRING_TYPE);

	FILE * fp = fopen(path_to_load_file,"rb");

	if(!fp){
		logger("File non trovato, errore grave... verificare che il file si trovi dentro la directory FILES");
		compose_protocol_mess(ERROR,"Errore generale del server nel download del contenuto",mess);
		return WRITE_SOCKET;
	}
	
	else{
		logger("Inizio gestione messaggi di sincronizzazione download");
		//ottengo il pid del processo corrente
		int pid = getpid();
		compose_mess_to_log("Pid del processo corrente per determinare porta: ",&pid,INTEGER_TYPE);
		//pid_processo + porta per mettere il server in ascolto su un'altra porta
		int port_download = pid + PORT;
		compose_mess_to_log("Nuova porta per gestione download: ",&port_download,INTEGER_TYPE);
		//metto in ascolto il server su di una nuova porta
		logger("Metto in ascolto il server sulla nuova porta");

		int download_sd = 0;

		/*In caso di dupplicazione di connessioni aumenta di 1 la porta alla quale connettersi (più download paralleli, MAX 5)*/
		do{
			download_sd= create_server(port_download,1);

				if(download_sd == -1){
					port_download++;	//sposto di un'unità la porta di download per cercare di creare un nuovo TSAP
				}
				//else if(num_download > 0){
				//}
		}while(download_sd == -1 );//&& num_download < MAX_DOWNLOAD_SIM);


		num_download ++;	//incremento il contattore di download per tenere traccia di tutti i download che il client ha in esecuzione


		/*if(num_download == MAX_DOWNLOAD_SIM)	
		{
			logger("Gestione download del client errata, troppi download contemporanei");
			compose_protocol_mess(ERROR,"Errore, troppi download contemporanei",mess);
			write_to_socket(socket,mess,PROTOCOL,strlen(mess)+1);
			close(download_sd);
			return WRITE_SOCKET;
		}*/

		//scrivo sul "vecchio" canale socket la nuova porta sulla quale effettuare il download
		char tosend[20];
		sprintf(tosend,"%d",port_download);
		logger("Richiedo apertura nuova connessione al client");
		compose_protocol_mess(REQUEST_CHANNEL,tosend,mess);
		//non effettuo alcuna return, poichè il messaggio d'ok deve stare dentro un flusso di download
		write_to_socket(socket,mess,PROTOCOL,strlen(mess)+1);
		
		logger("In attesa che il client si connetta per iniziare download ... ");
		//Attendo che il client si connetta sulla nuova porta (MAX 5 DOWNLOAD CONTEMPORANEI)
		int sd_client_download = create_client_channel(download_sd);
		close(download_sd);
		//..client connesso

		int pid_download_process = fork();
		if(pid_download_process == -1){
			//errore nella fork...attenzione, scrivo sul canale di sincronizzazione (ossia quello utilizzato per scambiare i mess)
			compose_protocol_mess(ERROR,"Impossibile effuettuare download",mess);
			return WRITE_SOCKET;
		}

		if(pid_download_process == 0){
			//nuovo processo
			int current_pid = getpid();
			compose_mess_to_log("Nuovo processo di download creato, PID: ",&current_pid,INTEGER_TYPE);
			logger("Download started");
			//Leggo bytes da file fino alla fine fisica del file

			char bytes_file[MAX_MESS_SOCKET_LENGHT];
			int nbytes = 0;
			int count_b = 0;

			bzero(bytes_file,MAX_MESS_SOCKET_LENGHT);	
			while(nbytes = fread(bytes_file,1,MAX_MESS_SOCKET_LENGHT,fp)){				
				//compose_mess_to_log("La fread ha restituito numero di bytes: ",&nbytes,INTEGER_TYPE);
				write_to_socket(sd_client_download,bytes_file, FILE_BYTES, nbytes);
				count_b += nbytes;
				bzero(bytes_file,MAX_MESS_SOCKET_LENGHT);
			}

			compose_mess_to_log("Numero di bytes scritti ", &count_b, INTEGER_TYPE);
			/*do{
				
			}while(!feof(fp) && (nbytes!= -1 || nbytes != 0)); */

			num_download--; //riacquista credito di download quando finisco il download corrente

			/*SINCRONIZZAZIONE CHIUSURA DOWNLOAD CLIENT-SERVER*/
			logger("Inizio Sincronizzazione per Chiusura");
		
			//mistero della vita.. ci vuole un secondo di sleep prima di riutilizzare il canale socket
			sleep(1);

			//Comunico fine download
			compose_protocol_mess(OK,"Fine download",mess);
			write_to_socket(sd_client_download,mess,PROTOCOL,strlen(mess)+1);

			//Attendo reply del client, non do importanza al messaggio
			read_from_socket(sd_client_download,mess);
			logger("Chiudo socket download e file");

			//chiudo file e socket
			fclose(fp);
			close(sd_client_download);

			/*************************************************/
			
			int pid_end_proc = getpid();
			compose_mess_to_log("Fine processo: ",&pid_end_proc,INTEGER_TYPE);
			exit(0);
		}

		else{
			//processo padre (attesa non bloccante in attesa che termini il processo figlio di download)
			signal(SIGCHLD,_manage_exit_child); 
			return  NO_WRITE_SOCKET;
		}
	}
}


/*
	Esegue chiusura connessione client
	NB: Decomentare i blocchi in cui si compongono i messaggi se si vuole mandare notifica
	al client, ma se il client chiude la connessione senza occuparsi del messaggio del server,
	cosa molto probabile, allora si ottiene un effetto echo, e quindi computazione inutile con 
	conseguente overhead oltretutto inutile
*/

int do_close_conn(int socket,char * mess,char * session_id){

				//if(num_val == 2){
					//se la trasformazione d
					if(atoi(session_id)){

						/*
							NB: Questo passaggio non è strettamente necessario poichè posso chiudere la connessione 
								anche direttamente attraverso l'id letto nella query di login e memorizzato in una variabile globale.
								Per evitare che bugs possano inficiare questo comportamento preferisco leggere da DB tutte le informazioni
								riguardanti il client
						*/

						//verifico l'id passato e chiudo connessione 
						if(atoi(session_id) > MIN_ID){
				
							get_id_from_socket(session_id);
	
							if(flag_execution_query == NO_EXEC){
								logger("Non eseguo alcuna operazione di disconessione, la request è errata");
								//l'id di sessione passato non esiste!!!
								/*compose_protocol_mess(ERROR,"ID di sessione non valido",mess);  //compongo messaggio d'errore
								//libero la memoria dopo aver utilizzato il vettore di stringhe	
								return WRITE_SOCKET;*/
							}
							else{
							/*Fine della chiamata, chiudo il socket*/
            			_manage_close_socket(atoi(socket_db));
							//pulisco socket e session_id su db
							clear_db_session_socket(atoi(id_db),REQUEST_CLOSE);
							//nothing to-do... restituisco REQUEST_CLOSE (500) in modo tale da terminare il ciclo while e chiudere socket e terminare
							}
						}
						else{
							/*Fine della chiamata, chiudo il socket*/
            			_manage_close_socket(atoi(socket_db));
							//pulisco socket e session_id su db
							clear_db_session_socket(atoi(id_db),REQUEST_CLOSE);
							//nothing to-do... restituisco REQUEST_CLOSE (500) in modo tale da terminare il ciclo while e chiudere socket e terminare
						}
					}

					else{
					logger("Non eseguo alcuna operazione di disconessione, la request è errata");
						/*compose_protocol_mess(ERROR,"ID di sessione non valido",mess);  //compongo messaggio d'errore
						return WRITE_SOCKET;*/
					}

			/*	}

				else{
					logger("Non eseguo alcuna operazione di disconessione, la request è errata");
					/*compose_protocol_mess(ERROR,"nessuna corrispondenza con la sintassi del protocollo",mess);  //compongo messaggio d'errore
					return WRITE_SOCKET;*/
				//}

	return REQUEST_CLOSE;
}



/*
	Genera un numero casuale fra 1000 e 9000 e somma a questo numero l'id
*/
int generate_session_id(int id){
	//srand per generare sempre un numero diverso
	srand(time(NULL));
	//numero fra 
	int num = rand() % MAX_ID + MIN_ID;

	char log[100];
	sprintf(log,"%s%d","session_id generato: ",(num+id));
	logger(log);

	return (num+id);
}


/*
	Update e cancella socket e session su db
*/

void clear_db_session_socket(int id, int type){

	//Pulisco session_id e socket su db
				
	//INSERIRE NEL DB IL CANALE SOCKET
	char tmp_sql[70];
	char * first;
		
	//switcho in base al tipo della request (se il processo termina inaspettatamente pulisco fields del db dal padre)
	if(type == REQUEST_CLOSE)
		first = "UPDATE users SET socket = NULL , session = NULL WHERE id='";
	else
		first = "UPDATE users SET socket = NULL , session = NULL WHERE socket='";

	char second[5];
	sprintf(second,"%d",id);
	strcpy(tmp_sql,first);	
	strcat(tmp_sql,second);
	strcat(tmp_sql,"'");
					
	executeQuery(tmp_sql, UPDATE, REQUEST_CLOSE);	
}

/*set del socket*/

void set_socket_db(int socket,char *id){

	//Pulisco session_id e socket su db
				
	//INSERIRE NEL DB IL CANALE SOCKET
	char tmp_sql[70];
	char * first = "UPDATE users SET socket = '";
	char second[5];
	strcpy(tmp_sql,first);
	sprintf(second,"%d",socket);
	strcat(tmp_sql,second);
	strcat(tmp_sql,"'");
	strcat(tmp_sql," WHERE id='");
	strcat(tmp_sql,id);
	strcat(tmp_sql,"'");
					
	executeQuery(tmp_sql, UPDATE, REQUEST_LOGIN);
}


/*Legge numero del canale socket utilizzato dal DB*/
void get_id_from_socket(char * id){

	char tmp_sql[70];						
	char * first = "SELECT id,socket FROM users WHERE session='";
	strcat(tmp_sql,first);
	//converto da int a stringa
	strcat(tmp_sql,id);
	strcat(tmp_sql,"'");

	executeQuery(tmp_sql, SELECT, SOCKET_ID_CLIENT);
}

/*Leggo categorie dal db*/
void get_categorie(void){

	logger("Preparo query per leggere nome delle categorie");

	char tmp_sql[70];	
	//inutile cancellare i byte.					
	bzero(tmp_sql,70);
	char * first = "SELECT id,descrizione FROM catalogo";
	strcpy(tmp_sql,first);

	//reset dei contatori/indici utilizzati per questa query
	index_elements = 0;
	num_categorie = 0;
	
	executeQuery(tmp_sql, SELECT, ALL_CATEGORIES_DB);

	compose_mess_to_log("Numero categorie lette: ",&num_categorie,INTEGER_TYPE);

}

/*Lettura file per una determinata categoria*/
void get_file_categoria(char tipo_categoria[]){
	
	logger("Preparo query per leggere tutti i file per una categoria");

	char tmp_sql[70];
	char * first = "SELECT id,path,descrizione FROM items where tipo = '";
	strcpy(tmp_sql,first);
	strcat(tmp_sql,tipo_categoria);
	strcat(tmp_sql,"'");

	//reset dei contatori/indici utilizzati per questa query
	num_files = 0;
	index_elements_file =0;

	executeQuery(tmp_sql,SELECT,ALL_ITEMS_CATEGORY);

	compose_mess_to_log("Numero di files letti: ",&num_files,INTEGER_TYPE);
}


/*Lettura su db delle info riguardanti il file*/
void get_file_infos(char *category_id,char *file_id){

logger("Preparo query per leggere tutti i file per una categoria");

	char tmp_sql[70];
	char * first = "SELECT descrizione,path FROM items where tipo = '";
	strcpy(tmp_sql,first);
	strcat(tmp_sql,category_id);
	strcat(tmp_sql,"' and id='");
	strcat(tmp_sql,file_id);
	strcat(tmp_sql,"'");

	executeQuery(tmp_sql,SELECT,FILE_INFOS);
}


