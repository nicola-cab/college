#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>

#include "gui.h"
#include "message.h"
#include "op.h"
//#include "logger.h"

#define MAX_WIDGETS_PASS 5


#define LOGIN 0
#define CATALOGO 1
#define SCARICATI 2
#define LOGOUT 3
#define FILES 4

#define TRUE_VALUE 1
#define FALSE_VALUE  0


struct download_refresh{
	GtkProgressBar * bar;
	float perc;
	int pipe_fd;
};

int flag_empty_space = FALSE_VALUE;

/*
	Alloca la memoria e costruisce nuovo record da passare come parametro alla routine di gestione per un certo evento
	
	char * -> nome dell'operazione da eseguire
	int -> codice operazione da eseguire
	GtkWidget * -> array di widget
	char ** -> vettore di stringhe 
	int -> numero di elementi del vettore di stringhe
*/

struct pass_info{
	char info_name[30];
	int  info_op;
	GtkWidget * widgets[MAX_WIDGETS_PASS]; //numero massimo di widget che si possono passare 
	char ** elements; //vettore di stringhe con tutti gli items da scaricare
	int num_elements; //numero di elementi
	char ** elements_files; //numero elementi file
	int num_elements_files; //numero dei files
	int pos_file_download_vect; //posizione del file da scaricarica nel vettore di files 
	int id_category; //id categoria da richiedere nel download di un file
};


/*Puntatori alle istanze della struttura pass_info che utilizzo*/

struct pass_info * dp_login = NULL;
struct pass_info * dp_catalogo = NULL;
struct pass_info * dp_scaricati = NULL;
struct pass_info * info_login = NULL;
struct pass_info * tdp = NULL;
struct pass_info * dp_catalogo_to_pass = NULL;
struct pass_info * dp_files = NULL;

char ** vs_files_gui = NULL;
char ** vs_categorie_gui = NULL;


int enter_download_view = FALSE_VALUE;
int enter_catalogo = FALSE_VALUE;
int enter_scaricati = FALSE_VALUE;
int enter_files = FALSE_VALUE;
int enter_download = FALSE_VALUE;

/*Routines utilità*/
void do_operation_gui(GtkWidget * widget, gpointer data);
void do_operation_protocol(GtkWidget * widget, gpointer data);
void delete_event(GtkWidget * widget, gpointer data);
void remove_prev_empty_space(GtkWidget *, int);

//routine invocata dal thread per il refresh
void * pthread_function( void * arguments);


/*Codice GTK*/
static GtkWidget * make_buttons(void);
static GtkWidget * make_empty_space(int);
static GtkWidget * make_login_panel(void);
static GtkWidget * make_catalogo_panel(void);
static GtkWidget * make_scaricati_panel(void);
static GtkWidget * make_download_panel(void);
static GtkWidget * make_window_download();
gint refresh_perc_download(gpointer);
static GtkProgressBar * add_download(char *);

static GtkWidget * xpm_label_box(gchar * xpm_filename,gchar *label_text);
GdkPixbuf * create_pixbuf(const gchar * filename);

const char * login_gui = "login";
const char * catalogo_gui = "catalogo";
const char * scaricati_gui = "scaricati";
const char * logout = "logout";
const char * submit = "submit";

const char * dialog_login_gui = "dialog_login_gui";
const char * dialog_catalogo_gui = "dialog_catalogo_gui";
const char * download_window_type = "download_window_type";

int socket_descriptor = -1;
GtkWidget * main_window;
GtkWidget * download_window;
GtkWidget * gtklist; //lista download in esecuzione
GtkWidget * list_items;
int create_new_list_downlod = 0;

//semaforo utilizzato nella fase di download
sem_t sem;
//semaforo binario (troppo restrittivo)
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


GtkWidget * global_win;
int enter_d_win = 0;
int hidden = 0;


/*****************************************************************************************************/
/*****************************************************************************************************
															GTK UTILITIES
******************************************************************************************************/
/*****************************************************************************************************/


/*
	Funzione di inizializzazione della gtk
*/
void build_gui(int argc, char ** argv, int sd){

	logger("Creazione intefaccia grafica");

	//inizializzo semaforo
	sem_init(&sem, 0 , 1);

	//salvo il socket descriptor
	socket_descriptor = sd;

	GtkWidget *window;
	GtkWidget *buttons;
	GtkWidget *hbox_window;
	GtkWidget * empty_space;
	
	//inizializzo gtk
  	gtk_init(&argc, &argv);

	//creo nuova window
  	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  	gtk_window_set_title(GTK_WINDOW(window), "Client");
  	gtk_window_set_default_size(GTK_WINDOW(window), 650, 300);
  	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  	gtk_window_set_icon(GTK_WINDOW(window), create_pixbuf("../TEMPLATE/icone/onApp.png"));
	gtk_window_set_resizable(GTK_WINDOW(window),TRUE);

	//hbox principale container nella window
	hbox_window = gtk_hbox_new(FALSE, 2);
	main_window = hbox_window;

	/*****************set del container principale************/
	gtk_container_add(GTK_CONTAINER(window), hbox_window);
	/*********************************************************/
	
	//carico immagine logo client
	empty_space = make_empty_space(-1);
	//empty_space = gtk_hbox_new(TRUE,2);
	//creo menu bottoni
	buttons = make_buttons();
	
	//posiziono i vari box sul container principale della window
	gtk_box_pack_start(GTK_BOX(hbox_window),buttons,FALSE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(hbox_window),empty_space,TRUE,FALSE,0);

	//finestra non ridimensionabile
	//gtk_window_set_resizable(GTK_WINDOW(window),FALSE);

	gtk_widget_show_all(window);

	//destroy window
	g_signal_connect(window, "destroy",
		G_CALLBACK (delete_event), NULL);

	//ACCROCCHIO GLOBALE!!!!

	//lancio gestore gtk in attea di eventi
	/* main loop */
	gdk_threads_enter();
	gtk_main ();
	gdk_threads_leave();

	//dealloco semaforo dopo che tutti i thread sono "exited" e l'applicazione termina
	sem_destroy(&sem);

}

/*
	Costuisco elenco di bottoni
*/
static GtkWidget * make_buttons(void){

	GtkWidget *vbox_button;
	GtkWidget *hbox_button;

	/*Widget bottone login*/
	GtkWidget *button_login;
	GtkWidget *box_login;

	/*Widget bottone catalogo*/
	GtkWidget *button_catalogo;
   GtkWidget *box_catalogo;

	/*Widget bottone scaricati*/
	GtkWidget *button_scaricati;
   GtkWidget *box_scaricati;

	/*Widget bottone disconessione*/
	GtkWidget *button_logout;
   GtkWidget *box_logout;

	//vbox dei bottoni
	vbox_button = gtk_vbox_new(TRUE, 1);

	//hbox per dimensionare i 4 bottoni (login, catalogo, scaricati, logout)
	hbox_button = gtk_hbox_new(FALSE, 4);

	/* Create a new button */
   button_login = gtk_button_new ();
	button_catalogo = gtk_button_new ();
	button_scaricati = gtk_button_new ();
	button_logout = gtk_button_new ();

	/* Unisco al bottone l'immagine LOGIN*/
	box_login = xpm_label_box ("../TEMPLATE/icone/login.png", "login");
	box_catalogo =	xpm_label_box ("../TEMPLATE/icone/catalogo.svg", "catalogo");
	box_scaricati = xpm_label_box ("../TEMPLATE/icone/downloaded.svg", "scaricati");
	box_logout =  xpm_label_box ("../TEMPLATE/icone/stop.png", "logout");

	/* Inserisco nei bottoni il box creato in modo tale che gtk riesca a capire che si tratta di bottoni*/
   gtk_container_add (GTK_CONTAINER (button_login), box_login);
	gtk_container_add (GTK_CONTAINER (button_catalogo), box_catalogo);
	gtk_container_add (GTK_CONTAINER (button_scaricati), box_scaricati);
	gtk_container_add (GTK_CONTAINER (button_logout), box_logout);

	/*Bottoni in verticale*/
	gtk_box_pack_start(GTK_BOX(vbox_button), button_login, FALSE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox_button), button_catalogo, FALSE, TRUE, 0);
  	gtk_box_pack_start(GTK_BOX(vbox_button), button_scaricati, FALSE, TRUE, 0);
  	gtk_box_pack_start(GTK_BOX(vbox_button), button_logout, FALSE, TRUE, 0);

	/*****Posiziono il box dei bottoni in orizzontale verticalmente*****/	
	gtk_box_pack_start(GTK_BOX(hbox_button),vbox_button,FALSE,FALSE,0);
	/******************************************************************/

	//eventi

	//click login

	if(!dp_login)
		dp_login = (struct pass_info *) malloc(sizeof(struct pass_info));
	if(!dp_catalogo)
		dp_catalogo = (struct pass_info *) malloc(sizeof(struct pass_info));
	if(!dp_scaricati)
		dp_scaricati = (struct pass_info *) malloc(sizeof(struct pass_info));
	if(!dp_catalogo_to_pass)
		dp_catalogo_to_pass = (struct pass_info *) malloc(sizeof(struct pass_info));		



	strcpy(dp_login->info_name, login_gui);
	dp_login->widgets[0]= main_window;
	dp_scaricati->widgets[0]= main_window;
	dp_catalogo->widgets[0]= main_window;

	dp_catalogo_to_pass->widgets[2] = main_window;

	
	g_signal_connect(button_login, "clicked", 
      G_CALLBACK(do_operation_gui), (gpointer) dp_login);

	//click scaricati
	strcpy(dp_scaricati->info_name, scaricati_gui);
 	g_signal_connect(button_scaricati, "clicked", 
      G_CALLBACK(do_operation_gui), (gpointer) dp_scaricati);

	//click per fare download
	strcpy(dp_catalogo->info_name, catalogo_gui);
 	g_signal_connect(button_catalogo, "clicked", 
      G_CALLBACK(do_operation_gui),(gpointer) dp_catalogo);

	//click logout
 	g_signal_connect(button_logout, "clicked", 
      G_CALLBACK(delete_event), (gpointer) logout);

	//setto inattivi i bottoni per richiedere il catalogo e per effettuare il logout (Non è possibile utilizzarli se prima non ci logga).

	//gtk_widget_set_sensitive(button_catalogo, FALSE);
	//gtk_widget_set_sensitive(button_logout, FALSE);

	
	return hbox_button;
}


/*
	Gestisco lo spazio a destra dei bottoni a seconda del tipo di operazione richiesta.
	Eseguo di fatto uno switch in base al tipo di operazione, organizzando lo spazio libero.
*/
static GtkWidget * make_empty_space(int type){

	logger("Costruisco empty space a dx");

	GtkWidget * box_empty = NULL;
 
	switch(type){
		case LOGIN:

			logger("Costruisco pannello Login");

			//printf("Login costruisco panel\n");
			//costruisco menù utente per leggere username e password
			box_empty = make_login_panel();			
			return box_empty;
		break;

		case CATALOGO:

			logger("Costruisco pannello catalogo");

			box_empty = make_catalogo_panel();

			if(!box_empty)
				box_empty = xpm_label_box ("../TEMPLATE/icone/onApp.png", "Client");

			return box_empty;
		break;

		case SCARICATI:

			logger("Costruisco pannello scaricati");

			box_empty = make_scaricati_panel();

			if(!box_empty){
				box_empty = xpm_label_box ("../TEMPLATE/icone/onApp.png", "Client");
				flag_empty_space = TRUE_VALUE;
			}

			return box_empty;
		break;

		case FILES:

			logger("Costruisco pannello files da scaricare");

			box_empty = make_download_panel();

			if(!box_empty){
				box_empty = xpm_label_box ("../TEMPLATE/icone/onApp.png", "Client");
				flag_empty_space = TRUE_VALUE;
			}

			return box_empty;			
	
		default:	

			logger("Pannello di default con icona app");

			//mostra messaggio di default
			box_empty = xpm_label_box ("../TEMPLATE/icone/onApp.png", "Client");
			return box_empty;
		break;
	}

	return box_empty;
}

/*
	Costruisco Menu per costruire Panello Login
*/
static GtkWidget * make_login_panel(void){

	//in pilo dentro 
	GtkWidget * vbox_form;
	GtkWidget * label_username;
	GtkWidget * label_password;
	GtkWidget * entry_username;
	GtkWidget * entry_password;
	GtkWidget * table;
	//GtkWidget * title;
	GtkWidget * box_submit;
	GtkWidget * submit;
	//GtkWidget * reset;

	// Creiamo una un layout di tipo tabella, e gli passeremo come parametri il numero di righe, il numero di colonne e se vogliamo una disposizione dei 		nostri oggetti sulla tabella omogenea gli passeremo la voce TRUE.
	table = gtk_table_new (7, 2, FALSE);
	//title = gtk_label_new ("LOGIN");


	label_username = gtk_label_new("USERNAME:");
	label_password = gtk_label_new("PASSWORD:");
	
	entry_username = gtk_entry_new();
	entry_password = gtk_entry_new();
	gtk_entry_set_visibility(GTK_ENTRY(entry_password),FALSE);

	submit = gtk_button_new();
	box_submit = xpm_label_box ("../TEMPLATE/icone/ok.png", "INVIA");
   gtk_container_add (GTK_CONTAINER (submit), box_submit);

	//submit = gtk_button_new_with_label ("OK");
	//reset = gtk_button_new_with_label ("");

	/* Aggiungiamo i widget alla tabella. Passandogli le coordinate, cioè mettendo i nostri oggetti widget nelle celle della nostra tabella passando a 		gtk_table_attach la tabella nella quale vogliamo inserire l'oggetto, le coordinate sinistra, destra, alto e basso. Gli diciamo di espandere 			l'oggetto tanto grande quanto la cella per quanto riguarda l'asse x e per l'asse y ed infine il padding (lo spazio dal bordo della cella) per 			l'asse x e l'asse y. */

	//gtk_table_attach (GTK_TABLE (table), title, 0, 1, 0, 1, GTK_EXPAND, GTK_SHRINK, 0 ,0);

//	GTK_EXPAND, GTK_SHRINK,

	gtk_table_attach (GTK_TABLE (table), label_username, 0, 1, 0, 1, 0,0, 0 ,0);
	gtk_table_attach (GTK_TABLE (table), entry_username, 0, 1, 1, 2, 0,0, 0 ,0);
	gtk_table_attach (GTK_TABLE (table), label_password, 0, 1, 2, 3, 0,0, 0 ,0);
	gtk_table_attach (GTK_TABLE (table), entry_password, 0, 1, 3, 4, 0,0, 0 ,0);	
	gtk_table_attach (GTK_TABLE (table), submit, 0, 1, 5, 6, 0,0, 0 ,0);
	//gtk_table_attach (GTK_TABLE (table), reset, 0, 1, 3, 4, GTK_EXPAND, GTK_SHRINK, 0 ,0);

	// Settiamo lo spazio per riga e per colonna.
	gtk_table_set_row_spacings (GTK_TABLE (table), 0);
	gtk_table_set_col_spacings (GTK_TABLE (table), 0);


	vbox_form = gtk_vbox_new(TRUE, 0);	
	
	//non mi interessa attaccare dei gestori di eventi ai due text-box
	//gtk_signal_connect(GTK_OBJECT(entry_username), "activate", GTK_SIGNAL_FUNC(enter_callback_username), entry_username);
	//gtk_signal_connect(GTK_OBJECT(entry_password), "activate",GTK_SIGNAL_FUNC(enter_callback_password), entry_password);

	//printf("Sono qui\n");

	
	if(!info_login)	
		info_login = (struct pass_info *)malloc(sizeof(struct pass_info));

	info_login->info_op = LOGIN;
	info_login->widgets[0] = entry_username;
	info_login->widgets[1] = entry_password;  

	g_signal_connect(submit, "clicked", G_CALLBACK(do_operation_protocol),(gpointer) info_login);

	//g_signal_connect(submit, "clicked", G_CALLBACK(do_operation),(gpointer) "reset");
	gtk_box_pack_start (GTK_BOX (vbox_form), table, TRUE, FALSE, 0);

	return vbox_form;
}


/*
	Costruisco menu a tendina per selezionare tipo di categoria da richiedere al server
*/

static GtkWidget * make_catalogo_panel(void){

	int num_cat;

	int k = 0;	
	GtkWidget *combo;
	GtkWidget *combo_button;
	GtkWidget *box_combo_button;
	GtkWidget *box;
	GtkWidget * table;
	GList *list=NULL;

	int ret;
	
	if(!vs_categorie_gui){
		vs_categorie_gui = (char**)malloc(MAX_CATEGORIE * sizeof(char*)); //vettore di stinghe
		//richiedo elenco categorie al server

		int j = 0;
		for(j ; j < MAX_CATEGORIE ; j++)
			vs_categorie_gui[j] = NULL;
		
	}

	ret = catalogo(socket_descriptor,vs_categorie_gui,&num_cat,TRUE_VALUE);

	

	//layout di tipo tabella, elementi disposti in maniera omogenea
	table = gtk_table_new (2, 2, FALSE);
	

	switch(ret){
		case OK:
			
			//Aggiungo alla GList l'elenco delle categorie
			for(k;k<num_cat;k++){
				compose_mess_to_log("DEBUG: " , vs_categorie_gui[k] , STRING_TYPE);
				list=g_list_append(list,vs_categorie_gui[k]);
			}
			
			 //box principale 
			 box=gtk_vbox_new(TRUE,0);

		   /* Creates a combo box */
		   combo=gtk_combo_new();

   		/* Sets the list */
		   gtk_combo_set_popdown_strings(GTK_COMBO(combo),list);

   		/* Enables up/down keys change the value. */
   		gtk_combo_set_use_arrows_always(GTK_COMBO(combo),TRUE);

   		
			combo_button = gtk_button_new();
			box_combo_button = xpm_label_box ("../TEMPLATE/icone/ok.png", "INVIA");
		   gtk_container_add (GTK_CONTAINER (combo_button), box_combo_button);

			//richiamo la routine di gestione per effettuare la richiesta di tutti i file presenti sul server
			//if(!dp_catalogo_to_pass){
			//	dp_catalogo_to_pass = (struct pass_info *) malloc(sizeof(struct pass_info));
			//	dp_catalogo_to_pass->elements = (char**)malloc(num_cat * sizeof(char*));
			//	logger("Alloco memoria per dp_Catalogo");
			//}

			compose_mess_to_log("Numero delle categorie:",&num_cat,INTEGER_TYPE);

			dp_catalogo_to_pass->info_op = CATALOGO;
			dp_catalogo_to_pass->widgets[0] = box;
			dp_catalogo_to_pass->widgets[1] = combo;
			dp_catalogo_to_pass->elements = vs_categorie_gui;
			dp_catalogo_to_pass->num_elements = num_cat;

			
			gtk_table_attach (GTK_TABLE (table), combo, 0, 1, 0, 1, GTK_EXPAND, GTK_SHRINK, 0 ,0);
			gtk_table_attach (GTK_TABLE (table), combo_button, 0, 1, 1, 2, GTK_EXPAND, GTK_SHRINK, 0 ,0);

		
			// Settiamo lo spazio per riga e per colonna.
			gtk_table_set_row_spacings (GTK_TABLE (table), 0);
			gtk_table_set_col_spacings (GTK_TABLE (table), 0);

			//Premendo Enter si preme invia!!!
			//GTK_WIDGET_SET_FLAGS (combo_button, GTK_CAN_DEFAULT);
			//gtk_widget_grab_default (combo_button);

			
			gtk_box_pack_start(GTK_BOX(box), table,TRUE,FALSE,0);


			gtk_signal_connect(GTK_OBJECT(combo_button),
									"clicked",
									GTK_SIGNAL_FUNC(do_operation_protocol),
									(gpointer) dp_catalogo_to_pass);


			logger("DO OPERATION PROTOCOL PER CATALOGO INVOCATO");


		  // gtk_box_pack_start(GTK_BOX(box), combo_button,FALSE,FALSE,0); 

		break;
		
		case ERROR:
			//costruisco il Dialogo, (solo in questo caso) non è necessario alcun messaggio per l'ok.
			draw_dialog_panel(ret, "Notifica", "", "Catalogo non accessibile, devi effettuare il login !!!", dialog_catalogo_gui);
			free(vs_categorie_gui);
			vs_categorie_gui = NULL; 
			box = NULL;
		break;
		
		default:
			box = NULL;
		break;
	}

	//free(vs_categorie);

	return box;
}

/*
	Costruisce la tabella contenente tutti i dati riguardanti
	il download effettuato.
*/
static GtkWidget * make_scaricati_panel(void){

	//Box principale da restituire
	GtkWidget * box = NULL;
	//Matrice di GtkWidget per stampare a video i file scaricati
	const int colums_table_downloaded = 4;
	FILE * fp_local;
	GtkWidget *scrolled_window;
	GtkWidget *table;

	//leggo da db tutti i file scaricati
	struct  download_item * tmp  = scaricati();

	logger("Ho la lista dei file scaricati");

	
	//non ci sono dowload
	if(!tmp){
		//in caso non ci siano download memorizzati creo una finestra di notifica
		draw_dialog_panel(ERROR, "Notifica", "", "Nessun download trovato !!!", dialog_catalogo_gui);
		return box;
	}

	else{

		//ho delle info sui download da renderizzare, organizzo il mio box e la tabella
		box=gtk_vbox_new(TRUE,0);

		/* crea una nuova scrolled window. */
		scrolled_window = gtk_scrolled_window_new (NULL, NULL);

		gtk_container_set_border_width (GTK_CONTAINER (scrolled_window), TRUE);

		/* Setta la policy */
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

		table = gtk_table_new (num_same_item_db+1, colums_table_downloaded +1, FALSE);

		//gtk_box_pack_start (GTK_BOX (box), table, FALSE, FALSE, 0);		

		//costruisco intestazione
		//,GTK_EXPAND, GTK_SHRINK, 0 ,0
		// GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL

		gtk_table_attach/*_defaults*/ (GTK_TABLE (table), gtk_label_new("ID") ,     0, 1, 0, 1, 0, 0, 0, 0);
		gtk_table_attach/*_defaults*/ (GTK_TABLE (table), gtk_label_new("NAME") ,   1, 2, 0, 1, 0, 0, 0, 0);
		gtk_table_attach/*_defaults*/ (GTK_TABLE (table), gtk_label_new("STATUS") , 2, 3, 0, 1, 0, 0, 0, 0);
		gtk_table_attach/*_defaults*/ (GTK_TABLE (table), gtk_label_new("PATH") ,   3, 4, 0, 1, 0, 0, 0, 0);
		//gtk_table_attach/*_defaults*/ (GTK_TABLE (table), gtk_label_new("IN PATH") ,4, 5, 0, 1, 0, 0, 0, 0);	 
	

		/* setta la spaziatura della table */
		gtk_table_set_row_spacings (GTK_TABLE (table), 5);
		gtk_table_set_col_spacings (GTK_TABLE (table), 20);
	
		//utilizzo gli indici i,j per posizionare i widgets
		int i = 1,j = 0;

		while(tmp){

			/********************** LAYER PRESENTAZIONE INFORMAZIONI *****************************/			
					
			//Presenza o meno del file nella directory
			char buf_tmp[60];
			strcpy(buf_tmp,tmp->path);
			//strcat(buf_tmp,tmp->name);
			compose_mess_to_log("PATH:",buf_tmp,STRING_TYPE);
			fp_local = fopen(buf_tmp,"rb");

			if(!fp_local){
				//no presente
				bzero(buf_tmp,strlen(buf_tmp)+1);
				strcpy(buf_tmp,"NO PRESENTE IN ../FILES");
			}

			else{
				//presente	
				bzero(buf_tmp,strlen(buf_tmp)+1);
				strcpy(buf_tmp,"PRESENTE IN ../FILES");
			}
			

			//Percentuale per determinare se è completo o meno
			char str[20]; 
			if(tmp->percentuale == 100)	
				strcpy(str,"COMPLETATO");
			else
				strcpy(str,"INCOMPLETO");

			/**********************************************************************************/

			logger("Devo posizionare i valori nella tabella");

			char id[5];

			for(j = 0 ; j < (colums_table_downloaded) ; j++){

				if(!j){
					sprintf(id,"%d",i);
					compose_mess_to_log("Indice di colonna :",&i,INTEGER_TYPE);
					compose_mess_to_log("Valore da scrivere: ", id, STRING_TYPE);
					gtk_table_attach/*_defaults*/ (GTK_TABLE (table), gtk_label_new(id) ,j,j+1,i,i+1,0, 0, 0, 0);
				}
			
				else if(j == 1){
					compose_mess_to_log("Indice di colonna :",&i,INTEGER_TYPE);
					compose_mess_to_log("Valore da scrivere: ", tmp->name, STRING_TYPE);
					gtk_table_attach/*_defaults*/ (GTK_TABLE (table), gtk_label_new(tmp->name) ,j,j+1,i,i+1,0, 0, 0, 0);
				}				
		
				else if(j == 2){
					compose_mess_to_log("Indice di colonna :",&i,INTEGER_TYPE);
					compose_mess_to_log("Valore da scrivere: ", str, STRING_TYPE);
					gtk_table_attach/*_defaults*/ (GTK_TABLE (table), gtk_label_new(str) ,j,j+1,i,i+1,0, 0, 0, 0);
				}

				else if(j == 3){
					compose_mess_to_log("Indice di colonna :",&i,INTEGER_TYPE);
					compose_mess_to_log("Valore da scrivere: ", tmp->path, STRING_TYPE);
					gtk_table_attach/*_defaults*/ (GTK_TABLE (table), gtk_label_new(tmp->path) ,j,j+1,i,i+1,0, 0, 0, 0);
				}

				/*else{
					compose_mess_to_log("Indice di colonna :",&i,INTEGER_TYPE);
					compose_mess_to_log("Valore da scrivere: ", buf_tmp, STRING_TYPE);
					gtk_table_attach/*_defaults*/ /*(GTK_TABLE (table), gtk_label_new(buf_tmp) ,j,j+1,i,i+1,0, 0, 0, 0);
				}*/
			}	
		
			compose_mess_to_log("Ho composto la riga i-esima della tabella :", &i, INTEGER_TYPE);
			
			i++;
			tmp = tmp->next;
		} 
		free(tmp);
	} 

	
	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled_window), table);
	gtk_box_pack_start (GTK_BOX (box), scrolled_window,TRUE, TRUE, 0);
	
	return box;
}


static GtkWidget * make_download_panel(void){

	GtkWidget * box = NULL;
	GtkWidget *scrolled_window;
	GtkWidget *table;
	GtkWidget * button_download;
	GtkWidget * box_button_download;

	if(!dp_files->num_elements_files){
		//non ci sono files da scaricare per la categoria

		char buffer_notifica[100];
		strcpy(buffer_notifica,"Nessun file scaricabile per la categoria ");
		strcat(buffer_notifica,dp_files->info_name);

		draw_dialog_panel(ERROR,"Notifica","", buffer_notifica, dialog_login_gui);
	}

	else{

		const int max_col = 3;

		//disegna tabella nella quale riportare l'elenco dei file che si possono scaricare

		//ho delle info sui download da renderizzare, organizzo il mio box e la tabella	
		box=gtk_vbox_new(TRUE,0);

		/* crea una nuova scrolled window. */
		scrolled_window = gtk_scrolled_window_new (NULL, NULL);

		gtk_container_set_border_width (GTK_CONTAINER (scrolled_window), TRUE);

		/* Setta la policy */
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

		table = gtk_table_new (dp_files->num_elements_files +1 , 4, FALSE);

		//gtk_box_pack_start (GTK_BOX (box), table, FALSE, FALSE, 0);		

		//costruisco intestazione
		//,GTK_EXPAND, GTK_SHRINK, 0 ,0
		// GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL

		gtk_table_attach/*_defaults*/ (GTK_TABLE (table), gtk_label_new("ID") ,     0, 1, 0, 1, 0, 0, 0, 0);
		gtk_table_attach/*_defaults*/ (GTK_TABLE (table), gtk_label_new("NAME") ,   1, 2, 0, 1, 0, 0, 0, 0);
		gtk_table_attach/*_defaults*/ (GTK_TABLE (table), gtk_label_new("SIZE") ,   2, 3, 0, 1, 0, 0, 0, 0);
		gtk_table_attach/*_defaults*/ (GTK_TABLE (table), gtk_label_new("DOWNLOAD") ,   3, 4, 0, 1, 0, 0, 0, 0);
		//gtk_table_attach/*_defaults*/ (GTK_TABLE (table), gtk_label_new("PATH") ,   3, 4, 0, 1, 0, 0, 0, 0);
		//gtk_table_attach/*_defaults*/ (GTK_TABLE (table), gtk_label_new("IN PATH") ,4, 5, 0, 1, 0, 0, 0, 0);	 
	

		/* setta la spaziatura della table */
		gtk_table_set_row_spacings (GTK_TABLE (table), 7);
		gtk_table_set_col_spacings (GTK_TABLE (table), 30);
	
		//utilizzo gli indici i,j per posizionare i widgets
		int i = 1,j=0, k=0;

		compose_mess_to_log("Numero degli elementi da leggere : ", &(dp_files->num_elements_files), INTEGER_TYPE);

		int max_tmp_col =  max_col * 2;
		int last_j = 1;

		for ( j ; j < dp_files->num_elements_files ; j++){
		
			char id[5];
			sprintf(id,"%d",j+1);

			gtk_table_attach(GTK_TABLE (table), gtk_label_new(id) ,k,k+1,i,i+1,0, 0, 0, 0);
			gtk_table_attach(GTK_TABLE (table), gtk_label_new(elenco_file[j].descrizione) ,k+1,k+2,i,i+1,0, 0, 0, 0);

			char dim[25];
			sprintf(dim,"%li%s",elenco_file[j].dimensione," Bytes");
			gtk_table_attach(GTK_TABLE (table), gtk_label_new(dim) ,k+2,k+3,i,i+1,0, 0, 0, 0);	

			//costruisco bottone per avviare download

			/* Create a new button */
		   button_download = gtk_button_new ();

			GtkWidget *image;	
			/* Create box for image and label */
			box_button_download = gtk_hbox_new (FALSE, 0);
			gtk_container_set_border_width (GTK_CONTAINER (box_button_download), 0);
    		/* Now on to the image stuff */
		   image = gtk_image_new_from_file ("../TEMPLATE/icone/download.png");
			/* Pack the image and label into the box */
			gtk_box_pack_start (GTK_BOX (box_button_download), image, FALSE, FALSE, 0);
			gtk_widget_show (image);


			gtk_container_add (GTK_CONTAINER (button_download), box_button_download);		
	
			gtk_table_attach(GTK_TABLE (table), button_download ,k+3,k+4,i,i+1, 0, 0, 0, 0);


			struct pass_info * download_item_to_pass = (struct pass_info *)malloc(sizeof(struct pass_info));
			download_item_to_pass->info_op = FILES; 
			download_item_to_pass->pos_file_download_vect = j;
			download_item_to_pass->id_category = dp_files->id_category;

			g_signal_connect(button_download, "clicked", G_CALLBACK(do_operation_protocol), (gpointer) download_item_to_pass);
			
			i++;

		}

		gtk_box_pack_start (GTK_BOX (box), scrolled_window,TRUE, TRUE, 0);
			/* Inserisce la table nella scrolled window */
		gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled_window), table);
	} 

	return box;

}

GdkPixbuf * create_pixbuf(const gchar * filename)
{
	GdkPixbuf *pixbuf;
	GError *error = NULL;
   pixbuf = gdk_pixbuf_new_from_file(filename, &error);

   if(!pixbuf) {
      fprintf(stderr, "%s\n", error->message);
      g_error_free(error);
   }

	return pixbuf;
}

/* Create a new hbox with an image and a label packed into it
 * and return the box. */

static GtkWidget * xpm_label_box( gchar *xpm_filename,  gchar *label_text )
{
    GtkWidget *box;
    GtkWidget *label;
    GtkWidget *image;

    /* Create box for image and label */
    box = gtk_hbox_new (FALSE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (box), 2);

    /* Now on to the image stuff */
    image = gtk_image_new_from_file (xpm_filename);

    /* Create a label for the button */
    label = gtk_label_new (label_text);

    /* Pack the image and label into the box */
    gtk_box_pack_start (GTK_BOX (box), image, FALSE, FALSE, 3);
    gtk_box_pack_start (GTK_BOX (box), label, FALSE, FALSE, 3);

    gtk_widget_show (image);
    gtk_widget_show (label);

    return box;
}

/* CALLBACKS ROUTINES*/

/*Intercetto evento chiusura dell'applicazione, chiudo anche comunicazione con il server, oltre che l'app gtk*/
void delete_event (GtkWidget *widget, gpointer data)
{
	char * str = (char *) data;

	logger("Uscita dall'applicazione");

	//debug per capire se funziona il riconoscitore del tipo di eventi
	if(str){
		if(!strcmp(str,logout)){
			logger("Uscita da bottone logout\n");
		}	
	}

	else{
		logger("Uscita da window manager\n");
	}

	//preparo disconessione del client
	close_conn(socket_descriptor);

	//dealloco tutta la memoria utilizzata
	free(dp_login);
	free(dp_catalogo);
	free(dp_scaricati);
	free(info_login);
	free(tdp);
	free(dp_catalogo_to_pass);
	free(vs_categorie_gui);
	free(vs_files_gui);
	free(dp_files);

	//evita di incasinare la terminazione dell'applicazione 
	kill(0, SIGTERM); 

	//termino gestore gtk
   gtk_main_quit ();
}

/*gestore eventi generico*/
void do_operation_gui(GtkWidget *widget, gpointer data)
{

	//cast per ricostruire dato passato

	struct pass_info *  dp = (struct pass_info *) data;

	compose_mess_to_log("Operazione : ", dp->info_name ,STRING_TYPE);

	/*DIALOGO PER COMUNICARE LOGIN OK/KO*/	
	if(!strcmp((char *)dp->info_name,dialog_login_gui) || 
		!strcmp((char *)dp->info_name,dialog_catalogo_gui) || 
		!strcmp((char *)dp->info_name,extern_use)){

			logger("Devo rispondere all'evento generato dalla finestra di dialogo");

			//devo riattivare la finestra principale che ho precedentemente bloccato
			if(!GTK_WIDGET_IS_SENSITIVE(dp->widgets[1])){
				gtk_widget_set_sensitive(dp->widgets[1], TRUE);
			}
			//chiudo la finestra di dialogo
			gtk_widget_destroy(dp->widgets[0]);
			
			//Accrocchio per riportare la schermata al menù principale (LOGO client) in caso di LOGIN OK
			if(dp->info_op == OK ) //&& !strcmp((char *)dp->info_name,dialog_login_gui) ||)
				remove_prev_empty_space(dp->widgets[1],-1);
	
			logger("Dealloco risorse della finestra di dialogo");
	}

	else{

		logger("Operazioni di default (Login/Catalogo/Scaricati/Downloads)");

		if(!strcmp(dp->info_name,login_gui)){
			remove_prev_empty_space(dp->widgets[0],LOGIN); /*FORM COMPILAZIONE PER LOGIN*/
			logger("Dealloco memoria utilizzate per gestire risorse LOGIN");
		}
		else if(!strcmp(dp->info_name,catalogo_gui))
			remove_prev_empty_space(dp->widgets[0],CATALOGO); /*RICHIESTA CATALOGO E DOWNLOAD*/

		else if(!strcmp(dp->info_name,scaricati_gui))
			remove_prev_empty_space(dp->widgets[0],SCARICATI); /*ELENCO TUTTI I FILE SCARICATI*/

		else if(!strcmp(dp->info_name,download_window_type)){
		
			//TODO (gestire elenco dei file da scaricare)

			gtk_widget_destroy(dp->widgets[0]);
			//gtk_widget_hide_all(dp->widgets[0]);
			enter_download_view = FALSE_VALUE;
			create_new_list_downlod = 0;
			hidden = 1;
			//free(dp);
			
		}	

		else{
			//NOTHING TODO
		}
	
	}

	logger("Ho finito di gestire il layout grafico");
}


void do_operation_protocol(GtkWidget *widget, gpointer  data){

	struct pass_info *  d = (struct pass_info *) data;

	switch(d->info_op){

		case LOGIN:
			//leggo username e password
			compose_mess_to_log("Ho letto dal form username: ",gtk_entry_get_text((GtkEntry *)d->widgets[0]), STRING_TYPE);
			compose_mess_to_log("Ho letto dal form password: ",gtk_entry_get_text((GtkEntry *)d->widgets[1]), STRING_TYPE);
			//printf("Ho letto dal form: %s - %s\n", gtk_entry_get_text((GtkEntry *)d->widgets[0]), gtk_entry_get_text((GtkEntry *)d->widgets[1]));
			//eseguo operazione di login
			int ret = login(socket_descriptor, 
								(char *)gtk_entry_get_text((GtkEntry *)d->widgets[0]),
								(char *)gtk_entry_get_text((GtkEntry *)d->widgets[1]));

			draw_dialog_panel(ret,"Notifica","Autenticazione sul server eseguita!!!", "Autenticazione sul server fallita!!!", dialog_login_gui);

		break;

		case CATALOGO:
	
			logger("Dentro operazione catalogo !!!");
			//effettua richiesta per effettuare download di tutti i file per cartegoria
			char name_cat[50];
			strcpy(name_cat, (char*)gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(d->widgets[1])->entry)));
			compose_mess_to_log("Nome categoria : ", name_cat , STRING_TYPE);			

			compose_mess_to_log("Numero elementi : ", &d->num_elements , INTEGER_TYPE);
		
			//recupero id categoria
			int k = 0;	
			while(strcmp(name_cat,d->elements[k]) && k < (d->num_elements))
				k++;

			compose_mess_to_log("Selezionato: ",name_cat,STRING_TYPE);
			compose_mess_to_log("Contenuto nel vettore : ", d->elements[k],STRING_TYPE);

			//richiedo al server i file per la categoria selezionata
			logger("richiedo al server i file per la categoria selezionata");

			compose_mess_to_log("ID cateogia nel vettore : ", &k , INTEGER_TYPE);


			if(!dp_files)
				dp_files = (struct pass_info *)malloc(sizeof(struct pass_info));

			
			if(!vs_files_gui){
				logger("Prima allocazione");
				vs_files_gui =  (char**)malloc(MAX_FILES * sizeof(char*));

				int j = 0;
				for(j; j< MAX_FILES; j++)
					vs_files_gui[j] = NULL; 
			}
				
			files(socket_descriptor, elenco_categorie[k].id , vs_files_gui, &(dp_files->num_elements_files), TRUE_VALUE);
			
		
			strcpy(dp_files->info_name, name_cat);
			dp_files->info_op = FILES;
			dp_files->elements_files  = vs_files_gui;
			dp_files->widgets[0] = main_window;
			dp_files->id_category = elenco_categorie[k].id;
	
			logger("Inizio routine di refresh per download");
			remove_prev_empty_space(dp_files->widgets[0],FILES);
			logger("Fine routine di refresh per download");  
			
		break;

		case SCARICATI:
				//NOTHING TODO
		break;

		case FILES:
			//costuisco una nuova window nel quale inserire la pila dei download
			
			if(!enter_download_view)
				make_window_download();

			//inserisco un nuovo item nella finestra di download
			//richiamo gestore download dal file op.c
			//Parametri da passare:
			//id record download sulla finestra di dialogo (ret_id)
			//id_categoria da richiedere (d->id_category)
			//id_files da richiedere (elenco_file[d->pos_file_download_vect].id)

			//invoco routine per la gestione del download

			//creo una pipe
			int pipe_fd[2];
			int pipe_fd1[2];

			char id_categoria[10];
			sprintf(id_categoria, "%d", d->id_category);

			char id_file[10];
			sprintf(id_file, "%d", elenco_file[d->pos_file_download_vect].id);

		
			//creo la pipe
			if(pipe(pipe_fd)){
				//impossibile creare la pipe
				draw_dialog_panel(ERROR,"Notifica","", "Impossibile eseguire download del file", dialog_login_gui);
			}

			if(!create_new_list_downlod){
				gtklist=gtk_list_new();
				gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (download_window), gtklist);
				create_new_list_downlod = 1;
			}


			GtkProgressBar * bar; 
			bar = add_download(elenco_file[d->pos_file_download_vect].descrizione);	

			
			switch (fork()) {
				case -1:
					//errore fork.
					draw_dialog_panel(ERROR,"Notifica","","Impossibile eseguire download del file", extern_use);
				break;

				case 0:

					close(pipe_fd[0]);
					manage_files(socket_descriptor, d->pos_file_download_vect, id_categoria, id_file, pipe_fd[1]); 
					exit(0);

				break;

				default:	

					//if(ret_join)

					close(pipe_fd[1]);
					struct download_refresh info_thread_pass;
					info_thread_pass.bar = bar;
					info_thread_pass.pipe_fd = pipe_fd[0];

					refresh_perc_download( &info_thread_pass );

					/*//debug
					if(GTK_IS_PROGRESS_BAR(info_thread_pass.bar))
						logger("Progress Bar da passare al thread");
				
					compose_mess_to_log("PIPE ID: ", &(info_thread_pass.pipe_fd), INTEGER_TYPE);

					logger("Creo THREAD");

					//thread refresh download
					pthread_t  pthread_refresh_bar;
					int ret_thread = pthread_create (&pthread_refresh_bar, NULL, pthread_function, &(info_thread_pass));

					//debug					
					if(ret_thread){
						logger("Errore creazione thread di refresh");
					}
				
					//join del thread
					pthread_join (pthread_refresh_bar, NULL); */

					signal(SIGCHLD,_manage_exit_child);   //intercetto la terminazione del figlio e evito di creare ZOMBIE
		
				break;
			}
			
		break;

		default:
			//TODO
		break;
	}
	
}


void draw_dialog_panel(int ret, char * name_dialog, char * text_ok, char * text_ko, const char * type){

	//printf("Fuori da login");

	logger("Disegna Dialog");
	GtkWidget * dialog_window;
	GtkWidget * dialog_label;
	GtkWidget * dialog_button;
	GtkWidget * box_dialog_button;
	//creo finestra di dialogo
	dialog_window  = gtk_dialog_new ();
	gtk_window_set_title(GTK_WINDOW(dialog_window), name_dialog);
	gtk_window_set_default_size(GTK_WINDOW(dialog_window), 200, 150);

	switch(ret){
		case ERROR:				
			logger("Disegna Dialog per comunicare errore");
			dialog_label = xpm_label_box( "../TEMPLATE/icone/ko.png",text_ko);
		break;

		case OK:
			logger("Disegna Dialog per comunicare login");
			dialog_label = xpm_label_box( "../TEMPLATE/icone/ok.png",text_ok);
		break;

		default:
			//Nothing TODO
		break;
	}

	//preparo il Dialog
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog_window)->vbox), dialog_label, TRUE,TRUE, 0);
	dialog_button = gtk_button_new();
	box_dialog_button = xpm_label_box ("../TEMPLATE/icone/ok.png", "OK");
	gtk_container_add (GTK_CONTAINER (dialog_button), box_dialog_button);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog_window)->action_area),dialog_button,TRUE, TRUE, 0);

	//visualizzo la finestra di dialogo
	gtk_widget_show_all(dialog_window);

	//disattivo finestra principale 
	gtk_widget_set_sensitive(main_window, FALSE);


	//assegno gestore di eventi alla finestra di dialogo
	
	if(!tdp)
		tdp  = (struct pass_info *)malloc(sizeof(struct pass_info)); 
	
	tdp->info_op = ret;
	strcpy(tdp->info_name,type);
	tdp->widgets[0] = dialog_window;
	tdp->widgets[1] = main_window; 

	logger("Imposto callback per dialogo");

	g_signal_connect(dialog_button, "clicked", G_CALLBACK(do_operation_gui), (gpointer) tdp);
}


void remove_prev_empty_space(GtkWidget * container, int type_op){

	logger("Renderizzazione Grafica del pannello destro");

	//GtkWidget * parent  = NULL;
	GtkWidget * focus_child = NULL; 

	logger("Leggo focus child");
	focus_child = g_list_nth_data (gtk_container_get_children(GTK_CONTAINER(container)),1); 
	logger("Rimuovo focus child");
	gtk_container_remove(GTK_CONTAINER(container),focus_child);
	logger("Organizzo nuovo panello empty_space");

	//focus_child = NULL;

	focus_child = make_empty_space(type_op);

	//if(!focus_child) logger("Ancora A NULL");
	

	compose_mess_to_log("tipo operazione per gestire layout grafico -- ",&type_op,INTEGER_TYPE);
	compose_mess_to_log("flag operazione per gestire layout grafico -- ",&flag_empty_space,INTEGER_TYPE);

	if(flag_empty_space == TRUE_VALUE){ 
		type_op = LOGIN; //barba-accrocchio
		flag_empty_space = FALSE_VALUE;
	}

	if( type_op == SCARICATI  || type_op == FILES ){
		logger("Scaricati o files, intabellare tutto correttamente");
		gtk_box_pack_start(GTK_BOX(container),focus_child, TRUE, TRUE, 0);
		logger("Eseguita operazione di refresh");
	}
	else{
		logger("Operazione di inserimento box di default");
	
		/*if(container) logger("Contenitore non null");
		if(focus_child) logger("child dx non null"); */

		gtk_box_pack_start(GTK_BOX(container),focus_child, TRUE, FALSE, 0);		
	}

	
	gtk_widget_show_all(focus_child);

	logger("Esco routine refresh");
}

/*
	Costruisco una nuova finestra contenente tutti i download 
	che vengono eseguiti 
*/

static GtkWidget * make_window_download(){


	enter_download_view = TRUE_VALUE;

	//if(!enter_d_win){

			//Creo finestra di dialogo per comunicare tutti i download in esecuzione

		logger("Disegna Dialog Download in esecuzione");

		GtkWidget * dialog_window;
		GtkWidget * dialog_label;
		GtkWidget * dialog_button;
		GtkWidget * box_dialog_button;

		//creo finestra di dialogo
		dialog_window  = gtk_dialog_new ();
		gtk_window_set_title(GTK_WINDOW(dialog_window), "Download");
		gtk_window_set_default_size(GTK_WINDOW(dialog_window), 400, 200);

		GtkWidget * scrolled_window;
		/* crea una nuova scrolled window. */
		scrolled_window = gtk_scrolled_window_new (NULL, NULL);
		gtk_widget_set_usize(scrolled_window, 250, 150);
		gtk_container_set_border_width (GTK_CONTAINER (scrolled_window), 0);
		/* Setta la policy */
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

		gtk_box_pack_start (GTK_BOX (GTK_DIALOG(dialog_window)->vbox), scrolled_window,FALSE, FALSE, 10);

		//Assegno a un widget globale il vbox della finestra di dialogo
		//download_window = scrolled_window;
		download_window=scrolled_window;
	
		dialog_button = gtk_button_new();
		box_dialog_button = xpm_label_box ("../TEMPLATE/icone/ok.png", "OK");
		gtk_container_add (GTK_CONTAINER (dialog_button), box_dialog_button);
		gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog_window)->action_area),dialog_button,TRUE, TRUE, 0);

		//visualizzo la finestra di dialogo
		gtk_widget_show_all(dialog_window);

		enter_d_win = 1;

		if(!tdp)
			tdp  = (struct pass_info *)malloc(sizeof(struct pass_info)); 
	
		strcpy(tdp->info_name,download_window_type);
		tdp->widgets[0] = dialog_window;

		logger("Imposto callback per dialogo download");
		g_signal_connect(dialog_button, "clicked", G_CALLBACK(do_operation_gui), (gpointer) tdp);

		global_win = dialog_window;
	//}

	//else{
	//	gtk_widget_show_all(global_win);
	//}


}

/*
	Aggiunge un nuovo download al pannello dei download

	nome_file: nome del file che si sta scaricando
*/




static GtkProgressBar * add_download(char * nome_file){

	logger("Costruisco nuovo item");

	//if(hidden)
	//	gtk_widget_show_all(global_win);

	GtkWidget * label = gtk_label_new(nome_file);
	GtkWidget * hbox = gtk_hbox_new(FALSE,2);
	GtkWidget * vbox = gtk_vbox_new(FALSE,1);
	GtkWidget * pbar = gtk_progress_bar_new();//_with_adjustment (adj);

	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE,FALSE, 20);	
	gtk_box_pack_start (GTK_BOX (hbox), pbar, FALSE,FALSE,50); 

	gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE,FALSE,3); 

	list_items = gtk_list_item_new();
	gtk_container_add(GTK_CONTAINER(list_items), vbox);

	gtk_container_add(GTK_CONTAINER(gtklist), list_items);

	gtk_widget_show_all(download_window);	

	//gtk_widget_show_all(global_win);

	return GTK_PROGRESS_BAR(pbar);

}

/*
	Refresh della percentuale di download
	per un contenuto

	id_download: id del download del quale si deve aggiornare la percentuale di bytes scaricati

	NB: Esposta verso l'esterno.
*/


gint refresh_perc_download(gpointer data ){


	struct download_refresh  * val;
	val = (struct download_refresh *) data;

	struct download_refresh d;
	char str_perc[7];

	logger("Dentro refresh download");

	compose_mess_to_log("Valore ID della PIPE ", &(val->pipe_fd), INTEGER_TYPE);

	
	do{

		compose_mess_to_log("Valore di percentuale letto dalla PIPE : ", &d.perc, FLOAT_TYPE);

		//sezione critica

		if(enter_download_view){
		
				if(d.perc >= 1.0){
					gtk_progress_bar_set_text ( GTK_PROGRESS_BAR(val->bar ), "Completato"); 
					gtk_progress_bar_update (GTK_PROGRESS_BAR (val->bar ), (gfloat) 1.0);
				}
				else{
					sprintf(str_perc,"%.0f%%",d.perc*100.0);
					gtk_progress_bar_set_text ( GTK_PROGRESS_BAR(val->bar ), str_perc); 
					gtk_progress_bar_update (GTK_PROGRESS_BAR (val->bar ), d.perc);
				}
				
		}

		//gestisci tutti gli eventi in coda, aggiornando continuamente la barra
		while (gtk_events_pending())
	    gtk_main_iteration();

	}while(read(val->pipe_fd,&d,sizeof(struct download_refresh)));
 	
	logger("Finito refresh bar")	;

	close(val->pipe_fd);
	return TRUE;
}

/*UNUSED*/
void * pthread_function(void * arguments){

	logger("Dentro la routine del thread");

	struct download_refresh  * val;
	val = ((struct download_refresh *) arguments);

	//sezione critica

	int p = val->pipe_fd;	
	GtkProgressBar * b  = val->bar;
	
	compose_mess_to_log("Valore ID della PIPE ", &p, INTEGER_TYPE);

	if(GTK_IS_PROGRESS_BAR(b)){
		logger("Progress BAr ricevuta");
	} 

	else{
		logger("NO Progress BAr ricevuta");
	}

	
	refresh_perc_download( val );

	
	pthread_exit(NULL);
	
}



