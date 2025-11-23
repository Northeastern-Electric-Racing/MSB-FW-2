#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "u_can.h"
#include "main.h"
#include "u_tx_debug.h"
#include "u_tx_queues.h"
#include "u_queues.h"

#include "sht30.h"

extern I2C_HandleTypeDef hi2c1;

static sht30_t sht30;

static uint8_t _sht30_i2c_write(uint8_t *data, uint8_t dev_address, uint8_t length){
    return HAL_I2C_Master_Transmit(&hi2c1, dev_address, data, length, HAL_MAX_DELAY);
}

static uint8_t _sht30_i2c_read(uint8_t *data, uint16_t command, uint8_t dev_address, uint8_t length) {
    return HAL_I2C_Mem_Read(&hi2c1, dev_address, command, sizeof(command), data, length, HAL_MAX_DELAY);
}

static uint8_t _sht30_i2c_blocking_read(uint8_t *data, uint16_t command, uint8_t dev_address, uint8_t length) {
    uint8_t command_buffer[2] = { (command & 0xff00u) >> 8u, command & 0xffu };
    _sht30_i2c_write(command_buffer, dev_address, sizeof(command_buffer));
    tx_thread_sleep(1);
    return HAL_I2C_Master_Receive(&hi2c1, dev_address, data, length, HAL_MAX_DELAY);
}

uint8_t init_sht30() {
    return sht30_init(&sht30, (Write_ptr) _sht30_i2c_write, (Read_ptr) _sht30_i2c_read, _sht30_i2c_blocking_read, SHT30_I2C_ADDR);
}

uint8_t read_sht30() {
    int status = sht30_get_temp_humid(&sht30);

    if (status) {
        PRINTLN_ERROR("ERROR: Failed to get sht30 data (Status: %d/%s).", status, hal_status_toString(status));
        return U_ERROR;
    }

    struct __attribute__((__packed__)) {
		float temp;
		float humidity;
	} data;

    data.temp = sht30.temp;
    data.humidity = sht30.humidity;

    can_msg_t can_message = { .id = SHT30_CAN_ID, .len = 8, .data = { 0 } };

    memcpy(can_message.data, &data, can_message.len);

    queue_send(&can_outgoing, &can_message, TX_NO_WAIT);

    return U_SUCCESS;
}