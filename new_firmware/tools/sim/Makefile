# PC simulator: renders the real UI components (components/ui) with LVGL to PPM/PNG. Usage: see README.md
LVGL ?= ../../managed_components/lvgl__lvgl
OUT  ?= /tmp/hmi-sim
UI   := ../../components/ui
CFLAGS = -DHMI_SIM -O1 -std=gnu11 -DLV_CONF_INCLUDE_SIMPLE -I. -I$(LVGL) -I$(LVGL)/src \
         $(addprefix -I,$(wildcard ../../components/*/*/include)) -I$(UI)/ui_app/src -Istub
LVSRC := $(shell find $(LVGL)/src -name '*.c' -not -path '*/drivers/*' -not -path '*/libs/*' -not -path '*/draw/nxp/*' -not -path '*/others/vg_lite_tvg/*')
LVOBJ := $(patsubst $(LVGL)/%.c,$(OUT)/lv/%.o,$(LVSRC))
# The UI components only; services, link and board are replaced by sim_*.c.
APPSRC = sim_main.c sim_scenarios.c sim_stubs.c sim_wifi.c \
         $(wildcard $(UI)/ui_app/src/*.c $(UI)/ui_app/src/pages/*.c $(UI)/ui_core/src/*.c \
                    $(UI)/ui_present/src/*.c $(UI)/ui_assets/src/*.c $(UI)/ui_assets/src/fonts/*.c)
APPOBJ = $(patsubst %.c,$(OUT)/app/%.o,$(notdir $(APPSRC)))
APPHDR = $(wildcard ../../components/*/*/include/*.h $(UI)/ui_app/src/*.h $(UI)/ui_app/src/pages/*.h) $(wildcard stub/*.h)
vpath %.c . $(sort $(dir $(APPSRC)))

all: $(OUT)/hmi_sim
$(OUT)/hmi_sim: $(LVOBJ) $(APPOBJ)
	$(CC) -o $@ $^ -lm
$(OUT)/lv/%.o: $(LVGL)/%.c lv_conf.h
	@mkdir -p $(dir $@) && $(CC) $(CFLAGS) -c $< -o $@
$(OUT)/app/%.o: %.c $(APPHDR)
	@mkdir -p $(dir $@) && $(CC) $(CFLAGS) -Wall -Wextra -Wno-unused-parameter -c $< -o $@
clean: ; rm -rf $(OUT)
