#pragma once
#include <HsFFI.h>
#include <string>
#include "hstherac25/dist-newstyle/build/x86_64-linux/ghc-9.8.1/hstherac25-0.1.0.0/build/HsTherac25_stub.h"
namespace thsAdapter {

enum ExtCallType {
  CheekyPadding,
  ExtCallSendMEOS,
  ExtCallToggleDatentComplete,
  ExtCallToggleEditingTakingPlace,
  ExtCallReset,
  ExtCallProceed
};

enum BeamType {
  _CheekyPadding,
  BeamTypeXRay,
  BeamTypeElectron,
  BeamTypeUndefined
};
enum CollimatorPosition {
  __CheekyPadding,
  CollimatorPositionXRay,
  CollimatorPositionElectronBeam,
  CollimatorPositionUndefined
};

class TheracSimulatorAdapter {

  public:
    void externalCallWrap(HsStablePtr wrapped_comms, ExtCallType ext_call_type,
                          BeamType beam_type, CollimatorPosition collimator_position,
                          HsInt beam_energy);    
    HsStablePtr startMachine(void);
    HsPtr requestStateInfo(HsStablePtr a1, HsInt a2);
	void hs_init(std::string args = "-threaded -rtsopts=-N");
	void hs_exit();
    

private:
    void * wrapped_comms;
};
} // namespace thsAdapter
