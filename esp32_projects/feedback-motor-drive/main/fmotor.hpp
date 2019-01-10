// Includes //
#include "driver/periph_ctrl.h"
#include "driver/gpio.h"
#include "driver/pcnt.h"
#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"

#include "fmotor_gpio_config.hpp"

//Define NUM_OF_FMOTORS elsewhere
#ifndef NUM_OF_FMOTORS
#define NUM_OF_FMOTORS 4
#endif
// External Configuration
struct fmotor_ext_config_t {
  int mot_num;  // ID num of the motor instance 0 to 7
  // GPIO Configuration
  int dir_pin;  // Dirn. to motor driver
  int pwm_pin;  // PWM to motor driver
  int pul_pin;  // Channel A from encoder
  int ctr_pin;  // Channel B from encoder
};

// Internal Configuration
struct fmotor_int_config_t {
  pcnt_unit_t pcnt_unit;
  mcpwm_unit_t mcpwm_unit;
  mcpwm_io_signals_t mcpwm_channel;
  mcpwm_operator_t mcpwm_opr;
};

// Live Configuration
struct fmotor_liv_config_t {
  // Theory Variables
  int fmotor_pos;
  int fmotor_pos_old;

  int fmotor_target;
  int fmotor_target_old;

  int fmotor_err;
  int fmotor_derr;
  // Drive Variables
  float fmotor_duty;
  bool fmotor_dirn;
};

// Physical configuration
struct fmotor_rel_config_t {
  // Encoder angle conversion
  float fmotor_rot_enc_pulses;
  // PD Control parameters
  int fmotor_prop_coeff;
  int fmotor_diff_coeff;
  //
  int fmotor_dt; // in milleseconds
};

class fmotor {

  private:
    // Using externally defined structs as configuration
    fmotor_ext_config_t fmotor_ext_config;
    fmotor_int_config_t fmotor_int_config;
    fmotor_liv_config_t fmotor_liv_config;
    fmotor_rel_config_t fmotor_rel_config;

  public:

    static int fmotor_nums;
    static fmotor fms[NUM_OF_FMOTORS];
    // Default Constructor for mandating initializing parameters
    fmotor(fmotor_ext_config_t fmotor_config_init){
      fmotor_ext_config = fmotor_config_init;
      // DEBUG
      std::cout << "[fm"<< fmotor_ext_config.mot_num <<"] FMotor Configuration Imported..." << '\n';

      // Initializing Drivers
      fmotor_pcnt_init();
      fmotor_mcpwm_init();

      // Initializing live configuration
      fmotor_liv_config.fmotor_pos = 0; // pulse counts
      fmotor_liv_config.fmotor_pos_old = 0; // old pulse count
      fmotor_liv_config.fmotor_del = 0; //
      fmotor_liv_config.fmotor_duty = 0.0;
      fmotor_liv_config.fmotor_dirn = 0;

    }

    // Selects PCNT unit from motor ID
    void fmotor_pcnt_select(void) {
      switch (fmotor_ext_config.mot_num) {
        case 0: fmotor_int_config.pcnt_unit = PCNT_UNIT_0; break;
        case 1: fmotor_int_config.pcnt_unit = PCNT_UNIT_1; break;
        case 2: fmotor_int_config.pcnt_unit = PCNT_UNIT_2; break;
        case 3: fmotor_int_config.pcnt_unit = PCNT_UNIT_3; break;
      }
      // DEBUG
      std::cout << "[fm"<< fmotor_ext_config.mot_num <<"] PCNT Unit Configured..." << '\n';
    }

    // Function for initializing pcnt driver
    void fmotor_pcnt_init(void){
      fmotor_pcnt_select();
      pcnt_config_t pcnt_config = {
        .pulse_gpio_num = fmotor_ext_config.pul_pin,
        .ctrl_gpio_num = fmotor_ext_config.ctr_pin,
        .channel = PCNT_CHANNEL_0,
        .unit = fmotor_int_config.pcnt_unit,
        .pos_mode = PCNT_COUNT_INC,   // Count up on the positive edge
        .neg_mode = PCNT_COUNT_DIS,   // Keep the counter value on the negative edge
        .lctrl_mode = PCNT_MODE_REVERSE, // Reverse counting direction if low
        .hctrl_mode = PCNT_MODE_KEEP,    // Keep the primary counter mode if high
        .counter_h_lim = 10,
        .counter_l_lim = -10,
      };
      pcnt_unit_config(&pcnt_config);
      pcnt_set_filter_value(fmotor_int_config.pcnt_unit, 100);
      pcnt_filter_enable(fmotor_int_config.pcnt_unit);
      pcnt_counter_pause(fmotor_int_config.pcnt_unit);
      pcnt_counter_clear(fmotor_int_config.pcnt_unit);
      pcnt_counter_resume(fmotor_int_config.pcnt_unit);
      std::cout << "[fm"<< fmotor_ext_config.mot_num <<"] Initialized PCNT Driver..." << '\n';
    }

    // Selects MCPWM unit from motor ID
    void fmotor_mcpwm_select(void) {
      switch (fmotor_ext_config.mot_num) {
        case 0: fmotor_int_config.mcpwm_unit = MCPWM_UNIT_0; fmotor_int_config.mcpwm_channel = MCPWM0A; break;
        case 1: fmotor_int_config.mcpwm_unit = MCPWM_UNIT_0; fmotor_int_config.mcpwm_channel = MCPWM1A; break;
        case 2: fmotor_int_config.mcpwm_unit = MCPWM_UNIT_1; fmotor_int_config.mcpwm_channel = MCPWM0A; break;
        case 3: fmotor_int_config.mcpwm_unit = MCPWM_UNIT_1; fmotor_int_config.mcpwm_channel = MCPWM1A; break;
      }
      // DEBUG
      std::cout << "[fm"<< fmotor_ext_config.mot_num <<"] MCPWM Unit Configured..." << '\n';
    }

    // Func for initializing motor pwm driver
    void fmotor_mcpwm_init(void) {
      fmotor_mcpwm_select();
      mcpwm_gpio_init(fmotor_int_config.mcpwm_unit, fmotor_int_config.mcpwm_channel, fmotor_ext_config.pwm_pin);
      gpio_pad_select_gpio(fmotor_ext_config.dir_pin);
      gpio_set_direction(fmotor_ext_config.dir_pin, GPIO_MODE_OUTPUT);
      // DEBUG
      std::cout << "[fm"<< fmotor_ext_config.mot_num <<"] Initialized Motor Hardware..." << '\n';

      mcpwm_config_t pwm_config;
      pwm_config.frequency = 25000;    //frequency = 25000Hz,
      pwm_config.cmpr_a = 0;    //duty cycle of PWMxA = 0
      pwm_config.cmpr_b = 0;    //duty cycle of PWMxB = 0
      pwm_config.counter_mode = MCPWM_UP_COUNTER;
      pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
      mcpwm_init(fmotor_ext_config.mcpwm_unit, MCPWM_TIMER_0, &pwm_config);
      // DEBUG
      std::cout << "[fm"<< fmotor_ext_config.mot_num <<"] Initialized PWM Driver..." << '\n';
    }

    // Func for driving motor
    void fmotor_drive() {
      gpio_set_level(fmotor_ext_config.dir_pin, fmotor_liv_config.fmotor_dirn);
      mcpwm_set_duty(fmotor_int_config.mcpwm_unit, MCPWM_TIMER_0, MCPWM_OPR_A, fmotor_liv_config.fmotor_duty);
      // mcpwm_set_duty_type(fmotor_int_config.mcpwm_unit, MCPWM_TIMER_0, fmotor_int_config.mcpwm_opr, MCPWM_DUTY_MODE_0); //call this each time, if operator was previously in low/high state
    }

    // Func for computing PD Controller
    void fmotor_compute_frame() {
      // Get current position into the address of fmotor_liv_config.fmotor_pos
      pcnt_get_counter_value(fmotor_int_config.pcnt_unit, &fmotor_liv_config.fmotor_pos);
      // Compute the delta - delta in
      fmotor_liv_config.fmotor_err = fmotor_liv_config.fmotor_target - fmotor_liv_config.fmotor_pos;
      fmotor_liv_config.fmotor_derr = fmotor_liv_config.fmotor_pos - fmotor_liv_config.fmotor_pos_old - fmotor_liv_config.fmotor_target + fmotor_liv_config.fmotor_target_old;

      // Computing the duty cycle and dirn.
      fmotor_liv_config.fmotor_duty = 0;// Kp*err + Kd*derr
      if (fmotor_liv_config.fmotor_duty>=0) {
        fmotor_liv_config.fmotor_dirn = 1;
      } else {
        fmotor_liv_config.fmotor_dirn = 0;
        fmotor_liv_config.fmotor_duty = (-1)*fmotor_liv_config.fmotor_duty;
      }



      // Assigning old Variables while exiting
      fmotor_liv_config.fmotor_pos_old = fmotor_liv_config.fmotor_pos;
      fmotor_liv_config.fmotor_target_old = fmotor_liv_config.fmotor_target;

    }

    // Function
    static void fmotor_timer_callback(void* arg){
      // For Each Fmotor
      for (int i = 0; i < NUM_OF_FMOTORS; i++) {
        fms[i].fmotor_compute_frame();
        fms[i].fmotor_drive();
      }
    }

    // Time Engine
    static void fmotor_timer_launch(void) {

      const esp_timer_create_args_t fmotor_timer_args = {
        .callback = &fmotor_timer_callback,
        .name = "fmotor_timer"
      };
      esp_timer_handle_t fmotor_timer;
      // Creates a timer - not started yet!
      esp_timer_create(&fmotor_timer_args, &fmotor_timer);
      // Starts the timer
      esp_timer_start_periodic(fmotor_timer, 1000*fmotor_rel_config.fmotor_dt); //time in 'us'

    }
};

fmotor_ext_config_t fmot_config_0 ={
  .mot_num = 0,
  .dir_pin = FMOTOR_0_DIR,
  .pwm_pin = FMOTOR_0_PWM,
  .pul_pin = FMOTOR_0_PUL,
  .ctr_pin = FMOTOR_0_CTR
};

#if NUM_OF_FMOTORS>1
fmotor_ext_config_t fmot_config_1 ={
  .mot_num = 1,
  .dir_pin = FMOTOR_1_DIR,
  .pwm_pin = FMOTOR_1_PWM,
  .pul_pin = FMOTOR_1_PUL,
  .ctr_pin = FMOTOR_1_CTR
};
#endif

#if NUM_OF_FMOTORS>2
fmotor_ext_config_t fmot_config_2 ={
  .mot_num = 2,
  .dir_pin = FMOTOR_2_DIR,
  .pwm_pin = FMOTOR_2_PWM,
  .pul_pin = FMOTOR_2_PUL,
  .ctr_pin = FMOTOR_2_CTR
};
#endif

#if NUM_OF_FMOTORS>3
fmotor_ext_config_t fmot_config_3 ={
  .mot_num = 3,
  .dir_pin = FMOTOR_3_DIR,
  .pwm_pin = FMOTOR_3_PWM,
  .pul_pin = FMOTOR_3_PUL,
  .ctr_pin = FMOTOR_3_CTR
};
#endif

int fmotor::fmotor_nums = NUM_OF_FMOTORS;
fmotor fmotor::fms[NUM_OF_FMOTORS] = {fmotor(fmot_config_0)
                #if NUM_OF_FMOTORS>1
                ,fmotor(fmot_config_1)
                #endif
                #if NUM_OF_FMOTORS>2
                ,fmotor(fmot_config_2)
                #endif
                #if NUM_OF_FMOTORS>3
                ,fmotor(fmot_config_3)
                #endif
              };
