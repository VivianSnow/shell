//
// Created by VivianSnow on 2017/5/27.
// Last reviewed by VivianSnow on 2017/6/8
//

#include "signal_handle.h"
#include <signal.h>

void shell_signal_init()
{
    /* Ignore interactive and job-control signals except SIGCHLD.  */
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGCHLD, SIG_DFL);
}


void job_signal_init()
{
    /* Set the handling for job control signals back to the default.  */
    signal (SIGINT, SIG_DFL);
    signal (SIGQUIT, SIG_DFL);
    signal (SIGTSTP, SIG_DFL);
    signal (SIGTTIN, SIG_DFL);
    signal (SIGTTOU, SIG_DFL);
    signal (SIGCHLD, SIG_DFL);
}


