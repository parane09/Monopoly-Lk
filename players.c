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

    if(player->cash < prop->purchas_price){
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
                if (player->cash + max_loan >= prop->purchase_price) {
                    return 1;
                }
            }
        }
    }
    
    // REASON 2: Build houses on existing monopoly
    PropertyGroup groups[] = {
        GROUP_BROWN, GROUP_LIGHT_BLUE, GROUP_PINK, GROUP_ORANGE,
        GROUP_RED, GROUP_YELLOW, GROUP_GREEN, GROUP_DARK_BLUE
    };
    
    for (int g = 0; g < 8; g++) {
        if (has_monopoly(player, groups[g])) {
            // Check if we can build at least 2 houses
            for (int i = 0; i < MAX_PROPERTIES; i++) {
                if (property_array[i].color_group == groups[g] &&
                    property_array[i].owner_id == player->player_id) {
                    
                    if (property_array[i].building_count < 4) {
                        // Check if building would increase rent enough
                        int current_rent = property_array[i].base_rent;
                        int future_rent = current_rent * 2;  // With houses
                        int cost = property_array[i].house_construction_cost;
                        
                        // If rent increase > 15% of cost, worth borrowing
                        if ((future_rent - current_rent) > (cost * 15) / 100) {
                            return 1;
                        }
                    }
                }
            }
        }
    }
    
    return 0;
}