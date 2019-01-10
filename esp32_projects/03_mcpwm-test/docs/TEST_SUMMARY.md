TEST SUMMARY
====================

## MCPWM Test using brushed DC motor on MD10C(Cytron) 

**Date : 10^th^ Jan, 2019**

---

This is a summary of learnings I've made while testing the encoder on `MCPWM` module

* Type-Casting `int` to `gpio_num_t`:

  The functions in the GPIO library used for this example only accept GPIO pins as `gpio_num_t` enumerated values. Therefore, we need to type-cast any `int` into  `gpio_num_t` by doing the follows:

  - [x] Tested!

  ```c++
  #define DIR_PIN 2
  // The above statement declares DIR_PIN as int 2
  
  gpio_num_t DIR_PIN_2 = static_cast<gpio_num_t>(DIR_PIN);
  // Or you can use just the right side while calling a function. 
  ```

* There's whining sounds from the motor, which is mostly due to the PWM frequency which has been currently set to 25kHz. It needs to be increased further.

