# PC simulator: renders the real hmi_ui.c with LVGL to PPM/PNG. Usage: see README.md
LVGL ?= ../../managed_components/lvgl__lvgl
OUT  ?= /tmp/hmi-sim
CFLAGS = -DHMI_SIM -O1 -std=gnu11 -DLV_CONF_INCLUDE_SIMPLE -I. -I$(LVGL) -I$(LVGL)/src -I../../main -I../../main/ui -I../../components/common/hmi_common/include -I../../components/protocol/rcc_link/include -Istub
LVSRC := $(shell find $(LVGL)/src -name '*.c' -not -path '*/drivers/*' -not -path '*/libs/*' -not -path '*/draw/nxp/*' -not -path '*/others/vg_lite_tvg/*')
LVOBJ := $(patsubst $(LVGL)/%.c,$(OUT)/lv/%.o,$(LVSRC))
APPSRC = sim_main.c sim_scenarios.c sim_stubs.c sim_wifi.c ../../main/hmi_ui.c ../../main/hmi_icons.c ../../main/ui/hmi_theme.c ../../main/ui/hmi_present.c ../../main/ui/hmi_model.c ../../main/ui/hmi_widgets.c ../../main/ui/hmi_nav.c ../../main/ui/hmi_shell.c ../../main/ui/hmi_wifi_ui.c ../../main/ui/hmi_pages.c $(wildcard ../../main/ui/pages/*.c) $(filter-out %hmi_f20b.c %hmi_hero40.c %hmi_hero48.c,$(wildcard ../../main/hmi_fonts/*.c))
APPOBJ = $(patsubst %.c,$(OUT)/app/%.o,$(notdir $(APPSRC)))
vpath %.c . ../../main ../../main/ui ../../main/ui/pages ../../main/hmi_fonts

all: $(OUT)/hmi_sim
$(OUT)/hmi_sim: $(LVOBJ) $(APPOBJ)
	$(CC) -o $@ $^ -lm
$(OUT)/lv/%.o: $(LVGL)/%.c lv_conf.h
	@mkdir -p $(dir $@) && $(CC) $(CFLAGS) -c $< -o $@
$(OUT)/app/%.o: %.c $(wildcard ../../main/*.h ../../main/ui/*.h ../../main/ui/pages/*.h ../../components/*/*/include/*.h) $(wildcard stub/*.h)
	@mkdir -p $(dir $@) && $(CC) $(CFLAGS) -Wall -Wextra -Wno-unused-parameter -c $< -o $@
clean: ; rm -rf $(OUT)
