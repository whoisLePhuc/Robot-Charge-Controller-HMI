#pragma once
#include <stddef.h>
#include <stdint.h>
typedef int esp_err_t;
#define ESP_OK 0
#define ESP_ERR_INVALID_STATE 1
#define ESP_ERR_NO_MEM 2
#define GPIO_NUM_43 43
#define GPIO_NUM_44 44
#define UART_NUM_1 1
#define UART_DATA_8_BITS 8
#define UART_PARITY_DISABLE 0
#define UART_STOP_BITS_1 1
#define UART_HW_FLOWCTRL_DISABLE 0
#define UART_SCLK_DEFAULT 0
#define UART_PIN_NO_CHANGE -1
#define pdMS_TO_TICKS(x) (x)
#define pdTRUE 1
#define pdPASS 1
#define portMAX_DELAY 0xffffffff
 typedef void *SemaphoreHandle_t;
typedef struct {int baud_rate,data_bits,parity,stop_bits,flow_ctrl,source_clk;} uart_config_t;
static int64_t test_now=1000000;
static int64_t esp_timer_get_time(void){return test_now;}
static int xSemaphoreTake(void *x,unsigned t){(void)x;(void)t;return 1;}
static void xSemaphoreGive(void *x){(void)x;}
static void *xSemaphoreCreateMutex(void){return (void*)1;}
static void vSemaphoreDelete(void*x){(void)x;}
static uint32_t esp_random(void){return 123;}
static int uart_write_bytes(int a,const void*b,size_t n){(void)a;(void)b;return (int)n;}
static int uart_read_bytes(int a,void*b,size_t n,unsigned t){(void)a;(void)b;(void)n;(void)t;return 0;}
static int uart_param_config(int a,const uart_config_t*b){(void)a;(void)b;return 0;}
static int uart_set_pin(int a,int b,int c,int d,int e){(void)a;(void)b;(void)c;(void)d;(void)e;return 0;}
static int uart_driver_install(int a,int b,int c,int d,void*e,int f){(void)a;(void)b;(void)c;(void)d;(void)e;(void)f;return 0;}
static int uart_driver_delete(int a){(void)a;return 0;}
static int xTaskCreate(void(*a)(void*),const char*b,int c,void*d,int e,void*f){(void)a;(void)b;(void)c;(void)d;(void)e;(void)f;return 1;}
static void esp_task_wdt_add(void*x){(void)x;}
static void esp_task_wdt_reset(void){}
