//! \file Scaler.hxx
//! \defines classes relevant to unpacking data from Scaler modules.
#ifndef SCALER_HXX
#define SCALER_HXX
#include "midas/TMidasEvent.h"

class Scaler {
public:
// ==== Classes ==== //
  /// Scalar variables
  class Variables {
  public:
    /// \brief Maps Scalar channel to BGO detector
    int Tail_triggers_presented_ch;
    int Tail_triggers_aquired_ch;
    int SB0_triggers_ch;
    int SB1_triggers_ch;
    int DSSSD_triggers_ch;
    int IC0_triggers_ch;
    int MCP0_triggers_ch;
    int MCP1_triggers_ch;
    int MCPTOF_triggers_ch;
    int NaI0_triggers_ch;
    int NaI1_triggers_ch;
    int Ge0_triggers_ch;

    /// Constuctor, sets Tail_triggers_presented_ch to 0, Tail_triggers_aquired_ch to 1, etc.
    Variables();

    /// Destructor, nothing to do
    ~Variables() { }

    /// Copy constructor
    Variables(const Variables& other);

    /// Equivalency operator
    Variables& operator= (const Variables& other);

  };

// ==== Data ==== //

  uint32_t ch_int[33];             //#
  
  float ch_double[33];              //#
  
  uint32_t count;                  //#
  
  float Tail_triggers_presented;   //#
  float Tail_triggers_aquired;     //#
  float SB0_triggers;              //#
  float SB1_triggers;              //#
  float DSSSD_triggers;            //#
  float IC0_triggers;              //#
  float MCP0_triggers;             //#
  float MCP1_triggers;             //#
  float MCPTOF_triggers;           //#
  float NaI0_triggers;             //#
  float NaI1_triggers;             //#
  float Ge0_triggers;              //#
  


  /// Instance of Scalar::Variable for mapping channels
  Variables variables;             //!

  /// Initialize all scalars
  Scaler();
  
  /// Deconstruct
  ~Scaler();
  
  /// Copy constructor
  Scaler(const Scaler& other);
  
  /// Equivalency operator
  Scaler& operator= (const Scaler& other);
  
  /// Reset all scalars
  void reset();

  /// Unpack Midas event data into scalar data structiures
  void unpack(const TMidasEvent& event);
};


#endif
