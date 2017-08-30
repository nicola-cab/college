#ifndef __monitor_h__
#define __monitor_h__

#define NUM_SONDE 4
struct sonde_monitor{
	int type;
	int dipendenza_w;
	int dipendenza_r;
}; 

#define printline(x, _a1, _aN...)         \
{                                         \
   char _buff[128];                       \
   sprintf(_buff, _a1, ##_aN);            \
   print_line(x, _buff);                  \
}

void print_line(int fd, const char *);

enum connection {LOCAL, REMOTE} CONNECTION;


// functions prototypes
void monitor_on(void);
void monitor_off(void);
void monitor_options(void);
void monitor_info(void);
void monitor_socket(void);
void monitor_exit(void);
void set_option(int,int);
   


#endif // __monitor_h__
