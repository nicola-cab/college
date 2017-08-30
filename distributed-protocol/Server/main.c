#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h> 
#include <sys/wait.h> 
#include <time.h>

#include "op.h"

  
int main(void) {

	/* varibile utilizzata per identificare il processo */ 
   pid_t pid;   

	logger(startup_server);         

	/* creo il server */
	int sd = create_server(PORT,MAX_CALL);

	//loggata su LOG/server.log
	logger(start);

   

    /***************************************
    * ORA COMINCIA LA GESTIONE CONCORRENTE*
	 * CICLO INFINITO. 
    * *************************************/

    while(1) {
         
			int sd_client = create_client_channel(sd);
			//char buff[MAX_MESS_SOCKET_LENGHT];
        
        /*
			*  Se l'accept è andata a buon fine allora creo un nuovo processo per gestire la chiamata
			*/		 
        if ((pid = fork()) < 0) {
				//OCCHIO ALL'ERRORE NELLA FORK
				logger(unable_fork);
        		//perror(" fork error\n");
        		exit(-1);
        }
		  /*
			*	Tutto OK, gestisco la chiamata
			*/
        if (pid == 0) { 
				/* sono nel figlio */

				//chiudo il vecchio socket descriptor.
            _manage_close_socket(sd);

				//Loop fino alla richiesta di disconnessione da parte del client
				int ret;
				//buffer di supporto per evitare che il messaggio appena inviato venga rigirato come echo al server
				char old_mess[MAX_MESS_SOCKET_LENGHT] = "NULL";
				do{
	            /*Ricevo dati dal client*/
					read_from_socket(sd_client,buff);
					//Logica di riconoscimento dei messaggi inviati dal client

					compose_mess_to_log("Messaggio letto: ",buff,STRING_TYPE);
					compose_mess_to_log("Messaggio inviato precedentemente: ",old_mess,STRING_TYPE);

					//se il messaggio letto dal socket non è vuoto o non è un messaggio echo
					if(strcmp(buff,"") && strcmp(buff,old_mess)){
						//splitto messaggio ricevuto ed eseguo operazione richiesta
	   	 			ret =	 recognize_mess_and_response(buff,sd_client);

						//copio in old buff il messaggio da spedire
						bzero(old_mess,MAX_MESS_SOCKET_LENGHT);
						strcpy(old_mess,buff);
					}
					else{
						logger("Ho letto un messaggio vuoto e/o non corretto dal socket, non rispondo!!!");

						compose_mess_to_log("MESSAGGIO CHE MI HA SCATURITO:",buff,STRING_TYPE);

						clear_db_session_socket(sd_client,0);
						_manage_close_socket(sd_client);

						//chiudo la connessione col client (suppongo sia terminato inaspettatamente)
						exit(-1); //torno al padre con status -1
						//ret = REQUEST_CLOSE; //(inutile, exit già eseguita)
					}

					//scrivo la risposta del server sul socket
					if(ret == WRITE_SOCKET)
						//scrivo utilizzando il canale per questo client solo per alcuni tipi di richiesta
						write_to_socket(sd_client,buff,PROTOCOL,strlen(buff)+1);

				}while(ret != REQUEST_CLOSE);

				logger("FINE ELABORAZIONE REQUESTS CLIENT...PROCESSO CONCLUSO...USCITA");
        			
				/*Exit, con valore 0 per permettere al padre di chiudere il figlio correttamente*/
            exit(0);
      }
      else { 
				compose_mess_to_log("Gestione richiesta Client, Processo in esecuzione con PID: ",&pid,INTEGER_TYPE);
				/* sono nel padre */
				signal(SIGCHLD,_manage_exit_child); 
				//compose_mess_to_log("In waiting per il PID:",&pid_waiting,INTEGER_TYPE);
      }
  }

  logger(end);
  exit(0);
}
