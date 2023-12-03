#include "Minigames/TheracSimulator/TheracMinigame.hpp"
#include "Minigames/TheracSimulator/TextBox.hpp"
#include "Plugins/TheracSimulatorAdapter.hpp"
#include "Minigames/TheracSimulator/TheracConsole.hpp"
#include "OpenSiv3D/Siv3D/include/Siv3D.hpp"
#include "marl/defer.h"
#include "marl/event.h"
#include "marl/scheduler.h"

namespace GAME::Minigames::TheracSimulator {
namespace tc = GAME::Minigames::TheracSimulator::Console;
namespace thsAdapter = GAME::Plugins::TheracSimulatorAdapter;
void TheracMinigame() {
  marl::Scheduler scheduler(marl::Scheduler::Config::allCores());
  scheduler.bind();
  defer(scheduler.unbind());
  auto background_colour = Palette::Black;
  auto main_monitor = System::GetCurrentMonitor();
  auto const mms = main_monitor.fullscreenResolution;
  auto const test_font_size = 100;
  
  Font test_font{FontMethod::MSDF, test_font_size,
                 U"resources/engine/font/hasklug/Hasklug.otf"};
  auto test_font_px_width = test_font.getGlyphInfo(U"█").width;
  String longest_column{U"COLLIMATOR ROTATION(DEG)"};
  ColorF transparent{1, 1, 1, 0};
  double num_columns = 7;
  double column_width = Math::Floor(mms.x / num_columns);
  auto pixel_budget = column_width / longest_column.length();
  auto pixels_per_point =
      static_cast<double>(test_font_px_width) / test_font_size;
  auto const font_size = Math::Floor(pixel_budget / pixels_per_point);

  Font myMonoFont{FontMethod::MSDF, static_cast<uint16_t>(font_size),
                                     U"resources/engine/font/hasklug/Hasklug.otf"};
  auto & _myMonoFont = myMonoFont;

  Font fat_font{FontMethod::MSDF, static_cast<uint16_t>(font_size)*3,
                U"resources/engine/font/hasklug/Hasklug.otf"};
  Array<double> columns_ = Array<double>(num_columns, column_width);
  SimpleTable grid{(Array<double> const &)columns_,
                   {.borderThickness = 0,
                    .backgroundColor = background_colour,
                    .textColor = Palette::Lime,
                    .variableWidth = false,
                    .font = myMonoFont,
                    .fontSize = font_size}};
  int32 fps;

  Window::Resize(mms);
  Window::Maximize();
  Scene::SetBackground(background_colour);


  tc::TheracConsole the_tc{};
  Array<Array<std::pair<String, tc::TheracConsoleWidgetType>>> ui_widgets;

  ui_widgets = the_tc.ui_widgets;
  HashTable<String, tc::TheracConsoleWidgetType> widget_types;

  ui_widgets.each([&grid, &widget_types](
                      Array<std::pair<String, tc::TheracConsoleWidgetType>> row) {
    auto label_row =
        row.map([&widget_types](std::pair<String, tc::TheracConsoleWidgetType> cell) {
          widget_types.insert(cell);
          return cell.first;
        });

    grid.push_back_row(label_row);
  });
  auto actual_row_height = grid.height() / grid.rows();

  auto tsa = std::make_shared<thsAdapter::TheracSimulatorAdapter>();
  auto screen_drawing_mutex = std::make_shared<std::mutex>();
  HashTable<String, tc::TheracConsoleWidget *> dynamic_widgets;
  auto overrides = phmap::parallel_node_hash_set<std::unique_ptr<std::function<void()>>>();
  
  //  Array<tc::TheracConfigWidget*> para_widget_array; // this approach
  //  increased performance from 10fps to 17
  grid.items().each_index([&, transparent,
                           tsa](auto i, auto v) {
    if (v.text.starts_with(U"PL")) {
      auto name = v.text;
      TextEditState tes;
      tc::TheracConsoleWidget *thc =
          new tc::TheracConsoleWidget{name, i, grid, tes, widget_types, tsa,screen_drawing_mutex,overrides,fat_font};
      dynamic_widgets.insert({name, thc});
      grid.setTextColor(i.y, i.x, transparent);
      grid.setBackgroundColor(i.y, i.x, transparent);
    }
  });
  for (auto w : dynamic_widgets) {
    w.second->dynamic_widgets = dynamic_widgets;
    w.second->finish_setup();
  }


  phmap::parallel_flat_hash_map<String, std::shared_ptr<marl::Event>> evs;
  auto locked_update = [=]() { // without this, incredibly trippy screen corruption and opengl errors i didnt know you could trigger from software
      std::lock_guard<std::mutex> sdm{*screen_drawing_mutex};
      return System::Update();
  };
      while (locked_update()) {
      {
          std::lock_guard<std::mutex> sdm{*screen_drawing_mutex};
          ClearPrint();
          grid.draw(Vec2{0, 0});
      }
    
      grid.items().each_index([=,&dynamic_widgets,
                             &evs](auto i, auto v) {
      if (v.text.starts_with(U"PL")) {
        tc::TheracConsoleWidget &w = *dynamic_widgets[v.text];
        {
            std::lock_guard<std::mutex> sdm{*screen_drawing_mutex};
            tc::TextBox(w,
                w.tes, Vec2{i.x * column_width, i.y * actual_row_height},
                column_width, w.max_chars, w.enabled, _myMonoFont, background_colour,
                actual_row_height);
        }
        if (evs.find(v.text) != evs.end()) {
          auto ev = evs[v.text];
          if ((*ev).isSignalled()) {
              (*ev).clear();
            marl::schedule([=, &w]() {
                defer((*ev).signal());
                
              w.mangle();
            });
          } else {
          }
        } else {
            auto ev = std::make_shared<marl::Event>(marl::Event::Mode::Manual);

          marl::schedule([=, &w]() {
              defer((*ev).signal());
            /*
            {
                // this DOESN'T WORK because this thing has to be overlaid on
                // and therefore synchronized with the layout grid
                std::lock_guard<std::mutex> sdm{*screen_drawing_mutex.get()};
                mine::UnfriendlyTextBox::TextBox(
                    w.tes, Vec2{i.x * column_width, i.y * actual_row_height},
                    column_width, w.max_chars, w.enabled, myMonoFont, background_colour,
                    actual_row_height);
            }
            */
            
            w.mangle();
          });
          evs[v.text] = ev;
        }
      }
    });
      {
          std::lock_guard<std::mutex> sdm{*screen_drawing_mutex.get()};
          //Console << overrides.get()->size();
          overrides.for_each([] (auto& fp) {(*fp)();});
      }
      {
          std::lock_guard<std::mutex> sdm{*screen_drawing_mutex.get()};
          fps = Profiler::FPS();
          Print(fps);
      }
  }
}
} // namespace GAME::Minigames
