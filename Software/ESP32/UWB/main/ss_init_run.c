#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "deca_device_api.h"
#include "deca_regs.h"
#include "ss_init_run.h"
#include "esp_log.h"
#include "config.h"

#include <string.h>

// typedef uint8_t  uint8;
// typedef uint16_t uint16;
// typedef uint32_t uint32;
// typedef uint64_t uint64;

static const char *TAG = "BU01_DRIVER";

/* Frames used in the ranging process. See NOTE 1,2 below. */
static uint8 tx_poll_msg[] = {0x41, 0x88, 0, 0xCA, 0xDE, 'W', 'A', 'V', 'E', 0xE0, 0, 0};
static uint8 rx_resp_msg[] = {0x41, 0x88, 0, 0xCA, 0xDE, 'V', 'E', 'W', 'A', 0xE1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
#define ALL_MSG_COMMON_LEN 10

/* Frame sequence number, incremented after each transmission. */
static uint8 frame_seq_nb = 0;

/* Buffer to store received response message.
* Its size is adjusted to longest frame that this example code is supposed to handle. */
static uint8 rx_buffer[RX_BUF_LEN];

/* Hold copies of computed time of flight and distance here for reference so that it can be examined at a debug breakpoint. */
static double tof;
static double distance;

/*Transactions Counters */
static volatile int tx_count = 0 ; // Successful transmit counter
static volatile int rx_count = 0 ; // Successful receive counter 

static uint32_t status_reg;

void ss_init_run() {
    tx_poll_msg[ALL_MSG_SN_IDX] = frame_seq_nb; 
    dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS);
    dwt_writetxdata(sizeof(tx_poll_msg), tx_poll_msg, 0); /* Zero offset in TX buffer. */
    dwt_writetxfctrl(sizeof(tx_poll_msg), 0, 1); /* Zero offset in TX buffer, ranging. */

    dwt_starttx(DWT_START_TX_IMMEDIATE | DWT_RESPONSE_EXPECTED);
    tx_count++;
    ESP_LOGI(TAG, "Transmission # : %d\r\n", tx_count);

    /* We assume that the transmission is achieved correctly, poll for reception of a frame or error/timeout. See NOTE 4 below. */
    while (!((status_reg = dwt_read32bitreg(SYS_STATUS_ID)) & (SYS_STATUS_RXFCG | SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR)));

    frame_seq_nb++;

    if (status_reg & SYS_STATUS_RXFCG) {
        uint32_t frame_len;

        // Write Receiver FC Good to status register
        dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG);
        
        // get length of frame from RX_FINFO register
        frame_len = dwt_read32bitreg(RX_FINFO_ID) & RX_FINFO_RXFLEN_MASK;

        if (frame_len <= RX_BUF_LEN) {
            dwt_readrxdata(rx_buffer, frame_len, 0);
        }

        rx_buffer[ALL_MSG_SN_IDX] = 0;
        if(memcmp(rx_buffer, rx_resp_msg, ALL_MSG_COMMON_LEN) == 0) {
            rx_count++;
            ESP_LOGI(TAG, "Reception #%d\n", rx_count);

            uint32_t poll_tx_ts, resp_rx_ts, poll_rx_ts, resp_tx_ts;
            int32_t rtd_init, rtd_resp;
            float clockOffsetRatio;

            // Get the lower 32 bits of transmission and response reception timestamps
            poll_tx_ts = dwt_readtxtimestamplo32();
            resp_rx_ts = dwt_readrxtimestamplo32();

            // Get the misalignment in the clocks
            clockOffsetRatio = dwt_readcarrierintegrator() * (FREQ_OFFSET_MULTIPLIER * HERTZ_TO_PPM_MULTIPLIER_CHAN_5 / 1.0e6);
            
            /* Get timestamps embedded in response message. */
            resp_msg_get_ts(&rx_buffer[RESP_MSG_POLL_RX_TS_IDX], &poll_rx_ts);
            resp_msg_get_ts(&rx_buffer[RESP_MSG_RESP_TX_TS_IDX], &resp_tx_ts);

            /* Compute time of flight and distance, using clock offset ratio to correct for differing local and remote clock rates */
            rtd_init = resp_rx_ts - poll_tx_ts;
            rtd_resp = resp_tx_ts - poll_rx_ts;

            tof = ((rtd_init - rtd_resp * (1.0f - clockOffsetRatio)) / 2.0f) * DWT_TIME_UNITS; // Specifying 1.0f and 2.0f are floats to clear warning 
            distance = tof * SPEED_OF_LIGHT;
            printf("Distance : %f\r\n",distance);
        }
    } else {
        // Clear any errors in the SYS_STATUS register and reset rx
        dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR);
        dwt_rxreset();
    }
}

void ss_initiator_task_function(void * pvParameter) {
    while(1) {
        ss_init_run();
        vTaskDelay(RNG_DELAY_MS);
    }
}
