#!/usr/bin/env ruby

# get all values
numerator = 1000 * 60 * 10
results = (0..3000).map {|i| i < 10 ? 0 : numerator / i}

puts <<~EOS
  #include "stdint.h"

  #if (defined(__AVR__))
  #include <avr/pgmspace.h>
  #endif

  #include "samples_per_output_lookup.h"

EOS

# FIXME ATTEMPT SIMPLE PROGSPACE

# puts("static const uint16_t sample_per_output_lookup_table[260] PROGMEM = {\n")
# puts(results.join(",\n"))
# puts("};\n")
#
# puts("uint16_t samples_per_output_lookup(int16_t tbpm) {")
# puts("  return pgm_read_word(&sample_per_output_lookup_table[tbpm]);")
# puts("}")

# FIXME ATTEMPT DUMBDUMB
# ifstrings = []
# results.each_with_index.map do |r, i|
#   ifstrings << "if(tbpm == #{i}) {\n  return #{r};\n}"
# end
#
# puts('uint16_t samples_per_output_lookup(int16_t tbpm) {')
# puts(ifstrings.join(' else '))
# puts('}')

# FIXME ATTEMPT WEEEEE

partitioned = results.each_slice(256).to_a

lookupstrings = []
ifstrings = []
partitioned.count.times do |i|
  min = 256 * i
  max = 256 * (i + 1)
  size = partitioned[i].count

  lookupstrings << "static const uint16_t samples_per_output_#{i}[#{size}] PROGMEM = {\n" +
    partitioned[i].join(",\n") +
    lookupstop = "};\n"

  ifstrings << "" +
    "if(tbpm < #{max}) {\n" +
    "  subindex = tbpm - #{min};\n" +
    "  result = pgm_read_word(&samples_per_output_#{i}[subindex]);\n" +
    "}"
end

puts <<~EOS
  #{lookupstrings.join("\n\n")}

  uint16_t samples_per_output_lookup(int16_t tbpm) {
  uint8_t subindex;
  uint16_t result;
  #{ifstrings.join(" else ")} else {
    result = 0;
  }
  return result;
  }

EOS

# require 'pry'; binding.pry
#
# puts((10..3000).map {|i| i < 10 ? 60000 : numerator / i}.uniq.count)
