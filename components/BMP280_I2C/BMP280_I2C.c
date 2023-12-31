#include "BMP280_I2C.h"

#define I2C_MASTER_SCL_IO           22  		        /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           21      		        /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0                           /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          400000                      /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                           /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                           /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000
#define I2C_MASTER_PORT             0

#define BMP280_SENSOR_ADDR          0x76	                /*!< Slave address of the BMP280 sensor */
#define BMP280_WHO_AM_I_REG_ADDR    0xD0        	        /*!< Register addresses of the "who am I" register, should return 0x58 */
#define BMP280_CTRL_MEAS_REG_ADDR   0xF4        	        
#define BMP280_CONFIG_REG_ADDR      0xF5        	        

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// t_fine carries fine temperature as global value
BMP280_compensation comp;
BMP280_S32_t t_fine;

BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t adc_T){
    BMP280_S32_t var1, var2, T;
    var1 = ((((adc_T>>3) - ((BMP280_S32_t)comp.dig_T1<<1))) * ((BMP280_S32_t)comp.dig_T2)) >> 11;
    var2 = (((((adc_T>>4) - ((BMP280_S32_t)comp.dig_T1)) * ((adc_T>>4) - ((BMP280_S32_t)comp.dig_T1))) >> 12) * ((BMP280_S32_t)comp.dig_T3)) >> 14;
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T;
}

// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
BMP280_U32_t bmp280_compensate_P_int64(BMP280_S32_t adc_P){
    BMP280_S64_t var1, var2, p;
    var1 = ((BMP280_S64_t)t_fine) - 128000;
    var2 = var1 * var1 * (BMP280_S64_t)comp.dig_P6;
    var2 = var2 + ((var1*(BMP280_S64_t)comp.dig_P5)<<17);
    var2 = var2 + (((BMP280_S64_t)comp.dig_P4)<<35);
    var1 = ((var1 * var1 * (BMP280_S64_t)comp.dig_P3)>>8) + ((var1 * (BMP280_S64_t)comp.dig_P2)<<12);
    var1 = (((((BMP280_S64_t)1)<<47)+var1))*((BMP280_S64_t)comp.dig_P1)>>33;
    if (var1 == 0){
        return 0; // avoid exception caused by division by zero
    }
    p = 1048576-adc_P;
    p = (((p<<31)-var2)*3125)/var1;
    var1 = (((BMP280_S64_t)comp.dig_P9) * (p>>13) * (p>>13)) >> 25;
    var2 = (((BMP280_S64_t)comp.dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((BMP280_S64_t)comp.dig_P7)<<4);
    return (BMP280_U32_t)p;
}

esp_err_t BMP280_init(void){
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,        
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL_IO,         
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
        .clk_flags = 0,                        
    };

    i2c_param_config(I2C_MASTER_PORT, &conf);

    return i2c_driver_install(I2C_MASTER_PORT, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);    
}

uint8_t BMP280_check_ID(void){
    uint8_t ID;
    BMP280_register_read(BMP280_WHO_AM_I_REG_ADDR, &ID, 1);
    return ID;
}

void BMP280_read_compensation_data(void){
    uint8_t data[24];
    BMP280_register_read(0x88, &data[0], 24);
    comp.dig_T1 = data[1]<<8 | data[0];
    comp.dig_T2 = data[3]<<8 | data[2];
    comp.dig_T3 = data[5]<<8 | data[4];
    comp.dig_P1 = data[7]<<8 | data[6];
    comp.dig_P2 = data[9]<<8 | data[8];
    comp.dig_P3 = data[11]<<8 | data[10];
    comp.dig_P4 = data[13]<<8 | data[12];
    comp.dig_P5 = data[15]<<8 | data[14];
    comp.dig_P6 = data[17]<<8 | data[16];
    comp.dig_P7 = data[19]<<8 | data[18];
    comp.dig_P8 = data[21]<<8 | data[20];
    comp.dig_P9 = data[23]<<8 | data[22];
}

esp_err_t BMP280_set_ctrl_meas(void){
    uint8_t ctrl_meas=0b00000000;
    return(BMP280_register_write_byte(BMP280_CTRL_MEAS_REG_ADDR, ctrl_meas));
}

esp_err_t BMP280_set_config(void){
    uint8_t config=0b00100101;
    return(BMP280_register_write_byte(BMP280_CONFIG_REG_ADDR, config));
}

bool BMP280_check_measuring_status(void){
    uint8_t data;
    BMP280_register_read(0xF3, &data, 1);
    return data & 0b00001000;
}

void BMP280_read_temp_and_press(*float temp, *float press){
    uint8_t data[6];
    BMP280_S32_t temp_, press_;
    BMP280_register_read(0xF3, &data, 6);
    press_ = data[0] << 12 | data[1] << 4 | data[2] >> 4;
    temp_ = data[3] << 12 | data[4] << 4 | data[5] >> 4;
    temp = bmp280_compensate_T_int32(temp_)/100.0;
    press = bmp280_compensate_P_int64(press_)/25600.0;
}

void BMP280_register_read(uint8_t reg_addr, uint8_t *data, size_t len)
{
    ESP_ERROR_CHECK(i2c_master_write_read_device(I2C_MASTER_NUM, BMP280_SENSOR_ADDR, &reg_addr, 1, data, len, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS));
}

esp_err_t BMP280_register_write_byte(uint8_t reg_addr, uint8_t data)
{
    uint8_t write_buf[2] = {reg_addr, data};
    return(i2c_master_write_to_device(I2C_MASTER_NUM, BMP280_SENSOR_ADDR, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS));
}

esp_err_t BMP280_deinit(void){
    return i2c_driver_delete(I2C_MASTER_NUM);
}


