#include <iostream>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "network_lib.hpp"

#include <stdlib.h>

extern "C" {
  void app_main(void);
}

void app_main(void)
{
    network_init();
    FSocketClient fsc1;
    fsc1.finitialize();
    fsc1.fconnect();

    float rv;
    float timecode = 0.0;

    while(true) {
      //std::cout << "Sending!" << '\n';
      std::stringstream ss;
      rv = (float)(10*sin(0.5*timecode) + 0.01*(rand()%100));
      ss << timecode << "," << rv << "," << 20 << ";";
      fsc1.fsend(ss.str());
      timecode += 0.01;
      vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    fsc1.fclose();

    network_close();
}
