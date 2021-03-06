#ifndef __II2C__
#define __II2C__

/**
 * @brief SD+SDK I2C interface
 * @{
 */

#include "nrf5x_interface.h"

// Nordic headers
#include "app_twi.h"

/**
 * @brief I2C nRF5x series speed available
 *
 */
#define II2C_FREQ_STANDARD				  NRF_TWI_FREQ_100K
#define II2C_FREQ_FAST						  NRF_TWI_FREQ_400K
#define II2C_FREQ_FAST_PLUS 			  0		// NOT USED
#define II2C_FREQ_HIGH							0		// NOT USED
#define II2C_FREQ_ULTRA						  0		// NOT USED

/**
 * @brief I2C default irq priority
 *
 */
#define II2C_DEFAULT_IRQ_PRIORITY		TWI_DEFAULT_CONFIG_IRQ_PRIORITY

/**
 * @brief help to create data sent via I2C
 *
 */
#define II2C_CREATE_DATA(_tx_data, _bytes...)\
	memcpy(_tx_data, (uint8_t[]) {_bytes}, sizeof( (uint8_t[]) {_bytes}))

/**
 * @brief help to convert data received via I2C
 *
 */
#define II2C_CONVERT_DATA(_data_conv, _data, _data_length)\
	memcpy(_data_conv, _data, _data_length)

/**
 * @brief I2C drivers reference enumeration
 *
 */
typedef enum {
	I2C0 = 0,
	I2C1
}	iI2c_id_t;

/**
 * @typedef iI2c_addr_reg_t
 * @brief bae size of device's register address
 *
 */
typedef uint8_t			iI2c_addr_reg_t;

/**
 * @typedef iI2c_addr_dev_t
 * @brief base size of device's address
 *
 */
typedef uint8_t			iI2c_addr_dev_t;

/**
 * @typedef iI2c_frequency_t
 * @brief frequency used for the I2C communication
 *
 */
typedef uint32_t		iI2c_frequency_t;

/**
 * @brief initialize I2C driver
 * @return	0 if no error or Nordic SDK error type
 *
 * @param[in]	id		id of the I2C driver to initialize
 * @param[in]	freq	frequency of the communication
 *
 */
int iI2c_init(iI2c_id_t id, iI2c_frequency_t freq);

/**
 * @brief read from a register via I2C
 * @return	0 if no error or Nordic SDK error type
 *
 * @param[in]	id						id of the I2C driver to use
 * @param[in]	addr_dev			address of the device to communicate with
 * @param[in]	addr_reg			adddres of the register to read the data from
 * @param[out] data					data to read
 * @param[in]	data_length		length of the data to read
 *
 */
int iI2c_read(iI2c_id_t id, iI2c_addr_dev_t addr_dev, iI2c_addr_reg_t addr_reg,
							uint8_t* data, size_t data_length);

/**
 * @brief write to a register via I2C
 * @return	0 if no error or Nordic SDK error type
 *
 * @param[in]	id						id of the I2C driver to use
 * @param[in]	addr_dev			address of the device to communicate with
 * @param[in]	addr_reg			adddres of the register to read the data from
 * @param[in] data					data to write
 * @param[in]	data_length		length of the data to read
 *
*/
int iI2C_write(iI2c_id_t id, iI2c_addr_dev_t addr_dev, iI2c_addr_reg_t addr_reg,
							 uint8_t* data, size_t data_length);

 /**
  * @}
  */

#endif  // __II2C__
