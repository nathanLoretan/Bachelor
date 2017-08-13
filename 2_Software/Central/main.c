#include "../configuration.h"

// BLE--------------------------------------------------------------------------
#if ENABLE_BLE
iBleC_scan_params_t conn_params = {
	.type 		= IBLEC_SCAN_ACTIVE,
	.window 	= SCAN_WINDOW,
	.interval = SCAN_INTERVAL,
	.timeout 	= SCAN_TIMEOUT,
}

iBleC_conn_params_t scan_params = {
	.interval_min = CONN_MIN_INTERVAL,
	.interval_max = CONN_MAX_INTERVAL,
	.latency 			= SLAVE_LATENCY,
	.timeout 			= CONN_TIMOUT,
}

#define ADC_SVC     		0x0ADC
#define ADC_CHRC_DATA   0x1ADC
#define ADC_BASE    		0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE2, 0xB1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

#define ACC_SVC     		0x0ACC
#define ACC_CHRC_DATA   0x1ACC
#define ACC_CHRC_CLICK  0x2ACC
#define ACC_BASE    	 	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE1, 0xB1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

iBleC_attr_disc_t attr_disc_list[] =
{
	ADD_SVC_TO_DISCOVER_UUID128(ACC_SVC, ACC_BASE),
	ADD_CHRC_TO_DISCOVER_UUID128(ACC_CHRC_DATA, ACC_BASE),
	ADD_DESC_TO_DISCOVER_UUID16(0x2902),
	ADD_CHRC_TO_DISCOVER_UUID128(ACC_CHRC_CLICK, ACC_BASE),
	ADD_DESC_TO_DISCOVER_UUID16(0x2902),

	ADD_SVC_TO_DISCOVER_UUID128(ADC_SVC, ADC_BASE),
	ADD_CHRC_TO_DISCOVER_UUID128(ADC_CHRC_DATA, ADC_BASE),
	ADD_DESC_TO_DISCOVER_UUID16(0x2902),
};
#endif  // ENABLE_BLE

// Interrupts-------------------------------------------------------------------
#if ENABLE_SWG

typedef enum {
	SWG_EVENT_FREQ = 0,
} swgEvent_t;

iEventQueue_t swg_EventQueue;

// iTimer_t newInterval_timer;
// ITIMER_HANDLER(on_newInterval)
// {
//   iEventQueue_add(&swg_EventQueue, SWG_EVENT_FREQ);
// }

iGpio_t btn_freq;
IGPIO_HANDLER(on_btn_freq, pin)
{
  iEventQueue_add(&swg_EventQueue, SWG_EVENT_FREQ);
}

iGpio_t interrupt;
IGPIO_HANDLER(on_interrupt, pin)
{
  EXT_INT_LATENCY();
}
#endif  // ENABLE_SWG

// -----------------------------------------------------------------------------
IBLEC_NOTIFY_HANDLER(on_acc_data, conn, params)
{
	iPrint("NOTIFY ACC DATA %d, %u\n", conn, *((uint32_t*) params->data));
}

IBLEC_NOTIFY_HANDLER(on_acc_click, conn, params)
{
	iPrint("NOTIFY ACC CLICK %d, %u\n", conn, *((uint32_t*) params->data));
}

IBLEC_NOTIFY_HANDLER(on_adc_data, conn, params)
{
	iPrint("NOTIFY ADC DATA %d, %u\n", conn, *((uint32_t*) params->data));
}

// Threads----------------------------------------------------------------------
#if ENABLE_BLE
DEFINE_ITHREAD(ble_thread, 4096, 0);
ITHREAD_HANDLER(ble)
{
  while(1)
  {
    // Wait for event
    while(iEventQueue_isEmpty(&bleC_EventQueue)) { iThread_sleep(); }

		iEvent_t ble_event = iEventQueue_get(&bleC_EventQueue);
    switch(ble_event)
		{
			case BLEC_EVENT_CONNECTED_BASE + 0:
			case BLEC_EVENT_CONNECTED_BASE + 1:
			case BLEC_EVENT_CONNECTED_BASE + 2:
			case BLEC_EVENT_CONNECTED_BASE + 3:
			case BLEC_EVENT_CONNECTED_BASE + 4:
			case BLEC_EVENT_CONNECTED_BASE + 5:
			case BLEC_EVENT_CONNECTED_BASE + 6:
			case BLEC_EVENT_CONNECTED_BASE + 7:
		  {

		  } break;

			case BLEC_EVENT_READY_BASE + 0:
			case BLEC_EVENT_READY_BASE + 1:
			case BLEC_EVENT_READY_BASE + 2:
			case BLEC_EVENT_READY_BASE + 3:
			case BLEC_EVENT_READY_BASE + 4:
			case BLEC_EVENT_READY_BASE + 5:
			case BLEC_EVENT_READY_BASE + 6:
			case BLEC_EVENT_READY_BASE + 7:
		  {
				iBleC_notify_params_t notify_params;
				notify_params.handler				= on_acc_data;
				notify_params.value_handle	= iBleC_get_chrc_val_handle(BLEC_EVENT_READY_BASE - ble_event, ADC_SVC, ADC_CHRC_DATA);
				notify_params.ccc_handle		= iBleC_get_desc_handle(BLEC_EVENT_READY_BASE - ble_event, ADC_SVC, ADC_CHRC_DATA, 0x2902);
				iBleC_subscribe_notify(BLEC_EVENT_READY_BASE - ble_event, &notify_params);

				notify_params.handler				= on_acc_click;
				notify_params.value_handle	= iBleC_get_chrc_val_handle(BLEC_EVENT_READY_BASE - ble_event, ACC_SVC, ACC_CHRC_DATA);
				notify_params.ccc_handle		= iBleC_get_desc_handle(BLEC_EVENT_READY_BASE - ble_event, ACC_SVC, ACC_CHRC_DATA, 0x2902);
				iBleC_subscribe_notify(BLEC_EVENT_READY_BASE - ble_event, &notify_params);

				notify_params.handler				= on_adc_data;
				notify_params.value_handle	= iBleC_get_chrc_val_handle(BLEC_EVENT_READY_BASE - ble_event, ACC_SVC, ACC_CHRC_CLICK);
				notify_params.ccc_handle		= iBleC_get_desc_handle(BLEC_EVENT_READY_BASE - ble_event, ACC_SVC, ACC_CHRC_CLICK, 0x2902);
				iBleC_subscribe_notify(BLEC_EVENT_READY_BASE - ble_event, &notify_params);

		  } break;

			case BLEC_EVENT_DISCONNECTED_BASE + 0:
			case BLEC_EVENT_DISCONNECTED_BASE + 1:
			case BLEC_EVENT_DISCONNECTED_BASE + 2:
			case BLEC_EVENT_DISCONNECTED_BASE + 3:
			case BLEC_EVENT_DISCONNECTED_BASE + 4:
			case BLEC_EVENT_DISCONNECTED_BASE + 5:
			case BLEC_EVENT_DISCONNECTED_BASE + 6:
			case BLEC_EVENT_DISCONNECTED_BASE + 7:
		  {
				iBleC_notify_params_t notify_params;
				notify_params.handler				= on_acc_data;
				notify_params.value_handle	= iBleC_get_chrc_val_handle(BLEC_EVENT_READY_BASE - ble_event, ADC_SVC, ADC_CHRC_DATA);
				notify_params.ccc_handle		= iBleC_get_desc_handle(BLEC_EVENT_READY_BASE - ble_event, ADC_SVC, ADC_CHRC_DATA, 0x2902);
				iBleC_unsubscribe_notify(BLEC_EVENT_READY_BASE - ble_event, &notify_params);

				notify_params.handler				= on_acc_click;
				notify_params.value_handle	= iBleC_get_chrc_val_handle(BLEC_EVENT_READY_BASE - ble_event, ACC_SVC, ACC_CHRC_DATA);
				notify_params.ccc_handle		= iBleC_get_desc_handle(BLEC_EVENT_READY_BASE - ble_event, ACC_SVC, ACC_CHRC_DATA, 0x2902);
				iBleC_unsubscribe_notify(BLEC_EVENT_READY_BASE - ble_event, &notify_params);

				notify_params.handler				= on_adc_data;
				notify_params.value_handle	= iBleC_get_chrc_val_handle(BLEC_EVENT_READY_BASE - ble_event, ACC_SVC, ACC_CHRC_CLICK);
				notify_params.ccc_handle		= iBleC_get_desc_handle(BLEC_EVENT_READY_BASE - ble_event, ACC_SVC, ACC_CHRC_CLICK, 0x2902);
				iBleC_unsubscribe_notify(BLEC_EVENT_READY_BASE - ble_event, &notify_params);

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

//------------------------------------------------------------------------------

void sys_init();
void ble_init();

int main()
{
	iDebug_init();

	#if ENABLE_BLE
	  ble_init();
	#endif  // ENABLE_BLE

	#if ENABLE_SWG
	  sys_init();
	#endif	// ENABLE_SWG

  while(1)
  {
		iThread_sleep();
  }

  iPrint("-> Exit\n");
  return 0;
}

#if ENABLE_BLE
void ble_init()
{
  iPrint("\nInitialize Bluetooth\n");
  iPrint("--------------------\n");

	iBleC_init(&conn_params);
	iBleC_discovery_init(attr_disc_list, sizeof(attr_disc_list) / sizeof(iBleC_attr_disc_t));
	iBleC_scan_start(&scan_params, IBLE_SEARCHED_DEVICE);

	iThread_run(&ble_thread, ble);
}
#endif	// ENABLE_BLE

#if ENABLE_SWG
void sys_init()
{
	iPrint("\nInitialize ExtBoard\n");
	iPrint("-------------------\n");

	iSpi_init(SWG_SPI, SWG_SPI_FREQUENCY, SWG_SPI_MODE, SWG_SPI_BIT_ORDER);
	swg_init(EXT_INT_FREQ);
	iEventQueue_init(&swg_EventQueue);
	iThread_run(&swg_thread, swg);

	iGpio_interrupt_init(&interrupt, INTERRUPT_PIN, IGPIO_RISING_EDGE, IGPIO_PULL_NORMAL, on_interrupt);
	iGpio_enable_interrupt(&interrupt);

	iGpio_interrupt_init(&btn_freq, BTN_FREQ_PIN, IGPIO_RISING_EDGE, IGPIO_PULL_NORMAL, on_btn_freq);
	iGpio_enable_interrupt(&btn_freq);

	#if ENABLE_BLE
		swg_sleep();
	#endif	// !ENABLE_BLE

	// #if !ENABLE_BLE && !POWER_MEASUREMENT
	// 	iTimer_start(&newInterval_timer, on_newInterval, INTERVAL);
	// #endif	// !ENABLE_BLE && !POWER_MEASUREMENT
}
#endif  // ENABLE_SWG
