# Robot Charge Controller HMI in LVGL Pro

The full workflow is now in the project documentation:
[docs/05-ui/lvgl-pro-workflow.md](../docs/05-ui/lvgl-pro-workflow.md).

In short: open this `lvgl_ui` folder in the LVGL Pro Editor, edit the layout components under `components/hmi/`, press Ctrl+B to export C, and build the firmware from an ESP-IDF shell with `idf.py build`. Keep the existing widget names: the firmware finds them by name. Only `components/hmi/*/*_gen.c` is compiled into the firmware; `screens/` holds previews with sample data.

The firmware needs `CONFIG_LV_USE_OBJ_NAME=y` and `CONFIG_LV_MEM_SIZE=131072`. If a `firmware/sdkconfig` already exists from before, update both through `idf.py menuconfig` or regenerate it from `sdkconfig.defaults`.
