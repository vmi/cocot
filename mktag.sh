#!/bin/sh

run=false
if [ -n "$1" ]; then
  run=true
  echo "* True run mode."
else
  echo "* Dry run mode."
fi

ver=`perl -nle '/^AC_INIT\([^,]+,\[([^\]]+)\]\)/ && print $1' configure.ac`

cmd="git tag -f cocot-$ver"

if $run; then
  echo "[$cmd -m '$1']"
  eval "$cmd -m '$1'"
else
  echo "[$cmd]"
fi
