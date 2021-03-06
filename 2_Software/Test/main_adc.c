#include "../configuration.h"
#include "../Drivers/acc.h"
#include "../Drivers/adc.h"
#include "../Drivers/swg.h"

int main()
{
  iPrint("ADC test started\n");
  iPrint("----------------\n");

  iSpi_init(SWG_SPI, SWG_SPI_FREQUENCY, SWG_SPI_MODE, SWG_SPI_BIT_ORDER);
  swg_init(EXT_INT_FREQ);

  iSpi_init(ADC_SPI, ADC_SPI_FREQUENCY, ADC_SPI_MODE, ADC_SPI_BIT_ORDER);
  adc_init();

  uint32_t measurement = 0;

  adc_wakeup();

  while(1)
  {
    // Wait for event
    while(iEventQueue_isEmpty(&adc_EventQueue)) { iSleep(); }

    iEvent_t adcEvent = iEventQueue_get(&adc_EventQueue);
    if(adcEvent == ADC_EVENT_DATA)
    {
      // Notify the central
      adc_getMeasurement(&measurement);
      iPrint("Measurement: %lu[uV]\n", measurement);
    }
  }

  iPrint("-> Exit\n");
  return 0;
}
