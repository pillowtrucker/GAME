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
    Int, //2
    SingleChar, //3
    Verifier, //4
    Date, //5
    Time, //6
    OID, //7
    TreatPhase, //8
    BeamMode, //9
    BeamEnergy, //10
    CmdEntry, //11
    Const, //12
    Reason, //13
    Normal, //14
    Subsys, //15
};
class TheracConfig
{
public:
    TheracConfig(String suf, Point p_in_grid,SimpleTable & grid);
    TheracTextType text_field_type;
    TextEditState tes;
private:
    std::optional<TheracConfig *> next_field;
    std::optional<std::variant<TheracConfigVerifier*,TheracConfigFloatDest*>> data;
};
} // namespace TheracConfig
