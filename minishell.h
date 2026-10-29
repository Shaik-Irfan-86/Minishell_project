#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>

#define BUILTIN 1
#define EXTERNAL 2
#define NO_COMMAND 3

#define STOPPED  1
#define RUNNING  2
#define DONE     3

typedef struct Node
{
    char process_name[30];      // Stores process name
    pid_t child_pid;            // stores shild pid
    int status;                 // Stores state: STOPPED, RUNNING, DONE
    struct Node *link;          // Points to next stopped process
} Slist;


extern char *external_command[153];   // to store external commands
extern Slist *head;                   // to maintain Linked List
extern pid_t child_pid;               // to add in Linked List and print at commands: jobs, bg
extern char process_name[30];         // to add in Linked List and print at commands: jobs, bg
extern char home_path[1024];          // for cd
extern char prev_path[1024];          // for cd -
extern int exit_status;               // for echo $?

void extract_external_commands(char **external_commands);

void scan_input(char *prompt, char *input_string);
char *get_command(char *input_string);
int check_command_type(char *command);
void execute_external_commands(char *input_string);
void execute_internal_commands(char *input_string);
void foreground_process(void);
void background_process(void);
void signal_handler(int sig_num);
void insert_first(char *name, pid_t pid);
void delete_first(void);
void print_nodes(void);

void convert_1D_to_2D(char *input_string,char *command_with_options[],int *command_count);
void execute_pipeline(char *input_string);

#endif