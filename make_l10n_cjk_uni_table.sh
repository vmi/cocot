#!/bin/sh -x

if [ ! -f EastAsianWidth.txt ]; then
    wget ftp://ftp.unicode.org/Public/UNIDATA/EastAsianWidth.txt
fi

perl make_l10n_cjk_uni_table.pl > l10n_cjk_uni_table.c
