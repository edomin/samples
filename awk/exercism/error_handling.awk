#!/usr/bin/awk -f

# Source: https://exercism.io/tracks/bash/exercises/error-handling

BEGIN {
    if (ARGC <= 1) {
        print "Usage:\n./error_handling.awk <person>";
        exit 1;
    }
    print "Hello, " ARGV[1];
}
