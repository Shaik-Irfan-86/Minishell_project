#include "minishell.h"

void execute_external_commands(char *input_string)
{
    char *command_with_options[50];
    int command_count;
    
    convert_1D_to_2D(input_string,command_with_options,&command_count);
            
    strcpy(process_name, input_string);

    child_pid = fork();
    if (child_pid == -1)
    {
        perror("fork");
        child_pid = 0;
        return;
    }
    
    if (child_pid == 0)
    {
        setpgid(0, 0);

        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);

        execvp(command_with_options[0], command_with_options);
        perror("execvp");

        exit(20);
    }
    else if (child_pid > 0)
    {
        setpgid(child_pid, child_pid);

        int status;
        waitpid(child_pid, &status, WUNTRACED);

        if (WIFEXITED(status))                  // Did the child terminate normally?
        {
            exit_status = WEXITSTATUS(status);  // What exit status did it return?
        }
        else if (WIFSIGNALED(status))             // Did the child terminate because of a signal?
        {
            exit_status = 128 + WTERMSIG(status); // Which signal terminated the child?
        }
        else if (WIFSTOPPED(status))                // Is the child stopped?
        {
            insert_first(process_name, child_pid);

            exit_status = 128 + WSTOPSIG(status);  // Which signal stopped the child?
        }
        child_pid = 0;
    }

    for (int i = 0; i < command_count; i++)
    {
        free(command_with_options[i]);
    }
}


void convert_1D_to_2D(char *input_string,char *command_with_options[],int *command_count)
{
    char buff[100];

    int i = 0;  //for input string
    int j = 0;  //for command_with_options
    int k = 0;  //for buff

    while (1)
    {
        if ( (input_string[i] != ' ') && (input_string[i] != '\t') && (input_string[i] != '\0') && (input_string[i] != '|') )
        {
            buff[k++] = input_string[i];
        }
        else
        {
            if (k > 0) 
            {
                buff[k] = '\0';
                command_with_options[j] = malloc(strlen(buff) + 1);
                if (command_with_options[j] == NULL)
                {
                    printf("Memory Allocation Failed\n");
                    return;
                }
                strcpy(command_with_options[j++], buff);
                k = 0;
            }
            if(input_string[i] == '|')
            {
                command_with_options[j] = malloc(2);
                strcpy(command_with_options[j++], "|");
            }
        }
        if(input_string[i] == '\0')
            break;
        else
            i++;
    }
    command_with_options[j] = NULL;
    *command_count = j;
}
