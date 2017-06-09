//
// Created by VivianSnow on 2017/5/11.
// Last reviewed by VivianSnow on 2017/6/3
//

#include "export.h"

static void update_prefix()
{
    char *temp;
    extern char prompt_string[50];
    if ((temp = getenv("PROMPT")) != NULL) {
        strcpy(prompt_string, temp);
    } else {
        strcpy(prompt_string, "shell>");
    }
}

int export_command(const char *cmd)
{
    wordexp_t p;
    if (wordexp(cmd, &p, 0) < 0)
    {
        goto error_return;
    }
    if (p.we_wordc <= 2) {
        goto print_help;
    }
    if((strcmp(p.we_wordv[2], "=") == 0) ||  (strcmp(p.we_wordv[2], "--assign") == 0))
    {
        if(p.we_wordc != 4)
        {
            goto print_help;
        }
        if(setenv(p.we_wordv[1], p.we_wordv[3], 1) <0) {
            goto error_return;
        }
    }
    if( (strcmp(p.we_wordv[2], "+=") == 0) || (strcmp(p.we_wordv[2], "--append") == 0) )
    {
        char env_tmp[1024] = {0};
        //Hint: user should add ':' manually, or will cause fatal error
        strcat(strcpy(env_tmp, getenv(p.we_wordv[1])), p.we_wordv[3]);
        if(setenv(p.we_wordv[1], env_tmp, 1) < 0) {
            goto error_return;
        }
    }
    if( (strcmp(p.we_wordv[2], "-d") == 0) || (strcmp(p.we_wordv[2], "--del") == 0) )
    {
        if(unsetenv(p.we_wordv[1]) < 0) {
            goto error_return;
        }
    }
    wordfree(&p);
    update_prefix();
    return 0;

    //goto

    print_help:
        wordfree(&p);
        printf("Usage: export [OPTION]... [FILE]...");
        return 0;

    error_return:
        wordfree(&p);
        perror("some error happen");
        return -1;
}


