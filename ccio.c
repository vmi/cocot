/*
 * I/O with Code Convertion
 *
 * Copyright (c) 2002, 2004, 2005  IWAMURO Motonori
 * All rights reserved.
 */

#if HAVE_CONFIG_H
#  include <config.h>
#endif
#if HAVE_STRING_H
#  include <string.h>
#endif
#if HAVE_UNISTD_H
#  include <unistd.h>
#endif
#include <errno.h>

#include "l10n_ja.h"
#include "l10n_cjk_uni.h"
#include "ccio.h"

#define striEQ(s1, s2) (!strcasecmp((s1), (s2)))

static size_t
ccio_noconv(void *noconv,
	    const char **spp, size_t *scp,
	    char **dpp, size_t *dcp)
{
    size_t size;
    size = *scp <= *dcp ? *scp : *dcp;
    memcpy(*dpp, *spp, size);
    *spp += size, *scp -= size;
    *dpp += size, *dcp -= size;
    return 0;
}

static int
ccio_noconv_destroy(void *noconv)
{
    return 0;
}

static int
ccio_skip(const char **spp, size_t *scp)
{
    (*spp)++;
    (*scp)--;
    return 1;
}

CCIO_STATUS
ccio_init(CCIO *c, char *tocode, char *fromcode, int dec_jis)
{
    memset(c, 0, sizeof(CCIO));

    if (tocode == NULL && fromcode == NULL) {
	c->conv = ccio_noconv;
	c->skip = ccio_skip; /* don't use */
	c->destroy = ccio_noconv_destroy;
    } else if ((striEQ(fromcode, "Shift_JIS")
	 || striEQ(fromcode, "CP932"))
	&& striEQ(tocode, "EUC-JP")) {
	if ((c->handle = l10n_ja_open(dec_jis)) == NULL)
	    return CCIO_ERROR;
	c->conv = l10n_ja_sjis2eucj;
	c->skip = ccio_skip;
	c->destroy = l10n_ja_close;
    } else if (striEQ(fromcode, "EUC-JP")
	       && (striEQ(tocode, "Shift_JIS")
		   || striEQ(tocode, "CP932"))) {
	if ((c->handle = l10n_ja_open(dec_jis)) == NULL)
	    return CCIO_ERROR;
	c->conv = l10n_ja_eucj2sjis;
	c->skip = ccio_skip;
	c->destroy = l10n_ja_close;
    } else {
	if ((c->handle = iconv_open(tocode, fromcode)) == (iconv_t) (-1))
	    return CCIO_ERROR;
	c->conv = (CCIO_CONV) iconv;
	if (striEQ(fromcode, "UTF-8") && !striEQ(tocode, "UTF-8"))
	    c->skip = l10n_cjk_uni_skip;
	else
	    c->skip = ccio_skip;
	c->destroy = iconv_close;
    }
    return CCIO_SUCCESS;
}

void
ccio_done(CCIO *c)
{
    c->destroy(c->handle);
}

CCIO_STATUS
ccio_read(CCIO *c, int fd, FILE *fp)
{
    int n;

    if ((n = read(fd, c->buf + c->len, sizeof(c->buf) - c->len)) <= 0)
	return n == 0 ? CCIO_EOF : CCIO_ERROR;
    if (fp)
	fwrite(c->buf + c->len, 1, n, fp);
    c->len += n;
    return CCIO_SUCCESS;
}

static CCIO_STATUS
writen(int fd, char *buf, int cnt)
{
    int n;

    while (cnt > 0) {
	if ((n = write(fd, buf, cnt)) < 0)
	    return CCIO_ERROR;
	buf += n;
	cnt -= n;
    }
    return CCIO_SUCCESS;
}

CCIO_STATUS
ccio_write(CCIO *c, int fd)
{
    size_t icnt, ocnt, rv, n;
    const char *ibuf;
    char *obuf;
    char buf[CCIO_BUF_SIZE * 2];

    ibuf = c->buf;
    icnt = c->len;
    while (icnt > 0) {
	obuf = buf;
	ocnt = sizeof(buf);
	rv = c->conv(c->handle, &ibuf, &icnt, &obuf, &ocnt);
	if ((n = sizeof(buf) - ocnt) > 0) {
	    if (writen(fd, buf, n) == CCIO_ERROR)
		return CCIO_ERROR;
	}
	if (rv != -1) {
	    c->len = 0;
	    break;
	}
	switch (errno) {
	case EINVAL:
	    memmove(c->buf, ibuf, icnt);
	    c->len = icnt;
	    goto loopout;
	case EILSEQ:
	    n = c->skip(&ibuf, &icnt);
	    do {
		write(fd, "#", 1); /* Ummm... */
	    } while (--n > 0);
	    break;
	default: /* E2BIG */
	    break;
	}
    }
loopout:
    return CCIO_SUCCESS;
}
