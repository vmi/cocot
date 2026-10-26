#!/bin/sh -x

if [ ! -f EastAsianWidth.txt ]; then
    wget https://www.unicode.org/Public/5.0.0/ucd/EastAsianWidth.txt
fi

perl make_l10n_cjk_uni_table.pl > l10n_cjk_uni_table.c
