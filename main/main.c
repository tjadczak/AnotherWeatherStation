#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "I2C.h"
#include "Interrupts.h"
#include "LcdDisplay.h"


void app_main(void){
    // Initialize variables
    // Initialize input and output ports
    // Initialize I2C bus
    // Initialize BMP280 sensor
    // Initialize LCD display
    // Initialize interrupts to read and display measurements on demand (button)
    while (1) {
        printf("Hello world!\n");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
	//read values from BMP280
	//display measurements on LCD display
	//go to deep sleep for X seconds
    }
}
