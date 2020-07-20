#!/usr/bin/awk -f

# Source: https://exercism.io/tracks/bash/exercises/raindrops

# Your task is to convert a number into a string that contains raindrop sounds
# corresponding to certain potential factors. A factor is a number that evenly
# divides into another number, leaving no remainder. The simplest way to test
# if a one number is a factor of another is to use the modulo operation.
# The rules of raindrops are that if a given number:
#     has 3 as a factor, add 'Pling' to the result.
#     has 5 as a factor, add 'Plang' to the result.
#     has 7 as a factor, add 'Plong' to the result.
#     does not have any of 3, 5, or 7 as a factor, the result should be the
#      digits of the number.

BEGIN {
    if (ARGC <= 1) {
        print "Usage:\n./raindrops.awk <number>";
        exit 1;
    }

    sound = "";

    if (ARGV[1] % 3 == 0)
        sound = sound "Pling";
    if (ARGV[1] % 5 == 0)
        sound = sound "Plang";
    if (ARGV[1] % 7 == 0)
        sound = sound "Plong";
    if (sound == "")
        sound = ARGV[1];

    print sound;
}
