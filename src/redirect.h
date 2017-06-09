//
// Created by VivianSnow on 2017/5/31.
//

#ifndef SHELL_REDIRECT_H
#define SHELL_REDIRECT_H


#include <limits.h>
/*Have big difference with O_ACCMODE in _default_fcntl.h */

#define O_MY_ACCMODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define O_NORMALFLAG (O_WRONLY | O_CREAT | O_TRUNC)
#define O_APPENDFALG (O_WRONLY | O_CREAT | O_APPEND)


struct redirect_path {
    char redirect_input_path[PATH_MAX];
    char redirect_output_path[PATH_MAX];
    char redirect_error_path[PATH_MAX];

    /*bit fields should more efficient and readable than mask and bitwise*/
    unsigned int redirect_output_append : 1;
    unsigned int redirect_error_append : 1;

};


void find_redirect_symbol(char *buf, struct redirect_path *redirect_path_ptr);
void redirect(struct redirect_path *redirect_path_ptr);

#endif //SHELL_REDIRECT_H
