# Layer: protocol. Controller frame codec (framing, CRC, request encoding). Pure C, host-tested
# (test/host/test_rcc_codec.c); depends on nothing.
idf_component_register(
    SRCS "src/rcc_codec.c"
    INCLUDE_DIRS "include")
target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra -Werror)
