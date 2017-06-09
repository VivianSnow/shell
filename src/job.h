//
// Created by VivianSnow on 2017/5/27.
//

#include <sys/types.h>
#include <termio.h>
#include <unistd.h>

#ifndef SHELL_JOB_H
#define SHELL_JOB_H


#define MAXLINE (1024)
#define MAX_JOB (256)

#define JOB_STATUS_T unsigned int
#define JOB_RUNNING (0)
#define JOB_STOPED (1)
#define JOB_DONE (2)

struct job {
    //In order to avoid problem cause by convert from unsigned to singed, we use int type
    int job_no; /* command no, used for messages */
    char job_cmd[MAXLINE]; /* command line, used for messages */
    JOB_STATUS_T job_stat; /*job status, RUNING,STOPED or DONE */
    pid_t job_pgid;         /* process group ID */
    struct termios job_tmodes;      /* saved terminal modes */
};

struct jobs{
    int jobs_next_no;
    int jobs_alive_no;
    struct job jobs_list[MAX_JOB*4];
};

struct jobs jobs;

pid_t shell_pgid;
int shell_terminal;
struct termios shell_tmodes;

void jobs_init();

int add_jobs(pid_t pid, const char *cmd);

void refresh_all_job_status();

void jobs_command();

int bg_command(const char* cmd);

int fg_command(const char* cmd);

void put_job_in_foreground(int no);

void put_job_in_background (int no);

#endif //SHELL_JOB_H
