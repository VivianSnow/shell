//
// Created by VivianSnow on 2017/5/31.
// Last reviewed by VivianSnow on 2017/6/8
//

#include "utils.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include "redirect.h"


void find_redirect_symbol(char *buf, struct redirect_path *redirect_path_ptr)
{
    int buf_N = strlen(buf); /* the len of buf */
    int i = buf_N - 1;
    int last_pos = buf_N;
    while(i) {
        switch (buf[i]) {
            case '>':
                if ((i - 1 > 0) && (buf[i - 1] == '>')) {
                    if (strncpy(redirect_path_ptr->redirect_output_path, &buf[i+1], (size_t) (last_pos - i)) < 0){
                        perror("Error when copy redirect path");
                    }
                    --i;
                    redirect_path_ptr->redirect_output_append = 1;
                } else {
                    if (strncpy(redirect_path_ptr->redirect_output_path, &buf[i+1], (size_t) (last_pos - i)) < 0){
                        perror("Error when copy redirect path");
                    }
                }
                if(trim(redirect_path_ptr->redirect_output_path) < 0){
                    fprintf(stderr, "A fatal error may happen!");
                }
                last_pos = i;
                break;
            case '<':
                if (strncpy(redirect_path_ptr->redirect_input_path, &buf[i+1], (size_t) (last_pos - i)) < 0){
                    perror("Error when copy redirect path");
                }
                last_pos = i;
                break;
            /* In order to make implementation easy, we use ways redirect stderr to other fd like fish shell
                ref : http://fishshell.com/docs/current/tutorial.html#tut_pipes_and_redirections */
            case '^':
                if ((i - 1 > 0) && (buf[i - 1] == '^')) {
                    if (strncpy(redirect_path_ptr->redirect_error_path, &buf[i+1], (size_t) (last_pos - i)) < 0){
                        perror("Error when copy redirect path");
                    }
                    --i;
                    redirect_path_ptr->redirect_error_append = 1;
                } else {
                    if (strncpy(redirect_path_ptr->redirect_error_path, &buf[i+1], (size_t) (last_pos - i)) < 0){
                        perror("Error when copy redirect path");
                    }
                }
                last_pos = i;
                break;
            default:
                break;
        }
        --i;
    }
    buf[last_pos] = '\0';
}


void redirect(struct redirect_path *redirect_path_ptr)
{
    if (redirect_path_ptr->redirect_output_path[0] != '\0') {
        int fd;
        if (redirect_path_ptr->redirect_output_append) {
            if ((fd = open(redirect_path_ptr->redirect_output_path, O_APPENDFALG, O_MY_ACCMODE)) < 0) {
                perror("Open error");
                exit(-1);
            }
        } else {
            if ((fd = open(redirect_path_ptr->redirect_output_path, O_NORMALFLAG, O_MY_ACCMODE)) < 0) {
                perror("Open error");
                exit(-1);
            }
        }
        if(dup2(fd, STDOUT_FILENO) <0) {
            perror("Dup error");
        }
    }
    if (redirect_path_ptr->redirect_input_path[0] != '\0') {
        int fd;
        if ((fd = open(redirect_path_ptr->redirect_input_path, O_NORMALFLAG, O_MY_ACCMODE)) < 0) {
            perror("Open error");
            exit(-1);
        }

        if(dup2(fd, STDIN_FILENO) <0) {
            perror("Dup error");
        }
    }
    if (redirect_path_ptr->redirect_error_path[0] != '\0') {
        int fd;
        if (redirect_path_ptr->redirect_error_append) {
            if ((fd = open(redirect_path_ptr->redirect_error_path, O_APPENDFALG, O_MY_ACCMODE)) < 0) {
                perror("Open error");
                exit(-1);
            }
        } else {
            if ((fd = open(redirect_path_ptr->redirect_error_path, O_NORMALFLAG, O_MY_ACCMODE)) < 0) {
                perror("Open error");
                exit(-1);
            }
        }
        if(dup2(fd, STDERR_FILENO) <0) {
            perror("Dup error");
        }
    }
}