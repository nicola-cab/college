//////////////////////////////////////////////////////////////////////
//
// It creates a menu
//
// N.B. The "NUM_MENU_RECORD" MUST be updated according to the number of the entries of the menu!!!!
//
//////////////////////////////////////////////////////////////////////

#include "monitor.h"
#include "menu.h"
#include "server.h"
#include "tx_rx.h"


#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Menu entries
// N.B. If you add or remove entries, update the NUM_MENU_RECORD accordingly!!!
struct menu record[NUM_MENU_RECORD] =
{
   // 0
   "Switch on the Monitor",
   monitor_on,
   
   // 1
   "Switch off the Monitor",
   monitor_off,
   
   // 2   
   "Show information",
   monitor_info,

	// 3
	"Options",
	monitor_options,
   
   // 4   
   "Switch on the remote control",
   monitor_socket,   
   
   // 5   
   "Exit",
   monitor_exit   
};

struct menu_options record_op[NUM_MENU_OPTIONS ] =
{
	//0
	"Tracking scheduler",
	set_option,

	//1
	"Tracking hardirq",
	set_option,

	//2
	"Tracking exception",
	set_option,
	
	//4
	"Tracking softirq",
	set_option,

	//5
	"Set buffer size",
	set_option,

	//6
	"Set wakeup",
	set_option,

	//7
	"Exit",
	set_option,
};

char menu_buffer[NUM_MENU_RECORD + 6][NUM_CHAR_FOR_LINE];
char menu_buffer_option[NUM_MENU_OPTIONS + 6][NUM_CHAR_FOR_LINE];

// This is the title of the menu
char menu_title[] = "Scheduler Monitor";

// This is the question for the user
char ask_user[] = "Choose";

// This is the title of options menu
char menu_options_title [] = "  Select tracking options ";


////////////////////////////////////////////////////////////////////////////////////
// This function creates a menu from the entries in "struct menu record"
// The menu is printed in a buffer ("menu_buffer"), not on stdout
// In this manner we can use the buffer for both printing on stdout and socket
////////////////////////////////////////////////////////////////////////////////////
void create_menu(void)
{
   int i = 0, j = 0;
   int max = 0, temp = 0, entry_num = 0;
   char *p;
   
   char *current_entry = NULL;
   
   // The length of the title
   max = strlen(menu_title);
   
   // It finds the maximum length of the line
   for(i = 0; i < NUM_MENU_RECORD; ++i)
   {
      j = strlen(record[i].entry);
      
      if(j > max)
         max = j;
   }
   
         
  
   // It sets the length of the border
   max += 8;
      
   ////////////////////////////////////////////////////////
   // Init - First line of the menu (the border)
   ////////////////////////////////////////////////////////
   j = 0;
   
   for(i = 0; i < max; ++i)
   {
      menu_buffer[j][i] = '#';
   }
   
   menu_buffer[j][i] = '\n';   
   ////////////////////////////////////////////////////////
   // End - First line of the menu (the border)
   ////////////////////////////////////////////////////////   
   
   
   
   ////////////////////////////////////////////////////////
   // Init - Second line of the menu (title)
   ////////////////////////////////////////////////////////   
   ++j;   
   p = menu_buffer[j];
   *p++ = '#';   
   
   temp = (max - 2 - strlen(menu_title)) / 2;
   
   // It puts the initial spaces
   for(i = 0; i < temp; ++i)
      *p++ = ' ';
      
   strncpy(p, menu_title, strlen(menu_title));

   p += strlen(menu_title);
   
   temp = max - 2 - temp - strlen(menu_title);
   
   // It puts the final spaces
   for(i = 0; i < temp; ++i)
      *p++ = ' ';   
   
   *p++ = '#';
   *p++ = '\n';
   ////////////////////////////////////////////////////////
   // End - Second line of the menu (title)
   ////////////////////////////////////////////////////////         
      
      
   ////////////////////////////////////////////////////////
   // Init - Third line of the menu (blank line)
   ////////////////////////////////////////////////////////      
   ++j;   
   p = menu_buffer[j];
   *p++ = '#';   
   
   temp = (max - 2);
   
   // It puts the spaces
   for(i = 0; i < temp; ++i)
      *p++ = ' ';
   
   *p++ = '#';
   *p++ = '\n';   
   ////////////////////////////////////////////////////////
   // End - Third line of the menu (blank line)
   ////////////////////////////////////////////////////////    
   
        
   
   ////////////////////////////////////////////////////////
   // Init - Following lines of the menu (menu entries)
   ////////////////////////////////////////////////////////       
   for(entry_num = 0; entry_num < NUM_MENU_RECORD; ++entry_num)
   {
      ++j;
      p = menu_buffer[j];
      *p++ = '#';   
      
      current_entry = record[entry_num].entry;
      
      *p++ = ' ';
      sprintf(p++, "%1d", entry_num);
      *p++ = ' ';
      *p++ = '-';
      *p++ = ' ';
               
      strncpy(p, current_entry, strlen(current_entry));
   
      p += strlen(current_entry);
      
      temp = max - 7 - strlen(current_entry);
      
      // It puts the final spaces
      for(i = 0; i < temp; ++i)
         *p++ = ' ';   
      
      *p++ = '#';
      *p++ = '\n';
   }
   ////////////////////////////////////////////////////////
   // End - Following lines of the menu (menu entries)
   ////////////////////////////////////////////////////////          
   
   
   
   ////////////////////////////////////////////////////////
   // Init - Last line of the menu (the border)
   ////////////////////////////////////////////////////////          
   ++j;
   
   for(i = 0; i < max; ++i)
   {
      menu_buffer[j][i] = '#';
   }
   
   menu_buffer[j][i] = '\n';      
   ////////////////////////////////////////////////////////
   // End - Last line of the menu (the border)
   ////////////////////////////////////////////////////////   
   
   
   // Blank line
   ++j;   
   p = menu_buffer[j];
   *p++ = '\n';   
   
   
   ////////////////////////////////////////////////////////
   // Init - Question for the user
   ////////////////////////////////////////////////////////      
   ++j;   
   p = menu_buffer[j];
   
   strncpy(p, ask_user, strlen(ask_user));
   
   p += strlen(ask_user);
   
   *p++ = ':';
   *p++ = ' ';     
   ////////////////////////////////////////////////////////
   // End - Question for the user
   ////////////////////////////////////////////////////////   
}



////////////////////////////////////////////////////////
// Init - print menu
//////////////////////////////////////////////////////// 
// It prints the menu
void print_menu(void)
{   
   int entry_num = 0;
   
   for(entry_num = 0; entry_num < (NUM_MENU_RECORD + 6); ++entry_num)
   {
      printline(conn_fd_client, "%s", menu_buffer[entry_num]);
   }
   
   fflush(stdout);
}
////////////////////////////////////////////////////////
// End - print menu
//////////////////////////////////////////////////////// 


////////////////////////////////////////////////////////
// Init - it gets the user input
////////////////////////////////////////////////////////
int get_user_choice(void)
{
   int choice = -1, i = 0;
   char c;
	int read_check =-1;
   
   
   while(1)
   {
      if(CONNECTION == LOCAL){
			do{ 
				if(!read_check){	
						scanf("%*[^\n]");
						printf("\n%s",ask_user);
				}
				read_check = scanf("%d",&choice);
			}while(!read_check || choice < 0 || choice >=  NUM_MENU_RECORD);
		}
      else
      {      
         c = (char) 255;
         
         if((fullwrite(conn_fd_client, &c, 1)) < 0)
         {
            perror("write error");
            return 0;            }
                  
         i = receive_data(conn_fd_client, &c);
         
         if(i < 0)
         {
            printf("Errors on received data!!\n\n");
            return;
         }
         
         choice = (int) c;
         
         printf("%d\n", choice);
      }      
      
      if((0 <= choice) && (choice < NUM_MENU_RECORD)) 
         break;
      
      printline(conn_fd_client, "La voce di menu' selezionata non esiste!\n");
      printline(conn_fd_client, "%s: ", ask_user);
      fflush(stdout);
   }
	
   return choice;
}
////////////////////////////////////////////////////////
// End - it gets the user input
//////////////////////////////////////////////////////// 


/*START OPTIONS SIDE*/

////////////////////////////////////////////////////////
// Init - print menu options
//////////////////////////////////////////////////////// 
// It prints the menu
void print_menu_options(void)
{   
   int entry_num = 0;
   
   for(entry_num = 0; entry_num < (NUM_MENU_OPTIONS + 6); ++entry_num)
   {
      printline(conn_fd_client, "%s", menu_buffer_option[entry_num]);
   }
   
   fflush(stdout);
}
////////////////////////////////////////////////////////
// End - print menu
//////////////////////////////////////////////////////// 

///////////////////////////////////////////////////////
// Menu to select tracking options
//////////////////////////////////////////////////////

void create_menu_options(void){
  	int i = 0, j = 0;
   int max = 0, temp = 0, entry_num = 0;
   char *p;
   
   char *current_entry = NULL;
   
   // The length of the title
   max = strlen(menu_options_title);
   
   // It finds the maximum length of the line
   for(i = 0; i < NUM_MENU_OPTIONS; ++i)
   {
      j = strlen(record_op[i].entry);
      
      if(j > max)
         max = j;
   }
   
         
  
   // It sets the length of the border
   max += 8;
      
   ////////////////////////////////////////////////////////
   // Init - First line of the menu (the border)
   ////////////////////////////////////////////////////////
   j = 0;
   
   for(i = 0; i < max; ++i)
   {
      menu_buffer_option[j][i] = '#';
   }
   
   menu_buffer_option[j][i] = '\n';   
   ////////////////////////////////////////////////////////
   // End - First line of the menu (the border)
   ////////////////////////////////////////////////////////   
   
   
   
   ////////////////////////////////////////////////////////
   // Init - Second line of the menu (title)
   ////////////////////////////////////////////////////////   
   ++j;   
   p = menu_buffer_option[j];
   *p++ = '#';   
   
   temp = (max - 2 - strlen(menu_options_title)) / 2;
   
   // It puts the initial spaces
   for(i = 0; i < temp; ++i)
      *p++ = ' ';
      
   strncpy(p, menu_options_title, strlen(menu_options_title));

   p += strlen(menu_options_title);
   
   temp = max - 2 - temp - strlen(menu_options_title);
   
   // It puts the final spaces
   for(i = 0; i < temp; ++i)
      *p++ = ' ';   
   
   *p++ = '#';
   *p++ = '\n';
   ////////////////////////////////////////////////////////
   // End - Second line of the menu (title)
   ////////////////////////////////////////////////////////         
      
      
   ////////////////////////////////////////////////////////
   // Init - Third line of the menu (blank line)
   ////////////////////////////////////////////////////////      
   ++j;   
   p = menu_buffer_option[j];
   *p++ = '#';   
   
   temp = (max - 2);
   
   // It puts the spaces
   for(i = 0; i < temp; ++i)
      *p++ = ' ';
   
   *p++ = '#';
   *p++ = '\n';   
   ////////////////////////////////////////////////////////
   // End - Third line of the menu (blank line)
   ////////////////////////////////////////////////////////    
   
        
   
   ////////////////////////////////////////////////////////
   // Init - Following lines of the menu (menu entries)
   ////////////////////////////////////////////////////////       
   for(entry_num = 0; entry_num < NUM_MENU_OPTIONS; ++entry_num)
   {
      ++j;
      p = menu_buffer_option[j];
      *p++ = '#';   
      
      current_entry = record_op[entry_num].entry;
      
      *p++ = ' ';
      sprintf(p++, "%1d", entry_num);
      *p++ = ' ';
      *p++ = '-';
      *p++ = ' ';
               
      strncpy(p, current_entry, strlen(current_entry));
   
      p += strlen(current_entry);
      
      temp = max - 7 - strlen(current_entry);
      
      // It puts the final spaces
      for(i = 0; i < temp; ++i)
         *p++ = ' ';   
      
      *p++ = '#';
      *p++ = '\n';
   }
   ////////////////////////////////////////////////////////
   // End - Following lines of the menu (menu entries)
   ////////////////////////////////////////////////////////          
   
   
   
   ////////////////////////////////////////////////////////
   // Init - Last line of the menu (the border)
   ////////////////////////////////////////////////////////          
   ++j;
   
   for(i = 0; i < max; ++i)
   {
      menu_buffer_option[j][i] = '#';
   }
   
   menu_buffer_option[j][i] = '\n';      
   ////////////////////////////////////////////////////////
   // End - Last line of the menu (the border)
   ////////////////////////////////////////////////////////   
   
   
   // Blank line
   ++j;   
   p = menu_buffer_option[j];
   *p++ = '\n';   
   
   
   ////////////////////////////////////////////////////////
   // Init - Question for the user
   ////////////////////////////////////////////////////////      
   ++j;   
   p = menu_buffer_option[j];
   
   strncpy(p, ask_user, strlen(ask_user));
   
   p += strlen(ask_user);
   
   *p++ = ':';
   *p++ = ' ';     
   ////////////////////////////////////////////////////////
   // End - Question for the user
   //////////////////////////////////////////////////////// 
}


////////////////////////////////////////////////////////
// Init - it gets the user input options
////////////////////////////////////////////////////////
int get_user_choice_options(void)
{
   int choice = 0, i = 0;
   char c;
	int read_check =-1;
   
   
   while(1)
   {		
      if(CONNECTION == LOCAL){
			do{ 
				if(!read_check){	
						scanf("%*[^\n]");
						printline(conn_fd_client,"\n%s",ask_user);
				}
				fflush(stdin);
				read_check = scanf("%d",&choice);	//leggo un valore
			}while(!read_check || choice < 0 || choice >=  NUM_MENU_OPTIONS);
		}
      else
      {      
         c = (char) 255;
         if((fullwrite(conn_fd_client, &c, 1)) < 0)
         {
            perror("write error");
            return 0;            }
                  
         i = receive_data(conn_fd_client, &c);
         
         if(i < 0)
         {
            printf("Errors on received data!!\n\n");
            return;
         }
         
         choice = (int) c;
         
         printf("%d\n", choice);
      }      
      
      if((0 <= choice) && (choice < NUM_MENU_OPTIONS)) 
         break;
      
      printline(conn_fd_client, "La voce di menu' selezionata non esiste!\n");
      printline(conn_fd_client, "%s: ", ask_user);
      fflush(stdout);
   }
   return choice;
}
////////////////////////////////////////////////////////
// End - it gets the user input
//////////////////////////////////////////////////////// 

void launch_menu_options(int choice,int mode)
{
   record_op[choice].p(choice,mode);
	return;
}

void print_menu_options_screen(int i){
	printline(conn_fd_client,"\n - %s\t",record_op[i].entry);
	return;
}

/*END OPTIONS SIDE*/



void launch_menu(int choice)
{
   record[choice].p();
}
