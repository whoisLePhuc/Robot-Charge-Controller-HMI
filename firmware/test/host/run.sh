#!/bin/sh
cd "$(dirname "$0")" || exit 1
CC=${CC:-cc}
set -e
$CC -std=c11 -Wall -Wextra -I../../components/protocol/rcc_codec/include test_rcc_codec.c ../../components/protocol/rcc_codec/src/rcc_codec.c -o /tmp/test_rcc_codec && /tmp/test_rcc_codec
# hmi_present is pure C: it needs only rcc_types.h from hmi_common, no ESP-IDF header or stub.
$CC -std=c11 -Wall -Wextra -Werror -I../../components/ui/ui_present/include -I../../components/common/hmi_common/include test_hmi_present.c ../../components/ui/ui_present/src/hmi_present.c -o /tmp/test_hmi_present && /tmp/test_hmi_present
# rcc_link: includes rcc_link.c to reach its static state; ESP-IDF APIs come from stubs/.
$CC -std=c11 -Wall -Wextra -Istubs -I../../components/protocol/rcc_link/include -I../../components/protocol/rcc_codec/include -I../../components/common/hmi_common/include test_rcc_link.c ../../components/protocol/rcc_codec/src/rcc_codec.c -o /tmp/test_rcc_link && /tmp/test_rcc_link
