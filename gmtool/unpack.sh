#!/bin/bash

## some files (like puff\ of\ smoke.gm1) may contain spaces
IFS=$'\n\t'

if [ ! "$TRANSPARENT" ]; then
    TRANSPARENT="#ff00ff";
fi;
echo "Set transparent to ${TRANSPARENT}";

if [ ! "$PALETTE" ];
then
    PALETTE=0
fi;
echo "Use palette index $PALETTE";

if [ ! "$GMTOOL" ]; then
    GMTOOL=./gmtool.out;
fi;
echo "Set binary to $GMTOOL";

if [ ! "$FTYPE" ]; then
    FTYPE=png;
fi;
echo "Set output file type to $FTYPE";

## each unpacked collection should be placed in it's own subdirectory
## in OUTDIR.
if [ ! "$OUTDIR" ]; then
    OUTDIR=gm;
fi;
echo "Set output directory $OUTDIR";

FILE="$*"
NUM_PALETTES=`$GMTOOL palette --count "$FILE"`
NUM_ENTRIES=`$GMTOOL header --count "$FILE"`
FILENAME=`basename "$FILE"`
DIR="$OUTDIR/$FILENAME"

if [ ! -d "$DIR" ];
then 
    echo "$DIR doesn't exist. Creating.";
    mkdir -p "$DIR";
fi;

echo Unpacking into "$DIR"

for I in `seq 0 $((NUM_ENTRIES-1))`;
do
    OUTPUT=`printf "$DIR/%.5d.$FTYPE" $I`;
    $GMTOOL render -f$FTYPE -i$I -p$PALETTE --transparent-color="${TRANSPARENT}" -- "$FILE" | dd of="$OUTPUT" >& /dev/null
done;
