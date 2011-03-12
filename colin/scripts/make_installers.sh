#!/bin/sh
#
# $Id$
#

#
# This script creates all windows installer packages 
# and proposes the scp/ssh calls for deploying
#

# for crosscompilation you need several packages from deb.clazzes.org' *-mingw-3 archives
# (choose the archive apropriate for your Linux)
# the packages from clazzes.org are: mingw-w32-qt4 mingw-w64-qt4
# you also need (from your distro): tofrodos 
# 

MYDIR=$( dirname $( readlink -f $( which $0 ) ) )
WSDIR="$MYDIR/.."
SRCDIR="$MYDIR/../src"
#echo "WSDIR=$WSDIR"

APPVERSION=$(/usr/share/pba-cbs/sh/get_deb_version.sh src)


# 1. translations

#echo "Calling $MYDIR/update_translations.sh"
#$MYDIR/update_translations.sh || exit 11


# 3. unix2dos docs

echo "Entering $WSDIR/doc"
cd $WSDIR/doc || exit 31

echo "DOSifiying *.txt"
for F in LICENSE.txt 
do
  todos $F || exit 32
done

echo "Leaving $WSDIR/doc"
cd $MYDIR || exit 39


# 4. clean and make win*

echo "Entering $WSDIR/nsis" 
cd $WSDIR/nsis

echo "Calling make to create nsis installers"
make clean || exit 41
make || exit 42
echo "Done creating nsis installers"

echo "Leaving $WSDIR/nsis"


# 99. done

echo "$0 Done:"
ls -ld $WSDIR/nsis/colin*.exe

echo "If you want to publically release those, do this:"
echo "chmod 0755 \\"
echo " $WSDIR/nsis/colin-${APPVERSION}*setup*.exe"
echo "scp \\"
echo " $WSDIR/nsis/colin-${APPVERSION}*setup*.exe \\"
echo " doc/LICENSE.txt \\"
echo " colin@www.clazzes.org:/var/www/htdocs/download.clazzes.org/colin/testing/"
echo "ssh colin@www.clazzes.org ls -ltrR /var/www/htdocs/download.clazzes.org/colin"
echo "# (let the server care for file ownership)"

# exit 0
