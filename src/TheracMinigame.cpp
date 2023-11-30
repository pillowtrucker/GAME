#include <Siv3D.hpp>
#include <future>
#include "TheracConfig.h"
#include "UnfriendlyTextBox.h"
#include "TheracSimulatorAdapter.hpp"
#include "marl/defer.h"
#include "marl/event.h"
#include "marl/scheduler.h"
#include "marl/waitgroup.h"
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
  namespace tc = TheracConfig;

  tc::TheracConfig the_tc{};
  Array<Array<std::pair<String, tc::TheracTextType>>> ui_widgets;

  ui_widgets = the_tc.ui_widgets;
  HashTable<String, tc::TheracTextType> widget_types;

  ui_widgets.each([&grid, &widget_types](
                      Array<std::pair<String, tc::TheracTextType>> row) {
    auto label_row =
        row.map([&widget_types](std::pair<String, tc::TheracTextType> cell) {
          widget_types.insert(cell);
          return cell.first;
        });

    grid.push_back_row(label_row);
  });
  auto actual_row_height = grid.height() / grid.rows();

  auto tsa = std::make_shared<thsAdapter::TheracSimulatorAdapter>();
  
  HashTable<String, tc::TheracConfigWidget *> dynamic_widgets;
//  Array<tc::TheracConfigWidget*> para_widget_array; // this approach increased performance from 10fps to 17
  grid.items().each_index(
      [&widget_types, &dynamic_widgets, &grid, transparent,tsa](auto i, auto v) {
        if (v.text.starts_with(U"PL")) {
          auto name = v.text;
          TextEditState tes;
          tc::TheracConfigWidget *thc =
              new tc::TheracConfigWidget{name, i, grid, tes, widget_types,tsa};
//          para_widget_array.push_back(thc);
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
 
  
//  phmap::parallel_flat_hash_map<String, std::thread> fuck;
  while (System::Update()) {
      ClearPrint();
//      koo.parallel_each([](auto k) {k->join();});
//      koo.clear();

      
    grid.draw(Vec2{0, 0});
    grid.items().each_index([&dynamic_widgets, background_colour, column_width,
                             actual_row_height, &myMonoFont,&evs](auto i, auto v) {
      if (v.text.starts_with(U"PL")) {
        tc::TheracConfigWidget &w = *dynamic_widgets[v.text];

        mine::UnfriendlyTextBox::TextBox(
            w.tes, Vec2{i.x * column_width, i.y * actual_row_height},
            column_width, w.max_chars, w.enabled, myMonoFont, background_colour,
            actual_row_height); // this is not thread-safe/reentrant due to some siv3d font dingdong breaking
        if(evs.find(v.text) != evs.end()) {
            auto ev = evs[v.text];
            if(ev.get()->isSignalled())
            {
                ev.get()->clear();
                marl::schedule([=,&w]()
                {
                    defer(ev.get()->signal());
                    w.mangle();
                });
                
                
            }
            else {
                
            }
        }
        else {
            auto ev = std::make_shared<marl::Event>(marl::Event::Mode::Manual);

            marl::schedule([=,&w]()
            {
                defer(ev.get()->signal());
                w.mangle();
            });
            evs[v.text] = ev;
        }
        
        /*
        if(fuck.find(v.text) != fuck.end()) {
            auto k = std::move(fuck[v.text]);
            if(k.joinable()) {
                k.join();
                fuck[v.text] = std::thread([&w](){w.mangle();});
            }
            else
            {
                fuck[v.text] = std::move(k);
            }
        }
        else {
            fuck[v.text] = std::thread([&w](){w.mangle();});
        }

        */
      }
    });
    

    //   auto garbage = para_widget_array.parallel_map([](auto w){w->mangle(); return 0;}); // parallel_each is fucked and Array thinks it needs to return something
    fps = Profiler::FPS();
    Window::SetTitle(fps);
    Print(fps);
  }
}
