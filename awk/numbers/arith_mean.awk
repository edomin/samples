# Make an awk script to calculate arithmetic mean value

BEGIN {
    sum = 0;
    count = 0;
}

{
    count += NF;
    for (i = 1; i <= NF; i++)
        sum = sum + $i;
}

END {
    print sum / count;
}
