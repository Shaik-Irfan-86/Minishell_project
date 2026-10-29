#include "minishell.h"

void execute_pipeline(char *input_string)
{
    char *command_with_options[50];
    int i=0,command_count;
    convert_1D_to_2D(input_string,command_with_options,&command_count);

    int pipe_index[10]={}, pipe_count=1;
    pipe_index[0]=0;
    while(command_with_options[i])
    {
        if( !strcmp(command_with_options[i],"|") )
        {
            pipe_index[pipe_count++] = i+1;
            command_with_options[i] = NULL;
        }
        i++;
    }
    pipe_count--;


    int pipe_fd[pipe_count][2];
    dup(0);
    for(int i=0; i<=pipe_count ; i++)
    {
        if(i<pipe_count)
            pipe(pipe_fd[i]);
        
        int pid1 = fork();
        if(pid1==0)
        {   
            if(i<pipe_count)
            {
                close( pipe_fd[i][0] );
                dup2( pipe_fd[i][1],1);
            }
            execvp(command_with_options[pipe_index[i]], &command_with_options[pipe_index[i]]);
            exit(0);
        }
        else if(pid1>0)
        {
            if(i<pipe_count)
            {
                dup2( pipe_fd[i][0],0);
                close(pipe_fd[i][0]);
                close(pipe_fd[i][1]);   
            }
            wait(NULL);
        }
    }
    dup2(3,0);
}
