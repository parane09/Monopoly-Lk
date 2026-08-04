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
void init_board_data(); // board data initialization proptotype
int roll_dice();
int move_player(Player* player, int dice_total);
Square* get_square(int position);
int check_game_over(GameState* game);

// Determines the order inside one tied group. Players who tie again keep
// rolling until every position in this group has been decided.
static void resolve_tied_player_order(GameState* game, int player_order[], int first_tied_position, int tied_player_count) {
    int tie_break_rolls[MAX_PLAYERS];

    if (tied_player_count <= 1) {
        return;
    }

    printf("\nTie detected between: ");
    for (int tied_offset = 0; tied_offset < tied_player_count; tied_offset++) {
        int player_index = player_order[first_tied_position + tied_offset];
        printf("%s", game->players[player_index].player_name);
        if (tied_offset < tied_player_count - 1) {
            printf(", ");
        }
    }
    printf(". Rolling again...\n");

    // Only the players in this tied group roll again.
    for (int tied_offset = 0; tied_offset < tied_player_count; tied_offset++) {
        int player_index = player_order[first_tied_position + tied_offset];
        tie_break_rolls[tied_offset] = roll_dice();
        printf("%s rolls %d.\n",
               game->players[player_index].player_name,
               tie_break_rolls[tied_offset]);
    }

    // Sort this tied group from the highest tie-break roll to the lowest.
    for (int current_position = 0; current_position < tied_player_count - 1; current_position++) {
        for (int comparison_position = current_position + 1;
             comparison_position < tied_player_count;
             comparison_position++) {
            if (tie_break_rolls[comparison_position] > tie_break_rolls[current_position]) {
                int temporary_roll = tie_break_rolls[current_position];
                tie_break_rolls[current_position] = tie_break_rolls[comparison_position];
                tie_break_rolls[comparison_position] = temporary_roll;

                int temporary_player_index = player_order[first_tied_position + current_position];
                player_order[first_tied_position + current_position] =
                    player_order[first_tied_position + comparison_position];
                player_order[first_tied_position + comparison_position] = temporary_player_index;
            }
        }
    }

    // Resolve any smaller groups that tied during this tie-break roll.
    int tied_group_start = 0;
    while (tied_group_start < tied_player_count) {
        int tied_group_end = tied_group_start + 1;

        while (tied_group_end < tied_player_count &&
               tie_break_rolls[tied_group_end] == tie_break_rolls[tied_group_start]) {
            tied_group_end++;
        }

        int remaining_tied_player_count = tied_group_end - tied_group_start;
        if (remaining_tied_player_count > 1) {
            resolve_tied_player_order(game,
                                      player_order,
                                      first_tied_position + tied_group_start,
                                      remaining_tied_player_count);
        }

        tied_group_start = tied_group_end;
    }
}

void init_game(GameState* game) {
    // initialize the board with all the data
    init_board_data();
    // Initialize round number
    game->round_number = 1;
    game->current_player_index = 0;
    game->starting_player_index = 0;
    // Default order is replaced by the dice-based order in print_game_start().
    for (int player_index = 0; player_index < MAX_PLAYERS; player_index++) {
        game->turn_order[player_index] = player_index;
    }
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
    
    int initial_rolls[MAX_PLAYERS];
    
    // Initial roll
    for (int player_index = 0; player_index < MAX_PLAYERS; player_index++) {
        initial_rolls[player_index] = roll_dice();
        game->turn_order[player_index] = player_index;
        printf("%s rolls %d.\n",
               game->players[player_index].player_name,
               initial_rolls[player_index]);
    }

    // Sort every player by the initial roll, from highest to lowest.
    for (int current_position = 0; current_position < MAX_PLAYERS - 1; current_position++) {
        for (int comparison_position = current_position + 1;
             comparison_position < MAX_PLAYERS;
             comparison_position++) {
            int current_player_index = game->turn_order[current_position];
            int comparison_player_index = game->turn_order[comparison_position];

            if (initial_rolls[comparison_player_index] > initial_rolls[current_player_index]) {
                int temporary_player_index = game->turn_order[current_position];
                game->turn_order[current_position] = game->turn_order[comparison_position];
                game->turn_order[comparison_position] = temporary_player_index;
            }
        }
    }

    // Reroll within every tied initial-roll group. A tie-break cannot move a
    // player outside the ranking group established by the initial roll.
    int tied_group_start = 0;
    while (tied_group_start < MAX_PLAYERS) {
        int tied_group_end = tied_group_start + 1;
        int first_player_in_group = game->turn_order[tied_group_start];

        while (tied_group_end < MAX_PLAYERS) {
            int next_player_in_group = game->turn_order[tied_group_end];
            if (initial_rolls[next_player_in_group] != initial_rolls[first_player_in_group]) {
                break;
            }
            tied_group_end++;
        }

        int tied_player_count = tied_group_end - tied_group_start;
        if (tied_player_count > 1) {
            resolve_tied_player_order(game,
                                      game->turn_order,
                                      tied_group_start,
                                      tied_player_count);
        }

        tied_group_start = tied_group_end;
    }

    // The first entry is retained for code that needs the starting player.
    game->starting_player_index = game->turn_order[0];
    game->current_player_index = game->turn_order[0];

    printf("\n%s will begin the game.\n",
           game->players[game->starting_player_index].player_name);

    // Print the complete dice-ranked turn order.
    printf("\nTurn order: ");
    for (int turn_position = 0; turn_position < MAX_PLAYERS; turn_position++) {
        int player_index = game->turn_order[turn_position];
        printf("%s", game->players[player_index].player_name);
        if (turn_position < MAX_PLAYERS - 1) {
            printf(", ");
        }
    }
    printf("\n\n");
}

// ============================================
// MAIN GAME LOOP (STUB VERSION)
// ============================================

void run_game(GameState* game) {
    printf("Starting simulation...\n");
    printf("Maximum Rounds: %d\n\n", MAX_ROUNDS);
    
    // TEMPORARY: Run 3 test rounds with real functions
    for (int round = 1; round <= 3 && !game->is_game_over; round++) {
        printf("\n=== ROUND %d (TEST MODE) ===\n", round);
        
        // Follow the complete dice-ranked order instead of wrapping around
        // from only the highest roller's player-array index.
        for (int turn_position = 0; turn_position < MAX_PLAYERS; turn_position++) {
            int player_index = game->turn_order[turn_position];
            game->current_player_index = player_index;
            Player* player = &game->players[player_index];
            
            if (!player->is_bankrupt) {
                printf("\n  %s's turn:\n", player->player_name);
                
                // USE REAL FUNCTIONS
                int dice = roll_dice();          // ← FROM board.c
                printf("    Rolled: %d\n", dice);
                
                int old_pos = player->board_position;
                int passed_go = move_player(player, dice);  // ← FROM board.c
                
                printf("    Moved from square %d to square %d\n", 
                       old_pos, player->board_position);
                
                if (passed_go) {
                    printf("    Passed GO! Collected LKR %d\n", GO_BONUS);
                }
                
                // Simplified landing (will expand later)
                Square* square = get_square(player->board_position);
                printf("    Landed on: %s\n", square->square_name);
                
                printf("    Cash: LKR %d\n", player->cash);
            } else {
                printf("  %s is bankrupt - skipping\n", player->player_name);
            }
        }
        
        printf("\n=== End of round %d ===\n", round);
        
        // End of round (placeholder)
        if (check_game_over(game)) break;
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

