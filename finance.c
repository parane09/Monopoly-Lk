#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================
// HELPER FUNCTIONS
// ============================================

// Get current property value including depreciation
// This is a base value - will be expanded with market effects later
int get_property_value(Property* prop) {
    if (prop == NULL) return 0;
    
    int value = prop->purchase_price;
    
    // Apply depreciation reduction (if any)
    if (prop->value_reduction > 0) {
        value = (value * (100 - prop->value_reduction)) / 100;
    }
    
    return value;
}

// Calculate total mortgage value of all unmortgaged, unlocked properties
// Used for determining maximum loan amount
int get_total_mortgage_value(Player* player) {
    if (player == NULL) return 0;
    
    int total = 0;
    
    for (int i = 0; i < player->owned_property_count; i++) {
        int prop_idx = player->owned_property_indices[i];
        if (prop_idx < 0 || prop_idx >= MAX_PROPERTIES) continue;
        
        Property* prop = &property_array[prop_idx];
        
        // Only count properties that are:
        // 1. Not already mortgaged
        // 2. Not locked as collateral for an existing loan
        if (!prop->is_mortgaged && !prop->is_loan_locked) {
            total += prop->mortgage_value;
        }
    }
    
    return total;
}

// Check if a player has a monopoly on a color group
// This will be used for building decisions
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

// Find the property with the fewest buildings in a color group
// Used for even building rule
int get_min_buildings_in_group(Player* player, PropertyGroup group) {
    if (player == NULL) return -1;
    
    int min_buildings = 999;
    int selected_property = -1;
    
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        if (property_array[i].color_group == group && 
            property_array[i].owner_id == player->player_id) {
            
            int buildings = property_array[i].building_count;
            // Hotel counts as 5 (can't build more)
            if (buildings < min_buildings) {
                min_buildings = buildings;
                selected_property = i;
            }
        }
    }
    
    return selected_property;
}

// Get building cost based on property
int get_building_cost(Property* prop) {
    if (prop == NULL) return 0;
    
    // If has 4 houses, this is a hotel upgrade
    if (prop->building_count == 4) {
        return prop->hotel_construction_cost;
    }
    
    // Otherwise building a house
    return prop->house_construction_cost;
}

// Print function to display player's financial status
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