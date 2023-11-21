# include <Siv3D.hpp>
// #include <list>

void Main()
{
    auto main_monitor = System::GetCurrentMonitor();
    auto const mms = main_monitor.fullscreenResolution;
    auto const test_font_size = 100;
    Font test_font{FontMethod::MSDF, test_font_size, U"resources/engine/font/hasklug/Hasklug.otf"};
    auto test_font_px_width = test_font.getGlyphInfo(U"█").width;
    String longest_column{U"COLLIMATOR ROTATION(DEG)"};
    ColorF color_black{0x00,0x00,0x00};
    ColorF color_green{0x00,0xff,0x00};
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
            .borderThickness = 0, .backgroundColor = color_black,.textColor = color_green, .variableWidth = false, .font = myMonoFont, .fontSize = font_size
        }
    };
    int32 fps;
    
    Window::Resize(mms);
    Window::Maximize();
    Scene::SetBackground(color_black);
    
    Array<Array<String>> grid_labels{
    {
        {U"PATIENT NAME:",           U"PLACEHOLDER_PN",  U"",                U"",               U"",             U"",               U"" },
        {U"TREATMENT MODE:",         U"FIX",             U"BEAM TYPE:",      U"PLACEHOLDER_BT", U"ENERGY (KeV):",U"PLACEHOLDER_EN", U""},
        {U"",                        U"",                U"",                U"",               U"",             U"",               U""},
        {U"",                        U"",                U"ACTUAL",          U"PRESCRIBED",     U"",             U"",               U""},
        {U"UNIT RATE/MINUTE",        U"PLACEHOLDER_URMA",U"PLACEHOLDER_URMP",U"PLACEHOLDER_V1", U"",             U"",               U""},
        {U"MONITOR UNITS",           U"PLACEHOLDER_MUA", U"PLACEHOLDER_MUP", U"PLACEHOLDER_V2", U"",             U"",               U""},
        {U"TIME(MIN)",               U"PLACEHOLDER_TMA", U"PLACEHOLDER_TMP", U"PLACEHOLDER_V3", U"",             U"",               U""},
        {U"",                        U"",                U"",                U"",               U"",             U"",               U""},
        {U"",                        U"",                U"",                U"",               U"",             U"",               U""},
        {U"",                        U"",                U"",                U"",               U"",             U"",               U""},
        {U"GANTRY ROTATION(DEG)",    U"PLACEHOLDER_GRA", U"PLACEHOLDER_GRP", U"PLACEHOLDER_V4", U"",             U"",               U""},
        {U"COLLIMATOR ROTATION(DEG)",U"PLACEHOLDER_CRA", U"PLACEHOLDER_CRP", U"PLACEHOLDER_V5", U"",             U"",               U""},
        {U"COLLIMATOR X",            U"PLACEHOLDER_CXA", U"PLACEHOLDER_CXP", U"PLACEHOLDER_V6", U"",             U"",               U""},
        {U"COLLIMATOR Y",            U"PLACEHOLDER_CYA", U"PLACEHOLDER_CYP", U"PLACEHOLDER_V7", U"",             U"",               U""},
        {U"WEDGE NUMBER",            U"PLACEHOLDER_WNA", U"PLACEHOLDER_WNP", U"PLACEHOLDER_V8", U"",             U"",               U""},
        {U"ACCESSORY NUMBER",        U"PLACEHOLDER_ANA", U"PLACEHOLDER_ANP", U"PLACEHOLDER_V9", U"",             U"",               U""},
        {U"",                        U"",                U"",                U"",               U"",             U"",               U""},
        {U"",                        U"",                U"",                U"",               U"",             U"",               U""},
        {U"",                        U"",                U"",                U"",               U"",             U"",               U""},
        {U"",                        U"",                U"",                U"",               U"",             U"",               U""},
        {U"",                        U"",                U"",                U"",               U"",             U"TREAT",          U"AUTO"},
        {U"",                        U"",                U"",                U"",               U"",             U"PLACEHOLDER_MOD",U"173777"},
        {U"OPR ID:",                 U"PLACEHOLDER_OID", U"REASON:",         U"PLACEHOLDER_REA",U"COMMAND:",     U"PLACEHOLDER_CMD",U""},
    }
    };
    grid_labels.each([&grid] (auto row){
        grid.push_back_row(row);
    });
    auto actual_width = grid.width();
    Console << actual_width;
    grid.items().each_index([](auto i, auto v)
    {
        if(v.text.starts_with(U"PLACEHOLDER"))
        { 
            //v.textWidth;
        }
    });
	while (System::Update())
	{
		// 1 秒間に何回メインループが実行されているかを取得する
		fps = Profiler::FPS();
        grid.draw(Vec2{0,0});
		// 1 秒間に何回メインループが実行されているかを、ウィンドウタイトルに表示する
		Window::SetTitle(fps);
	}
}
