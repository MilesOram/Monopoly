#include"player.h"

void player::add_cash(int amount)
{
    std::cout<<name<<" current balance: "<<cash;
    cash+=amount;
    std::cout<<" -> "<<cash<<std::endl;
}

void player::mortgage_required(int to_pay)
{
    for(auto itr{properties_owned.begin()}; itr<properties_owned.end(); ++itr)
    {
        if((*itr)->get_state()!= property_state::player_mortgaged)
        {
            (*itr)->mortgage(*this);
            if(cash>to_pay) {break;}
        }
    }
}
void player::mortgage_all()
{
    for(auto itr{properties_owned.begin()}; itr<properties_owned.end(); ++itr)
        {
            (*itr)->mortgage(*this);
        }
}
// default way to pay for anything that isn't giving money to another player
// the free_parking_cash bool is true in cases where money should be paid into free parking e.g. tax sqaure
void player::pay_bank(int amount, gamestate &_game, bool free_parking_cash)
{
    std::cout<<name<<" current balance: "<<cash<<std::endl;
    // if the player mortgages all properties, can they remain in the game, if not the bank takes all properties and the player is deleted and removed from the game
    if(amount>max_possible_cash())
    {
        std::cout<<"Out of cash. "<<name<<" is out of the game."<<std::endl;
        give_all_props_to_bank();
        _game.remove_current_player_from_game();

    }
    if(amount>cash && amount<max_possible_cash())
    {
        if(ai){mortgage_required(amount);}
        else
        {
            while(cash<amount)
            {
                // technically by giving the player the option to fully manage their properties, they could cycle their properties, mortgaging and unmortgaging a large number of times 
                // e.g. gain 50 from mortgaging, unmortgaging costs 60 -> each cycle = -10 cash
                // this would leave them with insufficient cash to pay so this max cash check is necessary in each loop 
                if(amount>max_possible_cash())
                {
                    std::cout<<"Out of cash."<<name<<" is out of the game."<<std::endl;
                    // hand over all properties in their current state to the bank
                    give_all_props_to_bank();
                    // remove from the game
                    _game.remove_current_player_from_game();
                    // exit function
                    return;
                }
                std::cout<<"Amount owed = "<<amount<< ". Cash = "<<cash<<std::endl;
                manage_properties(_game);
            }
        }
    }
    cash-=amount;
    if(free_parking_cash){_game.add_to_pool(amount);}
    std::cout<<name<<" balance after: "<<cash<<std::endl;
}
//  used by chance/community chest cards or sending to jail to move the player to a specified square on the board by index
// also input checks if player is allowed to collect 200 from passing go
void player::move_to_square(int new_square_index, bool can_collect_go_cash, gamestate &_game)
{
    // the only way a player can be moved to index 10 is if the player is sent to jail (it can never be just visiting)
    if(new_square_index==10 && in_jail){visuals::place_in_jail(player_number);current_square_index=new_square_index;}
    else
    {
        // translates to if you pass go, collect 200
        if(can_collect_go_cash && new_square_index < current_square_index)
        {
            std::cout<<"Passed GO, collecting 200."<<std::endl;
            add_cash(200);
        }
        // move the player and execute the landed_on function for that square 
        current_square_index=new_square_index;
        visuals::place_player_number(player_number,get_current_square());
        board[get_current_square()]->landed_on(_game);
    }
}
// used by some chance/community chest cards to move the player a set amount of squares either backwards or forwards
// primarily used for dice rolls to move players forward each turn
void player::move_squares(int move_squares, bool can_collect_go_cash, gamestate &_game)
{
    // translates to if you pass go, collect 200
    if(can_collect_go_cash && (move_squares+current_square_index)%40 < current_square_index && move_squares>0)
    {
        std::cout<<"Passed GO, collecting 200."<<std::endl;
        add_cash(200);
    }        
    // handle wrapping around board (39: mayfair -> 0: go) (this might be an odd way of coding this but had issues using % with negatives)
    current_square_index=(move_squares+current_square_index);
    current_square_index=(40+(current_square_index%40))%40;
    std::cout<<"New square: "<<current_square_index<<", "<<board[current_square_index]->get_name()<<std::endl;
    // move player to new square and execute landed_on
    visuals::place_player_number(player_number,get_current_square());
    board[get_current_square()]->landed_on(_game);
}
// player constructor - takes whether player is ai or not, a vector of properties to start the game with (used for easier testing of functionality), player name e.g. "Player 1" and player number e.g. 1
player::player(bool ai_controlled, std::vector<std::shared_ptr<property>> new_props, std::string _name, int _player_number): ai{ai_controlled}, name{_name}, player_number{_player_number}
{
    // add starting properties to owned properties
    for(auto itr = new_props.begin(); itr < new_props.end(); ++itr)
    {
        properties_owned.push_back(*itr);
        (*itr)->change_property_state(property_state::player_default);
    }
}

// used for adding a property mid game (e.g. after landing on and buying it)
void player::add_property(std::shared_ptr<property> &p)
{
    // make a copy of the shared pointer and set state to player owned and not mortgaged, then add shared pointer to player properties vector
    std::shared_ptr<property> _p(p);
    switch(_p->get_state())
    {
        case player_default:
            _p->change_property_state(property_state::player_default);
            break;
        case player_mortgaged:
            _p->change_property_state(property_state::player_mortgaged);
            break;
        case bank_default:
            _p->change_property_state(property_state::player_default);
            break;
        case bank_mortgaged:
            _p->change_property_state(property_state::player_default);
            break;
    }
    properties_owned.push_back(_p);
}

// used to remove property mid game - used when a player loses the game and must give all properties to either the bank or the player they lost to (owed money but could not pay)
void player::remove_property(std::shared_ptr<property> &p, property_state new_state)
{
    // make a copy of the shared pointer and set state to new specified state
    // remove from the player's owned property vector
    std::shared_ptr<property> _p(p);
    _p->change_property_state(new_state);
    properties_owned.erase(std::remove(properties_owned.begin(), properties_owned.end(), _p), properties_owned.end());   
}

// print out a list of the names of each property owned by the player
const void player::show_properties()
{   
    std::cout<<"Properties: "<<std::endl;
    for(auto itr = properties_owned.begin(); itr < properties_owned.end(); ++itr){std::cout<< (*itr)->name<< std::endl;}
}

// Need to give the player sufficient information to make decisions but not overload with all available info
// this function is only called when the player decides to manage their properties and shows all relevant info for each property they own 
const void player::show_properties_extended()
{   
    std::cout<<"Properties: "<<std::endl;
    // print name, state, set_identifier, rent_tiers (and number of houses if it's a street)
    for(auto itr = properties_owned.begin(); itr < properties_owned.end(); ++itr)
    {
        std::cout<< (*itr)->name<<": "<<(*itr)->state <<", "<<(*itr)->get_identifier()<<", Rent tiers: ";
        (*itr)->get_tiers();
    }
}

// roll function used only for rolling the dice at the start of each players turn to move, and subsequently if a double is rolled and they do not go to jail
// call this function again until they do not get a double or they roll three doubles in a turn and are sent to jail
void player::roll(gamestate &_game)
{
    // simulate rolling two independent dice
    int d1{useful::random_int(1,6)};
    int d2{useful::random_int(1,6)};
    std::cout<<"Dice roll: "<<d1<<" "<<d2<<std::endl;
    int sum{d1+d2};
    // move the player (true means they can collect 200 from passing go)
    this->move_squares(sum, true,_game);
    // update the player member doubles_rolled to keep track of doubles rolled this turn, reset to 0 if no double rolled this time
    if(d1==d2) {doubles_rolled+=1;std::cout<<"Doubles rolled: "<<doubles_rolled<<std::endl;}
    else{doubles_rolled=0;}
}

// theoretical maximum amount of cash a player could accumulate by mortgaging all properties and selling all houses
// used in all situations where a player must pay money to check if they are unable
// if they cannot pay, they lose the game
int player::max_possible_cash()
{
    int max{cash};    
    for(auto itr{properties_owned.begin()};itr<properties_owned.end(); itr++)
    {
        if((*itr)->get_state()!=property_state::player_mortgaged)
        {
            // get values of hotels and houses on all streets owned
            if(is_street(**itr))
            {
                // downcast to access street house and hotel data
                std::shared_ptr<property> p = (*itr);
                auto s = std::static_pointer_cast<street>(p);
                // houses and hotels sell for half as much as they were bought for
                max+=(s->houses*s->house_price/2);
                if(s->hotel)
                {
                    max+=s->hotel_price/2;
                    max+=s->house_price*4/2;
                }
            }
            // add on mortgage price for all propeties (railways, utilities, streets)
            max+=(*itr)->mortgage_price;
        }
    }
    return max;
}

// pay another player a set amount (used when landing on another players property or for chance/community chest cards)
void player::pay_player(player &owner, int amount, gamestate &_game)
{
    std::cout<<name<<" current balance: "<<cash<<std::endl;
    // if the amount to pay is more than their theoretical maximum cash, they lose the game
    if(amount>max_possible_cash())
    {
        std::cout<<"Out of cash."<<name<<" is out of the game."<<std::endl;
        // hand over all properties in their current state to owner
        give_all_props_to_player(owner);
        // give all cash to owner
        owner.add_cash(cash);
        // remove from the game
        _game.remove_current_player_from_game();
        // exit function
        return;
    }
    if(amount>cash && amount<max_possible_cash())
    {
        
        // ai will cycle through all properties, mortgaging until they have the required cash
        if(ai){mortgage_required(amount);}
        else
        {
            while(cash<amount)
            {
                // technically by giving the player the option to fully manage their properties, they could cycle their properties, mortgaging and unmortgaging a large number of times 
                // e.g. gain 50 from mortgaging, unmortgaging costs 60 -> each cycle = -10 cash
                // this would leave them with insufficient cash to pay so this max cash check is necessary in each loop 
                if(amount>max_possible_cash())
                {
                    std::cout<<"Out of cash."<<name<<" is out of the game."<<std::endl;
                    // hand over all properties in their current state to owner
                    give_all_props_to_player(owner);
                    // give all cash to owner
                    owner.add_cash(cash);
                    // remove from the game
                    _game.remove_current_player_from_game();
                    // exit function
                    return;
                }
                std::cout<<"Amount owed = "<<amount<< ". Cash = "<<cash<<std::endl;
                manage_properties(_game);
            }
        }
    }
    // pay cash
    cash-=amount;
    owner.add_cash(amount);
    std::cout<<name<<" balance after: "<<cash<<std::endl;
    
}
// when losing the game to a player (owing them more than you could pay) give all properties in their current state to the other player
void player::give_all_props_to_player(player &_player)
{
    auto temp_props{properties_owned};
    for(auto itr{temp_props.begin()}; itr< temp_props.end(); itr++)
    {
        // make copy of pointer to give to new player
        std::shared_ptr<property> _property(*itr);
        switch((*itr)->state)
        {
            case player_default:
                remove_property(*itr,property_state::player_default);
                _player.add_property(_property);
                break;
            case player_mortgaged:
                remove_property(*itr,property_state::player_mortgaged);
                _player.add_property(_property);
                break;
        }
    }
}

// when losing to the bank, remove all properties and change to bank states, mortgaged properties will remain mortgaged
void player::give_all_props_to_bank()
{
    auto temp_props{properties_owned};
    for(auto itr{temp_props.begin()}; itr< temp_props.end(); itr++)
    {
        switch((*itr)->state)
        {
            case player_default:
                remove_property(*itr,property_state::bank_default);
                break;
            case player_mortgaged:
                remove_property(*itr,property_state::bank_mortgaged);
                break;
        }
    }
}
// function used at the start of a player's turn if they are currently in jail
void player::handle_jail(gamestate &_game)
{
    // fallback - this should never happen
    if(!in_jail || turns_in_jail>2)
    {
        std::cout<<"should be in jail??"<<std::endl;
        in_jail=false;
        turns_in_jail=0;
        //shift to just visiting
        move_to_square(10,false,_game);
        return;
    }
    // do nothing on the turn they are released except move to just visiting (released on their third turn in jail)
    if(turns_in_jail==2)
    {
        std::cout<<"Out of jail!"<<std::endl;
        in_jail=false;
        turns_in_jail=0;
        //shift to just visiting
        move_to_square(10,false,_game);
        return;
    }
    if(get_out_of_jail_cards>0)
    {
        std::cout<<"Would you like to use a get out of jail card to leave jail immediately?"<<std::endl;
        while(true)
        {
            std::string result;
            if(!ai)
            {
                result = useful::request_input({"y","n"});
            }
            else{result = "y";}
            if(result=="y")
            {
                std::cout<<"Out of jail!"<<std::endl;
                in_jail=false;
                get_out_of_jail_cards-=1;
                turns_in_jail=0;
                //shift to just visiting
                move_to_square(10,false,_game);
                return;
            }
            if(result=="n"){break;}
        }
    }
    int d1{useful::random_int(1,6)};
    int d2{useful::random_int(1,6)};
    std::cout<<"Dice roll: "<<d1<<" "<<d2<<std::endl;
    if(d1==d2)
    {
        std::cout<<"Out of jail!"<<std::endl;
        in_jail=false;
        turns_in_jail=0;
        //shift to just visiting
        move_to_square(10,false,_game);
        return;
        
    }
    else
    {
        turns_in_jail+=1;
        std::cout<<"You're staying in jail. Turns in jail: "<<turns_in_jail<<std::endl;
    }

}

// option for the player to manage their properties at the end of their turn
// detailed property information is shown so player can make decisions
// player is taken through different levels of depth based on input to enact changes to properties like adding houses or mortgaging etc.
void player::manage_properties(gamestate &_game)
{
    show_properties_extended();
    // vectors to store the potential input options displayed to the player to access different properties: street inputs and railway and utility inputs
    std::vector<std::string> inputs_str;
    std::vector<std::string> inputs_util_rail;
    // streets must be handled separately and downcasted to access house and hotel info
    std::vector<std::shared_ptr<street>> temp_streets;
    std::vector<std::shared_ptr<property>> temp_util_rail;
    std::cout<<"Manage properties - enter: ";
    for(auto itr{properties_owned.begin()};itr<properties_owned.end(); itr++)
    {
        // if street
        if(is_street(**itr))
        {
            // add to temporary list of streets as a pointer to a street
            std::shared_ptr<property> p = (*itr);
            auto _str = std::static_pointer_cast<street>(p);
            temp_streets.push_back(_str);
        }
        else
        {
            // otherwise add to list of railways and utilities
            std::shared_ptr<property> p = (*itr);
            temp_util_rail.push_back(p);
        }
    }
    // option to look at just streets 's'
    inputs_util_rail.push_back("s");
    // add a number to list of inputs corresponding to each owned utility and railway
    for(int i{0}; i<size(temp_util_rail);i++)
    {
        inputs_util_rail.push_back(std::to_string(i));
    }
    // option to exit 'e'
    inputs_util_rail.push_back("e");
    // list of numbers for managing just streets
    for(int i{0}; i<size(temp_streets);i++)
    {
        inputs_str.push_back(std::to_string(i));
    }
    // option to exit streets 'e'
    inputs_str.push_back("e");

    // ## First Level ## 
    // give the player the option to manage just their streets with 's', option to exit managing with 'e', or manage an individual railway or utility as listed with a number '0','1',etc.
    while(true)
    {
        // prints e.g. "s to manage streets. 0 for Water Works. 1 for Liverpool Street Station. 2 for King's Cross Station. e to exit. (cash = 1200)
        std::cout<<"s to manage streets. ";
        for(int i{0}; i<size(temp_util_rail);i++)
        {
            std::cout<<i<<" for "<<temp_util_rail[i]->get_name()<<". ";
        }
        std::cout<<"e to exit. (cash = "<<cash<<")"<<std::endl;
        // requests input from list of inputs e.g. (s,0,1,2,e)
        std::string result{useful::request_input(inputs_util_rail)};
        // if e, exit
        if(result=="e"){break;}
        // if suitable number, iterate through rails, utils to find it and manage
        for(int i{0}; i<size(temp_util_rail);i++)
        {
            // use +1 since 0th is "s"
            if(inputs_util_rail[i+1]==result)
            {
                // ## Individual Property Level ## 
                // can mortgage/unmortgage individual properties
                std::string result_r_u;
                switch(temp_util_rail[i]->state)
                {
                    // if unmortgaged, only option for rail/util is to mortgage or exit
                    case player_default:
                        while(true)
                        {
                            std::cout<<"0 to mortgage this property for "<<temp_util_rail[i]->mortgage_price<<". e to exit: "<<std::endl;
                            result_r_u = useful::request_input({"0","e"});
                            if(result_r_u=="e"){break;}
                            if(result_r_u=="0"){temp_util_rail[i]->mortgage(*this);break;}
                        }
                        break;
                    // if mortgaged, only option for rail/util is to unmortgage or exit
                    case player_mortgaged:
                        while(true)
                        {
                            double temp=static_cast<double>(temp_util_rail[i]->mortgage_price);
                            int cost_to_unmortgage = 1.1*temp; 
                            std::cout<<"0 to unmortgage this property for "<<cost_to_unmortgage<<". e to exit: "<<std::endl;
                            result_r_u = useful::request_input({"0","e"});
                            if(result_r_u=="e"){break;}
                            // player having sufficient cash is handled in unmortgage function
                            if(result_r_u=="0"){temp_util_rail[i]->unmortgage(*this,_game);break;}
                        }
                        break;
                }
            }
        }
        if(result=="s")
        {
            // ## Street Level ## 
            // presents the player with only the streets they own
            while(true)
            {
                if(temp_streets.size()==0){std::cout<<"No streets to manage."<<std::endl; break;}
                // with the current structure of classes and members, it is easier to recalculate whether a player has a full set of a certain colour etc. each time it is required, since this could change frequently
                // if each colour was its own class (which would not strictly follow inheritance) they could have a static member for whether the full set was owned or not
                bool can_buy_houses{false};
                for(int i{0}; i<size(temp_streets);i++)
                {
                    std::cout<<i<<" for "<<temp_streets[i]->get_name()<<". ";
                }
                std::cout<<"e to exit. (cash = "<<cash<<")"<<std::endl;
                std::string result_str{useful::request_input(inputs_str)};
                // if e, exit
                if(result_str=="e"){break;}
                // iterate through streets to find street corresponding to number entered
                for(int i{}; i<size(temp_streets);i++)
                {
                    // ## Individual Street Level ## 
                    // can mortgage/unmortgage individual streets, add/sell houses and hotels if able
                    if(inputs_str[i]==result_str)
                    {
                        // find how many of the other owned streets have the same colour as the chosen street
                        int count{0};
                        for(int j{}; j<size(temp_streets);j++)
                        {
                            // mortgaged properties do not count towards a full colour set 
                            if((*temp_streets[j])==temp_streets[i] && temp_streets[j]->state != property_state::player_mortgaged){count++;}
                        }
                        // dark_blues and browns only have 2 properties in a set
                        if(count==2 && ((*temp_streets[i])==set_identifier::dark_blue || (*temp_streets[i])==set_identifier::brown))
                        {
                            can_buy_houses = true;
                        }
                        // the remaining colours all contain 3
                        if(count == 3 && ((*temp_streets[i])==set_identifier::light_blue || (*temp_streets[i])==set_identifier::pink
                                        || (*temp_streets[i])==set_identifier::orange || (*temp_streets[i])==set_identifier::red
                                        || (*temp_streets[i])==set_identifier::yellow || (*temp_streets[i])==set_identifier::green))
                        {
                            can_buy_houses = true;
                        }
                        // if the player cannot afford a house, do not give the option
                        if(cash<temp_streets[i]->house_price){can_buy_houses=false;}
                        std::cout<<temp_streets[i]->name<<": (cash available = "<<cash<<")"<<std::endl;
                        std::string result2;
                        // give different options depending on state of property
                        switch(temp_streets[i]->state)
                        {
                            case player_default:
                                // not mortgaged and has no hotels or houses
                                if(can_buy_houses && temp_streets[i]->houses==0 && !temp_streets[i]->hotel)
                                {
                                    while(true)
                                    {
                                        std::cout<<"0 to mortgage this property for "<<temp_streets[i]->mortgage_price<<". 1 to buy a house for "<<temp_streets[i]->house_price<<". 2 to buy a hotel for "<<temp_streets[i]->hotel_price<<". e to exit: "<<std::endl;
                                        result2 = useful::request_input({"0","1","2","e"});
                                        if(result2=="e"){break;}
                                        if(result2=="0"){temp_streets[i]->mortgage(*this);break;}
                                        // do not break out of loop after buying house to allow multiple purchases at a time
                                        if(result2=="1"){temp_streets[i]->buy_house(*this,_game);}
                                        if(result2=="2"){temp_streets[i]->buy_hotel(*this,_game);break;}
                                    }
                                    break;
                                }
                                // not mortgaged and has at least some houses/hotel
                                if(can_buy_houses && (temp_streets[i]->houses>0 || temp_streets[i]->hotel))
                                {
                                    while(true)
                                    {
                                        std::cout<<"0 to buy a house for "<<temp_streets[i]->house_price<<". 1 to sell a house for "<<temp_streets[i]->house_price/2<<". 2 to buy a hotel for "<<temp_streets[i]->hotel_price<<". 3 to sell a hotel for "<<temp_streets[i]->hotel_price/2<<". e to exit: "<<std::endl;
                                        result2 = useful::request_input({"0","1","2","3","e"});
                                        if(result2=="e"){break;}
                                        // allow repeated selling of house/hotel and buying of houses without exiting loop
                                        if(result2=="0"){temp_streets[i]->buy_house(*this,_game);}
                                        if(result2=="1"){temp_streets[i]->sell_house(*this);}
                                        if(result2=="2"){temp_streets[i]->buy_hotel(*this,_game);break;}
                                        if(result2=="3"){temp_streets[i]->sell_hotel(*this);}
                                    }
                                    break;
                                }
                                // not mortgaged but cannot buy houses and has no houses
                                if(!can_buy_houses && temp_streets[i]->houses==0 && !temp_streets[i]->hotel)
                                {
                                    while(true)
                                    {
                                        std::cout<<"0 to mortgage this property for "<<temp_streets[i]->mortgage_price<<". e to exit: "<<std::endl;
                                        result2 = useful::request_input({"0","e"});
                                        if(result2=="e"){break;}
                                        if(result2=="0"){temp_streets[i]->mortgage(*this);break;}
                                    }
                                    break;
                                }
                                // not mortgaged and has at least some houses/hotel but cannot purchase more
                                if(!can_buy_houses && (temp_streets[i]->houses>0 || temp_streets[i]->hotel))
                                {
                                    while(true)
                                    {
                                        std::cout<<"0 to sell a house for "<<temp_streets[i]->house_price/2<<". 1 to sell a hotel for "<<temp_streets[i]->hotel_price/2<<". e to exit: "<<std::endl;
                                        result2 = useful::request_input({"0","e"});
                                        if(result2=="e"){break;}
                                        if(result2=="0"){temp_streets[i]->sell_house(*this);}
                                        if(result2=="1"){temp_streets[i]->sell_hotel(*this);break;}
                                    }
                                    break;
                                }
                                
                            case player_mortgaged:
                                // mortgaged so only option is to unmortgage it
                                while(true)
                                {
                                    double temp=static_cast<double>(temp_streets[i]->mortgage_price);
                                    int cost_to_unmortgage = 1.1*temp; 
                                    std::cout<<"0 to unmortgage this property for "<<cost_to_unmortgage<<". e to exit: "<<std::endl;
                                    result2 = useful::request_input({"0","e"});
                                    if(result2=="e"){break;}
                                    if(result2=="0"){temp_streets[i]->unmortgage(*this,_game);break;}
                                }
                        }
                    }
                }
            }
        }
    }
}
