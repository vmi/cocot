/*
 * Localization for CJK Language with Unicode
 *
 * Copyright (c) 2004, 2005  IWAMURO Motonori
 * All rights reserved.
 */

#if HAVE_CONFIG_H
#  include <config.h>
#endif
#if HAVE_SYS_TYPES_H
#  include <sys/types.h>
#endif
#if HAVE_ICONV_H
#  include <iconv.h>
#endif
#include <errno.h>
#include "l10n_cjk_uni.h"
#include "l10n_cjk_uni_table.h"

static unsigned char char_bytes[256] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 1, 1,
};

#define IS_NOT_TRAILING_BYTE(ch) (((ch) & 0xc0) != 0x80)

int
l10n_cjk_uni_skip(const char **spp, size_t *scp)
{
    const unsigned char *sp;
    size_t cb;
    size_t ch;
    int w;

    sp = *spp;
    cb = char_bytes[sp[0]];
    if (cb > *scp)
	goto error;
    switch (cb) {
    case 1:
	ch = sp[0];
	break;
    case 2:
	if (IS_NOT_TRAILING_BYTE(sp[1]))
	    goto error;
	ch  = ((sp[0] & 0x1f) <<  6)
	    |  (sp[1] & 0x3f);
	break;
    case 3:
	if (IS_NOT_TRAILING_BYTE(sp[1])
	    || IS_NOT_TRAILING_BYTE(sp[2]))
	    goto error;
	ch  = ((sp[0] & 0x0f) << 12)
	    | ((sp[1] & 0x3f) <<  6)
	    |  (sp[2] & 0x3f);
	break;
    case 4:
	if (IS_NOT_TRAILING_BYTE(sp[1])
	    || IS_NOT_TRAILING_BYTE(sp[2])
	    || IS_NOT_TRAILING_BYTE(sp[3]))
	    goto error;
	ch  = ((sp[0] & 0x07) << 18)
	    | ((sp[1] & 0x3f) << 12)
	    | ((sp[2] & 0x3f) <<  6)
	    |  (sp[3] & 0x3f);
	break;
    case 5:
	if (IS_NOT_TRAILING_BYTE(sp[1])
	    || IS_NOT_TRAILING_BYTE(sp[2])
	    || IS_NOT_TRAILING_BYTE(sp[3])
	    || IS_NOT_TRAILING_BYTE(sp[4]))
	    goto error;
	ch  = ((sp[0] & 0x03) << 24)
	    | ((sp[1] & 0x3f) << 18)
	    | ((sp[2] & 0x3f) << 12)
	    | ((sp[3] & 0x3f) <<  6)
	    |  (sp[4] & 0x3f);
	break;
    case 6:
	if (IS_NOT_TRAILING_BYTE(sp[1])
	    || IS_NOT_TRAILING_BYTE(sp[2])
	    || IS_NOT_TRAILING_BYTE(sp[3])
	    || IS_NOT_TRAILING_BYTE(sp[4])
	    || IS_NOT_TRAILING_BYTE(sp[5]))
	    goto error;
	ch  = ((sp[0] & 0x01) << 30)
	    | ((sp[1] & 0x3f) << 24)
	    | ((sp[2] & 0x3f) << 18)
	    | ((sp[3] & 0x3f) << 12)
	    | ((sp[4] & 0x3f) <<  6)
	    |  (sp[5] & 0x3f);
	break;
    default:
	goto error;
    }
    if (ch < 0x010000) { /* BMP */
	w = (cjk_width[ch / 32] & (1 << (ch % 32))) ? 2 : 1;
    } else if (ch < 0x020000) { /* N */
	w = 1;
    } else if (ch < 0x0e0000) { /* W */
	w = 2;
    } else if (ch < 0x0e0100) { /* N */
	w = 1;
    } else { /* A */
	w = 2;
    }
    *spp += cb;
    *scp -= cb;
    return w;

error:
    (*spp)++;
    (*scp)--;
    return 1;
}
