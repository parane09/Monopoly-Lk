#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================
// FORWARD DECLARATIONS
// ============================================

// Helper functions
static int calculate_roi(Player* player, Property* prop);
static int can_afford_rent(Player* player);
static int get_property_priority(Property* prop);
static int get_group_development_status(Player* player, PropertyGroup group);

// ============================================
// INTERNAL HELPER FUNCTIONS
// ============================================

// Calculate Return on Investment for a property
static int calculate_roi(Player* player, Property* prop) {
    if (player == NULL || prop == NULL) return 0;
    if (prop->owner_id != -1) return 0;
    
    int cost = prop->purchase_price;
    int annual_rent = prop->base_rent * 10;  // Estimate 10 visits per round
    
    // If completing a monopoly, rent doubles
    if (has_monopoly(player, prop->color_group)) {
        annual_rent *= 2;
    }
    
    if (cost == 0) return 0;
    return (annual_rent * 100) / cost;
}

// Check if player can afford average rent payment
static int can_afford_rent(Player* player) {
    if (player == NULL) return 0;
    return (player->cash >= 500);  // Average rent ~LKR 500
}

// Get property priority (higher = more desirable)
static int get_property_priority(Property* prop) {
    if (prop == NULL) return 0;
    
    int priority = 0;
    
    // Premium properties get highest priority
    if (strcmp(prop->property_name, "Galle Face") == 0) priority = 10;
    else if (strcmp(prop->property_name, "Nuwara Eliya") == 0) priority = 9;
    else if (prop->color_group == GROUP_DARK_BLUE) priority = 8;
    else if (prop->color_group == GROUP_GREEN) priority = 7;
    else if (prop->color_group == GROUP_YELLOW) priority = 6;
    else if (prop->color_group == GROUP_RED) priority = 5;
    else if (prop->color_group == GROUP_ORANGE) priority = 4;
    else if (prop->color_group == GROUP_PINK) priority = 3;
    else if (prop->color_group == GROUP_LIGHT_BLUE) priority = 2;
    else if (prop->color_group == GROUP_BROWN) priority = 1;
    
    return priority;
}

// Get development status of a group (how many houses built)
static int get_group_development_status(Player* player, PropertyGroup group) {
    if (player == NULL) return 0;
    
    int total_buildings = 0;
    int property_count = 0;
    
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        if (property_array[i].color_group == group &&
            property_array[i].owner_id == player->player_id) {
            total_buildings += property_array[i].building_count;
            property_count++;
        }
    }
    
    if (property_count == 0) return 0;
    return total_buildings / property_count;  // Average buildings per property
}

// ============================================
// STRATEGY DISPATCHER
// ============================================

// ============================================
// STRATEGY DISPATCHER
// ============================================

int should_buy_property(Player* player, Property* prop) {
    if (player == NULL || prop == NULL) return 0;
    if (player->is_bankrupt) return 0;
    
    switch (player->strategy) {
        case STRATEGY_AGGRESSIVE:
            return aggressive_should_buy(player, prop);
        case STRATEGY_CONSERVATIVE:
            return conservative_should_buy(player, prop);
        case STRATEGY_RISK_TAKER:
            return risk_taker_should_buy(player, prop);
        case STRATEGY_OPPORTUNISTIC:
            return opportunistic_should_buy(player, prop);
        default:
            return 0;
    }
}

int get_auction_bid(Player* player, Property* prop, int current_bid) {
    if (player == NULL || prop == NULL) return -1;
    if (player->is_bankrupt) return -1;
    
    switch (player->strategy) {
        case STRATEGY_AGGRESSIVE:
            return aggressive_auction_bid(player, prop, current_bid);
        case STRATEGY_CONSERVATIVE:
            return conservative_auction_bid(player, prop, current_bid);
        case STRATEGY_RISK_TAKER:
            return risk_taker_auction_bid(player, prop, current_bid);
        case STRATEGY_OPPORTUNISTIC:
            return opportunistic_auction_bid(player, prop, current_bid);
        default:
            return -1;
    }
}

int should_take_loan(Player* player) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    if (player->player_loan.is_active) return 0;
    
    switch (player->strategy) {
        case STRATEGY_AGGRESSIVE:
            return aggressive_should_loan(player);
        case STRATEGY_CONSERVATIVE:
            return conservative_should_loan(player);
        case STRATEGY_RISK_TAKER:
            return risk_taker_should_loan(player);
        case STRATEGY_OPPORTUNISTIC:
            return opportunistic_should_loan(player);
        default:
            return 0;
    }
}

int get_loan_amount(Player* player) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    
    switch (player->strategy) {
        case STRATEGY_AGGRESSIVE:
            return aggressive_loan_amount(player);
        case STRATEGY_CONSERVATIVE:
            return conservative_loan_amount(player);
        case STRATEGY_RISK_TAKER:
            return risk_taker_loan_amount(player);
        case STRATEGY_OPPORTUNISTIC:
            return opportunistic_loan_amount(player);
        default:
            return 0;
    }
}

int should_repay_loan(Player* player) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    if (!player->player_loan.is_active) return 0;
    
    switch (player->strategy) {
        case STRATEGY_AGGRESSIVE:
            return aggressive_should_repay(player);
        case STRATEGY_CONSERVATIVE:
            return conservative_should_repay(player);
        case STRATEGY_RISK_TAKER:
            return risk_taker_should_repay(player);
        case STRATEGY_OPPORTUNISTIC:
            return opportunistic_should_repay(player);
        default:
            return 0;
    }
}

int should_build(Player* player) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    
    switch (player->strategy) {
        case STRATEGY_AGGRESSIVE:
            return aggressive_should_build(player);
        case STRATEGY_CONSERVATIVE:
            return conservative_should_build(player);
        case STRATEGY_RISK_TAKER:
            return risk_taker_should_build(player);
        case STRATEGY_OPPORTUNISTIC:
            return opportunistic_should_build(player);
        default:
            return 0;
    }
}

int choose_build_property(Player* player) {
    if (player == NULL) return -1;
    if (player->is_bankrupt) return -1;
    
    switch (player->strategy) {
        case STRATEGY_AGGRESSIVE:
            return aggressive_choose_build(player);
        case STRATEGY_CONSERVATIVE:
            return conservative_choose_build(player);
        case STRATEGY_RISK_TAKER:
            return risk_taker_choose_build(player);
        case STRATEGY_OPPORTUNISTIC:
            return opportunistic_choose_build(player);
        default:
            return -1;
    }
}

int should_build_hotel(Player* player) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    
    switch (player->strategy) {
        case STRATEGY_AGGRESSIVE:
            return aggressive_should_hotel(player);
        case STRATEGY_CONSERVATIVE:
            return conservative_should_hotel(player);
        case STRATEGY_RISK_TAKER:
            return risk_taker_should_hotel(player);
        case STRATEGY_OPPORTUNISTIC:
            return opportunistic_should_hotel(player);
        default:
            return 0;
    }
}

int choose_hotel_property(Player* player) {
    if (player == NULL) return -1;
    if (player->is_bankrupt) return -1;
    
    switch (player->strategy) {
        case STRATEGY_AGGRESSIVE:
            return aggressive_choose_hotel(player);
        case STRATEGY_CONSERVATIVE:
            return conservative_choose_hotel(player);
        case STRATEGY_RISK_TAKER:
            return risk_taker_choose_hotel(player);
        case STRATEGY_OPPORTUNISTIC:
            return opportunistic_choose_hotel(player);
        default:
            return -1;
    }
}

int should_buy_insurance(Player* player, int property_index) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    if (property_index < 0 || property_index >= MAX_PROPERTIES) return 0;
    
    switch (player->strategy) {
        case STRATEGY_AGGRESSIVE:
            return aggressive_should_insure(player, property_index);
        case STRATEGY_CONSERVATIVE:
            return conservative_should_insure(player, property_index);
        case STRATEGY_RISK_TAKER:
            return risk_taker_should_insure(player, property_index);
        case STRATEGY_OPPORTUNISTIC:
            return opportunistic_should_insure(player, property_index);
        default:
            return 0;
    }
}

int get_insurance_type(Player* player, int property_index) {
    if (player == NULL) return INSURANCE_NONE;
    if (player->is_bankrupt) return INSURANCE_NONE;
    if (property_index < 0 || property_index >= MAX_PROPERTIES) return INSURANCE_NONE;
    
    switch (player->strategy) {
        case STRATEGY_AGGRESSIVE:
            return aggressive_insurance_type(player, property_index);
        case STRATEGY_CONSERVATIVE:
            return conservative_insurance_type(player, property_index);
        case STRATEGY_RISK_TAKER:
            return risk_taker_insurance_type(player, property_index);
        case STRATEGY_OPPORTUNISTIC:
            return opportunistic_insurance_type(player, property_index);
        default:
            return INSURANCE_NONE;
    }
}

int should_renovate(Player* player, int property_index) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    if (property_index < 0 || property_index >= MAX_PROPERTIES) return 0;
    
    switch (player->strategy) {
        case STRATEGY_AGGRESSIVE:
            return aggressive_should_renovate(player, property_index);
        case STRATEGY_CONSERVATIVE:
            return conservative_should_renovate(player, property_index);
        case STRATEGY_RISK_TAKER:
            return risk_taker_should_renovate(player, property_index);
        case STRATEGY_OPPORTUNISTIC:
            return opportunistic_should_renovate(player, property_index);
        default:
            return 0;
    }
}

// Add this to the helpers section at the top of players.c
static int would_complete_monopoly(Player* player, PropertyGroup group) {
    if (player == NULL) return 0;
    
    int owned_in_group = 0;
    int total_in_group = 0;
    
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        if (property_array[i].color_group == group) {
            total_in_group++;
            if (property_array[i].owner_id == player->player_id) {
                owned_in_group++;
            }
        }
    }
    
    // Would complete monopoly if we own ALL BUT ONE property in the group
    return (owned_in_group == total_in_group - 1 && total_in_group > 0);
}


// AGGRESSIVE INVESTOR SPECIFIC FUNCTIONS

int aggressive_should_buy(Player* player, Property* prop) {
    // Guard clauses - validate input
    if (player == NULL || prop == NULL) return 0;
    if (player->is_bankrupt) return 0;
    
    // Check if property is unowned (AI should only be called for unowned properties)
    if (prop->owner_id != -1) return 0;

    if(player->cash < prop->purchase_price){
        return 0;
    }

    // HIGH PRIORITY: Premium properties (Dark Blue - Nuwara Eliya, Galle Face)
    if (prop->color_group == GROUP_DARK_BLUE || would_complete_monopoly(player, prop->color_group)) {
        return 1;
    }

    // Can we afford purchase AND still have at least LKR 500 left for rent?
    if (player->cash < prop->purchase_price + 500) {
        return 0;
    }
    return 1;
}

int aggressive_auction_bid(Player* player, Property* prop, int current_bid) {
    // Guard clauses - validate input
    if (player == NULL || prop == NULL) return -1;
    if (player->is_bankrupt) return -1;
    
    // Check if player can afford the current bid (minimum to participate)
    if (current_bid >= player->cash) return -1;
    
    // Calculate maximum bid: 120% of property purchase price
    int max_bid = (prop->purchase_price * 120) / 100;
    
    // Calculate next bid (round up to nearest multiple of 250)
    int next_bid = ((current_bid + 250) / 250) * 250;
    
    // Check if next bid exceeds maximum allowed or player's cash
    if (next_bid <= max_bid && next_bid <= player->cash) {
        return next_bid;  // Bid this amount
    }
    
    // If cannot bid, withdraw
    return -1;
}

int aggressive_should_loan(Player* player) {
    // Guard clauses - validate input
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    
    // Cannot take loan if already has one (only one active loan allowed)
    if (player->player_loan.is_active) return 0;
    
    // Check if any collateral available
    int max_loan = get_max_loan_amount(player);
    if (max_loan == 0) return 0;
    
    // REASON 1: Complete a monopoly
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        Property* prop = &property_array[i];
        if (prop->owner_id == -1) {
            if (would_complete_monopoly(player, prop->color_group)) {
                // Can we afford the property with loan + cash?
                if (player->cash < prop->purchase_price &&
                    player->cash + max_loan >= prop->purchase_price) {
                    return 1;
                }
            }
        }
    }
    
    // REASON 2: Build the next legal, worthwhile house when a loan is needed.
    PropertyGroup groups[] = {
        GROUP_BROWN, GROUP_LIGHT_BLUE, GROUP_PINK, GROUP_ORANGE,
        GROUP_RED, GROUP_YELLOW, GROUP_GREEN, GROUP_DARK_BLUE
    };
    
    for (int g = 0; g < 8; g++) {
        if (has_monopoly(player, groups[g])) {
            int minimum_buildings =
                get_min_buildings_in_group(player, groups[g]);

            for (int i = 0; i < MAX_PROPERTIES; i++) {
                Property* prop = &property_array[i];

                if (prop->color_group == groups[g] &&
                    prop->owner_id == player->player_id &&
                    prop->building_count < 4 &&
                    minimum_buildings >= 0 &&
                    prop->building_count == minimum_buildings) {
                    int current_rent = prop->base_rent *
                        get_building_multiplier(prop->building_count);
                    int future_rent = prop->base_rent *
                        get_building_multiplier(prop->building_count + 1);
                    int cost = prop->house_construction_cost;
                    int needs_loan = player->cash < cost;
                    int can_afford_with_loan =
                        player->cash + max_loan >= cost;
                    int worthwhile =
                        future_rent - current_rent > (cost * 15) / 100;

                    if (needs_loan && can_afford_with_loan && worthwhile) {
                        return 1;
                    }
                }
            }
        }
    }
    
    return 0;
}

int aggressive_loan_amount(Player* player) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    if (player->player_loan.is_active) return 0;
    
    int max_loan = get_max_loan_amount(player);
    if (max_loan <= 0) return 0;

    int best_shortfall = 0;

    // Borrow only the amount needed to acquire a property that completes
    // a developable colour monopoly.
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        Property* prop = &property_array[i];
        int is_developable_group =
            prop->color_group >= GROUP_BROWN &&
            prop->color_group <= GROUP_DARK_BLUE;

        if (prop->owner_id == -1 && is_developable_group &&
            would_complete_monopoly(player, prop->color_group)) {
            int shortfall = prop->purchase_price - player->cash;

            if (shortfall > 0 && shortfall <= max_loan &&
                (best_shortfall == 0 || shortfall < best_shortfall)) {
                best_shortfall = shortfall;
            }
        }
    }

    if (best_shortfall > 0) {
        return best_shortfall;
    }

    // Otherwise, borrow only the amount needed for the next legal house
    // whose rent increase satisfies the Aggressive Investor's threshold.
    PropertyGroup groups[] = {
        GROUP_BROWN, GROUP_LIGHT_BLUE, GROUP_PINK, GROUP_ORANGE,
        GROUP_RED, GROUP_YELLOW, GROUP_GREEN, GROUP_DARK_BLUE
    };

    for (int g = 0; g < 8; g++) {
        if (!has_monopoly(player, groups[g])) continue;

        int minimum_buildings =
            get_min_buildings_in_group(player, groups[g]);

        for (int i = 0; i < MAX_PROPERTIES; i++) {
            Property* prop = &property_array[i];

            if (prop->owner_id != player->player_id ||
                prop->color_group != groups[g] ||
                prop->building_count >= 4 ||
                minimum_buildings < 0 ||
                prop->building_count != minimum_buildings) {
                continue;
            }

            int current_rent = prop->base_rent *
                get_building_multiplier(prop->building_count);
            int future_rent = prop->base_rent *
                get_building_multiplier(prop->building_count + 1);
            int cost = prop->house_construction_cost;
            int shortfall = cost - player->cash;
            int worthwhile =
                future_rent - current_rent > (cost * 15) / 100;

            if (worthwhile && shortfall > 0 && shortfall <= max_loan &&
                (best_shortfall == 0 || shortfall < best_shortfall)) {
                best_shortfall = shortfall;
            }
        }
    }

    return best_shortfall;
}

int aggressive_should_repay(Player* player) {
    // Guard clauses - validate input
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    
    // Can only repay if there's an active loan
    if (!player->player_loan.is_active) return 0;
    
    // Calculate excess cash (keeping LKR 5,000 as reserve)
    int reserve = 5000;
    int excess_cash = player->cash - reserve;
    
    // Only repay if excess cash is MORE than double the outstanding loan
    // This means: we have so much cash that keeping the loan isn't worth it
    if (excess_cash > player->player_loan.current_amount * 2) {
        return 1;  // Repay the loan
    }
    
    // Also check: if loan is about to default (1 round left), repay
    if (player->player_loan.rounds_remaining <= 1) {
        // Can we afford to repay at least half?
        if (player->cash >= player->player_loan.current_amount / 2) {
            return 1;
        }
    }
    
    return 0;  // Don't repay yet
}

int aggressive_should_build(Player* player) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;

    PropertyGroup groups[] = {
        GROUP_BROWN, GROUP_LIGHT_BLUE, GROUP_PINK, GROUP_ORANGE,
        GROUP_RED, GROUP_YELLOW, GROUP_GREEN, GROUP_DARK_BLUE
    };

    for (int g = 0; g < 8; g++) {
        if (!has_monopoly(player, groups[g])) continue;

        int minimum_buildings =
            get_min_buildings_in_group(player, groups[g]);

        for (int i = 0; i < MAX_PROPERTIES; i++) {
            Property* prop = &property_array[i];

            if (prop->owner_id == player->player_id &&
                prop->color_group == groups[g] &&
                prop->building_count < 4 &&
                minimum_buildings >= 0 &&
                prop->building_count == minimum_buildings &&
                player->cash >= prop->house_construction_cost) {
                return 1;
            }
        }
    }
    
    return 0;
}

int aggressive_choose_build(Player* player) {
    // Guard clauses - validate input
    if (player == NULL) return -1;
    if (player->is_bankrupt) return -1;
    
    // Check all color groups for monopoly
    PropertyGroup groups[] = {
        GROUP_BROWN, GROUP_LIGHT_BLUE, GROUP_PINK, GROUP_ORANGE,
        GROUP_RED, GROUP_YELLOW, GROUP_GREEN, GROUP_DARK_BLUE
    };
    
    // Priority 1: Find first monopoly group with room to build
    for (int g = 0; g < 8; g++) {
        if (has_monopoly(player, groups[g])) {
            
            // Find property with FEWEST buildings in this group
            int min_buildings = 999;
            int chosen_property = -1;
            
            for (int i = 0; i < MAX_PROPERTIES; i++) {
                if (property_array[i].color_group == groups[g] &&
                    property_array[i].owner_id == player->player_id) {
                    
                    // Skip properties that already have 4 houses (can't build more)
                    if (property_array[i].building_count >= 4) continue;
                    
                    // Find the property with the fewest buildings
                    if (property_array[i].building_count < min_buildings) {
                        min_buildings = property_array[i].building_count;
                        chosen_property = i;
                    }
                }
            }
            
            // If we found a property to build on, return it
            if (chosen_property != -1) {
                return chosen_property;
            }
        }
    }
    
    return -1;  // No buildable property found
}

int aggressive_should_hotel(Player* player) {
    // Guard clauses - validate input
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    
    // Check all properties owned by player
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        Property* prop = &property_array[i];
        
        // Check if player owns this property and it has 4 houses
        if (prop->owner_id == player->player_id && prop->building_count == 4) {
            
            // Check if player can afford the hotel cost
            if (player->cash >= prop->hotel_construction_cost) {
                return 1;  // Build hotel!
            }
        }
    }
    
    return 0;  // No property ready for hotel upgrade
}

int aggressive_choose_hotel(Player* player) {
    // Guard clauses - validate input
    if (player == NULL) return -1;
    if (player->is_bankrupt) return -1;
    
    // Check all properties owned by player
    // Choose the first property with 4 houses (most valuable priority)
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        Property* prop = &property_array[i];
        
        // Check if player owns this property and it has 4 houses
        if (prop->owner_id == player->player_id && prop->building_count == 4) {
            return i;  // Return first property with 4 houses
        }
    }
    
    return -1;  // No property ready for hotel upgrade
}

int aggressive_insurance_type(Player* player, int property_index) {
    // Guard clauses - validate input
    if (player == NULL) return INSURANCE_NONE;
    if (player->is_bankrupt) return INSURANCE_NONE;
    if (property_index < 0 || property_index >= MAX_PROPERTIES) return INSURANCE_NONE;
    
    Property* prop = &property_array[property_index];
    
    // Check if player owns this property
    if (prop->owner_id != player->player_id) return INSURANCE_NONE;
    
    // Check if property has buildings
    if (prop->building_count == 0) return INSURANCE_NONE;
    
    // Check if property already has insurance
    if (prop->insurance_policy != INSURANCE_NONE) return INSURANCE_NONE;
    
    // Determine insurance type based on building type
    if (prop->building_count == 5) { // means has a hotel
        // Hotel: Comprehensive Insurance
        return INSURANCE_COMPREHENSIVE;
    } else {
        // Houses: Basic Insurance
        return INSURANCE_BASIC;
    }
}

int aggressive_should_renovate(Player* player, int property_index) {
    // Guard clauses - validate input
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    if (property_index < 0 || property_index >= MAX_PROPERTIES) return 0;
    
    Property* prop = &property_array[property_index];
    
    // Check if player owns this property
    if (prop->owner_id != player->player_id) return 0;
    
    // Check if property has buildings (only developed properties need renovation)
    if (prop->building_count == 0) return 0;
    
    // Check depreciation level
    int depreciation = calculate_depreciation(prop);
    
    // Aggressive renovates when depreciation exceeds 10%
    if (depreciation <= 10) {
        return 0;  // Not depreciated enough
    }
    
    // Check if player can afford renovation
    // Renovation cost = 10% of current property value
    int property_value = get_property_value(prop);
    int renovation_cost = (property_value * 10) / 100;
    
    if (renovation_cost > player->cash) {
        return 0;  // Can't afford
    }
    
    return 1;  // Renovate!
}

