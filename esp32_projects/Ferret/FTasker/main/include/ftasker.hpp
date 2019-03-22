#include <iostream>
#include "ferret_config.hpp"

class FTasker{
private:
public:
  FTasker(void);
  // Constructor

  static int f_process_count;
  // count of number of processes
  std::string f_process_list[F_MAX_NUM_OF_PROCESSES];
  // List of process strings

  static void read_config(void);
  // Reads the config file

  static void register_process(void);
  // Register a component and allots an ID to each device/process

  static void print_process_id(void);
  // prints the process list and corresponding ID

  static void start_process_id(int PID);

};
