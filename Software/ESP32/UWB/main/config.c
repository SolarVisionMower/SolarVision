#include "config.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "driver/spi_master.h"

// SPI Device Handle
spi_device_handle_t spi;

void setup_spi() {
    spi_bus_config_t buscfg = {
        .miso_io_num = ESP_SPI_MISO,
        .mosi_io_num = ESP_SPI_MOSI,
        .sclk_io_num = ESP_SPI_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4096
    };

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 1000000, // 1 MHz
        .mode = 0,                 // SPI mode 0
        .spics_io_num = 9,         // CS pin
        .queue_size = 7
    };

    // Initialize the SPI bus
    spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    // Add the device to the bus
    spi_bus_add_device(SPI2_HOST, &devcfg, &spi);
}

void setup_uart() {
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    // Configure UART parameters
    uart_param_config(UART_NUM_1, &uart_config);
    
    // Set UART pins (tx, rx, rts, cts)
    uart_set_pin(UART_NUM_1, ESP_TX_PIN, ESP_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    
    // Install UART driver with buffer sizes
    uart_driver_install(UART_NUM_1, 1024 * 2, 0, 0, NULL, 0);
}

void setup_irq() {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << DW_IRQ_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE, // DW1000 IRQ is usually active high
        .intr_type = GPIO_INTR_POSEDGE,       // Trigger on Rising Edge
    };
    gpio_config(&io_conf);
}
