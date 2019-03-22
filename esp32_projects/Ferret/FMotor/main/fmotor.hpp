// Includes //
#include "driver/periph_ctrl.h"
#include "driver/gpio.h"
#include "driver/pcnt.h"
#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"

// Imports Config
#include "fmotor_gpio_config.hpp"

// Imports Target table
// #include "fmotor_data.hpp"

#ifndef FMOTOR_DT
#define FMOTOR_DT 10 //in milleseconds
#endif

//Define NUM_OF_FMOTORS elsewhere
#ifndef NUM_OF_FMOTORS
#define NUM_OF_FMOTORS 1
#endif

// Target Size
#ifndef FMOTOR_TARGET_SIZE
#define FMOTOR_TARGET_SIZE 1000 // 10 seconds worth data a frame per 10 ms
#endif

// External Configuration
struct fmotor_ext_config_t {
  // Static Configuration
  int mot_num;  // ID num of the motor instance 0 to 7
  // GPIO Configuration
  int dir_pin;  // Dirn. to motor driver
  int pwm_pin;  // PWM to motor driver
  int pul_pin;  // Channel A from encoder
  int ctr_pin;  // Channel B from encoder
  // Constants
  // Encoder angle conversion
  int fmotor_rot_enc_pulses; //counts per revolution
  // PD Control parameters
  int fmotor_prop_coeff;
  int fmotor_diff_coeff;

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
  int16_t fmotor_pos;
  int16_t fmotor_pos_old;
  int fmotor_target;
  int fmotor_target_old;

  int fmotor_err;
  int fmotor_derr;
  // Drive Variables
  float fmotor_duty;
  bool fmotor_dirn;
};

class fmotor {

  private:
    // Using externally defined structs as configuration
    fmotor_ext_config_t fmotor_ext_config;
    fmotor_int_config_t fmotor_int_config;

    int fmotor_target_table[FMOTOR_TARGET_SIZE] = {
      8,16,24,33,41,49,57,65,73,81,88,96,103,110,117,124,130,137,143,149,155,160,165,170,175,179,183,187,190,194,196,199,201,203,205,206,207,207,208,208,207,206,205,204,202,200,198,195,192,189,185,181,177,173,168,163,158,152,146,140,134,127,121,114,107,100,92,85,77,69,61,53,45,37,29,21,12,4,-4,-13,-21,-29,-37,-46,-54,-62,-70,-77,-85,-93,-100,-107,-114,-121,-128,-134,-141,-147,-153,-158,-163,-168,-173,-178,-182,-186,-190,-193,-196,-199,-201,-203,-205,-206,-207,-208,-209,-209,-208,-208,-207,-206,-204,-202,-200,-198,-195,-192,-188,-184,-180,-176,-171,-167,-161,-156,-150,-144,-138,-132,-125,-119,-112,-105,-97,-90,-82,-74,-67,-59,-51,-43,-34,-26,-18,-9,-1,7,15,24,32,40,48,56,64,72,80,88,95,102,110,117,123,130,136,142,148,154,160,165,170,174,179,183,187,190,193,196,199,201,203,205,206,207,207,208,208,207,207,206,204,202,200,198,195,192,189,186,182,177,173,168,163,158,152,147,141,134,128,121,114,107,100,93,85,78,70,62,54,46,38,30,21,13,5,-4,-12,-20,-29,-37,-45,-53,-61,-69,-77,-84,-92,-99,-107,-114,-121,-127,-134,-140,-146,-152,-158,-163,-168,-173,-177,-182,-186,-189,-193,-196,-198,-201,-203,-205,-206,-207,-208,-209,-209,-209,-208,-207,-206,-204,-203,-200,-198,-195,-192,-189,-185,-181,-176,-172,-167,-162,-156,-151,-145,-139,-132,-126,-119,-112,-105,-98,-90,-83,-75,-67,-59,-51,-43,-35,-27,-18,-10,-2,7,15,23,31,40,48,56,64,72,79,87,94,102,109,116,123,129,136,142,148,154,159,164,169,174,178,183,186,190,193,196,199,201,203,204,206,207,207,208,208,207,207,206,204,203,201,198,196,193,189,186,182,178,173,169,164,158,153,147,141,135,128,122,115,108,101,93,86,78,71,63,55,47,38,30,22,14,5,-3,-11,-20,-28,-36,-44,-52,-60,-68,-76,-84,-91,-99,-106,-113,-120,-127,-133,-140,-146,-152,-157,-163,-168,-173,-177,-181,-185,-189,-192,-195,-198,-201,-203,-205,-206,-207,-208,-209,-209,-209,-208,-207,-206,-205,-203,-201,-198,-195,-192,-189,-185,-181,-177,-172,-167,-162,-157,-151,-145,-139,-133,-126,-120,-113,-106,-98,-91,-83,-76,-68,-60,-52,-44,-36,-27,-19,-11,-2,6,14,22,31,39,47,55,63,71,79,86,94,101,108,115,122,129,135,142,147,153,159,164,169,174,178,182,186,190,193,196,198,201,203,204,206,207,207,208,208,207,207,206,204,203,201,198,196,193,190,186,182,178,174,169,164,159,153,148,142,135,129,122,116,109,101,94,87,79,71,63,55,47,39,31,23,14,6,-2,-11,-19,-27,-35,-44,-52,-60,-68,-76,-83,-91,-98,-106,-113,-120,-126,-133,-139,-145,-151,-157,-162,-167,-172,-177,-181,-185,-189,-192,-195,-198,-201,-203,-204,-206,-207,-208,-209,-209,-209,-208,-207,-206,-205,-203,-201,-198,-196,-192,-189,-185,-181,-177,-173,-168,-163,-157,-152,-146,-140,-133,-127,-120,-113,-106,-99,-92,-84,-76,-69,-61,-53,-44,-36,-28,-20,-11,-3,5,13,22,30,38,46,54,62,70,78,86,93,101,108,115,122,128,135,141,147,153,158,164,169,173,178,182,186,189,193,196,198,201,203,204,206,207,207,208,208,207,207,206,204,203,201,199,196,193,190,186,183,178,174,169,164,159,154,148,142,136,130,123,116,109,102,95,87,79,72,64,56,48,40,32,23,15,7,-2,-10,-18,-27,-35,-43,-51,-59,-67,-75,-83,-90,-98,-105,-112,-119,-126,-132,-139,-145,-151,-156,-162,-167,-172,-176,-181,-185,-188,-192,-195,-198,-200,-203,-204,-206,-207,-208,-209,-209,-209,-208,-207,-206,-205,-203,-201,-198,-196,-193,-189,-186,-182,-177,-173,-168,-163,-158,-152,-146,-140,-134,-127,-121,-114,-107,-100,-92,-85,-77,-69,-61,-53,-45,-37,-29,-20,-12,-4,5,13,21,29,38,46,54,62,70,78,85,93,100,107,114,121,128,134,141,147,152,158,163,168,173,177,182,185,189,192,195,198,200,202,204,205,207,207,208,208,207,207,206,205,203,201,199,196,193,190,187,183,179,174,170,165,160,154,149,143,136,130,123,117,110,103,95,88,80,72,65,57,48,40,32,24,16,7,-1,-9,-18,-26,-34,-42,-50,-59,-66,-74,-82,-90,-97,-104,-112,-119,-125,-132,-138,-144,-150,-156,-161,-166,-171,-176,-180,-184,-188,-192,-195,-198,-200,-202,-204,-206,-207,-208,-208,-209,-209,-208,-207,-206,-205,-203,-201,-199,-196,-193,-190,-186,-182,-178,-173,-169,-163,-158,-153,-147,-141,-135,-128,-121,-114,-107,-100,-93,-85,-78,-70,-62,-54,-46,-38,-29,-21,-13,-4,4,12,20,29,37,45,53,61,69,77,85,92,99,107,114,121,127,134,140,146,152,157,163,168,173,177,181,185,189,192,195,198,200,202,204,205,206,207,208,208,207,207,206,205,203,201,199,196,194,190,187,183,179,175,170,165,160,155
    };

  public:
    fmotor_liv_config_t fmotor_liv_config;
    static int fmotor_nums;
    static fmotor fms[NUM_OF_FMOTORS];
    static int time_counter;
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

      fmotor_liv_config.fmotor_target_old = 0; //
      fmotor_liv_config.fmotor_target = 0;

      fmotor_liv_config.fmotor_err = 0; //
      fmotor_liv_config.fmotor_derr = 0;

      fmotor_liv_config.fmotor_duty = 0.0;
      fmotor_liv_config.fmotor_dirn = 0;

      /* fmotor_target_table = {
        8,16,24,33,41,49,57,65,73,81,88,96,103,110,117,124,130,137,143,149,155,160,165,170,175,179,183,187,190,194,196,199,201,203,205,206,207,207,208,208,207,206,205,204,202,200,198,195,192,189,185,181,177,173,168,163,158,152,146,140,134,127,121,114,107,100,92,85,77,69,61,53,45,37,29,21,12,4,-4,-13,-21,-29,-37,-46,-54,-62,-70,-77,-85,-93,-100,-107,-114,-121,-128,-134,-141,-147,-153,-158,-163,-168,-173,-178,-182,-186,-190,-193,-196,-199,-201,-203,-205,-206,-207,-208,-209,-209,-208,-208,-207,-206,-204,-202,-200,-198,-195,-192,-188,-184,-180,-176,-171,-167,-161,-156,-150,-144,-138,-132,-125,-119,-112,-105,-97,-90,-82,-74,-67,-59,-51,-43,-34,-26,-18,-9,-1,7,15,24,32,40,48,56,64,72,80,88,95,102,110,117,123,130,136,142,148,154,160,165,170,174,179,183,187,190,193,196,199,201,203,205,206,207,207,208,208,207,207,206,204,202,200,198,195,192,189,186,182,177,173,168,163,158,152,147,141,134,128,121,114,107,100,93,85,78,70,62,54,46,38,30,21,13,5,-4,-12,-20,-29,-37,-45,-53,-61,-69,-77,-84,-92,-99,-107,-114,-121,-127,-134,-140,-146,-152,-158,-163,-168,-173,-177,-182,-186,-189,-193,-196,-198,-201,-203,-205,-206,-207,-208,-209,-209,-209,-208,-207,-206,-204,-203,-200,-198,-195,-192,-189,-185,-181,-176,-172,-167,-162,-156,-151,-145,-139,-132,-126,-119,-112,-105,-98,-90,-83,-75,-67,-59,-51,-43,-35,-27,-18,-10,-2,7,15,23,31,40,48,56,64,72,79,87,94,102,109,116,123,129,136,142,148,154,159,164,169,174,178,183,186,190,193,196,199,201,203,204,206,207,207,208,208,207,207,206,204,203,201,198,196,193,189,186,182,178,173,169,164,158,153,147,141,135,128,122,115,108,101,93,86,78,71,63,55,47,38,30,22,14,5,-3,-11,-20,-28,-36,-44,-52,-60,-68,-76,-84,-91,-99,-106,-113,-120,-127,-133,-140,-146,-152,-157,-163,-168,-173,-177,-181,-185,-189,-192,-195,-198,-201,-203,-205,-206,-207,-208,-209,-209,-209,-208,-207,-206,-205,-203,-201,-198,-195,-192,-189,-185,-181,-177,-172,-167,-162,-157,-151,-145,-139,-133,-126,-120,-113,-106,-98,-91,-83,-76,-68,-60,-52,-44,-36,-27,-19,-11,-2,6,14,22,31,39,47,55,63,71,79,86,94,101,108,115,122,129,135,142,147,153,159,164,169,174,178,182,186,190,193,196,198,201,203,204,206,207,207,208,208,207,207,206,204,203,201,198,196,193,190,186,182,178,174,169,164,159,153,148,142,135,129,122,116,109,101,94,87,79,71,63,55,47,39,31,23,14,6,-2,-11,-19,-27,-35,-44,-52,-60,-68,-76,-83,-91,-98,-106,-113,-120,-126,-133,-139,-145,-151,-157,-162,-167,-172,-177,-181,-185,-189,-192,-195,-198,-201,-203,-204,-206,-207,-208,-209,-209,-209,-208,-207,-206,-205,-203,-201,-198,-196,-192,-189,-185,-181,-177,-173,-168,-163,-157,-152,-146,-140,-133,-127,-120,-113,-106,-99,-92,-84,-76,-69,-61,-53,-44,-36,-28,-20,-11,-3,5,13,22,30,38,46,54,62,70,78,86,93,101,108,115,122,128,135,141,147,153,158,164,169,173,178,182,186,189,193,196,198,201,203,204,206,207,207,208,208,207,207,206,204,203,201,199,196,193,190,186,183,178,174,169,164,159,154,148,142,136,130,123,116,109,102,95,87,79,72,64,56,48,40,32,23,15,7,-2,-10,-18,-27,-35,-43,-51,-59,-67,-75,-83,-90,-98,-105,-112,-119,-126,-132,-139,-145,-151,-156,-162,-167,-172,-176,-181,-185,-188,-192,-195,-198,-200,-203,-204,-206,-207,-208,-209,-209,-209,-208,-207,-206,-205,-203,-201,-198,-196,-193,-189,-186,-182,-177,-173,-168,-163,-158,-152,-146,-140,-134,-127,-121,-114,-107,-100,-92,-85,-77,-69,-61,-53,-45,-37,-29,-20,-12,-4,5,13,21,29,38,46,54,62,70,78,85,93,100,107,114,121,128,134,141,147,152,158,163,168,173,177,182,185,189,192,195,198,200,202,204,205,207,207,208,208,207,207,206,205,203,201,199,196,193,190,187,183,179,174,170,165,160,154,149,143,136,130,123,117,110,103,95,88,80,72,65,57,48,40,32,24,16,7,-1,-9,-18,-26,-34,-42,-50,-59,-66,-74,-82,-90,-97,-104,-112,-119,-125,-132,-138,-144,-150,-156,-161,-166,-171,-176,-180,-184,-188,-192,-195,-198,-200,-202,-204,-206,-207,-208,-208,-209,-209,-208,-207,-206,-205,-203,-201,-199,-196,-193,-190,-186,-182,-178,-173,-169,-163,-158,-153,-147,-141,-135,-128,-121,-114,-107,-100,-93,-85,-78,-70,-62,-54,-46,-38,-29,-21,-13,-4,4,12,20,29,37,45,53,61,69,77,85,92,99,107,114,121,127,134,140,146,152,157,163,168,173,177,181,185,189,192,195,198,200,202,204,205,206,207,208,208,207,207,206,205,203,201,199,196,194,190,187,183,179,175,170,165,160,155
      };
      */

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
        .lctrl_mode = PCNT_MODE_REVERSE,
        .hctrl_mode = PCNT_MODE_KEEP,
        .pos_mode = PCNT_COUNT_INC,
        .neg_mode = PCNT_COUNT_DIS,
        .counter_h_lim = INT16_MAX,
        .counter_l_lim = INT16_MIN,
        .unit = fmotor_int_config.pcnt_unit,
        .channel = PCNT_CHANNEL_0
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
      gpio_pad_select_gpio(static_cast<gpio_num_t>(fmotor_ext_config.dir_pin));
      gpio_set_direction(static_cast<gpio_num_t>(fmotor_ext_config.dir_pin), GPIO_MODE_OUTPUT);
      // DEBUG
      std::cout << "[fm"<< fmotor_ext_config.mot_num <<"] Initialized Motor Hardware..." << '\n';

      mcpwm_config_t pwm_config = {
        .frequency = 25000,
        .cmpr_a = 0,
        .cmpr_b = 0,
        .duty_mode = MCPWM_DUTY_MODE_0,
        .counter_mode = MCPWM_UP_COUNTER
      };

      mcpwm_init(fmotor_int_config.mcpwm_unit, MCPWM_TIMER_0, &pwm_config);
      // DEBUG
      std::cout << "[fm"<< fmotor_ext_config.mot_num <<"] Initialized PWM Driver..." << '\n';
    }

    // Func for driving motor
    void fmotor_drive() {
      gpio_set_level(static_cast<gpio_num_t>(fmotor_ext_config.dir_pin), fmotor_liv_config.fmotor_dirn);
      mcpwm_set_duty(fmotor_int_config.mcpwm_unit, MCPWM_TIMER_0, MCPWM_OPR_A, fmotor_liv_config.fmotor_duty);
      // mcpwm_set_duty_type(fmotor_int_config.mcpwm_unit, MCPWM_TIMER_0, fmotor_int_config.mcpwm_opr, MCPWM_DUTY_MODE_0); //call this each time, if operator was previously in low/high state
    }

    // Func for computing PD Controller
    void fmotor_compute_frame() {
      // Get current position into the address of fmotor_liv_config.fmotor_pos
      pcnt_get_counter_value(fmotor_int_config.pcnt_unit, &fmotor_liv_config.fmotor_pos);
      fmotor_liv_config.fmotor_target = fmotor_target_table[time_counter];
      // Compute the delta - delta in
      fmotor_liv_config.fmotor_err = fmotor_liv_config.fmotor_target - (int)fmotor_liv_config.fmotor_pos;
      fmotor_liv_config.fmotor_derr = (int)fmotor_liv_config.fmotor_pos_old - (int)fmotor_liv_config.fmotor_pos - fmotor_liv_config.fmotor_target_old + fmotor_liv_config.fmotor_target;

      // Computing the duty cycle and dirn.
      // duty lies b/w 1 and 99 float
      fmotor_liv_config.fmotor_duty = 0.05*fmotor_ext_config.fmotor_prop_coeff*fmotor_liv_config.fmotor_err + 0.0005*fmotor_ext_config.fmotor_diff_coeff*fmotor_liv_config.fmotor_derr;
      // Kp*err + Kd*derr
      // Revisit

      // Assigning the direction and reguting the pwm value
      if (fmotor_liv_config.fmotor_duty>=0) {
        fmotor_liv_config.fmotor_dirn = 1;
      } else {
        fmotor_liv_config.fmotor_dirn = 0;
        fmotor_liv_config.fmotor_duty = (-1)*fmotor_liv_config.fmotor_duty;
      }

      // Capping the output
      if (fmotor_liv_config.fmotor_duty<=1.0) {
        fmotor_liv_config.fmotor_duty = 1.0;
      }
      if (fmotor_liv_config.fmotor_duty>=99.0) {
        fmotor_liv_config.fmotor_duty = 99.0;
      }

      if (time_counter >= FMOTOR_TARGET_SIZE-1) {
        fmotor_liv_config.fmotor_duty = 0.0;
      }

      if (time_counter == FMOTOR_TARGET_SIZE-1) {
        std::cout << "Task Complete!" << '\n';
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

      time_counter++;

    }

    // Time Engine
    static void fmotor_timer_launch(void) {

      esp_timer_create_args_t fmotor_timer_args;

      fmotor_timer_args.callback = &fmotor_timer_callback;
      fmotor_timer_args.name = "fmotor_timer";

      esp_timer_handle_t fmotor_timer;
      // Creates a timer - not started yet!
      esp_timer_create(&fmotor_timer_args, &fmotor_timer);
      // Starts the timer
      esp_timer_start_periodic(fmotor_timer, 1000*FMOTOR_DT); //time in 'us'

    }
};

fmotor_ext_config_t fmot_config_0 ={
  .mot_num = 0,
  .dir_pin = FMOTOR_0_DIR,
  .pwm_pin = FMOTOR_0_PWM,
  .pul_pin = FMOTOR_0_PUL,
  .ctr_pin = FMOTOR_0_CTR,
  .fmotor_rot_enc_pulses = FMOTOR_0_ANG_CONV_FACT,
  .fmotor_prop_coeff = FMOTOR_0_PROP_COEFF,
  .fmotor_diff_coeff = FMOTOR_0_DIFF_COEFF
};

#if NUM_OF_FMOTORS>1
fmotor_ext_config_t fmot_config_1 ={
  .mot_num = 1,
  .dir_pin = FMOTOR_1_DIR,
  .pwm_pin = FMOTOR_1_PWM,
  .pul_pin = FMOTOR_1_PUL,
  .ctr_pin = FMOTOR_1_CTR,
  .fmotor_rot_enc_pulses = FMOTOR_1_ANG_CONV_FACT,
  .fmotor_prop_coeff = FMOTOR_1_PROP_COEFF,
  .fmotor_diff_coeff = FMOTOR_1_DIFF_COEFF
};
#endif

#if NUM_OF_FMOTORS>2
fmotor_ext_config_t fmot_config_2 ={
  .mot_num = 2,
  .dir_pin = FMOTOR_2_DIR,
  .pwm_pin = FMOTOR_2_PWM,
  .pul_pin = FMOTOR_2_PUL,
  .ctr_pin = FMOTOR_2_CTR,
  .fmotor_rot_enc_pulses = FMOTOR_2_ANG_CONV_FACT,
  .fmotor_prop_coeff = FMOTOR_2_PROP_COEFF,
  .fmotor_diff_coeff = FMOTOR_2_DIFF_COEFF
};
#endif

#if NUM_OF_FMOTORS>3
fmotor_ext_config_t fmot_config_3 ={
  .mot_num = 3,
  .dir_pin = FMOTOR_3_DIR,
  .pwm_pin = FMOTOR_3_PWM,
  .pul_pin = FMOTOR_3_PUL,
  .ctr_pin = FMOTOR_3_CTR,
  .fmotor_rot_enc_pulses = FMOTOR_3_ANG_CONV_FACT,
  .fmotor_prop_coeff = FMOTOR_3_PROP_COEFF,
  .fmotor_diff_coeff = FMOTOR_3_DIFF_COEFF
};
#endif

int fmotor::fmotor_nums = NUM_OF_FMOTORS;
int fmotor::time_counter = 0;
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
