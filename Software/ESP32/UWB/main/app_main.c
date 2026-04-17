#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "deca_types.h"
#include "deca_param_types.h"
#include "deca_regs.h"
#include "deca_device_api.h"
#include "port_platform.h"
#include "ss_init_run.h"
#include "esp_log.h"
#include "config.h"

static const char *TAG = "BU01_DRIVER";

static dwt_config_t config = {
    5,                /* Channel number. */
    DWT_PRF_64M,      /* Pulse repetition frequency. */
    DWT_PLEN_128,     /* Preamble length. Used in TX only. */
    DWT_PAC8,         /* Preamble acquisition chunk size. Used in RX only. */
    10,               /* TX preamble code. Used in TX only. */
    10,               /* RX preamble code. Used in RX only. */
    0,                /* 0 to use standard SFD, 1 to use non-standard SFD. */
    DWT_BR_6M8,       /* Data rate. */
    DWT_PHRMODE_STD,  /* PHY header mode. */
    (129 + 8 - 8)     /* SFD timeout (preamble length + 1 + SFD length - PAC size). Used in RX only. */
};

/* Preamble timeout, in multiple of PAC size. See NOTE 3 below. */
#define PRE_TIMEOUT 1000

/* Delay between frames, in UWB microseconds. See NOTE 1 below. */
#define POLL_TX_TO_RESP_RX_DLY_UUS 100 

/*Should be accurately calculated during calibration*/
#define TX_ANT_DLY 16456
#define RX_ANT_DLY 16456	

#define TASK_DELAY        200           /**< Task delay. Delays a LED0 task for 200 ms */
#define TIMER_PERIOD      2000          /**< Timer period. LED1 timer will expire after 1000 ms */

//-------------------------------------
// This function comes from ss_init_run.c
// and declared in ss_init_run.h
//-------------------------------------
// extern void ss_init_run(void * pvParameters);

int app_main(void) {
    setup_uart();
    setup_spi();
    ESP_LOGI(TAG, "Initialized UART and SPI to drive the BU01.");

    setup_irq();
    ESP_LOGI(TAG, "Initialized IRQ on pin DW_IRQ_PIN");

    dwt_softreset();    // Reset the BU01 before configuring
    
    if(dwt_initialise(DWT_LOADUCODE) == DWT_ERROR) {
        ESP_LOGE(TAG, "Error while initializing the BU01. Please reset.");
        return 1;
    }

    dwt_configure(&config);

    dwt_setrxantennadelay(RX_ANT_DLY);
    dwt_settxantennadelay(TX_ANT_DLY);

    dwt_setrxaftertxdelay(POLL_TX_TO_RESP_RX_DLY_UUS);
    dwt_setrxtimeout(65000); // Maximum value timeout with DW1000 is 65ms
    
    xTaskCreate(&ss_init_run, "Ranging_Function", 2048, NULL, 5, NULL);
    
    return 0;
}
