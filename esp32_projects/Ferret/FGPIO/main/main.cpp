#include <iostream>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "FGPIO.hpp"

extern "C" {
  void app_main(void);
}

int counter = 0;
FGPO ledpin(2);

static void IRAM_ATTR myfunc(void* arg)
{
    counter++;
    // std::cout << "Hi, This is from interrupt - " << counter << '\n';
    // vTaskDelay(1000 / portTICK_PERIOD_MS);
    if(counter%2 == 0) ledpin.setHigh();
    if(counter%2 == 1) ledpin.setLow();


}

void app_main(void)
{

  FGPI intpin(4);
  FGPI::initInterrupt();
  intpin.setPull(0);
  intpin.setInterrupt(1,myfunc);
  //
  // int gpio_num = 4;
  //
  // gpio_pad_select_gpio(gpio_num);
  // gpio_set_direction(static_cast<gpio_num_t>(gpio_num), GPIO_MODE_INPUT);
  // gpio_set_pull_mode(static_cast<gpio_num_t>(gpio_num),GPIO_PULLDOWN_ONLY);
  // gpio_set_intr_type(static_cast<gpio_num_t>(gpio_num), GPIO_INTR_POSEDGE);
  // gpio_install_isr_service(0);
  // gpio_isr_handler_add(static_cast<gpio_num_t>(gpio_num), myfunc, (void*)static_cast<gpio_num_t>(gpio_num));

  while (true) {
    // ledpin.setHigh();
    // vTaskDelay(500 / portTICK_PERIOD_MS);
    // ledpin.setLow();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    std::cout << "counter value is - " << counter <<'\n';
  }
  if (counter >= 4) {
    intpin.removeInterrupt();
    FGPI::endInterrupt();
    // gpio_isr_handler_remove(static_cast<gpio_num_t>(gpio_num));
    // gpio_uninstall_isr_service();
  }
}
