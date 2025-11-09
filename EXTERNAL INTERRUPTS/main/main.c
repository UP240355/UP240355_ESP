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
#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED    GPIO_NUM_23
#define BUTTON GPIO_NUM_22

// Variables compartidas entre ISR y tarea principal
volatile int int_count = 0;
volatile bool button_state = false;

// ISR con debounce (usa ticks del RTOS)
static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    TickType_t now = xTaskGetTickCountFromISR();
    static TickType_t last_isr_tick = 0;
    const TickType_t debounce_ticks = pdMS_TO_TICKS(200); 

    if ((now - last_isr_tick) > debounce_ticks) {
        int_count++;
        button_state = true;
        last_isr_tick = now;
    }
}

// Función para generar un punto (•) en código Morse
void punto(void) {
    gpio_set_level(LED, 1);
    vTaskDelay(pdMS_TO_TICKS(200));
    gpio_set_level(LED, 0);
    vTaskDelay(pdMS_TO_TICKS(200));
}

// Función para generar una raya (—) en código Morse
void raya(void) {
    gpio_set_level(LED, 1);
    vTaskDelay(pdMS_TO_TICKS(500));
    gpio_set_level(LED, 0);
    vTaskDelay(pdMS_TO_TICKS(500));
}

// Función para enviar la señal SOS en Morse: ••• --- •••
void SOS(void) {
    for (int i = 0; i < 3; i++) punto();
    for (int i = 0; i < 3; i++) raya();
    for (int i = 0; i < 3; i++) punto();
}

void app_main(void)
{
    // Reset y configura LED
    gpio_reset_pin(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    gpio_set_level(LED, 0); // LED apagado inicialmente

    // Reset y configura BUTTON
    gpio_reset_pin(BUTTON);
    gpio_set_direction(BUTTON, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON, GPIO_PULLUP_ONLY); // asumo boton a GND
    gpio_set_intr_type(BUTTON, GPIO_INTR_NEGEDGE); // flanco de bajada (presión a GND)

    // Instala servicio ISR y agrega el handler
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON, gpio_isr_handler, NULL);

    // Bucle principal
    while (1)
    {
        // Si detectamos 3 pulsos, enviar SOS
        printf("%d\n", int_count);
        if (int_count >= 3)
        {
           int_count = 0;
           button_state = false;
           SOS();
        }

        
        vTaskDelay(pdMS_TO_TICKS(100));
}
}