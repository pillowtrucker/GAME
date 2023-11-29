#include "TheracSimulatorAdapter.hpp"
#include "UnfriendlyTextBox.h"
#include <Siv3D.hpp>
namespace TheracConfig
{    
struct TheracConfigVerifier;
struct TheracConfigFloatDest;
class TheracConfig;
class TheracConfigWidget;

struct TheracConfigFloatDest {
    TheracConfigWidget & source_input;
};

struct TheracConfigVerifier
{
    TheracConfigWidget & source_input;
    TheracConfigWidget & dest_input;
};    
enum TheracTextType
{
    FloatSrc, //0
    FloatDest, //1
    BeamEnergy, //2
    SingleChar, //3 beam type _input_
    Verifier, //4
    Date, //5
    Time, //6
    OID, //7
    TreatPhase, //8
    BeamMode, //9 -- beam mode _output_
    CmdEntry, //10
    Const, //11
    Reason, //12
    Normal, //13
    Subsys, //14
};
class TheracConfigWidget
{
public:
    TheracConfigWidget(String name, Point _p_in_grid,SimpleTable & grid, TextEditState _tes, HashTable<String, TheracTextType> & types, thsAdapter::TheracSimulatorAdapter& _tsa);
    TheracTextType text_field_type;
    Point p_in_grid;
    TextEditState tes;
    TheracConfigWidget * next_field;
    bool enabled = false;
    Optional<HashTable<String, TheracConfigWidget*>> dynamic_widgets;
    SimpleTable & grid;
    String name;
    uint32_t max_chars = 200;
    bool jump_lock = false;
    bool autofill_lock = false;
    TheracConfigWidget * prev_field;
    void finish_setup();
    void mangle();
    std::variant<std::monostate,TheracConfigVerifier*,TheracConfigFloatDest*> my_data;

private:
    void verify_floats();
    void floatify();
    void enforce_int();
    thsAdapter::TheracSimulatorAdapter & tsa;

};
class TheracConfig {
public:
    TheracConfig(Array<Array<std::pair<String, TheracTextType>>> ui_widgets_): ui_widgets{ui_widgets_}{};
    TheracConfig(FilePath p);
    TheracConfig();
    void save_ui_widgets();
    void load_ui_widgets();
    Array<Array<std::pair<String, TheracTextType>>> ui_widgets;
private:
    FilePath widget_config_filepath = U"resources/stage1/therac_ui.json";
};    
} // namespace TheracConfig
