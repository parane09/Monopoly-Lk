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
            return aggressive_insurance_type(player, property_index) != INSURANCE_NONE;
        case STRATEGY_CONSERVATIVE:
            return conservative_should_insure(player, property_index) != INSURANCE_NONE;
        case STRATEGY_RISK_TAKER:
            return risk_taker_should_insure(player, property_index);
        case STRATEGY_OPPORTUNISTIC:
            return opportunistic_should_insure(player, property_index) != INSURANCE_NONE;
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
            return conservative_should_insure(player, property_index);
        case STRATEGY_RISK_TAKER:
            return risk_taker_should_insure(player, property_index)
                       ? INSURANCE_BASIC
                       : INSURANCE_NONE;
        case STRATEGY_OPPORTUNISTIC:
            return opportunistic_should_insure(player, property_index);
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
    
    // Rule-LK 20: increase the current bid by the minimum LKR 250.
    int next_bid = current_bid + 250;
    
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

// CONSERVATIVE BANKER SPECIFIC FUNCTIONS

int conservative_should_buy(Player* player, Property* prop) {
    // Guard clauses - validate input
    if (player == NULL || prop == NULL) return 0;
    if (player->is_bankrupt) return 0;
    
    // Check if property is unowned
    if (prop->owner_id != -1) return 0;
    
    // Check if player somehow already owns this property
    if (prop->owner_id == player->player_id) return 0;

        // PREFERENCE: Railways and Utilities (predictable income)
    if (prop->color_group == GROUP_RAILWAY || prop->color_group == GROUP_UTILITY) {
        if(player->cash > prop->purchase_price){
            return 1;
        }  // High priority
    }
    
    // Conservative Banker Rule:
    // After purchase, at least 50% of current cash must remain
    int cash_before = player->cash;
    int cash_after = cash_before - prop->purchase_price;
    
    // Need at least 50% of cash to remain
    if (cash_after < cash_before / 2) {
        return 0;  // Too expensive - would leave less than 50% cash
    }

    return 1;
}

int conservative_auction_bid(Player* player, Property* prop, int current_bid) {
    // Guard clauses - validate input
    if (player == NULL || prop == NULL) return -1;
    if (player->is_bankrupt) return -1;
    
    // Can't bid if we can't afford the current bid
    if (current_bid >= player->cash) return -1;
    
    // Conservative Banker Rule:
    // Maximum bid is the property's purchase price (market value)
    // Will NEVER bid above market value
    int max_bid = prop->purchase_price;
    
    // Also apply the 50% cash rule from buying
    // After winning auction, at least 50% cash must remain
    int cash_after_bid = player->cash - current_bid;
    if (cash_after_bid < player->cash / 2) {
        return -1;  // Would leave less than 50% cash
    }
    
    // Rule-LK 20: increase the current bid by the minimum LKR 250.
    int next_bid = current_bid + 250;
    
    // Check if next bid is within limits
    if (next_bid <= max_bid && next_bid <= player->cash) {
        return next_bid;
    }
    
    return -1;  // Withdraw - cannot bid at or above market value
}

int conservative_should_loan(Player* player) {
    // Guard clauses - validate input
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    
    // Cannot take loan if already has one
    if (player->player_loan.is_active) return 0;
    
    // Check if any collateral available
    int max_loan = get_max_loan_amount(player);
    if (max_loan == 0) return 0;
    
    // Conservative Banker Rule:
    // ONLY take loan if bankruptcy is imminent
    
    // Check 1: Cash below LKR 2,000 AND no properties to sell
    if (player->cash < 2000) { // average rent
        return 1;  // Imminent bankruptcy!
    }

    return 0;
}

int conservative_loan_amount(Player* player) {
    // Guard clauses - validate input
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    
    // Calculate maximum possible loan
    int max_loan = get_max_loan_amount(player);
    if (max_loan == 0) return 0;
    
    // Conservative Banker Rule:
    // Only borrow enough to reach LKR 5,000 (safe reserve)
    // This is the minimum cash they want to have
    int target_cash = 5000;
    int needed = target_cash - player->cash;
    
    // If cash is already above target, don't borrow
    if (needed <= 0) return 0;
    
    // Only borrow what's needed, rounded up to nearest 1000
    int loan_amount = ((needed + 999) / 1000) * 1000;
    
    // Ensure loan doesn't exceed maximum
    if (loan_amount > max_loan) {
        loan_amount = max_loan;
    }
    
    // Minimum loan amount (if less than 1000, round up to 1000)
    if (loan_amount < 1000 && loan_amount > 0) {
        loan_amount = 1000;
    }
    
    return loan_amount;
}

int conservative_should_repay(Player* player) {
    // Guard clauses - validate input
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    
    // Can only repay if there's an active loan
    if (!player->player_loan.is_active) return 0;
    
    // Conservative Banker Rule:
    // Repay immediately if sufficient funds exist
    // "Sufficient funds" means: can repay the ENTIRE loan AND still have LKR 2,000 left
    
    int loan_amount = player->player_loan.current_amount;

    if (player->cash >= loan_amount) {
        return 1;  // Repay the full loan
    }
    
    
    return 0;  // Can't afford to repay
}

int conservative_should_build(Player* player) {
    // Guard clauses - validate input
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    
    
    // Conservative Banker Rule 2:
    // Must maintain cash reserve of LKR 5,000
    int reserve = 5000;
    
    // Check all color groups for monopoly
    PropertyGroup groups[] = {
        GROUP_BROWN, GROUP_LIGHT_BLUE, GROUP_PINK, GROUP_ORANGE,
        GROUP_RED, GROUP_YELLOW, GROUP_GREEN, GROUP_DARK_BLUE
    };
    
    for (int g = 0; g < 8; g++) {
        if (has_monopoly(player, groups[g])) {
            
            // Check if any property in this group can be built on
            int can_build = 0;
            int total_cost = 0;
            
            for (int i = 0; i < MAX_PROPERTIES; i++) {
                if (property_array[i].color_group == groups[g] &&
                    property_array[i].owner_id == player->player_id) {
                    
                    // Only build houses (max 4), not hotels
                    if (property_array[i].building_count < 4) {
                        can_build = 1;
                        total_cost += property_array[i].house_construction_cost;
                    }
                }
            }
            
            // Can we afford to build AND keep LKR 5,000 reserve?
            if (can_build && player->cash >= total_cost + reserve) {
                return 1;  // Build houses
            }
        }
    }
    
    return 0;  // No building
}

int conservative_choose_build(Player* player) {
    // Guard clauses - validate input
    if (player == NULL) return -1;
    if (player->is_bankrupt) return -1;
    
    // Check all color groups for monopoly
    PropertyGroup groups[] = {
        GROUP_BROWN, GROUP_LIGHT_BLUE, GROUP_PINK, GROUP_ORANGE,
        GROUP_RED, GROUP_YELLOW, GROUP_GREEN, GROUP_DARK_BLUE
    };
    
    // Priority: Find first monopoly group with room to build
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

int conservative_should_hotel(Player* player) {
    // Guard clauses - validate input
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    
    // Conservative Banker Rule 1:
    // NO hotels if there are active loans
    if (player->player_loan.is_active) {
        return 0;  // Must settle loans first
    }
    
    // Conservative Banker Rule 2:
    // Must maintain cash reserve of LKR 5,000
    int reserve = 5000;
    
    // Check all properties owned by player
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        Property* prop = &property_array[i];
        
        // Check if player owns this property and it has 4 houses
        if (prop->owner_id == player->player_id && prop->building_count == 4) {
            
            // Check if player can afford hotel AND keep LKR 5,000 reserve
            int hotel_cost = prop->hotel_construction_cost;
            
            if (player->cash >= hotel_cost + reserve) {
                return 1;  // Build hotel!
            }
        }
    }
    
    return 0;  // No property ready for hotel upgrade
}

int conservative_choose_hotel(Player* player) {
    // Guard clauses - validate input
    if (player == NULL) return -1;
    if (player->is_bankrupt) return -1;
    
    // Check all properties owned by player
    // Choose the first property with 4 houses
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        Property* prop = &property_array[i];
        
        // Check if player owns this property and it has 4 houses
        if (prop->owner_id == player->player_id && prop->building_count == 4) {
            return i;  // Return first property with 4 houses
        }
    }
    
    return -1;  // No property ready for hotel upgrade
}

int conservative_should_insure(Player* player, int property_index) {
    // Guard clauses - validate input
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    if (property_index < 0 || property_index >= MAX_PROPERTIES) return 0;
    
    Property* prop = &property_array[property_index];
    
    // Check if player owns this property
    if (prop->owner_id != player->player_id) return 0;
    
    // Check if property already has insurance
    if (prop->insurance_policy != INSURANCE_NONE) return 0;
    
    // Conservative Banker Rule:
    // Always insure developed properties (has buildings)
    if (prop->building_count == 0) return 0;  // No buildings = no insurance
    
    // Check if player can afford the premium
    int premium = calculate_insurance_premium(prop, INSURANCE_COMPREHENSIVE);
    if (premium > player->cash) return 0;
    
    return INSURANCE_COMPREHENSIVE;  // Buy Comprehensive Insurance
}

int conservative_should_renovate(Player* player, int property_index) {
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
    
    // Conservative Banker Rule:
    // Renovate when depreciation exceeds 10%
    if (depreciation <= 10) {
        return 0;  // Not depreciated enough
    }
    
    // Calculate renovation cost: 10% of current property value
    int property_value = get_property_value(prop);
    int renovation_cost = (property_value * 10) / 100;
    
    // Conservative Banker Rule:
    // Must maintain LKR 5,000 cash reserve after renovation
    int reserve = 5000;
    int cash_after = player->cash - renovation_cost;
    
    if (cash_after < reserve) {
        return 0;  // Would drop below reserve
    }
    
    return 1;  // Renovate
}

// RISK TAKER SPECIFIC FUNCTIONS

int risk_taker_should_buy(Player* player, Property* prop) {
    // Guard clauses - validate input
    if (player == NULL || prop == NULL) return 0;
    if (player->is_bankrupt) return 0;
    
    // Check if property is unowned
    if (prop->owner_id != -1) return 0;
    
    // Check if player somehow already owns this property
    if (prop->owner_id == player->player_id) return 0;
    
    // Risk Taker Rule:
    // BUY EVERYTHING - as long as it's affordable
    // No cash reserve requirement - can leave LKR 0
    
    // Check if player can afford the property (at minimum)
    if (player->cash >= prop->purchase_price) {
        return 1;  // Buy it!
    }
    
    // Can't afford it - maybe sell something first?
    // This is handled elsewhere (selling logic)
    return 0;
}

int risk_taker_auction_bid(Player* player, Property* prop, int current_bid) {
    if (player == NULL || prop == NULL) return -1;
    if (player->is_bankrupt) return -1;
    
    // Can't bid if we can't afford current bid
    if (current_bid >= player->cash) return -1;
    
    // Rule-LK 20: increase the current bid by the minimum LKR 250.
    int next_bid = current_bid + 250;
    
    // Bid until cash is exhausted - no maximum limit
    if (next_bid <= player->cash) {
        return next_bid;
    }
    
    return -1;  // Withdraw - can't afford next bid
}

int risk_taker_should_loan(Player* player) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    
    // Already has a loan - can't take another
    if (player->player_loan.is_active) return 0;
    
    // Check if any collateral available
    int max_loan = get_max_loan_amount(player);
    if (max_loan == 0) return 0;
    
    // Risk Taker: Always borrows the maximum loan permitted
    return 1;
}

int risk_taker_loan_amount(Player* player) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    
    int max_loan = get_max_loan_amount(player);
    if (max_loan == 0) return 0;
    
    // Risk Taker: Takes maximum loan permitted
    return max_loan;
}

int risk_taker_should_repay(Player* player) {
    (void)player;
    // Risk Taker: Never repays loans
    // (Refinancing is handled separately when visiting Bank)
    return 0;
}

int risk_taker_should_build(Player* player) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    
    // Check all color groups for monopoly
    PropertyGroup groups[] = {
        GROUP_BROWN, GROUP_LIGHT_BLUE, GROUP_PINK, GROUP_ORANGE,
        GROUP_RED, GROUP_YELLOW, GROUP_GREEN, GROUP_DARK_BLUE
    };
    
    for (int g = 0; g < 8; g++) {
        if (has_monopoly(player, groups[g])) {
            // Check if any property in this group can be built on
            for (int i = 0; i < MAX_PROPERTIES; i++) {
                if (property_array[i].color_group == groups[g] &&
                    property_array[i].owner_id == player->player_id) {
                    
                    // Can build if less than 4 houses
                    if (property_array[i].building_count < 4) {
                        // Check if can afford at least one house
                        if (player->cash >= property_array[i].house_construction_cost) {
                            return 1;
                        }
                    }
                }
            }
        }
    }
    
    return 0;
}

int risk_taker_choose_build(Player* player) {
    if (player == NULL) return -1;
    if (player->is_bankrupt) return -1;
    
    // Risk Taker: Prioritize expensive property groups
    // Order: DARK_BLUE > GREEN > YELLOW > RED > ORANGE > PINK > LIGHT_BLUE > BROWN
    PropertyGroup groups[] = {
        GROUP_DARK_BLUE, GROUP_GREEN, GROUP_YELLOW, GROUP_RED,
        GROUP_ORANGE, GROUP_PINK, GROUP_LIGHT_BLUE, GROUP_BROWN
    };
    
    for (int g = 0; g < 8; g++) {
        if (has_monopoly(player, groups[g])) {
            // Find property with fewest buildings in this group
            int min_buildings = 999;
            int chosen = -1;
            
            for (int i = 0; i < MAX_PROPERTIES; i++) {
                if (property_array[i].color_group == groups[g] &&
                    property_array[i].owner_id == player->player_id) {
                    
                    if (property_array[i].building_count >= 4) continue;
                    
                    if (property_array[i].building_count < min_buildings) {
                        min_buildings = property_array[i].building_count;
                        chosen = i;
                    }
                }
            }
            
            if (chosen != -1) {
                return chosen;
            }
        }
    }
    
    return -1;
}

int risk_taker_should_hotel(Player* player) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    
    // Risk Taker: Hotels as early as possible
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        Property* prop = &property_array[i];
        
        if (prop->owner_id == player->player_id && prop->building_count == 4) {
            // Check if can afford hotel cost
            if (player->cash >= prop->hotel_construction_cost) {
                return 1;
            }
        }
    }
    
    return 0;
}

int risk_taker_choose_hotel(Player* player) {
    if (player == NULL) return -1;
    if (player->is_bankrupt) return -1;
    
    // Risk Taker: Choose the most expensive property with 4 houses
    int highest_value = 0;
    int chosen = -1;
    
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        Property* prop = &property_array[i];
        
        if (prop->owner_id == player->player_id && prop->building_count == 4) {
            int value = get_property_value(prop);
            if (value > highest_value) {
                highest_value = value;
                chosen = i;
            }
        }
    }
    
    return chosen;
}

int risk_taker_should_insure(Player* player, int property_index) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    if (property_index < 0 || property_index >= MAX_PROPERTIES) return 0;
    
    Property* prop = &property_array[property_index];
    
    // Check if player owns this property
    if (prop->owner_id != player->player_id) return 0;
    
    // Check if property already has insurance
    if (prop->insurance_policy != INSURANCE_NONE) return 0;
    
    // Check if property has buildings
    if (prop->building_count == 0) return 0;
    
    // Risk Taker Rule:
    // Purchases insurance only after experiencing a financial loss
    // For now, check if player has ever suffered a disaster loss
    // This could be tracked in player struct with a flag like 'has_suffered_loss'
    
    // Simplified: Risk Taker never buys insurance proactively
    // They only buy if they've already lost money to a disaster
    // Since we don't track losses yet, they never buy
    return 0;
}

int risk_taker_should_renovate(Player* player, int property_index) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    if (property_index < 0 || property_index >= MAX_PROPERTIES) return 0;
    
    Property* prop = &property_array[property_index];
    
    // Check if player owns this property
    if (prop->owner_id != player->player_id) return 0;
    
    // Check if property has buildings
    if (prop->building_count == 0) return 0;
    
    // Risk Taker Rule:
    // Ignores property depreciation until repair becomes unavoidable
    // Only renovate when depreciation exceeds 20%
    int depreciation = calculate_depreciation(prop);
    if (depreciation <= 20) {
        return 0;
    }
    
    // Check if can afford renovation
    int property_value = get_property_value(prop);
    int renovation_cost = (property_value * 10) / 100;
    
    if (renovation_cost > player->cash) {
        return 0;
    }
    
    return 1;
}

// Oppurtunistic trader SPECIFIC FUNCTIONS

// Calculate ROI (Return on Investment) for a property
static int calculate_roi(Player* player, Property* prop) {
    if (player == NULL || prop == NULL) return 0;
    if (prop->owner_id != -1) return 0;
    
    int cost = prop->purchase_price;
    int annual_rent = prop->base_rent * 10;  // Estimate 10 visits per round
    
    // If completing a monopoly, rent doubles
    if (would_complete_monopoly(player, prop->color_group)) {
        annual_rent *= 2;
    }
    
    if (cost == 0) return 0;
    return (annual_rent * 100) / cost;
}

int opportunistic_should_buy(Player* player, Property* prop) {
    if (player == NULL || prop == NULL) return 0;
    if (player->is_bankrupt) return 0;
    if (prop->owner_id != -1) return 0;
    if (prop->owner_id == player->player_id) return 0;
    
    // Opportunistic Trader: Buy if projected appreciation exceeds construction costs
    // For now, use ROI > 10% as a proxy
    int roi = calculate_roi(player, prop);
    
    // Also check if can afford
    if (player->cash < prop->purchase_price) return 0;
    
    // Buy if ROI > 10% (good investment)
    if (roi > 10) {
        return 1;
    }
    
    // Special case: Would complete a monopoly
    if (would_complete_monopoly(player, prop->color_group)) {
        return 1;
    }
    
    return 0;
}

int opportunistic_auction_bid(Player* player, Property* prop, int current_bid) {
    if (player == NULL || prop == NULL) return -1;
    if (player->is_bankrupt) return -1;
    
    // Can't bid if we can't afford current bid
    if (current_bid >= player->cash) return -1;
    
    // Calculate ROI to determine max bid
    int roi = calculate_roi(player, prop);
    
    // Max bid based on ROI:
    // ROI > 20% → bid up to 110% of value
    // ROI > 15% → bid up to 105% of value
    // ROI > 10% → bid up to 100% of value
    // ROI <= 10% → don't bid
    int max_bid = 0;
    
    if (roi > 20) {
        max_bid = (prop->purchase_price * 110) / 100;
    } else if (roi > 15) {
        max_bid = (prop->purchase_price * 105) / 100;
    } else if (roi > 10) {
        max_bid = prop->purchase_price;
    } else {
        return -1;  // ROI too low, withdraw
    }
    
    // Also consider 50% cash rule (buying property leaves some cash)
    int cash_after = player->cash - current_bid;
    if (cash_after < player->cash / 2) {
        // Would leave less than 50% cash, reduce max bid
        int safe_bid = player->cash / 2;
        if (safe_bid < max_bid) {
            max_bid = safe_bid;
        }
    }
    
    // Rule-LK 20: increase the current bid by the minimum LKR 250.
    int next_bid = current_bid + 250;
    
    if (next_bid <= max_bid && next_bid <= player->cash) {
        return next_bid;
    }
    
    return -1;  // Withdraw
}

int opportunistic_should_loan(Player* player) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    if (player->player_loan.is_active) return 0;
    
    int max_loan = get_max_loan_amount(player);
    if (max_loan == 0) return 0;
    
    // Opportunistic Trader: Take loan if projected return exceeds borrowing cost
    // For now, check if there's any property with ROI > 15%
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        Property* prop = &property_array[i];
        if (prop->owner_id == -1) {
            int roi = calculate_roi(player, prop);
            if (roi > 15) {
                return 1;  // Good investment opportunity
            }
        }
    }
    
    // Also check if loan would complete a monopoly
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        Property* prop = &property_array[i];
        if (prop->owner_id == -1) {
            if (would_complete_monopoly(player, prop->color_group)) {
                if (player->cash + max_loan >= prop->purchase_price) {
                    return 1;
                }
            }
        }
    }
    
    return 0;
}

int opportunistic_loan_amount(Player* player) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    
    int max_loan = get_max_loan_amount(player);
    if (max_loan == 0) return 0;
    
    // Opportunistic Trader: Borrow 70% of max to leave room
    int loan_amount = (max_loan * 70) / 100;
    
    // Round down to nearest 1000
    loan_amount = (loan_amount / 1000) * 1000;
    
    // Ensure minimum loan
    if (loan_amount < 1000 && max_loan >= 1000) {
        loan_amount = 1000;
    }
    
    return loan_amount;
}

int opportunistic_should_repay(Player* player) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    if (!player->player_loan.is_active) return 0;
    
    // Opportunistic Trader: Repay if no good investment opportunities
    // Check if there are any properties with ROI > current loan interest rate
    int interest_rate = player->player_loan.interest_rate;
    
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        Property* prop = &property_array[i];
        if (prop->owner_id == -1) {
            int roi = calculate_roi(player, prop);
            if (roi > interest_rate) {
                return 0;  // Keep loan, invest instead
            }
        }
    }
    
    // No good opportunities - repay the loan
    return 1;
}

int opportunistic_should_build(Player* player) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    
    // Check all color groups for monopoly
    PropertyGroup groups[] = {
        GROUP_BROWN, GROUP_LIGHT_BLUE, GROUP_PINK, GROUP_ORANGE,
        GROUP_RED, GROUP_YELLOW, GROUP_GREEN, GROUP_DARK_BLUE
    };
    
    for (int g = 0; g < 8; g++) {
        if (has_monopoly(player, groups[g])) {
            // Check if any property in this group can be built on
            for (int i = 0; i < MAX_PROPERTIES; i++) {
                if (property_array[i].color_group == groups[g] &&
                    property_array[i].owner_id == player->player_id) {
                    
                    if (property_array[i].building_count < 4) {
                        // Check if can afford at least one house
                        if (player->cash >= property_array[i].house_construction_cost) {
                            return 1;
                        }
                    }
                }
            }
        }
    }
    
    return 0;
}

int opportunistic_choose_build(Player* player) {
    if (player == NULL) return -1;
    if (player->is_bankrupt) return -1;
    
    // Opportunistic Trader uses standard even building rule
    // But priorities groups that are currently booming (future enhancement)
    // For now, use default order: DARK_BLUE > GREEN > YELLOW > RED > ORANGE > PINK > LIGHT_BLUE > BROWN
    
    PropertyGroup groups[] = {
        GROUP_DARK_BLUE, GROUP_GREEN, GROUP_YELLOW, GROUP_RED,
        GROUP_ORANGE, GROUP_PINK, GROUP_LIGHT_BLUE, GROUP_BROWN
    };
    
    for (int g = 0; g < 8; g++) {
        if (has_monopoly(player, groups[g])) {
            // Find property with fewest buildings
            int min_buildings = 999;
            int chosen = -1;
            
            for (int i = 0; i < MAX_PROPERTIES; i++) {
                if (property_array[i].color_group == groups[g] &&
                    property_array[i].owner_id == player->player_id) {
                    
                    if (property_array[i].building_count >= 4) continue;
                    
                    if (property_array[i].building_count < min_buildings) {
                        min_buildings = property_array[i].building_count;
                        chosen = i;
                    }
                }
            }
            
            if (chosen != -1) {
                return chosen;
            }
        }
    }
    
    return -1;
}

int opportunistic_should_hotel(Player* player) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    
    // Opportunistic Trader: Build hotel if profitable
    // For now, check if any property has 4 houses
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        Property* prop = &property_array[i];
        
        if (prop->owner_id == player->player_id && prop->building_count == 4) {
            // Can we afford hotel?
            if (player->cash >= prop->hotel_construction_cost) {
                // Calculate ROI on hotel upgrade
                int current_rent = prop->base_rent * 7;  // 4 houses = 7x
                int future_rent = prop->base_rent * 10;  // Hotel = 10x
                int rent_increase = future_rent - current_rent;
                int cost = prop->hotel_construction_cost;
                
                // ROI = rent increase / cost
                if (rent_increase * 100 / cost > 10) {  // > 10% ROI
                    return 1;
                }
            }
        }
    }
    
    return 0;
}

int opportunistic_choose_hotel(Player* player) {
    if (player == NULL) return -1;
    if (player->is_bankrupt) return -1;
    
    // Opportunistic Trader: Choose most valuable property with 4 houses
    int highest_value = 0;
    int chosen = -1;
    
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        Property* prop = &property_array[i];
        
        if (prop->owner_id == player->player_id && prop->building_count == 4) {
            int value = get_property_value(prop);
            if (value > highest_value) {
                highest_value = value;
                chosen = i;
            }
        }
    }
    
    return chosen;
}

int opportunistic_should_insure(Player* player, int property_index) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    if (property_index < 0 || property_index >= MAX_PROPERTIES) return 0;
    
    Property* prop = &property_array[property_index];
    
    if (prop->owner_id != player->player_id) return 0;
    if (prop->insurance_policy != INSURANCE_NONE) return 0;
    if (prop->building_count == 0) return 0;
    
    // Opportunistic Trader: Purchase Comprehensive Insurance only for high-value developments
    int property_value = get_property_value(prop);
    
    // Only insure properties worth more than LKR 20,000
    if (property_value < 20000) {
        return 0;
    }
    
    // Check if can afford premium
    int premium = calculate_insurance_premium(prop, INSURANCE_COMPREHENSIVE);
    if (premium > player->cash) {
        return 0;
    }
    
    return INSURANCE_COMPREHENSIVE;
}

int opportunistic_should_renovate(Player* player, int property_index) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    if (property_index < 0 || property_index >= MAX_PROPERTIES) return 0;
    
    Property* prop = &property_array[property_index];
    
    if (prop->owner_id != player->player_id) return 0;
    if (prop->building_count == 0) return 0;
    
    // Opportunistic Trader: Renovate when depreciation exceeds 15%
    int depreciation = calculate_depreciation(prop);
    if (depreciation <= 15) {
        return 0;
    }
    
    // Check if can afford renovation
    int property_value = get_property_value(prop);
    int renovation_cost = (property_value * 10) / 100;
    
    if (renovation_cost > player->cash) {
        return 0;
    }
    
    return 1;
}


