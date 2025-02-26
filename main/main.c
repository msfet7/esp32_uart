#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define RED 14
#define YELLOW 26   
#define UART_PORT UART_NUM_0
#define DELAY 100 / portTICK_PERIOD_MS

// Queue handlers
static QueueHandle_t commandQueue = NULL;

// Tasks

// receive and transmits data
void uartControl(void *arg){
    char buffer[64] = {0};                                                      // buffer for messages
    uint8_t toSend = 0;                                                         // id to send
    size_t len = 0;
    while(1){
        ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_PORT, &len));
        if(len != 0){
            len = uart_read_bytes(UART_PORT, buffer, len, 100);
            if(strcmp(buffer, "Hello") == 0){
                uart_write_bytes(UART_PORT, "World!\n", strlen("World!\n"));
                toSend = 1;
            }else toSend = 2;
            xQueueSend(commandQueue, &toSend , 0);                              // send id
            memset(buffer, 0, 64);                                              // clears the buffer (our string)
            len = 0;   
        }else vTaskDelay(DELAY);
    
    }
}

// controls led gpio
void ledControl(void *arg){
    uint8_t id = 0;
    while(1){
        if(xQueueReceive(commandQueue, &id, 0)){
            if(id == 1){                            // lights up yellow LED
                gpio_set_level(YELLOW, 1);
                gpio_set_level(RED, 0);
            }else if(id == 2){                      // lights up red LED
                gpio_set_level(YELLOW, 0);
                gpio_set_level(RED,1);
            }
        }else vTaskDelay(DELAY);
    }
    
}

void app_main(void){
    gpio_config_t pinConfig;

    // output pin configuration (14 and 26)
    pinConfig.pin_bit_mask = ((1ULL << RED) | (1ULL << YELLOW));
    pinConfig.mode = GPIO_MODE_OUTPUT;
    pinConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
    pinConfig.pull_up_en = GPIO_PULLUP_DISABLE;
    pinConfig.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&pinConfig);

    // uart configuration
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, 2048, 2048, 0, NULL, 0));

    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uart_config));

    ESP_ERROR_CHECK(uart_set_pin(UART_PORT, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    // queue config
    commandQueue = xQueueCreate(5, sizeof(uint8_t));

    // task config
    xTaskCreate(uartControl, "UART", 2048, NULL, 3, NULL);
    xTaskCreate(ledControl, "LED", 2048, NULL, 2, NULL);

}