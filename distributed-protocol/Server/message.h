//utilizzo questa costante per mettere in coda ai messaggi ulteriori informazioni che posso conoscere solo a runtime (come numero di bytes letti/scritti)
#define USEFULL_CONST 1100
#define ADD_LENGHT 100

//utilizzate in compose message
#define INTEGER_TYPE 0
#define STRING_TYPE 1
#define FLOAT_TYPE 2

extern char *start ;
extern char * end ;
extern char * unable_create_socket;
extern char * unable_make_bind;
extern char * unable_accept;
extern char * unable_fork; 
extern char * startup_server;
extern char make_socket[USEFULL_CONST];
extern char make_accept[USEFULL_CONST];
extern char make_accept_add[USEFULL_CONST];
extern char make_bind[USEFULL_CONST];
extern char make_bind_conn[USEFULL_CONST];
extern char mess_write_socket[USEFULL_CONST];
extern char mess_read_socket[USEFULL_CONST];
extern char *mess_write_socket_error;
extern char *mess_read_socket_error;
extern char mess_write_socket_plus[USEFULL_CONST];
extern char mess_read_socket_plus[USEFULL_CONST];
extern char close_socket[USEFULL_CONST];
