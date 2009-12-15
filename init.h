#ifndef INIT_H
#define INIT_H

void
init(int *mfd_p, int *sfd_p);

void
done(void);

volatile void
fatal(const char *fmt, ...);

#endif /* INIT_H */
