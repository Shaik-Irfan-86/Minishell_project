#include "minishell.h"

void execute_internal_commands(char *input_string)
{
    char new_path[1024], temp[1024];
    memset(temp,0,sizeof(temp));

    if ( strstr(input_string, "cd")==input_string )
    {
        if( *(input_string+2) == '\0' )
        { 
            getcwd(temp,sizeof(temp));
            memset(new_path,0,sizeof(new_path));
            strcpy(new_path,home_path);
        }
        else if( (input_string[3]=='-') &&  (input_string[4]=='\0') )
        {       
            getcwd(temp,sizeof(temp));
            memset(new_path,0,sizeof(new_path));
            strcpy(new_path,prev_path);
        }
        else
        {
            getcwd(temp,sizeof(temp));
            memset(new_path,0,sizeof(new_path));
            strcpy(new_path,input_string+3);
        }

        if(chdir(new_path) == -1)
        {
            if(errno == EACCES)
                printf("Permission is denied\n");
            else if(errno == EFAULT)
                printf("Path points to outside of accessable address space\n");
            else if(errno == ENOENT)
                printf("Specified path does not exist\n");
        }
        else
        {
            strcpy(prev_path,temp);
        }
    }
    else if (!strcmp(input_string, "jobs"))
    {
        print_nodes();
    }
    else if (!strcmp(input_string, "bg"))
    {
        background_process();
    }
    else if (!strcmp(input_string, "fg"))
    {
        foreground_process();
    }
}

void background_process(void)
{
    if (head == NULL)
    {
        printf("bg: no stopped process\n");
        return;
    }
    
    pid_t pid = head->child_pid;
    printf("[%d] %s &\n", pid, head->process_name);
    
    kill(pid, SIGCONT);
    
    int status;
    pid_t ret = waitpid(pid, &status, WNOHANG);
    if (ret == 0)
    {
        // Child is still running.
        head->status = RUNNING;
    }
    else if (ret == pid)
    {
        // A child status change was reported.
        if (WIFEXITED(status))
        {
            head->status = DONE;
            printf("[%d]+ Done %s\n", pid, head->process_name);
            exit_status = WEXITSTATUS(status);
        }
        else if (WIFSIGNALED(status))
        {
            head->status = DONE;
            exit_status = 128 + WTERMSIG(status);
        }
    }
    else if (ret == -1)
    {
        perror("waitpid");
    }
    child_pid = 0;
}

void foreground_process(void)
{
    if (head == NULL)
    {
        printf("fg: no stopped process\n");
        return;
    }

    pid_t pid = head->child_pid;
    strcpy(process_name, head->process_name);

    delete_first();

    child_pid = pid;
    kill(pid, SIGCONT);

    int status;

    waitpid(pid, &status, WUNTRACED);
    if (WIFEXITED(status))
    {
        exit_status = WEXITSTATUS(status);
    }
    else if (WIFSIGNALED(status))
    {
        exit_status = 128 + WTERMSIG(status);
    }
    else if (WIFSTOPPED(status))
    {
        insert_first(process_name, pid);
        exit_status = 128 + WSTOPSIG(status);
    }
    child_pid = 0;
}

