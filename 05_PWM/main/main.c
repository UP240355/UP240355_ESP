#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_adc/adc_oneshot.h"

// Definición de pines del motor
#define AIN1 GPIO_NUM_5
#define AIN2 GPIO_NUM_17
#define PWMA GPIO_NUM_16

// Configuración del ADC
#define ADC_CHANNEL ADC_CHANNEL_0  // GPIO36
adc_oneshot_unit_handle_t adc_handle;

// Configuración de GPIO
esp_err_t configureGpio(void)
{
    gpio_reset_pin(AIN1);
    gpio_reset_pin(AIN2);
    gpio_reset_pin(PWMA);
    gpio_set_direction(AIN1, GPIO_MODE_OUTPUT);
    gpio_set_direction(AIN2, GPIO_MODE_OUTPUT);
    gpio_set_direction(PWMA, GPIO_MODE_OUTPUT);
    return ESP_OK;
}

// Configuración del ADC
void configureADC(void)
{
    adc_oneshot_unit_init_cfg_t adc_config = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    adc_oneshot_new_unit(&adc_config, &adc_handle);

    adc_oneshot_chan_cfg_t channel_config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12,
    };
    adc_oneshot_config_channel(adc_handle, ADC_CHANNEL, &channel_config);
}

// Configuración del PWM
void setupPWM(void)
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK,
        .deconfigure = false
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel_A = {
        .gpio_num = PWMA,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0,
        .sleep_mode = LEDC_SLEEP_MODE_NO_ALIVE_NO_PD,
        .flags = {
            .output_invert = 0
        }
    };
    ledc_channel_config(&ledc_channel_A);
    ledc_fade_func_install(0);
}

// Función para controlar el motor basado en el valor del ADC
void controlMotor(int adc_value)
{
    int duty_cycle;
    float adc_percentage = (float)adc_value / 4095.0f * 100.0f;
    
    if (adc_percentage <= 50.0f) {
        // Sentido horario (0% - 50% del ADC)
        gpio_set_level(AIN1, 1);
        gpio_set_level(AIN2, 0);
        
        // Mapear 0-50% del ADC a 0-100% de velocidad
        float speed_percentage = (adc_percentage / 50.0f) * 100.0f;
        duty_cycle = (int)((speed_percentage / 100.0f) * 255.0f);
        
        printf("ADC: %d (%.1f%%) - Sentido: HORARIO - Velocidad: %.1f%% - Duty: %d\n", 
               adc_value, adc_percentage, speed_percentage, duty_cycle);
    } else {
        // Sentido antihorario (50% - 100% del ADC)
        gpio_set_level(AIN1, 0);
        gpio_set_level(AIN2, 1);
        
        // Mapear 50-100% del ADC a 0-100% de velocidad
        float speed_percentage = ((adc_percentage - 50.0f) / 50.0f) * 100.0f;
        duty_cycle = (int)((speed_percentage / 100.0f) * 255.0f);
        
        printf("ADC: %d (%.1f%%) - Sentido: ANTIHORARIO - Velocidad: %.1f%% - Duty: %d\n", 
               adc_value, adc_percentage, speed_percentage, duty_cycle);
    }
    
    // Asegurar que el duty cycle esté en el rango válido
    if (duty_cycle < 0) duty_cycle = 0;
    if (duty_cycle > 255) duty_cycle = 255;
    
    ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty_cycle, 0);
}

void app_main(void)
{
    // Inicializar todos los componentes
    configureGpio();
    configureADC();
    setupPWM();
    
    printf("Sistema de control de motor con potenciómetro iniciado\n");
    printf("ADC Range: 0-4095\n");
    printf("0-50%% ADC: Sentido horario, velocidad 0-100%%\n");
    printf("50-100%% ADC: Sentido antihorario, velocidad 0-100%%\n");
    printf("==================================================\n");

    int adc_value;
    
    while(1)
    {
        // Leer valor del ADC
        adc_oneshot_read(adc_handle, ADC_CHANNEL, &adc_value);
        
        // Controlar el motor basado en el valor del ADC
        controlMotor(adc_value);
        
        // Pequeño delay para estabilidad
        vTaskDelay(pdMS_TO_TICKS(50));
}
}