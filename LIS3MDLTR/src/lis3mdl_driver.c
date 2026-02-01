#include "lis3mdl_driver.h"
#include "i2c.h"

#define LIS3MDL_REG_CTRL1   ((uint8_t)0x20)
#define LIS3MDL_REG_CTRL2   ((uint8_t)0x21)
#define LIS3MDL_REG_INT_CFG ((uint8_t)0x30)

#define LIS3MDL_MASK_DO   ((uint8_t)0x1C)
#define LIS3MDL_MASK_FS   ((uint8_t)0x60)

#define LIS3MDL_BIT_FAST_ODR ((uint8_t)0x02)
#define LIS3MDL_BIT_AUTOINC  ((uint8_t)0x80)

static lis3mdl_status_t io_status(status_t s)
{
    return (s == STATUS_OK) ? LIS3MDL_STATUS_OK : LIS3MDL_STATUS_I2C_ERROR;
}

static uint8_t field_get(uint8_t reg, uint8_t mask, uint8_t shift)
{
    return (uint8_t)((reg & mask) >> shift);
}

static uint8_t field_set(uint8_t reg, uint8_t mask, uint8_t value)
{
    reg &= (uint8_t)~mask;
    reg |= (uint8_t)(value & mask);
    return reg;
}

lis3mdl_status_t lis3mdl_initialize(lis3mdl_driver_t *driver, uint8_t i2c_address)
{
    if (!driver) return LIS3MDL_STATUS_INVALID_ARGUMENT;

    driver->addr7 = (i2c_address != 0u) ? i2c_address : LIS3MDL_DEFAULT_I2C_ADDRESS;
    driver->is_ready = true;
    return LIS3MDL_STATUS_OK;
}



lis3mdl_status_t lis3mdl_get_full_scale(lis3mdl_driver_t *driver, lis3mdl_full_scale_t *scale)
{
    if (!driver || !scale) return LIS3MDL_STATUS_INVALID_ARGUMENT;
    if (!driver->is_ready) return LIS3MDL_STATUS_NOT_INITIALIZED;

    uint8_t v = 0;
    if (io_status(i2c_read(driver->addr7, LIS3MDL_REG_CTRL2, 1, &v)) != LIS3MDL_STATUS_OK)
        return LIS3MDL_STATUS_I2C_ERROR;

    *scale = (lis3mdl_full_scale_t)field_get(v, LIS3MDL_MASK_FS, 5);
    return LIS3MDL_STATUS_OK;
}

lis3mdl_status_t lis3mdl_set_data_rate(lis3mdl_driver_t *driver, lis3mdl_data_rate_t rate)
{
    if (!driver) return LIS3MDL_STATUS_INVALID_ARGUMENT;
    if (!driver->is_ready) return LIS3MDL_STATUS_NOT_INITIALIZED;
    if (rate >= LIS3MDL_DATA_RATE_ENUM_END) return LIS3MDL_STATUS_OUT_OF_RANGE;

    uint8_t ctrl1 = 0;
    if (io_status(i2c_read(driver->addr7, LIS3MDL_REG_CTRL1, 1, &ctrl1)) != LIS3MDL_STATUS_OK)
        return LIS3MDL_STATUS_I2C_ERROR;

    ctrl1 &= (uint8_t)~(LIS3MDL_MASK_DO | LIS3MDL_BIT_FAST_ODR);

    if (rate <= LIS3MDL_DATA_RATE_80_HZ) {
        ctrl1 = field_set(ctrl1, LIS3MDL_MASK_DO, (uint8_t)(rate << 2));
    } else {
        uint8_t fast_idx = (uint8_t)(rate - LIS3MDL_DATA_RATE_155_HZ); /* 0..3 */
        if (fast_idx > 3u) return LIS3MDL_STATUS_OUT_OF_RANGE;

        ctrl1 |= LIS3MDL_BIT_FAST_ODR;
        ctrl1 = field_set(ctrl1, LIS3MDL_MASK_DO, (uint8_t)(fast_idx << 2));
    }

    return io_status(i2c_write(driver->addr7, LIS3MDL_REG_CTRL1, 1, &ctrl1));
}

lis3mdl_status_t lis3mdl_get_data_rate(lis3mdl_driver_t *driver, lis3mdl_data_rate_t *rate)
{
    if (!driver || !rate) return LIS3MDL_STATUS_INVALID_ARGUMENT;
    if (!driver->is_ready) return LIS3MDL_STATUS_NOT_INITIALIZED;

    uint8_t ctrl1 = 0;
    if (io_status(i2c_read(driver->addr7, LIS3MDL_REG_CTRL1, 1, &ctrl1)) != LIS3MDL_STATUS_OK)
        return LIS3MDL_STATUS_I2C_ERROR;

    uint8_t do_bits = field_get(ctrl1, LIS3MDL_MASK_DO, 2);

    if ((ctrl1 & LIS3MDL_BIT_FAST_ODR) != 0u) {
        if (do_bits > 3u) return LIS3MDL_STATUS_OUT_OF_RANGE;
        *rate = (lis3mdl_data_rate_t)(LIS3MDL_DATA_RATE_155_HZ + do_bits);
    } else {
        if (do_bits > 7u) return LIS3MDL_STATUS_OUT_OF_RANGE;
        *rate = (lis3mdl_data_rate_t)do_bits;
    }

    return LIS3MDL_STATUS_OK;
}


lis3mdl_status_t lis3mdl_set_interrupt(lis3mdl_driver_t *driver, lis3mdl_interrupt_t interrupt_state)
{
    if (!driver) return LIS3MDL_STATUS_INVALID_ARGUMENT;
    if (!driver->is_ready) return LIS3MDL_STATUS_NOT_INITIALIZED;

    if (interrupt_state != LIS3MDL_INTERRUPT_DISABLED &&
        interrupt_state != LIS3MDL_INTERRUPT_ENABLED)
        return LIS3MDL_STATUS_OUT_OF_RANGE;

    uint8_t cfg = 0;
    if (io_status(i2c_read(driver->addr7, LIS3MDL_REG_INT_CFG, 1, &cfg)) != LIS3MDL_STATUS_OK)
        return LIS3MDL_STATUS_I2C_ERROR;

    cfg = (uint8_t)((cfg & 0xFEu) | (uint8_t)interrupt_state);
    return io_status(i2c_write(driver->addr7, LIS3MDL_REG_INT_CFG, 1, &cfg));
}

lis3mdl_status_t lis3mdl_read_axis(lis3mdl_driver_t *driver, lis3mdl_axis_t axis, uint16_t *value)
{
    if (!driver || !value) return LIS3MDL_STATUS_INVALID_ARGUMENT;
    if (!driver->is_ready) return LIS3MDL_STATUS_NOT_INITIALIZED;

    if (axis != LIS3MDL_AXIS_X && axis != LIS3MDL_AXIS_Y && axis != LIS3MDL_AXIS_Z)
        return LIS3MDL_STATUS_OUT_OF_RANGE;

    uint8_t buf[2] = {0};
    uint8_t reg = (uint8_t)((uint8_t)axis | LIS3MDL_BIT_AUTOINC);

    if (io_status(i2c_read(driver->addr7, reg, 2, buf)) != LIS3MDL_STATUS_OK)
        return LIS3MDL_STATUS_I2C_ERROR;

    *value = (uint16_t)(((uint16_t)buf[1] << 8) | buf[0]);
    return LIS3MDL_STATUS_OK;
}
