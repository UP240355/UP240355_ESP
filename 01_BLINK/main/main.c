#include "driver/gpio.h" // Esta librería es necesaria para manejar los pines GPIO, en el CMake debemos agregar la dependencia "driver"
#include "freertos/FreeRTOS.h" // Esta librería es necesaria para usar FreeRTOS 
#include "freertos/task.h" // Esta librería es necesaria para usar las tareas de FreeRTOS


#define LED GPIO_NUM_2 // Definimos el pin donde está conectado el LED

void app_main(void) // Función principal del programa
{

    gpio_reset_pin(LED); // Reseteamos el pin del LED
    gpio_set_direction(LED, GPIO_MODE_OUTPUT); // Configuramos el pin del LED como salida

    while (1) // Bucle infinito
    {
        gpio_set_level(LED, 1); // Encendemos el LED
        vTaskDelay(pdMS_TO_TICKS(1000)); // Esperamos 1 segundo
        gpio_set_level(LED, 0); // Apagamos el LED
        vTaskDelay(pdMS_TO_TICKS(1000)); // Esperamos 1 segundo
    }
}