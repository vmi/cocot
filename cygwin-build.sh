#!/bin/sh

PREFIX=/usr/local
TERM_CODE=CP932
PROC_CODE_LIST='EUC-JP UTF-8'

set -e

if [ ! -d bin ]; then
  mkdir bin
fi

if [ -f Makefile ]; then
  make distclean
fi

for proc_code in $PROC_CODE_LIST; do
  echo "** Build by $TERM_CODE / $proc_code"
  ./configure \
    --prefix=$PREFIX \
    --with-default-term-code=$TERM_CODE \
    --with-default-proc-code=$proc_code
  make
  strip -x cocot.exe
  cp -v cocot.exe bin/cocot-$proc_code.exe
  make distclean
done
