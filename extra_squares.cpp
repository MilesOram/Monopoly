#include"extra_squares.h"

void tax::landed_on(gamestate &_game)
{
    std::cout<<name<<": Pay "<<tax_fee<<std::endl;
    _game.get_current_player()->pay_bank(tax_fee, _game, true);
}
void free_parking::landed_on(gamestate &_game)
{
    // without the implementation of trading, games can get soft locked so that money keeps flowing between players but total in game cash keeps increasing as players pass go, to avoid this
    // if player chooses to start game without props, free parking cash is disabled
    if(!start_with_props)
    {
        std::cout<<"You landed on Free Parking! You gain all the cash in the cash pool!"<<std::endl;
        _game.get_current_player()->add_cash(_game.get_and_reset_cash_pool());
    }
}