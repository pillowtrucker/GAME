#include "Plugins/TheracSimulatorAdapter.hpp"
namespace GAME::Plugins::TheracSimulatorAdapter {
void TheracSimulatorAdapter::hs_init(String args) {
  auto _argv = args.split(' ');

  int argc = _argv.size();
  int i = 0;
  char **argv = new char *[argc + 1];
  argv[argc] = nullptr;

  for (auto hng : _argv) {
    // chz figured out wtf char *** argv is supposed to be for me in this
    // context
    for (i = 0; i < argc; i++) {
      argv[i] = new char[hng.length() + 1];
      strcpy(argv[i], hng.toUTF8().c_str());
    }
  }
  ::hs_init(&argc, &argv);
  wrapped_comms = ::startMachine();
}
TheracSimulatorAdapter::TheracSimulatorAdapter() { hs_init();}
void TheracSimulatorAdapter::hs_exit() { ::hs_exit(); }
TheracSimulatorAdapter::~TheracSimulatorAdapter() {
  hs_exit();
  wrapped_comms = nullptr;
  delete this;
}
void TheracSimulatorAdapter::externalCallWrap(
    ExtCallType ext_call_type, BeamType beam_type,
    CollimatorPosition collimator_position, HsInt beam_energy) {
  ::externalCallWrap(wrapped_comms, ext_call_type, beam_type,
                     collimator_position, beam_energy);
}
String
TheracSimulatorAdapter::requestStateInfo(StateInfoRequest state_info_request) {
  return Unicode::FromUTF8(static_cast<char const *>(
      ::requestStateInfo(wrapped_comms, state_info_request)));
}
bool TheracSimulatorAdapter::check_malfunction() {
    std::shared_lock<std::shared_mutex> lock{malfunctioning_mutex};
    return malfunctioning;
}
bool TheracSimulatorAdapter::set_malfunction() {
    std::unique_lock<std::shared_mutex> lock{malfunctioning_mutex};
    return malfunctioning = true;
}
bool TheracSimulatorAdapter::reset_malfunction() {
    std::unique_lock<std::shared_mutex> lock{malfunctioning_mutex};
    return not(malfunctioning = false);
    
}

} // namespace thsAdapter
