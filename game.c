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
    // initialize the board with all the data
    init_board_data();
    init_event_deck();
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
    game->national_event.affected_group = GROUP_NONE;
    strcpy(game->national_event.event_name, "None");

    for (int i = 0; i < MAX_PLAYERS; i++) {
        game->player_events[i].is_active = 0;
        game->player_events[i].rounds_remaining = 0;
        game->player_events[i].effect_percentage = 0;
        game->player_events[i].affected_group = GROUP_NONE;
        strcpy(game->player_events[i].event_name, "None");
    }
    
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
        game->players[i].last_dice_total = 0;
        
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
    if (game == NULL) return;

    printf("Starting simulation...\n");
    printf("Maximum Rounds: %d\n\n", MAX_ROUNDS);

    game->round_number = get_round(game);
    printf("\n========== Round %d ==========\n", game->round_number);

    while (game->round_number <= 50 && !game->is_game_over) {
        for (int i = 0; i < MAX_PLAYERS; i++) {
            int player_idx = (game->starting_player_index + i) % MAX_PLAYERS;
            Player* player = &game->players[player_idx];

            if (player->is_bankrupt) {
                printf("  %s is bankrupt - skipping\n", player->player_name);
                continue;
            }

            if (player->is_in_jail) {
                printf("\n  %s's jail turn:\n", player->player_name);

                int paid_fine = 0;
                if (player->strategy == STRATEGY_AGGRESSIVE ||
                    player->strategy == STRATEGY_RISK_TAKER) {
                    if (player->cash >= 300) {
                        player->cash -= 300;
                        player->total_expenses_paid += 300;
                        player->is_in_jail = 0;
                        player->jail_turns_served = 0;
                        paid_fine = 1;
                        printf("  %s immediately paid the LKR 300 jail fine.\n",
                               player->player_name);
                    } else {
                        printf("  %s cannot afford the LKR 300 jail fine and must roll for doubles.\n",
                               player->player_name);
                    }
                }

                // Paying the fine releases the player, but movement resumes
                // only on their next turn. A released player does not roll now.
                if (!paid_fine) {
                    DiceRoll jail_roll = roll_dice();
                    player->last_dice_total = jail_roll.total;
                    printf("    Rolled: %d + %d = %d%s\n",
                           jail_roll.first_die,
                           jail_roll.second_die,
                           jail_roll.total,
                           jail_roll.is_double ? " (doubles)" : "");

                    if (jail_roll.is_double) {
                        player->is_in_jail = 0;
                        player->jail_turns_served = 0;
                        printf("  %s rolled doubles and was released from jail. Movement resumes next turn.\n",
                               player->player_name);
                    } else {
                        player->jail_turns_served++;
                        printf("  %s remains in jail after turn %d.\n",
                               player->player_name, player->jail_turns_served);

                        if (player->jail_turns_served >= 3) {
                            player->is_in_jail = 0;
                            player->jail_turns_served = 0;
                            printf("  %s has served three turns and was released from jail. Movement resumes next turn.\n",
                                   player->player_name);
                        }
                    }
                }
            } else {
                process_turn(game, player);
            }

            if (check_game_over(game)) {
                break;
            }

            // Recalculate after every turn. The shared round advances only
            // when every solvent player has passed GO for that round.
            int updated_round = get_round(game);
            while (game->round_number < updated_round &&
                   game->round_number <= MAX_ROUNDS) {
                end_of_round_processing(game);
                game->round_number++;

                if (check_game_over(game)) {
                    break;
                }

                if (game->round_number <= MAX_ROUNDS) {
                    printf("\n========== Round %d ==========\n",
                           game->round_number);
                }
            }

            if (game->round_number > MAX_ROUNDS || game->is_game_over) {
                break;
            }
        }
    }

    // Keep the final displayed round within the simulation limit.
    if (game->round_number > MAX_ROUNDS) {
        game->round_number = MAX_ROUNDS;
    }

    game->is_game_over = 1;
    Player* winner = determine_winner(game);
    print_winner_details(winner);
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
 
// A game round is one more than the fewest GO passes among solvent players.
// Jailed players remain included because they have not passed GO while jailed.
int get_round(const GameState* game) {
    int minimum_round = 0;
    int found_eligible_player = 0;

    for (int i = 0; i < MAX_PLAYERS; i++) {
        const Player* player = &game->players[i];

        if (player->is_bankrupt) {
            continue;
        }

        if (!found_eligible_player || player->rounds_survived < minimum_round) {
            minimum_round = player->rounds_survived;
            found_eligible_player = 1;
        }
    }

    return found_eligible_player ? minimum_round + 1 : game->round_number;
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

// Select the solvent player with the highest assignment-defined net worth.
// The assignment does not define ties, so cash and then player ID are used to
// produce a deterministic result.
Player* determine_winner(GameState* game) {
    if (game == NULL) return NULL;

    Player* winner = NULL;
    int highest_net_worth = 0;

    for (int i = 0; i < MAX_PLAYERS; i++) {
        Player* candidate = &game->players[i];
        if (candidate->is_bankrupt) continue;

        int candidate_net_worth = calculate_net_worth(candidate);
        if (winner == NULL || candidate_net_worth > highest_net_worth ||
            (candidate_net_worth == highest_net_worth &&
             candidate->cash > winner->cash) ||
            (candidate_net_worth == highest_net_worth &&
             candidate->cash == winner->cash &&
             candidate->player_id < winner->player_id)) {
            winner = candidate;
            highest_net_worth = candidate_net_worth;
        }
    }

    game->winner_player_id = (winner != NULL) ? winner->player_id : -1;
    return winner;
}

// End-of-game output format from the assignment specification.
void print_winner_details(const Player* winner) {
    printf("\n=============================================\n");
    printf("                 GAME OVER\n");
    printf("=============================================\n");
    if (winner == NULL) {
        printf("Winner\nNone - all players are bankrupt\n");
        printf("=============================================\n");
        return;
    }

    int total_property_value =
        calculate_total_property_value((Player*)winner);
    int net_worth = calculate_net_worth((Player*)winner);

    printf("Winner\n  %s\n", winner->player_name);
    printf("Total Cash\n  LKR %d\n", winner->cash);
    printf("Total Property Value\n  LKR %d\n", total_property_value);
    printf("Outstanding Loans\n  ");
    if (winner->player_loan.is_active &&
        winner->player_loan.current_amount > 0) {
        printf("LKR %d\n", winner->player_loan.current_amount);
    } else {
        printf("None\n");
    }
    printf("Net Worth\n  LKR %d\n", net_worth);
    printf("=============================================\n");
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
    player->last_dice_total = diceroll.total;
    printf("    Rolled: %d\n", diceroll.total);

    int old_position = player->board_position;
    int passed_go = move_player(player, diceroll.total);

    printf("    Moved from square %d to square %d\n",
           old_position, player->board_position);

    if(passed_go){
        player->rounds_survived++;
        printf("    Passed GO! Collected LKR %d\n", GO_BONUS);
    }

    Square* square = get_square(player->board_position);
    printf("    Landed on: %s\n", square->square_name);
    printf("    Cash: LKR %d\n", player->cash);

    resolve_landing(game, player);
}


// ============================================
// RESOLVE LANDING - Handle square effects
// ============================================

void resolve_landing(GameState* game, Player* player) {
    if (player == NULL) return;
    
    int position = player->board_position;
    Square* square = get_square(position);
    
    if (square == NULL) {
        printf("  ERROR: Invalid square position %d\n", position);
        return;
    }
    
    printf("  Landed on: %s\n", square->square_name);
    
    // Handle based on square type
    switch (square->square_type) {
        
        // ============================================
        // CASE: START (GO)
        // ============================================
        case SQUARE_START:
            // GO bonus is already handled in move_player()
            // Nothing else to do
            break;
            
        // ============================================
        // CASE: PROPERTY
        // ============================================
        case SQUARE_PROPERTY: {
            Property* prop = get_property_at_position(position);
            if (prop == NULL) {
                printf("  ERROR: Property not found at position %d\n", position);
                break;
            }
            
            if (prop->owner_id == -1) {
                printf("  %s is available for LKR %d.\n",
                       prop->property_name, prop->purchase_price);

                if (should_buy_property(player, prop)) {
                    buy_property(player, prop);
                } else {
                    start_auction(game, prop);
                }
            } else if (prop->owner_id == player->player_id) {
                printf("  %s already owns this property.\n", player->player_name);

                int property_index = (int)(prop - property_array);
                if (should_renovate(player, property_index)) {
                    renovate_property(player, property_index);
                }
            } else {
                Player* owner = &game->players[prop->owner_id];
                int rent = calculate_rent_with_buildings(prop);

                if (has_monopoly(owner, prop->color_group)) {
                    rent *= 2;
                    printf("  Owner has monopoly! Rent doubled.\n");
                }

                int amount_paid = (player->cash < rent) ? player->cash : rent;
                player->cash -= amount_paid;
                owner->cash += amount_paid;

                printf("  %s pays LKR %d rent to %s.\n",
                       player->player_name, amount_paid, owner->player_name);

                if (amount_paid < rent) {
                    declare_bankruptcy(player, "unable to pay the full property rent");
                }
            }
            break;
        }
        
        // ============================================
        // CASE: RAILWAY
        // ============================================
        case SQUARE_RAILWAY: {
            Property* prop = get_property_at_position(position);
            if (prop == NULL) break;
            
            // Check if unowned
            if (prop->owner_id == -1) {
                printf("  %s is available for LKR %d.\n", 
                       prop->property_name, prop->purchase_price);
                
                if (should_buy_property(player, prop)) {
                    buy_property(player, prop);
                } else {
                    start_auction(game, prop);
                }
                break;
            }
            
            // Pay rent if owned by someone else
            if (prop->owner_id != player->player_id) {
                Player* owner = &game->players[prop->owner_id];
                
                // Count how many railways the owner has
                int railway_count = 0;
                for (int i = 0; i < MAX_PROPERTIES; i++) {
                    if (property_array[i].color_group == GROUP_RAILWAY &&
                        property_array[i].owner_id == prop->owner_id) {
                        railway_count++;
                    }
                }
                
                // Railway rent based on number owned (Table 2)
                int rent = 0;
                switch (railway_count) {
                    case 1: rent = 250; break;
                    case 2: rent = 500; break;
                    case 3: rent = 1000; break;
                    case 4: rent = 2000; break;
                    default: rent = 250; break;
                }
                
                // Pay rent
                if (player->cash < rent) {
                    int amount_paid = player->cash;
                    owner->cash += amount_paid;
                    player->cash = 0;
                    printf("  %s pays LKR %d to %s.\n", 
                           player->player_name, amount_paid, owner->player_name);
                    declare_bankruptcy(player, "unable to pay the full railway rent");
                } else {
                    player->cash -= rent;
                    owner->cash += rent;
                    printf("  %s pays LKR %d railway rent to %s.\n", 
                           player->player_name, rent, owner->player_name);
                }
            }
            break;
        }
        
        // ============================================
        // CASE: UTILITY
        // ============================================
        case SQUARE_UTILITY: {
            Property* prop = get_property_at_position(position);
            if (prop == NULL) break;
            
            // Check if unowned
            if (prop->owner_id == -1) {
                printf("  %s is available for LKR %d.\n", 
                       prop->property_name, prop->purchase_price);
                
                if (should_buy_property(player, prop)) {
                    buy_property(player, prop);
                } else {
                    start_auction(game, prop);
                }
                break;
            }
            
            // Pay rent if owned by someone else
            if (prop->owner_id != player->player_id) {
                Player* owner = &game->players[prop->owner_id];
                
                // Count how many utilities the owner has
                int utility_count = 0;
                for (int i = 0; i < MAX_PROPERTIES; i++) {
                    if (property_array[i].color_group == GROUP_UTILITY &&
                        property_array[i].owner_id == prop->owner_id) {
                        utility_count++;
                    }
                }
                
                // Utility rent based on dice value
                int dice_total = player->last_dice_total;
                
                int rent = 0;
                if (utility_count == 1) {
                    rent = 4 * dice_total;
                } else if (utility_count == 2) {
                    rent = 10 * dice_total;
                }
                
                // Pay rent
                if (player->cash < rent) {
                    int amount_paid = player->cash;
                    owner->cash += amount_paid;
                    player->cash = 0;
                    printf("  %s pays LKR %d to %s.\n", 
                           player->player_name, amount_paid, owner->player_name);
                    declare_bankruptcy(player, "unable to pay the full utility rent");
                } else {
                    player->cash -= rent;
                    owner->cash += rent;
                    printf("  %s pays LKR %d utility rent to %s.\n", 
                           player->player_name, rent, owner->player_name);
                }
            }
            break;
        }
        
        // ============================================
        // CASE: EVENT
        // ============================================
        case SQUARE_EVENT:
            printf("  Drawing an event card...\n");
            draw_event_card(game);
            break;
            
        // ============================================
        // CASE: BANK
        // ============================================
        case SQUARE_BANK:
            printf("  Landed on Bank of Ceylon.\n");
            if (player->player_loan.is_active) {
                printf("  Active loan: LKR %d (Interest: %d%%)\n",
                       player->player_loan.current_amount,
                       player->player_loan.interest_rate);

                if (should_repay_loan(player)) {
                    int repayment = player->player_loan.current_amount;
                    if (repayment > player->cash) repayment = player->cash;
                    if (repayment > 0) repay_loan(player, repayment);
                }
            } else {
                int max_loan = get_max_loan_amount(player);
                printf("  Maximum loan available: LKR %d\n", max_loan);

                if (should_take_loan(player)) {
                    int amount = get_loan_amount(player);
                    if (amount > max_loan) amount = max_loan;
                    if (amount > 0) take_loan(player, amount);
                }
            }
            break;
            
        // ============================================
        // CASE: INSURANCE
        // ============================================
        case SQUARE_INSURANCE:
            printf("  Landed on Insurance company.\n");
            for (int i = 0; i < player->owned_property_count; i++) {
                int property_index = player->owned_property_indices[i];

                if (should_buy_insurance(player, property_index)) {
                    int policy_type = get_insurance_type(player, property_index);
                    if (policy_type != INSURANCE_NONE) {
                        buy_insurance(player, property_index, policy_type);
                    }
                }
            }
            break;
            
        // ============================================
        // CASE: TAX
        // ============================================
        case SQUARE_TAX:
            printf("  Paying income tax...\n");
            // Income tax: LKR 2000
            if (player->cash >= 2000) {
                player->cash -= 2000;
                printf("  %s paid LKR 2,000 income tax.\n", player->player_name);
            } else {
                declare_bankruptcy(player, "unable to pay income tax");
            }
            break;
            
        // ============================================
        // CASE: GO TO JAIL
        // ============================================
        case SQUARE_GO_TO_JAIL:
            printf("  GO TO JAIL!\n");
            player->is_in_jail = 1;
            player->board_position = 10;  // Jail square
            printf("  %s was sent to jail.\n", player->player_name);
            break;
            
        // ============================================
        // CASE: JAIL / JUST VISITING
        // ============================================
        case SQUARE_JAIL:
            printf("  Just visiting jail.\n");
            break;
            
        // ============================================
        // CASE: FREE PARKING
        // ============================================
        case SQUARE_FREE_PARKING:
            printf("  Free parking - nothing happens.\n");
            break;
            
        // ============================================
        // DEFAULT
        // ============================================
        default:
            printf("  Unknown square type!\n");
            break;
    }
}

// ============================================
// END OF ROUND PROCESSING
// Called after all 4 players have completed their turns
// ============================================

void end_of_round_processing(GameState* game) {
    if (game == NULL) return;
    
    printf("\n=== END OF ROUND %d PROCESSING ===\n", game->round_number);
    
    // ============================================
    // 1. LOAN INTEREST
    // ============================================
    printf("\n[1] Processing Loan Interest...\n");
    for (int i = 0; i < MAX_PLAYERS; i++) {
        Player* player = &game->players[i];
        if (!player->is_bankrupt && player->player_loan.is_active) {
            apply_loan_interest(player);
        }
    }
    
    // ============================================
    // 2. INSURANCE EXPIRY
    // ============================================
    printf("\n[2] Processing Insurance Expiry...\n");
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        Property* prop = &property_array[i];
        if (prop->owner_id != -1 && prop->insurance_policy != INSURANCE_NONE) {
            process_insurance_expiry(prop);
        }
    }
    
    // ============================================
    // 3. BUILDING CONDITION
    // ============================================
    printf("\n[3] Updating Building Conditions...\n");
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        Property* prop = &property_array[i];
        if (prop->owner_id != -1 && prop->building_count > 0) {
            update_building_condition(prop);
        }
    }
    
    // ============================================
    // 4. PROPERTY AGE & DEPRECIATION
    // ============================================
    printf("\n[4] Updating Property Ages...\n");
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        Property* prop = &property_array[i];
        if (prop->owner_id != -1) {
            // Increment age
            update_property_age(prop);
            
            // Check depreciation
            int depreciation = calculate_depreciation(prop);
            if (depreciation > 0 && depreciation % 5 == 0) {
                printf("  %s depreciated to %d%%\n", 
                       prop->property_name, 100 - depreciation);
            }
        }
    }
    
    // ============================================
    // 5. ROUND-BASED TRIGGERS
    // ============================================
    int round = game->round_number;
    
    // Every 10 rounds: Inflation, Disasters, Market Review
    if (round % 10 == 0) {
        printf("\n[5a] 10-ROUND EVENTS TRIGGERED!\n");
        
        // Inflation
        printf("  Processing Inflation...\n");
        process_inflation(game);
        
        // Disaster
        printf("  Checking for Disasters...\n");
        check_disaster(game);
        
        // Market Review
        printf("  Processing Market Review...\n");
        process_market_review(game);
    }
    
    // Every 15 rounds: National Events, Regional Development
    if (round % 15 == 0) {
        printf("\n[5b] 15-ROUND EVENTS TRIGGERED!\n");
        
        // National Event
        printf("  Processing National Event...\n");
        process_national_event(game);
        
        // Regional Development
        printf("  Processing Regional Development...\n");
        process_regional_development(game);
    }
    
    // Every 20 rounds: Government Regulations
    if (round % 20 == 0) {
        printf("\n[5c] 20-ROUND EVENTS TRIGGERED!\n");
        printf("  Processing Government Regulation...\n");
        process_government_regulation(game);
    }
    
    // ============================================
    // 6. BANKRUPTCY CHECK
    // ============================================
    printf("\n[6] Checking Bankruptcy Status...\n");
    for (int i = 0; i < MAX_PLAYERS; i++) {
        Player* player = &game->players[i];
        if (!player->is_bankrupt) {
            // Check if player has negative cash
            if (player->cash < 0) {
                // Try to resolve debt
                printf("  %s has negative cash: LKR %d\n", 
                       player->player_name, player->cash);
                // TODO: Asset liquidation
            }
            
            // Check if player is bankrupt (no cash, no properties, no income)
            if (player->cash <= 0 && player->owned_property_count == 0) {
                // Check if they have any income sources
                int has_income = 0;
                // TODO: Check railways, utilities, event income
                if (!has_income) {
                    declare_bankruptcy(player, "no cash, property, or income remains");
                }
            }
        }
    }
    // print finances
    printf("\n....printing finances....\n");
    for(int i = 0; i < MAX_PLAYERS; i++){
        Player* player = &game->players[i];
        if(player->is_bankrupt){
            printf("%s:  BANKRUPT\n", player->player_name);
        }
        else{
            print_player_finance(player);
        }
    }
    
    update_event_durations(game);
    printf("\n=== END OF ROUND %d PROCESSING COMPLETE ===\n", game->round_number);
}

// Helper function to buy property
void buy_property(Player* player, Property* prop) {
    if (player == NULL || prop == NULL) return;
    if (player->cash < prop->purchase_price) return;
    
    player->cash -= prop->purchase_price;
    prop->owner_id = player->player_id;
    
    // Add to player's owned properties
    player->owned_property_indices[player->owned_property_count] = prop - property_array;
    player->owned_property_count++;
    
    printf("  %s purchased %s for LKR %d.\n", 
           player->player_name, prop->property_name, prop->purchase_price);
}

// Run an auction according to Rules 6 and LK 19-23.
void start_auction(GameState* game, Property* prop) {
    if (game == NULL || prop == NULL) return;
    if (prop->owner_id != -1) {
        printf("  Auction cancelled: %s is already owned.\n",
               prop->property_name);
        return;
    }

    int active[MAX_PLAYERS] = {0};
    int active_count = 0;
    int highest_bidder = -1;
    int current_bid = get_property_value(prop) / 2;

    printf("\nAuction Started.\n");
    printf("Property :\n%s\n", prop->property_name);
    printf("Opening Bid :\nLKR %d.\n", current_bid);

    // Rule-LK 19: every solvent player enters the auction.
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (!game->players[i].is_bankrupt) {
            active[i] = 1;
            active_count++;
        }
    }

    while (active_count > 0) {
        int bids_this_pass = 0;

        for (int turn = 0; turn < MAX_PLAYERS; turn++) {
            int player_index =
                (game->starting_player_index + turn) % MAX_PLAYERS;
            Player* bidder = &game->players[player_index];

            if (!active[player_index] || player_index == highest_bidder) {
                continue;
            }

            int proposed_bid = get_auction_bid(bidder, prop, current_bid);

            // The controller enforces the rules even if a strategy returns an
            // invalid amount: minimum increment, available cash, no loans.
            if (proposed_bid >= current_bid + 250 &&
                proposed_bid <= bidder->cash) {
                current_bid = proposed_bid;
                highest_bidder = player_index;
                bids_this_pass++;
                printf("%s bids LKR %d.\n",
                       bidder->player_name, current_bid);
            } else {
                active[player_index] = 0;
                active_count--;
                printf("%s withdraws.\n", bidder->player_name);
            }
        }

        // Once every challenger has withdrawn, the current high bidder wins.
        if (highest_bidder != -1 && active_count == 1) {
            break;
        }

        // No bid in a complete pass means no remaining player can advance it.
        if (bids_this_pass == 0) {
            break;
        }
    }

    if (highest_bidder == -1) {
        printf("No player bids. %s remains with the Bank.\n",
               prop->property_name);
        return;
    }

    Player* winner = &game->players[highest_bidder];
    int property_index = (int)(prop - property_array);

    winner->cash -= current_bid;
    prop->owner_id = winner->player_id;
    if (winner->owned_property_count < MAX_PROPERTIES) {
        winner->owned_property_indices[winner->owned_property_count] =
            property_index;
        winner->owned_property_count++;
    }
    winner->auction_wins++;

    printf("%s wins the auction for LKR %d.\n",
           winner->player_name, current_bid);
}

