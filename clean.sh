#!/bin/bash
# clean everything so source packages et.al. get as small as possible

for DIR in Debug Release Mingw32 Mingw64 nsis
do
  test -d $DIR && make -C $DIR clean
done

rm -rfv $(find . -iname '*.o' -o -iname '*.d')

