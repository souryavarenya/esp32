#include <iostream>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "FNet.hpp"

extern "C" {
  void app_main(void);
}

void app_main(void)
{
    // FNet fnet1;
    FNet::fnetInit();
    FNet::wifiStart();
    FNet::FSockClient fsc1;
    fsc1.startSock();

    for (int i = 0; i < 5; i++) {
      std::cout << "Sending!" << '\n';
      std::stringstream ss;
      ss << "Hi there - " << i;
      fsc1.sendSock(ss.str());
      vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

    fsc1.closeSock();
    // std::cout << "DAMN!" << '\n';
    // vTaskDelay(2000 / portTICK_PERIOD_MS);
    FNet::wifiClose();
}
