#!/usr/bin/env python

import sys

# evenly distribute remainder across collection
# will need to integrate this into channels so remainder can be applied
# to samples_per_beat after each beat

"""

typedef struct HeadwaterStateChannel {
    uint16_t samples_per_beat_with_remainder;
    uint16_t remainder_frequency;
    uint16_t remainder_priority;
    uint16_t no_remainder_frequency;
    uint16_t no_remainder_priority;
} HeadwaterStateChannel;

"""

samples_per_beat = int(sys.argv[1])
multiplier = int(sys.argv[2])
remainder_count = int(sys.argv[3])
no_remainder_count = multiplier - remainder_count

# need to guard for division by zero
remainder_frequency = int((multiplier * 100) / remainder_count)
no_remainder_frequency = int((multiplier * 100) / no_remainder_count)

# reset priorities to base frequency when multiplier resets
remainder_priority = remainder_frequency
no_remainder_priority = no_remainder_frequency

result = []

# doesn't care if it overruns multiplier, will keep repeating forever
for _ in range(multiplier):
    samples_per_beat_total = samples_per_beat

    # add remainder, update priorities
    if(remainder_priority < no_remainder_priority):
        samples_per_beat_total += 1
        remainder_priority += remainder_frequency
    else:
        no_remainder_priority += no_remainder_frequency

    # reduce priority to difference to keep size small
    if(remainder_priority < no_remainder_priority):
        no_remainder_priority -= remainder_priority
        remainder_priority = 0
    else:
        remainder_priority -= no_remainder_priority
        no_remainder_priority = 0

    result.append(samples_per_beat_total)

print(result)
