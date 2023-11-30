#include "TheracConfig.h"
#include "OpenSiv3D/Siv3D/src/ThirdParty/nlohmann/json.hpp"
#include <Siv3D.hpp>
namespace TheracConfig {

TheracConfigWidget::TheracConfigWidget(
    String _name, Point _p_in_grid, SimpleTable &_grid, TextEditState _tes,
    HashTable<String, TheracTextType> &types,
    std::shared_ptr<thsAdapter::TheracSimulatorAdapter> _tsa)
    : p_in_grid{_p_in_grid}, tes{_tes}, grid{_grid}, name{_name}, tsa{_tsa} {
  text_field_type = types[name];
}

// finish doing setup after all the other widgets are set up
void TheracConfigWidget::finish_setup() {
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
    next_field = dynamic_widgets.value()[U"PLACEHOLDER_URMP"];
    next_field->prev_field = this;
    break;
  case Verifier:
    my_data = new TheracConfigVerifier{
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
    break;
  case FloatDest:
    enabled = true;
    max_chars = 10;
    maybe_next = grid.getItem(Point(p_in_grid.x, p_in_grid.y + 1)).text;
    next_field = dynamic_widgets.value()[maybe_next];
    if (name == U"PLACEHOLDER_TMP")
      next_field = dynamic_widgets.value()[U"PLACEHOLDER_GRP"];
    if (name == U"PLACEHOLDER_ANP")
      next_field = dynamic_widgets.value()[U"PLACEHOLDER_CMD"];
    next_field->prev_field = this;
    my_data = new TheracConfigFloatDest{
        .source_input =
            *dynamic_widgets.value()
                 [grid.getItem(Point(p_in_grid.x - 1, p_in_grid.y)).text]};
    break;
  case Normal:
    enabled = true;
    if (name == U"PLACEHOLDER_PN") {
      next_field = dynamic_widgets.value()[U"PLACEHOLDER_BT"];
      next_field->prev_field = this;
    }
    break;
  case BeamModeInput:
    max_chars = 1;
    enabled = true;
    next_field = dynamic_widgets.value()[U"PLACEHOLDER_EN"];
    next_field->prev_field = this;
    break;
  case CmdEntry:
    max_chars = 1;
    enabled = true;
    break;
  }
}

void TheracConfigWidget::verify_floats() {
  TheracConfigVerifier &d = *std::get<TheracConfigVerifier *>(my_data);
  if (d.source_input.tes.text == d.dest_input.tes.text)
    tes.text = U"VERIFIED";
  else
    tes.text = U"";
}

void TheracConfigWidget::floatify() {
  double x = ParseOpt<double>(tes.text).value_or(0.0);
  tes.text = U"{:.7f}"_fmt(x);
}
void TheracConfigWidget::enforce_int() {
  uint32_t x = ParseOpt<uint32_t>(tes.text).value_or(0);
  tes.text = U"{}"_fmt(x);
}

void TheracConfigWidget::mangle() {

  switch (text_field_type) {
  case FloatSrc:
    floatify();
    break;
  case FloatDest:
    floatify();
    if (enabled && tes.active) {
      if ((tes.tabKey || tes.enterKey || KeyDown.up()) &&
          U"{:.7f}"_fmt(0.0) == tes.text && !autofill_lock) {
        tes.text =
            std::get<TheracConfigFloatDest *>(my_data)->source_input.tes.text;
        if (next_field != nullptr && next_field->text_field_type == FloatDest) {
          next_field->autofill_lock = true;
        }

      } else {
        autofill_lock = false;
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
    if (tes.text == U"X")
      next_field->tes.text = U"25000";
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
    if(!tes.active)
        break;
    if(tes.text == U"T"){
        if(verifyInputComplete()) {
            tsa.get()->externalCallWrap(thsAdapter::ExtCallSendMEOS,translateBeamType() , translateColPos(), getBeamEnergy());
            tsa.get()->externalCallWrap(thsAdapter::ExtCallToggleEditingTakingPlace);
            tsa.get()->externalCallWrap(thsAdapter::ExtCallToggleDatentComplete);
            }
        else {
            MALFUNCTION();
        }
    } else if(tes.text == U"P") {
        tsa.get()->externalCallWrap(thsAdapter::ExtCallProceed);
    } else if(tes.text == U"R") {
        tsa.get()->externalCallWrap(thsAdapter::ExtCallReset);
    }
    break;
  case Const:
    break;
  case Reason:
    tes.text = tsa.get()->requestStateInfo(thsAdapter::RequestReason);
    if(tes.text.starts_with(U"M")) {
        MALFUNCTION(ParseOpt<int>(tes.text.split(' ')[2]).value_or(777));
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
    if (next_field != nullptr && (tes.tabKey || tes.enterKey || KeyDown.up()) &&
        !jump_lock) {
      tes.active = false;
      next_field->tes.active = true;
      next_field->jump_lock = true;
      next_field->autofill_lock = true;
    } else if (prev_field != nullptr && KeyUp.up() && !jump_lock) {
        if(text_field_type == CmdEntry){
            if(tes.text == U"T") {
                tsa.get()->externalCallWrap(thsAdapter::ExtCallToggleEditingTakingPlace);
                tsa.get()->externalCallWrap(thsAdapter::ExtCallToggleDatentComplete);
            }
            tes.text.clear();
        }
        tes.active = false;
        prev_field->tes.active = true;
        prev_field->jump_lock = true;
        next_field->autofill_lock = true;
    } else
      jump_lock = false;
  }
}
TheracConfig::TheracConfig(FilePath p) {
  widget_config_filepath = p;
  load_ui_widgets();
}
TheracConfig::TheracConfig() { load_ui_widgets(); }
void TheracConfig::save_ui_widgets() {
  auto ui_widgets_ = ui_widgets.parallel_map(
      [](Array<std::pair<String, TheracTextType>> v)
          -> Array<std::pair<std::string, TheracTextType>> {
        return v.map([](std::pair<String, TheracTextType> v_) {
          return std::pair<std::string, TheracTextType>{
              std::get<0>(v_).toUTF8(), std::get<1>(v_)};
        });
      });
  nlohmann::json ok4{ui_widgets_};
  TextWriter w_ui_widgets{widget_config_filepath};
  w_ui_widgets.writeUTF8(ok4.dump(4));
  w_ui_widgets.close();
}
void TheracConfig::load_ui_widgets() {

  TextReader r_therac_ui{widget_config_filepath};
  Array<Array<std::pair<std::string, TheracTextType>>> widgets_u8 =
      nlohmann::json::parse(r_therac_ui.readAll().toUTF8())[0];
  ui_widgets = widgets_u8.parallel_map(
      [](auto v) -> Array<std::pair<String, TheracTextType>> {
        return v.map([](auto v_) {
          return std::pair<String, TheracTextType>{
              Unicode::FromUTF8(std::get<0>(v_)), std::get<1>(v_)};
        });
      });
  r_therac_ui.close();
}

} // namespace TheracConfig
