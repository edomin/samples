# Make an awk script to find the maximum value

BEGIN {
    max = 0;
    maxInited = 0;
}

{
    if (!maxInited) {
        max = $1;
        maxInited = 1;
    }

    for (i = 1; i <= NF; i++) {
        if ($i > max)
            max = $i;
    }
}

END {
    print max;
}
