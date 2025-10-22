#include <stdio.h> // Incluye la biblioteca estándar de entrada/salida
#include "freertos/FreeRTOS.h" // Incluye la biblioteca de FreeRTOS
#include "esp_adc/adc_oneshot.h" // Incluye el controlador ADC en modo de una sola toma

int adc_value = 0; // Variable para almacenar el valor leído del ADC
adc_oneshot_unit_handle_t adc1_handle; // Manejador para la unidad ADC1
int adc_raw = 0; // Variable para almacenar el valor crudo del ADC

void configuracion(void){
    
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1, // Identificador de la unidad ADC1
        .ulp_mode = ADC_ULP_MODE_DISABLE, // Deshabilita el modo ULP
    };
    adc_oneshot_new_unit(&init_config, &adc1_handle); // Inicializa el ADC

    adc_oneshot_chan_cfg_t channel_config = {
        .atten = ADC_ATTEN_DB_11, // Atenuación de 11dB
        .bitwidth = ADC_BITWIDTH_12, // Ancho de datos de 12 bits
    };
    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_0, &channel_config); // Configura el canal 0 del ADC1
}

void app_main(void)
{

    configuracion(); // Llama a la función de configuración del ADC


    while (true) // Bucle infinito
    {
        // Lee el valor crudo del canal 0 del ADC1
        esp_err_t ret = adc_oneshot_read(adc1_handle, ADC_CHANNEL_0, &adc_raw);
        if (ret != ESP_OK) {
            printf("ADC read failed\n");
            continue; // Si la lectura falla, continúa con la siguiente iteración
        }
        float voltage = adc_raw * (3.3 / 4095); // Convierte el valor ADC a voltaje (asumiendo Vref = 3.3V y resolución de 12 bits)
        // Imprime el valor leído por el ADC
        printf("ADC Value is %d, Voltage is %.2fV\n", adc_raw, voltage);
        vTaskDelay(pdMS_TO_TICKS(100)); // Espera 100 ms antes de la siguiente lectura
    }
}