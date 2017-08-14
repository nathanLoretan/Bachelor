#include "../configuration.h"
#include "../Drivers/acc.h"
#include "../Drivers/adc.h"
#include "../Drivers/swg.h"

// BLE--------------------------------------------------------------------------
#if ENABLE_BLE

#define UUID_BASE    		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xB1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

#define ACC_SVC     			0x0ACC
#define ACC_CHRC_DATA   	0x1ACC
#define ACC_CHRC_CLICK  	0x2ACC

acc_sample_t  sample = {0};
uint8_t       acc_click = 0;
uint32_t      adc_measurement = 0;

iBleP_svc_t acc_svc = {
  .nbr_attrs = 7,
  .attrs = {
    ADD_SVC_DECL(UUID128(ACC_SVC, UUID_BASE)),
    ADD_CHRC_DECL(UUID128(ACC_CHRC_DATA, UUID_BASE),
                  IBLEP_CHRC_PROPS_READ | IBLEP_CHRC_PROPS_NOTIFY,
                  IBLEP_ATTR_PERM_READ | IBLEP_ATTR_PERM_WRITE, NULL, &sample),
    ADD_DESC_CCC(),
    ADD_CHRC_DECL(UUID128(ACC_CHRC_CLICK, UUID_BASE),
                  IBLEP_CHRC_PROPS_READ | IBLEP_CHRC_PROPS_NOTIFY,
                  IBLEP_ATTR_PERM_READ | IBLEP_ATTR_PERM_WRITE, NULL, &acc_click),
    ADD_DESC_CCC(),
  },
};

#define ADC_SVC     		0x0ADC
#define ADC_CHRC_DATA   0x1ADC

iBleP_svc_t adc_svc = {
  .nbr_attrs = 4,
  .attrs = {
    ADD_SVC_DECL(UUID128(ADC_SVC, UUID_BASE)),
    ADD_CHRC_DECL(UUID128(ADC_CHRC_DATA, UUID_BASE),
                  IBLEP_CHRC_PROPS_READ | IBLEP_CHRC_PROPS_NOTIFY,
                  IBLEP_ATTR_PERM_READ | IBLEP_ATTR_PERM_WRITE, NULL, &adc_measurement),
    ADD_DESC_CCC(),
  },
};

iBleP_adv_params_t adv_params = {
	.interval = ADV_INTERVAL,
	.timeout 	= ADV_TIMEOUT,
};

iBleP_advdata_t advdata[] = {
  ADD_ADVDATA(IBLEP_ADVDATA_FLAGS, IBLEP_FLAGS_GENERAL | IBLEP_FLAGS_NO_BREDR),
  ADD_ADVDATA(IBLEP_ADVDATA_UUID16_ALL, 0xCC, 0x0A, 0xDC, 0x0A),
};

iBleP_advdata_t scanrsp[] = {
  ADD_ADVDATA_TEXT(IBLEP_ADVDATA_NAME_COMPLETE, IBLEP_DEVICE_NAME),
};
#endif  // ENABLE_BLE

// Interrupts-------------------------------------------------------------------
#if ENABLE_SWG

typedef enum {
	SWG_EVENT_FREQ = 0,
} swgEvent_t;

iEventQueue_t swg_EventQueue;

iGpio_t interrupt;
IGPIO_HANDLER(on_interrupt, pin)
{
  EXT_INT_LATENCY();
}
#endif  // ENABLE_SWG

// Button-----------------------------------------------------------------------
#if ENABLE_ADC | ENABLE_ACC | ENABLE_SWG
int debouncer_ms = 200;
bool btn_adc_debouncer  = false;
bool btn_acc_debouncer  = false;
bool btn_swg_debouncer  = false;
bool btn_freq_debouncer = false;

static iTimer_t debouncer_timer;
ITIMER_HANDLER(on_debouncer_timer)
{
	btn_adc_debouncer  = false;
	btn_acc_debouncer  = false;
	btn_swg_debouncer  = false;
  btn_freq_debouncer = false;
	iTimer_stop(&debouncer_timer);
}
#endif  // ENABLE_ADC | ENABLE_ACC | ENABLE_SWG

#if ENABLE_ADC
static iGpio_t btn_adc;
IGPIO_HANDLER(on_btn_adc, pin)
{
	static bool isEnabled = false;

	if(btn_adc_debouncer)
		return;

	if(isEnabled)
	{
		iPrint("-> ADC disabled\n");
    adc_sleep();
		isEnabled = false;
	}
	else
	{
		iPrint("-> ADC enabled\n");
    adc_wakeup();
		isEnabled = true;
	}

	btn_adc_debouncer = true;
	iTimer_start(&debouncer_timer, on_debouncer_timer, debouncer_ms);

}
#endif  // ENABLE_ADC

#if ENABLE_ACC
static iGpio_t btn_acc;
IGPIO_HANDLER(on_btn_acc, pin)
{
	static bool isEnabled = false;

	if(btn_acc_debouncer)
		return;

	if(isEnabled)
	{
		iPrint("-> ACC disabled\n");
    acc_sleep();
		isEnabled = false;
	}
	else
	{
		iPrint("-> ACC enabled\n");
    acc_wakeup();
		isEnabled = true;
	}

	btn_acc_debouncer = true;
	iTimer_start(&debouncer_timer, on_debouncer_timer, debouncer_ms);

}
#endif  // ENABLE_ACC

#if ENABLE_SWG
iGpio_t btn_freq;
IGPIO_HANDLER(on_btn_freq, pin)
{
  if(btn_freq_debouncer)
    return;

  iEventQueue_add(&swg_EventQueue, SWG_EVENT_FREQ);

  btn_freq_debouncer = true;
	iTimer_start(&debouncer_timer, on_debouncer_timer, debouncer_ms);

}

static iGpio_t btn_swg;
IGPIO_HANDLER(on_btn_swg, pin)
{
	static bool isEnabled = false;

	if(btn_swg_debouncer)
		return;

	if(isEnabled)
	{
		iPrint("-> SWG disabled\n");
    swg_sleep();
    iGpio_disable_interrupt(&interrupt);
    iGpio_disable_interrupt(&btn_freq);
		isEnabled = false;
	}
	else
	{
		iPrint("-> SWG enabled\n");
    swg_wakeup();
    iGpio_enable_interrupt(&interrupt);
    iGpio_enable_interrupt(&btn_freq);
		isEnabled = true;
	}

	btn_swg_debouncer = true;
	iTimer_start(&debouncer_timer, on_debouncer_timer, debouncer_ms);

}
#endif  // ENABLE_SWG

// Threads----------------------------------------------------------------------
#if ENABLE_BLE
DEFINE_ITHREAD(ble_thread, 4096, 0);
ITHREAD_HANDLER(ble)
{
  while(1)
  {
    // Wait for event
    while(iEventQueue_isEmpty(&bleP_EventQueue)) { iThread_sleep(); }

    switch(iEventQueue_get(&bleP_EventQueue))
		{
			case BLEP_EVENT_CONNECTED:
		  {

		  } break;

			case BLEP_EVENT_DISCONNECTED:
			{

			} break;

			default: // NOTHING
			break;
  	}
  }
}
#endif  // ENABLE_BLE

#if ENABLE_SWG
DEFINE_ITHREAD(swg_thread, 4096, 0);
ITHREAD_HANDLER(swg)
{
  static float ext_int_freq = EXT_INT_FREQ;

  while(1)
  {
    // Wait for event
    while(iEventQueue_isEmpty(&swg_EventQueue)) { iThread_sleep(); }

    switch(iEventQueue_get(&swg_EventQueue))
		{
	    case SWG_EVENT_FREQ:
	    {
		      CHANGE_FREQUENCY(ext_int_freq);

		      iGpio_disable_interrupt(&interrupt);
		      swg_set_frequency(ext_int_freq);
		      iGpio_enable_interrupt(&interrupt);

		      iPrint("Interrupt frequency: %d\n", (int) ext_int_freq);

	    } break;

			default: // NOTHING
			break;
		}
  }
}
#endif  // ENABLE_SWG

#if ENABLE_ACC
DEFINE_ITHREAD(acc_thread, 4096, 0);
ITHREAD_HANDLER(acc)
{
	#if !ENABLE_BLE
		static acc_sample_t sample;
		static uint8_t acc_click = 0;
	#endif	// !ENABLE_BLE

  while(1)
  {
    // Wait for event
    while(iEventQueue_isEmpty(&acc_EventQueue)) { iThread_sleep(); }

    switch(iEventQueue_get(&acc_EventQueue))
		{
			case ACC_EVENT_INT1:  // FIFO full
			{
		    acc_getXYZ(&sample, 1);

				#if ENABLE_BLE
					iBleP_svc_notify(&acc_svc.attrs[1], (uint8_t*) &sample, sizeof(sample));
				#endif	// ENABLE_BLE

				// iPrint("XYZ: 0x%04x, 0x%04x, 0x%04x\n", sample.x, sample.y, sample.z);

			} break;

    	case ACC_EVENT_INT2: // Click
    	{
				#if ENABLE_BLE
					acc_click++;
					iBleP_svc_notify(&acc_svc.attrs[4], (uint8_t*) &acc_click, sizeof(acc_click));
				#endif	// ENABLE_BLE

				// iPrint("Click\n");

			} break;

			default: // NOTHING
			break;
  	}
	}
}
#endif  // ENABLE_ACC

#if ENABLE_ADC
DEFINE_ITHREAD(adc_thread, 4096, 0);
ITHREAD_HANDLER(adc)
{
	#if !ENABLE_BLE
		static uint32_t adc_measurement;
	#endif	// !ENABLE_BLE

  while(1)
  {
    // Wait for event
    while(iEventQueue_isEmpty(&adc_EventQueue)) { iThread_sleep(); }

    switch(iEventQueue_get(&adc_EventQueue))
		{
			case ADC_EVENT_DATA:  // Data Ready
			{
				adc_getMeasurement(&adc_measurement);

				#if ENABLE_BLE
				    iBleP_svc_notify(&adc_svc.attrs[1], (uint8_t*) &adc_measurement,
                             sizeof(adc_measurement));
				#endif	// ENABLE_BLE

				// iPrint("Measurement: %u[uV]\n", adc_measurement);

			} break;

			default: // NOTHING
			break;
		}
  }
}
#endif  // ENABLE_ADC

// -----------------------------------------------------------------------------
void ble_init();
void sys_init();

int main()
{
  iPrint("\nPeripheral\n");
	iPrint("----------\n");

  // iDebug_init();

	#if ENABLE_BLE
	  ble_init();
	#endif  // ENABLE_BLE

	#if ENABLE_SWG || ENABLE_ADC || ENABLE_ACC
	  sys_init();
	#endif	// ENABLE_SWG || ENABLE_ADC || ENABLE_ACC

  while(1)
  {
    iThreads_start();
  }

  iPrint("-> Exit\n");
  return 0;
}

#if ENABLE_BLE
void ble_init()
{
  iPrint("\nInitialize Bluetooth\n");
  iPrint("--------------------\n");

  iBleP_init();
  iBleP_svc_init(&acc_svc);
  iBleP_svc_init(&adc_svc);
  iBleP_adv_start(&adv_params, advdata, sizeof(advdata)/sizeof(iBleP_advdata_t),
                               scanrsp, sizeof(scanrsp)/sizeof(iBleP_advdata_t));

	iThread_run(&ble_thread, ble);
}
#endif	// ENABLE_BLE

#if ENABLE_SWG || ENABLE_ADC || ENABLE_ACC
void sys_init()
{
  iPrint("\nInitialize ExtBoard\n");
  iPrint("-------------------\n");

	#if ENABLE_SWG

		iSpi_init(SWG_SPI, SWG_SPI_FREQUENCY, SWG_SPI_MODE, SWG_SPI_BIT_ORDER);
		swg_init(EXT_INT_FREQ);
    swg_sleep();

		iEventQueue_init(&swg_EventQueue);
		iThread_run(&swg_thread, swg);

		iGpio_interrupt_init(&interrupt, INTERRUPT_PIN, IGPIO_RISING_EDGE, IGPIO_PULL_NORMAL, on_interrupt);
    iGpio_interrupt_init(&btn_freq, BTN_FREQ_PIN, IGPIO_RISING_EDGE, IGPIO_PULL_UP, on_btn_freq);

    iGpio_interrupt_init(&btn_swg, BTN_SWG, IGPIO_RISING_EDGE, IGPIO_PULL_UP, on_btn_swg);
    iGpio_enable_interrupt(&btn_swg);

	#endif  // ENABLE_SWG

	#if ENABLE_ADC

	  iSpi_init(ADC_SPI, ADC_SPI_FREQUENCY, ADC_SPI_MODE, ADC_SPI_BIT_ORDER);
		adc_init();
    adc_sleep();

		iThread_run(&adc_thread, adc);

    iGpio_interrupt_init(&btn_adc,  BTN_ADC,  IGPIO_RISING_EDGE, IGPIO_PULL_UP, on_btn_adc);
    iGpio_enable_interrupt(&btn_adc);

	#endif  // ENABLE_ADC

	#if ENABLE_ACC

	  iI2c_init(ACC_I2C, ACC_I2C_FREQEUNCY);
		acc_init();
    acc_sleep();

    iThread_run(&acc_thread, acc);

    iGpio_interrupt_init(&btn_acc, BTN_ACC, IGPIO_RISING_EDGE, IGPIO_PULL_UP, on_btn_acc);
    iGpio_enable_interrupt(&btn_acc);

	#endif  // ENABLE_ACC
}
#endif	// ENABLE_SWG || ENABLE_ADC || ENABLE_ACC
