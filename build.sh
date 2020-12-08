#!/bin/bash

set -e -x
TARGET="target/$1/main"
SIMULATOR="target/$1/sim"
ARCH="$2"
echo "Building $1"

make -f Makefile.${ARCH} clean

cat BUILD.num | awk '{ print $1 + 1 }' | tee BUILD.num 

make -f Makefile.${ARCH} ${TARGET}.d ARCH=$ARCH
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
DEPENDENT_OBJECTS=`echo $DEPENDENT_OBJECTS | sed -e 's|/\./|/|g' -e ':a' -e 's|/[^/]*/\.\./|/|' -e 't a'`

if [ "$ARCH" == "AVR" ]; then
    make -f Makefile.${ARCH} "${TARGET}.hex" OBJECTS="$DEPENDENT_OBJECTS" ARCH=$ARCH PRODUCT_NAME=$1
    #make "${SIMULATOR}.sim.elf"
    make -f Makefile.${ARCH} flash TARGET="${TARGET}.hex" OBJECTS="$DEPENDENT_OBJECTS" ARCH=$ARCH
fi

if [ "$ARCH" == "STM8L" ]; then

    make -f Makefile.${ARCH} "${TARGET}.ihx" RELS="$DEPENDENT_OBJECTS" ARCH=$ARCH PRODUCT_NAME=$1
    sudo stm8flash -c stlinkv2 -p stm8l152k6 -w ${TARGET}.ihx
fi

if [ "$ARCH" == "x86" ]; then
    make -f Makefile.${ARCH} "${TARGET}.hex" OBJECTS="$DEPENDENT_OBJECTS" ARCH=$ARCH PRODUCT_NAME=$1
    make -f Makefile.${ARCH} "${TARGET}" OBJECTS="$DEPENDENT_OBJECTS" ARCH=$ARCH PRODUCT_NAME=$1
fi

