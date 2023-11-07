#pragma once
#include "driver/i2c.h"
#include "esp_log.h"

typedef int32_t BMP280_S32_t;
typedef uint32_t BMP280_U32_t;
typedef int64_t BMP280_S64_t;

BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t adc_T);
BMP280_U32_t bmp280_compensate_P_int64(BMP280_S32_t adc_P);

typedef struct BMP280_compensation {
    uint16_t dig_T1;
    int16_t dig_T2;
    int16_t dig_T3;
    uint16_t dig_P1;
    int16_t dig_P2;
    int16_t dig_P3;
    int16_t dig_P4;
    int16_t dig_P5;
    int16_t dig_P6;
    int16_t dig_P7;
    int16_t dig_P8;
    int16_t dig_P9;
} BMP280_compensation;

esp_err_t BMP280_init(void);
uint8_t BMP280_check_ID(void);
esp_err_t BMP280_set_ctrl_meas(void);
esp_err_t BMP280_set_config(void);
esp_err_t BMP280_deinit(void);
void BMP280_read_compensation_data(void);
//void BMP280_read_temp_and_press(*float temp, *uint8_t press);


void BMP280_register_read(uint8_t reg_addr, uint8_t *data, size_t len);
esp_err_t BMP280_register_write_byte(uint8_t reg_addr, uint8_t data);


