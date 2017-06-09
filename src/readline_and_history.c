//
// Created by VivianSnow on 2017/5/17.
// Last reviewed by VivianSnow on 2017/6/5
//

#include "readline_and_history.h"

static char *buf = NULL;  //终端输入字符串
static char *buf_cont = NULL; //符号\后接着输入的内容
static char *repeat_buf = NULL;

void history_init() {

    using_history();
}

void history_command()
{
    register HIST_ENTRY **list;
    register int i;
    list = history_list();
    if (list)
        for (i = 0; list[i]; i++)
            printf ("%d: %s\n", i + history_base, list[i]->line);
}


char* history_repeat(char *cmd)
{
    int n;
    HIST_ENTRY **list;
    list = history_list();
    if(repeat_buf) {
        free(repeat_buf);
        repeat_buf = NULL;
    }

    //atoi can not handle error, we use `sscanf` instead
    if (sscanf(cmd + 1, "%d", &n) == 0) {
        fprintf(stderr, "%s: event not found", cmd);
        return NULL;
    }
    if (abs(n) >= history_length || n == 0){
        fprintf(stderr, "%s: event not found", cmd);
        return NULL;
    }
    if (n < 0) {
        n = (history_length + n);
    }
    if( (repeat_buf = malloc(sizeof(list[n]->line)+1)) == NULL) {
        fprintf(stderr, "maclloc error");
    }
    strcpy(repeat_buf, list[n-1]->line);
    return repeat_buf;
}

char *read_cmd_line()
{
    //if buf is not NULL, free buf first
    if(buf)
    {
        free(buf);
        buf = NULL;
    }

    extern char prompt_string[50];
    if( (buf = readline(prompt_string)) == NULL)
    {
        return buf;
    }

    if (buf[strlen(buf) - 1] == '\n') {
        buf[strlen(buf) - 1] = '\0';
    }
    while (buf[strlen(buf) - 1] == '\\') {
        buf[strlen(buf) - 1] = '\0';
        if( (buf_cont = readline(">") )) {
            if (buf_cont[strlen(buf_cont) - 1] == '\n') {
                buf_cont[strlen(buf_cont) - 1] = '\0';
            }
            void *new_ptr = realloc(buf, sizeof(buf) + sizeof(buf_cont) + 1);
            if (!new_ptr) {
                perror("Realloc error, check your memory");
                exit(-1);
            }
            strcat(buf, buf_cont);
            free(buf_cont);
        } else{
            break;
        }
    }

    //trim buf
    if (trim(buf) < 0) {
        perror("Trim error!");
    }

    if('\0' != buf) {
        add_history(buf);
    }

    return buf;
}


char** get_cmd(char *cmd_line) {
    static char **cmd = NULL;
    int n = 0; //number of sub cmds

    if (cmd != NULL) {
        free(cmd);
    }
    if( (cmd = malloc(2 * sizeof(char*)) ) == NULL)
    {
        goto error_exit;
    } else{
        cmd[0] = cmd[1] = NULL;
        // make sure last one element of cmd is NULL
    }

    if (cmd_line == NULL) {  //end of line or NULL
        goto error_exit;
    }

    char *cmd_start = cmd_line;

    while (*cmd_line != '\0') {
        if (*cmd_line == ';') {
            char **temp = NULL;
            if ( (temp = realloc(cmd, n+3 * sizeof(char *)) ) == NULL) {
                free(cmd); //if realloc faild, we should avoid memory leak
                goto error_exit;
            } else {
                cmd = temp;
            }
            cmd[n + 2] = cmd[n+1] = NULL;
            if( (cmd[n] = malloc(sizeof(char) * (cmd_line - cmd_start + 1)) ) == NULL) {
                goto error_exit;
            }
            strncpy(cmd[n], cmd_start, cmd_line - cmd_start);
            cmd[n][cmd_line - cmd_start] = '\0'; //strncpy will not add '\0' at last
            n++;
            cmd_start = cmd_line + 1;
        }
        cmd_line++;
    }

    //copy before '\0'
    if( (cmd[n] = malloc(sizeof(char) * (cmd_line - cmd_start + 1)) ) == NULL) {
        goto error_exit;
    }
    strncpy(cmd[n], cmd_start, cmd_line - cmd_start + 1);

    return cmd;


    //error exit
    error_exit:
        fprintf(stderr, "some error happened in spilt cmd");
        free(cmd);
        cmd = NULL;
        return cmd;
}

