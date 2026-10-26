#include <signal.h>

extern volatile sig_atomic_t do_tstp;

void sigtstp(int);

int reg_sigtstp(void);

int rm_sigtstp(void);

void do_suspend(void);

void setfg(void);
