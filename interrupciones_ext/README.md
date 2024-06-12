# Interrupciones externas

En este ejemplo configuramos los pines 2, 3, 10 y 11 como pines de entrada con la interrupción del flanco de subida para detectar los eventos de cuatro pulsadores, cada uno activa al LED RGB integrado en un color distinto.

Debido a que las funciones de la librería led_strip no se deben llamar desde una función de interrupción, se implementó una estructura de cola para enviar datos de forma segura entre el ámbito de la interrupción y el ámbito de la tarea principal.

## Hardware utilizado

* [ESP32-C6-DevKitC-1](https://docs.espressif.com/projects/espressif-esp-dev-kits/en/latest/esp32c6/esp32-c6-devkitc-1/user_guide_v1.1.html)

* [2x2 Key click](https://www.mikroe.com/2x2-key-click)

