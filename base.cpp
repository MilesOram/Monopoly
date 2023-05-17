#include "base.h"
std::ostream & operator<<(std::ostream &os, const set_identifier &id)
{
    std::string text;
    switch(id)
    {
        case brown:
            text = "brown";
            break;
        case light_blue:
            text = "light blue";
            break;
        case pink:
            text = "pink";
            break;
        case orange:
            text = "orange";
            break;
        case red:
            text = "red";
            break;
        case yellow:
            text = "yellow";
            break;
        case green:
            text = "green";
            break;
        case dark_blue:
            text = "dark blue";
            break;
        case railways:
            text = "railway";
            break;
        case utilities:
            text = "utility";
            break;
    }
    os<<text;
    return os;
}

std::ostream & operator<<(std::ostream &os, const property_state &_state)
{
    std::string text;
    switch(_state)
    {
        case bank_default:
            text = "Available for purchase";
            break;
        case player_default:
            text = "Owned - Not mortgaged";
            break;
        case player_mortgaged:
            text = "Owned - Mortgaged";
            break;
        case bank_mortgaged:
            text = "Available for purchase - Mortgaged";
            break;
    }
    os<<text;
    return os;
}
bool print_destr{false};
bool detailed_ai_props{true};
bool print_board_every_turn{false};
bool start_with_props{false};

namespace useful
{
    // takes a vector of potential inputs, returns 'repeat' unless the user input was valid
    std::string request_input(std::vector<std::string> valid_inputs)
    {
        std::string input{"repeat"};
        std::getline(std::cin,input);
        for(auto itr=valid_inputs.begin();itr<valid_inputs.end();itr++){if(input==*itr)return input;}
        return input;
    }
    // create sufficiently random int in given range
    int random_int(int range_lower, int range_upper)
    {
        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_int_distribution<std::mt19937::result_type> rnd_dist(range_lower,range_upper);
        return rnd_dist(rng);
    }
}