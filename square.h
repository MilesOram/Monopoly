#ifndef SQUARE_H
#define SQUARE_H
#include "base.h"
#include "player.h"
#include "game.h"

class square
{
    protected:
        int board_index;
        std::string name;
    public:
        square(){}
        square(int _board_index, std::string _name): board_index(_board_index),  name(_name){}
        virtual ~square(){};
        virtual void landed_on(gamestate &_game)=0;
        int get_index();
        void print_name();
        void change_name(std::string a);
        std::string get_name();
};

class property : public square
{
    // A property is a square on that board that a player can own, this includes streets, utilities and railways
    protected:
        // price to buy a property for the first time
        int cost;
        int mortgage_price;
        property_state state{property_state::bank_default};
        set_identifier identifier;

    public:
        property(): cost{0}, mortgage_price{0}, square{0, "None"}{};
        property(int _cost, int _board_index, int _mortgage, std::string _name, set_identifier _identifier);
        virtual ~property(){};
        void landed_on(gamestate &_game);
        void change_property_state(property_state new_state);
        virtual void get_tiers()=0;
        property_state get_state();
        set_identifier get_identifier();
        bool option_to_buy(gamestate &_game);
        virtual int amount_to_pay(player &_owner);
        virtual void mortgage(player &_player);
        void unmortgage(player &_player, gamestate &_game);
        friend bool is_street(property &_property);
        friend class player;

};

class street : public property
{
    protected:
        int *rent_tiers;
        // number of houses on property and price to buy each house (max number = 4), starts at 0
        int houses{0};
        int const house_price;
        // boolean for whether property has a hotel since max number is one and price to buy a hotel, starts false
        bool hotel{false};
        int const hotel_price;
        // boolean for whether this street and all other streets of the same colour are owned by one player
        // (the exact player that owns the set is not relevant here; the bool is used as a check for the ability to buy houses/hotels and a check for double rent payment), start false

    public:
        street(int tiers[6], int _cost, int _board_index, int _mortgage, std::string _name,
                     set_identifier _colour, int _house_price, int _hotel_price);
        ~street(){delete[] rent_tiers; if(print_destr)std::cout<<name<<" destr..."<<std::endl;}

        int amount_to_pay(player &_owner);
        void get_tiers();
        void buy_house(player &_owner, gamestate &_game);
        void buy_hotel(player &_owner, gamestate &_game);
        void sell_house(player &_owner);
        void sell_hotel(player &_owner);
        int get_houses(){return houses;}
        bool get_hotel(){return hotel;}
        bool full_set_owned(player &_owner);
        void mortgage(player &_player);
        bool operator==(std::shared_ptr<property> _property);
        bool operator==(std::shared_ptr<street> _street);
        bool operator==(set_identifier _id);
        friend class player;
};

class railway : public property
{
    protected:
        const static int rent_tiers[];
        
    public:
        // constructor for railway: takes location on board index and name of railway
        railway(int _board_index, std::string _name, set_identifier _identifier): property{200, _board_index, 100, _name, _identifier}{};
        ~railway(){if(print_destr)std::cout<<name<<" destr..."<<std::endl;}
        int amount_to_pay(player &_owner);
        void get_tiers();
        bool operator==(std::shared_ptr<property> _property);
};

class utility : public property
{
    private:
        const static int base_multiplier{4};
        const static int set_multiplier{10};
    public:
        utility(int _board_index, std::string _name, set_identifier _identifier): property{150, _board_index, 75, _name, _identifier}{};
        ~utility(){if(print_destr)std::cout<<name<<" destr..."<<std::endl;}
        int amount_to_pay(player &_owner);
        void get_tiers();
        bool operator ==(std::shared_ptr<property> _property);
};

extern std::shared_ptr<std::shared_ptr<square>[]> board;
#endif

