// COMPONENT FLAGS
#define F_FGPIO_FLAG 1
#define F_FMOTOR_FLAG 1
#define F_FTEL_FLAG 1

// FGPIO CONFIG
#if F_FGPIO_FLAG == 1

#endif

// FMOTOR CONFIG
#if F_FMOTOR_FLAG == 1

  #define F_NUM_OF_FMOTORS 2

  #define FMOTOR_0_DIR 18
  #define FMOTOR_0_PWM 19
  #define FMOTOR_0_PUL 15
  #define FMOTOR_0_CTR 4
  #define FMOTOR_0_ANG_CONV_FACT 2500 // CPR
  #define FMOTOR_0_PROP_COEFF 10
  #define FMOTOR_0_DIFF_COEFF 1

  #if F_NUM_OF_FMOTORS > 1
    #define FMOTOR_1_DIR 13
    #define FMOTOR_1_PWM 12
    #define FMOTOR_1_PUL 14
    #define FMOTOR_1_CTR 27
    #define FMOTOR_1_ANG_CONV_FACT 2500 // CPR
    #define FMOTOR_1_PROP_COEFF 10
    #define FMOTOR_1_DIFF_COEFF 1
  #endif

  #if F_NUM_OF_FMOTORS > 2
    #define FMOTOR_2_DIR 15
    #define FMOTOR_2_PWM 2
    #define FMOTOR_2_PUL 4
    #define FMOTOR_2_CTR 16
    #define FMOTOR_2_ANG_CONV_FACT 2500 // CPR
    #define FMOTOR_2_PROP_COEFF 10
    #define FMOTOR_2_DIFF_COEFF 1
  #endif

  #if F_NUM_OF_FMOTORS > 3
    #define FMOTOR_3_DIR 17
    #define FMOTOR_3_PWM 5
    #define FMOTOR_3_PUL 18
    #define FMOTOR_3_CTR 19
    #define FMOTOR_3_ANG_CONV_FACT 2500 // CPR
    #define FMOTOR_3_PROP_COEFF 10
    #define FMOTOR_3_DIFF_COEFF 1
  #endif
#else
  #define F_NUM_OF_FMOTORS 0
#endif

// FTEL CONFIG
#if F_FTEL_FLAG == 1
  #define FTEL_TX_ENABLE 1    // FROM ESP
  #define FTEL_RX_ENABLE 0    // TO ESP
  #define FTEL_ON_WIFI 1
  #define FTEL_ON_ETHERNET 0
#endif

// GENERAL SETTINGS
#define F_MAX_NUM_OF_PROCESSES 10 // maximum number of processes
#define F_DELTA_T 10              // ms
