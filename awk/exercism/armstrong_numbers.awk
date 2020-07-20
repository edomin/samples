#!/usr/bin/awk -f

# Source: https://exercism.io/tracks/bash/exercises/armstrong-numbers

# An Armstrong number is a number that is the sum of its own digits each raised
# to the power of the number of digits.
# For example:
#     9 is an Armstrong number, because 9 = 9^1 = 9
#     10 is not an Armstrong number, because 10 != 1^2 + 0^2 = 1
#     153 is an Armstrong number, because: 153 = 1^3 + 5^3 + 3^3 = 1 + 125 + 27
#      = 153
#     154 is not an Armstrong number, because: 154 != 1^3 + 5^3 + 4^3 = 1 + 125
#      + 64 = 190
#
# Write some code to determine whether a number is an Armstrong number.

BEGIN {
    if (ARGC == 1) {
        print "Usage:\n./armstrong_numbers.awk <number>";
        exit 1;
    }

    split(ARGV[1], digits, "");
    sum = 0;
    len = length(ARGV[1]);

    for (i = 1; i <= len; i++) {
        split(ARGV[i], array, "");

        sum += digits[i] ^ len;
    }

    if (ARGV[1] == sum)
        print "Armstrong number";
    else
        print "Simple number";
}
