/*
   MPU6050.cpp : driver for Invensense MPU6050

   Adapted from https://github.com/multiwii/baseflight/blob/master/src/drv_mpu.c

   This file is part of BreezySTM32.

   BreezySTM32 is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   BreezySTM32 is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with BreezySTM32.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <Arduino.h>
#include <MPU6050.h>

#include <math.h>

// This is generally where all Invensense devices are at, for default (AD0 down) I2C address
#define MPU_ADDRESS                         (0x68)

#define GYRO_INT_PIN                        (Pin_13)

#define MPU_RA_WHO_AM_I                     (0x75)
#define MPU_RA_GYRO_XOUT_H                  (0x43)
#define MPU_RA_ACCEL_XOUT_H                 (0x3B)
#define MPU_RA_TEMP_OUT_A                   (0x41)
// For debugging/identification purposes
#define MPU_RA_XA_OFFS_H                    (0x06)    //[15:0] XA_OFFS
#define MPU_RA_PRODUCT_ID                   (0x0C)    // Product ID Register

// WHO_AM_I register contents for 6050
#define MPUx0x0_WHO_AM_I_CONST              (0x68)

enum lpf_e {
    INV_FILTER_256HZ_NOLPF2 = 0,
    INV_FILTER_188HZ,
    INV_FILTER_98HZ,
    INV_FILTER_42HZ,
    INV_FILTER_20HZ,
    INV_FILTER_10HZ,
    INV_FILTER_5HZ,
    INV_FILTER_2100HZ_NOLPF,
    NUM_FILTER
};

enum gyro_fsr_e {
    INV_FSR_250DPS = 0,
    INV_FSR_500DPS,
    INV_FSR_1000DPS,
    INV_FSR_2000DPS,
    NUM_GYRO_FSR
};

enum clock_sel_e {
    INV_CLK_INTERNAL = 0,
    INV_CLK_PLL,
    NUM_CLK
};

enum accel_fsr_e {
    INV_FSR_2G = 0,
    INV_FSR_4G,
    INV_FSR_8G,
    INV_FSR_16G,
    NUM_ACCEL_FSR
};


// Lowpass
static uint8_t mpuLowPassFilter = INV_FILTER_42HZ;

// MPU6xxx registers
#define MPU_RA_SMPLRT_DIV       0x19
#define MPU_RA_CONFIG           0x1A
#define MPU_RA_GYRO_CONFIG      0x1B
#define MPU_RA_ACCEL_CONFIG     0x1C
#define MPU_RA_FIFO_EN          0x23
#define MPU_RA_I2C_MST_CTRL     0x24
#define MPU_RA_INT_PIN_CFG      0x37
#define MPU_RA_INT_ENABLE       0x38
#define MPU_RA_SIGNAL_PATH_RST  0x68
#define MPU_RA_USER_CTRL        0x6A
#define MPU_RA_PWR_MGMT_1       0x6B
#define MPU_RA_PWR_MGMT_2       0x6C
#define MPU_RA_FIFO_COUNT_H     0x72
#define MPU_RA_FIFO_R_W         0x74

// MPU6050 bits
#define MPU6050_INV_CLK_GYROZ   0x03
#define MPU6050_BIT_FIFO_RST    0x04
#define MPU6050_BIT_DMP_RST     0x08
#define MPU6050_BIT_FIFO_EN     0x40

static bool mpuReadRegisterI2C(uint8_t reg, uint8_t *data, int length)
{
    return Wire.read(MPU_ADDRESS, reg, length, data);
}

static bool mpuWriteRegisterI2C(uint8_t reg, uint8_t data)
{
    Wire.beginTransmission(MPU_ADDRESS);
    return Wire.write(reg, data) ? Wire.endTransmission() : false;
}

void MPU6050::begin(void)
{
    uint8_t rev;
    uint8_t tmp[6];

    // determine product ID and accel revision
    mpuReadRegisterI2C(MPU_RA_XA_OFFS_H, tmp, 6);
    rev = ((tmp[5] & 0x01) << 2) | ((tmp[3] & 0x01) << 1) | (tmp[1] & 0x01);
    if (rev) {
        // Congrats, these parts are better
        if (rev == 1) {
        } else if (rev == 2) {
        } else {
        }
    } else {
        mpuReadRegisterI2C(MPU_RA_PRODUCT_ID, &rev, 1);
        rev &= 0x0F;
        if (!rev) {
        } else if (rev == 4) {
        } else {
        }
    }

    // Device reset
    mpuWriteRegisterI2C(MPU_RA_PWR_MGMT_1, 0x80); // Device reset
    delay(100);

    // Gyro config
    mpuWriteRegisterI2C(MPU_RA_SMPLRT_DIV, 0x00); // Sample Rate = Gyroscope Output Rate / (1 + SMPLRT_DIV)
    mpuWriteRegisterI2C(MPU_RA_PWR_MGMT_1, MPU6050_INV_CLK_GYROZ); // Clock source = 3 (PLL with Z Gyro reference)
    delay(10);
    mpuWriteRegisterI2C(MPU_RA_CONFIG, mpuLowPassFilter); // set DLPF
    mpuWriteRegisterI2C(MPU_RA_GYRO_CONFIG, INV_FSR_2000DPS << 3); // full-scale 2kdps gyro range

    // Accel scale 8g (4096 LSB/g)
    mpuWriteRegisterI2C(MPU_RA_ACCEL_CONFIG, INV_FSR_8G << 3);

    // Data ready interrupt configuration:  INT_RD_CLEAR_DIS, I2C_BYPASS_EN
    mpuWriteRegisterI2C(MPU_RA_INT_PIN_CFG, 0 << 7 | 0 << 6 | 0 << 5 | 1 << 4 | 0 << 3 | 0 << 2 | 1 << 1 | 0 << 0);
    mpuWriteRegisterI2C(MPU_RA_INT_ENABLE, 0x01); // DATA_RDY_EN interrupt enable

}


void MPU6050::readAccel(int16_t *accData)
{
    uint8_t buf[6];

    mpuReadRegisterI2C(MPU_RA_ACCEL_XOUT_H, buf, 6);

    accData[0] = (int16_t)((buf[0] << 8) | buf[1]);
    accData[1] = (int16_t)((buf[2] << 8) | buf[3]);
    accData[2] = (int16_t)((buf[4] << 8) | buf[5]);
}


void MPU6050::readGyro(int16_t *gyroData)
{
    uint8_t buf[6];

    mpuReadRegisterI2C(MPU_RA_GYRO_XOUT_H, buf, 6);

    gyroData[0] = (int16_t)((buf[0] << 8) | buf[1]);
    gyroData[1] = (int16_t)((buf[2] << 8) | buf[3]);
    gyroData[2] = (int16_t)((buf[4] << 8) | buf[5]);
}
