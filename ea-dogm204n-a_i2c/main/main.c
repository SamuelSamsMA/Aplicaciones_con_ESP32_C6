#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "ea-dogm204n-a_i2c.h"


/* Definición de pines del controlador */
#define SCL_PIN         GPIO_NUM_7
#define SDA_PIN         GPIO_NUM_6
#define RST_PIN         GPIO_NUM_9      /* Pin de reset para los dispositivos conectados al controlador. */


i2c_master_bus_handle_t bus_handle;



void app_main(void)
{
    gpio_set_direction(RST_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(RST_PIN, 0);
    vTaskDelay(50 / portTICK_PERIOD_MS);

    /* Registra el bus maestro */
    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = -1,
        .scl_io_num = SCL_PIN,
        .sda_io_num = SDA_PIN,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = false
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));
    
    // Habilita los dispositivos externos
    gpio_set_level(RST_PIN, 1);

    ea_dogm204_i2c_config lcd_config = {
        .bus_i2c = &bus_handle,
        .direccion_i2c = 0x007A>>1,
        .frecuencia_reloj_i2c = 100000
    };

    ESP_ERROR_CHECK(LCD_iniciar(&lcd_config));
    LCD_cadenaTexto("Tarjeta ESP32 C6");

    int contador = 0;
    while (1)
    {
        char bufer[LCD_COLUMNAS];
        sprintf(bufer, "i = %04d", contador++);
        LCD_moverCursor(0, L2);
        LCD_cadenaTexto(bufer);

        if (contador == 1000)
            contador = 0;
        
        vTaskDelay(250 / portTICK_PERIOD_MS);
    }
}




