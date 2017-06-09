//
// Created by VivianSnow on 2017/5/17.
//

#ifndef SHELL_READLINE_H
#define SHELL_READLINE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include <readline/readline.h>
#include <readline/history.h>
char *read_cmd_line();

char** get_cmd(char *cmd_line);

void history_init();

void history_command();

char* history_repeat(char *cmd);



#endif //SHELL_READLINE_H
