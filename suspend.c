#if HAVE_CONFIG_H
#  include <config.h>
#endif
#include "errno.h"
#if HAVE_SIGNAL_H
#  include <signal.h>
#endif
#if HAVE_STRING_H
#  include <string.h>
#endif
#if HAVE_UNISTD_H
#  include <unistd.h>
#endif

#include "init.h"

int do_tstp = 0;

static struct sigaction oact;

void
sigtstp(int unused)
{

	do_tstp = 1;
}

int
reg_sigtstp(void)
{
    struct sigaction act;

    act.sa_handler = sigtstp;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_RESTART;
    return sigaction(SIGTSTP, &act, &oact);
}

int
rm_sigtstp(void)
{

    return sigaction(SIGTSTP, &oact, NULL);
}

void
setfg(void)
{
    sigset_t osigset, nsigset;

    /*
     * Block the tty signals till things set correctly. 
     * Taken from util.c of csh in NetBSD.
     */
    sigemptyset(&nsigset);
    sigaddset(&nsigset, SIGTSTP);
    sigaddset(&nsigset, SIGTTIN);
    sigaddset(&nsigset, SIGTTOU);
    sigprocmask(SIG_BLOCK, &nsigset, &osigset);

    if (setpgid(0, 0))
	fatal("setpgid %s", strerror(errno));
    if (tcsetpgrp(0, getpid()))
	fatal("tcsetpgrp %s", strerror(errno));

    sigprocmask(SIG_SETMASK, &osigset, NULL);
}
