/**
 * @file lvgl_ui_gen.h
 */

#ifndef LVGL_PRO_LVGL_UI_GEN_H
#define LVGL_PRO_LVGL_UI_GEN_H

#ifndef UI_SUBJECT_STRING_LENGTH
#define UI_SUBJECT_STRING_LENGTH 256
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
#endif

#if defined(LV_USE_XML) && LV_USE_XML
    #include "lv_xml/lv_xml.h"
#endif



/* Prototypes for target functions, needed by responsive const definitions */

void lvgl_ui_set_target(uint32_t target);
uint32_t lvgl_ui_get_target(void);
bool lvgl_ui_check_target(uint32_t target);

/*********************
 *      DEFINES
 *********************/

#define LVGL_UI_TARGET_UNDEFINED  (0 << 1)
#define LVGL_UI_TARGET_TARGET1    (1 << 1)
#define LVGL_UI_TARGET_ALL        0x0FFFFFFF

/* By default compile for all targets, allowing to switch to any targets at runtime */
#ifndef LVGL_UI_COMPILE_TARGET
#define LVGL_UI_COMPILE_TARGET LVGL_UI_TARGET_ALL
#endif

#define LVGL_UI_CHECK_COMPILE_TARGET(target) (LVGL_UI_COMPILE_TARGET & (target) ? 1 : 0)

#ifndef LV_XML_EVAL_STRING_BUF_SIZE
    #define LV_XML_EVAL_STRING_BUF_SIZE 256
#endif

/**
 * Animation path of a style transition with a `bezier(…)` easing. The control
 * points come from the transition descriptor's `user_data`, as a transition
 * descriptor cannot store animation parameters itself.
 */
static inline int32_t lvgl_ui_style_transition_bezier_path_helper(const lv_anim_t * a)
{
    const lv_anim_bezier3_para_t * para = (const lv_anim_bezier3_para_t *)lv_anim_get_user_data(a);
    if(para == NULL) return lv_anim_path_linear(a);

    lv_anim_t a_tmp = *a;
    lv_anim_set_bezier3_param(&a_tmp, para->x1, para->y1, para->x2, para->y2);
    return lv_anim_path_custom_bezier3(&a_tmp);
}

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL VARIABLES
 **********************/

/*-------------------
 * Permanent screens
 *------------------*/

/*----------------
 * Global styles
 *----------------*/

/*----------------
 * Fonts
 *----------------*/

/* Targets: any */
extern lv_font_t * hmi_f10;
extern lv_font_t * hmi_f12;
extern lv_font_t * hmi_f12b;
extern lv_font_t * hmi_f14b;
extern lv_font_t * hmi_f16b;
extern lv_font_t * hmi_f22b;
extern lv_font_t * hmi_f28b;
extern lv_font_t * hmi_hero28;


/*----------------
 * Images
 *----------------*/

/* Targets: any */
extern const void * hmi_icon_home_19;
extern const void * hmi_icon_gauge_19;
extern const void * hmi_icon_list_19;
extern const void * hmi_icon_chip_19;
extern const void * hmi_icon_warn_12;
extern const void * hmi_icon_warn_13;
extern const void * hmi_icon_warn_14;
extern const void * hmi_icon_warn_15;
extern const void * hmi_icon_warn_16;
extern const void * hmi_icon_warn_18;
extern const void * hmi_icon_warn_19;
extern const void * hmi_icon_warn_20;
extern const void * hmi_icon_checkc_15;
extern const void * hmi_icon_checkc_18;
extern const void * hmi_icon_check_11;
extern const void * hmi_icon_check_12;
extern const void * hmi_icon_cross_10;
extern const void * hmi_icon_relay_13;
extern const void * hmi_icon_link_13;
extern const void * hmi_icon_clock_13;
extern const void * hmi_icon_chev_10;
extern const void * hmi_icon_bolt_13;
extern const void * hmi_icon_wave_13;
extern const void * hmi_icon_power_13;
extern const void * hmi_icon_stop_16;
extern const void * hmi_icon_play_16;
extern const void * hmi_icon_spin_16;
extern const void * hmi_icon_spin_44;
extern const void * hmi_icon_refresh_11;
extern const void * hmi_icon_back_14;
extern const void * hmi_icon_lock_11;
extern const void * hmi_icon_shift_14;
extern const void * hmi_icon_bksp_16;
extern const void * hmi_icon_eye_14;
extern const void * hmi_icon_eyeoff_14;
extern const void * hmi_icon_wifi_13;
extern const void * hmi_icon_wifi_22;

/*----------------
 * Subjects
 *----------------*/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*----------------
 * Event Callbacks
 *----------------*/

/*----------------
 * Easing Callbacks
 *----------------*/

/**
 * Initialize the component library
 */

void lvgl_ui_init_gen(const char * asset_path);

/**********************
 *      MACROS
 **********************/

/**********************
 *   POST INCLUDES
 **********************/

/*Include all the widgets, components and screens of this library*/
#include "components/hmi/rcc_device_banner/rcc_device_banner_gen.h"
#include "components/hmi/rcc_device_comm/rcc_device_comm_gen.h"
#include "components/hmi/rcc_device_controller/rcc_device_controller_gen.h"
#include "components/hmi/rcc_device_settings/rcc_device_settings_gen.h"
#include "components/hmi/rcc_device_tabs/rcc_device_tabs_gen.h"
#include "components/hmi/rcc_faults_mask/rcc_faults_mask_gen.h"
#include "components/hmi/rcc_faults_primary/rcc_faults_primary_gen.h"
#include "components/hmi/rcc_faults_register/rcc_faults_register_gen.h"
#include "components/hmi/rcc_faults_row/rcc_faults_row_gen.h"
#include "components/hmi/rcc_faults_summary/rcc_faults_summary_gen.h"
#include "components/hmi/rcc_measure_channel/rcc_measure_channel_gen.h"
#include "components/hmi/rcc_measure_floor_badge/rcc_measure_floor_badge_gen.h"
#include "components/hmi/rcc_measure_matrix/rcc_measure_matrix_gen.h"
#include "components/hmi/rcc_measure_quality_badge/rcc_measure_quality_badge_gen.h"
#include "components/hmi/rcc_measure_row_heading/rcc_measure_row_heading_gen.h"
#include "components/hmi/rcc_measure_text_cell/rcc_measure_text_cell_gen.h"
#include "components/hmi/rcc_overview_block/rcc_overview_block_gen.h"
#include "components/hmi/rcc_overview_connector/rcc_overview_connector_gen.h"
#include "components/hmi/rcc_overview_hero/rcc_overview_hero_gen.h"
#include "components/hmi/rcc_overview_step/rcc_overview_step_gen.h"
#include "components/hmi/rcc_overview_system/rcc_overview_system_gen.h"
#include "components/hmi/rcc_session_detail_icon/rcc_session_detail_icon_gen.h"
#include "components/hmi/rcc_session_detail/rcc_session_detail_gen.h"
#include "components/hmi/rcc_session_event_row/rcc_session_event_row_gen.h"
#include "components/hmi/rcc_session_events/rcc_session_events_gen.h"
#include "components/hmi/rcc_session_summary/rcc_session_summary_gen.h"
#include "components/hmi/rcc_shell_action_status/rcc_shell_action_status_gen.h"
#include "components/hmi/rcc_shell_action/rcc_shell_action_gen.h"
#include "components/hmi/rcc_shell_banner/rcc_shell_banner_gen.h"
#include "components/hmi/rcc_shell_confirm/rcc_shell_confirm_gen.h"
#include "components/hmi/rcc_shell_frame/rcc_shell_frame_gen.h"
#include "components/hmi/rcc_shell_header_fault/rcc_shell_header_fault_gen.h"
#include "components/hmi/rcc_shell_header_link/rcc_shell_header_link_gen.h"
#include "components/hmi/rcc_shell_header_node/rcc_shell_header_node_gen.h"
#include "components/hmi/rcc_shell_no_authority/rcc_shell_no_authority_gen.h"
#include "components/hmi/rcc_shell_ota/rcc_shell_ota_gen.h"
#include "components/hmi/rcc_shell_overlay/rcc_shell_overlay_gen.h"
#include "components/hmi/rcc_shell_rail_badge/rcc_shell_rail_badge_gen.h"
#include "components/hmi/rcc_shell_rail_tab/rcc_shell_rail_tab_gen.h"
#include "components/hmi/rcc_shell_rail/rcc_shell_rail_gen.h"
#include "components/hmi/rcc_telemetry_card/rcc_telemetry_card_gen.h"
#include "components/hmi/rcc_wifi_busy/rcc_wifi_busy_gen.h"
#include "components/hmi/rcc_wifi_fail/rcc_wifi_fail_gen.h"
#include "components/hmi/rcc_wifi_header/rcc_wifi_header_gen.h"
#include "components/hmi/rcc_wifi_info/rcc_wifi_info_gen.h"
#include "components/hmi/rcc_wifi_list/rcc_wifi_list_gen.h"
#include "components/hmi/rcc_wifi_pass/rcc_wifi_pass_gen.h"
#include "components/hmi/rcc_wifi_row/rcc_wifi_row_gen.h"
#include "screens/hmi_command_accepted_gen.h"
#include "screens/hmi_command_noauth_gen.h"
#include "screens/hmi_command_queuefull_gen.h"
#include "screens/hmi_command_stopped_gen.h"
#include "screens/hmi_command_stopping_gen.h"
#include "screens/hmi_command_timeout_gen.h"
#include "screens/hmi_confirm_gen.h"
#include "screens/hmi_device_fault_gen.h"
#include "screens/hmi_device_gen.h"
#include "screens/hmi_device_idle_gen.h"
#include "screens/hmi_device_inhibited_gen.h"
#include "screens/hmi_device_link_gen.h"
#include "screens/hmi_device_offline_gen.h"
#include "screens/hmi_device_precheck_gen.h"
#include "screens/hmi_device_remote_gen.h"
#include "screens/hmi_device_settings_gen.h"
#include "screens/hmi_faults_fault_gen.h"
#include "screens/hmi_faults_gen.h"
#include "screens/hmi_faults_idle_gen.h"
#include "screens/hmi_faults_inhibited_gen.h"
#include "screens/hmi_faults_offline_gen.h"
#include "screens/hmi_faults_precheck_gen.h"
#include "screens/hmi_faults_remote_gen.h"
#include "screens/hmi_measure_fault_gen.h"
#include "screens/hmi_measure_gen.h"
#include "screens/hmi_measure_idle_gen.h"
#include "screens/hmi_measure_inhibited_gen.h"
#include "screens/hmi_measure_offline_gen.h"
#include "screens/hmi_measure_precheck_gen.h"
#include "screens/hmi_measure_remote_gen.h"
#include "screens/hmi_ota_receiving_gen.h"
#include "screens/hmi_ota_success_gen.h"
#include "screens/hmi_ota_verifying_gen.h"
#include "screens/hmi_overview_fault_gen.h"
#include "screens/hmi_overview_gen.h"
#include "screens/hmi_overview_idle_gen.h"
#include "screens/hmi_overview_inhibited_gen.h"
#include "screens/hmi_overview_offline_gen.h"
#include "screens/hmi_overview_precheck_gen.h"
#include "screens/hmi_overview_remote_gen.h"
#include "screens/hmi_session_fault_gen.h"
#include "screens/hmi_session_gen.h"
#include "screens/hmi_session_idle_gen.h"
#include "screens/hmi_session_inhibited_gen.h"
#include "screens/hmi_session_offline_gen.h"
#include "screens/hmi_session_precheck_gen.h"
#include "screens/hmi_session_remote_gen.h"
#include "screens/hmi_wifi_busy_gen.h"
#include "screens/hmi_wifi_busy2_gen.h"
#include "screens/hmi_wifi_done_gen.h"
#include "screens/hmi_wifi_fail_gen.h"
#include "screens/hmi_wifi_info_gen.h"
#include "screens/hmi_wifi_list_gen.h"
#include "screens/hmi_wifi_noap_gen.h"
#include "screens/hmi_wifi_noip_gen.h"
#include "screens/hmi_wifi_open_gen.h"
#include "screens/hmi_wifi_pass_gen.h"
#include "screens/hmi_wifi_scroll_gen.h"
#include "screens/hmi_wifi_shift_gen.h"
#include "screens/hmi_wifi_show_gen.h"
#include "screens/hmi_wifi_sym_gen.h"
#include "screens/hmi_wifi_taps_gen.h"

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_LVGL_UI_GEN_H*/