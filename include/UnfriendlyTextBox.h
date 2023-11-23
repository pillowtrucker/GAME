
#pragma once
#include <cstdint>
#include "Siv3D.hpp"
#include "Siv3D/Stopwatch.hpp"
#include "Siv3D/2DShapes.hpp"
#include "Siv3D/Common.hpp"
#include "Siv3D/ListBoxState.hpp"
#include "Siv3D/Optional.hpp"
#include "Siv3D/TextAreaEditState.hpp"
#include "Siv3D/TextEditState.hpp"
#include "Siv3D/Unspecified.hpp"


using namespace s3d;
namespace mine {
namespace UnfriendlyTextBox

{

/// @brief テキストボックスを表示します。
/// @param text テキストボックスの状態への参照
/// @param center テキストボックスの中心座標
/// @param width テキストボックスの幅（ピクセル）
/// @param maxChars 入力できる最大の文字数 (char32 の要素数）
/// @param enabled テキストボックスの操作を有効にするか
/// @return テキストが操作された場合 true, それ以外の場合は false
bool TextBoxAt(TextEditState &text, const Vec2 &center, double width = 200.0,
               const Optional<size_t> &maxChars = unspecified,
               bool enabled = true, Font const & font = SimpleGUI::GetFont(), ColorF bgcolor = ColorF{0}, double actual_row_height = 200);
/// @brief テキストボックスを表示します。
/// @param text テキストボックスの状態への参照
/// @param pos テキストボックスの左上の座標
/// @param width テキストボックスの幅（ピクセル）
/// @param maxChars 入力できる最大の文字数 (char32 の要素数）
/// @param enabled テキストボックスの操作を有効にするか
/// @return テキストが操作された場合 true, それ以外の場合は false
bool TextBox(TextEditState &text, const Vec2 &pos, double width = 200.0,
             const Optional<size_t> &maxChars = unspecified,
             bool enabled = true, Font const & font = SimpleGUI::GetFont(), ColorF bgcolor = ColorF{0}, double actual_row_height = 200);

} // namespace UnfriendlyTextBox
} // namespace mine
