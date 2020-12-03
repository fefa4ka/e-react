#!/bin/bash

set -e -x
TARGET="target/$1/main"
SIMULATOR="target/$1/sim"
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
        if [ "$ARCH" == "AVR" ]; then
            object=${sourcename/.c/.o}
        fi

        if [ "$ARCH" == "STM8L" ]; then

            if [[ "$filename" == *"main.c" ]]; then
                continue
            fi
            object=${sourcename/.c/.rel}
        fi

        DEPENDENT_OBJECTS="${DEPENDENT_OBJECTS} ${object}"
    fi
done
DEPENDENT_OBJECTS=`echo $DEPENDENT_OBJECTS | sed -e 's|/\./|/|g' -e ':a' -e 's|/[^/]*/\.\./|/|' -e 't a' | awk '{ for (i=NF; i>1; i--) printf("%s ",$i); print $1; }'`

if [ "$ARCH" == "AVR" ]; then
    make "${TARGET}.hex" OBJECTS="$DEPENDENT_OBJECTS" ARCH=$ARCH PRODUCT_NAME=$1
    make "${SIMULATOR}.sim.elf"
    make flash TARGET="${TARGET}.hex" OBJECTS="$DEPENDENT_OBJECTS" ARCH=$ARCH
fi

if [ "$ARCH" == "STM8L" ]; then
    make "${TARGET}.ihx" RELS="$DEPENDENT_OBJECTS" ARCH=$ARCH PRODUCT_NAME=$1
fi

if [ "$ARCH" == "x86" ]; then
    make "${TARGET}.hex" OBJECTS="$DEPENDENT_OBJECTS" ARCH=$ARCH PRODUCT_NAME=$1
    make "${TARGET}" OBJECTS="$DEPENDENT_OBJECTS" ARCH=$ARCH PRODUCT_NAME=$1
fi

