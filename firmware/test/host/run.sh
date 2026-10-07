#!/bin/sh
cd "$(dirname "$0")" && cc -std=c11 -Wall -Wextra -I../../src test_rcc_codec.c ../../src/rcc_codec.c -o /tmp/test_rcc_codec && /tmp/test_rcc_codec
