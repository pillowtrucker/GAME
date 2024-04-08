#include "Minigames/TheracSimulator/TheracConsole.hpp"
#include "OpenSiv3D/Siv3D/src/ThirdParty/nlohmann/json.hpp"
#include "OpenSiv3D/Siv3D/include/Siv3D.hpp"
#include <unistd.h>
#include "OpenSiv3D/Siv3D/src/Siv3D/Common/Siv3DEngine.hpp"
#include <OpenSiv3D/Siv3D/src/Siv3D/System/ISystem.hpp>
#include "Minigames/TheracSimulator/TextBox.hpp"
namespace GAME::Minigames::TheracSimulator::Console {
auto const BEAMTYPE_FIELD = U"PLACEHOLDER_BT";
auto const UNIT_RATE_P_FIELD = U"PLACEHOLDER_URMP";
auto const TIME_PRESCRIBED_P_FIELD = U"PLACEHOLDER_TMP";
auto const GANTRY_ROTATION_P_FIELD = U"PLACEHOLDER_GRP";
auto const ACCESSORY_NUMBER_P_FIELD = U"PLACEHOLDER_ANP";
auto const COMMAND_ENTRY_FIELD = U"PLACEHOLDER_CMD";
auto const BEAM_ENERGY_FIELD = U"PLACEHOLDER_EN";
auto const PATIENT_NAME_FIELD = U"PLACEHOLDER_PN";
TheracConsoleWidget::TheracConsoleWidget(
    String _name, Point _p_in_grid, SimpleTable &_grid, TextEditState _tes,
    HashTable<String, TheracConsoleWidgetType> &types,
    std::shared_ptr<thsAdapter::TheracSimulatorAdapter> _tsa,
    std::shared_ptr<std::mutex> _sdm,
    phmap::parallel_node_hash_set<std::unique_ptr<std::function<void()>>>& _overrides,
    Font & _fat_font)
    : p_in_grid{_p_in_grid}, tes{_tes}, grid{_grid}, name{_name}, tsa{_tsa},screen_drawing_mutex{_sdm}, overrides{_overrides}, fat_font{_fat_font} {
  text_field_type = types[name];
}

// finish doing setup after all the other widgets are set up
void TheracConsoleWidget::finish_setup() {
  String maybe_next;
  switch (text_field_type) {

  case BeamMode:
    break;
  case BeamEnergyOut:
    max_chars = 5;
    break;
  case BeamEnergy:
    max_chars = 5;
    enabled = true;
    next_field = dynamic_widgets.value()[UNIT_RATE_P_FIELD];
    next_field->prev_field = this;
    break;
  case Verifier:
    my_data = new TheracConsoleVerifier{
        .source_input =
            *dynamic_widgets.value()
                 [grid.getItem(Point(p_in_grid.x - 2, p_in_grid.y)).text],
        .dest_input =
            *dynamic_widgets.value()
                 [grid.getItem(Point(p_in_grid.x - 1, p_in_grid.y)).text]};
    break;
  case Date:
    tes.text = Date::Today().format(U"1985-MM-dd");
  case Time:
    break;
  case OID:
    tes.text = System::UserName().append(U"{:d}"_fmt(RandomInt16()));
    break;
  case TreatPhase:
  case Reason:
  case Subsys:
  case Const:
    break;
  case FloatSrc:
    max_chars = 10;
    tes.text = U"{:.7f}"_fmt(RandomClosed(0.0, 360.0));
    floatify();
    break;
  case FloatDest:
    enabled = true;
    max_chars = 10;
    floatify();
    maybe_next = grid.getItem(Point(p_in_grid.x, p_in_grid.y + 1)).text;
    next_field = dynamic_widgets.value()[maybe_next];
    if (name == TIME_PRESCRIBED_P_FIELD)
      next_field = dynamic_widgets.value()[GANTRY_ROTATION_P_FIELD];
    if (name == ACCESSORY_NUMBER_P_FIELD)
      next_field = dynamic_widgets.value()[COMMAND_ENTRY_FIELD];
    next_field->prev_field = this;
    my_data = new TheracConsoleFloatDest{
        .source_input =
            *dynamic_widgets.value()
                 [grid.getItem(Point(p_in_grid.x - 1, p_in_grid.y)).text]};
    break;
  case Normal:
    enabled = true;
    if (name == PATIENT_NAME_FIELD) {
      next_field = dynamic_widgets.value()[BEAMTYPE_FIELD];
      next_field->prev_field = this;
    }
    break;
  case BeamModeInput:
    max_chars = 1;
    enabled = true;
    next_field = dynamic_widgets.value()[BEAM_ENERGY_FIELD];
    next_field->prev_field = this;
    break;
  case CmdEntry:
    max_chars = 1;
    enabled = true;
    break;
  }
}

void TheracConsoleWidget::verify_floats() {
  TheracConsoleVerifier &d = *std::get<TheracConsoleVerifier *>(my_data);
  if(d.source_input.timed_state_mutex.try_lock_for(Milliseconds{16})) {
      if(d.dest_input.timed_state_mutex.try_lock_for(Milliseconds{16})) {
          if (d.source_input.tes.text == d.dest_input.tes.text)
              tes.text = U"VERIFIED";
          else
              tes.text = U"";
          d.dest_input.timed_state_mutex.unlock();
      }
      d.source_input.timed_state_mutex.unlock();
  }
}
  


void TheracConsoleWidget::floatify() {
  double x = ParseOpt<double>(tes.text).value_or(0.0);
  tes.text = U"{:.7f}"_fmt(x);
}
void TheracConsoleWidget::enforce_int() {
  uint32_t x = ParseOpt<uint32_t>(tes.text).value_or(0);
  tes.text = U"{}"_fmt(x);
}

void TheracConsoleWidget::mangle() {
    std::unique_lock<std::shared_timed_mutex> sm{timed_state_mutex};
    std::unique_lock<std::shared_mutex> jm{jump_mutex,std::defer_lock};
    std::unique_lock<std::shared_mutex> am{autofill_mutex,std::defer_lock};
    std::unique_lock<std::shared_mutex> jmn{next_field->jump_mutex,std::defer_lock};
    std::unique_lock<std::shared_mutex> amn{next_field->autofill_mutex,std::defer_lock};
    std::unique_lock<std::shared_mutex> jmp{prev_field->jump_mutex,std::defer_lock};
    std::unique_lock<std::shared_mutex> amp{prev_field->autofill_mutex,std::defer_lock};

    std::unique_lock<std::mutex> sdm{*screen_drawing_mutex,std::defer_lock};  

  switch (text_field_type) {
  case FloatSrc:
    break;
  case FloatDest:

        if (enabled && tes.active) {
            floatify();
            if ((keys_up[KeyTab.asUint32()] || keys_up[KeyEnter.asUint32()] || keys_up[KeyDown.asUint32()]) &&
                U"{:.7f}"_fmt(0.0) == tes.text && am.try_lock() && should_autofill){ 
                am.unlock();
                tes.text =
                    std::get<TheracConsoleFloatDest *>(my_data)->source_input.tes.text;
                
                if (next_field != nullptr && next_field->text_field_type == FloatDest) {
                    if(amn.try_lock()) {
                        next_field->should_autofill = false;
                        amn.unlock();
                    }
                }
                
            } else { if(am.owns_lock()) {am.unlock();};
                if(am.try_lock()) {
                    should_autofill = true;
                    am.unlock();
                }
            }
        }
    break;
  case BeamEnergy:
    enforce_int();
    break;
  case BeamEnergyOut:
    enforce_int();
    tes.text = tsa.get()->requestStateInfo(thsAdapter::RequestBeamEnergy);
    break;
  case BeamModeInput: // beam mode input
        tes.text.uppercase();
        if (tes.text == U"X") {
            if(next_field->timed_state_mutex.try_lock_for(Milliseconds{16})) {
                next_field->tes.text = U"25000";
                next_field->timed_state_mutex.unlock();
            }
        }
    break;
  case Verifier:
          verify_floats();
    break;
  case Date:
    break;
  case Time:
    tes.text = DateTime::Now().format(U"HH:mm:ss");
    break;
  case OID:
    break;
  case TreatPhase:
        tes.text = tsa.get()->requestStateInfo(thsAdapter::RequestTreatmentState);
    break;
  case BeamMode:
        tes.text = tsa.get()->requestStateInfo(thsAdapter::RequestBeamMode);
    break;
  case CmdEntry:
        tes.text.uppercase();
        if(!tes.active) {
            break;
        }
        if(tes.text == U"T"){

            sdm.lock();
            overrides.clear();
            sdm.unlock();
            tes.text.clear();
            if(verifyInputComplete()) {
                tsa.get()->externalCallWrap(thsAdapter::ExtCallSendMEOS,translateBeamType() , translateColPos(), getBeamEnergy());
                tsa.get()->externalCallWrap(thsAdapter::ExtCallToggleEditingTakingPlace);
                tsa.get()->externalCallWrap(thsAdapter::ExtCallToggleDatentComplete);
            }
            else {
                MALFUNCTION();
            }
        } else if(tes.text == U"P") {
            
            sdm.lock();
            overrides.clear();
            sdm.unlock();
            tsa.get()->externalCallWrap(thsAdapter::ExtCallProceed);
            tsa->reset_malfunction();
            tes.text.clear();
        } else if(tes.text == U"R") {
            
            sdm.lock();
            overrides.clear();
            sdm.unlock();
            tsa->externalCallWrap(thsAdapter::ExtCallReset);
            tsa->reset_malfunction();
            tes.text.clear();
        }
     
    break;
  case Const:
    break;
  case Reason:
          tes.text = tsa.get()->requestStateInfo(thsAdapter::RequestReason);
          if(tes.text.starts_with(U"M")) {
              MALFUNCTION(ParseOpt<int>(tes.text.split(' ')[1]).value_or(777));
          }
    break;
  case Normal:
        tes.text.uppercase();
    break;
  case Subsys:
        tes.text = tsa.get()->requestStateInfo(thsAdapter::RequestActiveSubsystem);
    break;
  }
      if (enabled && tes.active) {
      if (next_field != nullptr && (keys_up[KeyTab.asUint32()] || keys_up[KeyEnter.asUint32()] || keys_up[KeyDown.asUint32()]) && jm.try_lock() && should_jump){
          jm.unlock();
          if(jmn.try_lock() && next_field->timed_state_mutex.try_lock_shared_for(Milliseconds{16})) {
                      
                      next_field->keys_up[KeyTab.asUint32()] = false;
                      next_field->keys_up[KeyEnter.asUint32()] = false;
                      next_field->keys_up[KeyDown.asUint32()] = false;
                      next_field->tes.active = true;
                      next_field->should_jump = false;
                      tes.active = false;
                      jmn.unlock();
                      next_field->timed_state_mutex.unlock_shared();
                  }
      } else { if(jm.owns_lock()) {jm.unlock();};
          if (prev_field != nullptr && keys_up[KeyUp.asUint32()] && jm.try_lock() && should_jump){ 
          jm.unlock();
              if(text_field_type == CmdEntry){
                  //if(tes.text == U"T") {
                      tsa.get()->externalCallWrap(thsAdapter::ExtCallToggleEditingTakingPlace);
                      tsa.get()->externalCallWrap(thsAdapter::ExtCallToggleDatentComplete);
                      //}
                  tes.text.clear();
              } 

              if(jmp.try_lock() && prev_field->timed_state_mutex.try_lock_shared_for(Milliseconds{16})) {
                      tes.active = false;
                      prev_field->keys_up[KeyUp.asUint32()] = false;
                      prev_field->should_jump = false;
                      prev_field->tes.active = true;
                      prev_field->timed_state_mutex.unlock_shared();
                      jmp.unlock();
                  }

          } else {
          if(jm.try_lock())
              should_jump = true;
          jm.unlock();
          } 
      }
      
  }
}
thsAdapter::BeamType TheracConsoleWidget::translateBeamType() {
    auto btf = dynamic_widgets.value()[BEAMTYPE_FIELD];
    std::unique_lock<std::shared_timed_mutex> btfsm{btf->timed_state_mutex,std::defer_lock};
    if(btfsm.try_lock_for(Milliseconds{16})) {
        if(btf->tes.text == U"X") {
            return thsAdapter::BeamTypeXRay;
        } else if(btf->tes.text == U"E") {
            return thsAdapter::BeamTypeElectron;
        } else {
            return thsAdapter::BeamTypeUndefined; // this shouldn't happen, verification thingie should check that first
        }
    }
}
thsAdapter::CollimatorPosition TheracConsoleWidget::translateColPos() {
    auto bt = translateBeamType();
    switch(bt) {
    case thsAdapter::_CheekyPadding:
    case thsAdapter::BeamTypeUndefined:
        return thsAdapter::CollimatorPositionUndefined;
        break;
    case thsAdapter::BeamTypeXRay:
        return thsAdapter::CollimatorPositionXRay;
        break;
    case thsAdapter::BeamTypeElectron:
        return thsAdapter::CollimatorPositionElectronBeam;
        break;
    }
}
int TheracConsoleWidget::getBeamEnergy() {
    auto ben = dynamic_widgets.value()[BEAM_ENERGY_FIELD];
    std::unique_lock<std::shared_timed_mutex> bensm{ben->timed_state_mutex,std::defer_lock};
    if(bensm.try_lock_for(Milliseconds{500}))
        return ParseOpt<int>(ben->tes.text).value_or(25000); // good default number
    else
        return 25000;
}

bool TheracConsoleWidget::verifyInputComplete() {
    for (auto [l,w]: dynamic_widgets.value()){
        std::unique_lock<std::shared_timed_mutex> wsm{w->timed_state_mutex,std::defer_lock};
        if(wsm.try_lock_for(Milliseconds{16})) {
            auto t = w->text_field_type;
            auto te = w->tes.text;
            if(t == Verifier)
                {
                    if(!(te == U"VERIFIED"))
                        return false;
                }
            else if(t == Normal) {
                if(te == U"")
                    return false;
            } else if(t == BeamEnergy) {
                if(te == U"0")
                    return false;
            } else if (t == BeamModeInput) {
                if(!(te == U"X" || te == U"E"))
                    return false;
            }
            
        } else if(w->name == name)
            continue;
    }
    return true;
}

void TheracConsoleWidget::MALFUNCTION(int num) {
    if (tsa->check_malfunction())
        return;
    if (not tsa->set_malfunction())
        return;
    auto window_size = System::GetCurrentMonitor().fullscreenResolution;

    auto malfunction_override = std::make_unique<std::function<void()>>([&,window_size,num] () -> void {
        TextEditState _tes;

        _tes.text = U"MALFUNCTION {:d}"_fmt(num);
        HashTable<String, TheracConsoleWidgetType> dummy_types{{_tes.text,Const}};
        TheracConsoleWidget dummy_w(_tes.text,Point{},grid,_tes,dummy_types,tsa,screen_drawing_mutex,overrides,fat_font);
        TextBoxAt(dummy_w,
            _tes, Vec2{window_size.x/2, window_size.y/2},
            window_size.x/3, 80, false, fat_font, Palette::Red,
            window_size.y/4);
        
    });
    std::unique_lock<std::mutex> sdm{*screen_drawing_mutex,std::defer_lock};
    sdm.lock();
    overrides.insert(std::move(malfunction_override));
    sdm.unlock();
}
TheracConsole::TheracConsole(FilePath p) {
  widget_config_filepath = p;
  load_ui_widgets();
}
TheracConsole::TheracConsole() { load_ui_widgets(); }
void TheracConsole::save_ui_widgets() {
  auto ui_widgets_ = ui_widgets.parallel_map(
      [](Array<std::pair<String, TheracConsoleWidgetType>> v)
          -> Array<std::pair<std::string, TheracConsoleWidgetType>> {
        return v.map([](std::pair<String, TheracConsoleWidgetType> v_) {
          return std::pair<std::string, TheracConsoleWidgetType>{
              std::get<0>(v_).toUTF8(), std::get<1>(v_)};
        });
      });
  nlohmann::json ok4{ui_widgets_};
  TextWriter w_ui_widgets{widget_config_filepath};
  w_ui_widgets.writeUTF8(ok4.dump(4));
  w_ui_widgets.close();
}
void TheracConsole::load_ui_widgets() {

  TextReader r_therac_ui{widget_config_filepath};
  Array<Array<std::pair<std::string, TheracConsoleWidgetType>>> widgets_u8 =
      nlohmann::json::parse(r_therac_ui.readAll().toUTF8())[0];
  ui_widgets = widgets_u8.parallel_map(
      [](auto v) -> Array<std::pair<String, TheracConsoleWidgetType>> {
        return v.map([](auto v_) {
          return std::pair<String, TheracConsoleWidgetType>{
              Unicode::FromUTF8(std::get<0>(v_)), std::get<1>(v_)};
        });
      });
  r_therac_ui.close();
}

} // namespace GAME::Minigame::TheracSimulator::Console

