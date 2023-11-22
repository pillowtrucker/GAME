#include "UnfriendlyTextBox.h"
#include "Siv3D.hpp"
namespace mine {
namespace UnfriendlyTextBox {
constexpr double MinTextBoxWidth = 40.0;
constexpr int32 TextBoxHeight = 36;
constexpr int32 FontYOffset = -1;
constexpr ColorF ActiveTextColor{Palette::Lime};
constexpr ColorF DisabledTextColor{Palette::Lime};
constexpr ColorF TextAreaEditingTextBackgroundColor{Palette::Black};
[[nodiscard]]
inline constexpr ColorF GetTextColor(bool enabled) noexcept
{
    return (enabled ? ActiveTextColor : DisabledTextColor);
}

bool TextBoxAt(TextEditState &text, const Vec2 &center, const double _width,
               const Optional<size_t> &maxChars, const bool enabled, Font const & font, ColorF bgcolor) {
  text.cursorPos = Min(text.cursorPos, text.text.size());
  text.tabKey = false;
  text.enterKey = false;
  
  const int32 fontHeight = font.height();

  const String previousText = text.text;
  const String editingText =
      ((text.active && enabled) ? TextInput::GetEditingText() : U"");


//  if(!editingText.empty())
//      Console << editingText;
  // テキストを更新する
  {
    if (text.active && enabled) {
      // text.text を更新する
      text.cursorPos = TextInput::UpdateText(
          text.text, text.cursorPos, TextInputMode::Default);
      // something is fucked with the default backspace/delete/etc handling in the UpdateText above.
      // can't see the characters in raw stream etc either
      if(KeyEnter.up()){
          
      }
      if(KeyBackspace.up())
      {
          if(text.cursorPos > 0)
          {
              text.text.erase(text.text.begin() + text.cursorPos -1);
              --text.cursorPos;
          }
      }
    }

    // 最大字数を超えていたら削る
    if (maxChars && (*maxChars < text.text.size())) {
      text.text.resize(*maxChars);
      text.cursorPos = Min(text.cursorPos, *maxChars);
    }


    // 文字列に変更があったかを調べる
    text.textChanged = (text.text != previousText);
    auto raw = TextInput::GetRawInput();
    if(!raw.empty())
        Console << TextInput::GetRawInput().xml_escape();
    // 文字列に変更があれば
    if (text.textChanged) {

        Console << text.text.xml_escape();
      // カーソル点滅をリセットする
      text.cursorStopwatch.restart();
    }
  }

  // テキストボックス
  const double width = Max(_width, MinTextBoxWidth);
  const RectF region{Arg::center = center, Max(width, MinTextBoxWidth),
                     TextBoxHeight};

  // マウスカーソルを IBeam にする
  if (enabled && Cursor::OnClientRect() && region.mouseOver()) {
      
      Cursor::RequestStyle(CursorStyle::IBeam);
  }

  // 入力カーソルのアクティブ / 非アクティブを切り替える
  if (MouseL.down() && (TextInput::GetEditingText().isEmpty())) {
    if (enabled && Cursor::OnClientRect() && region.mouseOver()) {
      text.active = true;
      text.resetStopwatches();

      // カーソルの位置を計算する
      {
        const double posX = (Cursor::PosF().x - (region.x + 8));
        size_t index = 0;
        double pos = 0.0;

        for (const auto &advance : font(text.text).getXAdvances()) {
          if (posX <= (pos + (advance / 2))) {
            break;
          }

          pos += advance;
          ++index;
        }

        text.cursorPos = index;
      }
    } else {
      text.active = false;
    }
  }

  // テキストカーソルを更新する
  if (text.text) {
    if (text.active && enabled && (not editingText)) {
      // キーでテキストカーソルを移動させる
      // 一定時間押下すると、テキストカーソルが高速に移動

      // テキストカーソルを先頭へ移動させる
      if ((KeyControl + KeyHome).down()) // [ctrl] + [home]: 全体の先頭へ
      {
        text.cursorPos = 0;
        text.cursorStopwatch.restart();
      } else if (
#if SIV3D_PLATFORM(MACOS)
          ((KeyControl + KeyA).down() || KeyHome.down())
#else
          KeyHome.down()
#endif
              ) // [home]: 行頭へ
      {
        text.cursorPos = 0;
        text.cursorStopwatch.restart();
      }

      // テキストカーソルを末尾へ移動させる
      if ((KeyControl + KeyEnd).down()) // [ctrl] + [end]: 全体の末尾へ
      {
        text.cursorPos = text.text.size();
        text.cursorStopwatch.restart();
      } else if (
#if SIV3D_PLATFORM(MACOS)
          ((KeyControl + KeyE).down() || KeyEnd.down())
#else
          KeyEnd.down()
#endif
              ) // [end]: 行末へ
      {
        text.cursorPos = text.text.size();
        text.cursorStopwatch.restart();
      }

      // [←] キー
      if ((0 < text.cursorPos) &&
          (KeyLeft.down() || ((SecondsF{0.33} < KeyLeft.pressedDuration()) &&
                              (SecondsF{0.06} < text.leftPressStopwatch)))) {
        --text.cursorPos;
        text.leftPressStopwatch.restart();
      }

      // [→] キー
      if ((text.cursorPos < text.text.size()) &&
          (KeyRight.down() || ((SecondsF{0.33} < KeyRight.pressedDuration()) &&
                               (SecondsF{0.06} < text.rightPressStopwatch)))) {
        ++text.cursorPos;
        text.rightPressStopwatch.restart();
      }
    }
  }

  if (text.active && enabled && (not editingText)) {
    // [tab][enter] キーで入力カーソルを非アクティブに
    {
      const String raw = TextInput::GetRawInput();
      for(auto const ch: raw)
      {
          if(ch == U'\b')
              Console << U"BACKSPACE IN TEXT";
      }
      text.tabKey = raw.contains(U'\t');
      text.enterKey = raw.contains(U'\r');
      

      if (text.tabKey || text.enterKey) {
        text.active = false;
      }
    }
  }

  // 描画
  {
    const Vec2 textPos{(region.x + 8),
                       (center.y - font.height() / 2.0 + FontYOffset - 0.5)};
    

   
    region.draw(bgcolor);
    
    {
      const ColorF textColor = GetTextColor(enabled);
      const auto &pixelShader = Font::GetPixelShader(font.method());

      double cursorPosX = textPos.x;
      Vec2 editingTextPos = textPos;

      // テキストの描画
      {
        const ScopedCustomShader2D shader{pixelShader};
        Vec2 penPos = textPos;
        const Array<Glyph> glyphs = font.getGlyphs(text.text);

        for (auto &&[index, glyph] : Indexed(glyphs)) {
          const double xAdvance = glyph.xAdvance;
          const Vec2 glyphPos = (penPos + glyph.getOffset());

          glyph.texture.draw(glyphPos, textColor);
          penPos.x += xAdvance;

          // テキストカーソルの位置の計算を計算する
          if (text.active && (text.cursorPos == (index + 1))) {
            cursorPosX = penPos.x;
            editingTextPos = penPos;
          }
        }
      }

      if (editingText) {
        // 変換テキストとその領域の取得
        const Array<Glyph> editingGlyphs = font.getGlyphs(editingText);
        Array<Vec2> editingGlyphPositions(editingGlyphs.size());
        {
          Vec2 penPos = editingTextPos;

          for (size_t i = 0; i < editingGlyphs.size(); ++i) {
            const auto &glyph = editingGlyphs[i];
            editingGlyphPositions[i] = (penPos + glyph.getOffset());
            penPos.x += glyph.xAdvance;
          }
        }

        // 変換テキスト背景の描画
        if (editingGlyphs) {
          const auto &firstGlyph = editingGlyphs.front();
          const auto &lastGlyph = editingGlyphs.back();
          const Vec2 pos =
              (editingGlyphPositions.front() - firstGlyph.getOffset());
          const double w = ((editingGlyphPositions.back().x -
                             lastGlyph.getOffset().x + lastGlyph.xAdvance) -
                            pos.x);
          RectF{pos, w, fontHeight}.draw(ColorF{0,0xff,0});

          // 変換テキストの選択範囲の描画
          {
#if SIV3D_PLATFORM(WINDOWS)

            const std::pair<int32, int32> editingTarget =
                Platform::Windows::TextInput::GetCursorIndex();

#elif SIV3D_PLATFORM(WEB)

            const std::pair<int32, int32> editingTarget =
                Platform::Web::TextInput::GetCandicateCursorIndex();

#else

            const std::pair<int32, int32> editingTarget{-1, 0};

#endif

            if (editingTarget.second &&
                ((editingTarget.first + editingTarget.second) <=
                 editingGlyphPositions.size())) {
              const int32 firstIndex = editingTarget.first;
              const int32 lastIndex =
                  (editingTarget.first + editingTarget.second - 1);
              const double x0 = editingGlyphPositions[firstIndex].x;
              const double x1 = editingGlyphPositions[lastIndex].x +
                                editingGlyphs[lastIndex].xAdvance;
              RectF{x0, (pos.y + fontHeight - 2), (x1 - x0), 2}.draw(
                  ActiveTextColor);
            }
          }
        }

        // 変換テキストの描画
        {
          const ScopedCustomShader2D shader{pixelShader};

          for (size_t i = 0; i < editingGlyphs.size(); ++i) {
            const auto &glyph = editingGlyphs[i];
            glyph.texture.draw(editingGlyphPositions[i], textColor);
          }
        }
      }
      

      // テキスト入力カーソルの描画
      if (text.active && enabled) {
        const bool showCursor = (text.cursorStopwatch.ms() % 1200 < 600) ||
                                (text.leftPressStopwatch.isRunning() &&
                                 (text.leftPressStopwatch < SecondsF{0.5})) ||
                                (text.rightPressStopwatch.isRunning() &&
                                 (text.rightPressStopwatch < SecondsF{0.5}));

        if (showCursor) {
          const RectF cursor(
              Arg::leftCenter(Vec2{cursorPosX, region.center().y}.asPoint()), 1,
              26);
          cursor.draw(Palette::Lime);
        }
      }
    }
  }

#if SIV3D_PLATFORM(WEB)

  Platform::Web::TextInput::SetFocusToTextInput(text.active);

  if (text.active && not editingText) {
    if (text.lastCursorPos != text.cursorPos) {
      Platform::Web::TextInput::SyncronizeText(text.text);
      Platform::Web::TextInput::SetCursorIndex(text.cursorPos);
    } else if (auto currentCursorPos =
                   Platform::Web::TextInput::GetCursorIndex();
               text.lastCursorPos != currentCursorPos) {
      text.cursorPos = currentCursorPos;
      text.cursorStopwatch.restart();
    }

    text.lastCursorPos = text.cursorPos;
  }

#endif

  return text.textChanged;
}
bool TextBox(TextEditState &text, const Vec2 &pos, double width,
             const Optional<size_t> &maxChars, const bool enabled, Font const & font, ColorF bgcolor) {
  width = Max(width, MinTextBoxWidth);

  return TextBoxAt(text, pos + Vec2{width * 0.5, TextBoxHeight/2}, width, maxChars,
                   enabled);
}

} // namespace UnfriendlyTextBox
} // namespace mine
