#pragma once
/* Wi-Fi station backend used by the HMI "Wi-Fi" screens (hmi_ui.c).
 * The UI only polls hmi_wifi_get() and issues the commands below; all of them return quickly.
 * `version` must change whenever a command is accepted or the state changes: the UI re-renders on it.
 * hmi_wifi.c is the esp_wifi implementation (call hmi_wifi_init() once after NVS is ready; until then
 * hmi_wifi_get() reports HMI_WIFI_UNAVAILABLE). The PC simulator uses tools/sim/sim_wifi.c instead. */
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define HMI_WIFI_MAX_AP 12
#define HMI_WIFI_PASS_MIN 8
#define HMI_WIFI_PASS_MAX 63

typedef enum {
    HMI_WIFI_UNAVAILABLE = 0, /* no Wi-Fi driver in this build */
    HMI_WIFI_IDLE,            /* not connected, scan results (if any) valid */
    HMI_WIFI_SCANNING,
    HMI_WIFI_CONNECTING,      /* step: 0 associating, 1 authenticating, 2 obtaining IP */
    HMI_WIFI_CONNECTED,
    HMI_WIFI_FAILED,          /* fail_reason valid */
} hmi_wifi_state_t;

typedef enum {
    HMI_WIFI_FAIL_NONE = 0,
    HMI_WIFI_FAIL_PASSWORD,   /* wrong password / 4-way handshake failed */
    HMI_WIFI_FAIL_NO_AP,      /* SSID not found */
    HMI_WIFI_FAIL_NO_IP,      /* associated but DHCP timed out */
    HMI_WIFI_FAIL_OTHER,
} hmi_wifi_fail_t;

typedef struct {
    char ssid[33];
    int8_t rssi;     /* dBm */
    uint8_t secure;  /* 0 = open */
} hmi_wifi_ap_t;

typedef struct {
    hmi_wifi_state_t state;
    uint8_t step;
    uint8_t attempt;      /* 1.. while connecting / after failure */
    uint8_t fail_reason;  /* hmi_wifi_fail_t */
    uint8_t ap_count;
    hmi_wifi_ap_t ap[HMI_WIFI_MAX_AP];  /* sorted by RSSI, strongest first */
    char ssid[33];        /* connected or target network */
    char ip[16], gateway[16], mac[18];
    int8_t rssi;          /* link RSSI while connected */
    uint32_t version;     /* incremented on every change */
} hmi_wifi_status_t;

/* Starts the station and auto-connects to the credentials saved by the last successful connection.
 * Needs NVS (hmi_settings_init) and enough free internal RAM; returns false on failure (UI shows unavailable). */
bool hmi_wifi_init(void);
void hmi_wifi_get(hmi_wifi_status_t *out);
void hmi_wifi_scan(void);
void hmi_wifi_connect(const char *ssid, const char *password); /* password "" for open networks */
void hmi_wifi_cancel(void);      /* abort CONNECTING */
void hmi_wifi_disconnect(void);
void hmi_wifi_forget(void);

#ifdef __cplusplus
}
#endif
