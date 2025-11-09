#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"

// Definición de los pines para el LED y el botón
#define LED    GPIO_NUM_23
#define BUTTON GPIO_NUM_22


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

void app_main(void)
{
    // Reinicia la configuración de los pines LED y botón
    gpio_reset_pin(LED);
    gpio_reset_pin(BUTTON);

    // Configura el pin del LED como salida
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);

    // Configura el pin del botón con resistencia pull-up
    gpio_set_pull_mode(BUTTON, GPIO_PULLUP_ONLY);
    // Configura el pin del botón como entrada
    gpio_set_direction(BUTTON, GPIO_MODE_INPUT);

    int buttonState = 1;
    int lastButtonState = 1;
    int pressCount = 0;  // Contador de presiones
    TickType_t lastPressTime = 0;

    while (true) {
        buttonState = gpio_get_level(BUTTON);

        // Detectar flanco de bajada (botón presionado)
        if (buttonState == 0 && lastButtonState == 1) {
            // Anti-rebote
            vTaskDelay(pdMS_TO_TICKS(50));
            if (gpio_get_level(BUTTON) == 0) {
                TickType_t now = xTaskGetTickCount();

                // Si la segunda presión ocurre en menos de 1.5 segundos, cuenta como doble clic
                if ((now - lastPressTime) < pdMS_TO_TICKS(1500)) {
                    pressCount++;
                } else {
                    pressCount = 1;
                }

                lastPressTime = now;

                // Si ya hubo dos presiones, enviar SOS
                if (pressCount == 2) {
                    SOS();
                    pressCount = 0; // Reiniciar contador
                }

                // Esperar a que se suelte el botón
                while (gpio_get_level(BUTTON) == 0) {
                    vTaskDelay(pdMS_TO_TICKS(10));
                }
            }
        }

        lastButtonState = buttonState;
        vTaskDelay(pdMS_TO_TICKS(20));
}
}