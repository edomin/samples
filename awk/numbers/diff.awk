# Make an awk script to calculate the difference between numbers in the first column, i.e 2nd-1st, 3rd-2nd etc.

{
    if (NR > 1) {
        print NR "th-" NR-1 "th = " $1-prevValue;
    }
    prevValue = $1;
}
