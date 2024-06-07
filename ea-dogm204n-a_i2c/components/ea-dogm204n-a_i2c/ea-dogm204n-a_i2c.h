#pragma once

#include "driver/gpio.h"
#include "driver/i2c_master.h"


//
// Comandos predefinidos para la pantalla
//
#define LCD_cursorEncParpEnc	0x0F    /* Activa el cursor y el parpadeo. */
#define LCD_cursorEncParpApa	0x0E    /* Activa el cursor y apaga el parpadeo. */
#define LCD_cursorApaParpEnc	0x0D    /* Apaga el cursor y activa el parpadeo. */
#define LCD_cursorApaParpApa	0x0C    /* Apaga el cursor y el parpadeo. */
#define LCD_limpiarDisplay		0x01    /* Usar el macro limpiarPantalla(). */

//
// Datos de la pantalla
//
#define LCD_FILAS       4
#define LCD_COLUMNAS    20


/**
 * @brief Configuración de la pantalla EA-DOGM204 con el bus I2C.
 */
typedef struct {
    i2c_master_bus_handle_t* bus_i2c;       /*!< Puntero al bus I2C del controlador. */
    uint16_t direccion_i2c;                 /*!< Dirección física del dispositivo i2c. */
    uint32_t frecuencia_reloj_i2c;          /*!< Frecuencia de comunicación del protocolo. */
} ea_dogm204_i2c_config;

/**
 * @brief Direcciones de memoria para el inicio de cada fila.
 */
typedef enum {L1 = 0x00, L2 = 0x20, L3 = 0x40, L4 = 0x60} num_fila;

/**
 * @brief Set de caracteres disponibles.
 */
typedef enum {ROM_A = 0x0, ROM_B = 0x04, ROM_C = 0x08} lcd_charSet;

/**
 * @brief Indica el tipo de dato que se está enviando a la pantalla.
 */
typedef enum {COMANDO = 0, CARACTER = 0x40} lcd_tipoDato;


//
// Macros y funciones de interfaz.
//

/**
 * @brief Envía el byte "x" como un carácter imprimible en pantalla.
 */
#define LCD_caracter(x)         LCD_enviarByte(x,CARACTER)


/**
 * @brief Envía el byte "x" como un comando para la pantalla.
 */
#define LCD_comando(x)          LCD_enviarByte(x,COMANDO)


/**
 * Calcula el valor de la DDRAM de acuerdo a los valores "x,y"
 * que indican la columna "x" y la fila "y" de la pantalla.
 */
#define LCD_moverCursor(x,y)    LCD_comando(0x80 | (y + x))


/**
 * Limpia toda la pantalla escribiendo 0x20 (espacio) en todas las localidades
 * de la DDRAM y escribe 0x00 en la dirección de la DDRAM para retornar a HOME.
 */
#define LCD_limpiarPantalla()   LCD_comando(LCD_limpiarDisplay)


//
// Declaración de funciones
//

/**
 * @brief Inicia la configuración de la pantalla LCD.
 * 
 * @note Se debe llamar esta función antes que a cualquier otra de la librería.
 * 
 * @param config parámetros para la configuración del dispositivo en el bus I2C.
 * 
 * @return
 *      - ESP_OK: La pantalla se configuró correctamente.
 *      - ESP_ERR_XXX: Hubo algún problema con la comunicación.
 */
esp_err_t LCD_iniciar(ea_dogm204_i2c_config* config);


/**
 * @brief Envía un byte a la pantalla LCD con la comunicación I2C.
 * 
 * @note
 * Se recomienda usar los macros "LCD_caracter(x)" o "LCD_comando(x)"
 * para una mayor claridad en el código.
 * 
 * @param dato byte a enviar.
 * @param modo indica si el byte se envía como caracter o comando.
 * 
 * @return
 *      - ESP_OK: La información se envió correctamente.
 *      - ESP_ERR_XXX: Hubo algún problema con la comunicación.
 */
esp_err_t LCD_enviarByte(uint8_t dato, lcd_tipoDato modo);


/**
 * @brief Envía una cadena de caracteres imprimibles.
 * 
 * @param cadena puntero al arreglo de caracteres.
 * 
 * @return Número de caracteres que se enviaron.
 */
size_t LCD_cadenaTexto(char* cadena);


/**
 * @brief Permite cambiar el contraste de la pantalla.
 * 
 * @param nuevoC valor de contraste en porcentaje [0, 100].
 * 
 * @return
 *      - ESP_OK: El contraste cambió correctamente.
 *      - ESP_ERR_XXX: Hubo algún problema con la comunicación.
 */
esp_err_t LCD_cambiarContraste(unsigned nuevoC);


/**
 * @brief Cambia el set de caracteres de la pantalla.
 *
 * @param n nuevo set a utilizar.
 * 
 * @return
 *      - ESP_OK: El set se configuró correctamente.
 *      - ESP_ERR_XXX: Hubo algún problema con la comunicación.
 */
esp_err_t LCD_seleccionarROM(lcd_charSet n);
