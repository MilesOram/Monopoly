#ifndef PLAYER_H
#define PLAYER_H
#include "base.h"
#include "square.h"
#include "game.h"
#include "visuals.h"

class player
{
    private:
        std::vector<std::shared_ptr<property>> properties_owned;
        std::string name;
        bool ai;
        int cash{1500};
        int current_square_index{0};
        int get_out_of_jail_cards{0};
        int doubles_rolled{0};
        bool in_jail{false};
        int player_number;
        int turns_in_jail{0}; 
    public:
        player(bool ai_controlled, std::string _name, int _player_number): ai{ai_controlled}, name{_name}, player_number{_player_number}{}
        player(bool ai_controlled, std::vector<std::shared_ptr<property>> new_props, std::string _name, int _player_number);
        ~player(){properties_owned.clear();};
        void add_property(std::shared_ptr<property> &p);
        void remove_property(std::shared_ptr<property> &p, property_state new_state);
        std::vector<std::shared_ptr<property>> get_properties_owned(){return properties_owned;}
        const void show_properties();
        const void show_properties_extended();
        int get_current_square(){return current_square_index;}
        int get_doubles(){return doubles_rolled;}
        bool get_jail_bool(){return in_jail;}
        std::string get_name(){return name;}
        bool is_ai(){return ai;}
        void add_get_out_of_jail_card(){get_out_of_jail_cards+=1;}
        int get_cash(){return cash;}
        void add_cash(int amount);
        void pay_bank(int amount, gamestate &_game, bool free_parking_cash);
        void move_to_square(int new_square_index, bool can_collect_go_cash, gamestate &_game);
        void update_get_out_of_jail_cards();
        void move_squares(int move_squares, bool can_collect_go_cash, gamestate &_game);
        void handle_jail(gamestate &_game);
        void manage_properties(gamestate &_game);
        void mortgage_all();
        void mortgage_required(int to_pay);
        int max_possible_cash();
        void give_all_props_to_bank();
        void give_all_props_to_player(player &_player);
        void pay_player(player &owner, int amount, gamestate &_game);
        void roll(gamestate &_game);
        friend gamestate; 
};
#endif