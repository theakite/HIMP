#!/bin/sh

# this script runs the suite of unit tests

make -i clean

make -i -f keylist.mak clean
make -f keylist.mak all
./key
./keylist
make -i -f keylist.mak clean

make -i -f bacnet_text.mak clean
make -f bacnet_text.mak all
./bacnet_text
make -i -f bacnet_text.mak clean

make -i -f dstring.mak clean
make -f dstring.mak all
./dstring
make -i -f dstring.mak clean

make -i -f dbuffer.mak clean
make -f dbuffer.mak all
./dbuffer
make -i -f dbuffer.mak clean

make -i -f objectlist.mak clean
make -f objectlist.mak all
./objectlist
make -i -f objectlist.mak clean

make -i -f bacdcode.mak clean
make -f bacdcode.mak all
./bacdcode
make -i -f bacdcode.mak clean

