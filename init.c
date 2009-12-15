/*
 * Initialize & Finalize
 *
 * Copyright (c) 2002  IWAMURO Motonori
 * All rights reserved.
 */

#if HAVE_CONFIG_H
#  include <config.h>
#endif
#include <stdio.h>
#if HAVE_STDLIB_H
#  include <stdlib.h>
#endif
#include <stdarg.h>
#if HAVE_UNISTD_H
#  include <unistd.h>
#endif
#if HAVE_TERMIOS_H
#  include <termios.h>
#endif
#if HAVE_SYS_IOCTL_H
#  include <sys/ioctl.h>
#endif
#ifdef HAVE_LIBUTIL_H
#  include <libutil.h>
#endif
#if HAVE_PTY_H
#  include <pty.h>
#endif

#include "init.h"

static int initialized = 0;
static struct termios init_term;

#ifndef HAVE_CFMAKERAW

int cfmakeraw(struct termios *term_p)
{
    term_p->c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
    term_p->c_oflag &= ~OPOST;
    term_p->c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
    term_p->c_cflag &= ~(CSIZE|PARENB);
    term_p->c_cflag |= CS8;
    return 0;
}

#endif

void
init(int *mfd_p, int *sfd_p)
{
    struct termios term;
    struct winsize win;

    if (!isatty(STDIN_FILENO))
	fatal("Is not a tty.");
    if (tcgetattr(STDIN_FILENO, &term) < 0)
	fatal("tcgetattr()");
    if (ioctl(STDIN_FILENO, TIOCGWINSZ, &win) < 0)
	fatal("ioctl TIOCGWINSZ");
    if (openpty(mfd_p, sfd_p, NULL, &term, &win) < 0)
	fatal("openpty()");
    ioctl(*mfd_p, TIOCSWINSZ, &win); /* Ummm... Why don't set window size? */
    init_term = term;
    cfmakeraw(&term);
    /* term_p->c_lflag &= ~ECHO; (script.c in FreeBSD) */
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0)
	fatal("tcsetattr()");
    initialized = 1;
}

void
done(void)
{
    if (initialized)
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &init_term);
}

volatile void
fatal(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    fputs("Error: ", stderr);
    vfprintf(stderr, fmt, ap);
    fputs("\nAbort.\n", stderr);
    va_end(ap);
    done();
    exit(1);
}
