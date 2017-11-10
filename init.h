#ifndef INIT_H
#define INIT_H

struct termios;
struct winsize;

void
init(int *mfd_p, int *sfd_p);

void
init_tty(int mfd, struct termios *term_p, struct winsize *win_p);

void
reset_tty(void);

void
fatal(const char *fmt, ...);

#endif /* INIT_H */
