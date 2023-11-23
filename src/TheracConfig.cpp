#include "TheracConfig.h"
#include <Siv3D.hpp>
namespace myutb = mine::UnfriendlyTextBox;
namespace TheracConfig
{

TheracConfig::TheracConfig(String _name, Point _p_in_grid, SimpleTable &_grid,
                           TextEditState _tes,
                           HashTable<String, TheracTextType> & types)
    : p_in_grid{_p_in_grid}, tes{_tes}, grid{_grid},name{_name} {
      text_field_type = types[name];

}

struct TheracConfigFloatDest {
    TheracConfig & source_input;
};

struct TheracConfigVerifier
{
    TheracConfig & source_input;
    TheracConfig & dest_input;
};
// finish doing setup after all the other widgets are set up
void TheracConfig::finish_setup()
{
      switch(text_field_type)
      {

      case BeamMode:
          max_chars = 1;
          enabled = true;
          next_field = dynamic_widgets.value()[U"PLACEHOLDER_EN"];
          break;
      case BeamEnergy:
          max_chars = 5;
          enabled = true;
          next_field = dynamic_widgets.value()[U"PLACEHOLDER_URMP"];
          break;
      case Verifier:
          my_data = new TheracConfigVerifier{.source_input = *dynamic_widgets.value()[grid.getItem(Point (p_in_grid.x-2,p_in_grid.y)).text],.dest_input = *dynamic_widgets.value()[grid.getItem(Point (p_in_grid.x-1,p_in_grid.y)).text]};
          break;
      case Date:
      case Time:
      case OID:
      case TreatPhase:   
      case Reason:
      case Subsys:
      case Const:
          break;
      case FloatSrc:
          max_chars = 10;
          break;
      case FloatDest:
          enabled = true;
          max_chars = 10;
          next_field = dynamic_widgets.value()[grid.getItem(Point (p_in_grid.x,p_in_grid.y+1)).text];
          my_data = new TheracConfigFloatDest{.source_input = *dynamic_widgets.value()[grid.getItem(Point (p_in_grid.x-1,p_in_grid.y)).text]};
          break;
      case Normal:
          enabled = true;
          if(name == U"PLACEHOLDER_PN")
              next_field = dynamic_widgets.value()[U"PLACEHOLDER_BT"];
          break;
      case SingleChar:
          max_chars = 1;
      case CmdEntry:
          enabled = true;
          break;
      }
}

void TheracConfig::mangle()
{

}

} // namespace TheracConfig
