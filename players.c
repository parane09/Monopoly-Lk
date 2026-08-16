#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static GameState* strategy_game_state = NULL;


int average_rent = 500;

// Helper functions
static int calculate_roi(Player* player, Property* prop);
static int get_adjusted_market_value(Player* player, Property* prop);
static int is_economic_recession_active(void);
static int get_projected_rent(Player* player, Property* prop);
static int get_projected_appreciation(Player* player, Property* prop);
static int get_projected_property_return(Player* player, Property* prop,int investment_cost);
static int is_balanced_portfolio_purchase(Player* player, Property* prop);
static int get_development_return(Player* player, Property* prop,int next_building_count);

void set_player_game_state(GameState* game) {
    strategy_game_state = game;
    average_rent = 500;
}

void update_average_rent_for_inflation(int inflation_rate) {
    average_rent = (average_rent * (100 + inflation_rate)) / 100;

    if (average_rent < 1) {
        average_rent = 1;
    }
}

int get_adjusted_purchase_price(Player* player, Property* prop) {
    if (prop == NULL) return 0;

    int price = get_property_value(prop);
    if (strategy_game_state == NULL || player == NULL) return price;

    price = apply_event_value_modifiers(
        prop, strategy_game_state, player->player_id, price);
    price = apply_market_purchase_modifier(
        prop, strategy_game_state, price);
    return price;
}

static int get_adjusted_market_value(Player* player, Property* prop) {
    if (prop == NULL) return 0;

    int value = get_property_value(prop);
    if (strategy_game_state == NULL || player == NULL) return value;

    value = apply_event_value_modifiers(
        prop, strategy_game_state, player->player_id, value);
    value = apply_market_value_modifier(prop, strategy_game_state, value);
    return value;
}

static int is_economic_recession_active(void) {
    if (strategy_game_state == NULL) return 0;

    return strategy_game_state->national_event.is_active &&
           strcmp(strategy_game_state->national_event.event_name,
                  "Economic Recession") == 0;
}


// STRATEGY DISPATCHER

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

int should_perform_maintenance(Player* player, int property_index) {
    if (player == NULL || player->is_bankrupt) return 0;
    if (property_index < 0 || property_index >= MAX_PROPERTIES) return 0;

    switch (player->strategy) {
        case STRATEGY_AGGRESSIVE:
            return aggressive_should_maintain(player, property_index);
        case STRATEGY_CONSERVATIVE:
            return conservative_should_maintain(player, property_index);
        case STRATEGY_RISK_TAKER:
            return risk_taker_should_maintain(player, property_index);
        case STRATEGY_OPPORTUNISTIC:
            return opportunistic_should_maintain(player, property_index);
        default:
            return 0;
    }
}

int aggressive_should_maintain(Player* player, int property_index) {
    if (player == NULL || property_index < 0 ||
        property_index >= MAX_PROPERTIES) return 0;

    Property* prop = &property_array[property_index];
    if (prop->owner_id != player->player_id || prop->building_count == 0)
        return 0;

    int cost = get_maintenance_cost(prop);
    return prop->condition_percentage < 75 && cost <= player->cash;
}

int conservative_should_maintain(Player* player, int property_index) {
    if (player == NULL || property_index < 0 ||
        property_index >= MAX_PROPERTIES) return 0;

    Property* prop = &property_array[property_index];
    if (prop->owner_id != player->player_id || prop->building_count == 0)
        return 0;

    int cost = get_maintenance_cost(prop);
    int cash_after = player->cash - cost;
    return prop->condition_percentage < 90 && cash_after >= 5000;
}

int risk_taker_should_maintain(Player* player, int property_index) {
    if (player == NULL || property_index < 0 ||
        property_index >= MAX_PROPERTIES) return 0;

    Property* prop = &property_array[property_index];
    if (prop->owner_id != player->player_id || prop->building_count == 0)
        return 0;

    int cost = get_maintenance_cost(prop);
    return prop->condition_percentage < 25 && cost <= player->cash;
}

int opportunistic_should_maintain(Player* player, int property_index) {
    if (player == NULL || property_index < 0 ||
        property_index >= MAX_PROPERTIES) return 0;

    Property* prop = &property_array[property_index];
    if (prop->owner_id != player->player_id || prop->building_count == 0)
        return 0;
    if (prop->condition_percentage == 100) return 0;

    int cost = get_maintenance_cost(prop);
    if (cost > player->cash) return 0;

    int current_rent = calculate_rent_with_buildings(prop);
    int restored_rent =
        prop->base_rent * get_building_multiplier(prop->building_count);

    if (strategy_game_state != NULL) {
        current_rent = apply_event_rent_modifiers(
            prop, strategy_game_state, player->player_id, current_rent);
        restored_rent = apply_event_rent_modifiers(
            prop, strategy_game_state, player->player_id, restored_rent);
    }

    int projected_extra_rent = (restored_rent - current_rent) * 10;
    return projected_extra_rent > cost;
}

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
    if (player == NULL || prop == NULL) return 0;
    if (player->is_bankrupt) return 0;
    
    if (prop->owner_id != -1) return 0;

    int purchase_price = get_adjusted_purchase_price(player, prop);

    if(player->cash < purchase_price){
        return 0;
    }

    if (prop->color_group == GROUP_DARK_BLUE ||
        would_complete_monopoly(player, prop->color_group)) {
        return 1;
    }

    // A purchase is allowed only when one estimated future rent remains.
    if (player->cash < purchase_price + average_rent) {
        return 0;
    }



    return 1;
}

int aggressive_auction_bid(Player* player, Property* prop, int current_bid) {
    if (player == NULL || prop == NULL) return -1;
    if (player->is_bankrupt) return -1;
    
    if (current_bid >= player->cash) return -1;
    
    int estimated_value = get_adjusted_market_value(player, prop);
    int max_bid = (estimated_value * 120) / 100;
    
    int next_bid = current_bid + 250;

    if (next_bid <= max_bid && next_bid <= player->cash) {
        return next_bid;  
    }
    
    return -1;
}

int aggressive_should_loan(Player* player) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    
    if (player->player_loan.is_active) return 0;
    
    int max_loan = get_max_loan_amount(player);
    if (max_loan == 0) return 0;
    
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        Property* prop = &property_array[i];
        if (prop->owner_id == -1) {
            if (would_complete_monopoly(player, prop->color_group)) {
                int purchase_price =
                    get_adjusted_purchase_price(player, prop);
                int required_cash = purchase_price + average_rent;
                if (player->cash < required_cash &&
                    player->cash + max_loan >= required_cash) {
                    return 1;
                }
            }
        }
    }
    
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
            int purchase_price = get_adjusted_purchase_price(player, prop);
            int shortfall = purchase_price + average_rent - player->cash;

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
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    
    if (!player->player_loan.is_active) return 0;
    
    int reserve = 5000;
    int excess_cash = player->cash - reserve;
    
    if (excess_cash > player->player_loan.current_amount * 2) {
        return 1;  // Repay the loan
    }
    
    if (player->player_loan.rounds_remaining <= 1) {
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
    if (player == NULL) return -1;
    if (player->is_bankrupt) return -1;
    
    PropertyGroup groups[] = {
        GROUP_BROWN, GROUP_LIGHT_BLUE, GROUP_PINK, GROUP_ORANGE,
        GROUP_RED, GROUP_YELLOW, GROUP_GREEN, GROUP_DARK_BLUE
    };
    
    for (int g = 0; g < 8; g++) {
        if (has_monopoly(player, groups[g])) {
            
            int min_buildings = 999;
            int chosen_property = -1;
            
            for (int i = 0; i < MAX_PROPERTIES; i++) {
                if (property_array[i].color_group == groups[g] &&
                    property_array[i].owner_id == player->player_id) {
                    
                    if (property_array[i].building_count >= 4) continue;

                    if (player->cash < property_array[i].house_construction_cost) continue;
                    
                    if (property_array[i].building_count < min_buildings) {
                        min_buildings = property_array[i].building_count;
                        chosen_property = i;
                    }
                }
            }
            
            if (chosen_property != -1) {
                return chosen_property;
            }
        }
    }
    
    return -1;  // No buildable property found
}

int aggressive_should_hotel(Player* player) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        Property* prop = &property_array[i];
        
        if (prop->owner_id == player->player_id && prop->building_count == 4) {
            
            if (player->cash >= prop->hotel_construction_cost) {
                return 1;  // Build hotel!
            }
        }
    }
    
    return 0;  // No property ready for hotel upgrade
}

int aggressive_choose_hotel(Player* player) {
    if (player == NULL) return -1;
    if (player->is_bankrupt) return -1;
    
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        Property* prop = &property_array[i];
        
        if (prop->owner_id == player->player_id &&
            prop->building_count == 4 &&
            player->cash >= prop->hotel_construction_cost) {
            return i;  // Return first property with 4 houses
        }
    }
    
    return -1;  // No property ready for hotel upgrade
}

int aggressive_insurance_type(Player* player, int property_index) {
    if (player == NULL) return INSURANCE_NONE;
    if (player->is_bankrupt) return INSURANCE_NONE;
    if (property_index < 0 || property_index >= MAX_PROPERTIES) return INSURANCE_NONE;
    
    Property* prop = &property_array[property_index];
    
    if (prop->owner_id != player->player_id) return INSURANCE_NONE;
    
    if (prop->building_count == 0) return INSURANCE_NONE;
    
    if (prop->insurance_policy != INSURANCE_NONE) return INSURANCE_NONE;
    
    if (prop->building_count == 5) { // means has a hotel
        return INSURANCE_COMPREHENSIVE;
    } else {
        return INSURANCE_BASIC;
    }
}

int aggressive_should_renovate(Player* player, int property_index) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    if (property_index < 0 || property_index >= MAX_PROPERTIES) return 0;
    
    Property* prop = &property_array[property_index];
    
    if (prop->owner_id != player->player_id) return 0;
    
    if (prop->building_count == 0) return 0;
    
    int depreciation = calculate_depreciation(prop);
    
    // Aggressive renovates when depreciation exceeds 10%
    if (depreciation <= 10) {
        return 0;  // Not depreciated enough
    }
    
    int property_value = get_property_value(prop);
    int renovation_cost = (property_value * 10) / 100;
    
    if (renovation_cost > player->cash) {
        return 0;  // Can't afford
    }
    
    return 1;  // Renovate!
}

// CONSERVATIVE BANKER SPECIFIC FUNCTIONS

int conservative_should_buy(Player* player, Property* prop) {
    if (player == NULL || prop == NULL) return 0;
    if (player->is_bankrupt) return 0;
    
    if (prop->owner_id != -1) return 0;
    
    if (prop->owner_id == player->player_id) return 0;

    // Conservative bankers avoid new investments during a recession.
    if (is_economic_recession_active()) return 0;

    int purchase_price = get_adjusted_purchase_price(player, prop);
    
    // Every purchase must leave at least 50% of current cash. To express the
    // railway/utility preference when only one landed property is available,
    // ordinary properties use a larger 60% reserve.
    int cash_before = player->cash;
    int cash_after = cash_before - purchase_price;
    int minimum_reserve_percentage = 60;

    if (prop->color_group == GROUP_RAILWAY ||
        prop->color_group == GROUP_UTILITY) {
        minimum_reserve_percentage = 50;
    }
    
    int minimum_reserve =
        (cash_before * minimum_reserve_percentage) / 100;

    if (cash_after < minimum_reserve) {
        return 0;
    }

    return 1;
}

int conservative_auction_bid(Player* player, Property* prop, int current_bid) {
    if (player == NULL || prop == NULL) return -1;
    if (player->is_bankrupt) return -1;

    // Auctions are investments too, so do not bid during a recession.
    if (is_economic_recession_active()) return -1;
    
    if (current_bid >= player->cash) return -1;
    
    int max_bid = get_adjusted_market_value(player, prop);
    
    int cash_after_bid = player->cash - current_bid;
    if (cash_after_bid < player->cash / 2) {
        return -1;  // Would leave less than 50% cash
    }
    
    // Rule-LK 20: increase the current bid by the minimum LKR 250.
    int next_bid = current_bid + 250;
    
    if (next_bid <= max_bid && next_bid <= player->cash) {
        return next_bid;
    }
    
    return -1;  // Withdraw - cannot bid at or above market value
}

int conservative_should_loan(Player* player) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    
    if (player->player_loan.is_active) return 0;
    
    int max_loan = get_max_loan_amount(player);
    if (max_loan == 0) return 0;
    
    
    if (player->cash < 2000) { // average rent
        return 1;  // Imminent bankruptcy!
    }

    return 0;
}

int conservative_loan_amount(Player* player) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    
    int max_loan = get_max_loan_amount(player);
    if (max_loan == 0) return 0;
    
    int target_cash = 5000;
    int needed = target_cash - player->cash;
    
    if (needed <= 0) return 0;
    
    int loan_amount = ((needed + 999) / 1000) * 1000;
    
    if (loan_amount > max_loan) {
        loan_amount = max_loan;
    }
    
    if (loan_amount < 1000 && loan_amount > 0) {
        loan_amount = 1000;
    }
    
    return loan_amount;
}

int conservative_should_repay(Player* player) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    
    if (!player->player_loan.is_active) return 0;
    
    
    int loan_amount = player->player_loan.current_amount;

    if (player->cash >= loan_amount) {
        return 1;  // Repay the full loan
    }
    
    
    return 0;  // Can't afford to repay
}

int conservative_should_build(Player* player) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;

    // Do not make new development investments during a recession.
    if (is_economic_recession_active()) return 0;
    
    
    int reserve = 5000;
    
    PropertyGroup groups[] = {
        GROUP_BROWN, GROUP_LIGHT_BLUE, GROUP_PINK, GROUP_ORANGE,
        GROUP_RED, GROUP_YELLOW, GROUP_GREEN, GROUP_DARK_BLUE
    };
    
    for (int g = 0; g < 8; g++) {
        if (has_monopoly(player, groups[g])) {
            
            int can_build = 0;
            int total_cost = 0;
            
            for (int i = 0; i < MAX_PROPERTIES; i++) {
                if (property_array[i].color_group == groups[g] &&
                    property_array[i].owner_id == player->player_id) {
                    
                    if (property_array[i].building_count < 4) {
                        can_build = 1;
                        total_cost += property_array[i].house_construction_cost;
                    }
                }
            }
            
            if (can_build && player->cash >= total_cost + reserve) {
                return 1;  // Build houses
            }
        }
    }
    
    return 0;  // No building
}

int conservative_choose_build(Player* player) {
    if (player == NULL) return -1;
    if (player->is_bankrupt) return -1;
    
    PropertyGroup groups[] = {
        GROUP_BROWN, GROUP_LIGHT_BLUE, GROUP_PINK, GROUP_ORANGE,
        GROUP_RED, GROUP_YELLOW, GROUP_GREEN, GROUP_DARK_BLUE
    };
    
    for (int g = 0; g < 8; g++) {
        if (has_monopoly(player, groups[g])) {
            
            int min_buildings = 999;
            int chosen_property = -1;
            
            for (int i = 0; i < MAX_PROPERTIES; i++) {
                if (property_array[i].color_group == groups[g] &&
                    property_array[i].owner_id == player->player_id) {
                    
                    if (property_array[i].building_count >= 4) continue;
                    
                    if (property_array[i].building_count < min_buildings) {
                        min_buildings = property_array[i].building_count;
                        chosen_property = i;
                    }
                }
            }
            
            if (chosen_property != -1) {
                return chosen_property;
            }
        }
    }
    
    return -1;  // No buildable property found
}

int conservative_should_hotel(Player* player) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;

    // Do not make new development investments during a recession.
    if (is_economic_recession_active()) return 0;
    
    if (player->player_loan.is_active) {
        return 0;  // Must settle loans first
    }
    
    int reserve = 5000;
    
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        Property* prop = &property_array[i];
        
        if (prop->owner_id == player->player_id && prop->building_count == 4) {
            
            int hotel_cost = prop->hotel_construction_cost;
            
            if (player->cash >= hotel_cost + reserve) {
                return 1;  // Build hotel!
            }
        }
    }
    
    return 0;  // No property ready for hotel upgrade
}

int conservative_choose_hotel(Player* player) {
    if (player == NULL) return -1;
    if (player->is_bankrupt) return -1;
    
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        Property* prop = &property_array[i];
        
        if (prop->owner_id == player->player_id && prop->building_count == 4) {
            return i;  // Return first property with 4 houses
        }
    }
    
    return -1;  // No property ready for hotel upgrade
}

int conservative_should_insure(Player* player, int property_index) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    if (property_index < 0 || property_index >= MAX_PROPERTIES) return 0;
    
    Property* prop = &property_array[property_index];
    
    if (prop->owner_id != player->player_id) return 0;
    
    if (prop->insurance_policy != INSURANCE_NONE) return 0;
    
    if (prop->building_count == 0) return 0;  // No buildings = no insurance
    
    int premium = calculate_insurance_premium(prop, INSURANCE_COMPREHENSIVE);
    if (premium > player->cash) return 0;
    
    return INSURANCE_COMPREHENSIVE;  // Buy Comprehensive Insurance
}

int conservative_should_renovate(Player* player, int property_index) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    if (property_index < 0 || property_index >= MAX_PROPERTIES) return 0;
    
    Property* prop = &property_array[property_index];
    
    if (prop->owner_id != player->player_id) return 0;
    
    if (prop->building_count == 0) return 0;
    
    int depreciation = calculate_depreciation(prop);
    
    if (depreciation <= 10) {
        return 0;  // Not depreciated enough
    }
    
    int property_value = get_property_value(prop);
    int renovation_cost = (property_value * 10) / 100;
    
    int reserve = 5000;
    int cash_after = player->cash - renovation_cost;
    
    if (cash_after < reserve) {
        return 0;  // Would drop below reserve
    }
    
    return 1;  // Renovate
}

// RISK TAKER SPECIFIC FUNCTIONS

int risk_taker_should_buy(Player* player, Property* prop) {
    if (player == NULL || prop == NULL) return 0;
    if (player->is_bankrupt) return 0;
    
    if (prop->owner_id != -1) return 0;
    
    if (prop->owner_id == player->player_id) return 0;
    
    
    int purchase_price = get_adjusted_purchase_price(player, prop);
    if (player->cash >= purchase_price) {
        return 1;  // Buy it!
    }
    
    return 0;
}

int risk_taker_auction_bid(Player* player, Property* prop, int current_bid) {
    if (player == NULL || prop == NULL) return -1;
    if (player->is_bankrupt) return -1;
    
    if (current_bid >= player->cash) return -1;
    
    // Rule-LK 20: increase the current bid by the minimum LKR 250.
    int next_bid = current_bid + 250;
    
    if (next_bid <= player->cash) {
        return next_bid;
    }
    
    return -1;  // Withdraw - can't afford next bid
}

int risk_taker_should_loan(Player* player) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    
    if (player->player_loan.is_active) return 0;
    
    int max_loan = get_max_loan_amount(player);
    if (max_loan == 0) return 0;
    
    return 1;
}

int risk_taker_loan_amount(Player* player) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    
    int max_loan = get_max_loan_amount(player);
    if (max_loan == 0) return 0;
    
    return max_loan;
}

int risk_taker_should_repay(Player* player) {
    (void)player;
    return 0;
}

int risk_taker_should_build(Player* player) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    
    PropertyGroup groups[] = {
        GROUP_BROWN, GROUP_LIGHT_BLUE, GROUP_PINK, GROUP_ORANGE,
        GROUP_RED, GROUP_YELLOW, GROUP_GREEN, GROUP_DARK_BLUE
    };
    
    for (int g = 0; g < 8; g++) {
        if (has_monopoly(player, groups[g])) {
            for (int i = 0; i < MAX_PROPERTIES; i++) {
                if (property_array[i].color_group == groups[g] &&
                    property_array[i].owner_id == player->player_id) {
                    
                    if (property_array[i].building_count < 4) {
                        Property* prop = &property_array[i];
                        int cost = prop->house_construction_cost;
                        if (strategy_game_state != NULL) {
                            cost = apply_event_construction_modifiers(
                                prop, cost, strategy_game_state,
                                player->player_id);
                        }
                        int projected_return = get_development_return(
                            player, prop, prop->building_count + 1);

                        if (player->cash >= cost && projected_return > cost) {
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
    
    PropertyGroup groups[] = {
        GROUP_DARK_BLUE, GROUP_GREEN, GROUP_YELLOW, GROUP_RED,
        GROUP_ORANGE, GROUP_PINK, GROUP_LIGHT_BLUE, GROUP_BROWN
    };
    
    for (int g = 0; g < 8; g++) {
        if (has_monopoly(player, groups[g])) {
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
    
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        Property* prop = &property_array[i];
        
        if (prop->owner_id == player->player_id && prop->building_count == 4) {
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
    
    if (prop->owner_id != player->player_id) return 0;
    
    if (prop->insurance_policy != INSURANCE_NONE) return 0;
    
    if (prop->building_count == 0) return 0;
    
    // Purchase insurance only after an out-of-pocket disaster loss.
    return player->has_experienced_financial_loss;
}

int risk_taker_should_renovate(Player* player, int property_index) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    if (property_index < 0 || property_index >= MAX_PROPERTIES) return 0;
    
    Property* prop = &property_array[property_index];
    
    if (prop->owner_id != player->player_id) return 0;
    
    if (prop->building_count == 0) return 0;
    
    // Only renovate when depreciation exceeds 20%
    int depreciation = calculate_depreciation(prop);
    if (depreciation <= 20) {
        return 0;
    }
    
    int property_value = get_property_value(prop);
    int renovation_cost = (property_value * 10) / 100;
    
    if (renovation_cost > player->cash) {
        return 0;
    }
    
    return 1;
}

// OPPORTUNISTIC TRADER SPECIFIC FUNCTIONS

static int get_projected_rent(Player* player, Property* prop) {
    if (player == NULL || prop == NULL) return 0;

    int rent = 0;

    if (prop->color_group == GROUP_RAILWAY) {
        int railway_count = 1;
        for (int i = 0; i < MAX_PROPERTIES; i++) {
            if (&property_array[i] != prop &&
                property_array[i].owner_id == player->player_id &&
                property_array[i].color_group == GROUP_RAILWAY) {
                railway_count++;
            }
        }

        if (railway_count == 1) rent = 250;
        else if (railway_count == 2) rent = 500;
        else if (railway_count == 3) rent = 1000;
        else rent = 2000;
    } else if (prop->color_group == GROUP_UTILITY) {
        int utility_count = 1;
        for (int i = 0; i < MAX_PROPERTIES; i++) {
            if (&property_array[i] != prop &&
                property_array[i].owner_id == player->player_id &&
                property_array[i].color_group == GROUP_UTILITY) {
                utility_count++;
            }
        }

        // Seven is the average result of two six-sided dice.
        rent = (utility_count >= 2) ? 10 * 7 : 4 * 7;
    } else {
        int multiplier = get_building_multiplier(prop->building_count);
        rent = prop->base_rent * multiplier;

        if (prop->owner_id == -1 &&
            would_complete_monopoly(player, prop->color_group)) {
            rent *= 2;
        }
    }

    if (strategy_game_state != NULL) {
        rent = apply_event_rent_modifiers(
            prop, strategy_game_state, player->player_id, rent);
    }

    return rent;
}

static int get_projected_appreciation(Player* player, Property* prop) {
    if (player == NULL || prop == NULL) return 0;

    int projected_value = get_adjusted_market_value(player, prop);
    int investment_value = get_adjusted_purchase_price(player, prop);
    return projected_value - investment_value;
}

static int get_projected_property_return(Player* player, Property* prop,
                                         int investment_cost) {
    if (player == NULL || prop == NULL || investment_cost <= 0) return 0;

    int appreciation = get_projected_appreciation(player, prop);
    int rental_return = get_projected_rent(player, prop) * 10;
    return appreciation + rental_return;
}

static int is_balanced_portfolio_purchase(Player* player, Property* prop) {
    if (player == NULL || prop == NULL) return 0;

    int residential_count = 0;
    int railway_count = 0;
    int utility_count = 0;

    for (int i = 0; i < MAX_PROPERTIES; i++) {
        if (property_array[i].owner_id != player->player_id) continue;

        if (property_array[i].color_group == GROUP_RAILWAY) railway_count++;
        else if (property_array[i].color_group == GROUP_UTILITY) utility_count++;
        else residential_count++;
    }

    int smallest_count = residential_count;
    if (railway_count < smallest_count) smallest_count = railway_count;
    if (utility_count < smallest_count) smallest_count = utility_count;

    if (prop->color_group == GROUP_RAILWAY) {
        return railway_count == smallest_count;
    }
    if (prop->color_group == GROUP_UTILITY) {
        return utility_count == smallest_count;
    }
    return residential_count == smallest_count;
}

static int get_development_return(Player* player, Property* prop,
                                  int next_building_count) {
    if (player == NULL || prop == NULL) return 0;
    if (next_building_count < 1 || next_building_count > 5) return 0;

    int current_multiplier = get_building_multiplier(prop->building_count);
    int next_multiplier = get_building_multiplier(next_building_count);
    int current_rent = prop->base_rent * current_multiplier;
    int future_rent = prop->base_rent * next_multiplier;

    if (strategy_game_state != NULL) {
        current_rent = apply_event_rent_modifiers(
            prop, strategy_game_state, player->player_id, current_rent);
        future_rent = apply_event_rent_modifiers(
            prop, strategy_game_state, player->player_id, future_rent);
    }

    return (future_rent - current_rent) * 10;
}

// Return on the acquisition, including ten projected rent collections and
// appreciation indicated by the currently active events and market state.
static int calculate_roi(Player* player, Property* prop) {
    if (player == NULL || prop == NULL) return 0;
    if (prop->owner_id != -1) return 0;

    int cost = get_adjusted_purchase_price(player, prop);
    if (cost <= 0) return 0;

    int projected_return =
        get_projected_property_return(player, prop, cost);
    return (projected_return * 100) / cost;
}

int opportunistic_should_buy(Player* player, Property* prop) {
    if (player == NULL || prop == NULL) return 0;
    if (player->is_bankrupt) return 0;
    if (prop->owner_id != -1) return 0;
    if (prop->owner_id == player->player_id) return 0;
    
    int purchase_price = get_adjusted_purchase_price(player, prop);
    if (player->cash < purchase_price) return 0;

    if (!is_balanced_portfolio_purchase(player, prop)) return 0;

    int construction_cost = prop->house_construction_cost;
    if (strategy_game_state != NULL && construction_cost > 0) {
        construction_cost = apply_event_construction_modifiers(
            prop, construction_cost, strategy_game_state, player->player_id);
    }

    int appreciation = get_projected_appreciation(player, prop);
    int projected_return =
        get_projected_property_return(player, prop, purchase_price);

    return appreciation > construction_cost && projected_return > 0;
}

int opportunistic_auction_bid(Player* player, Property* prop, int current_bid) {
    if (player == NULL || prop == NULL) return -1;
    if (player->is_bankrupt) return -1;
    if (!is_balanced_portfolio_purchase(player, prop)) return -1;
    
    if (current_bid >= player->cash) return -1;
    
    int roi = calculate_roi(player, prop);
    
    int max_bid = 0;
    
    if (roi > 20) {
        int estimated_value = get_adjusted_market_value(player, prop);
        max_bid = (estimated_value * 110) / 100;
    } else if (roi > 15) {
        int estimated_value = get_adjusted_market_value(player, prop);
        max_bid = (estimated_value * 105) / 100;
    } else if (roi > 10) {
        max_bid = get_adjusted_market_value(player, prop);
    } else {
        return -1;  // ROI too low, withdraw
    }

    // Prefer a clearly discounted auction purchase. Even a high-return
    // opportunity cannot be bid beyond 90% of its adjusted market value.
    int discounted_limit =
        (get_adjusted_market_value(player, prop) * 90) / 100;
    if (max_bid > discounted_limit) max_bid = discounted_limit;
    
    int cash_after = player->cash - current_bid;
    if (cash_after < player->cash / 2) {
        int safe_bid = player->cash / 2;
        if (safe_bid < max_bid) {
            max_bid = safe_bid;
        }
    }
    
    // Rule-LK 20: increase the current bid by the minimum LKR 250.
    int next_bid = current_bid + 250;
    int projected_return =
        get_projected_property_return(player, prop, next_bid);
    
    if (next_bid <= max_bid && next_bid <= player->cash &&
        projected_return > next_bid) {
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
    
    int loan_amount = (max_loan * 70) / 100;
    int interest_rate = 8;
    if (strategy_game_state != NULL) {
        interest_rate = apply_event_interest_modifiers(
            strategy_game_state->current_interest_rate,
            strategy_game_state, player->player_id);
    }
    int borrowing_cost = (loan_amount * interest_rate) / 100;

    for (int i = 0; i < MAX_PROPERTIES; i++) {
        Property* prop = &property_array[i];
        if (prop->owner_id != -1) continue;
        if (!is_balanced_portfolio_purchase(player, prop)) continue;

        int purchase_price = get_adjusted_purchase_price(player, prop);
        int projected_return =
            get_projected_property_return(player, prop, purchase_price);

        if (player->cash + loan_amount >= purchase_price &&
            projected_return > borrowing_cost) return 1;
    }
    
    return 0;
}

int opportunistic_loan_amount(Player* player) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    
    int max_loan = get_max_loan_amount(player);
    if (max_loan == 0) return 0;
    
    int loan_amount = (max_loan * 70) / 100;
    
    loan_amount = (loan_amount / 1000) * 1000;
    
    if (loan_amount < 1000 && max_loan >= 1000) {
        loan_amount = 1000;
    }
    
    return loan_amount;
}

int opportunistic_should_repay(Player* player) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    if (!player->player_loan.is_active) return 0;
    
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
    
    return 1;
}

int opportunistic_should_build(Player* player) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    
    PropertyGroup groups[] = {
        GROUP_BROWN, GROUP_LIGHT_BLUE, GROUP_PINK, GROUP_ORANGE,
        GROUP_RED, GROUP_YELLOW, GROUP_GREEN, GROUP_DARK_BLUE
    };
    
    for (int g = 0; g < 8; g++) {
        if (has_monopoly(player, groups[g])) {
            for (int i = 0; i < MAX_PROPERTIES; i++) {
                if (property_array[i].color_group == groups[g] &&
                    property_array[i].owner_id == player->player_id) {
                    
                    if (property_array[i].building_count < 4) {
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
    
    int best_property = -1;
    int best_surplus = 0;

    PropertyGroup groups[] = {
        GROUP_DARK_BLUE, GROUP_GREEN, GROUP_YELLOW, GROUP_RED,
        GROUP_ORANGE, GROUP_PINK, GROUP_LIGHT_BLUE, GROUP_BROWN
    };
    
    for (int g = 0; g < 8; g++) {
        if (has_monopoly(player, groups[g])) {
            int min_buildings = 999;

            for (int i = 0; i < MAX_PROPERTIES; i++) {
                if (property_array[i].color_group == groups[g] &&
                    property_array[i].owner_id == player->player_id) {
                    if (property_array[i].building_count >= 4) continue;
                    if (property_array[i].building_count < min_buildings)
                        min_buildings = property_array[i].building_count;
                }
            }

            for (int i = 0; i < MAX_PROPERTIES; i++) {
                Property* prop = &property_array[i];
                if (prop->color_group != groups[g] ||
                    prop->owner_id != player->player_id ||
                    prop->building_count != min_buildings) continue;

                int cost = prop->house_construction_cost;
                if (strategy_game_state != NULL) {
                    cost = apply_event_construction_modifiers(
                        prop, cost, strategy_game_state, player->player_id);
                }
                int surplus = get_development_return(
                    player, prop, prop->building_count + 1) - cost;
                if (cost <= player->cash && surplus > best_surplus) {
                    best_surplus = surplus;
                    best_property = i;
                }
            }
        }
    }

    return best_property;
}

int opportunistic_should_hotel(Player* player) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        Property* prop = &property_array[i];
        
        if (prop->owner_id == player->player_id && prop->building_count == 4) {
            int cost = prop->hotel_construction_cost;
            if (strategy_game_state != NULL) {
                cost = apply_event_construction_modifiers(
                    prop, cost, strategy_game_state, player->player_id);
            }
            int projected_return = get_development_return(player, prop, 5);
            if (player->cash >= cost && projected_return > cost) return 1;
        }
    }
    
    return 0;
}

int opportunistic_choose_hotel(Player* player) {
    if (player == NULL) return -1;
    if (player->is_bankrupt) return -1;
    
    int best_surplus = 0;
    int chosen = -1;
    
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        Property* prop = &property_array[i];
        
        if (prop->owner_id == player->player_id && prop->building_count == 4) {
            int cost = prop->hotel_construction_cost;
            if (strategy_game_state != NULL) {
                cost = apply_event_construction_modifiers(
                    prop, cost, strategy_game_state, player->player_id);
            }
            int surplus = get_development_return(player, prop, 5) - cost;
            if (cost <= player->cash && surplus > best_surplus) {
                best_surplus = surplus;
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
    
    int property_value = get_property_value(prop);
    
    if (property_value < 20000) {
        return 0;
    }
    
    int premium = calculate_insurance_premium(prop, INSURANCE_COMPREHENSIVE);
    if (strategy_game_state != NULL) {
        premium = apply_event_insurance_modifiers(
            premium, strategy_game_state, player->player_id);
    }
    if (premium > player->cash) {
        return 0;
    }

    // Estimate one significant loss as 20% of the development's value.
    int expected_avoided_loss = property_value / 5;
    if (expected_avoided_loss <= premium) return 0;
    
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
    
    // Compare the value restored by renovation with its cost.
    int property_value = get_property_value(prop);
    int renovation_cost = (property_value * 10) / 100;
    
    if (renovation_cost > player->cash) {
        return 0;
    }
    
    int restored_value = (prop->purchase_price * depreciation) / 100;
    return restored_value > renovation_cost;
}


