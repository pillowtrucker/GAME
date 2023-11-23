#include "TheracConfig.h"
#include <Siv3D.hpp>
namespace myutb = mine::UnfriendlyTextBox;
namespace TheracConfig
{

TheracConfig::TheracConfig(String suf, Point p_in_grid, SimpleTable & grid)
{
    
}
struct TheracConfigFloatDest {
    TheracConfig & source_input;
};
struct TheracConfigVerifier
{
    TheracConfig & source_input;
    TheracConfig & dest_input;
    
};
} // namespace TheracConfig
