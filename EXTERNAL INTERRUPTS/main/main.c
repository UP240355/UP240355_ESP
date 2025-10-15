/*
 * Este programa para ESP32 utiliza una interrupción externa para contar el número de veces
 * que se presiona un botón conectado al pin GPIO5. Cuando se detecta un flanco de subida
 * en el pin, se incrementa el contador y se actualiza el estado del botón.
 *
 * Principales componentes:
 * - INT_PIN: Pin GPIO configurado para la interrupción externa.
 * - int_count: Variable que almacena el número de interrupciones detectadas.
 * - button_state: Bandera que indica si se ha detectado una interrupción.
 *
 * Funciones:
 * - gpio_isr_handler: Manejador de la interrupción, incrementa el contador y actualiza el estado.
 *   // Esta función es llamada automáticamente cuando ocurre una interrupción en el pin INT_PIN.
 *   // Incrementa el contador de interrupciones y actualiza la bandera de estado del botón.
 *
 * - app_main: Configura el pin, la interrupción y muestra el contador por consola cada vez que se detecta una pulsación.
 *   // Función principal del programa. Inicializa el GPIO, configura la interrupción externa,
 *   // y en un bucle principal muestra el número de veces que se ha presionado el botón.
 *
 * Notas:
 * - El manejador de interrupción vuelve a registrar la ISR y habilita la interrupción, lo cual no es necesario y puede causar problemas.
 * - Se utiliza FreeRTOS para la gestión de tareas y retardos.
 */
#include <stdio.h> // Incluye funciones de entrada/salida estándar
#include "freertos/FreeRTOS.h" // Incluye FreeRTOS para gestión de tareas
#include "driver/gpio.h" // Incluye funciones para manejar GPIO
#include "inttypes.h" // Incluye definiciones de tipos enteros

#define INT_PIN GPIO_NUM_5 // Define el pin GPIO5 como pin de interrupción
uint16_t int_count = 0; // Variable para contar interrupciones
bool button_state = false; // Bandera para indicar si el botón fue presionado

// Manejador de la interrupción externa
static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    int_count ++; // Incrementa el contador de interrupciones
    button_state = true; // Indica que el botón fue presionado
}

// Función principal del programa
void app_main(void)
{
    gpio_reset_pin(INT_PIN); // Resetea la configuración previa del pin
    gpio_set_direction(INT_PIN, GPIO_MODE_INPUT); // Configura el pin como entrada

    gpio_set_pull_mode(INT_PIN, GPIO_PULLUP_ONLY); // Activa resistencia pull-up interna
    gpio_set_intr_type(INT_PIN, GPIO_INTR_POSEDGE); // Configura interrupción por flanco de subida

    gpio_install_isr_service(0); // Instala el servicio de ISR para GPIO
    gpio_isr_handler_add(INT_PIN, gpio_isr_handler, NULL); // Registra el manejador de la interrupción

    gpio_intr_enable(INT_PIN); // Habilita la interrupción en el pin

    while(1) // Bucle principal
    {
        if(button_state == true) // Si se detectó una pulsación
        {
            printf("%d\n", int_count); // Muestra el contador por consola
            button_state = false; // Reinicia la bandera
        }
        vTaskDelay(pdMS_TO_TICKS(100)); // Espera 100 ms antes de repetir
    }
}