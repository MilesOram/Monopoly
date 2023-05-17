#ifndef VISUALS_H
#define VISUALS_H
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
    extern std::string ascii;

    // these quantities pertain to the size of different squares
    extern const int LARGE_SQUARE_WIDTH;
    extern const int LARGE_SQUARE_HEIGHT;
    extern const int LR_SMALL_SQUARE_WIDTH;
    extern const int LR_SMALL_SQUARE_HEIGHT;
    extern const int TB_SMALL_SQUARE_WIDTH;
    extern const int TB_SMALL_SQUARE_HEIGHT;
    extern const int WIDTH;
    extern const int HEIGHT;
    // useful point to start iterating from
    extern const int START;
    // these are the indices keeping track of where each player's 'piece'/number is within the R-string for the board, they are updated when a player moves
    extern int P_1_index;
    extern int P_2_index;
    extern int P_3_index;
    extern int P_4_index;
    // starting position for each number - on the GO square
    extern const int P_1_START;
    extern const int P_2_START;  
    extern const int P_3_START;
    extern const int P_4_START;

    // test function - ignore
    void place_ones(std::string &_ascii);
    // used to move a player specifically to the jail part of the jail square
    void place_in_jail(int player);
    // general function used whenever a player's board index (0 to 39) is changed, swaps their number/'piece' with the specified char
    // I tried to be as effecient as I could, but would not recommend trying to work these positions out from the code
    // there are four unique squares, the corners, and then each side of the board
    void place_player_number(int player, int _board_index);
}
#endif