#!/bin/bash

IFS=$'\n\t'

if [ ! "$TRANSPARENT" ];
then
    TRANSPARENT="#ff00ff";
    echo "Set transparent to ${TRANSPARENT}";
fi;

if [ ! "$PALETTE" ];
then
    PALETTE=0
fi;

GMTOOL=./gmtool.out
FILE="$*"
NUM_PALETTES=`$GMTOOL palette --count "$FILE"`
NUM_ENTRIES=`$GMTOOL header --count "$FILE"`
FILENAME=`basename "$FILE"`
OUTDIR=gm
DIR="$OUTDIR/$FILENAME"
FTYPE=png

if [ ! -d "$OUTDIR" ];
then
    echo "Create directory $OUTDIR";
    mkdir "$OUTDIR";
fi;

if [ ! -d "$DIR" ];
then 
    echo "Create directory $DIR";
    mkdir "$DIR";
fi;

echo Unpacking into "$DIR"

for I in `seq 0 $((NUM_ENTRIES-1))`;
do
    OUTPUT=`printf "$DIR/%.5d.$FTYPE" $I`;
    $GMTOOL render -f$FTYPE -i$I -p$PALETTE -o"$OUTPUT" --transparent-color="${TRANSPARENT}" -- "$FILE";
done;
