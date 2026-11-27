#!/bin/sh
cd "$(dirname "$0")" || exit 1
CC=${CC:-cc}
set -e
$CC -std=c11 -Wall -Wextra -I../../main test_rcc_codec.c ../../main/rcc_codec.c -o /tmp/test_rcc_codec && /tmp/test_rcc_codec
# hmi_present is pure C; rcc_link.h only needs the esp_err.h stub from the simulator.
$CC -std=c11 -Wall -Wextra -Werror -I../../main -I../../main/ui -I../../tools/sim/stub test_hmi_present.c ../../main/ui/hmi_present.c -o /tmp/test_hmi_present && /tmp/test_hmi_present
