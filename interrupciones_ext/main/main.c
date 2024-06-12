#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "led_strip.h"


#define PULSADOR_1  GPIO_NUM_3
#define PULSADOR_2  GPIO_NUM_2
#define PULSADOR_3  GPIO_NUM_11
#define PULSADOR_4  GPIO_NUM_10
#define LED_RGB     GPIO_NUM_8


led_strip_handle_t led_rgb;
QueueHandle_t cola;
const char app[] = "aplicacion";


void isr_pulsadores(void *arg)
{
    int pulsador = (int)arg;
    xQueueSendFromISR(cola, &pulsador, NULL);
}


led_strip_handle_t configurar_led_rgb(void)
{
    led_strip_config_t led_config = {
        .strip_gpio_num = LED_RGB,
        .max_leds = 1,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB,
        .led_model = LED_MODEL_WS2812
    };

    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10 * 1000 * 1000,      // 10 MHz
        .flags.with_dma = false
    };

    led_strip_handle_t led_handle;
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&led_config, &rmt_config, &led_handle));
    return led_handle;
}


void app_main(void)
{
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = ((1ULL << PULSADOR_1) | (1ULL << PULSADOR_2) |
                            (1ULL << PULSADOR_3) | (1ULL << PULSADOR_4));
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    gpio_install_isr_service(0);
    gpio_isr_handler_add(PULSADOR_1, isr_pulsadores, (void*) PULSADOR_1);
    gpio_isr_handler_add(PULSADOR_2, isr_pulsadores, (void*) PULSADOR_2);
    gpio_isr_handler_add(PULSADOR_3, isr_pulsadores, (void*) PULSADOR_3);
    gpio_isr_handler_add(PULSADOR_4, isr_pulsadores, (void*) PULSADOR_4);

    led_rgb = configurar_led_rgb();

    cola = xQueueCreate(10, sizeof(int));
    if (cola == NULL)
        ESP_LOGE(app, "Error al crear la cola de mensajes.");

    int pulsador;
    while (1)
    {
        if (xQueueReceive(cola, &pulsador, portMAX_DELAY))
        {
            //ESP_LOGI(app, "Pulsador %d detectado.", pulsador);

            switch (pulsador)
            {
            case PULSADOR_1:
                led_strip_set_pixel(led_rgb, 0, 50, 0, 0);
                break;
            case PULSADOR_2:
                led_strip_set_pixel(led_rgb, 0, 0, 50, 0);
                break;
            case PULSADOR_3:
                led_strip_set_pixel(led_rgb, 0, 0, 0, 50);
                break;
            case PULSADOR_4:
                led_strip_set_pixel(led_rgb, 0, 50, 50, 50);
                break;
            default:
                break;
            }
            led_strip_refresh(led_rgb);
        }
    }
    
}
