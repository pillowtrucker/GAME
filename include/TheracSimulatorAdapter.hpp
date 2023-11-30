#pragma once
#include <HsFFI.h>
#include <string>
#include "hstherac25/dist-newstyle/build/x86_64-linux/ghc-9.8.1/hstherac25-0.1.0.0/build/HsTherac25_stub.h"
#include <Siv3D.hpp>
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
enum StateInfoRequest {
  ___CheekyPadding,
  RequestTreatmentOutcome,
  RequestActiveSubsystem,
  RequestTreatmentState,
  RequestReason,
  RequestBeamMode,
  RequestBeamEnergy
};

class TheracSimulatorAdapter {

  public:
    TheracSimulatorAdapter();
    ~TheracSimulatorAdapter();
    void externalCallWrap(ExtCallType ext_call_type,
                          BeamType beam_type, CollimatorPosition collimator_position,
                          HsInt beam_energy);
    String requestStateInfo(StateInfoRequest state_info_request);
    

private:
    void hs_init(String args = U"-threaded +RTS -N -RTS");
	void hs_exit();
    HsStablePtr startMachine(void);
    void * wrapped_comms;
};
} // namespace thsAdapter
