#include"game.h"

gamestate::gamestate(int players, std::map<std::string,std::shared_ptr<property>> _property_list): property_list{_property_list}
{
    if(players>0)
    {
        auto p1 = std::make_shared<player>(false, "Player 1",1);
        all_players.push_back(p1);
    }
    if(players>1)
    {
        auto p2 = std::make_shared<player>(true, "Player 2",2);
        all_players.push_back(p2);
    }

    if(players>2)
    {
        auto p3 = std::make_shared<player>(true, "Player 3",3);
        all_players.push_back(p3);
    }
    if(players>3)
    {
        auto p4 = std::make_shared<player>(true, "Player 4",4);
        all_players.push_back(p4);
    }
    current_player=0;
}

void gamestate::determine_order()
{
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(std::begin(all_players), std::end(all_players), g);
    std::cout<<"The player order is: "<<std::endl;
    std::for_each(all_players.begin(), all_players.end(), [&] (std::shared_ptr<player> &p) {std::cout<<p->get_name()<<std::endl;});
}

std::shared_ptr<player> gamestate::get_owner(property &_property)
{
    for(auto p_itr{all_players.begin()};p_itr<all_players.end(); p_itr++)
    {
        for(auto itr{(*p_itr)->properties_owned.begin()};itr<(*p_itr)->properties_owned.end(); itr++)
        {
            if((**itr).get_name()==_property.get_name()){return (*p_itr);}
        }
    }
    return nullptr;
}

void gamestate::end_turn()
{
    get_current_player()->doubles_rolled=0;
    update_to_next_player();
}

void gamestate::send_to_jail()
{
    get_current_player()->in_jail=true;
    std::cout<<"Sending to jail..."<<std::endl;
    get_current_player()->move_to_square(10,false,*this);
}

void gamestate::execute_turn(std::shared_ptr<std::shared_ptr<square>[]> &_board)
{
    player_out_this_turn=false;
    if(print_board_every_turn){std::cout<<visuals::ascii;}
    else
    {
        if(!get_current_player()->ai){std::cout<<visuals::ascii;}
    }
    std::cout<<"######### "<<get_current_player()->get_name()<<"'s Turn #########"<<std::endl;
    if(get_current_player()->get_jail_bool()){get_current_player()->handle_jail(*this);}
    else
    {
        get_current_player()->roll(*this);
        if(player_out_this_turn){return;}
        while(get_current_player()->get_doubles()>0)
        {
            if(player_out_this_turn){return;}
            if(get_current_player()->in_jail){break;}
            get_current_player()->roll(*this);
            if(get_current_player()->get_doubles()>2) {send_to_jail(); break;}
        }
        if(!get_current_player()->ai)
        {
            if(player_out_this_turn){return;}
            std::cout<<"Manage properties? y/n"<<std::endl;
            while(true)
            {
                std::string result{useful::request_input({"y","n"})};
                if(result=="n") break;
                if(result=="y") {get_current_player()->manage_properties(*this); break;}
            }
        }
        else
        {
            if(detailed_ai_props){get_current_player()->show_properties_extended();}
            if(player_out_this_turn){return;}
            std::vector<std::shared_ptr<property>> temp_props{get_current_player()->properties_owned};
            std::map<set_identifier, int> full_sets_check;
            for(int i{}; i<size(temp_props);i++)
            {
                // mortgaged properties do not count towards a full colour set 
                if(temp_props[i]->get_state()!=property_state::player_mortgaged)
                {
                    full_sets_check[temp_props[i]->get_identifier()]+=1;
                }
                else
                {
                    if(get_current_player()->cash>500){temp_props[i]->unmortgage(*get_current_player(), *this);}
                }
            }
            std::vector<set_identifier> _colour;
            //_colour.push_back(dark_blue);
            if(full_sets_check[set_identifier::dark_blue]==2){_colour.push_back(dark_blue);}
            if(full_sets_check[set_identifier::brown]==2){_colour.push_back(brown);}
            if(full_sets_check[set_identifier::light_blue]==3){_colour.push_back(light_blue);}
            if(full_sets_check[set_identifier::pink]==3){_colour.push_back(pink);}
            if(full_sets_check[set_identifier::orange]==3){_colour.push_back(orange);}
            if(full_sets_check[set_identifier::red]==3){_colour.push_back(red);}
            if(full_sets_check[set_identifier::yellow]==3){_colour.push_back(yellow);}
            if(full_sets_check[set_identifier::green]==3){_colour.push_back(green);}
            for(int i{}; i<size(temp_props);i++)
            {
                if(get_current_player()->cash > 300 && is_street(*temp_props[i]))
                {
                    if (std::find(_colour.begin(), _colour.end(), temp_props[i]->get_identifier()) != _colour.end())
                    {
                        auto s = std::static_pointer_cast<street>(temp_props[i]);
                        if(s->get_houses()==4 && !s->get_hotel())
                        {
                            s->buy_hotel(*get_current_player(), *this);
                        }
                        if(s->get_houses()<4 && !s->get_hotel())
                        {
                            s->buy_house(*get_current_player(), *this);
                        }
                    }
                }

                if(is_street(*temp_props[i]))
                {
                    
                    full_sets_check[temp_props[i]->get_identifier()]+=1;
                }
                
            }
        }
    }
}
int gamestate::get_and_reset_cash_pool()
{
    int temp{cash_pool};
    cash_pool=0;
    return temp;
}
const void gamestate::all_prop_info()
{
    std::cout<<"All Property info for reference: "<<std::endl;
    // print name, set_identifier, rent_tiers (and number of houses if it's a street)
    for(auto itr = property_list.begin(); itr != property_list.end(); ++itr)
    {
        std::cout<< (*itr).second->get_name()<<": "<<(*itr).second->get_identifier()<<", Rent tiers: ";
        (*itr).second->get_tiers();
    }
}
void gamestate::remove_current_player_from_game()
{
    all_players.erase(std::remove(all_players.begin(), all_players.end(), all_players[current_player]), all_players.end());
    current_player = current_player % all_players.size();
    player_out_this_turn=true;
}