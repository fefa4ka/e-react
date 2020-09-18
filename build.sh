#!/bin/bash

TARGET="target/$1"
ARCH="$2"
echo "Building $1"

make clean

cat BUILD.num | awk '{ print $1 + 1 }' | tee BUILD.num 

make ${TARGET}.d ARCH=$ARCH
DEPS=`cat ${TARGET}.d | cut -d':' -f2`
DEPENDENT_OBJECTS=""
for filename in ${DEPS}; do
    sourcename=${filename/.h/.c}
    if [ -f $sourcename ]; then
        object=${sourcename/.c/.o}
        DEPENDENT_OBJECTS="${DEPENDENT_OBJECTS} ${object}"
    fi
done
DEPENDENT_OBJECTS=`echo $DEPENDENT_OBJECTS | sed -e 's|/\./|/|g' -e ':a' -e 's|/[^/]*/\.\./|/|' -e 't a'`

if [ "$ARCH" == "AVR" ]; then
    make "${TARGET}.hex" OBJECTS="$DEPENDENT_OBJECTS" ARCH=$ARCH
    make flash TARGET=${TARGET}.hex ARCH=$ARCH
else
    make "${TARGET}.elf" OBJECTS="$DEPENDENT_OBJECTS" ARCH=$ARCH
fi

