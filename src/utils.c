//
// Created by VivianSnow on 2017/5/18.
// Last reviewed by VivianSnow on 2017/5/18
//

#include <stdio.h>
#include <string.h>
#include "utils.h"

#define ISSPACE(x) ((x)==' '||(x)=='\r'||(x)=='\n'||(x)=='\f'||(x)=='\b'||(x)=='\t')

/*Thanks http://bbs.chinaunix.net/thread-277036-1-1.html */
int trim(char *string)
{
    char *tail_ptr, *head_ptr;
    for (tail_ptr = string + strlen(string) - 1; tail_ptr >= string; tail_ptr--) {
        if (!ISSPACE(*tail_ptr)) {
            break;
        }
    }
    *(tail_ptr+1) = 0;
    for (head_ptr = string; head_ptr <= tail_ptr; head_ptr++) {
        if (!ISSPACE(*head_ptr)) {
            break;
        }
    }
    if(memcpy(string, head_ptr, (tail_ptr - head_ptr + 2) * sizeof(char)) < 0) {
        perror("Memcpy error");
        return -1;
    }
    return 0;
}


