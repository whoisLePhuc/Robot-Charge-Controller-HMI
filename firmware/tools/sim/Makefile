# PC simulator: renders the real hmi_ui.c with LVGL to PPM/PNG. Usage: see README.md
LVGL ?= ../../managed_components/lvgl__lvgl
OUT  ?= /tmp/hmi-sim
CFLAGS = -O1 -std=gnu11 -DLV_CONF_INCLUDE_SIMPLE -I. -I$(LVGL) -I$(LVGL)/src -I../../src -Istub
LVSRC := $(shell find $(LVGL)/src -name '*.c' -not -path '*/drivers/*' -not -path '*/libs/*' -not -path '*/others/vg_lite_tvg/*')
LVOBJ := $(patsubst $(LVGL)/%.c,$(OUT)/lv/%.o,$(LVSRC))
APPSRC = sim_main.c sim_scenarios.c ../../src/hmi_ui.c ../../src/hmi_icons.c $(wildcard ../../src/hmi_fonts/*.c)
APPOBJ = $(patsubst %.c,$(OUT)/app/%.o,$(notdir $(APPSRC)))
vpath %.c . ../../src ../../src/hmi_fonts

all: $(OUT)/hmi_sim
$(OUT)/hmi_sim: $(LVOBJ) $(APPOBJ)
	$(CC) -o $@ $^ -lm
$(OUT)/lv/%.o: $(LVGL)/%.c lv_conf.h
	@mkdir -p $(dir $@) && $(CC) $(CFLAGS) -c $< -o $@
$(OUT)/app/%.o: %.c $(wildcard ../../src/*.h) $(wildcard stub/*.h)
	@mkdir -p $(dir $@) && $(CC) $(CFLAGS) -Wall -Wextra -Wno-unused-parameter -c $< -o $@
clean: ; rm -rf $(OUT)
