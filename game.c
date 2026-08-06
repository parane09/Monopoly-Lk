#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ============================================
// FUNCTION PROTOTYPES (Internal to game.c)
// ============================================

// Forward declarations for functions we'll implement later
// These are just to keep the compiler happy for now

// ============================================
// GAME INITIALIZATION
// ============================================

// function prototypes
void init_board_data(); 
DiceRoll roll_dice(void);
int move_player(Player* player, int dice_total);
Square* get_square(int position);
int check_game_over(GameState* game);
int determine_first_player(GameState* game);
void process_turn(GameState* game, Player* player);

void init_game(GameState* game) {
    // initialize the board with all the data
    init_board_data();
    // Initialize round number
    game->round_number = 1;
    game->current_player_index = 0;
    game->starting_player_index = 0;
    game->is_game_over = 0;
    game->winner_player_id = -1;
    
    // Initialize economic rates
    game->current_inflation_rate = 0;
    game->current_interest_rate = 8;  // Base interest rate (8%)
    
    // Clear all active events
    game->national_event.is_active = 0;
    game->national_event.rounds_remaining = 0;
    game->national_event.effect_percentage = 0;
    strcpy(game->national_event.event_name, "None");
    
    game->regional_development.is_active = 0;
    game->regional_development.rounds_remaining = 0;
    game->regional_development.effect_percentage = 0;
    strcpy(game->regional_development.event_name, "None");
    strcpy(game->regional_development.region_name, "None");
    
    game->government_regulation.is_active = 0;
    game->government_regulation.rounds_remaining = 0;
    game->government_regulation.effect_percentage = 0;
    strcpy(game->government_regulation.regulation_name, "None");
    
    game->market_boom.is_active = 0;
    game->market_boom.rounds_remaining = 0;
    game->market_boom.effect_percentage = 0;
    strcpy(game->market_boom.group_name, "None");
    
    game->market_decline.is_active = 0;
    game->market_decline.rounds_remaining = 0;
    game->market_decline.effect_percentage = 0;
    strcpy(game->market_decline.group_name, "None");
    
    // Initialize all 4 players
    const char* player_names[] = {
        "Aggressive Investor",
        "Conservative Banker", 
        "Risk Taker",
        "Opportunistic Trader"
    };
    
    const PlayerStrategy strategies[] = {
        STRATEGY_AGGRESSIVE,
        STRATEGY_CONSERVATIVE,
        STRATEGY_RISK_TAKER,
        STRATEGY_OPPORTUNISTIC
    };
    
    for (int i = 0; i < MAX_PLAYERS; i++) {
        game->players[i].player_id = i;
        strcpy(game->players[i].player_name, player_names[i]);
        game->players[i].strategy = strategies[i];
        
        // Starting cash
        game->players[i].cash = STARTING_CASH;
        
        // Board position (starts at GO)
        game->players[i].board_position = 0;
        
        // Property ownership (none initially)
        for (int j = 0; j < MAX_PROPERTIES; j++) {
            game->players[i].owned_property_indices[j] = -1;
        }
        game->players[i].owned_property_count = 0;
        
        // Loan (none initially)
        game->players[i].player_loan.is_active = 0;
        game->players[i].player_loan.current_amount = 0;
        game->players[i].player_loan.original_amount = 0;
        game->players[i].player_loan.interest_rate = 0;
        game->players[i].player_loan.rounds_remaining = 0;
        game->players[i].player_loan.initial_duration = LOAN_DURATION;
        game->players[i].player_loan.collateral_count = 0;
        for (int j = 0; j < MAX_COLLATERAL; j++) {
            game->players[i].player_loan.collateral_properties[j] = -1;
        }
        
        // Jail status
        game->players[i].is_in_jail = 0;
        game->players[i].jail_turns_served = 0;
        game->players[i].consecutive_doubles_rolled = 0;
        
        // Status
        game->players[i].is_bankrupt = 0;
        game->players[i].rounds_survived = 0;
        
        // Statistics
        game->players[i].total_rent_collected = 0;
        game->players[i].total_expenses_paid = 0;
        game->players[i].auction_wins = 0;
    }
    
    printf("Game initialized successfully.\n");
}

// ============================================
// GAME START PRINTING
// ============================================

void print_game_start(GameState* game) {
    printf("MONOPOLY-LK Simulation\n");
    printf("========================\n\n");
    
    // Print player information
    for (int i = 0; i < MAX_PLAYERS; i++) {
        printf("Player %d: %s\n", i + 1, game->players[i].player_name);
    }
    
    printf("\nEach player begins with LKR %d.\n\n", STARTING_CASH);
    
    // Simulate dice rolls to determine first player
    printf("Determining the First Player...\n");


    game->starting_player_index = determine_first_player(game);
    game->current_player_index = game->starting_player_index;

    printf("%s will begin the game\n",game->players[game->starting_player_index].player_name);

    // After the highest roller starts, play proceeds clockwise through the
    // fixed player array as demonstrated in the assignment brief.
    printf("\nTurn order:\n");
    for (int turn_position = 0; turn_position < MAX_PLAYERS; turn_position++) {
        int player_index = (game->starting_player_index + turn_position) % MAX_PLAYERS;
        printf("%s\n", game->players[player_index].player_name);
    }
    printf("\n");
}

// ============================================
// MAIN GAME LOOP (STUB VERSION)
// ============================================

void run_game(GameState* game) {
    printf("Starting simulation...\n");
    printf("Maximum Rounds: %d\n\n", MAX_ROUNDS);

    for(int round = 0; round<4 && !game->is_game_over; round++){
        printf("\n========== ROUND %d (TEST MODE) ==========\n", round);
        for(int i =0; i<MAX_PLAYERS; i++){
            int player_idx = (game->starting_player_index + i) % MAX_PLAYERS;
            Player* player = &game->players[player_idx];
            if(!player->is_bankrupt){
                process_turn(game,player);
            }
            else{
                printf("  %s is bankrupt - skipping\n", player->player_name);
            }
        }
        /*
        if(check_game_over){
            break;
        }
            */
    }
    printf("\n=== TEST MODE COMPLETE ===\n");
    printf("Full game loop will be implemented with finance.c\n");
}

// ============================================
// HELPER FUNCTIONS (Will be expanded later)
// ============================================

// Function to get player by ID
Player* get_player_by_id(GameState* game, int player_id) {
    if (player_id >= 0 && player_id < MAX_PLAYERS) {
        return &game->players[player_id];
    }
    return NULL;
}

// Function to check if game is over
int check_game_over(GameState* game) {
    int solvent_count = 0;
    int last_solvent = -1;
    
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (!game->players[i].is_bankrupt) {
            solvent_count++;
            last_solvent = i;
        }
    }
    
    if (solvent_count <= 1) {
        game->is_game_over = 1;
        if (solvent_count == 1) {
            game->winner_player_id = last_solvent;
        }
        return 1;
    }
    
    return 0;
}

// Function to calculate net worth (will be expanded)
int calculate_net_worth(Player* player) {
    int net_worth = player->cash;
    
    // Add property values (simplified for now)
    for (int i = 0; i < player->owned_property_count; i++) {
        int prop_idx = player->owned_property_indices[i];
        if (prop_idx >= 0 && prop_idx < MAX_PROPERTIES) {
            net_worth += property_array[prop_idx].purchase_price;
            // Add building value (simplified)
            if (property_array[prop_idx].building_count > 0) {
                net_worth += property_array[prop_idx].building_count * 1000;
            }
        }
    }
    
    // Subtract loan amount
    if (player->player_loan.is_active) {
        net_worth -= player->player_loan.current_amount;
    }
    
    return net_worth;
}

int determine_first_player(GameState* game){
    int players[MAX_PLAYERS] = {0, 1, 2, 3};
    int count = 4;

    while (count > 1) {
        int highest = 0;
        int next_players[4];
        int next_count = 0;
        
        for (int i = 0; i < count; i++) {
            DiceRoll roll = roll_dice();
            int player_index = players[i];
            printf("%s rolled %d\n", game->players[player_index].player_name, roll.total);
            if (roll.total > highest) {
                highest = roll.total;
                next_players[0] = players[i];
                next_count = 1;
            } else if (roll.total == highest) {
                next_players[next_count++] = players[i];
            }
        }
        // Only tied players continue
        for (int i = 0; i < next_count; i++) {
            players[i] = next_players[i];
        }
        count = next_count;
        
        if (count > 1) {
            printf("Tie! Rolling again...\n");
        }
    }
    return players[0];
}

void process_turn(GameState* game, Player* player){
    printf("\n  %s's turn:\n", player->player_name);

    DiceRoll diceroll = roll_dice();
    printf("    Rolled: %d\n", diceroll.total);

    int old_position = player->board_position;
    int passed_go = move_player(player, diceroll.total);

    printf("    Moved from square %d to square %d\n",
           old_position, player->board_position);

    if(passed_go){
        printf("    Passed GO! Collected LKR %d\n", GO_BONUS);
    }

    Square* square = get_square(player->board_position);
    printf("    Landed on: %s\n", square->square_name);
    printf("    Cash: LKR %d\n", player->cash);
}

