/*
 * Main Loop
 *
 * Copyright (c) 2002  IWAMURO Motonori
 * All rights reserved.
 */

#ifndef LOOP_H
#define LOOP_H

#include <stdio.h>

void
loop(int mfd, FILE *fp,
     char *term_input_code, char *term_output_code,
     char *proc_input_code, char *proc_output_code,
     int dec_jis);

#endif /* LOOP_H */
