#!/bin/sh

# args:
# 1 - exe path
# 2 - conf filename

generate() {
    DIRNAME=`dirname $2`
    if [ $DIRNAME="." ]; then
        DIRNAME=`pwd`
    fi
    BASENAME=`basename $2`

    printf "\
[autoexec] \n\
MOUNT D $DIRNAME \n\
D: \n\
D:\\$BASENAME \n\
EXIT \n\
" > $2
}

if [ $# -eq 0 ]; then
    printf "Usage: \ngenerate_make_conf.sh <tasm_dir> <exe_filename>\n"
elif [ $# -eq 2 ]; then
    generate $1 $2
else
    printf "Incorrect arguments count. Run without parameters for usage instruction\n"
fi
