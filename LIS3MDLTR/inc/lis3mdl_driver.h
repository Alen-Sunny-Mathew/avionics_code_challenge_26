#ifndef LIS3MDL_DRIVER_H
#define LIS3MDL_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

#define LIS3MDL_OUT_X_L ((uint8_t)0x28)
#define LIS3MDL_OUT_Y_L ((uint8_t)0x2A)
#define LIS3MDL_OUT_Z_L ((uint8_t)0x2C)

#define LIS3MDL_DEFAULT_I2C_ADDRESS ((uint8_t)0x1E)

typedef enum {
    LIS3MDL_AXIS_X = LIS3MDL_OUT_X_L,
    LIS3MDL_AXIS_Y = LIS3MDL_OUT_Y_L,
    LIS3MDL_AXIS_Z = LIS3MDL_OUT_Z_L
} lis3mdl_axis_t;

typedef struct {
    bool    is_ready;
    uint8_t addr7;
} lis3mdl_driver_t;

typedef enum {
    LIS3MDL_STATUS_OK = 0,
    LIS3MDL_STATUS_INVALID_ARGUMENT,
    LIS3MDL_STATUS_NOT_INITIALIZED,
    LIS3MDL_STATUS_OUT_OF_RANGE,
    LIS3MDL_STATUS_I2C_ERROR
} lis3mdl_status_t;

typedef enum {
    LIS3MDL_INTERRUPT_DISABLED = 0,
    LIS3MDL_INTERRUPT_ENABLED  = 1
} lis3mdl_interrupt_t;

typedef enum {
    LIS3MDL_DATA_RATE_0_625_HZ = 0,
    LIS3MDL_DATA_RATE_1_25_HZ  = 1,
    LIS3MDL_DATA_RATE_2_5_HZ   = 2,
    LIS3MDL_DATA_RATE_5_HZ     = 3,
    LIS3MDL_DATA_RATE_10_HZ    = 4,
    LIS3MDL_DATA_RATE_20_HZ    = 5,
    LIS3MDL_DATA_RATE_40_HZ    = 6,
    LIS3MDL_DATA_RATE_80_HZ    = 7,

    LIS3MDL_DATA_RATE_155_HZ   = 8,
    LIS3MDL_DATA_RATE_300_HZ   = 9,
    LIS3MDL_DATA_RATE_560_HZ   = 10,
    LIS3MDL_DATA_RATE_1000_HZ  = 11,

    LIS3MDL_DATA_RATE_ENUM_END
} lis3mdl_data_rate_t;

typedef enum {
    LIS3MDL_FULL_SCALE_4G  = 0,
    LIS3MDL_FULL_SCALE_8G  = 1,
    LIS3MDL_FULL_SCALE_12G = 2,
    LIS3MDL_FULL_SCALE_16G = 3
} lis3mdl_full_scale_t;


lis3mdl_status_t lis3mdl_initialize(lis3mdl_driver_t *driver, uint8_t i2c_address);
lis3mdl_status_t lis3mdl_get_full_scale(lis3mdl_driver_t *driver, lis3mdl_full_scale_t *scale);
lis3mdl_status_t lis3mdl_set_data_rate(lis3mdl_driver_t *driver, lis3mdl_data_rate_t rate);
lis3mdl_status_t lis3mdl_get_data_rate(lis3mdl_driver_t *driver, lis3mdl_data_rate_t *rate);
lis3mdl_status_t lis3mdl_set_interrupt(lis3mdl_driver_t *driver, lis3mdl_interrupt_t interrupt_state);
lis3mdl_status_t lis3mdl_read_axis(lis3mdl_driver_t *driver, lis3mdl_axis_t axis, uint16_t *value);

#endif
