#!/bin/sh
cd "$(dirname "$0")" || exit 1
CC=${CC:-cc}
set -e
$CC -std=c11 -Wall -Wextra -I../../main test_rcc_codec.c ../../main/rcc_codec.c -o /tmp/test_rcc_codec && /tmp/test_rcc_codec
# hmi_present is pure C: it needs only rcc_types.h from hmi_common, no ESP-IDF header or stub.
$CC -std=c11 -Wall -Wextra -Werror -I../../main/ui -I../../components/common/hmi_common/include test_hmi_present.c ../../main/ui/hmi_present.c -o /tmp/test_hmi_present && /tmp/test_hmi_present
