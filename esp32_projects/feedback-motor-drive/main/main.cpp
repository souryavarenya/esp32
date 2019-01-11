#include <iostream>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// #include "freertos/portmacro.h"
// #include "freertos/queue.h"
// MCPWM, GPIO and PCNT drivers included in "fmotor.hpp"

#include "esp_attr.h"
#include "esp_log.h"
#include "soc/gpio_sig_map.h"

#define NUM_OF_FMOTORS 1
#include "fmotor.hpp"

extern "C" {
   void app_main(void);
}

void app_main(void)
{
    std::cout << "Hello! Started!" << '\n';
    fmotor::fmotor_timer_launch();
}
