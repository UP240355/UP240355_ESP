/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>                      // Cabeceras estándar
#include "freertos/FreeRTOS.h"          // API de FreeRTOS
#include "freertos/task.h"              // Manejo de tareas (xTaskCreate, vTaskDelay, etc.)
#include "driver/gpio.h"                // Control de pines GPIO del ESP32
#include "driver/ledc.h"                // Incluye el controlador para LEDC (PWM)
#include "esp_adc/adc_oneshot.h"        // Incluye el controlador ADC en modo de una sola toma
#include "esp_log.h"                    // Logging (ESP_LOGI, ESP_LOGW, ESP_LOGE)

static const char *ESP = "Mi ESP";

#define AIN1 GPIO_NUM_5
#define AIN2 GPIO_NUM_17
#define PWMA GPIO_NUM_16

int adc_value = 0; // Variable para almacenar el valor leído del ADC
int adc_raw = 0; // Variable para almacenar el valor crudo del ADC
adc_oneshot_unit_handle_t adc1_handle; // Manejador para la unidad ADC1

esp_err_t configureGpio(void)
{
    // Configure GPIO pins for input and output modes
    gpio_reset_pin(AIN1);   // Reset AIN1 pin
    gpio_reset_pin(AIN2);   // Reset AIN2 pin
    gpio_reset_pin(PWMA);   // Reset PWMA pin
    gpio_set_direction(AIN1, GPIO_MODE_OUTPUT);
    gpio_set_direction(AIN2, GPIO_MODE_OUTPUT);
    gpio_set_direction(PWMA, GPIO_MODE_OUTPUT);
    return ESP_OK; // Return success
}

void setupPWM(void)
{
    // Configuración del canal PWM
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_8_BIT, // Resolución de 8 bits
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 5000, // Frecuencia de 5 kHz
        .clk_cfg = LEDC_AUTO_CLK,
        .deconfigure = false // No desconfigurar el temporizador
    };
    ledc_timer_config(&ledc_timer);

    // Configuración del canal A
    ledc_channel_config_t ledc_channel_A = {
        .gpio_num = PWMA, // Primero el GPIO
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0,
        .sleep_mode = LEDC_SLEEP_MODE_NO_ALIVE_NO_PD, // Deshabilitar el modo de sueño
        .flags = {
            .output_invert = 0 // No invertir la salida
        }};
    ledc_channel_config(&ledc_channel_A);
    ledc_fade_func_install(0); // Instala la función de desvanecimiento
}

void configuracion(void){
    
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_2, // Identificador de la unidad ADC1
        .ulp_mode = ADC_ULP_MODE_DISABLE, // Deshabilita el modo ULP
    };
    adc_oneshot_new_unit(&init_config, &adc1_handle); // Inicializa el ADC

    adc_oneshot_chan_cfg_t channel_config = {
        .atten = ADC_ATTEN_DB_12, // Atenuación de 11dB
        .bitwidth = ADC_BITWIDTH_12, // Ancho de datos de 12 bits
    };
    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_0, &channel_config); // Configura el canal 0 del ADC1
    ESP_LOGI(ESP, "Ya termine la configuración \n");
}

void Task(void *pvParameters)
{
    while (1)
    {
        // Lee el valor crudo del canal 0 del ADC1
        esp_err_t ret = adc_oneshot_read(adc1_handle, ADC_CHANNEL_0, &adc_raw);
        if (ret != ESP_OK) 
        {
            printf("ADC read failed\n");
            continue;                                           // Si la lectura falla, continúa con la siguiente iteración
        }
        float porcentaje = (adc_raw / 4095.0f) * 100.0f;        //Conversion de la lectura para el porcentaje
        int duty = 0;

        if (porcentaje < 50.0f)
        {
            gpio_set_level(AIN1,1);                             //Giro horario
            gpio_set_level(AIN2,0);
            duty = (int)((porcentaje / 50.0f) * 255);           //Escala 0-50%
        }
        else
        {
            gpio_set_level(AIN1,0);                             //Giro antihorario
            gpio_set_level(AIN2,1);
            duty = (int)((porcentaje - 50.0f) * 255);           //Escala 0-50% 
        }

        ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty, 0);                                                                                             //Actualizar PWM
        float voltage = adc_raw * (3.3 / 4095);                                                                                                                             //Muestra la lectura y accion actual
        printf("ADC=%d | %.2fV | %.1f%% | Duty=%d | Direccion: %s\n",adc_raw, voltage, porcentaje, duty, (porcentaje < 50.0f) ? "Horario" : "Antihorario");
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    
}

void app_main(void)
{
    configuracion(); // Llama a la función de configuración del ADC
    configureGpio();
    xTaskCreatePinnedToCore(Task, "Tarea", 2048, NULL,1, NULL, 0);
}