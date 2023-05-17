#ifndef EXTRAS_H
#define EXTRAS_H
#include "base.h"
#include "game.h"
#include "square.h"
#include "player.h"

// ##### EXTRA SQUARE DERIVED CLASSES #####

// tax square - there are two tax squares on the board, their only difference is the tax fee to be paid
// on landing on a tax square, the player pays the fee into the free parking cash pool
class tax : public square
{
    private:
        int tax_fee;
    public:
        tax(int _tax_fee, int _board_index, std::string _name):  tax_fee{_tax_fee}, square{_board_index, _name}{};
        ~tax(){if(print_destr)std::cout<<name<<" destr..."<<std::endl;}
        void landed_on(gamestate &_game);
};

// free parking square - unique square
// on landing on free parking, the player gains all cash from the cash pool, stored in the game class, and the pool is the set to 0
class free_parking : public square
{
    public:
        free_parking(int _board_index): square{_board_index, "Free Parking"}{};
        ~free_parking(){if(print_destr)std::cout<<name<<" destr..."<<std::endl;}
        void landed_on(gamestate &_game);
};

// go to jail square - unique square
// on landing on the go to jail square, the player is sent to jail, they do not collect 200. Whether or not they had previously rolled a double, their turn ends.
class go_to_jail : public square
{
    public:
        go_to_jail(int _board_index): square{_board_index, "Go To Jail"}{};
        ~go_to_jail(){if(print_destr)std::cout<<name<<" destr..."<<std::endl;}
        void landed_on(gamestate &_game){_game.send_to_jail();}
};

// jail square - unique square
// contains both the 'just visiting' part and the 'jail' part. When naturally landing on this square, nothing happens. 
// The difference is determined by a boolean in the player class, which is set when sending the player to jail
class jail : public square
{
    public:
        jail(int _board_index): square{_board_index, "Jail/Just Visiting"}{};
        ~jail(){if(print_destr)std::cout<<name<<" destr..."<<std::endl;}
        void landed_on(gamestate &_game){};
};


// go square - unique square
// When landing on or passing go during the turn, unless otherwise specified, the player gains 200 (handled in move square functions)
class go : public square
{
    public:
        go(int _board_index): square{_board_index, "Go"}{};
        ~go(){if(print_destr)std::cout<<name<<" destr..."<<std::endl;}
        void landed_on(gamestate &_game){};
};
#endif