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

int exec_local_cmd_loop() {
    char cmd_line[SH_CMD_MAX];
    command_list_t clist;

    while (1) {
        printf(SH_PROMPT);
        if (fgets(cmd_line, sizeof(cmd_line), stdin) == NULL) {
            break;
        }

        cmd_line[strcspn(cmd_line, "\n")] = '\0';

        if (strlen(cmd_line) == 0) {
            continue;
        }

        if (strcmp(cmd_line, EXIT_CMD) == 0) {
            printf("exiting...\n");
            return OK_EXIT;
        }

        int rc = build_cmd_list(cmd_line, &clist);
        if (rc < 0) {
            fprintf(stderr, "Failed to parse commands\n");
            continue;
        }

        execute_pipeline(&clist);
        free_cmd_list(&clist);
    }

    return OK;
}

int execute_pipeline(command_list_t *clist) {
    int num_cmds = clist->num;
    int pipefds[2 * (num_cmds - 1)];
    pid_t pids[CMD_MAX];
    int i;

    for (i = 0; i < num_cmds - 1; i++) {
        if (pipe(pipefds + i * 2) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < num_cmds; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            if (i > 0) {
                dup2(pipefds[(i - 1) * 2], STDIN_FILENO);
            }

            if (i < num_cmds - 1) {
                dup2(pipefds[i * 2 + 1], STDOUT_FILENO);
            }

            for (int j = 0; j < 2 * (num_cmds - 1); j++) {
                close(pipefds[j]);
            }

            cmd_buff_t *cmd = &clist->commands[i];
            if (execvp(cmd->argv[0], cmd->argv) == -1) {
                perror(cmd->argv[0]);
                exit(ERR_EXEC_CMD);
            }
        } else if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else {
            pids[i] = pid;
        }
    }

    for (i = 0; i < 2 * (num_cmds - 1); i++) {
        close(pipefds[i]);
    }

    int status;
    for (i = 0; i < num_cmds; i++) {
        waitpid(pids[i], &status, 0);
    }

    return OK;
}

int alloc_cmd_buff(cmd_buff_t *cmd_buff) {
    cmd_buff->_cmd_buffer = malloc(SH_CMD_MAX);
    if (!cmd_buff->_cmd_buffer) {
        return ERR_MEMORY;
    }
    return OK;
}

int free_cmd_list(command_list_t *cmd_lst) {
    for (int i = 0; i < cmd_lst->num; i++) {
        free(cmd_lst->commands[i]._cmd_buffer);
    }
    return OK;
}

int build_cmd_list(char *cmd_line, command_list_t *clist) {
    clist->num = 0;
    char *saveptr;
    char *token = strtok_r(cmd_line, PIPE_STRING, &saveptr);

    while (token != NULL) {
        if (clist->num >= CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }

        cmd_buff_t *cmd = &clist->commands[clist->num];
        alloc_cmd_buff(cmd);

        cmd->_cmd_buffer = strdup(token);
        cmd->argc = 0;

        char *arg_saveptr;
        char *arg = strtok_r(token, " ", &arg_saveptr);
        while (arg != NULL && cmd->argc < CMD_ARGV_MAX - 1) {
            cmd->argv[cmd->argc++] = arg;
            arg = strtok_r(NULL, " ", &arg_saveptr);
        }
        cmd->argv[cmd->argc] = NULL;

        clist->num++;
        token = strtok_r(NULL, PIPE_STRING, &saveptr);
    }

    return OK;
}