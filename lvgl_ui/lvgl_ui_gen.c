/**
 * @file lvgl_ui_gen.c
 */

/*********************
 *      INCLUDES
 *********************/

#include "lvgl_ui.h"

#if defined(LV_USE_XML) && LV_USE_XML
#endif /* LV_USE_XML */

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void check_font(lv_font_t ** font, const char * name);

/**********************
 *  STATIC VARIABLES
 **********************/

static uint32_t lvgl_ui_target = LVGL_UI_COMPILE_TARGET;

/*----------------
 * Translations
 *----------------*/

#ifndef LV_EDITOR_PREVIEW
    static const char * translation_languages[] = {"en", "de", "es", NULL};
    static const char * translation_tags[] = {"dark_theme", "buttons", "controls", "text_inputs", "selection", NULL};
    static const char * translation_texts[] = {
        "Dark theme", "Dunkles Design", "Tema oscuro", /* dark_theme */
        "Buttons", "Tasten", "Botones", /* buttons */
        "Controls", "Steuerelemente", "Controles", /* controls */
        "Text inputs", "Texteingaben", "Campos de texto", /* text_inputs */
        "Selection", "Auswahl", "Seleccion", /* selection */
    };
#endif

/**********************
 *  GLOBAL VARIABLES
 **********************/

/*--------------------
 *  Permanent screens
 *-------------------*/

/*----------------
 * Fonts
 *----------------*/

lv_font_t * hmi_f10;
extern lv_font_t hmi_f10_data;
lv_font_t * hmi_f12;
extern lv_font_t hmi_f12_data;
lv_font_t * hmi_f12b;
extern lv_font_t hmi_f12b_data;
lv_font_t * hmi_f14b;
extern lv_font_t hmi_f14b_data;
lv_font_t * hmi_f16b;
extern lv_font_t hmi_f16b_data;
lv_font_t * hmi_f22b;
extern lv_font_t hmi_f22b_data;
lv_font_t * hmi_f28b;
extern lv_font_t hmi_f28b_data;
lv_font_t * hmi_hero28;
extern lv_font_t hmi_hero28_data;

/*----------------
 * Images
 *----------------*/

/* Targets: any */
const void * hmi_icon_home_19 = NULL;
extern const void * hmi_icon_home_19_data;
const void * hmi_icon_gauge_19 = NULL;
extern const void * hmi_icon_gauge_19_data;
const void * hmi_icon_list_19 = NULL;
extern const void * hmi_icon_list_19_data;
const void * hmi_icon_chip_19 = NULL;
extern const void * hmi_icon_chip_19_data;
const void * hmi_icon_warn_12 = NULL;
extern const void * hmi_icon_warn_12_data;
const void * hmi_icon_warn_13 = NULL;
extern const void * hmi_icon_warn_13_data;
const void * hmi_icon_warn_14 = NULL;
extern const void * hmi_icon_warn_14_data;
const void * hmi_icon_warn_15 = NULL;
extern const void * hmi_icon_warn_15_data;
const void * hmi_icon_warn_16 = NULL;
extern const void * hmi_icon_warn_16_data;
const void * hmi_icon_warn_18 = NULL;
extern const void * hmi_icon_warn_18_data;
const void * hmi_icon_warn_19 = NULL;
extern const void * hmi_icon_warn_19_data;
const void * hmi_icon_warn_20 = NULL;
extern const void * hmi_icon_warn_20_data;
const void * hmi_icon_checkc_15 = NULL;
extern const void * hmi_icon_checkc_15_data;
const void * hmi_icon_checkc_18 = NULL;
extern const void * hmi_icon_checkc_18_data;
const void * hmi_icon_check_11 = NULL;
extern const void * hmi_icon_check_11_data;
const void * hmi_icon_check_12 = NULL;
extern const void * hmi_icon_check_12_data;
const void * hmi_icon_cross_10 = NULL;
extern const void * hmi_icon_cross_10_data;
const void * hmi_icon_relay_13 = NULL;
extern const void * hmi_icon_relay_13_data;
const void * hmi_icon_link_13 = NULL;
extern const void * hmi_icon_link_13_data;
const void * hmi_icon_clock_13 = NULL;
extern const void * hmi_icon_clock_13_data;
const void * hmi_icon_chev_10 = NULL;
extern const void * hmi_icon_chev_10_data;
const void * hmi_icon_bolt_13 = NULL;
extern const void * hmi_icon_bolt_13_data;
const void * hmi_icon_wave_13 = NULL;
extern const void * hmi_icon_wave_13_data;
const void * hmi_icon_power_13 = NULL;
extern const void * hmi_icon_power_13_data;
const void * hmi_icon_stop_16 = NULL;
extern const void * hmi_icon_stop_16_data;
const void * hmi_icon_play_16 = NULL;
extern const void * hmi_icon_play_16_data;
const void * hmi_icon_spin_16 = NULL;
extern const void * hmi_icon_spin_16_data;
const void * hmi_icon_spin_44 = NULL;
extern const void * hmi_icon_spin_44_data;
const void * hmi_icon_refresh_11 = NULL;
extern const void * hmi_icon_refresh_11_data;
const void * hmi_icon_back_14 = NULL;
extern const void * hmi_icon_back_14_data;
const void * hmi_icon_lock_11 = NULL;
extern const void * hmi_icon_lock_11_data;
const void * hmi_icon_shift_14 = NULL;
extern const void * hmi_icon_shift_14_data;
const void * hmi_icon_bksp_16 = NULL;
extern const void * hmi_icon_bksp_16_data;
const void * hmi_icon_eye_14 = NULL;
extern const void * hmi_icon_eye_14_data;
const void * hmi_icon_eyeoff_14 = NULL;
extern const void * hmi_icon_eyeoff_14_data;
const void * hmi_icon_wifi_13 = NULL;
extern const void * hmi_icon_wifi_13_data;
const void * hmi_icon_wifi_22 = NULL;
extern const void * hmi_icon_wifi_22_data;

/*----------------
 * Global styles
 *----------------*/

/*----------------
 * Subjects
 *----------------*/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lvgl_ui_init_gen(const char * asset_path)
{
    /* When running from the editor the theme set from the XML should overwrite this */
#if !defined(LV_EDITOR_PREVIEW)
#if LV_USE_THEME_SIMPLE
    lv_display_t * disp = lv_display_get_default();
    lv_theme_t * th = lv_theme_simple_init(disp);
    lv_display_set_theme(disp, th);
#else
    LV_LOG_WARN("Simple theme is selected in project.xml but LV_USE_THEME_SIMPLE is disabled");
#endif
#endif /*LV_EDITOR_PREVIEW*/


    /*----------------
     * Fonts
     *----------------*/

    /* Targets: any */

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        if (!hmi_f10) {
            /* hmi_f10 */
            /* get font 'hmi_f10' from a C array */
            hmi_f10 = &hmi_f10_data;

        }
        if (!hmi_f12) {
            /* hmi_f12 */
            /* get font 'hmi_f12' from a C array */
            hmi_f12 = &hmi_f12_data;

        }
        if (!hmi_f12b) {
            /* hmi_f12b */
            /* get font 'hmi_f12b' from a C array */
            hmi_f12b = &hmi_f12b_data;

        }
        if (!hmi_f14b) {
            /* hmi_f14b */
            /* get font 'hmi_f14b' from a C array */
            hmi_f14b = &hmi_f14b_data;

        }
        if (!hmi_f16b) {
            /* hmi_f16b */
            /* get font 'hmi_f16b' from a C array */
            hmi_f16b = &hmi_f16b_data;

        }
        if (!hmi_f22b) {
            /* hmi_f22b */
            /* get font 'hmi_f22b' from a C array */
            hmi_f22b = &hmi_f22b_data;

        }
        if (!hmi_f28b) {
            /* hmi_f28b */
            /* get font 'hmi_f28b' from a C array */
            hmi_f28b = &hmi_f28b_data;

        }
        if (!hmi_hero28) {
            /* hmi_hero28 */
            /* get font 'hmi_hero28' from a C array */
            hmi_hero28 = &hmi_hero28_data;

        }
    }
    #endif

    /*----------------
     * Images
     *----------------*/

    /* Targets: any */
    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        /* hmi_icon_home_19 */
        if (!hmi_icon_home_19) {
            hmi_icon_home_19 = &hmi_icon_home_19_data;
        }
        /* hmi_icon_gauge_19 */
        if (!hmi_icon_gauge_19) {
            hmi_icon_gauge_19 = &hmi_icon_gauge_19_data;
        }
        /* hmi_icon_list_19 */
        if (!hmi_icon_list_19) {
            hmi_icon_list_19 = &hmi_icon_list_19_data;
        }
        /* hmi_icon_chip_19 */
        if (!hmi_icon_chip_19) {
            hmi_icon_chip_19 = &hmi_icon_chip_19_data;
        }
        /* hmi_icon_warn_12 */
        if (!hmi_icon_warn_12) {
            hmi_icon_warn_12 = &hmi_icon_warn_12_data;
        }
        /* hmi_icon_warn_13 */
        if (!hmi_icon_warn_13) {
            hmi_icon_warn_13 = &hmi_icon_warn_13_data;
        }
        /* hmi_icon_warn_14 */
        if (!hmi_icon_warn_14) {
            hmi_icon_warn_14 = &hmi_icon_warn_14_data;
        }
        /* hmi_icon_warn_15 */
        if (!hmi_icon_warn_15) {
            hmi_icon_warn_15 = &hmi_icon_warn_15_data;
        }
        /* hmi_icon_warn_16 */
        if (!hmi_icon_warn_16) {
            hmi_icon_warn_16 = &hmi_icon_warn_16_data;
        }
        /* hmi_icon_warn_18 */
        if (!hmi_icon_warn_18) {
            hmi_icon_warn_18 = &hmi_icon_warn_18_data;
        }
        /* hmi_icon_warn_19 */
        if (!hmi_icon_warn_19) {
            hmi_icon_warn_19 = &hmi_icon_warn_19_data;
        }
        /* hmi_icon_warn_20 */
        if (!hmi_icon_warn_20) {
            hmi_icon_warn_20 = &hmi_icon_warn_20_data;
        }
        /* hmi_icon_checkc_15 */
        if (!hmi_icon_checkc_15) {
            hmi_icon_checkc_15 = &hmi_icon_checkc_15_data;
        }
        /* hmi_icon_checkc_18 */
        if (!hmi_icon_checkc_18) {
            hmi_icon_checkc_18 = &hmi_icon_checkc_18_data;
        }
        /* hmi_icon_check_11 */
        if (!hmi_icon_check_11) {
            hmi_icon_check_11 = &hmi_icon_check_11_data;
        }
        /* hmi_icon_check_12 */
        if (!hmi_icon_check_12) {
            hmi_icon_check_12 = &hmi_icon_check_12_data;
        }
        /* hmi_icon_cross_10 */
        if (!hmi_icon_cross_10) {
            hmi_icon_cross_10 = &hmi_icon_cross_10_data;
        }
        /* hmi_icon_relay_13 */
        if (!hmi_icon_relay_13) {
            hmi_icon_relay_13 = &hmi_icon_relay_13_data;
        }
        /* hmi_icon_link_13 */
        if (!hmi_icon_link_13) {
            hmi_icon_link_13 = &hmi_icon_link_13_data;
        }
        /* hmi_icon_clock_13 */
        if (!hmi_icon_clock_13) {
            hmi_icon_clock_13 = &hmi_icon_clock_13_data;
        }
        /* hmi_icon_chev_10 */
        if (!hmi_icon_chev_10) {
            hmi_icon_chev_10 = &hmi_icon_chev_10_data;
        }
        /* hmi_icon_bolt_13 */
        if (!hmi_icon_bolt_13) {
            hmi_icon_bolt_13 = &hmi_icon_bolt_13_data;
        }
        /* hmi_icon_wave_13 */
        if (!hmi_icon_wave_13) {
            hmi_icon_wave_13 = &hmi_icon_wave_13_data;
        }
        /* hmi_icon_power_13 */
        if (!hmi_icon_power_13) {
            hmi_icon_power_13 = &hmi_icon_power_13_data;
        }
        /* hmi_icon_stop_16 */
        if (!hmi_icon_stop_16) {
            hmi_icon_stop_16 = &hmi_icon_stop_16_data;
        }
        /* hmi_icon_play_16 */
        if (!hmi_icon_play_16) {
            hmi_icon_play_16 = &hmi_icon_play_16_data;
        }
        /* hmi_icon_spin_16 */
        if (!hmi_icon_spin_16) {
            hmi_icon_spin_16 = &hmi_icon_spin_16_data;
        }
        /* hmi_icon_spin_44 */
        if (!hmi_icon_spin_44) {
            hmi_icon_spin_44 = &hmi_icon_spin_44_data;
        }
        /* hmi_icon_refresh_11 */
        if (!hmi_icon_refresh_11) {
            hmi_icon_refresh_11 = &hmi_icon_refresh_11_data;
        }
        /* hmi_icon_back_14 */
        if (!hmi_icon_back_14) {
            hmi_icon_back_14 = &hmi_icon_back_14_data;
        }
        /* hmi_icon_lock_11 */
        if (!hmi_icon_lock_11) {
            hmi_icon_lock_11 = &hmi_icon_lock_11_data;
        }
        /* hmi_icon_shift_14 */
        if (!hmi_icon_shift_14) {
            hmi_icon_shift_14 = &hmi_icon_shift_14_data;
        }
        /* hmi_icon_bksp_16 */
        if (!hmi_icon_bksp_16) {
            hmi_icon_bksp_16 = &hmi_icon_bksp_16_data;
        }
        /* hmi_icon_eye_14 */
        if (!hmi_icon_eye_14) {
            hmi_icon_eye_14 = &hmi_icon_eye_14_data;
        }
        /* hmi_icon_eyeoff_14 */
        if (!hmi_icon_eyeoff_14) {
            hmi_icon_eyeoff_14 = &hmi_icon_eyeoff_14_data;
        }
        /* hmi_icon_wifi_13 */
        if (!hmi_icon_wifi_13) {
            hmi_icon_wifi_13 = &hmi_icon_wifi_13_data;
        }
        /* hmi_icon_wifi_22 */
        if (!hmi_icon_wifi_22) {
            hmi_icon_wifi_22 = &hmi_icon_wifi_22_data;
        }
    }
    #endif

    /*----------------
     * Global styles
     *----------------*/

    /*----------------
     * Subjects
     *----------------*/
    /*----------------
     * Translations
     *----------------*/

    #ifndef LV_EDITOR_PREVIEW
        lv_translation_add_static(translation_languages, translation_tags, translation_texts);
        lv_translation_set_language(translation_languages[0]);
    #endif

#if defined(LV_USE_XML) && LV_USE_XML
    /* Register widgets */

    /* Check all fonts / default if needed. This prevents fonts that are used in one target but
       defined in another from causing assertion failures during rendering of the Preview. */
    check_font(&hmi_f10, "hmi_f10");
    check_font(&hmi_f12, "hmi_f12");
    check_font(&hmi_f12b, "hmi_f12b");
    check_font(&hmi_f14b, "hmi_f14b");
    check_font(&hmi_f16b, "hmi_f16b");
    check_font(&hmi_f22b, "hmi_f22b");
    check_font(&hmi_f28b, "hmi_f28b");
    check_font(&hmi_hero28, "hmi_hero28");

    /* Register fonts */
    lv_xml_register_font(NULL, "hmi_f10", hmi_f10);
    lv_xml_register_font(NULL, "hmi_f12", hmi_f12);
    lv_xml_register_font(NULL, "hmi_f12b", hmi_f12b);
    lv_xml_register_font(NULL, "hmi_f14b", hmi_f14b);
    lv_xml_register_font(NULL, "hmi_f16b", hmi_f16b);
    lv_xml_register_font(NULL, "hmi_f22b", hmi_f22b);
    lv_xml_register_font(NULL, "hmi_f28b", hmi_f28b);
    lv_xml_register_font(NULL, "hmi_hero28", hmi_hero28);

    /* Register subjects */

    /* Register callbacks */

    /* Register easing callbacks */
#endif

    /* Register all the global assets so that they won't be created again when globals.xml is parsed.
     * While running in the editor skip this step to update the preview when the XML changes */
#if defined(LV_USE_XML) && LV_USE_XML && !defined(LV_EDITOR_PREVIEW)
    /* Register images */
    lv_xml_register_image(NULL, "hmi_icon_home_19", hmi_icon_home_19);
    lv_xml_register_image(NULL, "hmi_icon_gauge_19", hmi_icon_gauge_19);
    lv_xml_register_image(NULL, "hmi_icon_list_19", hmi_icon_list_19);
    lv_xml_register_image(NULL, "hmi_icon_chip_19", hmi_icon_chip_19);
    lv_xml_register_image(NULL, "hmi_icon_warn_12", hmi_icon_warn_12);
    lv_xml_register_image(NULL, "hmi_icon_warn_13", hmi_icon_warn_13);
    lv_xml_register_image(NULL, "hmi_icon_warn_14", hmi_icon_warn_14);
    lv_xml_register_image(NULL, "hmi_icon_warn_15", hmi_icon_warn_15);
    lv_xml_register_image(NULL, "hmi_icon_warn_16", hmi_icon_warn_16);
    lv_xml_register_image(NULL, "hmi_icon_warn_18", hmi_icon_warn_18);
    lv_xml_register_image(NULL, "hmi_icon_warn_19", hmi_icon_warn_19);
    lv_xml_register_image(NULL, "hmi_icon_warn_20", hmi_icon_warn_20);
    lv_xml_register_image(NULL, "hmi_icon_checkc_15", hmi_icon_checkc_15);
    lv_xml_register_image(NULL, "hmi_icon_checkc_18", hmi_icon_checkc_18);
    lv_xml_register_image(NULL, "hmi_icon_check_11", hmi_icon_check_11);
    lv_xml_register_image(NULL, "hmi_icon_check_12", hmi_icon_check_12);
    lv_xml_register_image(NULL, "hmi_icon_cross_10", hmi_icon_cross_10);
    lv_xml_register_image(NULL, "hmi_icon_relay_13", hmi_icon_relay_13);
    lv_xml_register_image(NULL, "hmi_icon_link_13", hmi_icon_link_13);
    lv_xml_register_image(NULL, "hmi_icon_clock_13", hmi_icon_clock_13);
    lv_xml_register_image(NULL, "hmi_icon_chev_10", hmi_icon_chev_10);
    lv_xml_register_image(NULL, "hmi_icon_bolt_13", hmi_icon_bolt_13);
    lv_xml_register_image(NULL, "hmi_icon_wave_13", hmi_icon_wave_13);
    lv_xml_register_image(NULL, "hmi_icon_power_13", hmi_icon_power_13);
    lv_xml_register_image(NULL, "hmi_icon_stop_16", hmi_icon_stop_16);
    lv_xml_register_image(NULL, "hmi_icon_play_16", hmi_icon_play_16);
    lv_xml_register_image(NULL, "hmi_icon_spin_16", hmi_icon_spin_16);
    lv_xml_register_image(NULL, "hmi_icon_spin_44", hmi_icon_spin_44);
    lv_xml_register_image(NULL, "hmi_icon_refresh_11", hmi_icon_refresh_11);
    lv_xml_register_image(NULL, "hmi_icon_back_14", hmi_icon_back_14);
    lv_xml_register_image(NULL, "hmi_icon_lock_11", hmi_icon_lock_11);
    lv_xml_register_image(NULL, "hmi_icon_shift_14", hmi_icon_shift_14);
    lv_xml_register_image(NULL, "hmi_icon_bksp_16", hmi_icon_bksp_16);
    lv_xml_register_image(NULL, "hmi_icon_eye_14", hmi_icon_eye_14);
    lv_xml_register_image(NULL, "hmi_icon_eyeoff_14", hmi_icon_eyeoff_14);
    lv_xml_register_image(NULL, "hmi_icon_wifi_13", hmi_icon_wifi_13);
    lv_xml_register_image(NULL, "hmi_icon_wifi_22", hmi_icon_wifi_22);
#endif

#if !defined(LV_USE_XML) || LV_USE_XML == 0
    /*--------------------
     *  Permanent screens
     *-------------------*/
    /* If XML is enabled it's assumed that the permanent screens are created
     * manually from XML using lv_xml_create() */
#endif
}

void lvgl_ui_set_target(uint32_t target)
{
    lvgl_ui_target = target;
}

uint32_t lvgl_ui_get_target(void)
{
    return lvgl_ui_target;
}

bool lvgl_ui_check_target(uint32_t target)
{
    return (lvgl_ui_target & target) ? true : false;
}

/* Callbacks */

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void check_font(lv_font_t ** font, const char * name)
{
    if (!(*font)) {
        *font = (lv_font_t *)LV_FONT_DEFAULT;
        LV_LOG_WARN("font `%s` was not set. Using `LV_FONT_DEFAULT` instead", name);
    }
}