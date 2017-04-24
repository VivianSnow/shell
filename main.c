#include <elf.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <limits.h>
#include <wordexp.h>
#include <sys/fcntl.h>

#define MAXLINE (1024)


/*Have big difference with O_ACCMODE in _default_fcntl.h */
#define O_MY_ACCMODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define O_NORMALFLAG (O_WRONLY | O_CREAT | O_TRUNC)
#define O_APPENDFALG (O_WRONLY | O_CREAT | O_APPEND)

//#define SET

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

struct redirect_path {
    char redirect_input_path[PATH_MAX];
    char redirect_output_path[PATH_MAX];
    char redirect_error_path[PATH_MAX];
    int redirect_output;

    /*bit fields should more efficient and readable than mask and bitwise*/
    unsigned int redirect_input_append : 1;
    unsigned int redirect_output_append : 1;
    unsigned int redirect_error_append : 1;

};

int find_redirect_symbol(char *buf, struct redirect_path *redirect_path_ptr)
{
    int buf_N = strlen(buf); /* the len of buf */
    int i = buf_N - 1;
    int last_pos = buf_N;
    while(i) {
        switch (buf[i]) {
            case '>':
                if ((i - 1 > 0) && (buf[i - 1] == '>')) {
                    if (strncpy(redirect_path_ptr->redirect_output_path, &buf[i+1], last_pos - i) < 0){
                        perror("Error when copy redirect path");
                    }
                    --i;
                    redirect_path_ptr->redirect_output_append = 1;
                } else {
                    if (strncpy(redirect_path_ptr->redirect_output_path, &buf[i+1], last_pos - i) < 0){
                        perror("Error when copy redirect path");
                    }
                }
                if(trim(redirect_path_ptr->redirect_output_path) < 0){

                }
                redirect_path_ptr->redirect_output = 1;
                last_pos = i;
                break;
            case '<':
                if ((i - 1 > 0) && (buf[i - 1] == '<')) {
                    if (strncpy(redirect_path_ptr->redirect_input_path, &buf[i+1], last_pos - i) < 0){
                        perror("Error when copy redirect path");
                    }
                    --i;
                    redirect_path_ptr->redirect_input_append = 1;
                } else {
                    if (strncpy(redirect_path_ptr->redirect_input_path, &buf[i+1], last_pos - i) < 0){
                        perror("Error when copy redirect path");
                    }
                }
                last_pos = i;
                break;
            /* In order to make implementation easy, we use ways redirect stderr to other fd like fish shell
                ref : http://fishshell.com/docs/current/tutorial.html#tut_pipes_and_redirections */
            case '^':
                if ((i - 1 > 0) && (buf[i - 1] == '^')) {
                    if (strncpy(redirect_path_ptr->redirect_error_path, &buf[i+1], last_pos - i) < 0){
                        perror("Error when copy redirect path");
                    }
                    --i;
                    redirect_path_ptr->redirect_error_append = 1;
                } else {
                    if (strncpy(redirect_path_ptr->redirect_error_path, &buf[i+1], last_pos - i) < 0){
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

//temporarily replace with wordexp in wordexp.h
int split_buffer2token(char *buffer, char **argvs)
{


}

int main(int argc, char *argv[])
{
    char cmd[MAXLINE];
    char cmd_cont[MAXLINE];
    pid_t pid;
    int status;

    printf("%% ");
    while (fgets(cmd, MAXLINE, stdin) != NULL) {
        if (cmd[strlen(cmd) - 1] == '\n') {
            cmd[strlen(cmd) - 1] = '\0';
        }
        while (cmd[strlen(cmd) - 1] == '\\') {
            //fflush(stdout);
            printf("> ");
            cmd[strlen(cmd) - 1] = '\0';
            if(fgets(cmd_cont, MAXLINE, stdin) != NULL) {
                if (cmd_cont[strlen(cmd_cont) - 1] == '\n') {
                    cmd_cont[strlen(cmd_cont) - 1] = '\0';
                }
                strcat(cmd, cmd_cont);
            } else{
                break;
            }
        }


        //Todo: Using Finite-state machine spilit
        puts(cmd);


        if ((pid = fork()) < 0) {
            perror("fork error:");
        } else if (pid == 0) {
            struct redirect_path *redirect_path_ptr = calloc(1, sizeof(struct redirect_path));
            find_redirect_symbol(cmd, redirect_path_ptr);
            wordexp_t p;
            int i;
            if (wordexp(cmd, &p, 0) < 0)
            {
                perror("Error when split args");
            }
            for (i = 0; i < p.we_wordc; i++)
                printf("%s\n", p.we_wordv[i]);
            if (redirect_path_ptr->redirect_output) {
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
                printf("%d\n", fd);
                if(dup2(fd, STDOUT_FILENO) <0) {
                    perror("Dup error");
                }
            }
            execvp(p.we_wordv[0], p.we_wordv);
            wordfree(&p);
            exit(127);
        }


        // parent
        //Todo redirect input、output and error
        if (pid > 0) {
            if (pid == waitpid(pid, &status, 0) < 0) {
                perror("waitpid error");
            }
            printf("%% ");
        }
    }
    return 0;
}