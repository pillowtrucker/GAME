#include "UnfriendlyTextBox.h"
#include <Siv3D.hpp>
namespace TheracConfig
{    
struct TheracConfigVerifier;
struct TheracConfigFloatDest;

    
enum TheracTextType
{
    FloatSrc, //0
    FloatDest, //1
    BeamEnergy, //2
    SingleChar, //3
    Verifier, //4
    Date, //5
    Time, //6
    OID, //7
    TreatPhase, //8
    BeamMode, //9
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
    std::optional<TheracConfig *> next_field;
    bool enabled = false;
    std::optional<HashTable<String, TheracConfig*>> dynamic_widgets;
    SimpleTable & grid;
    String name;
    uint32_t max_chars = 200;
    void finish_setup();
    void mangle();
    std::variant<std::monostate,TheracConfigVerifier *,TheracConfigFloatDest *> my_data;
private:

};
} // namespace TheracConfig
