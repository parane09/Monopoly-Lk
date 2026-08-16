#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// HELPER FUNCTIONS

int get_property_value(Property* prop) {
    if (prop == NULL) return 0;
    
    int value = prop->purchase_price;
    
    if (prop->value_reduction > 0) {
        value = (value * (100 - prop->value_reduction)) / 100;
    }

    // Structural damage causes a separate 15% loss in property value.
    if (prop->has_structural_damage) {
        value = (value * 85) / 100;
    }

    return value;
}

int get_total_mortgage_value(Player* player) {
    if (player == NULL) return 0;
    
    int total = 0;
    
    for (int i = 0; i < player->owned_property_count; i++) {
        int prop_idx = player->owned_property_indices[i];
        if (prop_idx < 0 || prop_idx >= MAX_PROPERTIES) continue;
        
        Property* prop = &property_array[prop_idx];
        
        if (!prop->is_mortgaged && !prop->is_loan_locked) {
            total += prop->mortgage_value;
        }
    }
    
    return total;
}

int has_monopoly(Player* player, PropertyGroup group) {
    if (player == NULL) return 0;
    
    int owned_count = 0;
    int total_in_group = 0;
    
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        if (property_array[i].color_group == group) {
            total_in_group++;
            if (property_array[i].owner_id == player->player_id) {
                owned_count++;
            }
        }
    }
    
    return (owned_count == total_in_group && total_in_group > 0);
}

int get_min_buildings_in_group(Player* player, PropertyGroup group) {
    if (player == NULL) return -1;
    
    int min_buildings = 999;
    int found_property = 0;
    
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        if (property_array[i].color_group == group && 
            property_array[i].owner_id == player->player_id) {
            
            int buildings = property_array[i].building_count;
            if (buildings < min_buildings) {
                min_buildings = buildings;
                found_property = 1;
            }
        }
    }

    if (!found_property) return -1;
    return min_buildings;
}

void print_player_finance(Player* player) {
    if (player == NULL) return;
    
    printf("\n=== %s FINANCIAL STATUS ===\n", player->player_name);
    printf("Cash: LKR %d\n", player->cash);
    
    if (player->player_loan.is_active) {
        printf("Loan: LKR %d (Interest: %d%%, Rounds left: %d)\n",
               player->player_loan.current_amount,
               player->player_loan.interest_rate,
               player->player_loan.rounds_remaining);
    } else {
        printf("Loan: None\n");
    }
    
    printf("Properties owned: %d\n", player->owned_property_count);
    printf("==============================\n");
}

// ============================================
// LOAN SYSTEM
// ============================================

void prepare_property_for_bank_auction(Property* prop) {
    if (prop == NULL) return;

    prop->owner_id = -1;
    prop->is_mortgaged = 0;
    prop->is_loan_locked = 0;
    prop->building_count = 0;
    prop->condition_percentage = 100;
    prop->rounds_since_maintenance = 0;
    prop->has_structural_damage = 0;
    prop->has_disaster_damage = 0;
    prop->pending_repair_cost = 0;
    prop->event_closed_rounds = 0;
    prop->insurance_policy = INSURANCE_NONE;
    prop->insurance_rounds_remaining = 0;
    prop->insurance_started_round = -1;
    prop->property_age = 0;
    prop->value_reduction = 0;
}

// Mark a player as bankrupt and auction every asset returned to the Bank.
void declare_bankruptcy(GameState* game, Player* player, const char* reason) {
    if (player == NULL || player->is_bankrupt) return;

    int assets_to_auction[MAX_PROPERTIES];
    int asset_count = 0;

    for (int i = 0; i < MAX_PROPERTIES; i++) {
        Property* prop = &property_array[i];
        if (prop->owner_id != player->player_id) continue;

        assets_to_auction[asset_count++] = i;
        prepare_property_for_bank_auction(prop);
    }

    for (int i = 0; i < MAX_PROPERTIES; i++) {
        player->owned_property_indices[i] = -1;
    }
    player->owned_property_count = 0;
    player->player_loan.is_active = 0;
    player->player_loan.current_amount = 0;
    player->player_loan.original_amount = 0;
    player->player_loan.rounds_remaining = 0;
    player->player_loan.collateral_count = 0;
    player->cash = 0;
    player->is_bankrupt = 1;
    player->is_in_jail = 0;
    player->jail_turns_served = 0;

    if (reason != NULL && reason[0] != '\0') {
        printf("  %s is BANKRUPT: %s.\n", player->player_name, reason);
    } else {
        printf("  %s is BANKRUPT!\n", player->player_name);
    }

    for (int i = 0; i < asset_count; i++) {
        Property* prop = &property_array[assets_to_auction[i]];
        start_special_auction(game, prop, "Bankruptcy",
                              player->player_id);
    }
}

// Maximum = 75% of total mortgage value of unmortgaged, unlocked properties
int get_max_loan_amount(Player* player) {
    if (player == NULL) return 0;
    
    int total_mortgage_value = get_total_mortgage_value(player);
    
    int max_loan = (total_mortgage_value * 75) / 100;
    
    return max_loan;
}

int get_max_loan_amount_with_market(Player* player, GameState* game) {
    if (player == NULL) return 0;

    int total_mortgage_value = 0;

    for (int i = 0; i < player->owned_property_count; i++) {
        int property_index = player->owned_property_indices[i];
        if (property_index < 0 || property_index >= MAX_PROPERTIES) continue;

        Property* prop = &property_array[property_index];
        if (prop->is_mortgaged || prop->is_loan_locked) continue;

        int mortgage_value = apply_market_mortgage_modifier(
            prop, game, prop->mortgage_value);
        total_mortgage_value += mortgage_value;
    }

    return (total_mortgage_value * 75) / 100;
}

int take_loan(Player* player, int amount, GameState* game) {
    if (player == NULL) return 0;
    
    if (player->player_loan.is_active) {
        printf("  %s already has an active loan!\n", player->player_name);
        return 0;
    }
    
    int max_loan = get_max_loan_amount_with_market(player, game);
    if (max_loan == 0) {
        printf("  %s has no eligible collateral for a loan.\n", player->player_name);
        return 0;
    }
    
    if (amount <= 0 || amount > max_loan) {
        printf("  Invalid loan amount. Maximum: LKR %d\n", max_loan);
        return 0;
    }
    
    
    // Lock only enough eligible collateral to secure the requested amount.
    lock_collateral(player, amount);
    int locked_count = player->player_loan.collateral_count;
    
    if (locked_count == 0) {
        printf("  No properties available to lock as collateral.\n");
        return 0;
    }
    
    player->player_loan.is_active = 1;
    player->player_loan.current_amount = amount;
    player->player_loan.original_amount = amount;
    int interest_rate = 8;
    if (game != NULL) {
        interest_rate = game->current_interest_rate;
        interest_rate = apply_event_interest_modifiers(
            interest_rate, game, player->player_id);
    }
    player->player_loan.interest_rate = interest_rate;
    player->player_loan.rounds_remaining = LOAN_DURATION;
    player->player_loan.initial_duration = LOAN_DURATION;
    player->player_loan.started_round =
        (game != NULL) ? game->round_number + 1 : -1;
    player->player_loan.collateral_count = locked_count;
    
    player->cash += amount;
    
    printf("  %s obtained a secured loan of LKR %d.\n", 
           player->player_name, amount);
    printf("  Interest Rate: %d%%, Duration: %d Rounds\n", 
           player->player_loan.interest_rate, LOAN_DURATION);
    printf("  Collateral: %d properties locked.\n", locked_count);
    
    return 1;
}

int repay_loan(Player* player, int amount) {
    if (player == NULL) return 0;
    
    if (!player->player_loan.is_active) {
        printf("  %s has no active loan to repay.\n", player->player_name);
        return 0;
    }
    
    if (amount <= 0) {
        printf("  Repayment amount must be positive.\n");
        return 0;
    }
    
    if (amount > player->cash) {
        printf("  Insufficient cash. Available: LKR %d\n", player->cash);
        return 0;
    }
    
    if (amount > player->player_loan.current_amount) {
        amount = player->player_loan.current_amount;
    }
    
    player->cash -= amount;
    player->player_loan.current_amount -= amount;
    
    printf("  %s repaid LKR %d. Remaining loan: LKR %d\n", 
           player->player_name, amount, player->player_loan.current_amount);
    
    if (player->player_loan.current_amount == 0) {
        unlock_collateral(player);
        player->player_loan.is_active = 0;
        printf("  Loan fully repaid! Collateral unlocked.\n");
    }
    
    return 1;
}

void apply_loan_interest(Player* player, int current_round, GameState* game) {
    if (player == NULL) return;
    if (!player->player_loan.is_active) return;
    if (player->player_loan.started_round == current_round) return;
    
    // Compound interest: amount = amount * (100 + rate) / 100
    int interest = (player->player_loan.current_amount * player->player_loan.interest_rate) / 100;
    player->player_loan.current_amount += interest;
    
    player->player_loan.rounds_remaining--;
    
    printf("  %s loan interest: LKR %d added. New balance: LKR %d (Rounds left: %d)\n",
           player->player_name, interest, 
           player->player_loan.current_amount,
           player->player_loan.rounds_remaining);
    
    if (player->player_loan.rounds_remaining <= 0) {
        process_loan_default(player, game);
    }
}

void process_loan_default(Player* player, GameState* game) {
    if (player == NULL) return;
    if (!player->player_loan.is_active) return;
    
    printf("\n LOAN DEFAULT: %s has failed to repay the loan! \n", 
           player->player_name);
    printf("  Foreclosure initiated...\n");

    int foreclosed_properties[MAX_COLLATERAL];
    int foreclosed_count = 0;
    
    for (int i = 0; i < player->player_loan.collateral_count; i++) {
        int prop_idx = player->player_loan.collateral_properties[i];
        if (prop_idx < 0 || prop_idx >= MAX_PROPERTIES) continue;
        
        Property* prop = &property_array[prop_idx];

        foreclosed_properties[foreclosed_count++] = prop_idx;
        
        for (int j = 0; j < player->owned_property_count; j++) {
            if (player->owned_property_indices[j] == prop_idx) {
                for (int k = j; k < player->owned_property_count - 1; k++) {
                    player->owned_property_indices[k] = player->owned_property_indices[k + 1];
                }
                player->owned_property_count--;
                break;
            }
        }
        
        prepare_property_for_bank_auction(prop);
        
        printf("    Property %s transferred to Bank.\n", prop->property_name);
    }
    
    player->player_loan.is_active = 0;
    player->player_loan.current_amount = 0;
    player->player_loan.original_amount = 0;
    player->player_loan.rounds_remaining = 0;
    player->player_loan.collateral_count = 0;
    for (int i = 0; i < MAX_COLLATERAL; i++) {
        player->player_loan.collateral_properties[i] = -1;
    }
    
    printf("  Outstanding debt cleared.\n");

    for (int i = 0; i < foreclosed_count; i++) {
        Property* prop = &property_array[foreclosed_properties[i]];
        start_special_auction(game, prop, "Foreclosure",
                              player->player_id);
    }
    
    if (player->owned_property_count == 0 && player->cash <= 0) {
        declare_bankruptcy(game, player,
                           "no assets remain after loan foreclosure");
    } else {
        printf("  %s continues with remaining assets.\n", player->player_name);
        print_player_finance(player);
    }
}

// Lock enough properties to support a loan at the 75% collateral limit.
void lock_collateral(Player* player, int loan_amount) {
    if (player == NULL) return;
    
    int locked = 0;
    int mortgage_value = 0;
    
    for (int i = 0; i < player->owned_property_count && locked < MAX_COLLATERAL; i++) {
        int property_index = player->owned_property_indices[i];
        if (property_index < 0 || property_index >= MAX_PROPERTIES) continue;

        Property* prop = &property_array[property_index];
        if (prop->owner_id == player->player_id &&
            !prop->is_mortgaged && !prop->is_loan_locked) {
            
            prop->is_loan_locked = 1;
            player->player_loan.collateral_properties[locked] = property_index;
            mortgage_value += prop->mortgage_value;
            locked++;

            if ((mortgage_value * 75) / 100 >= loan_amount) break;
        }
    }
    
    player->player_loan.collateral_count = locked;
}

void unlock_collateral(Player* player) {
    if (player == NULL) return;
    
    for (int i = 0; i < player->player_loan.collateral_count; i++) {
        int prop_idx = player->player_loan.collateral_properties[i];
        if (prop_idx >= 0 && prop_idx < MAX_PROPERTIES) {
            property_array[prop_idx].is_loan_locked = 0;
        }
    }
    
    player->player_loan.collateral_count = 0;
}

// ============================================
// INSURANCE SYSTEM
// ============================================

int calculate_insurance_premium(Property* prop, int policy_type) {
    if (prop == NULL) return 0;
    
    int property_value = get_property_value(prop);
    int premium = 0;
    
    switch (policy_type) {
        case INSURANCE_BASIC:
            premium = (property_value * 5) / 100;
            break;
            
        case INSURANCE_COMPREHENSIVE:
            premium = (property_value * 10) / 100;
            break;
            
        case INSURANCE_BUSINESS:
            if (prop->building_count == 5) {  // Has hotel
                premium = (property_value * 15) / 100;
            } else {
                printf("  Business Interruption Insurance only applies to hotels!\n");
                return 0;
            }
            break;
            
        case INSURANCE_NONE:
        default:
            return 0;
    }
    
    return premium;
}

int buy_insurance(Player* player, int property_index, int policy_type, GameState* game) {
    if (player == NULL) return 0;
    if (property_index < 0 || property_index >= MAX_PROPERTIES) return 0;
    
    Property* prop = &property_array[property_index];

    // Railway stations may be mortgaged, but cannot be insured.
    if (prop->color_group == GROUP_RAILWAY) {
        printf("  Railway stations cannot be insured.\n");
        return 0;
    }
    
    if (prop->owner_id != player->player_id) {
        printf("  %s does not own %s!\n", player->player_name, prop->property_name);
        return 0;
    }
    
    if (prop->insurance_policy != INSURANCE_NONE) {
        printf("  %s already has insurance on %s.\n", 
               player->player_name, prop->property_name);
        return 0;
    }
    
    int premium = calculate_insurance_premium(prop, policy_type);
    premium = apply_event_insurance_modifiers(
        premium, game, player->player_id);
    if (premium == 0) {
        printf("  Invalid insurance policy or property not eligible.\n");
        return 0;
    }
    
    if (premium > player->cash) {
        printf("  Insufficient funds! Premium: LKR %d, Available: LKR %d\n", 
               premium, player->cash);
        return 0;
    }
    
    player->cash -= premium;
    
    prop->insurance_policy = policy_type;
    prop->insurance_rounds_remaining = INSURANCE_DURATION;
    prop->insurance_started_round =
        (game != NULL) ? game->round_number + 1 : -1;
    
    const char* policy_names[] = {
        "None",
        "Basic Property Insurance",
        "Comprehensive Insurance",
        "Business Interruption Insurance"
    };
    
    printf("  %s purchased %s for %s.\n", 
           player->player_name, 
           policy_names[policy_type],
           prop->property_name);
    printf("  Premium paid: LKR %d. Duration: %d rounds.\n", 
           premium, INSURANCE_DURATION);
    
    return 1;
}

// Process insurance expiry - called at end of each round
void process_insurance_expiry(Property* prop, int current_round) {
    if (prop == NULL) return;
    if (prop->insurance_policy == INSURANCE_NONE) return;
    if (prop->insurance_started_round == current_round) return;
    
    prop->insurance_rounds_remaining--;
    
    if (prop->insurance_rounds_remaining == 3) {
        const char* policy_names[] = {
            "None",
            "Basic Property Insurance",
            "Comprehensive Insurance",
            "Business Interruption Insurance"
        };
        printf("  REMINDER: %s insurance on %s expires in 3 rounds!\n", 
               policy_names[prop->insurance_policy],
               prop->property_name);
    }
    
    if (prop->insurance_rounds_remaining <= 0) {
        const char* policy_names[] = {
            "None",
            "Basic Property Insurance",
            "Comprehensive Insurance",
            "Business Interruption Insurance"
        };
        printf("  Insurance expired for %s (%s).\n", 
               prop->property_name,
               policy_names[prop->insurance_policy]);
        prop->insurance_policy = INSURANCE_NONE;
        prop->insurance_rounds_remaining = 0;
        prop->insurance_started_round = -1;
    }
}

int process_disaster_claim(Property* prop, int damage_cost,
                           const char* disaster_type, GameState* game) {
    if (prop == NULL) return 0;
    if (prop->insurance_policy == INSURANCE_NONE) {
        printf("  No insurance on %s. Owner must pay full repair cost: LKR %d\n",
               prop->property_name, damage_cost);
        return 0;
    }
    
    int compensation = 0;
    const char* policy_names[] = {
        "None",
        "Basic Property Insurance",
        "Comprehensive Insurance",
        "Business Interruption Insurance"
    };
    
    switch (prop->insurance_policy) {
        case INSURANCE_BASIC:
            if (strcmp(disaster_type, "Fire") != 0 &&
                strcmp(disaster_type, "Flood") != 0) {
                printf("  Basic Insurance does not cover %s.\n", disaster_type);
                return 0;
            }
            compensation = (damage_cost * 80) / 100;
            printf("  Basic Insurance covers 80%% of LKR %d = LKR %d\n",
                   damage_cost, compensation);
            break;
            
        case INSURANCE_COMPREHENSIVE:
            if (strcmp(disaster_type, "Fire") != 0 &&
                strcmp(disaster_type, "Flood") != 0 &&
                strcmp(disaster_type, "Riot") != 0) {
                printf("  Comprehensive Insurance does not cover %s.\n",
                       disaster_type);
                return 0;
            }
            compensation = damage_cost;
            printf("  Comprehensive Insurance covers 100%% = LKR %d\n", compensation);
            break;
            
        case INSURANCE_BUSINESS:
            // Repair cost + 5 rounds lost hotel rental income
            if (prop->building_count == 5) {  // Has hotel
                int lost_rent = calculate_rent_with_buildings(prop);
                lost_rent = apply_event_rent_modifiers(
                    prop, game, prop->owner_id, lost_rent);
                lost_rent *= 5;
                compensation = damage_cost + lost_rent;
                printf("  Business Interruption covers repair + 5 rounds hotel rent = LKR %d\n",
                       compensation);
            } else {
                compensation = damage_cost;
                printf("  No hotel on property. Treated as Comprehensive: LKR %d\n", compensation);
            }
            break;
            
        default:
            return 0;
    }
    /*
    prop->insurance_rounds_remaining -= 2;
    if (prop->insurance_rounds_remaining < 0) {
        prop->insurance_rounds_remaining = 0;
    }
    */
    
    printf("  %s insurance claim approved for %s.\n", 
           policy_names[prop->insurance_policy],
           prop->property_name);
    
    return compensation;
}

int has_active_insurance(Property* prop) {
    if (prop == NULL) return 0;
    return (prop->insurance_policy != INSURANCE_NONE);
}

int get_insurance_reminder(Property* prop) {
    if (prop == NULL) return 0;
    if (prop->insurance_policy == INSURANCE_NONE) return 0;
    
    if (prop->insurance_rounds_remaining == 3) {
        return 1;
    }
    
    return 0;
}

const char* get_insurance_policy_name(int policy_type) {
    static const char* names[] = {
        "None",
        "Basic Property Insurance",
        "Comprehensive Insurance",
        "Business Interruption Insurance"
    };
    
    if (policy_type < 0 || policy_type > 3) {
        return "Unknown";
    }
    
    return names[policy_type];
}

void print_player_insurance(Player* player) {
    if (player == NULL) return;
    
    printf("\n=== %s INSURANCE POLICIES ===\n", player->player_name);
    int has_insurance = 0;
    
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        if (property_array[i].owner_id == player->player_id &&
            property_array[i].insurance_policy != INSURANCE_NONE) {
            
            const char* policy_name = get_insurance_policy_name(property_array[i].insurance_policy);
            printf("  %s: %s (%d rounds remaining)\n",
                   property_array[i].property_name,
                   policy_name,
                   property_array[i].insurance_rounds_remaining);
            has_insurance = 1;
        }
    }
    
    if (!has_insurance) {
        printf("  No active insurance policies.\n");
    }
    printf("===============================\n");
}


// ============================================
// BUILDING SYSTEM
// ============================================

int can_build_house(Player* player, int property_index, GameState* game) {
    if (player == NULL) return 0;
    if (property_index < 0 || property_index >= MAX_PROPERTIES) return 0;
    
    Property* prop = &property_array[property_index];

    if (is_event_construction_suspended(game, player->player_id)) {
        printf("  Construction is currently suspended by an event.\n");
        return 0;
    }

    // Railway stations and utilities cannot be developed.
    if (prop->color_group == GROUP_RAILWAY || prop->color_group == GROUP_UTILITY) {
        printf("  %s cannot be developed.\n", prop->property_name);
        return 0;
    }
    
    if (prop->owner_id != player->player_id) {
        printf("  %s does not own %s!\n", player->player_name, prop->property_name);
        return 0;
    }
    
    if (prop->building_count == 5) {
        printf("  %s already has a hotel. Cannot build more.\n", prop->property_name);
        return 0;
    }
    
    if (prop->building_count == 4) {
        printf("  %s has 4 houses. Upgrade to hotel instead.\n", prop->property_name);
        return 0;
    }
    
    if (!has_monopoly(player, prop->color_group)) {
        printf("  %s does not have a monopoly on this color group.\n", player->player_name);
        return 0;
    }
    
    int cost = apply_event_construction_modifiers(
        prop, prop->house_construction_cost, game, player->player_id);
    if (player->cash < cost) {
        printf("  Insufficient funds! House cost: LKR %d, Available: LKR %d\n", 
               cost, player->cash);
        return 0;
    }
    
    int min_buildings = get_min_buildings_in_group(player, prop->color_group);
    if (min_buildings == -1) {
        return 1;  // Can build if it's the only property in group
    }
    
    int current_buildings = prop->building_count;
    
    int min_count = 999;
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        if (property_array[i].color_group == prop->color_group && 
            property_array[i].owner_id == player->player_id) {
            if (property_array[i].building_count < min_count) {
                min_count = property_array[i].building_count;
            }
        }
    }
    
    if (current_buildings > min_count) {
        printf("  Even building rule: %s has %d houses, but another property has only %d.\n",
               prop->property_name, current_buildings, min_count);
        return 0;
    }
    
    return 1;
}

int build_house(Player* player, int property_index, GameState* game) {
    if (player == NULL) return 0;
    if (property_index < 0 || property_index >= MAX_PROPERTIES) return 0;
    
    if (!can_build_house(player, property_index, game)) {
        return 0;
    }
    
    Property* prop = &property_array[property_index];
    int cost = apply_event_construction_modifiers(
        prop, prop->house_construction_cost, game, player->player_id);
    
    player->cash -= cost;
    
    prop->building_count++;
    
    prop->condition_percentage = 100;
    prop->rounds_since_maintenance = 0;
    
    printf("  %s constructed a house on %s. (Cost: LKR %d, Houses: %d)\n",
           player->player_name, prop->property_name, cost, prop->building_count);
    
    return 1;
}

int can_build_hotel(Player* player, int property_index, GameState* game) {
    if (player == NULL) return 0;
    if (property_index < 0 || property_index >= MAX_PROPERTIES) return 0;
    
    Property* prop = &property_array[property_index];

    if (is_event_construction_suspended(game, player->player_id)) {
        printf("  Construction is currently suspended by an event.\n");
        return 0;
    }

    // Railway stations and utilities cannot be developed.
    if (prop->color_group == GROUP_RAILWAY || prop->color_group == GROUP_UTILITY) {
        printf("  %s cannot be developed.\n", prop->property_name);
        return 0;
    }
    
    if (prop->owner_id != player->player_id) {
        printf("  %s does not own %s!\n", player->player_name, prop->property_name);
        return 0;
    }
    
    if (prop->building_count == 5) {
        printf("  %s already has a hotel.\n", prop->property_name);
        return 0;
    }
    
    if (prop->building_count != 4) {
        printf("  %s needs 4 houses before building a hotel. Currently: %d\n",
               prop->property_name, prop->building_count);
        return 0;
    }
    
    if (!has_monopoly(player, prop->color_group)) {
        printf("  %s does not have a monopoly on this color group.\n", player->player_name);
        return 0;
    }
    
    int cost = apply_event_construction_modifiers(
        prop, prop->hotel_construction_cost, game, player->player_id);
    if (player->cash < cost) {
        printf("  Insufficient funds! Hotel cost: LKR %d, Available: LKR %d\n", 
               cost, player->cash);
        return 0;
    }
    
    // Every property must have reached four houses. Properties already
    // converted to hotels remain legally developed.
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        if (property_array[i].color_group == prop->color_group && 
            property_array[i].owner_id == player->player_id) {
            if (property_array[i].building_count < 4) {
                printf("  Hotel rule: %s is ready, but %s has only %d houses.\n",
                       prop->property_name, property_array[i].property_name,
                       property_array[i].building_count);
                return 0;
            }
        }
    }
    
    return 1;
}

int build_hotel(Player* player, int property_index, GameState* game) {
    if (player == NULL) return 0;
    if (property_index < 0 || property_index >= MAX_PROPERTIES) return 0;
    
    if (!can_build_hotel(player, property_index, game)) {
        return 0;
    }
    
    Property* prop = &property_array[property_index];
    int cost = apply_event_construction_modifiers(
        prop, prop->hotel_construction_cost, game, player->player_id);
    
    player->cash -= cost;
    
    prop->building_count = 5;
    
    prop->condition_percentage = 100;
    prop->rounds_since_maintenance = 0;
    
    printf("  %s upgraded %s to a HOTEL! (Cost: LKR %d)\n",
           player->player_name, prop->property_name, cost);
    
    return 1;
}

int calculate_rent_with_buildings(Property* prop) {
    if (prop == NULL) return 0;
    
    int base_rent = prop->base_rent;
    int buildings = prop->building_count;
    
    if (prop->is_mortgaged) {
        printf("  %s is mortgaged. No rent collected.\n", prop->property_name);
        return 0;
    }
    
    if (prop->condition_percentage < 25 && buildings > 0) {
        printf("  %s building is closed. No rent collected.\n", prop->property_name);
        return 0;
    }
    
    int multiplier = 1;
    
    switch (buildings) {
        case 0:  // No buildings
            multiplier = 1;
            break;
        case 1:  // 1 house
            multiplier = 2;
            break;
        case 2:  // 2 houses
            multiplier = 3;
            break;
        case 3:  // 3 houses
            multiplier = 5;
            break;
        case 4:  // 4 houses
            multiplier = 7;
            break;
        case 5:  // Hotel
            multiplier = 10;
            break;
        default:
            multiplier = 1;
            break;
    }
    
    int condition_multiplier = get_rent_multiplier(prop);
    if (condition_multiplier == 0) {
        return 0;  // Building closed
    }
    
    // Calculate rent: base_rent * building_multiplier * condition_multiplier / 100
    int rent = (base_rent * multiplier * condition_multiplier) / 100;

    // Structural damage reduces maximum rent by 25%.
    if (prop->has_structural_damage) {
        rent = (rent * 75) / 100;
    }
    
    if (buildings == 0) {
        rent = base_rent;
    }
    
    return rent;
}

int get_rent_multiplier(Property* prop) {
    if (prop == NULL) return 0;
    
    int condition = prop->condition_percentage;
    
    if (prop->building_count == 0) {
        return 100;
    }
    
    // Table 3: Building Condition Table
    if (condition >= 90 && condition <= 100) {
        return 100;   // 100% rent
    } else if (condition >= 75 && condition <= 89) {
        return 90;    // 90% rent
    } else if (condition >= 50 && condition <= 74) {
        return 75;    // 75% rent
    } else if (condition >= 25 && condition <= 49) {
        return 50;    // 50% rent
    } else if (condition < 25) {
        return 0;     // Building closed - 0% rent
    }
    
    return 100;  // Default
}

int get_building_multiplier(int building_count) {
    switch (building_count) {
        case 0:  return 1;
        case 1:  return 2;
        case 2:  return 3;
        case 3:  return 5;
        case 4:  return 7;
        case 5:  return 10;
        default: return 1;
    }
}

void print_building_status(Property* prop) {
    if (prop == NULL) return;
    
    printf("  %s: ", prop->property_name);
    
    if (prop->building_count == 0) {
        printf("No buildings\n");
    } else if (prop->building_count == 5) {
        printf("HOTEL (Condition: %d%%)\n", prop->condition_percentage);
    } else {
        printf("%d house(s) (Condition: %d%%)\n", 
               prop->building_count, prop->condition_percentage);
    }
}

void print_player_buildings(Player* player) {
    if (player == NULL) return;
    
    printf("\n=== %s BUILDINGS ===\n", player->player_name);
    int has_buildings = 0;
    
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        if (property_array[i].owner_id == player->player_id &&
            property_array[i].building_count > 0) {
            print_building_status(&property_array[i]);
            has_buildings = 1;
        }
    }
    
    if (!has_buildings) {
        printf("  No buildings.\n");
    }
    printf("====================\n");
}

int get_building_cost(Property* prop, GameState* game) {
    if (prop == NULL) return 0;
    
    int cost;
    if (prop->building_count == 4) {
        cost = prop->hotel_construction_cost;
    } else {
        cost = prop->house_construction_cost;
    }
    
    if (game != NULL) {
        int player_id = prop->owner_id;
        cost = apply_event_construction_modifiers(prop, cost, game, player_id);
    }
    
    return cost;
}

// ============================================
// BUILDING CONDITION SYSTEM
// ============================================

void update_building_condition(Property* prop) {
    if (prop == NULL) return;
    
    if (prop->building_count == 0) return;
    
    prop->condition_percentage -= 2;
    
    if (prop->condition_percentage < 0) {
        prop->condition_percentage = 0;
    }
    
    prop->rounds_since_maintenance++;
    
    if (prop->condition_percentage < 25) {
        printf("  WARNING: %s building is CLOSED! (Condition: %d%%)\n",
               prop->property_name, prop->condition_percentage);
    }
    
    if (prop->rounds_since_maintenance > 20 && !prop->has_structural_damage) {
        process_structural_damage(prop);
    }
}

int perform_maintenance(Player* player, int property_index) {
    if (player == NULL) return 0;
    if (property_index < 0 || property_index >= MAX_PROPERTIES) return 0;
    
    Property* prop = &property_array[property_index];
    
    if (prop->owner_id != player->player_id) {
        printf("  %s does not own %s!\n", player->player_name, prop->property_name);
        return 0;
    }
    
    if (prop->building_count == 0) {
        printf("  %s has no buildings to maintain.\n", prop->property_name);
        return 0;
    }
    
    if (prop->condition_percentage == 100) {
        printf("  %s is already in perfect condition.\n", prop->property_name);
        return 0;
    }
    
    // Calculate maintenance cost
    // House: 5% of construction cost
    // Hotel: 8% of construction cost
    int cost = get_maintenance_cost(prop);
    
    if (cost == 0) {
        printf("  Error calculating maintenance cost.\n");
        return 0;
    }
    
    if (player->cash < cost) {
        printf("  Insufficient funds! Maintenance cost: LKR %d, Available: LKR %d\n",
               cost, player->cash);
        return 0;
    }
    
    player->cash -= cost;
    
    prop->condition_percentage = 100;
    prop->rounds_since_maintenance = 0;
    
    
    printf("  %s performed maintenance on %s. (Cost: LKR %d)\n",
           player->player_name, prop->property_name, cost);
    printf("    Condition restored to 100%%.\n");
    
    return 1;
}

int get_maintenance_cost(Property* prop) {
    if (prop == NULL) return 0;
    if (prop->building_count == 0) return 0;
    
    int construction_cost;
    int percentage;
    
    if (prop->building_count == 5) {
        construction_cost = prop->hotel_construction_cost;
        percentage = 8;
    } else {
        construction_cost = prop->house_construction_cost * prop->building_count;
        percentage = 5;
    }
    
    int cost = (construction_cost * percentage) / 100;
    
    // If property has structural damage, maintenance costs 50% more
    if (prop->has_structural_damage) {
        cost = (cost * 150) / 100;
    }
    
    return cost;
}

void process_structural_damage(Property* prop) {
    if (prop == NULL) return;
    if (prop->has_structural_damage) return;
    
    printf("\n  STRUCTURAL DAMAGE on %s!\n", prop->property_name);
    printf("  Maintenance ignored for over 20 rounds.\n");
    
    prop->has_structural_damage = 1;
    
    printf("  Effects:\n");
    printf("    - Property value reduced by 15%%\n");
    printf("    - Maximum rent reduced by 25%%\n");
    printf("    - Future maintenance costs increased by 50%%\n");
    printf("  Renovation required to restore property.\n");
}

int is_building_closed(Property* prop) {
    if (prop == NULL) return 1;  // NULL considered closed
    if (prop->event_closed_rounds > 0) return 1; // Closed by Political Rally
    if (prop->has_disaster_damage) return 1; // Closed until disaster repair
    if (prop->building_count == 0) return 0;  // No building = not closed
    
    return (prop->condition_percentage < 25);
}

int needs_maintenance(Property* prop) {
    if (prop == NULL) return 0;
    if (prop->building_count == 0) return 0;
    
    return (prop->condition_percentage < 50);
}

const char* get_condition_status(Property* prop) {
    if (prop == NULL) return "Unknown";
    if (prop->building_count == 0) return "No Building";
    
    int cond = prop->condition_percentage;
    
    if (cond >= 90) return "Excellent";
    else if (cond >= 75) return "Good";
    else if (cond >= 50) return "Fair";
    else if (cond >= 25) return "Poor";
    else return "CLOSED";
}

void print_player_conditions(Player* player) {
    if (player == NULL) return;
    
    printf("\n=== %s BUILDING CONDITIONS ===\n", player->player_name);
    int has_buildings = 0;
    
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        if (property_array[i].owner_id == player->player_id &&
            property_array[i].building_count > 0) {
            
            Property* prop = &property_array[i];
            const char* status = get_condition_status(prop);
            
            printf("  %s: %d%% (%s)", 
                   prop->property_name, 
                   prop->condition_percentage,
                   status);
            
            if (prop->has_structural_damage) {
                printf(" [STRUCTURAL DAMAGE]");
            }
            if (prop->condition_percentage < 25) {
                printf(" [CLOSED]");
            }
            printf("\n");
            
            has_buildings = 1;
        }
    }
    
    if (!has_buildings) {
        printf("  No buildings.\n");
    }
    printf("================================\n");
}

// ============================================
// DEPRECIATION SYSTEM
// ============================================

void update_property_age(Property* prop) {
    if (prop == NULL) return;
    
    if (prop->owner_id == -1) return;
    
    prop->property_age++;
}

// Properties older than 50 rounds lose 1% every 5 rounds (max 30%)
int calculate_depreciation(Property* prop) {
    if (prop == NULL) return 0;
    if (prop->owner_id == -1) return 0;
    
    int age = prop->property_age;
    
    if (age <= PROPERTY_AGE_THRESHOLD) {
        return 0;
    }
    
    int over_threshold = age - PROPERTY_AGE_THRESHOLD;
    
    int depreciation = over_threshold / 5;
    
    if (depreciation > 30) {
        depreciation = 30;
    }
    
    prop->value_reduction = depreciation;
    
    return depreciation;
}

int get_depreciated_value(Property* prop) {
    return get_property_value(prop);
}

int needs_renovation(Property* prop) {
    if (prop == NULL) return 0;
    if (prop->owner_id == -1) return 0;
    
    if (prop->has_structural_damage) return 1;
    if (prop->property_age > PROPERTY_AGE_THRESHOLD) return 1;
    
    return 0;
}

// Age/depreciation renovation costs 10% of current market value (Rule-LK 17).
// Structural building renovation costs 25% of replacement value (Rule-LK 29).
int renovate_property(Player* player, int property_index) {
    if (player == NULL) return 0;
    if (property_index < 0 || property_index >= MAX_PROPERTIES) return 0;
    
    Property* prop = &property_array[property_index];
    
    if (prop->owner_id != player->player_id) {
        printf("  %s does not own %s!\n", player->player_name, prop->property_name);
        return 0;
    }
    
    if (!prop->has_structural_damage && prop->property_age <= PROPERTY_AGE_THRESHOLD) {
        printf("  %s does not need renovation. Age: %d, Threshold: %d\n",
               prop->property_name, prop->property_age, PROPERTY_AGE_THRESHOLD);
        return 0;
    }
    
    int renovation_cost;
    if (prop->has_structural_damage) {
        int replacement_value;
        if (prop->building_count == 5) {
            replacement_value = prop->hotel_construction_cost;
        } else {
            replacement_value = prop->house_construction_cost * prop->building_count;
        }
        renovation_cost = (replacement_value * 25) / 100;
    } else {
        renovation_cost = (get_property_value(prop) * 10) / 100;
    }
    
    if (player->cash < renovation_cost) {
        printf("  Insufficient funds! Renovation cost: LKR %d, Available: LKR %d\n",
               renovation_cost, player->cash);
        return 0;
    }
    
    player->cash -= renovation_cost;
    
    prop->has_structural_damage = 0;
    prop->property_age = 0;
    prop->value_reduction = 0;
    prop->condition_percentage = 100;
    prop->rounds_since_maintenance = 0;
    
    printf("  %s renovated %s. (Cost: LKR %d)\n",
           player->player_name, prop->property_name, renovation_cost);
    printf("    Property restored: age reset, value restored, condition 100%%.\n");
    
    return 1;
}

int calculate_total_property_value(Player* player, GameState* game) {
    if (player == NULL || player->is_bankrupt) return 0;

    int total_value = 0;

    for (int i = 0; i < MAX_PROPERTIES; i++) {
        Property* prop = &property_array[i];
        if (prop->owner_id != player->player_id) continue;

        int property_value = get_depreciated_value(prop);
        property_value = apply_event_value_modifiers(
            prop, game, player->player_id, property_value);
        property_value = apply_market_value_modifier(prop, game, property_value);
        total_value += property_value;

        // A hotel replaces four houses, so their values are not both counted.
        if (prop->building_count == 5) {
            total_value += prop->hotel_construction_cost;
        } else if (prop->building_count > 0) {
            total_value += prop->house_construction_cost * prop->building_count;
        }
    }

    return total_value;
}

// Rule 15 net worth. Insurance claims and taxes have no pending balances in
// the current model because both are settled immediately. Accrued interest is
// already compounded into player_loan.current_amount.
int calculate_net_worth(Player* player, GameState* game) {
    if (player == NULL || player->is_bankrupt) return 0;

    int net_worth = player->cash + calculate_total_property_value(player, game);

    if (player->player_loan.is_active) {
        net_worth -= player->player_loan.current_amount;
    }

    // The property remains an asset while its mortgage is a liability.
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        Property* prop = &property_array[i];
        if (prop->owner_id == player->player_id && prop->is_mortgaged) {
            net_worth -= prop->mortgage_value;
        }
    }

    return net_worth;
}

void print_property_depreciation(Player* player) {
    if (player == NULL) return;
    
    printf("\n=== %s PROPERTY DEPRECIATION ===\n", player->player_name);
    int has_properties = 0;
    
    for (int i = 0; i < player->owned_property_count; i++) {
        int prop_idx = player->owned_property_indices[i];
        if (prop_idx < 0 || prop_idx >= MAX_PROPERTIES) continue;
        
        Property* prop = &property_array[prop_idx];
        int depreciation = calculate_depreciation(prop);
        int current_value = get_depreciated_value(prop);
        
        printf("  %s:\n", prop->property_name);
        printf("    Age: %d rounds", prop->property_age);
        if (prop->property_age > PROPERTY_AGE_THRESHOLD) {
            printf(" (Depreciating: %d%%)", depreciation);
        } else {
            printf(" (Not depreciating)");
        }
        printf("\n");
        printf("    Current Value: LKR %d (Original: LKR %d)\n", 
               current_value, prop->purchase_price);
        
        if (prop->has_structural_damage) {
            printf("    STRUCTURAL DAMAGE - Needs renovation!\n");
        }
        if (prop->property_age > PROPERTY_AGE_THRESHOLD + 20) {
            printf("    Property is significantly aged - consider renovation.\n");
        }
        
        has_properties = 1;
    }
    
    if (!has_properties) {
        printf("  No properties owned.\n");
    }
    printf("================================\n");
}
