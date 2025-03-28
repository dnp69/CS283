#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */
void trim(char *input, cmd_buff_t *cmd)
{
    cmd->argc = 0;
    bool in_quotes = false;
    char *start = input, *arg_start = NULL;
    int arg_len = 0;

    while (*start && isspace((unsigned char)*start))
        start++;

    char *dest = cmd->_cmd_buffer;
    while (*start)
    {
        if (*start == '"')
        {
            in_quotes = !in_quotes;
        }
        else if (!in_quotes && isspace((unsigned char)*start))
        {
            if (arg_len > 0)
            {
                *dest++ = '\0';
                cmd->argv[cmd->argc++] = arg_start;
                arg_len = 0;
            }
        }
        else
        {
            if (arg_len == 0)
                arg_start = dest;
            *dest++ = *start;
            arg_len++;
        }
        start++;
    }

    if (arg_len > 0)
    {
        *dest++ = '\0';
        cmd->argv[cmd->argc++] = arg_start;
    }
    cmd->argv[cmd->argc] = NULL;
}

int exec_local_cmd_loop()
{
    int rc = 0;
    char *cmd_buff;

    cmd_buff = malloc(ARG_MAX);
    if (!cmd_buff)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return ERR_MEMORY;
    }

    cmd_buff_t cmd;
    cmd._cmd_buffer = cmd_buff;

    while (1)
    {
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL)
        {
            printf("\n");
            break;
        }

        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';
        if (strlen(cmd_buff) == 0)
        {
            printf(CMD_WARN_NO_CMD);
            continue;
        }

        trim(cmd_buff, &cmd);

        if (cmd.argc == 0)
        {
            printf(CMD_WARN_NO_CMD);
            continue;
        }

        if (strcmp(cmd.argv[0], EXIT_CMD) == 0)
        {
            free(cmd_buff);
            exit(0);
        }

        if (strcmp(cmd.argv[0], "cd") == 0)
        {
            if (cmd.argc > 1)
            {
                if (chdir(cmd.argv[1]) != 0)
                {
                    perror("cd");
                }
            }
            continue;
        }

        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork");
            continue;
        }
        if (pid == 0)
        {
            execvp(cmd.argv[0], cmd.argv);
            perror("execvp");
            exit(EXIT_FAILURE);
        }
        else
        {
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status))
            {
                int exit_status = WEXITSTATUS(status);
                if (exit_status != 0)
                    fprintf(stderr, "Execution failed with exit code %d\n", exit_status);
            }
        }
    }

    free(cmd_buff);
    return rc;
}
