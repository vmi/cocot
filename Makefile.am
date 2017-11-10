bin_PROGRAMS = cocot
cocot_SOURCES = ccio.c ccio.h cocot.c init.c init.h l10n_ja.c l10n_ja.h l10n_cjk_uni.c l10n_cjk_uni.h l10n_cjk_uni_table.c l10n_cjk_uni_table.h loop.c loop.h sigwinch.c sigwinch.h suspend.c suspend.h malloc.c debug.h
EXTRA_DIST = ChangeLog.ja README.ja UNICODE_MEMO make_l10n_cjk_uni_table.sh make_l10n_cjk_uni_table.pl cygwin-install.sh cocot.exe

mydist::
	make
	strip cocot.exe
	make dist-bzip2
	mv cocot-*.tar.bz2 ..
