# include "Siv3D.hpp"
#include "UnfriendlyTextBox.h"
#include "TheracConfig.h"

void Main()
{
    auto main_monitor = System::GetCurrentMonitor();
    auto const mms = main_monitor.fullscreenResolution;
    auto const test_font_size = 100;
    auto max_chars_in_field = 200;
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
    namespace tc = TheracConfig;    
    Array<Array<std::tuple<String,tc::TheracTextType>>> grid_labels{
    {
        {{U"PATIENT NAME:",tc::TheracTextType::Const},           {U"PLACEHOLDER_PN",tc::TheracTextType::Normal},{U"",tc::TheracTextType::Const},                   {U"",tc::TheracTextType::Const},                      {U"",tc::TheracTextType::Const},                  {U"",tc::TheracTextType::Const},                  {U"",tc::TheracTextType::Const}},
        {{U"TREATMENT MODE:",tc::TheracTextType::Const},         {U"FIX",tc::TheracTextType::Const},            {U"BEAM TYPE:",tc::TheracTextType::Const},         {U"PLACEHOLDER_BT",tc::TheracTextType::SingleChar},   {U"ENERGY (KeV):",tc::TheracTextType::Const},     {U"PLACEHOLDER_EN",tc::TheracTextType::Int},      {U"",tc::TheracTextType::Const}},
        {{U"",tc::TheracTextType::Const},                        {U"",tc::TheracTextType::Const},               {U"",tc::TheracTextType::Const},                   {U"",tc::TheracTextType::Const},                      {U"",tc::TheracTextType::Const},                  {U"",tc::TheracTextType::Const},                  {U"",tc::TheracTextType::Const}},
        {{U"",tc::TheracTextType::Const},                        {U"",tc::TheracTextType::Const},               {U"ACTUAL",tc::TheracTextType::Const},             {U"PRESCRIBED",tc::TheracTextType::Const},            {U"",tc::TheracTextType::Const},                  {U"",tc::TheracTextType::Const},                  {U"",tc::TheracTextType::Const}},
        {{U"UNIT RATE/MINUTE",tc::TheracTextType::Const},        {U"",tc::TheracTextType::Const},               {U"PLACEHOLDER_URMA",tc::TheracTextType::FloatSrc},{U"PLACEHOLDER_URMP",tc::TheracTextType::FloatDest},  {U"PLACEHOLDER_V1",tc::TheracTextType::Verifier}, {U"",tc::TheracTextType::Const},                  {U"",tc::TheracTextType::Const}},
        {{U"MONITOR UNITS",tc::TheracTextType::Const},           {U"",tc::TheracTextType::Const},               {U"PLACEHOLDER_MUA",tc::TheracTextType::FloatSrc}, {U"PLACEHOLDER_MUP",tc::TheracTextType::FloatDest},   {U"PLACEHOLDER_V2",tc::TheracTextType::Verifier}, {U"",tc::TheracTextType::Const},                  {U"",tc::TheracTextType::Const}},
        {{U"TIME(MIN)",tc::TheracTextType::Const},               {U"",tc::TheracTextType::Const},               {U"PLACEHOLDER_TMA",tc::TheracTextType::FloatSrc}, {U"PLACEHOLDER_TMP",tc::TheracTextType::FloatDest},   {U"PLACEHOLDER_V3",tc::TheracTextType::Verifier}, {U"",tc::TheracTextType::Const},                  {U"",tc::TheracTextType::Const}},
        {{U"",tc::TheracTextType::Const},                        {U"",tc::TheracTextType::Const},               {U"",tc::TheracTextType::Const},                   {U"",tc::TheracTextType::Const},                      {U"",tc::TheracTextType::Const},                  {U"",tc::TheracTextType::Const},                  {U"",tc::TheracTextType::Const}},
        {{U"",tc::TheracTextType::Const},                        {U"",tc::TheracTextType::Const},               {U"",tc::TheracTextType::Const},                   {U"",tc::TheracTextType::Const},                      {U"",tc::TheracTextType::Const},                  {U"",tc::TheracTextType::Const},                  {U"",tc::TheracTextType::Const}},
        {{U"",tc::TheracTextType::Const},                        {U"",tc::TheracTextType::Const},               {U"",tc::TheracTextType::Const},                   {U"",tc::TheracTextType::Const},                      {U"",tc::TheracTextType::Const},                  {U"",tc::TheracTextType::Const},                  {U"",tc::TheracTextType::Const}},
        {{U"GANTRY ROTATION(DEG)",tc::TheracTextType::Const},    {U"",tc::TheracTextType::Const},               {U"PLACEHOLDER_GRA",tc::TheracTextType::FloatSrc}, {U"PLACEHOLDER_GRP",tc::TheracTextType::FloatDest},   {U"PLACEHOLDER_V4",tc::TheracTextType::Verifier}, {U"",tc::TheracTextType::Const},                  {U"",tc::TheracTextType::Const}},
        {{U"COLLIMATOR ROTATION(DEG)",tc::TheracTextType::Const},{U"",tc::TheracTextType::Const},               {U"PLACEHOLDER_CRA",tc::TheracTextType::FloatSrc}, {U"PLACEHOLDER_CRP",tc::TheracTextType::FloatDest},   {U"PLACEHOLDER_V5",tc::TheracTextType::Verifier}, {U"",tc::TheracTextType::Const},                  {U"",tc::TheracTextType::Const}},
        {{U"COLLIMATOR X",tc::TheracTextType::Const},            {U"",tc::TheracTextType::Const},               {U"PLACEHOLDER_CXA",tc::TheracTextType::FloatSrc}, {U"PLACEHOLDER_CXP",tc::TheracTextType::FloatDest},   {U"PLACEHOLDER_V6",tc::TheracTextType::Verifier}, {U"",tc::TheracTextType::Const},                  {U"",tc::TheracTextType::Const}},
        {{U"COLLIMATOR Y",tc::TheracTextType::Const},            {U"",tc::TheracTextType::Const},               {U"PLACEHOLDER_CYA",tc::TheracTextType::FloatSrc}, {U"PLACEHOLDER_CYP",tc::TheracTextType::FloatDest},   {U"PLACEHOLDER_V7",tc::TheracTextType::Verifier}, {U"",tc::TheracTextType::Const},                  {U"",tc::TheracTextType::Const}},
        {{U"WEDGE NUMBER",tc::TheracTextType::Const},            {U"",tc::TheracTextType::Const},               {U"PLACEHOLDER_WNA",tc::TheracTextType::FloatSrc}, {U"PLACEHOLDER_WNP",tc::TheracTextType::FloatDest},   {U"PLACEHOLDER_V8",tc::TheracTextType::Verifier}, {U"",tc::TheracTextType::Const},                  {U"",tc::TheracTextType::Const}},
        {{U"ACCESSORY NUMBER",tc::TheracTextType::Const},        {U"",tc::TheracTextType::Const},               {U"PLACEHOLDER_ANA",tc::TheracTextType::FloatSrc}, {U"PLACEHOLDER_ANP",tc::TheracTextType::FloatDest},   {U"PLACEHOLDER_V9",tc::TheracTextType::Verifier}, {U"",tc::TheracTextType::Const},                  {U"",tc::TheracTextType::Const}},
        {{U"",tc::TheracTextType::Const},                        {U"",tc::TheracTextType::Const},               {U"",tc::TheracTextType::Const},                   {U"",tc::TheracTextType::Const},                      {U"",tc::TheracTextType::Const},                  {U"",tc::TheracTextType::Const},                  {U"",tc::TheracTextType::Const}},
        {{U"",tc::TheracTextType::Const},                        {U"",tc::TheracTextType::Const},               {U"",tc::TheracTextType::Const},                   {U"",tc::TheracTextType::Const},                      {U"",tc::TheracTextType::Const},                  {U"",tc::TheracTextType::Const},                  {U"",tc::TheracTextType::Const}},
        {{U"",tc::TheracTextType::Const},                        {U"",tc::TheracTextType::Const},               {U"",tc::TheracTextType::Const},                   {U"",tc::TheracTextType::Const},                      {U"",tc::TheracTextType::Const},                  {U"",tc::TheracTextType::Const},                  {U"",tc::TheracTextType::Const}},
        {{U"",tc::TheracTextType::Const},                        {U"",tc::TheracTextType::Const},               {U"",tc::TheracTextType::Const},                   {U"",tc::TheracTextType::Const},                      {U"",tc::TheracTextType::Const},                  {U"",tc::TheracTextType::Const},                  {U"",tc::TheracTextType::Const}},
        {{U"DATE:",tc::TheracTextType::Const},                   {U"PLACEHOLDER_DAT",tc::TheracTextType::Date}, {U"SYSTEM:",tc::TheracTextType::Const},            {U"PLACEHOLDER_SYS",tc::TheracTextType::Subsys},      {U"OP MODE:",tc::TheracTextType::Const},          {U"TREAT",tc::TheracTextType::Const},             {U"AUTO",tc::TheracTextType::Const}},
        {{U"TIME",tc::TheracTextType::Const},                    {U"PLACEHOLDER_TIM",tc::TheracTextType::Time}, {U"TREAT:",tc::TheracTextType::Const},             {U"PLACEHOLDER_TREAT",tc::TheracTextType::TreatPhase},{U"",tc::TheracTextType::Const},                  {U"PLACEHOLDER_MOD",tc::TheracTextType::Const},   {U"173777",tc::TheracTextType::Const}},
        {{U"OPR ID:",tc::TheracTextType::Const},                 {U"PLACEHOLDER_OID",tc::TheracTextType::OID},  {U"REASON:",tc::TheracTextType::Const},            {U"PLACEHOLDER_REAS",tc::TheracTextType::Reason},     {U"COMMAND:",tc::TheracTextType::Const},          {U"PLACEHOLDER_CMD",tc::TheracTextType::CmdEntry},{U"",tc::TheracTextType::Const}},
    }
    };
    grid_labels.each([&grid] (auto row){
        auto label_row = row.map([](auto l)
        {
            return std::get<0>(l);
        });
        grid.push_back_row(label_row);
    });
    //JSON testojson{grid_labels};
    //testojson.save(U"./testo.json");
    auto actual_row_height = grid.height()/grid.rows();
    Console << actual_row_height;


    
    namespace myutb = mine::UnfriendlyTextBox;

    HashTable<String, TextEditState> text_boxes;
//    HashTable<String, tc::TheracConfig> text_boxes;

    grid.items().each_index([&text_boxes,&grid,transparent](auto i, auto v)
    {
        
        if(v.text.starts_with(U"PL") )
        {

            TextEditState tes;
            auto suf = v.text.substr(12);
            //auto thc = tc::TheracConfig{suf};
//            tc::TheracConfig thc{tes};

            text_boxes.insert({v.text,tes});

            grid.setTextColor(i.y,i.x,transparent);
            grid.setBackgroundColor(i.y,i.x,transparent);

        }
    });
   
	while (System::Update())
	{
        grid.draw(Vec2{0,0});
        grid.items().each_index([&text_boxes,column_width,actual_row_height,myMonoFont,max_chars_in_field](auto i, auto v)
        {
            if(v.text.starts_with(U"PLACEHOLDER"))
            {
                TextEditState & tes = text_boxes[v.text];
                
                mine::UnfriendlyTextBox::TextBox(tes, Vec2{ i.x * column_width, i.y * actual_row_height},column_width,max_chars_in_field,true,myMonoFont,Palette::Black,actual_row_height);
 

            }
            });
		fps = Profiler::FPS();
		Window::SetTitle(fps);
	}
}
