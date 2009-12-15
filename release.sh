#!/bin/sh

distdir=$HOME/www/htdocs/software/cygwin

set -e

if [ ! -d $distdir ]; then
  echo "$distdir is not found."
  echo "Abort."
  exit 1
fi

version=`grep ^AC_INIT configure.ac | sed -e 's/^.*\[\|\].*$//g'`
if [ X"$version" = X ]; then
  echo "Can't get current version."
  echo "Abort."
  exit 1
fi
echo "Version: $version"

name="cocot-$version"
tar=$name.tar.bz2
cd ..
if [ -d $name ]; then
  echo "Remove old release files."
  rm -rf $name
fi
echo "Copy cocot to $name"
cp -a cocot $name
cd $name
echo "Clean up $name"
rm -rf .svn EastAsianWidth.txt autom4te.cache cygwin-build.sh release.sh *~
cd ..
echo "Archiving $tar"
tar cfj $tar $name
echo "Copy $tar to $distdir"
cp $tar $distdir
cd $distdir
svn status
echo "Done."
echo ""
echo "* Run following commands for release."
echo "cd $distdir"
echo "svn add $tar"
echo "svn stat"
