/// \file Scaler.cxx
/// \brief Implements Scaler.hxx
#include "scaler/Scaler.hxx"
#include "utils/Incrvoid.hxx"
#include <iostream>

// ========== Class Scaler ========== //

Scaler::Scaler() {
  
  
  
  for (int i=0;i<=16;i++) {
    ch_int[i]=0;
    ch_double[i]=0;
    count=0;
  }
}

Scaler::Scaler(const Scaler& other) {
  for (int i=0;i<=16;i++) {
    ch_int[i] = other.ch_int[i];
    ch_double[i] = other.ch_double[i];
  }
  count = other.count;
}

Scaler::~Scaler() {}


Scaler& Scaler::operator= (const Scaler& other) {
    for (int i=0;i<=16;i++) {
      ch_int[i] = other.ch_int[i];
      ch_double[i] = other.ch_double[i];
    }
    return *this;
}

void Scaler::reset() {
  count=0;
}

void Scaler::unpack(const TMidasEvent& event) {

  int length;
  int type;
  void* addr;

  ++count;

  if(event.FindBank("SCLD",&length,&type,&addr)) {
    
    for (int i=0;i<length;i++) {
      uint32_t* data = reinterpret_cast<uint32_t*>(addr);
      ch_int[i]= *data;
      increment_void(addr,type);
    }
  }

  if(event.FindBank("SCLR",&length,&type,&addr)) {
    for (int i=0;i<length;i++) {
      double* data = reinterpret_cast<double*>(addr);
      ch_double[i]= *data;
      increment_void(addr,type);
    }
  }

    
  Tail_triggers_presented = ch_double[variables.Tail_triggers_presented_ch];
  Tail_triggers_aquired = ch_double[variables.Tail_triggers_aquired_ch];
  SB0_triggers = ch_double[variables.SB0_triggers_ch];
  SB1_triggers = ch_double[variables.SB1_triggers_ch];
  DSSSD_triggers = ch_double[variables.DSSSD_triggers_ch];
  IC0_triggers = ch_double[variables.IC0_triggers_ch];
  MCP0_triggers = ch_double[variables.MCP0_triggers_ch];
  MCP1_triggers = ch_double[variables.MCP1_triggers_ch];
  MCPTOF_triggers = ch_double[variables.MCPTOF_triggers_ch];
  NaI0_triggers = ch_double[variables.NaI0_triggers_ch];
  NaI1_triggers = ch_double[variables.NaI1_triggers_ch];
  Ge0_triggers = ch_double[variables.Ge0_triggers_ch];
  


}

// ========== Class Scaler::Variables ========== //

Scaler::Variables::Variables() {
  Tail_triggers_presented_ch=0;
  Tail_triggers_aquired_ch=1;
  SB0_triggers_ch=2;
  SB1_triggers_ch=3;
  DSSSD_triggers_ch=4;
  IC0_triggers_ch=5;
  MCP0_triggers_ch=6;
  MCP1_triggers_ch=7;
  MCPTOF_triggers_ch=8;
  NaI0_triggers_ch=9;
  NaI1_triggers_ch=10;
  Ge0_triggers_ch=11;
}
