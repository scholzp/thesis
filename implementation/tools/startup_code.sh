#!/bin/sh
PWD=$(pwd)
SRC=${PWD}/../kmod
# ls ${SRC}
as -q -o ${PWD}/startup.o -mx86-used-note=no -c ${SRC}/startup.S
ld -w -Ttext 0 --oformat binary -o ${PWD}/startup.bin ${PWD}/startup.o
# xxd -i ${SRC}/startup.bin
XXD=$(xxd -i ${PWD}/startup.bin)
xxd -n STARTUP_CODE -i ${PWD}/startup.bin startup_code.h

echo ${XXD}
