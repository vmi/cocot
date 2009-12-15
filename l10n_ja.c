/*
 * Localization for Japanese Language
 *
 * Copyright (c) 2002  IWAMURO Motonori
 * All rights reserved.
 */

#if HAVE_CONFIG_H
#  include <config.h>
#endif
#if HAVE_STDLIB_H
#  include <stdlib.h>
#endif
#if HAVE_STRING_H
#  include <string.h>
#endif
#include <iconv.h>
#include <errno.h>
#include "l10n_ja.h"

#define WRITE_BACK(back) \
    (*spp = (sp - (back)), *scp = sc, *dpp = dp, *dcp = dc)

#define JA_REF(p, m) (((L10N_JA *) (p))->m)

#define ST_NO_DECODE -1
#define ST_ASCII      0
#define ST_JISX0208   1
#define ST_JISX0201   2

typedef struct L10N_JA {
    int jis;
    char nc_a;
    char nc_s[2];
    char nc_e[2];
} L10N_JA;

void *
l10n_ja_open(int dec_jis)
{
    L10N_JA *ja;

    if ((ja = malloc(sizeof(L10N_JA))) == NULL)
	return (void *) -1;
    ja->jis = dec_jis ? ST_ASCII : ST_NO_DECODE;
    ja->nc_s[0] = 0x81; /*g h*/
    ja->nc_s[1] = 0xa0;
    ja->nc_e[0] = 0xa2;
    ja->nc_e[1] = 0xa2;
    return ja;
}

int
l10n_ja_close(void *ja)
{
    memset(ja, 0, sizeof(L10N_JA));
    free(ja);
    return 0;
}

size_t
l10n_ja_eucj2sjis(void *ja,
		  const char **spp, size_t *scp,
		  char **dpp, size_t *dcp)
{
    const unsigned char *sp;
    unsigned char *dp;
    int sc, dc;
    int jis;
    int c, d;
    int nc_cnt = 0; /* •ÏŠ·Œ³‚É‚ ‚Á‚Ä•ÏŠ·æ‚É‚È‚¢•¶Žš‚Ì” */

    sp = *spp;
    sc = *scp;
    dp = *dpp;
    dc = *dcp;
    jis = JA_REF(ja, jis);
    d = 0; /* avoid compiler warning */

    while (dc > 0 && sc > 0) {
	c = *sp++;
	switch (jis) {
	case ST_JISX0208:
	    if (sc == 1) {
		WRITE_BACK(1);
		errno = EINVAL;
		return -1;
	    }
	    d = *sp;
	    if (0x21 <= c && c <= 0x7e && 0x21 <= d && d <= 0x7e) {
		c |= 0x80;
		d |= 0x80;
	    }
	    break;

	case ST_JISX0201:
	    if (0x21 <= c && c <= 0x5f) {
		*dp++ = c | 0x80;
		sc--;
		dc--;
		continue;
	    }
	    break;

	default:
	    break;
	}
	if (c == 0x1b && jis != ST_NO_DECODE) { /* Escape Sequence */
	    int esc = 0;
	    if (sc <= 2) {
		WRITE_BACK(1);
		errno = EINVAL;
		return -1;
	    }
	    if (sp[0] == '$') {
		if (sp[1] == 'B' || sp[1] == '@') { /* to JIS X 0208 */
		    esc = 1;
		    jis = ST_JISX0208;
		}
	    } else if (sp[0] == '(') {
		if (sp[1] == 'B' || sp[1] == 'J') { /* to ASCII & EUC-JP */
		    if (jis != ST_ASCII) {
			esc = 1;
			jis = ST_ASCII;
		    }
		} else if (sp[1] == 'I') { /* to JIS X 0201 RH */
		    esc = 1;
		    jis = ST_JISX0201;
		}
	    }
	    if (esc) {
		JA_REF(ja, jis) = jis;
		sp += 2;
		sc -= 3;
	    } else {
		*dp++ = c;
		sc--;
		dc--;
	    }
	} else if (c <= 0x7f) { /* ASCII */
	    *dp++ = c;
	    sc--;
	    dc--;
	} else if (0xa0 <= c && c <= 0xfe) { /* JIS X 0208 */
	    if (sc == 1) {
		WRITE_BACK(1);
		errno = EINVAL;
		return -1;
	    } else if (dc == 1) {
		WRITE_BACK(1);
		errno = E2BIG;
		return -1;
	    }
	    if (jis == ST_JISX0208)
		sp++;
	    else
		d = *sp++;
	    if (d < 0xa0 || d == 0xff) {
		WRITE_BACK(2);
		errno = EILSEQ;
		return -1;
	    }
	    *dp++ = ((c - 1) >> 1) + ((c <= 0xde) ? 0x31 : 0x71);
	    *dp++ = d - ((c & 1) ? ((d < 0xe0) ? 0x61 : 0x60) : 0x02);
	    sc -= 2;
	    dc -= 2;
	} else if (c == 0x8e) { /* JIS X 0201 RH */
	    if (sc == 1) {
		WRITE_BACK(1);
		errno = EINVAL;
		return -1;
	    }
	    d = *sp++;
	    if (d < 0xa1 || 0xdf < d) {
		WRITE_BACK(2);
		errno = EILSEQ;
		return -1;
	    }
	    *dp++ = d;
	    sc -= 2;
	    dc--;
	} else if (c == 0x8f) { /* JIS X 0212 */
	    if (sc <= 2) {
		WRITE_BACK(1);
		errno = EINVAL;
		return -1;
	    } else if (dc == 1) {
		WRITE_BACK(1);
		errno = E2BIG;
		return -1;
	    }
	    *dp++ = JA_REF(ja, nc_s[0]);
	    *dp++ = JA_REF(ja, nc_s[1]);
	    sc -= 3;
	    dc -= 2;
	    nc_cnt++;
	} else {
	    WRITE_BACK(1);
	    errno = EILSEQ;
	    return -1;
	}
    }
    WRITE_BACK(0);
    if (sc) {
	errno = E2BIG;
	return -1;
    }
    return nc_cnt;
}

size_t
l10n_ja_sjis2eucj(void *ja,
		  const char **spp, size_t *scp,
		  char **dpp, size_t *dcp)
{
    const unsigned char *sp;
    unsigned char *dp;
    int sc, dc;
    int jis;
    int c, d;
    int nc_cnt = 0;

    sp = *spp;
    sc = *scp;
    dp = *dpp;
    dc = *dcp;
    jis = JA_REF(ja, jis);

    while (dc > 0 && sc > 0) {
	c = *sp++;
	switch (jis) {
	case ST_JISX0208:
	    if (sc == 1) {
		WRITE_BACK(1);
		errno = EINVAL;
		return -1;
	    } else if (dc == 1) {
		WRITE_BACK(1);
		errno = E2BIG;
		return -1;
	    }
	    d = *sp;
	    if (0x21 <= c && c <= 0x7e && 0x21 <= d && d <= 0x7e) {
		sp++;
		*dp++ = c | 0x80;
		*dp++ = d | 0x80;
		sc -= 2;
		dc -= 2;
		continue;
	    }
	    break;

	case ST_JISX0201:
	    if (dc == 1) {
		WRITE_BACK(1);
		errno = E2BIG;
		return -1;
	    }
	    if (0x21 <= c && c <= 0x5f) {
		*dp++ = 0x8e;
		*dp++ = c | 0x80;
		sc--;
		dc -= 2;
		continue;
	    }
	    break;

	default:
	    break;
	}
	if (c == 0x1b && jis != ST_NO_DECODE) { /* Escape Sequence */
	    int esc = 0;
	    if (sc <= 2) {
		WRITE_BACK(1);
		errno = EINVAL;
		return -1;
	    }
	    if (sp[0] == '$') {
		if (sp[1] == 'B' || sp[1] == '@') { /* JIS */
		    esc = 1;
		    jis = ST_JISX0208;
		}
	    } else if (sp[0] == '(') {
		if (sp[1] == 'B' || sp[1] == 'J') { /* ASCII */
		    esc = 1;
		    jis = ST_ASCII;
		} else if (sp[1] == 'I') {
		    esc = 1;
		    jis = ST_JISX0201;
		}
	    }
	    if (esc) {
		JA_REF(ja, jis) = jis;
		sp += 2;
		sc -= 3;
	    } else {
		*dp++ = c;
		sc--;
		dc--;
	    }
	} else if (c <= 0x7f) { /* ASCII */
	    *dp++ = c;
	    sc--;
	    dc--;
	} else if ((0x81 <= c && c <= 0x9f) || (0xe0 <= c && c <= 0xef)) {
	    /* JIS X 0208 */
	    if (sc == 1) {
		WRITE_BACK(1);
		errno = EINVAL;
		return -1;
	    } else if (dc == 1) {
		WRITE_BACK(1);
		errno = E2BIG;
		return -1;
	    }
	    d = *sp++;
	    if (d < 0x40 || 0xfc < d || d == 0x7f) {
		WRITE_BACK(2);
		errno = EILSEQ;
		return -1;
	    }
	    *dp++ = ((c << 1) | 0x80) + ((d < 0x9f) ? 0x1f : 0x20);
	    *dp++ = d + ((d < 0x7f) ? 0x61 : ((d < 0x9f) ? 0x60 : 0x02));
	    sc -= 2;
	    dc -= 2;
	} else if (0xa0 <= c && c <= 0xdf) { /* JIS X 0201 */
	    if (dc == 1) {
		WRITE_BACK(1);
		errno = E2BIG;
		return -1;
	    }
	    *dp++ = 0x8e;
	    *dp++ = c;
	    sc--;
	    dc -= 2;
	} else if (0xf0 <= c && c <= 0xfc) { /* GAIJI */
	    if (sc == 1) {
		WRITE_BACK(1);
		errno = EINVAL;
		return -1;
	    } else if (dc == 1) {
		WRITE_BACK(1);
		errno = E2BIG;
		return -1;
	    }
	    d = *sp++;
	    if (d < 0x40 || 0xfc < d || d == 0x7f) {
		WRITE_BACK(2);
		errno = EILSEQ;
		return -1;
	    }
	    *dp++ = JA_REF(ja, nc_e[0]);
	    *dp++ = JA_REF(ja, nc_e[1]);
	    sc -= 2;
	    dc -= 2;
	    nc_cnt++;
	} else {
	    WRITE_BACK(1);
	    errno = EILSEQ;
	    return -1;
	}
    }
    WRITE_BACK(0);
    if (sc) {
	errno = E2BIG;
	return -1;
    }
    return nc_cnt;
}
