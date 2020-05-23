#!/bin/sh

# args:
# 1 - tasm dir
# 2 - asm path
# 3 - conf filename

generate() {
    DIRNAME=`dirname $2`
    if [ $DIRNAME="." ]; then
        DIRNAME=`pwd`
    fi
    BASENAME=`basename $2`
    BASENAME_WITHOUT_EXT=`basename --suffix=.asm $2`

    printf "\
[autoexec] \n\
MOUNT C $1 \n\
MOUNT D $DIRNAME \n\
D: \n\
C:\\TASM.EXE D:\\$BASENAME > TASM.LOG \n\
C:\\TLINK.EXE /v D:\\$BASENAME_WITHOUT_EXT.obj > TLINK.LOG \n\
EXIT \n\
" > $3
}

if [ $# -eq 0 ]; then
    printf "Usage: \ngenerate_make_conf.sh <tasm_dir> <asm_filename> <bosbox_conf_filename>\n"
elif [ $# -eq 3 ]; then
    generate $1 $2 $3
else
    printf "Incorrect arguments count. Run without parameters for usage instruction\n"
fi
