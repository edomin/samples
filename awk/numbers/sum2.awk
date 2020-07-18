# Can you write a script to print the cumulative sum i.e. on each row, next to the row's sum, you print the sum of all above values?

BEGIN {
    sum = 0;
}

{
    rowSum = 0;

    for (i = 1; i <= NF; i++)
        rowSum += $i;

    sum += rowSum;

    print rowSum " " sum;
}
