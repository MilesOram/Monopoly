#include"square.h"

int square::get_index(){return board_index;}
void square::print_name(){std::cout<<name<<std::endl;}
void square::change_name(std::string a){name=a;}
std::string square::get_name(){return name;}

void property::change_property_state(property_state new_state){state = new_state;}
property_state property::get_state(){return state;}
set_identifier property::get_identifier(){return identifier;}
int property::amount_to_pay(player &_owner){return 11;}




// property constructor
property::property(int _cost, int _board_index, int _mortgage, std::string _name, set_identifier _identifier): square{_board_index, _name}, identifier{_identifier}, cost{_cost}, mortgage_price{_mortgage}{}

// mortgage a property - gain cash equal to mortgage price then mortgage property
// mortgaged properties do not count towards owning a full set or modifying rail or utility rent values
void property::mortgage(player &_player)
{
    if(state==property_state::player_mortgaged){std::cout<<name<<" is already mortgaged.";}
    else
    {
        std::cout<<"Mortgaging "<<name<<std::endl;
        _player.add_cash(mortgage_price);
        state=property_state::player_mortgaged;
    }
}

// unmortgage a property - pay cash equal to 1.1 x mortgage price (10% tax to unmortgage) and unmortgage the property
void property::unmortgage(player &_player, gamestate &_game)
{
    if(state==property_state::player_default){std::cout<<name<<" is not mortgaged.";}
    else
    {
        double temp=static_cast<double>(mortgage_price);
        int cost_to_unmortgage = 1.1*temp; 
        if(_player.get_cash()<cost_to_unmortgage){std::cout<< "Not enough cash"<<std::endl;}
        else
        {   
            std::cout<<"Unmortgaging "<<name<<" for "<<cost_to_unmortgage<<std::endl;
            _player.pay_bank(cost_to_unmortgage, _game, false);
            state=property_state::player_default;
        }
    }
}

// executed when landing on any property square
void property::landed_on(gamestate &_game)
{
    switch(state)
    {
        int temp_to_pay;
        case bank_default:
            // if player cannot afford the property, do not offer choice to buy
            if(cost>_game.get_current_player()->get_cash()){std::cout<<"Cannot afford this property at "<<cost<<std::endl; break;}
            // request input from player for decision to buy
            if(option_to_buy(_game))
            {
                // add property to player class property vector and take cash from player
                std::shared_ptr<property> _p(_game.get_property_pointer(name));
                _game.get_current_player()->pay_bank(cost, _game, false);
                _game.get_current_player()->add_property(_p);
            }
            break;
        case player_mortgaged:
            std::cout<<name<< " is mortgaged."<<std::endl;
            break;
        case player_default:
            // property is owned by a player and unmortgaged
            if(_game.get_owner(*this)==nullptr){std::cout<<"cannot find owner"<<std::endl;break;}
            // if this player owns the property do nothing
            if(_game.get_owner(*this)->get_name()==_game.get_current_player()->get_name()){std::cout<<"You own this property."<<std::endl;break;}
            // otherwise pay the owner the required amount
            temp_to_pay = amount_to_pay(*_game.get_owner(*this));
            std::cout<<"Amount to pay: "<<temp_to_pay<<std::endl;
            _game.get_current_player()->pay_player(*_game.get_owner(*this), temp_to_pay,_game);
            break;
        case bank_mortgaged:
            if(cost>_game.get_current_player()->get_cash()){std::cout<<"Cannot afford this property at "<<cost<<std::endl; break;}

            if(option_to_buy(_game))
            {
                std::shared_ptr<property> _p(_game.get_property_pointer(name));
                // buy the property for the amount it would cost to unmortgage it plus the usual cost    and add it to the player's properties in an unmortgaged state
                double ammended_price = static_cast<double>(mortgage_price);
                ammended_price*=1.1;
                ammended_price+=cost;
                _game.get_current_player()->pay_bank(ammended_price, _game, false);
                _game.get_current_player()->add_property(_p);
            }
            break;
    }
}

// request input from the player (either y or n) for whether they want to buy something
// returns their choice as a bool
// if player is ai, buy the property if they own 3 or fewer properties or if cash>800, if they own more, only buy if part of set currently owned
bool property::option_to_buy(gamestate &_game)
{
    if(_game.get_current_player()->is_ai())
    {
        bool should_ai_buy{false};
        std::vector<std::shared_ptr<property>> temp = _game.get_current_player()->get_properties_owned();
        if(temp.size()<4 || _game.get_current_player()->get_cash()>800)
        {
            std::cout<<"Player is buying "<<name<<std::endl; 
            return true;
        }
        else
        {
            // count number of properties owned by owner of type railway
            for(auto itr{temp.begin()};itr<temp.end(); itr++)
            {
                if(identifier == (*itr)->get_identifier()){should_ai_buy = true; break;}
            }
        }
        if(should_ai_buy){return true;}
        else{return false;}
    }
    else
    {
        std::cout<<"Cash: "<<_game.get_current_player()->get_cash()<<std::endl;
        std::cout<<"Would you like to buy "<<name<<" for "<<cost<<"? y/n"<<std::endl;
        while(true)
        {
            std::string result{useful::request_input({"y","n"})};
            if(result=="n") {return false;}
            if(result=="y") {return true;}
        }
    }

}
// use identifier to determine if street, friend of property class
bool is_street(property &_property)
{
    if(_property.identifier==set_identifier::brown || _property.identifier==set_identifier::light_blue 
         || _property.identifier==set_identifier::pink   || _property.identifier==set_identifier::orange 
         || _property.identifier==set_identifier::red    || _property.identifier==set_identifier::yellow 
         || _property.identifier==set_identifier::green  || _property.identifier==set_identifier::dark_blue)
    {return true;}
    else{return false;}
}

// calculate amount to pay based on number of hotels/houses, get from rent tiers
int street::amount_to_pay(player &_owner)
{
    if(hotel){return rent_tiers[5];}
    else{return rent_tiers[houses];}
}
// prints out the tiers and houses, hotel for a street
void street::get_tiers()
{
    std::cout<< rent_tiers[0];
    for(size_t i{1}; i<6;i++) std::cout<<", " <<rent_tiers[i];
    std::cout<<", Houses: "<<houses<<", Hotel: "<<hotel<<std::endl;
}

// buy a house provided correct conditions
void street::buy_house(player &_owner, gamestate &_game)
{
    // last minute bug where street had 4 houses and 1 hotel, might have been caused by spamming inputs, haven't been able to reproduce, will break player out of loop if need to sell stuff to pay rent
    if(hotel){houses=0;}
    if(houses<4 && !hotel)
    {
        if(_owner.get_cash()<house_price){std::cout<<"Not enough cash to buy a house."<<std::endl; return;}
        _owner.pay_bank(house_price, _game, false);
        houses+=1;
        std::cout<<name<<" has "<<houses<<" house(s) now."<<std::endl;
    }
    else
    {
        std::cout<<"A street must have fewer than 4 houses and no hotel in order to add a house."<<std::endl;
    }
}
// buy a hotel provided correct conditions
void street::buy_hotel(player &_owner, gamestate &_game)
{
    if(_owner.get_cash()<hotel_price){std::cout<<"Not enough cash to buy a hotel."<<std::endl; return;}
    if(houses==4 && !hotel)
    {
        _owner.pay_bank(hotel_price, _game, false);
        houses=0;
        hotel = true;
        std::cout<<name<<" has a hotel now."<<std::endl;
    }
    else
    {
        std::cout<<"A street must have exactly 4 houses and no hotel in order to add a hotel."<<std::endl;
    }

}
// sell a house provided correct conditions
void street::sell_house(player &_owner)
{
    // last minute bug where street had 4 houses and 1 hotel, might have been caused by spamming inputs, haven't been able to reproduce, will break player out of loop if need to sell stuff to pay rent
    if(hotel){houses=0;}
    if(houses>0 && !hotel)
    {
        _owner.add_cash(house_price/2);
        houses-=1;
        std::cout<<name<<" has "<<houses<<" house(s) now."<<std::endl;
    }
    else
    {
        std::cout<<"A street must have more than 0 houses and no hotel in order to sell a house."<<std::endl;
    }
}
// sell a hotel provided correct conditions
void street::sell_hotel(player &_owner)
{
    if(houses==0 && hotel)
    {
        _owner.add_cash(house_price/2);
        hotel = true;
        houses = 4;
        std::cout<<name<<" has "<<houses<<" house(s) now."<<std::endl;
    }
    else
    {
        std::cout<<"A street must have 0 houses and a hotel in order to sell a hotel."<<std::endl;
    }
}
// overloaded mortgage specific to streets - sells all houses and removes them and mortgages the property
void street::mortgage(player &_player)
{
    if(state==property_state::player_mortgaged){std::cout<<name<<" is already mortgaged.";}
    else
    {
        std::cout<<"Mortgaging "<<name<<std::endl;
        int cash_to_add{mortgage_price};
        cash_to_add+=houses*house_price/2;
        cash_to_add+=hotel*hotel_price/2;
        houses=0;
        hotel=false;
        _player.add_cash(cash_to_add);
        state=property_state::player_mortgaged;
    }
}
// street constructor 
street::street(int tiers[6], int _cost, int _board_index, int _mortgage, std::string _name,
                     set_identifier _colour, int _house_price, int _hotel_price):
                     house_price{_house_price}, hotel_price{_hotel_price}, property{_cost, _board_index, _mortgage, _name, _colour}
{
    rent_tiers = new int[6];
    for(size_t i{}; i<6;i++) rent_tiers[i] = tiers[i];
}

// overload == to check if a property is a street
bool street::operator==(std::shared_ptr<property> _property)
{
    return (is_street(*_property));
}
// overload == to check if a street is the same colour as this street
bool street::operator==(std::shared_ptr<street> _street)
{
    return (identifier== _street->get_identifier());
}
// overload == to check if this street is a certain colour
bool street::operator==(set_identifier _id)
{
    return (identifier== _id);
}

void railway::get_tiers(){std::cout<< rent_tiers[0];for(size_t i{1}; i<4;i++) std::cout<<", " <<rent_tiers[i];std::cout<<std::endl;}


// overload == to check if a property is a railway
bool railway::operator==(std::shared_ptr<property> _property)
{
    return (set_identifier::railways == _property->get_identifier());
}
// calculate amount to pay when landing on a railway, depends on the number of other railways owned by the owner of this railway 
int railway::amount_to_pay(player &_owner)
{
    int count{0};
    std::vector<std::shared_ptr<property>> temp = _owner.get_properties_owned();
    // count number of properties owned by owner of type railway
    for(auto itr{temp.begin()};itr<temp.end(); itr++)
    {
        if((*this)==(*itr)){count++;}
    }
    if(count<1){std::cout<<"Incorrectly calculated rent to pay"<<std::endl;return 0;}
    return rent_tiers[count-1];
}
// initialize static rent tiers
const int railway::rent_tiers[] = {25,50,100,200};

void utility::get_tiers(){std::cout<< "4x dice roll, 10x dice roll"<<std::endl;}


// overload == to check if a property is a utility
bool utility::operator==(std::shared_ptr<property> _property)
{
    return (set_identifier::utilities == _property->get_identifier());
}
// calculate amount to pay when landing on a utility, depends on the number of other railways owned by the owner of this utility 
// rent is calculated by rolling two dice and multiplying this number by the relevant multiplier
int utility::amount_to_pay(player &_owner)
{
    int count{0};
    std::vector<std::shared_ptr<property>> temp = _owner.get_properties_owned();
    // count number of properties owned by owner of type utility
    for(auto itr{temp.begin()};itr<temp.end(); itr++)
    {
        if((*this)==(*itr)){count++;}
    }
    if(count==1){return base_multiplier*(useful::random_int(1,6)+useful::random_int(1,6));}
    if(count==2){return set_multiplier*(useful::random_int(1,6)+useful::random_int(1,6));}
    else{std::cout<<"Incorrectly calculated rent to pay"<<std::endl;return 0;}
}
std::shared_ptr<std::shared_ptr<square>[]> board(new std::shared_ptr<square>[40]);

