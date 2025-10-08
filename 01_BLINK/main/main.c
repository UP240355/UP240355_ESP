#include "driver/gpio.h" // Esta librería es necesaria para manejar los pines GPIO, en el CMake debemos agregar la dependencia "driver"
#include "freertos/FreeRTOS.h" // Esta librería es necesaria para usar FreeRTOS 
#include "freertos/task.h" // Esta librería es necesaria para usar las tareas de FreeRTOS


#define LED GPIO_NUM_2 // Definimos el pin donde está conectado el LED


void punto(void)
{
gpio_set_level(LED, 1); // Encendemos el LED
        vTaskDelay(pdMS_TO_TICKS(200)); // Esperamos 1 segundo
        gpio_set_level(LED, 0); // Apagamos el LED
        vTaskDelay(pdMS_TO_TICKS(200)); // Esperamos 1 segundo
}

void raya(void)
{
    gpio_set_level(LED, 1); // Encendemos el LED
        vTaskDelay(pdMS_TO_TICKS(500)); // Esperamos 1 segundo
        gpio_set_level(LED, 0); // Apagamos el LED
        vTaskDelay(pdMS_TO_TICKS(500)); // Esperamos 1 segundo
}

void SOS()
{
    for (int i = 0; 1 < 3; i++)
    {
    punto();
    }
    for (int i = 0; 1 < 3; i++)
    {
    raya();
    }
    for (int i = 0; 1 < 3; i++)
    {
    punto();
    }
}
void app_main(void) // Función principal del programa
{

    gpio_reset_pin(LED); // Reseteamos el pin del LED
    gpio_set_direction(LED, GPIO_MODE_OUTPUT); // Configuramos el pin del LED como salida
while (1) 
{
    SOS();
            vTaskDelay(pdMS_TO_TICKS(1000)); // Esperamos 2 segundo

}
    while (1) // Bucle infinito
    {
        gpio_set_level(LED, 1); // Encendemos el LED
        vTaskDelay(pdMS_TO_TICKS(1000)); // Esperamos 1 segundo
        gpio_set_level(LED, 0); // Apagamos el LED
        vTaskDelay(pdMS_TO_TICKS(1000)); // Esperamos 1 segundo
    }
}