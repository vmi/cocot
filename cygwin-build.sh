#!/bin/sh

PREFIX=/usr/local
TERM_CODE=CP932
PROC_CODE_LIST='EUC-JP UTF-8'

set -e

if [ ! -d bin ]; then
  mkdir bin
fi

for proc_code in $PROC_CODE_LIST; do
  echo "** Build by $TERM_CODE / $proc_code"
  rm -rf build
  cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=$PREFIX \
    -DDEFAULT_TERM_CODE=$TERM_CODE \
    -DDEFAULT_PROC_CODE=$proc_code
  cmake --build build
  strip -x build/cocot.exe
  cp -v build/cocot.exe bin/cocot-$proc_code.exe
done
rm -rf build
