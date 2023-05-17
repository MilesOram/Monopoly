#include "base.h"

// the following is an arguably long winded and inefficient way of visually displaying a board
// the result however, is that each player's "piece" is their number as shown in the GO square below
// when a player moves to a different square, their number is moved accordingly to that square
// number 1 is always in the top left of a square, 2 is top right, 3 is bottom left and 4 is bottom right (slight exception for jail, just visiting vs. jail is handled properly)
// 1 and 2 always swap with ' ' chars whilst 3 and 4 always swap with '_' chars when moving 
// Despite being the UK version of monopoly, $ is used instead of £ for ease, since £ is not displayed properly by default
// best displayed with "externalConsole": true in launch.json
namespace visuals
{
std::string ascii= R"(
____________________________________________________________________________________________________________________________________
|               |          |    __    |          |          |          |          |          |          |          |               |
|               |   THE    |   /  \   |  FLEET   | TRAFALGAR| FENCHURCH| LEICESTER| COVENTRY |   WATER  |  PICCA-  |               |
|     FREE      |  STRAND  |     _/   |  STREET  |  SQUARE  |  STREET  |  SQUARE  |  STREET  |   WORKS  |  DILLY   |     GO TO     |
|               |          |    |     |          |          |  STATION |          |          |          |          |               |
|    PARKING    |__________|__________|__________|__________|__________|__________|__________|__________|__________|      JAIL     |
|               |   $220   |  CHANCE  |   $220   |   $240   |   $200   |   $260   |   $260   |   $150   |   $280   |               |
|_______________|__________|__________|__________|__________|__________|__________|__________|__________|__________|_______________|
|           |   |                                                                                                  |   |           |
|   VINE    |$  |                                                                                                  |$  |  REGENT   |
|  STREET   |200|                                                                                                  |300|  STREET   |
|           |   |                                                                                                  |   |           |
|___________|___|                                                                                                  |___|___________|
|           |   |                                                                                                  |   |           |
|MARLBOROUGH|$  |                                                                                                  |$  |  OXFORD   |
|  STREET   |180|                                                                                                  |300|  STREET   |
|           |   |                                                                                                  |   |           |
|___________|___|                                                                                                  |___|___________|
|           |   |                                                                                                  |   |           |
| COMMUNITY |   |                                                                                                  |   | COMMUNITY |
|   CHEST   |   |                                                                                                  |   |   CHEST   |
|           |   |                                                                                                  |   |           |
|___________|___|                                                                                                  |___|___________|
|           |   |                                                                                                  |   |           |
|    BOW    |$  |                                                                                                  |$  |   BOND    |
|   STREET  |180|                                                                                                  |320|  STREET   |
|           |   |                                                                                                  |   |           |
|___________|___|                                                                                                  |___|___________|
|           |   |                                                                                                  |   |           |
| MARYLEBONE|$  |                                                                                                  |$  | LIVERPOOL |
|   STATION |200|                                                                                                  |200|  STREET   |
|           |   |                                                                                                  |   |  STATION  |
|___________|___|                                                                                                  |___|___________|
|           |   |                                                                                                  |   |    __     |
| NORTHUMB- |$  |                                                                                                  |CHA|   /  \    |
|  ERLAND   |160|                                                                                                  |NCE|     _/    |
|  AVENUE   |   |                                                                                                  |   |    |      |
|___________|___|                                                                                                  |___|___________|
|           |   |                                                                                                  |   |           |
| WHITEHALL |$  |                                                                                                  |$  |   PARK    |
|           |140|                                                                                                  |350|   LANE    |
|           |   |                                                                                                  |   |           |
|___________|___|                                                                                                  |___|___________|
|           |   |                                                                                                  |   |           |
|  ELECTRIC |$  |                                                                                                  |PAY|   SUPER   |
|  COMPANY  |150|                                                                                                  |$  |    TAX    |
|           |   |                                                                                                  |100|           |
|___________|___|                                                                                                  |___|___________|
|           |   |                                                                                                  |   |           |
|   PALL    |$  |                                                                                                  |$  |  MAYFAIR  |
|   MALL    |140|                                                                                                  |400|           |
|           |   |                                                                                                  |   |           |
|___________|___|__________________________________________________________________________________________________|___|___________|
|    |          |   $120   |   $100   |  CHANCE  |   $100   |   $200   | PAY $200 |   $60    |          |   $60    |1             2|
|  J |          |__________|__________|__________|__________|__________|__________|__________|__________|__________|   ___   ___   |
|  U |   JAIL   |          |          |    __    |          |          |          |          |          |          |  |  __ |   |  |
|  S |          |  PENTON- |  EUSTON  |   /  \   |    THE   |   KINGS  |  INCOME  |  WHITE-  | COMMUNITY| OLD KENT |  |___| |___|  |
|  T |__________|   VILLE  |   ROAD   |     _/   |   ANGEL  |   CROSS  |   TAX    |  CHAPEL  |   CHEST  |   ROAD   |               |
|     VISITING  |   ROAD   |          |    |     | ISLINGTON|  STATION |          |   ROAD   |          |          |  COLLECT $200 |
|_______________|__________|__________|__________|__________|__________|__________|__________|__________|__________|3_____________4|
)";

    // these quantities pertain to the size of different squares
    const int LARGE_SQUARE_WIDTH{16};
    const int LARGE_SQUARE_HEIGHT{7};
    const int LR_SMALL_SQUARE_WIDTH{16};
    const int LR_SMALL_SQUARE_HEIGHT{5};
    const int TB_SMALL_SQUARE_WIDTH{11};
    const int TB_SMALL_SQUARE_HEIGHT{7};
    const int WIDTH{133};
    const int HEIGHT{60};
    // useful point to start iterating from
    const int START{WIDTH+2+LARGE_SQUARE_WIDTH+WIDTH*(LARGE_SQUARE_HEIGHT+9*LR_SMALL_SQUARE_HEIGHT+2)};
    // these are the indices keeping track of where each player's 'piece'/number is within the R-string for the board, they are updated when a player moves
    int P_1_index{7166};
    int P_2_index{7180};
    int P_3_index{7964};
    int P_4_index{7978};
    // starting position for each number - on the GO square
    const int P_1_START{7166};
    const int P_2_START{7180};  
    const int P_3_START{7964};
    const int P_4_START{7978};

    // test function - ignore
    void place_ones(std::string &_ascii)
    {
        ascii[WIDTH+2] = *"1";
        ascii[WIDTH+LARGE_SQUARE_WIDTH] = *"2";
        ascii[WIDTH+6*WIDTH+2] = *"3";
        ascii[WIDTH+LARGE_SQUARE_WIDTH+6*WIDTH] = *"4";
        for(size_t i{0}; i<10;i++)
        {
            ascii[WIDTH+2+LARGE_SQUARE_WIDTH+TB_SMALL_SQUARE_WIDTH*i]= *"1";
        }
        for(size_t i{1}; i<10;i++)
        {
            ascii[WIDTH+LARGE_SQUARE_WIDTH+TB_SMALL_SQUARE_WIDTH*i]= *"2";
        }
        ascii[WIDTH+LARGE_SQUARE_WIDTH+LARGE_SQUARE_WIDTH+TB_SMALL_SQUARE_WIDTH*9]= *"2";
        for(size_t i{0}; i<9;i++)
        {
            ascii[WIDTH+2+LARGE_SQUARE_WIDTH+TB_SMALL_SQUARE_WIDTH*i+4*WIDTH]= *"3";
        }
        ascii[WIDTH+LARGE_SQUARE_WIDTH+2+TB_SMALL_SQUARE_WIDTH*9+WIDTH*6]= *"3";
        for(size_t i{1}; i<10;i++)
        {
            ascii[WIDTH+LARGE_SQUARE_WIDTH+TB_SMALL_SQUARE_WIDTH*i+4*WIDTH]= *"4";
        }
        ascii[WIDTH+LARGE_SQUARE_WIDTH+LARGE_SQUARE_WIDTH+TB_SMALL_SQUARE_WIDTH*9+WIDTH*6]= *"4";
        for(size_t i{0}; i<9;i++)
        {
            ascii[START+TB_SMALL_SQUARE_WIDTH*i] = *"1";
            ascii[START+9+TB_SMALL_SQUARE_WIDTH*i] = *"2";
            ascii[START+WIDTH*4+TB_SMALL_SQUARE_WIDTH*i] = *"3";
            ascii[START+9+WIDTH*4+TB_SMALL_SQUARE_WIDTH*i] = *"4";
        }
    }
    // used to move a player specifically to the jail part of the jail square
    void place_in_jail(int player)
    {
        if(player==1){std::swap(ascii[P_1_index], ascii[WIDTH*53+7]);P_1_index=WIDTH*53+7;}
        if(player==2){std::swap(ascii[P_2_index], ascii[WIDTH*53+LARGE_SQUARE_WIDTH]);P_2_index=WIDTH*53+LARGE_SQUARE_WIDTH;}
        if(player==3){std::swap(ascii[P_3_index], ascii[WIDTH*57+7]);P_3_index=WIDTH*57+7;}
        if(player==4){std::swap(ascii[P_4_index], ascii[WIDTH*57+LARGE_SQUARE_WIDTH]);P_4_index=WIDTH*57+LARGE_SQUARE_WIDTH;}
    }
    // general function used whenever a player's board index (0 to 39) is changed, swaps their number/'piece' with the specified char
    // I tried to be as effecient as I could, but would not recommend trying to work these positions out from the code
    // there are four unique squares, the corners, and then each side of the board
    void place_player_number(int player, int _board_index)
    {
        if(_board_index==0)
        {   
            if(player==1){std::swap(ascii[P_1_index], ascii[P_1_START]);P_1_index=P_1_START;}
            if(player==2){std::swap(ascii[P_2_index], ascii[P_2_START]);P_2_index=P_2_START;}
            if(player==3){std::swap(ascii[P_3_index], ascii[P_3_START]);P_3_index=P_3_START;}
            if(player==4){std::swap(ascii[P_4_index], ascii[P_4_START]);P_4_index=P_4_START;}
            
        }
        if(_board_index==10)
        {   
            if(player==1){std::swap(ascii[P_1_index], ascii[WIDTH*58+2]);P_1_index=WIDTH*58+2;}
            if(player==2){std::swap(ascii[P_2_index], ascii[WIDTH*58+3]);P_2_index=WIDTH*58+3;}
            if(player==3){std::swap(ascii[P_3_index], ascii[WIDTH*59+2]);P_3_index=WIDTH*59+2;}
            if(player==4){std::swap(ascii[P_4_index], ascii[WIDTH*59+3]);P_4_index=WIDTH*59+3;}
            
        }
        if(_board_index==20)
        {   
            if(player==1){std::swap(ascii[P_1_index], ascii[WIDTH+2]);P_1_index=WIDTH+2;}
            if(player==2){std::swap(ascii[P_2_index], ascii[WIDTH+LARGE_SQUARE_WIDTH]);P_2_index=WIDTH+LARGE_SQUARE_WIDTH;}
            if(player==3){std::swap(ascii[P_3_index], ascii[WIDTH+6*WIDTH+2]);P_3_index=WIDTH+6*WIDTH+2;}
            if(player==4){std::swap(ascii[P_4_index], ascii[WIDTH+LARGE_SQUARE_WIDTH+6*WIDTH]);P_4_index=WIDTH+LARGE_SQUARE_WIDTH+6*WIDTH;}
            
        }
        if(_board_index==30)
        {   
            if(player==1){std::swap(ascii[P_1_index], ascii[WIDTH+2+LARGE_SQUARE_WIDTH+TB_SMALL_SQUARE_WIDTH*9]);P_1_index=WIDTH+2+LARGE_SQUARE_WIDTH+TB_SMALL_SQUARE_WIDTH*9;}
            if(player==2){std::swap(ascii[P_2_index], ascii[WIDTH+LARGE_SQUARE_WIDTH+LARGE_SQUARE_WIDTH+TB_SMALL_SQUARE_WIDTH*9]);P_2_index=WIDTH+LARGE_SQUARE_WIDTH+LARGE_SQUARE_WIDTH+TB_SMALL_SQUARE_WIDTH*9;}
            if(player==3){std::swap(ascii[P_3_index], ascii[WIDTH+LARGE_SQUARE_WIDTH+2+TB_SMALL_SQUARE_WIDTH*9+WIDTH*6]);P_3_index=WIDTH+LARGE_SQUARE_WIDTH+2+TB_SMALL_SQUARE_WIDTH*9+WIDTH*6;}
            if(player==4){std::swap(ascii[P_4_index], ascii[WIDTH+LARGE_SQUARE_WIDTH+LARGE_SQUARE_WIDTH+TB_SMALL_SQUARE_WIDTH*9+WIDTH*6]);P_4_index=WIDTH+LARGE_SQUARE_WIDTH+LARGE_SQUARE_WIDTH+TB_SMALL_SQUARE_WIDTH*9+WIDTH*6;}
            
        }
        if(_board_index>0 && _board_index<10)
        {
            int placement{(9-_board_index)*TB_SMALL_SQUARE_WIDTH+START};
            if(player==1){std::swap(ascii[P_1_index], ascii[placement]);P_1_index=placement;}
            placement+=9;
            if(player==2){std::swap(ascii[P_2_index], ascii[placement]);P_2_index=placement;}
            placement+=523;
            if(player==3){std::swap(ascii[P_3_index], ascii[placement]);P_3_index=placement;}
            placement+=9;
            if(player==4){std::swap(ascii[P_4_index], ascii[placement]);P_4_index=placement;}
            
        }
        if(_board_index>10 && _board_index<20)
        {
            int placement{(19-_board_index)*WIDTH*LR_SMALL_SQUARE_HEIGHT+WIDTH*8+2};
            if(player==1){std::swap(ascii[P_1_index], ascii[placement]);P_1_index=placement;}
            placement+=10;
            if(player==2){std::swap(ascii[P_2_index], ascii[placement]);P_2_index=placement;}
            placement+=522;
            if(player==3){std::swap(ascii[P_3_index], ascii[placement]);P_3_index=placement;}
            placement+=10;
            if(player==4){std::swap(ascii[P_4_index], ascii[placement]);P_4_index=placement;}
            
        }
        if(_board_index>20 && _board_index<30)
        {
            int placement{(_board_index-21)*TB_SMALL_SQUARE_WIDTH+WIDTH+2+LARGE_SQUARE_WIDTH};
            if(player==1){std::swap(ascii[P_1_index], ascii[placement]);P_1_index=placement;}
            placement+=9;
            if(player==2){std::swap(ascii[P_2_index], ascii[placement]);P_2_index=placement;}
            placement+=523;
            if(player==3){std::swap(ascii[P_3_index], ascii[placement]);P_3_index=placement;}
            placement+=9;
            if(player==4){std::swap(ascii[P_4_index], ascii[placement]);P_4_index=placement;}
            
        }
        if(_board_index>30 && _board_index<40)
        {
            int placement{(_board_index-31)*LR_SMALL_SQUARE_HEIGHT*WIDTH+WIDTH*8+9*TB_SMALL_SQUARE_WIDTH+LR_SMALL_SQUARE_WIDTH+6};
            if(player==1){std::swap(ascii[P_1_index], ascii[placement]);P_1_index=placement;}
            placement+=10;
            if(player==2){std::swap(ascii[P_2_index], ascii[placement]);P_2_index=placement;}
            placement+=522;
            if(player==3){std::swap(ascii[P_3_index], ascii[placement]);P_3_index=placement;}
            placement+=10;
            if(player==4){std::swap(ascii[P_4_index], ascii[placement]);P_4_index=placement;}
            
        }
    }
}