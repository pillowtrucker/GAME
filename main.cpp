# include "Siv3D.hpp"
# include "include/UnfriendlyTextBox.h"
void Main()
{
    auto main_monitor = System::GetCurrentMonitor();
    auto const mms = main_monitor.fullscreenResolution;
    auto const test_font_size = 100;
    Font test_font{FontMethod::MSDF, test_font_size, U"resources/engine/font/hasklug/Hasklug.otf"};
    auto test_font_px_width = test_font.getGlyphInfo(U"█").width;
    String longest_column{U"COLLIMATOR ROTATION(DEG)"};
    ColorF transparent{1,1,1,0};
    double num_columns = 7;
    double column_width = Math::Floor(mms.x/num_columns);
    auto pixel_budget = column_width/longest_column.length();
    auto pixels_per_point = static_cast<double>(test_font_px_width)/test_font_size;
    auto const font_size = Math::Floor(pixel_budget/pixels_per_point);
    
    Font myMonoFont{FontMethod::MSDF, static_cast<uint16_t>(font_size), U"resources/engine/font/hasklug/Hasklug.otf"};
    Array<double> columns_ = Array<double>(num_columns,column_width);
    SimpleTable grid
    {
        (Array<double> const &) columns_,
        {
            .borderThickness = 0, .backgroundColor = Palette::Black,.textColor = Palette::Lime, .variableWidth = false, .font = myMonoFont, .fontSize = font_size
        }
    };
    int32 fps;
    
    Window::Resize(mms);
    Window::Maximize();
    Scene::SetBackground(Palette::Black);

    
    Array<Array<String>> grid_labels{
    {
        {U"PATIENT NAME:",           U"PLACEHOLDER_PN",  U"",                U"",                U"",             U"",               U""},
        {U"TREATMENT MODE:",         U"FIX",             U"BEAM TYPE:",      U"PLACEHOLDER_BT",  U"ENERGY (KeV):",U"PLACEHOLDER_EN", U""},
        {U"",                        U"",                U"",                U"",                U"",             U"",               U""},
        {U"",                        U"",                U"ACTUAL",          U"PRESCRIBED",      U"",             U"",               U""},
        {U"UNIT RATE/MINUTE",        U"",                U"PLACEHOLDER_URMA",U"PLACEHOLDER_URMP",U"PLACEHOLDER_V1", U"",             U""},
        {U"MONITOR UNITS",           U"",                U"PLACEHOLDER_MUA", U"PLACEHOLDER_MUP", U"PLACEHOLDER_V2", U"",             U""},
        {U"TIME(MIN)",               U"",                U"PLACEHOLDER_TMA", U"PLACEHOLDER_TMP", U"PLACEHOLDER_V3", U"",             U""},
        {U"",                        U"",                U"",                U"",                U"",             U"",               U""},
        {U"",                        U"",                U"",                U"",                U"",             U"",               U""},
        {U"",                        U"",                U"",                U"",                U"",             U"",               U""},
        {U"GANTRY ROTATION(DEG)",    U"",                U"PLACEHOLDER_GRA", U"PLACEHOLDER_GRP", U"PLACEHOLDER_V4", U"",             U""},
        {U"COLLIMATOR ROTATION(DEG)",U"",                U"PLACEHOLDER_CRA", U"PLACEHOLDER_CRP", U"PLACEHOLDER_V5", U"",             U""},
        {U"COLLIMATOR X",            U"",                U"PLACEHOLDER_CXA", U"PLACEHOLDER_CXP", U"PLACEHOLDER_V6", U"",             U""},
        {U"COLLIMATOR Y",            U"",                U"PLACEHOLDER_CYA", U"PLACEHOLDER_CYP", U"PLACEHOLDER_V7", U"",             U""},
        {U"WEDGE NUMBER",            U"",                U"PLACEHOLDER_WNA", U"PLACEHOLDER_WNP", U"PLACEHOLDER_V8", U"",             U""},
        {U"ACCESSORY NUMBER",        U"",                U"PLACEHOLDER_ANA", U"PLACEHOLDER_ANP", U"PLACEHOLDER_V9", U"",             U""},
        {U"",                        U"",                U"",                U"",                U"",             U"",               U""},
        {U"",                        U"",                U"",                U"",                U"",             U"",               U""},
        {U"",                        U"",                U"",                U"",                U"",             U"",               U""},
        {U"",                        U"",                U"",                U"",                U"",             U"",               U""},
        {U"",                        U"",                U"",                U"",                U"",             U"TREAT",          U"AUTO"},
        {U"",                        U"",                U"",                U"",                U"",             U"PLACEHOLDER_MOD",U"173777"},
        {U"OPR ID:",                 U"PLACEHOLDER_OID", U"REASON:",         U"PLACEHOLDER_REA", U"COMMAND:",     U"PLACEHOLDER_CMD",U""},
    }
    };
    grid_labels.each([&grid] (auto row){
        grid.push_back_row(row);
    });

    auto actual_row_height = grid.height()/grid.rows();

    HashTable<String, TextEditState> text_boxes;
    grid.items().each_index([&text_boxes,&grid,transparent](auto i, auto v)
    {
        if(v.text.starts_with(U"PLACEHOLDER"))
        { 
            TextEditState tes;

            text_boxes.insert(std::pair(v.text,tes));

            grid.setTextColor(i.y,i.x,transparent);
            grid.setBackgroundColor(i.y,i.x,transparent);

        }
    });
   
	while (System::Update())
	{
        grid.draw(Vec2{0,0});
        grid.items().each_index([&text_boxes,column_width,actual_row_height,myMonoFont](auto i, auto v)
        {
            if(v.text.starts_with(U"PLACEHOLDER"))
            {
                TextEditState & tes = text_boxes[v.text];
                
                mine::UnfriendlyTextBox::TextBox(tes, Vec2{ i.x * column_width, i.y * actual_row_height},column_width,200,true,myMonoFont,Palette::Black);
                //SimpleGUI::TextBox(tes, Vec2{ i.x * column_width, i.y * actual_row_height},column_width);

            }
            });
        /*       ClearPrint();


        		const Array<Input> keys = Keyboard::GetAllInputs();

        		for (const auto& key : keys)
        		{
        			Print << key.name() << (key.pressed() ? U" pressed" : U" up");
        		}
        */
		fps = Profiler::FPS();
		Window::SetTitle(fps);
	}
}
