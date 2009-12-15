/*
 * Register & Remove SIGWINCH handler
 *
 * Copyright (c) 2002  IWAMURO Motonori
 * All rights reserved.
 */

#ifndef SIGWINCH_H
#define SIGWINCH_H

int
reg_sigwinch(int fd);

int
rm_sigwinch(void);

#endif /* SIGWINCH_H */
