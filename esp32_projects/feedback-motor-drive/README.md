Feedback Motor Drive `fmotor` Library
====================

This library essentially gives you an `fmotor` object that has a built in PD controller that runs on a periodic timer ~~that's declared externally by the user~~. 

You can at max initialize 8 independent `fmotor`s simultaneously and implement PD control over them in the defined time-frame (frequency). 

!Documentation Pending!

---

Progress (as on **6th Jan, 2019**)

- [x] System Initialization
- [x] Motor Drive Function
  - [ ] Constant Drive ***Testing***
- [ ] Global Calibration Access Setting
  - [ ] Auto-Calibration + Drive ***Testing***
- [ ] PD Controller
- [ ] Periodic Timer Engine
  - [ ] Trajectory Drive ***Testing***
- [ ] Functional PWM range map

---





