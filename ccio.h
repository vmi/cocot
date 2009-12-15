/*
 * I/O with Code Convertion
 *
 * Copyright (c) 2002, 2004  IWAMURO Motonori
 * All rights reserved.
 */

#ifndef CCIO_H
#define CCIO_H

#include <stdio.h>
#if HAVE_ICONV_H
#  include <iconv.h>
#endif

typedef enum CCIO_STATUS {
    CCIO_ERROR   = -1,
    CCIO_SUCCESS =  0,
    CCIO_EOF     =  1,
} CCIO_STATUS;

/* 2^12 = 4096 */
#define CCIO_BUF_SIZE (1 << 12)

typedef size_t (*CCIO_CONV)(void *,
			    const char **, size_t *,
			    char **, size_t *);

typedef int (*CCIO_SKIP)(const char **, size_t *);

typedef int (*CCIO_DESTROY)(void *);

typedef struct CCIO {
    void *handle;
    CCIO_CONV conv;
    CCIO_SKIP skip;
    CCIO_DESTROY destroy;
    iconv_t cd;
    size_t len;
    char buf[CCIO_BUF_SIZE];
} CCIO;

CCIO_STATUS
ccio_init(CCIO *c, char *tocode, char *fromcode, int dec_jis);

void
ccio_done(CCIO *c);

CCIO_STATUS
ccio_read(CCIO *c, int fd, FILE *fp);

CCIO_STATUS
ccio_write(CCIO *c, int fd);

#endif /* CCIO_H */
