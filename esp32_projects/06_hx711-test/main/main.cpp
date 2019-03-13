#include <iostream>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" {
   void app_main(void);
}

void app_main(void)
{
    while (true) {
        std::string hellomsg = "Hello There!";
        std::cout << hellomsg << '\n';
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}
