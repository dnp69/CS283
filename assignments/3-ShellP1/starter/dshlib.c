#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */
int build_cmd_list(char *cmd_line, command_list_t *clist) {

    clist->num = 0;

    if (cmd_line == NULL || *cmd_line == '\0') {
        return WARN_NO_CMDS;
    }

    char *command = cmd_line;
    while (command != NULL && *command != '\0') {
        
        while (*command == ' ') {
            command++;
        }

        if (clist->num >= CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }

        char *pipe = strchr(command, '|');
        if (pipe != NULL) {
            *pipe = '\0';
        }

        char *space_end = command + strlen(command) - 1;
        while (space_end > command && isspace((unsigned char)*space_end)) {
            space_end--;
        }
        *(space_end + 1) = '\0';

        if (*command == '\0') {
            if (pipe != NULL) {
                command = pipe + 1;
                continue;
            }
            break;
        }

        char *exe = command;
        char *args = strchr(command, ' ');
        if (args != NULL) {
            *args = '\0';
            args++;
        }

        if (strlen(exe) >= EXE_MAX) {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }
        if (args && strlen(args) >= ARG_MAX) {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }

        strncpy(clist->commands[clist->num].exe, exe, EXE_MAX);
        clist->commands[clist->num].exe[EXE_MAX - 1] = '\0';

        if (args) {
            strncpy(clist->commands[clist->num].args, args, ARG_MAX);
            clist->commands[clist->num].args[ARG_MAX - 1] = '\0';
        } else {
            clist->commands[clist->num].args[0] = '\0';
        }

        clist->num++;

        if (pipe != NULL) {
            command = pipe + 1;
        } else {
            break;
        }
    }

    return OK;
}