#include <iostream>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "network_lib.hpp"

extern "C" {
  void app_main(void);
}

void app_main(void)
{
    network_init();

    FSocketClient fsc1;
    fsc1.finitialize();
    fsc1.fconnect();

    int i = 0;

    while (true) {
        std::cout << "Sending!" << '\n';

        std::stringstream ss;
        ss << "Hi there - " << i;
        fsc1.fsend(ss.str());
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        i += 1;
        if (i >= 5) {
          break;
        }
    }
    fsc1.fclose();

    network_close();
}
