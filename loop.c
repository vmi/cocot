/*
 * Main Loop
 *
 * Copyright (c) 2002, 2004  IWAMURO Motonori
 * All rights reserved.
 */

#if HAVE_CONFIG_H
#  include <config.h>
#endif
#if HAVE_UNISTD_H
#  include <unistd.h>
#endif
#if HAVE_SYS_TYPES_H
#  include <sys/types.h>
#endif
#if HAVE_SYS_TIME_H
#  include <sys/time.h>
#endif
#if HAVE_STRING_H
#  include <string.h>
#endif
#if HAVE_SIGNAL_H
#  include <signal.h>
#endif
#include <errno.h>

#include "init.h"
#include "sigwinch.h"
#include "ccio.h"
#include "suspend.h"

#ifndef HAVE_MAX
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif

void
loop(int mfd, FILE *fp,
     char *term_input_code, char *term_output_code,
     char *proc_input_code, char *proc_output_code,
     int dec_jis)
{
    CCIO master, slave;
    fd_set fds;
    int fdmax = max(STDIN_FILENO, STDOUT_FILENO) + 1;
    if (fdmax < mfd)
	fdmax = mfd + 1;
    if (ccio_init(&master, proc_input_code, term_input_code, 0) == CCIO_ERROR ||
	ccio_init(&slave, term_output_code, proc_output_code, dec_jis) == CCIO_ERROR)
	fatal("%s: TERM_CODE(%s,%s) and/or PROC_CODE(%s,%s) is invalid.",
	      strerror(errno),
	      term_input_code, term_output_code,
	      proc_input_code, proc_output_code);
    reg_sigwinch(mfd);
    reg_sigtstp();
    while (1) {
	if (do_tstp) {
	    do_tstp = 0;
    	    rm_sigtstp();
	    reset_tty();
	    kill(0, SIGTSTP);
	    init_tty(mfd, NULL, NULL);
    	    reg_sigtstp();
	}
	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds);
	FD_SET(mfd, &fds);
	if (select(fdmax, &fds, NULL, NULL, NULL) <= 0)
	    continue;
	if (FD_ISSET(STDIN_FILENO, &fds))
	    if (ccio_read(&master, STDIN_FILENO, NULL) != CCIO_SUCCESS)
		break;
	if (ccio_write(&master, mfd) != CCIO_SUCCESS)
	    break;
	if (FD_ISSET(mfd, &fds))
	    if (ccio_read(&slave, mfd, fp) != CCIO_SUCCESS)
		break;
	if (ccio_write(&slave, STDOUT_FILENO) == CCIO_ERROR)
	    break;
    }
    rm_sigtstp();
    rm_sigwinch();
    ccio_done(&master);
    ccio_done(&slave);
}
