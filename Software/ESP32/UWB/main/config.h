#include <stdint.h>

#define ESP_SPI_MISO    12
#define ESP_SPI_MOSI    11
#define ESP_SPI_CS       9
#define ESP_SPI_SCLK    10

#define ESP_TX_PIN      17
#define ESP_RX_PIN      18

#define DW_IRQ_PIN      15

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

void setup_spi();
void setup_uart();
void setup_irq();
