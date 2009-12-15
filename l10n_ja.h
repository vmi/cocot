/*
 * Localization for Japanese Language
 *
 * Copyright (c) 2002  IWAMURO Motonori
 * All rights reserved.
 */

#ifndef L10N_JA_H
#define L10N_JA_H

#if HAVE_SYS_TYPES_H
#  include <sys/types.h>
#endif

void *
l10n_ja_open(int dec_jis);

int
l10n_ja_close(void *ja);

size_t
l10n_ja_eucj2sjis(void *ja,
		  const char **spp, size_t *scp,
		  char **dpp, size_t *dcp);

size_t
l10n_ja_sjis2eucj(void *ja,
		  const char **spp, size_t *scp,
		  char **dpp, size_t *dcp);

#endif /* L10N_JA_H */
