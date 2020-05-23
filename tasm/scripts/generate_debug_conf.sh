#!/bin/sh

# args:
# 1 - tasm dir
# 2 - exe path
# 3 - conf filename

generate() {
    DIRNAME=`dirname $2`
    if [ $DIRNAME="." ]; then
        DIRNAME=`pwd`
    fi
    BASENAME=`basename $2`

    printf "\
[autoexec] \n\
MOUNT C $1 \n\
MOUNT D $DIRNAME \n\
D: \n\
C:\\TD.EXE D:\\$BASENAME \n\
EXIT \n\
" > $3
}

if [ $# -eq 0 ]; then
    printf "Usage: \ngenerate_make_conf.sh <tasm_dir> <exe_filename> <bosbox_conf_filename>\n"
elif [ $# -eq 3 ]; then
    generate $1 $2 $3
else
    printf "Incorrect arguments count. Run without parameters for usage instruction\n"
fi
