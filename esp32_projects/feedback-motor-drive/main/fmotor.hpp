// Includes //

int fmotors_num = 0;

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
  int fmotor_del;
  int fmotor_dt;
  // Drive Variables
  float fmotor_duty;
  bool fmotor_dirn;
};

class fmotor {

  private:
    // Using externally defined structs as configuration
    fmotor_ext_config_t fmotor_ext_config;
    fmotor_int_config_t fmotor_int_config;
    fmotor_liv_config_t fmotor_liv_config;

  public:

    // Default Constructor for mandating initializing parameters
    fmotor(fmotor_ext_config_t fmotor_config_init){
      fmotor_ext_config = fmotor_config_init;
      // DEBUG
      std::cout << "[o] ["<< fmotor_ext_config.mot_num <<"] FMotor Configuration Imported..." << '\n';

      // Initializing Drivers
      fmotor_pcnt_init();
      fmotor_mcpwm_init();

      // Initializing live configuration
      fmotor_liv_config.fmotor_pos = 0;
      fmotor_liv_config.fmotor_del = 0;
      fmotor_liv_config.fmotor_dt = 10;
      fmotor_liv_config.fmotor_duty = 0.0;
      fmotor_liv_config.fmotor_dirn = 0;

    }

    // Selects PCNT unit from motor ID
    static void fmotor_pcnt_select(void) {
      switch (fmotor_ext_config.mot_num) {
        case 0: fmotor_int_config.pcnt_unit = PCNT_UNIT_0; break;
        case 1: fmotor_int_config.pcnt_unit = PCNT_UNIT_1; break;
        case 2: fmotor_int_config.pcnt_unit = PCNT_UNIT_2; break;
        case 3: fmotor_int_config.pcnt_unit = PCNT_UNIT_3; break;
        case 4: fmotor_int_config.pcnt_unit = PCNT_UNIT_4; break;
        case 5: fmotor_int_config.pcnt_unit = PCNT_UNIT_5; break;
        case 6: fmotor_int_config.pcnt_unit = PCNT_UNIT_6; break;
        case 7: fmotor_int_config.pcnt_unit = PCNT_UNIT_7; break;
      }
      // DEBUG
      std::cout << "[o] ["<< fmotor_ext_config.mot_num <<"] PCNT Unit Configured..." << '\n';
    }

    // Function for initializing pcnt driver
    static void fmotor_pcnt_init(void){
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
      std::cout << "[o] ["<< fmotor_ext_config.mot_num <<"] Initialized PCNT Driver..." << '\n'
    }

    // Selects MCPWM unit from motor ID
    static void fmotor_mcpwm_select(void) {
      switch (fmotor_ext_config.mot_num) {
        case 0: fmotor_int_config.mcpwm_unit = MCPWM_UNIT_0; fmotor_int_config.mcpwm_channel = MCPWM0A; fmotor_int_config.mcpwm_opr = MCPWM_OPR_A; break;
        case 1: fmotor_int_config.mcpwm_unit = MCPWM_UNIT_0; fmotor_int_config.mcpwm_channel = MCPWM0B; fmotor_int_config.mcpwm_opr = MCPWM_OPR_B; break;
        case 2: fmotor_int_config.mcpwm_unit = MCPWM_UNIT_0; fmotor_int_config.mcpwm_channel = MCPWM1A; fmotor_int_config.mcpwm_opr = MCPWM_OPR_A; break;
        case 3: fmotor_int_config.mcpwm_unit = MCPWM_UNIT_0; fmotor_int_config.mcpwm_channel = MCPWM1B; fmotor_int_config.mcpwm_opr = MCPWM_OPR_B; break;
        case 4: fmotor_int_config.mcpwm_unit = MCPWM_UNIT_1; fmotor_int_config.mcpwm_channel = MCPWM0A; fmotor_int_config.mcpwm_opr = MCPWM_OPR_A; break;
        case 5: fmotor_int_config.mcpwm_unit = MCPWM_UNIT_1; fmotor_int_config.mcpwm_channel = MCPWM0B; fmotor_int_config.mcpwm_opr = MCPWM_OPR_B; break;
        case 6: fmotor_int_config.mcpwm_unit = MCPWM_UNIT_1; fmotor_int_config.mcpwm_channel = MCPWM1A; fmotor_int_config.mcpwm_opr = MCPWM_OPR_A; break;
        case 7: fmotor_int_config.mcpwm_unit = MCPWM_UNIT_1; fmotor_int_config.mcpwm_channel = MCPWM1B; fmotor_int_config.mcpwm_opr = MCPWM_OPR_B; break;
      }
      // DEBUG
      std::cout << "[o] ["<< fmotor_ext_config.mot_num <<"] MCPWM Unit Configured..." << '\n';
    }

    // Func for initializing motor pwm driver
    static void fmotor_mcpwm_init(void) {
      fmotor_mcpwm_select();
      mcpwm_gpio_init(fmotor_int_config.mcpwm_unit, fmotor_int_config.mcpwm_channel, fmotor_ext_config.pwm_pin);
      gpio_pad_select_gpio(fmotor_ext_config.dir_pin);
      gpio_set_direction(fmotor_ext_config.dir_pin, GPIO_MODE_OUTPUT);
      // DEBUG
      std::cout << "[o] ["<< fmotor_ext_config.mot_num <<"] Initialized Motor Hardware..." << '\n';

      mcpwm_config_t pwm_config;
      pwm_config.frequency = 25000;    //frequency = 25000Hz,
      pwm_config.cmpr_a = 0;    //duty cycle of PWMxA = 0
      pwm_config.cmpr_b = 0;    //duty cycle of PWMxB = 0
      pwm_config.counter_mode = MCPWM_UP_COUNTER;
      pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
      mcpwm_init(fmotor_ext_config.mcpwm_unit, MCPWM_TIMER_0, &pwm_config);
      // DEBUG
      std::cout << "[o] ["<< fmotor_ext_config.mot_num <<"] Initialized PWM Driver..." << '\n';
    }

    // Func for driving motor
    static void fmotor_drive(void) {
      gpio_set_level(fmotor_ext_config.dir_pin, fmotor_liv_config.fmotor_dirn);
      mcpwm_set_duty(fmotor_int_config.mcpwm_unit, MCPWM_TIMER_0, fmotor_int_config.mcpwm_opr, fmotor_liv_config.fmotor_duty);
      // mcpwm_set_duty_type(fmotor_int_config.mcpwm_unit, MCPWM_TIMER_0, fmotor_int_config.mcpwm_opr, MCPWM_DUTY_MODE_0); //call this each time, if operator was previously in low/high state
    }
};
