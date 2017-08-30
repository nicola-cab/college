#include "op.h"

const char * extern_use = "extern_use";

//separatori utilizzati per concatenare le stringhe
char sep[1] = {';'};
//path nel quale salvare i files scaricati
char * path_to_save = "../FILES/";

//path dei file per definire il template
char * path_template_menu = "../TEMPLATE/menu.txt";
char * path_template_loggin = "../TEMPLATE/menu_loggin.txt";

//conta il numero di download eseguiti
int download_files = 0;

//status da inserire sul db
char * status_downloaded = "downloaded";
char * status_in_download = "in_download";


struct download_refresh{
	GtkProgressBar * bar;
	float perc;
	int pipe_fd;
};



//pipe per comunicare percentuale download
//int fd_pipe[2];

void close_conn(int sd){

	/*Chiudo canale*/
	
	//debug
	char b[30] = "Chiudo il canale: ";
	char b1[3];
	sprintf(b1,"%d",sd);
	strcat(b,b1);
	logger(b);

	//invio la richiesta di chiusura connessione al server
	char tosend[10];
	sprintf(tosend,"%d",session_id.id);
	compose_protocol_mess(REQUEST_CLOSE,tosend,buff);
	write_to_socket(sd,buff);

	//attendo una risposta del server alla quale non do importanza, mi serve solo ed esclusivamente per sincronizzare client e server in chiusura
	read_from_socket(sd,buff,PROTOCOL);

	//libero la memoria dalla lista dei download (NON SERVE PIU')
//	free(items);
	close(sd);
}

/*
	Leggo l'input da tastiera verificando possibili errori
*/
void read_stdinput(int * choice, int left , int right ){

	//leggo la scelta operata dall'utente (controllando l'input)
	//il controllo riguarda:
	//Input errrato (esempio caratteri o stringhe e float)
	//valore compreso fra le opzioni che vengono riportate nel menu
	int ret;
	do{
		//svuota buffer scanf, serve per evitare che il programma vada in loop se c'è stato un errore di digitazione
		if(!ret)
			scanf ("%*[^\n]");
		
		printf("Choice: ");
		ret = scanf("%d",&(*choice));

	}while (!ret || ((*choice-1) < left || (*choice-1) > right));

	//decremento di un'unità perchè le macro partono da 0
	*choice = *choice - 1;
}


/*
	Leggo dal file menu.txt e aggiorno la posizione del cursore per permettere all'utente di eseguire una scelta
*/

void draw_menu(int * choice){

	*choice = LOGIN;  //di default si parte con LOGIN
	
	//disegno menu
	draw_from_file(path_template_menu);

	//leggo input
	read_stdinput(&(*choice),LOGIN,EXIT);
	
	//debug	
	//printf("\nScelta : %d\n", (*choice-1));
}

/*
	Legge da file il template da renderizzare sullo schermo
*/
void draw_from_file (char * source){

	/*FILE	* fp;
	int MAXLINE = 1000;
	fp = fopen(source,"r");

	if(!fp){
		//errore grave, manca il file sorgente per il template
		logger(unable_draw);
		exit(-1);
	}

	//pulisco lo schermo
	//system("clear");

	char buffer[MAXLINE];
	//Leggo fino alla EOF
	while (fgets(buffer, MAXLINE, fp))
		printf("%s",buffer);	 */
}

/*Printa a video il menù per la selezione della categoria*/

int draw_items_menu(int * max_choice,int num_val, char ** vs, int tipo){

	int i = 1; //elimino la risposta del server
	int choice;

	/*system("clear");

	printf("***************************************************************\n");
	if(tipo == REQUEST_CATALOGO)
		printf("Seleziona categoria... \n");
	if(tipo == REQUEST_CATALOGO_ITEMS) 
		printf("Seleziona files... \n"); */

	if(tipo == REQUEST_CATALOGO){
		
		logger("Preparo menu categorie");

		compose_mess_to_log("Numero di valori da gestire: ",&num_val,INTEGER_TYPE);

		int k = 0;
		for(i;i<num_val;i++){

			//oltre alla stampa della categoria è necessario costruire un'array di strutture di tipo categoria per memorizzare
			//ID categoria (seguendo l'ordine col quale il server le restituisce)
			//Nome categoria

			compose_mess_to_log("Valore di K : ",&k,INTEGER_TYPE);

			int j=0;
			for(j ; j<2 ; j++){

				if(j == 0){
					//nome categoria
					bzero(elenco_categorie[k].descrizione,strlen(elenco_categorie[i+j].descrizione)+1);
					strcpy(elenco_categorie[k].descrizione,vs[i+j]);
				}

				if(j == 1){
					//id categoria
					elenco_categorie[k].id = atoi(vs[i+j]);
				}				
			}

			logger("Memorizzo categoria:");
			compose_mess_to_log("ID :",&elenco_categorie[k].id,INTEGER_TYPE);			
			compose_mess_to_log("Nome :",elenco_categorie[k].descrizione,STRING_TYPE);

//			printf("%d) %s\n",k+1,elenco_categorie[k].descrizione);
		
			i+=1;
	
			compose_mess_to_log("Valore di i:",&i, INTEGER_TYPE);

			k++;

		}

	//	printf("%d) %s\n",k+1,"Esci");
	//	printf("***************************************************************\n");

		//leggo input da tastiera
		//printf("\nScegli: ");

		//compose_mess_to_log("Valore di k ",&k, INTEGER_TYPE);

		//read_stdinput(&choice,0,k);

		//compose_mess_to_log("Valore della choice ",&choice,INTEGER_TYPE);

		//choice+=1;
		*max_choice = k;
	}

	if(tipo == REQUEST_CATALOGO_ITEMS){

		logger("Preparo menu files");
	
		i=0;	
		int j = 1;	
		int index = 0;

		compose_mess_to_log("Num valori:",&num_val,INTEGER_TYPE);

		num_val -=1;
		 
		for(i=1;i<num_val;i++){

			//compose_mess_to_log("Valore di I : " , &i , INTEGER_TYPE);
			
			for(j=0;j < 3 ; j++){
				switch(j){
					case 0:
						strcpy(elenco_file[index].descrizione , vs[j+i]);
					break;
					
					case 1:
						elenco_file[index].id = atoi(vs[i+j]);
					break;
		
					case 2:
						elenco_file[index].dimensione = (long) atoi(vs[i+j]);
					break;
				}
			}

			i += j-1;
			//compose_mess_to_log("Nuovo valore di I : ", &i, INTEGER_TYPE);
			index++;

		}

		/*logger("Inizio printing files download");

		//printo a video i file selezionabili
		for(j=0;j<index;j++){
			char info[20];
			if(elenco_file[index].status == DOWNLOADED)
				strcpy(info,"Scaricato");
			if(elenco_file[index].status == IN_DOWNLOAD)
				strcpy(info,"In download");
			else
				strcpy(info,"Scarica");
	
			printf("%d) %s\t\t%li%s\t\t%s\n",j+1,elenco_file[j].descrizione,elenco_file[j].dimensione," Bytes",info);
		}

		printf("%d) %s\n",j+1,"Esci");
		printf("***************************************************************\n");

		//leggo input da tastiera
		//printf("\nScegli: ");
		read_stdinput(&choice,0,index); */

		*max_choice = index;
	}

	return choice;
}

/*
	Chiede credenziali d'accesso e le invia al server
*/
int login(int sd, char * username , char * password){
		
	/*int choice;
	char username[10], password[10];

	//system("clear");
	logger("Eseguo il login");

	draw_from_file(path_template_loggin);

	read_stdinput(&choice,0,1);

	switch(choice){
		case 0:
				//leggo da tastiera credenziali
				printf("Nome utente: ");	
				scanf("%s",username);

				//flush del buffer per la scanf, per evitare problemi nella lettura successiva
				scanf ("%*[^\n]");

				printf("Password: ");
				scanf("%s",password);

				//flush del buffer per la scanf, per evitare problemi nella lettura successiva
				scanf ("%*[^\n]"); */


				//concateno le due stringe
				char ** vs = (char**)malloc(NUM_STR * sizeof(char*)); //vettore di stinghe
				vs[0] = (char*)malloc(strlen(username)+1); //alloco la memoria per la stringa contenente l'username
				vs[1] = (char*)malloc(strlen(password)+1); //alloco la memoria per la stringa contenente la password

				strcpy(vs[0],username);
				strcpy(vs[1],password);

				//debug
				//printf("%s\n",vs[0]);
				//printf("%s\n",vs[1]);


				char tosend [MAX_DIM_USER_PLUS_PASSWORD];
				cat(vs, sep, tosend, NUM_STR);
				
				//compongo messaggio di richiesta autenticazione verso il server
				compose_protocol_mess(REQUEST_LOGIN,tosend,buff);

				//debug
				//compose_mess_to_log("Request per il login generata: ",buff,STRING_TYPE);
				//printf("%s\n",buff);
				
				//spedisco richiesta
				write_to_socket(sd,buff);
				free(vs);				
				

				//attendo risposta per memorizza il session_id assegnato dal server
				bzero(buff,MAX_MESS_SOCKET_LENGHT);
				read_from_socket(sd, buff,PROTOCOL);

				//Eseguo il parse del messaggio inviato per leggere il session_id, o eventuali errori.
				/*
					Il valore di ritorno della funzione parse può essere:
					0;<id_sessione>; (significa che il client è correttamente loggato al server e quindi può accedere ai servizi offerti del server)
					-1;<mess_errore>; (significa che c'è stato un errore, il client non è loggato e non può accedere ai servizi offerti dal server)
				*/
				
				//libero la memoria 
				//free(vs);				
				//debug
				//logger("Arrivo fino a qui");
				char ** vs1 = (char**)malloc(NUM_STR * sizeof(char*)); //vettore di stinghe
				int num_val = 0;  //numero di valori letti dal server (in questo caso un singolo valore) 
				int ret = parse(buff,*(&vs1),&num_val);
				//memorizzo la session_id (session_id == -1 -> client non loggato)

				//debug
				//compose_mess_to_log("Split 0:",vs1[0],STRING_TYPE);
				//compose_mess_to_log("Split 1:",vs1[1],STRING_TYPE);

				if(atoi(vs1[0]) == OK){
					session_id.id = atoi(vs1[1]);
					compose_mess_to_log("Sessione sul server: ",&session_id.id,INTEGER_TYPE);
				}
				 
				//logger("Preparo return");
				int ret_val  = OK;
				//system("clear");
				//flush del buffer
				//fflush(stdin);
				//autenticazione fallita
				if(session_id.id == -1 || session_id.id == 0 /*errore nella ATOI*/ || atoi(vs[0]) == ERROR /*Messaggio d'errore server*/){
					logger("AUTENTICAZIONE FALLITA, RIPROVA\n");
					compose_mess_to_log("Risposta del server: %s ",vs[1], STRING_TYPE);
					ret_val = ERROR;
				}
				else{
					logger("AUTENTICAZIONE AVVENUTA CON SUCCESSO");
					ret_val = OK;
				}

				//logger("Prima della return");
				free(vs1);
				return ret_val;

				//printf("Premi invio per tornare al menu principale ...\n");
				//getchar();

				//sleep(2);

				//libero la memoria
				//break;

		//default:
			//uscita, si torna al menù principale
			//break;
		
	//}
}

/*
	Richiede catalogo al server
*/
int  catalogo(int sd, char ** vs_ret, int * num_val_ret, int alloc){

	//system("clear");
	logger("Devo richiedere il catalogo");

	//compongo messaggio da spedire al server per richiedere il catalogo.
	//Il messaggio è nella forma: "200;<id_sessione>;"

	logger("Compongo richiesta catalogo e invio messaggio al server");

	//logger("ARRIVO");
	//trasformo il session_id assegnato dal server in stringa
	char tosend[30];
	sprintf(tosend,"%d",session_id.id);
	//compongo messaggio 200;<id_sessione>;
	compose_protocol_mess(REQUEST_CATALOGO,tosend,buff);	
	//scrivo sul socket
	write_to_socket(sd,buff);

	//attendo risposta dal server
	bzero(buff,MAX_MESS_SOCKET_LENGHT);
	read_from_socket(sd,buff,PROTOCOL);

	logger("Decodifico risposta del server");

	logger("Alloco memoria necessaria vs_new");
	char ** vs = (char**)malloc(MAX_CATEGORIE * sizeof(char*)); //vettore di stinghe
	compose_mess_to_log("Indirizzo vs_new:",&vs,INTEGER_TYPE);
	logger("Fine allocazione vs_new");


	//decodifico la risposta del server
	int num_val = 0;  //numero di valori letti dal server (in questo caso un singolo valore) 
	int ret = parse(buff,*(&vs),&num_val);
	
	logger("Preparo menu utente per selezionare categoria di files");

	int ret_val = OK;

	if(num_val && atoi(vs[0]) == OK){
		int choice = 0;
		int max_choice = 0;

		ret_val = OK;	

		choice = draw_items_menu(&max_choice,num_val,vs,REQUEST_CATALOGO);

		compose_mess_to_log("Max choice :",&max_choice,INTEGER_TYPE);	
		compose_mess_to_log("ALLOC:",&alloc,INTEGER_TYPE);

		int k;
		for(k=0;k<max_choice;k++){

			if(!vs_ret[k]){
				logger("Allocazione memoria");
				vs_ret[k] = (char*)malloc(sizeof(elenco_categorie[k].descrizione));
			}

			strcpy(vs_ret[k],elenco_categorie[k].descrizione);	
		}
		
		
		//int enter = 0;

		/*do{
			logger("Chiamata per draw categorie");
			
			choice = draw_items_menu(&max_choice,num_val,vs,REQUEST_CATALOGO);
			
			if(choice != max_choice){

				logger("Alloco memoria necessaria info_categoria");
				char **info_categoria = /*(char**)*///malloc(2 * sizeof(char*)); //vettore di stinghe per memorizzare id categoria e id di sessione
		/*		compose_mess_to_log("Indirizzo info_categoria:",&info_categoria,INTEGER_TYPE);
				logger("Fine allocazione info_catagoria");

				//Devo costruire la richiesta 
				char id_categoria[6];
				//trasformo in stringa id categoria
				sprintf(id_categoria,"%d",elenco_categorie[choice].id-1);
				char id_session[6];
				sprintf(id_session,"%d",session_id.id);

				compose_mess_to_log("Id categoria: ",id_categoria,STRING_TYPE);
				compose_mess_to_log("Id sessione: ",id_session,STRING_TYPE);
			
				//alloca la memoria solo al primo giro, dopo ri-inizializzo stringhe
			//	if(!enter){
					//debug
					logger("Allocazione per la prima volta delle stringhe necessarie a memorizza id sessione e categoria");
					info_categoria[0] = (char*)malloc(strlen(id_session)+1); 
					info_categoria[1] = (char*)malloc(strlen(id_categoria)+1); 
			//	}
				/*else{
					//
					logger("Cancellazione vecchi valori id sessione e categoria"); 
					bzero(info_categoria[0],strlen(info_categoria[0])+1);
					bzero(info_categoria[1],strlen(info_categoria[1])+1);
				}*/
	
				//copio valori
			/*	logger("copio valori id categoria e id sessione");
				strcpy(info_categoria[0],id_session);
				strcpy(info_categoria[1],id_categoria);
	
				char tosend [MAX_DIM_USER_PLUS_PASSWORD];
				cat(info_categoria, sep, tosend, 2);

					
				//compongo messaggio di richiesta autenticazione verso il server
				compose_protocol_mess(REQUEST_CATALOGO_ITEMS,tosend,buff); 

				//invio richiesta 
				write_to_socket(sd,buff);

				//attendo risposta del server
				bzero(buff,MAX_MESS_SOCKET_LENGHT);
				read_from_socket(sd,buff,PROTOCOL); 

				//Preparare menu dei files per categoria.
				manage_files(buff,sd,info_categoria[1],elenco_categorie[choice].descrizione);

				free(info_categoria);
				
				//enter  = 1;
			}
		}while(choice != max_choice); */
	}
	else{	

		ret_val = ERROR;

		//nessun catalogo sul server (possibili cause potrebbero essere:
		//Utente non loggato
		/*printf("***************************************************************\n");
		printf("Nessun catalogo restituito dal server\n");
		printf("Premi invio per tornare al menù principale ... \n");
		printf("***************************************************************\n");
		fflush(stdin);
		getchar(); */

	}	

	//eseguo copia dei valori dei due vettori di stringhe 
	
	//memorizzo numero delle categorie restituite
	*num_val_ret = (num_val-1)/2;

	logger("Fine Catalogo dealloco memoria");
	free(vs);	
	logger("Dealloco puntatore alla testa del vettore con tutte le categorie lette dal server");	

	return ret_val;
}

/*
	Richiede tutti i files per una determinata categoria
	* sd -> socket descriptor
	* id_cat -> id_categoria
*/
int files(int sd, int id_cat, char ** vs_ret, int * num_val_ret, int alloc){

	int ret_val = ERROR;

	//TODO
	compose_mess_to_log("Devo richiedere i file per categoria : ", &id_cat, INTEGER_TYPE);


	logger("Alloco memoria necessaria info_categoria");
	char ** info_categoria = (char**)malloc(2 * sizeof(char*)); //vettore di stinghe per memorizzare id categoria e id di sessione
	compose_mess_to_log("Indirizzo info_categoria:",&info_categoria,INTEGER_TYPE);
	logger("Fine allocazione info_catagoria");

	//Devo costruire la richiesta 
	char id_categoria[6];
	//trasformo in stringa id categoria
	sprintf(id_categoria,"%d",id_cat);
	char id_session[6];
	sprintf(id_session,"%d",session_id.id);

	compose_mess_to_log("Id categoria: ",id_categoria,STRING_TYPE);
	compose_mess_to_log("Id sessione: ",id_session,STRING_TYPE);
			
	logger("Allocazione per la prima volta delle stringhe necessarie a memorizza id sessione e categoria");
	info_categoria[0] = (char*)malloc(strlen(id_session)+1); 
	info_categoria[1] = (char*)malloc(strlen(id_categoria)+1); 
	
	//copio valori
	logger("copio valori id categoria e id sessione");
	strcpy(info_categoria[0],id_session);
	strcpy(info_categoria[1],id_categoria);
	
	char tosend [MAX_DIM_USER_PLUS_PASSWORD];
	cat(info_categoria, sep, tosend, 2);

					
	//compongo messaggio di richiesta autenticazione verso il server
	compose_protocol_mess(REQUEST_CATALOGO_ITEMS,tosend,buff); 

	//invio richiesta 
	write_to_socket(sd,buff);

	//attendo risposta del server
	bzero(buff,MAX_MESS_SOCKET_LENGHT);
	read_from_socket(sd,buff,PROTOCOL);

	char ** vs_files = (char**)malloc(MAX_FILES * sizeof(char*)); //vettore di stinghe 
	int num_val = 0;  //numero di valori letti dal server (in questo caso un singolo valore) 
	int ret = parse(buff,*(&vs_files),&num_val);

	int choice, max_choice;
	
	logger("Decodifico risposta del server e leggo tutti i nomi dei file restituiti per categoria");

	if(num_val && atoi(vs_files[0]) == OK){
	
		ret_val = OK;		

		logger("Copia fisica dei nomi dei files");

		compose_mess_to_log("Numero di valori : ", &num_val, INTEGER_TYPE);

		choice = draw_items_menu(&max_choice,num_val,vs_files,REQUEST_CATALOGO_ITEMS);
	}
 

	//Preparare menu dei files per categoria.
	//manage_files(buff,sd,info_categoria[1],elenco_categorie[choice].descrizione);

	free(info_categoria); 
	free(vs_files);

	*num_val_ret = max_choice;
	return ret_val;
}

/*
	Gestisce menu file
*/

void manage_files(int sd, int choice, char * id_categoria, char * id_file, int pipe_fd){

	char buffer_notifica[50];
	sprintf(buffer_notifica,"%s%s","IMPOSSIBILE EFFETTUARE DOWNLOAD DEL FILE ... ",elenco_file[choice].descrizione);
	
	char * mess = buff;
	
	char ** vs_files = (char**)malloc(MAX_FILES * sizeof(char*)); //vettore di stinghe 
	char ** info_request_file = (char**)malloc(3 * sizeof(char*)); //memorizzo risposta server nel vettore di stringhe per preparare download file
	int num_val = 0;  //numero di valori letti dal server (in questo caso un singolo valore);

	char id_session[6];
	sprintf(id_session,"%d",session_id.id);

	compose_mess_to_log("Id file: ",id_file,STRING_TYPE);
	compose_mess_to_log("Id sessione: ",id_session,STRING_TYPE);
	compose_mess_to_log("Id categoria: ",id_categoria,STRING_TYPE);
	
	logger("Allocazione per la prima volta delle stringhe necessarie a memorizza id sessione e id file");
	info_request_file[0] = (char*)malloc(strlen(id_session)+1); 
	info_request_file[1] = (char*)malloc(strlen(id_categoria)+1); 						
	info_request_file[2] = (char*)malloc(strlen(id_file)+1); 
					
	//copio valori
	logger("copio valori id categoria e id sessione");
	strcpy(info_request_file[0],id_session);
	strcpy(info_request_file[1],id_categoria);
	strcpy(info_request_file[2],id_file);

	logger("Richiedo contenuto al server");
	
	char tosend [MAX_DIM_USER_PLUS_PASSWORD];
	cat(info_request_file, sep, tosend, 3);
				
	//compongo messaggio di richiesta file da spedire al server
	compose_protocol_mess(REQUEST_FILE,tosend,buff); 

	//invio richiesta 
	write_to_socket(sd,buff);

	//leggo dal socket response al download (mi aspetto una richiesta per un nuovo canale)
	bzero(buff,strlen(buff)+1);
	read_from_socket(sd,buff,PROTOCOL);

	//split della risposta del server (mi aspetto una nuova richiesta di connessione
	logger("Split risposta server per decodificare porta alla quale connettermi");
	char ** response_to_download = (char**)malloc(2 * sizeof(char*));
	int num_val_download = 0;
	int ret = parse(mess,&(*response_to_download),&num_val_download);

	if(num_val_download == 2){
		if(atoi(response_to_download[0]) == REQUEST_CHANNEL){
			//devo mettere in ascolto sulla nuova porta
			unsigned int port = atoi(response_to_download[1]);
							
			compose_mess_to_log("Porta alla quale connettermi: ",&port,INTEGER_TYPE);

			logger("Creo nuovo processo per il download");
			//Creo un nuovo processo per effettuare il download
	
			//dimensione file
			long dimensione_file_bytes = elenco_file[choice].dimensione;	

			int pid_new_download = fork();

			if(pid_new_download == -1){

				//crea notifica

				draw_dialog_panel(ERROR,"Notifica","", buffer_notifica, extern_use);

				logger("IMPOSSIBILE EFFETTUARE DOWNLOAD, ERRORE NELLA CREAZIONE DEL PROCESSO");
			}
					
			if(pid_new_download == 0){
		
				logger("Processo di download inizio computazione per avviare download");
				//In ascolto sulla nuova porta
				int new_socket_download = create_client(ip_server_connect,port);

				FILE * fp = NULL;
				char salva_file_in_path[50];
				strcpy(salva_file_in_path,path_to_save);
				strcat(salva_file_in_path,elenco_file[choice].descrizione);

				compose_mess_to_log("Path nel quale salvare file:",salva_file_in_path,STRING_TYPE);

				//cerco di aprire il file il lettura per capire se esiste già o meno
				fp = fopen(salva_file_in_path,"rb");

				if(fp){
					logger("Rinomino File perchè già scaricato");
					int num_same_download = count_num_download(elenco_file[choice].descrizione);
					char tmp_num[10];
					sprintf(tmp_num,"%c%d%c",'(',num_same_download,')');

					char buf_name[200];
					strcpy(buf_name,elenco_file[choice].descrizione);
					bzero(elenco_file[choice].descrizione,strlen(elenco_file[choice].descrizione)+1);
					strcpy(elenco_file[choice].descrizione,tmp_num);
					strcat(elenco_file[choice].descrizione,buf_name);
		
					compose_mess_to_log("RINOMINATO IN: ",elenco_file[choice].descrizione,STRING_TYPE);

					//se il file esiste di già aggiungo un'estensione con (2) per evitare di sovrascrivere il precedente download
					bzero(salva_file_in_path,strlen(salva_file_in_path)+1);
					strcpy(salva_file_in_path,path_to_save);
					strcat(salva_file_in_path,elenco_file[choice].descrizione);

					compose_mess_to_log("RINOMINATO IN: ",salva_file_in_path,STRING_TYPE);
					//fp = fopen(buf_to_rename,"wb");								
						
				}

				//chiudo il file precedentemente aperto in lettura							
				//fclose(fp);
				//else
				fp = fopen(salva_file_in_path,"wb");
								
								
				if(!fp){
					//system("clear");
					logger("IMPOSSIBILE EFFETTUARE DOWNLOAD, IMPOSSIBILE CREARE FILE");

					//crea notifica
					draw_dialog_panel(ERROR,"Notifica","", buffer_notifica, extern_use);

					/*printf("***************************************************************\n");
					printf("IMPOSSIBILE EFFETTUARE DOWNLOAD DEL FILE ... %s\n",elenco_file[choice].descrizione);
					printf("Premi invio per tornare indietro ... \n");
					printf("***************************************************************\n");
					fflush(stdin);
					getchar(); */
				}
				else{
					compose_mess_to_log("Inizio download file: ",elenco_file[choice].descrizione,STRING_TYPE);	
					compose_mess_to_log("Dimensione file: ",&elenco_file[choice].dimensione,INTEGER_TYPE);	
					elenco_file[choice].status = IN_DOWNLOAD;
	
					//scrivo su db nuovo download
					execute_insert_new_item(elenco_file[choice].descrizione, status_in_download ,path_to_save);
					//int flag_end_download = 1;
					//inserisco nella PILA un nuovo download
					//Testa dello stack dei download
					struct download_item * items = NULL;
					items = write_new_download(items,elenco_file[choice].descrizione, IN_DOWNLOAD ,path_to_save);

					compose_mess_to_log("APPENA INSERITO:",items->name,STRING_TYPE);									
					struct download_item * tmp = items;

					while(tmp){
						compose_mess_to_log("VALORE NEW: ",tmp->name,STRING_TYPE);
						tmp = tmp->next;
					}
									
					//current = items;
					int ret_socket = 0;
					long bytes = 0;
					long count_b = 0;
					//chiudo pipe in lettura
					//close(fd_pipe[0]);

					struct download_refresh pass;

					while((count_b < dimensione_file_bytes)){

						bzero(buff,MAX_MESS_SOCKET_LENGHT);
						bytes = read_from_socket(new_socket_download,buff,BYTE_FILE); 
						fwrite(buff,1,bytes,fp);
						count_b += bytes;
						
						//scrivo percentuale downlaod per il file 
						double perc = (double)((double)count_b/(double)elenco_file[choice].dimensione);
						int perc_d = perc * 100;

						//refresh percentuale download effettuato
						//refresh_perc_download(perc_bar, perc);

						/* Aggiunge un timer callback per aggiornare il value della progress bar */
						pass.perc = perc;

						//int timer = gtk_timeout_add (100, refresh_perc_download, pass);
						//aggiorno percentuale download
						items->percentuale = perc_d;

						logger("Scrivo sulla PIPE percentuale download");
						compose_mess_to_log("-->" , &(pass.perc) , FLOAT_TYPE);

						//scrivo puntatore corrente
						write(pipe_fd,&pass,sizeof(struct download_refresh));

					} 

					compose_mess_to_log("Ho scritto bytes : ", &bytes, INTEGER_TYPE);

					//free(pass);

					//chiudo pipe
					//close(w_pipe);
					close(pipe_fd);

					logger("Fuori dal ciclo di download");
					//aggiorno stato download, in scaricato
					items->status = DOWNLOADED;

					//scrivo su db fine download
					update_perc(elenco_file[choice].descrizione,items->percentuale);

					free(items);

					//mistero della vita.. ci vuole un secondo di sleep prima di riutilizzare il canale socket
					sleep(1);

					/*SINCRONIZZAZIONE CHIUSURA CLIENT SERVER*/
					logger("Inizio Sincronizzazione per Chiusura");
					//leggo response del server quando download termina
					bzero(mess,MAX_MESS_SOCKET_LENGHT);	
					//logger("Inizio Sincronizzazione per Chiusura 2");
					read_from_socket(new_socket_download,mess,PROTOCOL);

					write_to_socket(new_socket_download,"OK");
							
					logger("Chiudo canale socket e file aperto per il download");
					fclose(fp);
					close(new_socket_download);
	
					/*******************************************/

					//incremento contattore download e setto file come scricato
					elenco_file[choice].status = DOWNLOADED;
					download_files++;		
					compose_mess_to_log("Fine download file: ",elenco_file[choice].descrizione,STRING_TYPE);


					//scrivo su db la fine del download
					//execute_insert_new_item(elenco_file[choice].descrizione, status_downloaded , path_to_save);
					}
						//elenco_file[choice].status = NO_DOWNLOADED;
			
						exit(0);
				}

				else{
								

								//stampo a video messaggio per informare che sta scaricando il file
						/*		system("clear");
								printf("***************************************************************\n");
								printf("Download avviato, file ... %s\n",elenco_file[choice].descrizione);
								printf("Premi invio per tornare indietro ... \n");
								printf("***************************************************************\n");
								fflush(stdin);
								getchar(); */

		
								//processo padre (attesa non bloccante in attesa che termini il processo figlio di download)
								signal(SIGCHLD,_manage_exit_child);   //intercetto la terminazione del figlio e evito di creare ZOMBIE

								//char ** end_download = (char**)malloc(2 * sizeof(char*)); //fine download
								//int num_val_download = 0;  //numero di valori letti dal server (in questo caso un singolo valore) 
								//int ret = parse(mess,&(*end_download),&num_val_download); 
				
								//if(ret == 2){
									//fine del figlio
									//int status;
									//wait(&status);
								//}
							}
						}
						else{

							//crea notifica
							draw_dialog_panel(ERROR,"Notifica","", buffer_notifica, extern_use);

							/*system("clear");
							logger("IMPOSSIBILE EFFETTUARE DOWNLOAD");
							printf("***************************************************************\n");
							printf("IMPOSSIBILE EFFETTUARE DOWNLOAD DEL FILE ... %s\n",elenco_file[choice].descrizione);
							printf("RISPOSTA DEL SERVER ... %s\n",response_to_download[1]);
							printf("Premi invio per tornare indietro ... \n");
							printf("***************************************************************\n");
							fflush(stdin);
							getchar();							 */
						}
					}
					else{


						//crea notifica
						draw_dialog_panel(ERROR,"Notifica","", buffer_notifica, extern_use);

					/*	system("clear");
						logger("IMPOSSIBILE EFFETTUARE DOWNLOAD, RISPOSTA SERVER NON CORRETTA");
						printf("***************************************************************\n");
						printf();
						printf("Premi invio per tornare indietro ... \n");
						printf("***************************************************************\n");
						fflush(stdin);
						getchar(); */
					}

					free(response_to_download);
				//}
			//}while(choice != max_choice);
	//}

	free(vs_files);
	free(info_request_file);
}


/*
	Visualizza a video tutti i file scaricati
*/
struct download_item *  scaricati(void){


	logger("Devo visualizzare i file scaricati");

	struct download_item * tmp = NULL;
	int flag_enter = 0;

	logger("USO CLONE LISTA TMP");
	
	//ricarico da db files scaricati per evitare che mi sia perso qualcosa
	tmp = (struct download_item *) execute_select_items(tmp);
	//non perdo la testa della lista
	//tmp = safe;

	//	system("clear");
	//int i = 0;
/*
	system("clear");
	printf("*************************************************************************************************************************************************************\n\n");

	if(!tmp){
		logger("Lista non caricata da db correttamente");
		printf("NESSUN FILE SCARICATO\n");
	}

	else{
		int i = 0;
		//struct download_item * tmp = items;
		//printf("\tNOME\t\tPERC\t\tPATH\t\tPRESENT\n\n");
		//int j = 0;
		FILE * fp;
		while(tmp){
				//if(tmp->status == DOWNLOADED){
					flag_enter = 1;
					char buf_tmp[60];
					strcpy(buf_tmp,tmp->path);
					//strcat(buf_tmp,tmp->name);
					compose_mess_to_log("PATH:",buf_tmp,STRING_TYPE);
					fp = fopen(buf_tmp,"rb");
					if(!fp){
						//no presente
						bzero(buf_tmp,strlen(buf_tmp)+1);
						strcpy(buf_tmp,"NO PRESENTE IN ../FILES");
					}
					else{
						//presente	
						bzero(buf_tmp,strlen(buf_tmp)+1);
						strcpy(buf_tmp,"PRESENTE IN ../FILES");
					}
					char str[20]; 
					if(tmp->percentuale == 100)	
						strcpy(str,"COMPLETATO");
					else
						strcpy(str,"INCOMPLETO");

					printf("%d) %-35s | %-35s | %-35s | %-35s |\n\n",i+1,tmp->name,str,tmp->path,buf_tmp);
					i++;
				//}
			fclose(fp);
			tmp = tmp->next;
			//compose_mess_to_log("J: ",&j,INTEGER_TYPE);
			//j++;
		}

		if(!flag_enter)
			printf("NESSUN FILE SCARICATO\n");
	}

	free(tmp);

	printf("*************************************************************************************************************************************************************\n\n");

	printf("Premi invio per tornare indietro ... \n");
	fflush(stdin);
	getchar();
	*/
}

/*
	Visualizza i file che si stanno scaricando	
*/
void download(void){

	/*char c = '0';
	struct download_item * tmp = NULL;

	//chiudo pipe in scrittura

	//pressione dell'invio
//	while(c != 13){

		//compose_mess_to_log("Carattere letto: ",&c,INTEGER_TYPE);

		tmp = NULL;

		close(fd_pipe[0]);
		logger("Attendo lettura sulla pipe");
		read(fd_pipe[1],tmp,sizeof(struct download_item *));
		close(fd_pipe[1]);

		logger("USO CLONE LISTA TMP");

	
		//ricarico da db files scaricati per evitare che mi sia perso qualcosa
		//tmp = (struct download_item *) execute_select_items(tmp);
		//	tmp = safe;

		int flag_enter=0;
	
		//system("clear");
		logger("Devo visualizzare file che sto scaricando");

		system("clear");
		printf("********************************************************************************************\n\n");

		if(!tmp){
			logger("Nessun download in esecuzione");
			printf("NESSUN FILE IN DOWNLOAD\n");
		}

		else{
			logger("Ho download da leggere");
			int i = 0;
			//struct download_item * tmp = items;
			//printf("\t\tNOME\t\tPERC\t\tPATH\n\n");
			while(tmp){
				//if(tmp->status == IN_DOWNLOAD){
					char str[20]; 
					if(tmp->percentuale == 100)	
						strcpy(str,"DOWNLOADED");
					else
						strcpy(str,"IN DOWNLOAD");

					printf("%d) %-5s | %-5s | %-5s\n\n",i+1,tmp->name,str,tmp->path);
					i++;
					flag_enter = 1;
				//}
			tmp = tmp->next;
			}
	
			if(!flag_enter)
				printf("NESSUN FILE IN DOWNLOAD\n");
		}

		free(tmp);

		printf("\n********************************************************************************************\n\n");
		printf("Premi invio per tornare indietro ... \n");
		fflush(stdin);
		getchar();
//	}*/
}

/*
	Creo LIFO DOWNLOAD (STACK)
*/
int num_items = 0;
struct download_item * write_new_download(struct download_item * inizio, char * name, int status, char * path){

	logger("Ho inserito un nuovo download nella pila");

	num_items ++;
	compose_mess_to_log("ITEMS: ",&num_items,INTEGER_TYPE);
	compose_mess_to_log("INSERISCO NUOVO DOWNLOAD: ", name, STRING_TYPE);


	struct download_item * p;
	p = (struct download_item *)malloc(sizeof(struct download_item));

	/*Memorizzo info download*/
 	strcpy(p->name,name);
	p->status = status;
	p->percentuale = 0;
	strcpy(p->path,path);
	strcat(p->path,name);

	/*Creo Struttura Stack*/
	p->next = inizio;
	
	/*Restituisco testa della lista*/
	return (p);
}

