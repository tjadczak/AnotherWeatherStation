#pragma once
#include "driver/i2c.h"
#include "esp_log.h"

esp_err_t BMP280_init(void);
uint8_t BMP280_check_ID(void);
esp_err_t BMP280_set_ctrl_meas(void);
esp_err_t BMP280_set_config(void);
esp_err_t BMP280_deinit(void);
//void BMP280_read_temp_and_press(*float temp, *uint8_t press);


void BMP280_register_read(uint8_t reg_addr, uint8_t *data, size_t len);
esp_err_t BMP280_register_write_byte(uint8_t reg_addr, uint8_t data);


