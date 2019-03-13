#pragma once

#include "driver/gpio.h"

// GPIO DIRN 0 -> OUTPUT
// GPIO DIRN 1 -> INPUT

class FGPO{
private:

  int gpio_num;

public:

  FGPO(int num){
    gpio_num = num;
    gpio_pad_select_gpio(gpio_num);
    gpio_set_direction(static_cast<gpio_num_t>(gpio_num), GPIO_MODE_OUTPUT);
  }

  void setHigh(void) {
    gpio_set_level(static_cast<gpio_num_t>(gpio_num), 1);
  }

  void setLow(void) {
    gpio_set_level(static_cast<gpio_num_t>(gpio_num), 0);
  }

  void setOut(bool state) {
    gpio_set_level(static_cast<gpio_num_t>(gpio_num), state);
  }
};

class FGPI{
private:

  int gpio_num;
  bool gpio_state;

public:

  FGPI(int num){
    gpio_num = num;
    gpio_pad_select_gpio(gpio_num);
    gpio_set_direction(static_cast<gpio_num_t>(gpio_num), GPIO_MODE_INPUT);
  }

  bool getIn(void) {
    return (bool)gpio_get_level(static_cast<gpio_num_t>(gpio_num));
  }

  void setPull(bool pull) {
    if (pull == 0) {gpio_set_pull_mode(static_cast<gpio_num_t>(gpio_num),GPIO_PULLDOWN_ONLY);}
    else if (pull == 1) {gpio_set_pull_mode(static_cast<gpio_num_t>(gpio_num),GPIO_PULLUP_ONLY);}
  }

  static void initInterrupt(void){
    gpio_install_isr_service(0);
  }
  static void endInterrupt(void){
    gpio_uninstall_isr_service();
  }

  void setInterrupt(int mode, gpio_isr_t gpio_isr_handler) {
    // gpio_intr_enable(static_cast<gpio_num_t>(gpio_num))
    gpio_int_type_t mode_type;
    switch (mode) {
      case 0:
        mode_type = GPIO_INTR_DISABLE;
        break;
      case 1:
        mode_type = GPIO_INTR_POSEDGE;
        break;
      case -1:
        mode_type = GPIO_INTR_NEGEDGE;
        break;
      case -2:
        mode_type = GPIO_INTR_LOW_LEVEL;
        break;
      case 2:
        mode_type = GPIO_INTR_HIGH_LEVEL;
        break;
      default:
        mode_type = GPIO_INTR_ANYEDGE;
    }

    gpio_set_intr_type(static_cast<gpio_num_t>(gpio_num), mode_type);
    gpio_isr_handler_add(static_cast<gpio_num_t>(gpio_num), gpio_isr_handler, (void*)static_cast<gpio_num_t>(gpio_num));
    // (void*)static_cast<gpio_num_t>(gpio_num)
  }

  void removeInterrupt(void) {
    gpio_isr_handler_remove(static_cast<gpio_num_t>(gpio_num));
  }
};
