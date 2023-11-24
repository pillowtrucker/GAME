#include "UnfriendlyTextBox.h"
#include <Siv3D.hpp>
namespace TheracConfig
{    
struct TheracConfigVerifier;
struct TheracConfigFloatDest;
class TheracConfig;

struct TheracConfigFloatDest {
    TheracConfig & source_input;
};

struct TheracConfigVerifier
{
    TheracConfig & source_input;
    TheracConfig & dest_input;
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
class TheracConfig
{
public:
    TheracConfig(String name, Point _p_in_grid,SimpleTable & grid, TextEditState _tes, HashTable<String, TheracTextType> & types);
    TheracTextType text_field_type;
    Point p_in_grid;
    TextEditState tes;
    TheracConfig * next_field;
    bool enabled = false;
    std::optional<HashTable<String, TheracConfig*>> dynamic_widgets;
    SimpleTable & grid;
    String name;
    uint32_t max_chars = 200;
    bool lock = false;
    TheracConfig * prev_field;
    void finish_setup();
    void mangle();
    std::variant<std::monostate,TheracConfigVerifier*,TheracConfigFloatDest*> my_data;
private:
    void verify_floats();
    void floatify();
    void enforce_int();
};
} // namespace TheracConfig
