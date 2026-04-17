#include "config.h"

#define RNG_DELAY_MS 100

/* UWB microsecond (uus) to device time unit (dtu, around 15.65 ps) conversion factor.
* 1 uus = 512 / 499.2 µs and 1 µs = 499.2 * 128 dtu. */
#define UUS_TO_DWT_TIME 65536

/* Receiving buffer length*/
#define RX_BUF_LEN 20

/* Indexes to access some of the fields in the frames defined in ss_init_run.c */
#define ALL_MSG_SN_IDX 2
#define RESP_MSG_POLL_RX_TS_IDX 10
#define RESP_MSG_RESP_TX_TS_IDX 14
#define RESP_MSG_TS_LEN 4

/* Speed of light in air, in metres per second. */
#define SPEED_OF_LIGHT 299702547

/* Declaration of static functions. */
void ss_init_run(void * pvParameters);
static void resp_msg_get_ts(uint8_t *ts_field, uint32_t *ts);
void ss_initiator_task_function(void * pvParameter);
