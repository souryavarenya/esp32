#include <iostream>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/pcnt.h"

extern "C" {
   void app_main(void);
}

static void pcnt_init(){

    pcnt_config_t pcnt_config = {
        .pulse_gpio_num = 15,
        .ctrl_gpio_num = 4,
        .lctrl_mode = PCNT_MODE_REVERSE,
        .hctrl_mode = PCNT_MODE_KEEP,
        .pos_mode = PCNT_COUNT_INC,
        .neg_mode = PCNT_COUNT_DIS,
        .counter_h_lim = INT16_MAX,
        .counter_l_lim = INT16_MIN,
        .unit = PCNT_UNIT_0,
        .channel = PCNT_CHANNEL_0
    };

    pcnt_unit_config(&pcnt_config);

    /* Configure and enable the input filter */
    // pcnt_set_filter_value(PCNT_UNIT_0, 10);
    // pcnt_filter_enable(PCNT_UNIT_0);

    pcnt_intr_disable(PCNT_UNIT_0);
    /* Initialize PCNT's counter */
    pcnt_counter_pause(PCNT_UNIT_0);
    pcnt_counter_clear(PCNT_UNIT_0);
    pcnt_counter_resume(PCNT_UNIT_0);

    std::cout << "Initialized PCNT unit" << '\n';
}

void app_main(void)
{
    pcnt_init();
    int16_t pcount = 0;
    while (true) {
        pcnt_get_counter_value(PCNT_UNIT_0, &pcount);
        std::cout << pcount << '\n';
        //pcnt_counter_clear(PCNT_UNIT_0);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
