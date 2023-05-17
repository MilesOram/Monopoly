// C++ OOP Project 2022
// PHYS 30762 
#include "visuals.h"
#include "base.h"
#include "extra_squares.h"
#include "game.h"
#include "player.h"
#include "square.h"

// abstract base class containing the text and functions of individual cards -> chance and community chest
// this parallels the physical cards that would exist in the middle of the board in monopoly
// this class provides a scalable way to make new cards using lambda functions
class real_card
{
    protected:
        std::function<void (gamestate &_game)> card_function;
        std::string card_text;
    public:
        real_card(std::function<void (gamestate &_game)> func, std::string _card_text): card_function{func}, card_text{_card_text}{}
        virtual ~real_card(){};
        virtual void effect(gamestate &_game)=0;
        std::string get_text(){return card_text;}
};
// physical chance card
class chance_card : public real_card
{        
    public:
        void effect(gamestate &_game){std::cout<<"Chance card: "<<get_text()<<std::endl; card_function(_game);}
        chance_card(std::function<void (gamestate &_game)> func, std::string _card_text): real_card{func, _card_text}{};
        ~chance_card(){};
};
// physical community chest card
class community_chest_card : public real_card
{
    public:
        void effect(gamestate &_game){std::cout<<"Community chest card: "<<get_text()<<std::endl; card_function(_game);}
        community_chest_card(std::function<void (gamestate &_game)> func, std::string _card_text): real_card{func, _card_text}{};
        ~community_chest_card(){};
};
// template used for chance square and community chest card squares
// this is a class for the card squares on the board and contain the static member for the (physical) chance and community chest cards
// the two classes which come from this are card_square<chance_card> and card_square<community_chest_card> which each have their respective list of real, chance and community chest cards 
template <class c_type> class card_square : public square
{
    static_assert(std::is_base_of<real_card, c_type>::value, "type parameter of this class must derive from real_card");
    private:
        // vector of chance or community chest cards
        static std::vector<c_type> all_cards;
    public:
        card_square(int _board_index, std::string _name): square{_board_index, _name}{}
        card_square(int _board_index, gamestate &_game, int intialize_static, std::string _name);
        ~card_square(){};
        void shuffle();
        void landed_on(gamestate &_game);
        void move_top_card_to_bottom();
};  
template<class c_type> card_square<c_type>::card_square(int _board_index, gamestate &_game, int intialize_static, std::string _name): square{_board_index, _name}
{
    // Not sure of the exact best way to initialize static members in templates like this, opted to use the input 
    // variable 'initialize_static' which would be 0 for the first chance square made and 1 for the first community chest square
    // the remaining card sqaures made would just take the board index
    if(size(all_cards)==0 && intialize_static==0)
    {
        // CHANCE CARDS
        all_cards.push_back(c_type([&] (gamestate &_game) {_game.get_current_player()->add_cash(50);}, "Bank pays you dividend of $50."));
        all_cards.push_back(c_type([&] (gamestate &_game) {_game.get_current_player()->pay_bank(15, _game, true);}, "Pay speeding fine of $15."));
        all_cards.push_back(c_type([&] (gamestate &_game) {_game.get_current_player()->move_squares(-3,false,_game);}, "Go back 3 spaces."));
        all_cards.push_back(c_type([&] (gamestate &_game) {_game.get_current_player()->move_to_square(0,true,_game);}, "Advance to Go."));
        all_cards.push_back(c_type([&] (gamestate &_game) {_game.get_current_player()->move_to_square(39,true,_game);}, "Advance to Mayfair."));
        all_cards.push_back(c_type([&] (gamestate &_game) {int index{_game.get_current_player()->get_current_square()};
                    if(index>35 || index<5) index=5;
                    if(index>5 && index<15) index=15;
                    if(index>15 && index<25) index=25;
                    if(index>25 && index<35) index=35;
                    _game.get_current_player()->move_to_square(index,true,_game);}, "Advance to nearest Station."));
                    
        all_cards.push_back(c_type([&] (gamestate &_game) {int index{_game.get_current_player()->get_current_square()};
                    if(index>12 && index<28) index=28;
                    if(index>28 || index<12) index=12;
                    _game.get_current_player()->move_to_square(index,true,_game);},"Advance to nearest Utility."));

        all_cards.push_back(c_type([&] (gamestate &_game) {
                        std::vector<std::shared_ptr<player>> temp_players{_game.get_all_players()};
                        for(auto itr{temp_players.begin()}; itr<temp_players.end(); itr++)
                        {
                            if((*itr)->get_name()!=(_game.get_current_player()->get_name())){_game.get_current_player()->pay_player((**itr),50,_game);}
                        }
                    
                    }, "You have been elected Chairman of the Board. Pay each player $50"));
        all_cards.push_back(c_type([&] (gamestate &_game) {_game.get_current_player()->move_to_square(11,true,_game);}, "Advance to Pall Mall."));
        all_cards.push_back(c_type([&] (gamestate &_game) {_game.send_to_jail();}, "Go to Jail. Go directly to Jail, do not pass Go, do not collect $200."));
        all_cards.push_back(c_type([&] (gamestate &_game) {_game.get_current_player()->add_get_out_of_jail_card();}, "Get Out of Jail Free. Use to get out of jail."));
        all_cards.push_back(c_type([&] (gamestate &_game) {
                        int total_houses{0};
                        int total_hotel{0};
                        std::vector<std::shared_ptr<property>> _props = _game.get_current_player()->get_properties_owned();
                        for(auto itr{_props.begin()}; itr<_props.end(); itr++)
                        {
                            if(is_street(**itr))
                            {
                                auto s = std::static_pointer_cast<street>(*itr);
                                total_houses+= s->get_houses();
                                total_hotel+= s->get_hotel();
                            }
                        }
                        _game.get_current_player()->pay_bank(total_houses*25+total_hotel*100, _game, true);                    
                    }, "Make general repairs on all your property. For each house pay $25. For each hotel pay $100."));
        shuffle();
    }
    if(size(all_cards)==0 && intialize_static==1)
    {
        // COMMUNITY CHEST CARDS
        all_cards.push_back(c_type([&] (gamestate &_game) {_game.get_current_player()->move_to_square(0,true,_game);}, "Advance to Go."));
        all_cards.push_back(c_type([&] (gamestate &_game) {_game.get_current_player()->add_cash(200);}, "Bank error in your favour. Collect $200."));
        all_cards.push_back(c_type([&] (gamestate &_game) {_game.get_current_player()->add_cash(100);}, "You inherit $100."));
        all_cards.push_back(c_type([&] (gamestate &_game) {_game.get_current_player()->add_cash(10);}, "You have won second prize in a beauty contest. Collect $10."));
        all_cards.push_back(c_type([&] (gamestate &_game) {_game.get_current_player()->pay_bank(50, _game, true);}, "Pay school fees of $50."));
        all_cards.push_back(c_type([&] (gamestate &_game) {_game.get_current_player()->pay_bank(100, _game, true);}, "Pay hospital fees of $100."));
        all_cards.push_back(c_type([&] (gamestate &_game) {_game.get_current_player()->add_get_out_of_jail_card();}, "Get Out of Jail Free. Use to get out of jail."));
        all_cards.push_back(c_type([&] (gamestate &_game) {
                        int total_houses{0};
                        int total_hotel{0};
                        std::vector<std::shared_ptr<property>> _props = _game.get_current_player()->get_properties_owned();
                        for(auto itr{_props.begin()}; itr<_props.end(); itr++)
                        {
                            if(is_street(**itr))
                            {
                                auto s = std::static_pointer_cast<street>(*itr);
                                total_houses+= s->get_houses();
                                total_hotel+= s->get_hotel();
                            }
                        }
                        _game.get_current_player()->pay_bank(total_houses*40+total_hotel*115, _game, true);                    
                    }, "You are assessed for street repairs. $40 per house. $115 per hotel."));
        shuffle();
    }
}
// equivalent to putting the chance/community chest card on the bottom of the pile after drawing it
template<class c_type> void card_square<c_type>::move_top_card_to_bottom()
{
    std::rotate(all_cards.begin(), all_cards.begin() + 1, all_cards.end());
}
// formally initialize static member
template<class c_type> std::vector<c_type> card_square<c_type>::all_cards{};

template<class c_type> void card_square<c_type>::shuffle()
{
    // make sufficiently random shuffle
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(std::begin(all_cards), std::end(all_cards), g);
}
template<class c_type> void card_square<c_type>::landed_on(gamestate &_game)
{
    // execute the effect of the card on top of the 'pile' of cards
    all_cards[0].effect(_game);
    // then put that card on the bottom of the 'pile'
    move_top_card_to_bottom();
}




namespace useful
{
    template <class T> std::shared_ptr<property> make_property_pointer(const std::shared_ptr<T> &p){std::shared_ptr<property> prop_pointer = p; return prop_pointer;}
    template <class T> void place_in_board(std::shared_ptr<std::shared_ptr<square>[]> &_board, std::shared_ptr<T> &sq)
    {
        // make copy of pointer, add to board, reset old pointer
        auto temp(sq);
        _board[sq->get_index()] = temp;
        sq.reset();
    }
};




// ## RENT TIERS FOR STREETS ##
// there are two rent structures per colour, for colour sets of two (brown and dark blue), the first street has the first rent structure and the second has the second
// for colour sets of three, the first and second street have the first rent structure and the third street has the second structure
//brown
int arr_b1[6] = {2,10,30,90,160,250};
int arr_b2[6] = {4,20,60,180,320,450};
//light blue
int arr_lb1[6] = {6,30,90,270,400,550};
int arr_lb2[6] = {8,40,100,300,450,600};
//pink
int arr_p1[6] = {10,50,150,450,625,750};
int arr_p2[6] = {12,60,180,500,700,900};
//orange
int arr_o1[6] = {14,70,200,550,750,950};
int arr_o2[6] = {16,80,220,600,800,1000};
//red
int arr_r1[6] = {18,90,250,700,875,1050};
int arr_r2[6] = {20,100,300,750,925,1100};
//yellow
int arr_y1[6] = {22,110,330,800,975,1150};
int arr_y2[6] = {22,120,360,850,1025,1200};
//green
int arr_g1[6] = {26,130,390,900,1100,1275};
int arr_g2[6] = {28,150,450,1000,1200,1400};
//dark blue
int arr_db1[6] = {35,175,500,1100,1300,1500};
int arr_db2[6] = {50,200,600,1400,1700,2000};

int main()
{

    // ### MAKING SHARED_PTR TO DIFFERENT SQUARES ###
    //(board index, name, identifier)
    //utility
    auto water_works = std::make_shared<utility>(28, "Water Works", set_identifier::utilities);
    auto electric_company = std::make_shared<utility>(12, "Electric Company", set_identifier::utilities);

    //railway
    auto kings_cross_station = std::make_shared<railway>(5,"King's Cross Station", set_identifier::railways);
    auto marylebone_station = std::make_shared<railway>(15,"Marylebone Station", set_identifier::railways);
    auto fenchurch_st_station = std::make_shared<railway>(25,"Fenchurch St Station", set_identifier::railways);
    auto liverpool_street_station = std::make_shared<railway>(35,"Liverpool Street Station", set_identifier::railways);

    // STREETS
    //(rent_tiers, cost, board index, mortgage price, name, colour, house price, hotel price)
    //brown
    auto old_kent_road = std::make_shared<street>(arr_b1,60,1,30,"Old Kent Road",set_identifier::brown,50,50);
    auto whitechapel_road = std::make_shared<street>(arr_b2,60,3,30,"Whitechapel Road",set_identifier::brown,50,50);
    //light blue
    auto the_angel_islington = std::make_shared<street>(arr_lb1,100,6,50,"The Angel Islington",set_identifier::light_blue,50,50);
    auto euston_road = std::make_shared<street>(arr_lb1,100,8,50,"Euston Road",set_identifier::light_blue,50,50);
    auto pentonville_road = std::make_shared<street>(arr_lb2,120,9,60,"Pentonville Road",set_identifier::light_blue,50,50);
    //pink
    auto pall_mall = std::make_shared<street>(arr_o1,140,11,70,"Pall Mall",set_identifier::pink,100,100);
    auto whitehall = std::make_shared<street>(arr_p1,140,13,70,"Whitehall",set_identifier::pink,100,100);
    auto northumberland_avenue = std::make_shared<street>(arr_p2,160,14,80,"Northumberland Avenue",set_identifier::pink,100,100);
    //orange
    auto bow_street = std::make_shared<street>(arr_o1,180,16,90,"Bow Street",set_identifier::orange,100,100);
    auto marlborough_street = std::make_shared<street>(arr_o1,180,18,90,"Marlborough Street",set_identifier::orange,100,100);
    auto vine_street = std::make_shared<street>(arr_o2,200,19,100,"Vine Street",set_identifier::orange,100,100);
    //red
    auto the_strand = std::make_shared<street>(arr_r1,220,21,110,"The Strand",set_identifier::red,150,150);
    auto fleet_street = std::make_shared<street>(arr_r1,220,23,110,"Fleet Street",set_identifier::red,150,150);
    auto trafalgar_square = std::make_shared<street>(arr_r2,240,24,120,"Trafalgar Square",set_identifier::red,150,150);
    //yellow
    auto leicester_square = std::make_shared<street>(arr_y1,260,26,130,"Leicester Square",set_identifier::yellow,150,150);
    auto coventry_street = std::make_shared<street>(arr_y1,260,27,130,"Coventry Street",set_identifier::yellow,150,150);
    auto piccadilly = std::make_shared<street>(arr_y2,280,29,140,"Piccadilly",set_identifier::yellow,200,200);
    //green
    auto regent_street = std::make_shared<street>(arr_g1,300,31,150,"Regent Street",set_identifier::green,200,200);
    auto oxford_street = std::make_shared<street>(arr_g1,300,32,150,"Oxford Street",set_identifier::green,200,200);
    auto bond_street = std::make_shared<street>(arr_g2,320,34,160,"Bond Street",set_identifier::green,200,200);
    //dark blue
    auto park_lane = std::make_shared<street>(arr_db1,350,37,175,"Park Lane",set_identifier::dark_blue,200,200);
    auto mayfair = std::make_shared<street>(arr_db2,400,39,200,"Mayfair",set_identifier::dark_blue,200,200);

    //extras
    auto go_sq = std::make_shared<go>(0);
    auto jail_sq = std::make_shared<jail>(10);
    auto free_parking_sq = std::make_shared<free_parking>(20);
    auto go_to_jail_sq = std::make_shared<go_to_jail>(30);
    auto income_tax = std::make_shared<tax>(200,4,"Income Tax");
    auto super_tax = std::make_shared<tax>(100,38,"Super Tax");


    int number_of_players{3};
    bool skip{false};
    if(!skip)
    {
        std::cout<<"How many players?: 2,3 or 4 (there will always be 1 user-controlled player and between 1 and 3 ai players)"<<std::endl;
        while(true)
        {
            std::string result{useful::request_input({"2","3","4"})};
            if(result=="2") {number_of_players=2; break;}
            if(result=="3") {number_of_players=3; break;}
            if(result=="4") {number_of_players=4; break;}
        }
        std::cout<<"Would you like the board to be printed at the start of each player's turn (y) or just your own (n)?: "<<std::endl;
        while(true)
        {
            std::string result{useful::request_input({"y","n"})};
            if(result=="y") {print_board_every_turn=true; break;}
            if(result=="n") {print_board_every_turn=false; break;}
        }
        std::cout<<"Would you like to see detailed ai player property information at the end of each of their turns? y/n"<<std::endl;
        while(true)
        {
            std::string result{useful::request_input({"y","n"})};
            if(result=="y") {detailed_ai_props=true; break;}
            if(result=="n") {detailed_ai_props=false; break;}
        }
        std::cout<<"Would you like players to start the game with some properties (specifically you would start with all pink streets)? y/n"<<std::endl;
        while(true)
        {
            std::string result{useful::request_input({"y","n"})};
            if(result=="y") {start_with_props=true; break;}
            if(result=="n") {start_with_props=false; break;}
        }
    }
    
    
    std::map<std::string,std::shared_ptr<property>> property_pointers{};

    // make and store pointers to properties in the game class
    // the inheritance structure can be a little confusing here, but i like to think of these pointers as the physical property cards in the game
    // the property cards that a player has in monopoly are in some respects just acting like a pointer to that square on the board and displaying its info
    // these pointers act in much the same way except copies are handed out to the player when they purchase the property and added to their property list
    property_pointers[old_kent_road->get_name()] = useful::make_property_pointer(old_kent_road);
    property_pointers[whitechapel_road->get_name()] = useful::make_property_pointer(whitechapel_road);
    property_pointers[the_angel_islington->get_name()] = useful::make_property_pointer(the_angel_islington);
    property_pointers[euston_road->get_name()] = useful::make_property_pointer(euston_road);
    property_pointers[pentonville_road->get_name()] = useful::make_property_pointer(pentonville_road);
    property_pointers[pall_mall->get_name()] = useful::make_property_pointer(pall_mall);
    property_pointers[whitehall->get_name()] = useful::make_property_pointer(whitehall);
    property_pointers[northumberland_avenue->get_name()] = useful::make_property_pointer(northumberland_avenue);
    property_pointers[bow_street->get_name()] = useful::make_property_pointer(bow_street);
    property_pointers[marlborough_street->get_name()] = useful::make_property_pointer(marlborough_street);
    property_pointers[vine_street->get_name()] = useful::make_property_pointer(vine_street);
    property_pointers[the_strand->get_name()] = useful::make_property_pointer(the_strand);
    property_pointers[fleet_street->get_name()] = useful::make_property_pointer(fleet_street);
    property_pointers[trafalgar_square->get_name()] = useful::make_property_pointer(trafalgar_square);
    property_pointers[leicester_square->get_name()] = useful::make_property_pointer(leicester_square);
    property_pointers[coventry_street->get_name()] = useful::make_property_pointer(coventry_street);
    property_pointers[piccadilly->get_name()] = useful::make_property_pointer(piccadilly);
    property_pointers[regent_street->get_name()] = useful::make_property_pointer(regent_street);
    property_pointers[oxford_street->get_name()] = useful::make_property_pointer(oxford_street);
    property_pointers[bond_street->get_name()] = useful::make_property_pointer(bond_street);
    property_pointers[park_lane->get_name()] = useful::make_property_pointer(park_lane);
    property_pointers[mayfair->get_name()] = useful::make_property_pointer(mayfair);
    property_pointers[kings_cross_station->get_name()] = useful::make_property_pointer(kings_cross_station);
    property_pointers[marylebone_station->get_name()] = useful::make_property_pointer(marylebone_station);
    property_pointers[fenchurch_st_station->get_name()] = useful::make_property_pointer(fenchurch_st_station);
    property_pointers[liverpool_street_station->get_name()] = useful::make_property_pointer(liverpool_street_station);
    property_pointers[electric_company->get_name()] = useful::make_property_pointer(electric_company);
    property_pointers[water_works->get_name()] = useful::make_property_pointer(water_works);

    gamestate game(number_of_players, property_pointers);
    property_pointers.clear();
    // make the chance and community chest squares (have to be made after the gamestate is constructed as its used for the lambda functions stored in each real_card class)
    // the first community chest and chance square made are using the alternative constructor for the template class
    // they alter the static all_cards member and effectively initialize it (although not technically initialize)
    auto community_chest_1 = std::make_shared<card_square<community_chest_card>>(2,game,1,"Community Chest");
    auto chance_card_1 = std::make_shared<card_square<chance_card>>(7,game,0,"Chance");

    auto community_chest_2 = std::make_shared<card_square<community_chest_card>>(17,"Community Chest");
    auto chance_card_2 = std::make_shared<card_square<chance_card>>(22,"Chance");
    auto community_chest_3 = std::make_shared<card_square<community_chest_card>>(33,"Community Chest");
    auto chance_card_3 = std::make_shared<card_square<chance_card>>(36,"Chance");   

    // place all cards in board
    useful::place_in_board(board, go_sq);
    useful::place_in_board(board, old_kent_road);
    useful::place_in_board(board, community_chest_1);
    useful::place_in_board(board, whitechapel_road);
    useful::place_in_board(board, income_tax);
    useful::place_in_board(board, kings_cross_station);
    useful::place_in_board(board, the_angel_islington);
    useful::place_in_board(board, chance_card_1);
    useful::place_in_board(board, euston_road);
    useful::place_in_board(board, pentonville_road);
    useful::place_in_board(board, jail_sq);
    useful::place_in_board(board, pall_mall);
    useful::place_in_board(board, electric_company);
    useful::place_in_board(board, whitehall);
    useful::place_in_board(board, northumberland_avenue);
    useful::place_in_board(board, marylebone_station);
    useful::place_in_board(board, bow_street);
    useful::place_in_board(board, community_chest_2);
    useful::place_in_board(board, marlborough_street);
    useful::place_in_board(board, vine_street);
    useful::place_in_board(board, free_parking_sq);
    useful::place_in_board(board, the_strand);
    useful::place_in_board(board, chance_card_2);
    useful::place_in_board(board, fleet_street);
    useful::place_in_board(board, trafalgar_square);
    useful::place_in_board(board, fenchurch_st_station);
    useful::place_in_board(board, leicester_square);
    useful::place_in_board(board, coventry_street);
    useful::place_in_board(board, water_works);
    useful::place_in_board(board, piccadilly);
    useful::place_in_board(board, go_to_jail_sq);
    useful::place_in_board(board, regent_street);
    useful::place_in_board(board, oxford_street);
    useful::place_in_board(board, community_chest_3);
    useful::place_in_board(board, bond_street);
    useful::place_in_board(board, liverpool_street_station);
    useful::place_in_board(board, chance_card_3);
    useful::place_in_board(board, park_lane);
    useful::place_in_board(board, super_tax);
    useful::place_in_board(board, mayfair);
    
    // option to start with properties  
    if(start_with_props)
    {
        std::shared_ptr<property> _p(game.get_property_pointer("Pall Mall"));
        std::shared_ptr<property> _p1(game.get_property_pointer("Whitehall"));
        std::shared_ptr<property> _p2(game.get_property_pointer("Northumberland Avenue"));
        game.get_current_player()->add_property(_p);
        game.get_current_player()->add_property(_p1);
        game.get_current_player()->add_property(_p2);

        std::shared_ptr<property> _p3(game.get_property_pointer("Whitechapel Road"));
        std::shared_ptr<property> _p4(game.get_property_pointer("Old Kent Road"));
        game.get_all_players()[1]->add_property(_p3);
        game.get_all_players()[1]->add_property(_p4);

        if(number_of_players>2)
        {
            std::shared_ptr<property> _p5(game.get_property_pointer("Piccadilly"));
            game.get_all_players()[2]->add_property(_p5);
        }
        if(number_of_players>3)
        {
            std::shared_ptr<property> _p6(game.get_property_pointer("Vine Street"));
            game.get_all_players()[3]->add_property(_p6);
        }
    }
    game.determine_order();
    game.all_prop_info();

    //gameplay loop
    while(true)
    {
        game.execute_turn(board);
        game.end_turn();
        if(game.get_all_players().size() <= 1)
        {
            std::cout<<game.get_current_player()->get_name()<< " has won the game!"<<std::endl;
            std::cout<<"Enter 'e' to exit."<<std::endl;
            game.game_end();
            while(true)
            {
                std::string result{useful::request_input({"e"})};
                if(result=="e") {break;}
            }
            break;
        }
    }
    for(int i; i<40;i++) board[i].reset();
    board.reset();
    return 0;
}