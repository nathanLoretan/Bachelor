#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef 	ZEPHYR_USED
  #include "AL/Zephyr/zephyr_interface.h"
#endif

#ifdef BAREMETALE_USED
  #include "AL/BareMetal/nrf5x_interface.h"
#endif

// Externa. Interrupt----------------------------------------------------------------------
#define ACC_INT2           30
#define ACC_INT1           31

#define INTERRUPT_PIN		   13

// Serial Configuration---------------------------------------------------------
#define ADC_SPI_CS              CS0
#define ADC_SPI_FREQUENCY       ISPI_FREQ_1M
#define ADC_SPI_BIT_ORDER       ISPI_BIT_ORDER_MSB_FIRST
#define ADC_SPI_IRQ_PRIORITY    ISPI_DEFAULT_IRQ_PRIORITY
#define ADC_SPI_MODE            ISPI_MODE_SCK_HIGH_CAPTURE_LEADING

#define SWG_SPI_CS              CS1
#define SWG_SPI_FREQUENCY       ISPI_FREQ_1M
#define SWG_SPI_BIT_ORDER       ISPI_BIT_ORDER_MSB_FIRST
#define SWG_SPI_IRQ_PRIORITY    ISPI_DEFAULT_IRQ_PRIORITY
#define SWG_SPI_MODE            ISPI_MODE_SCK_LOW_CAPTURE_LEADING

#define ACC_I2C_FREQEUNCY       II2C_FREQ_FAST
#define ACC_I2C_IRQ_PRIORITY    II2C_DEFAULT_IRQ_PRIORITY

// BLE Configuration------------------------------------------------------------
// Unit [ms]
#define ADV_INTERVAL_MIN       30                         // [ms] ranges from 20ms to 10.24s
#define ADV_INTERVAL_MAX       60                         // [ms] ranges from 20ms to 10.24s
#define ADV_TIMEOUT            IBLE_ADV_TIMEOUT_NONE      // [ms]

#define CONN_MIN_INTERVAL     10                        // [ms] ranges from 7.5ms to 4s
#define CONN_MAX_INTERVAL     20                        // [ms] ranges from 7.5ms to 4s
#define CONN_TIMEOUT          4000                      // [ms]

#define SLAVE_LATENCY         0

// Sensors Configuration--------------------------------------------------------
#define ADC_DATA_RATE         ADC_DATA_RATE_5
#define ADC_CONVERSION_MODE   ADC_CONVERSION_CONTINUOUS

#define ACC_DATA_RATE         ACC_DATA_RATE_9
#define ACC_MODE              ACC_MODE_HIGH_RESOLUTION
#define ACC_FIFO              1

#define EXT_INT_FREQ          0.5
#define SOFT_INT_FREQ         1000

#endif  // __CONFIGURATION_H__