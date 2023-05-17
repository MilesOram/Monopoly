#ifndef GAME_H
#define GAME_H
#include "base.h"
#include "square.h"

// class used for executing the gameplay loop and controls the players
// each player is constructed inside the constructor for this class
class gamestate
{
    private:
        int current_player;
        std::vector<std::shared_ptr<player>> all_players;
        std::map<std::string,std::shared_ptr<property>> property_list;
        int cash_pool{0};
        bool player_out_this_turn{false};
    public:
        gamestate(int players, std::map<std::string,std::shared_ptr<property>> _property_list);
        ~gamestate(){all_players.clear(); property_list.clear();}
        friend void print_board(std::shared_ptr<std::shared_ptr<square>[]> const &_board, gamestate _game);
        void set_current_player(int _index){current_player = _index;}
        std::vector<std::shared_ptr<player>> get_all_players(){return all_players;}
        void update_to_next_player(){current_player = (current_player+1)% all_players.size();}
        void determine_order();
        std::shared_ptr<player> get_owner(property &_property);
        std::shared_ptr<property> get_property_pointer(std::string _name){return property_list[_name];}
        void end_turn();
        void send_to_jail();
        void execute_turn(std::shared_ptr<std::shared_ptr<square>[]> &_board);
        std::shared_ptr<player> get_current_player(){return all_players[current_player];}
        int get_and_reset_cash_pool();
        const void all_prop_info();
        void game_end(){property_list.clear(); all_players.clear();}
        
        void add_to_pool(int amount){cash_pool+=amount;}
        void remove_current_player_from_game();
};
#endif