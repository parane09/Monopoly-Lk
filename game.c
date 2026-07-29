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

void init_game(GameState* game) {
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
    
    int rolls[MAX_PLAYERS];
    int highest_roll = 0;
    int starting_player = 0;
    
    // Initial roll
    for (int i = 0; i < MAX_PLAYERS; i++) {
        // Simulate two dice (2-12 range)
        rolls[i] = (rand() % 6 + 1) + (rand() % 6 + 1);
        printf("%s rolls %d.\n", game->players[i].player_name, rolls[i]);
        
        if (rolls[i] > highest_roll) {
            highest_roll = rolls[i];
            starting_player = i;
        }
    }
    
    game->starting_player_index = starting_player;
    game->current_player_index = starting_player;
    
    printf("\n%s will begin the game.\n", game->players[starting_player].player_name);
    
    // Print turn order (starting from the determined player)
    printf("\nTurn order: ");
    for (int i = 0; i < MAX_PLAYERS; i++) {
        int player_index = (starting_player + i) % MAX_PLAYERS;
        printf("%s", game->players[player_index].player_name);
        if (i < MAX_PLAYERS - 1) {
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
    
    // This is a STUB - will be replaced with full implementation later
    printf("=== GAME LOOP STUB ===\n");
    printf("The game loop will be implemented here.\n");
    printf("It will run for up to %d rounds.\n\n", MAX_ROUNDS);
    
    // TEMPORARY: Run 3 test rounds to show it works
    for (int round = 1; round <= 3 && !game->is_game_over; round++) {
        printf("Round %d (TEST MODE)\n", round);
        
        // Each player takes a turn
        for (int i = 0; i < MAX_PLAYERS; i++) {
            int player_idx = (game->starting_player_index + i) % MAX_PLAYERS;
            Player* player = &game->players[player_idx];
            
            if (!player->is_bankrupt) {
                printf("  %s takes turn... (stub)\n", player->player_name);
                
                // Simulate player movement
                int dice = (rand() % 6 + 1) + (rand() % 6 + 1);
                printf("    Rolled: %d\n", dice);
                
                // Move player (simplified)
                int old_pos = player->board_position;
                player->board_position = (player->board_position + dice) % BOARD_SIZE;
                printf("    Moved from square %d to square %d\n", old_pos, player->board_position);
                
                // Check if passed GO
                if (player->board_position < old_pos) {
                    player->cash += GO_BONUS;
                    printf("    Passed GO! Collected LKR %d\n", GO_BONUS);
                }
                
                printf("    Cash: LKR %d\n", player->cash);
            } else {
                printf("  %s is bankrupt - skipping\n", player->player_name);
            }
        }
        
        // End of round
        printf("  End of round %d\n\n", round);
    }
    
    printf("=== END OF STUB ===\n");
    printf("Game loop placeholder complete.\n");
    printf("The full implementation will add:\n");
    printf("  - Property purchase and rent\n");
    printf("  - Loans and interest\n");
    printf("  - Building construction\n");
    printf("  - Insurance\n");
    printf("  - Events and inflation\n");
    printf("  - AI decision making\n");
    printf("  - And much more...\n");
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