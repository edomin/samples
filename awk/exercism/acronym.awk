#!/usr/bin/awk -f

# Source: https://exercism.io/tracks/bash/exercises/acronym

# Help generate some jargon by writing a program that converts a long name like
# Portable Network Graphics to its acronym (PNG).

BEGIN {
    if (ARGC == 1) {
        print "Usage:\n./acronym.awk <word1> <word2> ...";
        exit 1;
    }

    acronym = "";

    for (i = 1; i < ARGC; i++) {
        split(ARGV[i], array, "");
        acronym = acronym array[1];
    }

    print toupper(acronym);
}
