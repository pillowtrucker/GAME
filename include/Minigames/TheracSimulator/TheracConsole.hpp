#pragma once
#include "Plugins/TheracSimulatorAdapter.hpp"
#include "OpenSiv3D/Siv3D/include/Siv3D.hpp"

namespace GAME::Minigames::TheracSimulator::Console {

namespace thsAdapter = GAME::Plugins::TheracSimulatorAdapter;

struct TheracConsoleVerifier;
struct TheracConsoleFloatDest;
class TheracConsole;
class TheracConsoleWidget;

struct TheracConsoleFloatDest {
    TheracConsoleWidget & source_input;
};

struct TheracConsoleVerifier
{
    TheracConsoleWidget & source_input;
    TheracConsoleWidget & dest_input;
};    
enum TheracConsoleWidgetType
{
    FloatSrc, //0
    FloatDest, //1
    BeamEnergy, //2
    BeamModeInput, //3 beam type _input_
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
    BeamEnergyOut // 15
};
class TheracConsoleWidget
{
public:
    TheracConsoleWidget(String name, Point _p_in_grid,SimpleTable & grid, TextEditState _tes, HashTable<String, TheracConsoleWidgetType> & types, std::shared_ptr<thsAdapter::TheracSimulatorAdapter> _tsa, std::shared_ptr<std::mutex> _sdm,    phmap::parallel_node_hash_set<std::unique_ptr<std::function<void()>>>& _overrides, Font & _fat_font);
    TheracConsoleWidgetType text_field_type;
    Point p_in_grid;
    TextEditState tes;
    TheracConsoleWidget * next_field;
    bool enabled = false;
    Optional<HashTable<String, TheracConsoleWidget*>> dynamic_widgets;
    SimpleTable & grid;
    String name;
    uint32_t max_chars = 200;
    std::timed_mutex timed_state_mutex;
    TheracConsoleWidget * prev_field;
    void finish_setup();
    void mangle();
    std::variant<std::monostate,TheracConsoleVerifier*,TheracConsoleFloatDest*> my_data;
    phmap::parallel_node_hash_map<uint32_t, bool> keys_up;
    std::shared_mutex jump_mutex;
    bool should_jump = true;
    std::shared_mutex autofill_mutex;
    bool should_autofill = true;

private:
    void verify_floats();
    void floatify();
    void enforce_int();
    std::shared_ptr<thsAdapter::TheracSimulatorAdapter> tsa;
    thsAdapter::BeamType translateBeamType();
    thsAdapter::CollimatorPosition translateColPos();
    int getBeamEnergy();
    bool verifyInputComplete();
    void MALFUNCTION(int num = 11);
    std::shared_ptr<std::mutex> screen_drawing_mutex;
    phmap::parallel_node_hash_set<std::unique_ptr<std::function<void()>>> & overrides;
    Font & fat_font;
};
class TheracConsole {
public:
    TheracConsole(Array<Array<std::pair<String, TheracConsoleWidgetType>>> ui_widgets_): ui_widgets{ui_widgets_}{};
    TheracConsole(FilePath p);
    TheracConsole();
    void save_ui_widgets();
    void load_ui_widgets();
    Array<Array<std::pair<String, TheracConsoleWidgetType>>> ui_widgets;
private:
    FilePath widget_config_filepath = U"resources/stage1/therac_ui.json";
};
} // namespace GAME::Minigames::TheracSimulator::Console
