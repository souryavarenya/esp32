#include <iostream>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/portmacro.h"
#include "freertos/queue.h"

#include "driver/periph_ctrl.h"
#include "driver/gpio.h"
#include "driver/pcnt.h"

#include "esp_attr.h"
#include "esp_log.h"
#include "soc/gpio_sig_map.h"

#include "fmotor.hpp"

extern "C" {
   void app_main(void);
}

void app_main(void)
{
    while (true) {
        std::string hellomsg = "Hello There!";
        std::cout << hellomsg << '\n';
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
