#include "../configuration.h"
#include "../Board/acc.h"
#include "../Board/adc.h"
#include "../Board/swg.h"

iGpio_t gpio1;
iGpio_t gpio2;

iGpio_t int1;
IGPIO_HANDLER(handler1, pin)
{
  static int cnt = 0;
  cnt++;

  iPrint("Interruption pin 11 %d\n", cnt);
}

iGpio_t int2;
IGPIO_HANDLER(handler2, pin)
{
  static int cnt = 0;
  cnt++;

  iPrint("Interruption pin 12 %d\n", cnt);
}

iTimer_t timer1;
ITIMER_HANDLER(timer_handler1)
{
  static int cnt = 0;
  cnt = (cnt + 1) % 2;

  iGpio_write(&gpio1, cnt);
}

iTimer_t timer2;
ITIMER_HANDLER(timer_handler2)
{
  static int cnt = 0;
  cnt = (cnt + 1) % 2;

  iGpio_write(&gpio2, cnt);
}

int main()
{
  iPrint("Programme started\n");
  iPrint("-----------------\n");

  iGpio_interrupt_init(&int1, 11, IGPIO_RISING_EDGE, IGPIO_PULL_UP, handler1);
  iGpio_enable_interrupt(&int1);

  iGpio_interrupt_init(&int2, 12, IGPIO_RISING_EDGE, IGPIO_PULL_UP, handler2);
  iGpio_enable_interrupt(&int2);

  iGpio_init(&gpio1, 13, IGPIO_PIN_OUT, IGPIO_PULL_NORMAL);
  iGpio_init(&gpio2, 14, IGPIO_PIN_OUT, IGPIO_PULL_NORMAL);

  iTimer_start(&timer1, timer_handler1, 10);
  iTimer_start(&timer2, timer_handler2, 20);

  while(1)
  {
    iSleep();
  }

  iPrint("-> Exit\n");
  return 0;
}