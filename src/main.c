#include <elf.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <wordexp.h>
#include <errno.h>

#include "export.h"
#include "readline_and_history.h"
#include "job.h"
#include "signal_handle.h"
#include "redirect.h"

char prompt_string[50] = "shell>";

int main(int argc, char *argv[])
{
    char **cmds = NULL;
    register char *cmd = NULL;
    register char *buf = NULL;
    pid_t pid;
    int is_background = 0;

    shell_signal_init();
    history_init();
    jobs_init();

    printf("Note: Welcome to ezsh!\n");

    while ( (buf = read_cmd_line() ) ) {

        refresh_all_job_status();
        cmds = get_cmd(buf);

        while( (cmd = *cmds) != NULL)
        {
            cmds++;

            //builtin command

            if(strncasecmp(cmd, "exit", 4) == 0) {
                exit(0);
            }

            if (strncmp(cmd, "!", 1) == 0) {
                char *temp = NULL;
                if( (temp = history_repeat(cmd)) == NULL) {
                    continue;
                } else {
                    cmds--;
                    free(*cmds);
                    *cmds = malloc(sizeof(char) * (strlen(temp) + 2));
                    strcpy(*cmds, temp);
                    continue;
                }
            }

            if(strncmp(cmd, "export", 6) == 0) {
                export_command(cmd);
                continue;
            }

            if (strncmp(cmd, "cd", 2) == 0) {
                fprintf(stderr, "cd command is not supported now");
                continue;
            }

            //TODO add source command
            if (strncmp(cmd, "source", 6) == 0) {
                //In fact, I have no idea how to run a script now .....
                fprintf(stderr, "source command is not supported now");
                continue;
            }

            if (strncmp(cmd, "fg", 2) == 0) {
                fg_command(cmd);
                continue;
            }

            if (strncmp(cmd, "bg", 2) == 0) {
                bg_command(cmd);
                continue;
            }

            if (strncmp(cmd, "jobs", 4) == 0) {
                jobs_command();
                continue;
            }

            if (strncmp(cmd, "history", 7) == 0) {
                history_command();
                continue;
            }

            if (cmd[strlen(cmd) - 1] == '&') {
                is_background = 1;
                cmd[strlen(cmd) - 1] = '\0';
            } else{
                is_background = 0;
            }

            if ((pid = fork()) < 0) {
                perror("fork error:");
            } else if (pid == 0) { //child process

                job_signal_init();

                struct redirect_path *redirect_path_ptr = (struct redirect_path*)calloc(1, sizeof(struct redirect_path));
                find_redirect_symbol(cmd, redirect_path_ptr);
                redirect(redirect_path_ptr);

                wordexp_t p;
                if (wordexp(cmd, &p, 0) < 0)
                {
                    perror("Error when split args");
                }

                //if command cannot be resolved, exit child process
                if (p.we_wordc == 0) {
                    /* HINT: man 3 wordexp saids cmd should not include some characters
                     * although I have handle most of them, some characters like (,),{,} and some situation like "ls & -l" still can not handle
                     * -------------------------------------------------------------------------------
                     * Since the expansion is the same as the expansion by the shell (see sh(1)) of the parameters to a command, the string s must not contain characters that would be illegal in shell command parameters.
                     * In particular, there must not be any unescaped newline or |, &, ;, <, >, (, ), {, } characters outside a command substitution or parameter substitution context.
                     * -------------------------------------------------------------------------------
                     */
                    fprintf(stderr, "shell: %s : cannot be resolved\n", cmd);
                    return (-1);
                }

                setpgid(getpid(), getpid());

                if(!is_background) {
                    if (tcsetpgrp(shell_terminal, getpid()) < 0) {
                        perror("tcsetpgrp");
                    }
                } else{
                    if (tcsetpgrp(shell_terminal, shell_pgid) < 0) {
                        perror("tcsetpgrp");
                    }
                }

                if(execvp(p.we_wordv[0], p.we_wordv) < 0){}
                {
                    fprintf(stderr, "shell: %s : %s\n", p.we_wordv[0], strerror(errno));
                }
                wordfree(&p); //don't forget to free when exec failed
                free(redirect_path_ptr);
                redirect_path_ptr = NULL;
                exit(127);
            }


            // parent
            //Todo redirect input、output and error
            if (pid > 0) {
                if (is_background) {
                    //record in list of background jobs
                    int job_no = add_jobs(pid, cmd);
                    put_job_in_background(job_no);

                    if (tcsetpgrp(shell_terminal, getpid()) < 0) {
                        perror("tcsetpgrp");
                    }
                } else {
                    int job_no = add_jobs(pid, cmd);
                    put_job_in_foreground(job_no);
                }
            }
        } //END OF while( (cmd = *cmds) != NULL)
    } // END OF while ( buf = read_cmd_line() )
    return 0;
}