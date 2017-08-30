#ifndef __menu_h__
#define __menu_h__



#define NUM_CHAR_FOR_LINE 80
#define NUM_MENU_RECORD 6        // N.B. This number must be equal to the number of entries of the menu record!!
#define NUM_MENU_OPTIONS 7


#define SCHED 0
#define HARD_INT 1
#define EXCEPTION 2
#define SOFT_INT 3
#define BUFFER 4
#define WAKE 5
#define EXIT 6


struct menu
{
   char entry[NUM_CHAR_FOR_LINE];
   void (*p)(void);
};

struct menu_options
{
	char entry[NUM_CHAR_FOR_LINE];
	void (*p)(int,int);
};


// functions prototypes
void create_menu(void);
void print_menu(void);
int get_user_choice(void);
void launch_menu(int);

void create_menu_options(void);
int get_user_choice_options(void);
void print_menu_options(void);
void lauch_menu_options(int,int);
void print_menu_options_screen(int);

#endif // __menu_h__
