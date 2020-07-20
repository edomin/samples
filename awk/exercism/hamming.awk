#!/usr/bin/awk -f

# Source: https://exercism.io/tracks/bash/exercises/hamming

# Calculate the Hamming Distance between two DNA strands.
# Your body is made up of cells that contain DNA. Those cells regularly wear
# out and need replacing, which they achieve by dividing into daughter cells.
# In fact, the average human body experiences about 10 quadrillion cell
# divisions in a lifetime!
#
# When cells divide, their DNA replicates too. Sometimes during this process
# mistakes happen and single pieces of DNA get encoded with the incorrect
# information. If we compare two strands of DNA and count the differences
# between them we can see how many mistakes occurred. This is known as the
# "Hamming Distance".
#
# We read DNA using the letters C,A,G and T. Two strands might look like this:
# GAGCCTACTAACGGGAT
# CATCGTAATGACGGCCT
# ^ ^ ^  ^ ^    ^^
# They have 7 differences, and therefore the Hamming Distance is 7.

# Implementation notes
# The Hamming distance is only defined for sequences of equal length, so an
# attempt to calculate it between sequences of different lengths should not
# work.

BEGIN {
    if (ARGC <= 2) {
        print "Usage:\n./hamming.awk <dna1> <dna2>";
        exit 1;
    }

    len1 = length(ARGV[1]);
    len2 = length(ARGV[2]);

    if (len1 != len2) {
        print "DNAs' lengths are not equal."
        exit 2;
    }

    split(ARGV[1], dna1, "");
    split(ARGV[2], dna2, "");

    hamming = 0;
    for (i = 1; i <= len1; i++) {
        if (dna1[i] != dna2[i])
            hamming++;
    }

    print "Hamming distance: " hamming;
}
