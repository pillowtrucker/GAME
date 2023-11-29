#include <TheracSimulatorAdapter.hpp>
namespace thsAdapter {
void TheracSimulatorAdapter::hs_init(String args) {
    auto _argv = args.split(' ');
        
    int argc = _argv.size();
    char *** argv = new char ** [argc];
    int i = 0;
    char** bargv = new char*[argc+1];
    argv[argc] = nullptr;
    
    for(auto hng: _argv) {
        for (i = 0; i < argc; i++) { bargv[i] = new char[hng.length()+1]; strcpy(bargv[i], hng.toUTF8().c_str()); }
        /*
        argv[i] = (char **) malloc(sizeof(char *));
        *argv[i] = (char *) malloc(sizeof(char)* hng.length()+1);
        strcpy(*argv[i],hng.toUTF8().c_str());
        *((*argv[i])+hng.length()) = '\0';
        std::cout << *argv[i];
        ++i;
        */
    }
    ::hs_init(&argc, &bargv);
    wrapped_comms = ::startMachine();
}
TheracSimulatorAdapter::TheracSimulatorAdapter() { hs_init(); }
void TheracSimulatorAdapter::hs_exit() { ::hs_exit(); }
TheracSimulatorAdapter::~TheracSimulatorAdapter() {
    hs_exit();
    wrapped_comms = nullptr;
    delete this;
}
void TheracSimulatorAdapter::externalCallWrap(ExtCallType ext_call_type,
                      BeamType beam_type, CollimatorPosition collimator_position,
                      HsInt beam_energy) {
    ::externalCallWrap(wrapped_comms, ext_call_type, beam_type, collimator_position, beam_energy);
}
String TheracSimulatorAdapter::requestStateInfo(StateInfoRequest state_info_request) {
    return Unicode::FromUTF8(static_cast<char const *>(::requestStateInfo(wrapped_comms, state_info_request)));
}
}
