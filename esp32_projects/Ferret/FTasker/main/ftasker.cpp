#pragma once

#include "ftasker.hpp"

int FTasker::f_process_count = 0;

FTasker::FTasker(){
  f_process_count = 0;
  FTasker::read_config();
}

static void FTasker::read_config(void){
  if(F_FTEL_FLAG){
    if (FTEL_TX_ENABLE) {f_process_list[f_process_count] = "FERRET > FTEL > TX"; f_process_count++;}
    if (FTEL_RX_ENABLE) {f_process_list[f_process_count] = "FERRET > FTEL > RX"; f_process_count++;}
  }
  if(F_FMOTOR_FLAG){
    f_process_list[f_process_count] = "FERRET > FMOTOR > M0"; f_process_count++;
    if (F_NUM_OF_FMOTORS>=1) {f_process_list[f_process_count] = "FERRET > FMOTOR > M1"; f_process_count++;}
    if (FTEL_RX_ENABLE) {f_process_list[f_process_count] = "FERRET > FMOTOR > M2"; f_process_count++;}
  }
}
