#ifndef BASE_H
#define BASE_H

#include<iostream>
#include<iomanip>
#include<string>
#include<vector>
#include<cmath>
#include<fstream>
#include<sstream>
#include<istream>
#include<memory>
#include<algorithm>
#include<functional>
#include<map>
#include<random>

// declare player and gamestate classes
class player;
class gamestate;
class square;
class property;

// enum for storing street property colours and railway and utility type
// for clarification 'set' in 'set_identifier' doesn't mean you are setting the identifier, the enum is an identifier for the set the property belongs to whether that be a colour set or rail/util set
enum set_identifier{brown, light_blue, pink, orange, red, yellow, green, dark_blue, railways, utilities};


// overload << for printing out the set_identifier
std::ostream & operator<<(std::ostream &os, const set_identifier &id);

// enum for the state of the property: bank_defaul - default property state at start of game, owned by a player but not mortgaged, owned by a player and mortgaged
enum property_state{bank_default, player_default, player_mortgaged, bank_mortgaged};


// overload << for printing state
std::ostream & operator<<(std::ostream &os, const property_state &_state);

extern bool print_destr;
extern bool detailed_ai_props;
extern bool print_board_every_turn;
extern bool start_with_props;

namespace useful
{
    // takes a vector of potential inputs, returns 'repeat' unless the user input was valid
    std::string request_input(std::vector<std::string> valid_inputs);
    // create sufficiently random int in given range
    int random_int(int range_lower, int range_upper);
}
#endif
