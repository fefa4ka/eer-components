#!/bin/sh

sigrok=$1
input_file=$2
expected_result="01 02 03 04 05 06 07 08 09 0A "

output=$(${sigrok} -i ${input_file} -P spi:miso=logic.MISO_1:mosi=logic.MOSI_1:clk=logic.CLK_1 -A spi=mosi-data | cut -f2 -d' ' | tr '\n' ' ')
if [ "$output" = "$expected_result"  ]; then
    exit 0
fi

echo "Expected $expected_result, but decoded ${output}"
exit 1
