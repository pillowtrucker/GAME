#pragma once
#include "Minigames/TheracSimulator/TheracConsole.hpp"
#include "OpenSiv3D/Siv3D/include/Siv3D.hpp"


namespace GAME::Minigames::TheracSimulator::Console {

    bool TextBoxAt(TheracConsoleWidget& w,TextEditState &text, const Vec2 &center, double width = 200.0,
               const Optional<size_t> &maxChars = unspecified,
               bool enabled = true, Font const & font = SimpleGUI::GetFont(), ColorF bgcolor = ColorF{0}, double actual_row_height = 200);

    bool TextBox(TheracConsoleWidget& w,TextEditState &text, const Vec2 &pos, double width = 200.0,
             const Optional<size_t> &maxChars = unspecified,
             bool enabled = true, Font const & font = SimpleGUI::GetFont(), ColorF bgcolor = ColorF{0}, double actual_row_height = 200);
} // namespace GAME::Minigames::TheracSimulator::Console
