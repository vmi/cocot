#!/bin/sh

instdir=/usr/local/bin

if [ ! -d bin ]; then
    echo "Installable binaries not exist."
    echo "Abort."
    exit 1
fi

if [ $# != 1 ]; then
    echo "Install cocot to $instdir"
    echo ""
    echo "Usage: $0 DEFAULT_PROC_CODE"
    echo "Available DEFAULT_PROC_CODE: "`ls -1 bin | sed -e 's/^.*cocot-\(.*\)\.exe$/\1/'`
    exit 1
fi

src="bin/cocot-$1.exe"
dst="$instdir/cocot.exe"

if [ -f $bin ]; then
    echo "Installing $src to $dst"
    cp -v $src $dst
    chmod +x $dst
else
    echo "Error: $src is not exists."
    exit 1
fi
