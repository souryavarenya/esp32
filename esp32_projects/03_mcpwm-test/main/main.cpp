#include <iostream>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"

#define PWM_PIN 4
#define DIR_PIN 2

extern "C" {
   void app_main(void);
}

void fmotor_mcpwm_init() {

  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, PWM_PIN);
  gpio_pad_select_gpio(static_cast<gpio_num_t>(DIR_PIN));
  gpio_set_direction(static_cast<gpio_num_t>(DIR_PIN), GPIO_MODE_OUTPUT);
  // DEBUG
  std::cout << "[fm0] Initialized MCPWM Hardware..." << '\n';

  mcpwm_config_t pwm_config;
  pwm_config.frequency = 25000;
  pwm_config.cmpr_a = 0;
  pwm_config.cmpr_b = 0;
  pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
  pwm_config.counter_mode = MCPWM_UP_COUNTER;

  mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);
  // DEBUG
  std::cout << "[fm0] Initialized MCPWM Driver..." << '\n';
}

void motor_drive(float duty, bool dirn) {
  gpio_set_level(static_cast<gpio_num_t>(DIR_PIN), dirn);
  mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, duty);
  // mcpwm_set_duty_type(fmotor_int_config.mcpwm_unit, MCPWM_TIMER_0, fmotor_int_config.mcpwm_opr, MCPWM_DUTY_MODE_0); //call this each time, if operator was previously in low/high state
}

void app_main(void)
{
  std::cout << "Hello There!" << '\n';
  fmotor_mcpwm_init();

  float val = 20.0;
  float inc = 5.0;
  bool dir = 0;

  while (true) {

    motor_drive(val,dir);

    vTaskDelay(300 / portTICK_PERIOD_MS);
    if (val == 80.0) {
      inc = -5.0;
    }
    if (val == 20.0) {
      inc = 5.0;
      dir = !dir;
    }

    val += inc;
  }
}
