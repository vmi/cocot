/*
 * cocot - COde COnverter on Tty
 *
 * Copyright (c) 2002, 2004, 2005, 2008  IWAMURO Motonori
 * All rights reserved.
 */

#if HAVE_CONFIG_H
#  include <config.h>
#endif
#include <stdio.h>
#if HAVE_STDLIB_H
#  include <stdlib.h>
#endif
#if HAVE_UNISTD_H
#  include <unistd.h>
#endif
#if HAVE_UTMP_H
#  include <utmp.h>
#endif
#if HAVE_STRING_H
#  include <string.h>
#endif
#include <errno.h>
#if HAVE_SYS_TYPES_H
#  include <sys/types.h>
#endif
#if HAVE_SYS_WAIT_H
#  include <sys/wait.h>
#endif
#if !defined(HAVE_LOGIN_TTY) && defined(HAVE_TERMIOS_H)
#  include <termios.h>
#endif
#include <getopt.h>

#include "init.h"
#include "loop.h"

#if DEBUG
#define DEBUG_LOG "debug.log"
FILE *debug = NULL;
#endif

static void
show_version(void)
{
    fprintf(stderr, "%s, version %s\n", PACKAGE_NAME, PACKAGE_VERSION);
    exit(1);
}

volatile static void
usage(int argc, char *argv[])
{
    fprintf(stderr,
	    "Usage: %s [OPTIONS] [--] COMMAND ARG1 ARG2 ...\n"
	    "\n"
	    "Options:\n"
	    "    -o LOGFILE     logging all output of command.\n"
	    "    -a             append log file.\n"
	    "    -t TERM_CODE\n"
	    "    -t TERM_INPUT_CODE,TERM_OUTPUT_CODE\n"
            "                   character encoding(s) for terminal. (default is %s)\n"
	    "    -p PROC_CODE\n"
	    "    -p PROC_INPUT_CODE,PROC_OUTPUT_CODE\n"
	    "                   character encoding(s) in command process. (default is %s)\n"
	    "    -i             ignore ISO-2022-JP escape sequence.\n"
	    "    -n             no conversion. (like script(1))\n"
	    "    -h, --help     show this message.\n"
	    "    -v, --version  show version.\n",
	    argv[0],
	    DEFAULT_TERM_CODE,
	    DEFAULT_PROC_CODE);
    exit(1);
}

int
main(int argc, char *argv[])
{
    int i;
    int c;
    char *logfn = NULL;
    char *logmd = "w";
    FILE *logfp = NULL;
    char *term_input_code  = DEFAULT_TERM_CODE;
    char *term_output_code = DEFAULT_TERM_CODE;
    char *proc_input_code  = DEFAULT_PROC_CODE;
    char *proc_output_code = DEFAULT_PROC_CODE;
    char *p;
    int dec_jis = 1;

    int mfd, sfd;
    int status;
    pid_t pid;

    if (argc == 1)
	usage(argc, argv);
    for (i = 1; i < argc && argv[i][0] == '-'; i++) {
	if (strcmp(argv[i], "--help") == 0)
	    argv[i] = "-h";
	else if (strcmp(argv[i], "--version") == 0)
	    argv[i] = "-v";
    }
    while ((c = getopt(argc, argv, "ao:t:p:inhv")) != -1) {
	switch (c) {
	case 'a':
	    logmd = "a";
	    break;
	case 'o':
	    logfn = optarg;
	    break;
	case 't':
	    if ((p = strchr(optarg, ',')) != NULL) {
		term_input_code = optarg;
		*p++ = '\0';
		term_output_code = p;
	    } else {
		term_input_code = term_output_code = optarg;
	    }
	    break;
	case 'p':
	    if ((p = strchr(optarg, ',')) != NULL) {
		proc_input_code = optarg;
		*p++ = '\0';
		proc_output_code = p;
	    } else {
		proc_input_code = proc_output_code = optarg;
	    }
	    break;
	case 'i':
	    dec_jis = 0;
	    break;
	case 'n':
	    term_input_code = term_output_code =
	    proc_input_code = proc_output_code = NULL;
	    break;
	case 'h':
	    usage(argc, argv);
	    break;
	case 'v':
	    show_version();
	    break;
	}
    }
    if (logfn) {
	if ((logfp = fopen(logfn, logmd)) == NULL)
	    fatal("Can't open file '%s' (%s).", logfn, strerror(errno));
	setvbuf(logfp, NULL, _IONBF, 0);
    }
    init(&mfd, &sfd);
    if ((pid = fork()) < 0) {
	/* error */
	fatal("Can't fork process");
    } else if (pid == 0) {
	/* child */
	close(mfd);
	if (logfp)
	    fclose(logfp);
#ifdef HAVE_LOGIN_TTY
	login_tty(sfd);
#else
	setsid();
	ioctl(sfd, TIOCSCTTY, 0);
	dup2(sfd, 0);
	dup2(sfd, 1);
	dup2(sfd, 2);
	if (sfd > 2)
	    close(sfd);
#endif
	execvp(argv[optind], argv + optind);
	fatal("Can't exec process");
    }
    /* parent */
    close(sfd);
#ifdef DEBUG
    if ((debug = fopen(DEBUG_LOG, "a")) == NULL)
	fatal("Can't open file '%s' (%s).", DEBUG_LOG, strerror(errno));
    setvbuf(debug, NULL, _IONBF, 0);
#endif
    loop(mfd, logfp,
	 term_input_code, term_output_code,
	 proc_input_code, proc_output_code, dec_jis);
    close(mfd);
    if (logfp)
	fclose(logfp);
    wait(&status);
    done();
    return 0;
}
