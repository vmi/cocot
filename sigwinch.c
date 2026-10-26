/*
 * Register & Remove SIGWINCH handler
 *
 * Copyright (c) 2002  IWAMURO Motonori
 * All rights reserved.
 */

#if HAVE_CONFIG_H
#  include <config.h>
#endif
#if HAVE_UNISTD_H
#  include <unistd.h>
#endif
#if HAVE_TERMIOS_H
#  include <termios.h>
#endif
#if HAVE_SIGNAL_H
#  include <signal.h>
#endif
#if HAVE_SYS_TYPES_H
#  include <sys/types.h>
#endif
#if HAVE_SYS_IOCTL_H
#  include <sys/ioctl.h>
#endif
#include <errno.h>

static struct sigaction oact;
static int master_fileno;

static void
sigwinch(int unused)
{
    struct winsize win;
    int saved_errno = errno;

    ioctl(STDIN_FILENO,  TIOCGWINSZ, &win);
    ioctl(master_fileno, TIOCSWINSZ, &win);
    errno = saved_errno;
}

int
reg_sigwinch(int fd)
{
    struct sigaction act;

    master_fileno = fd;
    act.sa_handler = sigwinch;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_RESTART;
    return sigaction(SIGWINCH, &act, &oact);
}

int
rm_sigwinch(void)
{
    struct sigaction act;
    return sigaction(SIGWINCH, &oact, &act);
}
