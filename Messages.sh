#!/bin/sh
$EXTRACTRC src/*.rc >> rc.cpp || exit 11
$XGETTEXT rc.cpp src/*.cpp src/*.h plasma/*.cpp plasma/*.h -o $podir/rsibreak.pot
rm -f rc.cpp
