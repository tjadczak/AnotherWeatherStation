#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "BMP280_I2C.h"
#include "Interrupts.h"
#include "LcdDisplay.h"

#define PROJECT_NAME "AnotherWeatherStation"

void app_main(void){
    // Initialize variables
    //float temp; 
    bool measuring_status;
    //uint8_t press;
    // Initialize input and output ports
    // Initialize I2C bus
    // Initialize BMP280 sensor 
    ESP_ERROR_CHECK(BMP280_init());
    ESP_LOGI(PROJECT_NAME, "I2C initialized successfully");
    ESP_LOGI(PROJECT_NAME, "BMP280 ID == %#02x", BMP280_check_ID());
    BMP280_read_compensation_data();
    //ESP_ERROR_CHECK(BMP280_set_config());
    //ESP_LOGI(PROJECT_NAME, "BMP280 config set");

    // Initialize LCD display
    // Initialize interrupts to read and display measurements on demand (button)
    while (1) {
        //set mode to forced
        ESP_ERROR_CHECK(BMP280_set_ctrl_meas());
        ESP_LOGI(PROJECT_NAME, "BMP280 ctrl meas set");
        //check measuring status
        do {
            vTaskDelay(10 / portTICK_PERIOD_MS);
            printf("Checking measuring status\n");
            measuring_status = BMP280_check_measuring_status();
            printf("Mmeasuring status == %b\n", measuring_status);
        } while measuring_status != false;
	    //read values from BMP280
        BMP280_read_temp_and_press (&temp, &press);
	    //display measurements on LCD display
	    //go to deep sleep for 5 seconds
        printf("temp = %f *C, press = %f hPa\n", temp, press);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
