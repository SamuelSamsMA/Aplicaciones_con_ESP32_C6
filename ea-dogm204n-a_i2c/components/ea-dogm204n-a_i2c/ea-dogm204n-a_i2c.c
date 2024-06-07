#include "ea-dogm204n-a_i2c.h"
#include "esp_err.h"
#include <string.h>


i2c_master_dev_handle_t lcd_handle;


esp_err_t LCD_iniciar(ea_dogm204_i2c_config* config)
{
    /* Registra la pantalla LCD como nuevo dispositivo */
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = config->direccion_i2c,
        .scl_speed_hz = config->frecuencia_reloj_i2c
    };

    esp_err_t err = i2c_master_bus_add_device(*(config->bus_i2c), &dev_cfg, &lcd_handle);
    if (err != ESP_OK)
        return err;

    uint8_t bufer[] = {
	    0x00,	// Control byte
	    0x3A,	// 8 bit data length extension Bit RE=1; REV=0
	    0x09,	// 4 line display
	    0x06,	// bottom view
	    0x1E,	// BS1=1
	    0x39,	// 8 bit data length extension Bit RE=0; IS=1
	    0x1B,	// BS0=1 -> Bias=1/6
	    0x6E,	// Devider on and set value
	    0x57,	// Booster on and set contrast (DB1=C5, DB0=C4)
	    0x72,	// Set contrast (DB3-DB0=C3-C0)
	    0x38,	// 8 bit data length extension Bit RE=0; IS=0
	    0x0C,	// Display on, cursor off, blink off
	    0x01	// Limpiar pantalla
    };
    
    return i2c_master_transmit(lcd_handle, bufer, sizeof(bufer), 100);
}


esp_err_t LCD_enviarByte(uint8_t dato, lcd_tipoDato modo)
{
	uint8_t bufer[] = { (uint8_t)modo, dato };
	
    return i2c_master_transmit(lcd_handle, bufer, sizeof(bufer), -1);
}


size_t LCD_cadenaTexto(char* cadena)
{
    uint8_t bufer[LCD_COLUMNAS + 1];
	size_t longCadena = strlen(cadena);
	
	if (longCadena > LCD_COLUMNAS)
        longCadena = LCD_COLUMNAS;

	bufer[0] = (uint8_t)CARACTER;	// Byte de control
	memcpy(bufer + 1, cadena, longCadena);

    ESP_ERROR_CHECK(i2c_master_transmit(lcd_handle, bufer, longCadena + 1, -1));
    return longCadena;
}


esp_err_t LCD_cambiarContraste(unsigned nuevoC)
{
	// Convierte la escala de [0, 100] a [25, 63] que son los valores
	// aceptados por la pantalla como mínimo y máximo útil.
	unsigned c = (0.38 * nuevoC) + 25;
	
	// Ver la hoja de datos de la pantalla EADOGM204N para comprender estos
	// bytes de configuración.
    uint8_t bufer[] = {
        0x00,	                            // Byte de control
        0x39,
        (uint8_t)((c >> 4) | 0x0054),
        (uint8_t)((c & 0x000F) | 0x0070),
        0x38
    };

    return i2c_master_transmit(lcd_handle, bufer, sizeof(bufer), 100);
}


esp_err_t LCD_seleccionarROM(lcd_charSet n)
{
    unsigned char bufer[] = { 0x80, 0x3A, 0x80, 0x72, 0xC0, (uint8_t)n, 0x00, 0x38 };

    return i2c_master_transmit(lcd_handle, bufer, sizeof(bufer), 100);
}


