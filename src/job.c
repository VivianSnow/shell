//
// Created by VivianSnow on 2017/5/27.
// Last reviewed by VivianSnow on 2017/6/8
//

#include <string.h>
#include "job.h"
#include <stdio.h>
#include <sys/signal.h>
#include <wordexp.h>
#include <stdlib.h>
#include <limits.h>
#include <wait.h>
#include <errno.h>


void jobs_init()
{
    jobs.jobs_next_no = 0;
    jobs.jobs_alive_no = 0;
    /* Put ourselves in our own process group.  */
    shell_pgid = getpid();
    shell_terminal = STDIN_FILENO;

    if (setpgid (shell_pgid, shell_pgid) < 0)
    {
        perror("Couldn't put the shell in its own process group");
        exit (1);
    }

    /* Grab control of the terminal.  */
    if(tcsetpgrp (shell_terminal, shell_pgid) < 0) {
        perror("tcsetpgrp");
    }

    /* Save default terminal attributes for shell.  */
    if(tcgetattr (shell_terminal, &shell_tmodes) < 0) {
        perror("tcsetpgrp");
    }
}


int add_jobs(pid_t pid, const char *cmd)
{
    int job_no = jobs.jobs_next_no;
    strcpy(jobs.jobs_list[job_no].job_cmd, cmd);
    jobs.jobs_list[job_no].job_no = job_no;
    jobs.jobs_list[job_no].job_pgid = pid;
    jobs.jobs_list[job_no].job_stat = JOB_RUNNING;
    jobs.jobs_alive_no++;
    jobs.jobs_next_no++;
    return job_no;
}


static void format_job_info (int no)
{
    printf("[%d%-10c ", jobs.jobs_list[no].job_no, ']');
    switch (jobs.jobs_list[no].job_stat) {
        case JOB_STOPED:
            printf("%-10s", "STOPED");
            break;
        case JOB_DONE:
            printf("%-10s", "");
            break;
        case JOB_RUNNING:
            printf("%-10s", "RUNNING");
            break;
        default:
            printf("%-10s", "UNKNOW");
            break;
    }
    puts(jobs.jobs_list[no].job_cmd);
}


void jobs_command()
{
    int i;
    for (i = 0; i < jobs.jobs_next_no; i++) {
        if (jobs.jobs_list[i].job_stat != JOB_DONE) {
            format_job_info(i);
        }
    }
}


int mark_process_status (pid_t pid, int status)
{
    int i;

    if (pid > 0)
    {
        /* Update the record for the process.  */
        for(i=0; i<jobs.jobs_next_no; i++)
            if (jobs.jobs_list[i].job_pgid == pid)
            {
                if (WIFSTOPPED (status))
                    jobs.jobs_list[i].job_stat = JOB_STOPED;
                else {
                    jobs.jobs_list[i].job_stat = JOB_DONE;
                    jobs.jobs_alive_no -= 1;
                    if (jobs.jobs_alive_no == 0) {
                        jobs.jobs_next_no = 0;
                    }
                }
                return 0;
            }
        return -1;
    }
    else if (pid == 0 || errno == ECHILD) {
        /* No processes ready to report.  */
        return -1;
    }
    else {
        /* Other weird errors.  */
        perror ("waitpid");
        return -1;
    }
}


void refresh_all_job_status()
{
    int i;
    int status;
    pid_t pid;
    for (i = 0; i < jobs.jobs_next_no; i++) {
        pid = waitpid(jobs.jobs_list[i].job_pgid, &status, WNOHANG | WUNTRACED);
        JOB_STATUS_T status_before = jobs.jobs_list[i].job_stat;
        mark_process_status(pid, status);
        if (jobs.jobs_list[i].job_stat != status_before) {
            format_job_info(i);
        }
    }
}


void wait_for_job (int no)
{
    int status;
    pid_t pid;

    do {
        pid = waitpid(WAIT_ANY, &status, WUNTRACED);
    } while (!mark_process_status(pid, status)
             && jobs.jobs_list[no].job_stat == JOB_RUNNING);
    //wait until JOB's status is not RUNNING
}


void put_job_in_background (int no)
{
    /* Send the job a continue signal, if necessary.  */
    if (jobs.jobs_list[no].job_stat == JOB_STOPED) {
        if (kill (- jobs.jobs_list[no].job_pgid, SIGCONT) < 0) {
            perror ("kill (SIGCONT)");
        }
        jobs.jobs_list[no].job_stat = JOB_RUNNING;
    }
}


int bg_command(const char* cmd)
{
    wordexp_t p;
    if (wordexp(cmd, &p, 0) < 0)
    {
        goto error_return;
    }

    // "+" and "-" are not support in our shell now
    if (p.we_wordc < 2) {
        goto print_help;
    }

    else if (p.we_wordc == 2) {
        if((strcmp(p.we_wordv[1], "-help") == 0))
            goto print_help;
        long no;
        char **endprt = NULL;
        no = strtol(p.we_wordv[1], endprt, 10);
        //atoi cannot handle convert error

        if (endprt || no < 0 || no > INT_MAX) {
            goto cannot_resolve_number;
        } else if (no < 0 || no >= jobs.jobs_next_no || jobs.jobs_list[no].job_stat == JOB_DONE) {
            printf("-shell: bg: %d: no such job\n", (int)no);
            wordfree(&p);
            return -3;
        } else {
            put_job_in_background((int) no);
        }
    } else goto print_help;

    wordfree(&p);
    return 0;

    /*
     * goto
     * */
    print_help:
        wordfree(&p);
        printf("bg: usage: bg [job_spec ...]\n");
        return 0;

    cannot_resolve_number:
        fprintf(stderr, "cannot resolve number \"%s\"\n", p.we_wordv[1]);
        wordfree(&p);
        return -1;

    error_return:
        wordfree(&p);
        perror("some error happen");
        return -2;
}

void put_job_in_foreground(int no)
{

    /* Put the job into the foreground.  */
    if (tcsetpgrp(shell_terminal, jobs.jobs_list[no].job_pgid) < 0) {
        perror("tcsetpgrp");
    }

    /* Send the job a continue signal, if necessary.  */
    if (jobs.jobs_list[no].job_stat == JOB_STOPED)
    {
        tcsetattr (shell_terminal, TCSADRAIN, &jobs.jobs_list[no].job_tmodes);
        if (kill (- jobs.jobs_list[no].job_pgid, SIGCONT) < 0)
            perror ("kill (SIGCONT)");
        jobs.jobs_list[no].job_stat = JOB_RUNNING;
    }

    /* Wait for it to report.  */
    wait_for_job(no);

    /* Put the shell back in the foreground.  */
    tcsetpgrp (shell_terminal, shell_pgid);

    /* Restore the shell’s terminal modes.  */
    tcgetattr (shell_terminal, &jobs.jobs_list[no].job_tmodes);
    tcsetattr (shell_terminal, TCSADRAIN, &shell_tmodes);

}

int fg_command(const char* cmd)
{
    wordexp_t p;
    if (wordexp(cmd, &p, 0) < 0)
    {
        goto error_return;
    }

    // "+" and "-" are not support in our shell now
    if (p.we_wordc < 2) {
        goto print_help;
    }

    else if (p.we_wordc == 2) {
        if((strcmp(p.we_wordv[1], "-help") == 0))
            goto print_help;

        long no;
        char **endprt = NULL;
        no = strtol(p.we_wordv[1], endprt, 10);

        if (endprt || no > INT_MAX) {
            goto cannot_resolve_number;

        }else if (no < 0 || no >= jobs.jobs_next_no || jobs.jobs_list[no].job_stat == JOB_DONE) {
            wordfree(&p);
            printf("-shell: bg: %d: no such job", ((int) no));
            return -3;

        }else{
            put_job_in_foreground((int)no);

        }
    } else goto print_help;

    wordfree(&p);
    return 0;

    /*
     * goto
     * */
    print_help:
    wordfree(&p);
    printf("fg: usage: fg [job_spec ...]\n");
    return 0;

    cannot_resolve_number:
    fprintf(stderr, "cannot resolve number \"%s\"\n", p.we_wordv[1]);
    wordfree(&p);
    return -1;

    error_return:
    wordfree(&p);
    perror("some error happen");
    return -3;
}


