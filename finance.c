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

// ============================================
// LOAN SYSTEM
// ============================================

// Calculate maximum loan amount a player can get
// Maximum = 75% of total mortgage value of unmortgaged, unlocked properties
int get_max_loan_amount(Player* player) {
    if (player == NULL) return 0;
    
    int total_mortgage_value = get_total_mortgage_value(player);
    
    // 75% of total mortgage value
    int max_loan = (total_mortgage_value * 75) / 100;
    
    return max_loan;
}

// Take a loan - player borrows money with properties as collateral
// Returns: 1 = success, 0 = failure
int take_loan(Player* player, int amount) {
    if (player == NULL) return 0;
    
    // Check if player already has an active loan
    if (player->player_loan.is_active) {
        printf("  %s already has an active loan!\n", player->player_name);
        return 0;
    }
    
    // Calculate maximum loan amount
    int max_loan = get_max_loan_amount(player);
    if (max_loan == 0) {
        printf("  %s has no eligible collateral for a loan.\n", player->player_name);
        return 0;
    }
    
    // Check if requested amount is valid
    if (amount <= 0 || amount > max_loan) {
        printf("  Invalid loan amount. Maximum: LKR %d\n", max_loan);
        return 0;
    }
    
    // Check if player has enough cash? No - loan adds cash, doesn't require it
    
    // Lock collateral properties
    int locked_count = 0;
    int total_needed = 0;
    
    // We need to lock properties worth at least the loan amount
    // Lock properties in order (highest mortgage value first)
    // Simple approach: lock all unmortgaged, unlocked properties
    

    lock_collateral(player);
    printf("  Collateral: %d properties locked.\n", player->player_loan.collateral_count);
    
    if (locked_count == 0) {
        printf("  No properties available to lock as collateral.\n");
        return 0;
    }
    
    // Create the loan
    player->player_loan.is_active = 1;
    player->player_loan.current_amount = amount;
    player->player_loan.original_amount = amount;
    player->player_loan.interest_rate = 8;  // Base interest rate (will be updated with inflation)
    player->player_loan.rounds_remaining = LOAN_DURATION;
    player->player_loan.initial_duration = LOAN_DURATION;
    player->player_loan.collateral_count = locked_count;
    
    // Add cash to player
    player->cash += amount;
    
    printf("  %s obtained a secured loan of LKR %d.\n", 
           player->player_name, amount);
    printf("  Interest Rate: %d%%, Duration: %d Rounds\n", 
           player->player_loan.interest_rate, LOAN_DURATION);
    printf("  Collateral: %d properties locked.\n", locked_count);
    
    return 1;
}

// Repay part or all of a loan
// Returns: 1 = success, 0 = failure
int repay_loan(Player* player, int amount) {
    if (player == NULL) return 0;
    
    // Check if loan is active
    if (!player->player_loan.is_active) {
        printf("  %s has no active loan to repay.\n", player->player_name);
        return 0;
    }
    
    // Check amount
    if (amount <= 0) {
        printf("  Repayment amount must be positive.\n");
        return 0;
    }
    
    // Check if player has enough cash
    if (amount > player->cash) {
        printf("  Insufficient cash. Available: LKR %d\n", player->cash);
        return 0;
    }
    
    // Don't repay more than owed
    if (amount > player->player_loan.current_amount) {
        amount = player->player_loan.current_amount;
    }
    
    // Deduct cash
    player->cash -= amount;
    player->player_loan.current_amount -= amount;
    
    printf("  %s repaid LKR %d. Remaining loan: LKR %d\n", 
           player->player_name, amount, player->player_loan.current_amount);
    
    // Check if fully repaid
    if (player->player_loan.current_amount == 0) {
        // Unlock all collateral
        unlock_collateral(player);
        player->player_loan.is_active = 0;
        printf("  Loan fully repaid! Collateral unlocked.\n");
    }
    
    return 1;
}

// Apply compound interest to a player's loan
// Called at the end of each round
void apply_loan_interest(Player* player) {
    if (player == NULL) return;
    if (!player->player_loan.is_active) return;
    
    // Compound interest: amount = amount * (100 + rate) / 100
    int interest = (player->player_loan.current_amount * player->player_loan.interest_rate) / 100;
    player->player_loan.current_amount += interest;
    
    // Decrease duration
    player->player_loan.rounds_remaining--;
    
    printf("  %s loan interest: LKR %d added. New balance: LKR %d (Rounds left: %d)\n",
           player->player_name, interest, 
           player->player_loan.current_amount,
           player->player_loan.rounds_remaining);
    
    // Check for default
    if (player->player_loan.rounds_remaining <= 0) {
        process_loan_default(player);
    }
}

// Process loan default - foreclosure
void process_loan_default(Player* player) {
    if (player == NULL) return;
    if (!player->player_loan.is_active) return;
    
    printf("\n LOAN DEFAULT: %s has failed to repay the loan! \n", 
           player->player_name);
    printf("  Foreclosure initiated...\n");
    
    // Transfer all collateral properties to the bank
    for (int i = 0; i < player->player_loan.collateral_count; i++) {
        int prop_idx = player->player_loan.collateral_properties[i];
        if (prop_idx < 0 || prop_idx >= MAX_PROPERTIES) continue;
        
        Property* prop = &property_array[prop_idx];
        
        // Demolish buildings
        if (prop->building_count > 0) {
            printf("    Demolishing buildings on %s\n", prop->property_name);
            prop->building_count = 0;
            prop->condition_percentage = 100;
        }
        
        // Cancel insurance
        if (prop->insurance_policy != INSURANCE_NONE) {
            printf("    Cancelling insurance on %s\n", prop->property_name);
            prop->insurance_policy = INSURANCE_NONE;
            prop->insurance_rounds_remaining = 0;
        }
        
        // Remove from player's owned properties
        for (int j = 0; j < player->owned_property_count; j++) {
            if (player->owned_property_indices[j] == prop_idx) {
                // Shift remaining properties down
                for (int k = j; k < player->owned_property_count - 1; k++) {
                    player->owned_property_indices[k] = player->owned_property_indices[k + 1];
                }
                player->owned_property_count--;
                break;
            }
        }
        
        // Transfer to bank (owner_id = -1)
        prop->owner_id = -1;
        prop->is_loan_locked = 0;
        
        printf("    Property %s transferred to Bank.\n", prop->property_name);
    }
    
    // Clear the loan
    player->player_loan.is_active = 0;
    player->player_loan.current_amount = 0;
    player->player_loan.collateral_count = 0;
    
    printf("  Outstanding debt cleared.\n");
    
    // Check if player has any remaining assets
    if (player->owned_property_count == 0 && player->cash <= 0) {
        player->is_bankrupt = 1;
        printf("  💀 %s is now BANKRUPT! 💀\n", player->player_name);
    } else {
        printf("  %s continues with remaining assets.\n", player->player_name);
        print_player_finance(player);
    }
}

// Lock properties as collateral for a loan
void lock_collateral(Player* player) {
    if (player == NULL) return;
    
    int locked = 0;
    
    for (int i = 0; i < MAX_PROPERTIES && locked < MAX_COLLATERAL; i++) {
        if (property_array[i].owner_id == player->player_id &&
            !property_array[i].is_mortgaged &&
            !property_array[i].is_loan_locked) {
            
            property_array[i].is_loan_locked = 1;
            player->player_loan.collateral_properties[locked] = i;
            locked++;
        }
    }
    
    player->player_loan.collateral_count = locked;
}

// Unlock all collateral properties (loan fully repaid)
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

// Update loan interest rate based on inflation
void update_loan_interest_rate(Player* player, int inflation_rate) {
    if (player == NULL) return;
    if (!player->player_loan.is_active) return;
    
    // Base rate + inflation adjustment
    int new_rate = 8 + inflation_rate;
    if (new_rate < 0) new_rate = 0;
    if (new_rate > 25) new_rate = 25;  // Cap at 25%
    
    player->player_loan.interest_rate = new_rate;
}

// ============================================
// INSURANCE SYSTEM
// ============================================

// Calculate insurance premium based on policy type and property value
// Returns: Premium amount in LKR
int calculate_insurance_premium(Property* prop, int policy_type) {
    if (prop == NULL) return 0;
    
    int property_value = get_property_value(prop);
    int premium = 0;
    
    switch (policy_type) {
        case INSURANCE_BASIC:
            // 5% of property value
            premium = (property_value * 5) / 100;
            break;
            
        case INSURANCE_COMPREHENSIVE:
            // 10% of property value
            premium = (property_value * 10) / 100;
            break;
            
        case INSURANCE_BUSINESS:
            // 15% of property value (only for hotels)
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

// Buy insurance for a property
// Returns: 1 = success, 0 = failure
int buy_insurance(Player* player, int property_index, int policy_type) {
    if (player == NULL) return 0;
    if (property_index < 0 || property_index >= MAX_PROPERTIES) return 0;
    
    Property* prop = &property_array[property_index];
    
    // Check if property belongs to player
    if (prop->owner_id != player->player_id) {
        printf("  %s does not own %s!\n", player->player_name, prop->property_name);
        return 0;
    }
    
    // Check if already has insurance
    if (prop->insurance_policy != INSURANCE_NONE) {
        printf("  %s already has insurance on %s.\n", 
               player->player_name, prop->property_name);
        return 0;
    }
    
    // Calculate premium
    int premium = calculate_insurance_premium(prop, policy_type);
    if (premium == 0) {
        printf("  Invalid insurance policy or property not eligible.\n");
        return 0;
    }
    
    // Check if player can afford premium
    if (premium > player->cash) {
        printf("  Insufficient funds! Premium: LKR %d, Available: LKR %d\n", 
               premium, player->cash);
        return 0;
    }
    
    // Deduct premium
    player->cash -= premium;
    
    // Set insurance on property
    prop->insurance_policy = policy_type;
    prop->insurance_rounds_remaining = INSURANCE_DURATION;
    
    // Print policy name
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
void process_insurance_expiry(Property* prop) {
    if (prop == NULL) return;
    if (prop->insurance_policy == INSURANCE_NONE) return;
    
    // Decrement remaining rounds
    prop->insurance_rounds_remaining--;
    
    // Check for renewal reminder (3 rounds before expiry)
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
    
    // Check for expiry
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
    }
}

// Process a disaster claim - returns compensation amount
// Returns: Compensation in LKR, or 0 if no insurance
int process_disaster_claim(Property* prop, int damage_cost) {
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
            // 80% of repair cost
            compensation = (damage_cost * 80) / 100;
            printf("  Basic Insurance covers 80%% of LKR %d = LKR %d\n",
                   damage_cost, compensation);
            break;
            
        case INSURANCE_COMPREHENSIVE:
            // 100% of repair cost
            compensation = damage_cost;
            printf("  Comprehensive Insurance covers 100%% = LKR %d\n", compensation);
            break;
            
        case INSURANCE_BUSINESS:
            // Repair cost + 5 rounds lost hotel rental income
            if (prop->building_count == 5) {  // Has hotel
                int lost_rent = prop->base_rent * 5;
                // Apply rent multiplier for hotel (10x)
                lost_rent = lost_rent * 10;
                compensation = damage_cost + lost_rent;
                printf("  Business Interruption covers repair + 5 rounds hotel rent = LKR %d\n",
                       compensation);
            } else {
                // If no hotel, treat as Comprehensive
                compensation = damage_cost;
                printf("  No hotel on property. Treated as Comprehensive: LKR %d\n", compensation);
            }
            break;
            
        default:
            return 0;
    }
    /*
    // Reduce insurance duration (disaster consumes some coverage)
    // This is a common house rule - not in spec but makes sense
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

// Check if a property has active insurance
int has_active_insurance(Property* prop) {
    if (prop == NULL) return 0;
    return (prop->insurance_policy != INSURANCE_NONE);
}

// Get insurance renewal reminder string
// Returns: 1 if reminder should be shown, 0 otherwise
int get_insurance_reminder(Property* prop) {
    if (prop == NULL) return 0;
    if (prop->insurance_policy == INSURANCE_NONE) return 0;
    
    // Show reminder at 3 rounds remaining
    if (prop->insurance_rounds_remaining == 3) {
        return 1;
    }
    
    return 0;
}

// Get insurance policy name as string
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

// Print all insurance policies for a player
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

// Check if a player can build a house on a property
// Returns: 1 = can build, 0 = cannot build
int can_build_house(Player* player, int property_index) {
    if (player == NULL) return 0;
    if (property_index < 0 || property_index >= MAX_PROPERTIES) return 0;
    
    Property* prop = &property_array[property_index];
    
    // Check if player owns the property
    if (prop->owner_id != player->player_id) {
        printf("  %s does not own %s!\n", player->player_name, prop->property_name);
        return 0;
    }
    
    // Check if property already has a hotel (building_count == 5)
    if (prop->building_count == 5) {
        printf("  %s already has a hotel. Cannot build more.\n", prop->property_name);
        return 0;
    }
    
    // Check if property already has 4 houses
    if (prop->building_count == 4) {
        printf("  %s has 4 houses. Upgrade to hotel instead.\n", prop->property_name);
        return 0;
    }
    
    // Check if player has a monopoly on this property's color group
    if (!has_monopoly(player, prop->color_group)) {
        printf("  %s does not have a monopoly on this color group.\n", player->player_name);
        return 0;
    }
    
    // Check if player has enough cash
    int cost = prop->house_construction_cost;
    if (player->cash < cost) {
        printf("  Insufficient funds! House cost: LKR %d, Available: LKR %d\n", 
               cost, player->cash);
        return 0;
    }
    
    // Check even building rule: cannot exceed other properties in group by more than 1
    int min_buildings = get_min_buildings_in_group(player, prop->color_group);
    if (min_buildings == -1) {
        // No other properties in group? Shouldn't happen if has_monopoly is true
        return 1;  // Can build if it's the only property in group
    }
    
    // Get current building count of this property
    int current_buildings = prop->building_count;
    
    // Get the minimum building count in the group
    int min_count = 999;
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        if (property_array[i].color_group == prop->color_group && 
            property_array[i].owner_id == player->player_id) {
            if (property_array[i].building_count < min_count) {
                min_count = property_array[i].building_count;
            }
        }
    }
    
    // Cannot build if this property has more buildings than the minimum
    if (current_buildings > min_count) {
        printf("  Even building rule: %s has %d houses, but another property has only %d.\n",
               prop->property_name, current_buildings, min_count);
        return 0;
    }
    
    return 1;
}

// Build a house on a property
// Returns: 1 = success, 0 = failure
int build_house(Player* player, int property_index) {
    if (player == NULL) return 0;
    if (property_index < 0 || property_index >= MAX_PROPERTIES) return 0;
    
    // Check if can build
    if (!can_build_house(player, property_index)) {
        return 0;
    }
    
    Property* prop = &property_array[property_index];
    int cost = prop->house_construction_cost;
    
    // Deduct cost
    player->cash -= cost;
    
    // Add house
    prop->building_count++;
    
    // Reset condition to 100%
    prop->condition_percentage = 100;
    prop->rounds_since_maintenance = 0;
    
    printf("  %s constructed a house on %s. (Cost: LKR %d, Houses: %d)\n",
           player->player_name, prop->property_name, cost, prop->building_count);
    
    return 1;
}

// Check if a player can build a hotel on a property
// Returns: 1 = can build, 0 = cannot build
int can_build_hotel(Player* player, int property_index) {
    if (player == NULL) return 0;
    if (property_index < 0 || property_index >= MAX_PROPERTIES) return 0;
    
    Property* prop = &property_array[property_index];
    
    // Check if player owns the property
    if (prop->owner_id != player->player_id) {
        printf("  %s does not own %s!\n", player->player_name, prop->property_name);
        return 0;
    }
    
    // Check if property already has a hotel
    if (prop->building_count == 5) {
        printf("  %s already has a hotel.\n", prop->property_name);
        return 0;
    }
    
    // Check if property has 4 houses
    if (prop->building_count != 4) {
        printf("  %s needs 4 houses before building a hotel. Currently: %d\n",
               prop->property_name, prop->building_count);
        return 0;
    }
    
    // Check if player has a monopoly on this property's color group
    if (!has_monopoly(player, prop->color_group)) {
        printf("  %s does not have a monopoly on this color group.\n", player->player_name);
        return 0;
    }
    
    // Check if player has enough cash
    int cost = prop->hotel_construction_cost;
    if (player->cash < cost) {
        printf("  Insufficient funds! Hotel cost: LKR %d, Available: LKR %d\n", 
               cost, player->cash);
        return 0;
    }
    
    // Check even building rule: all properties in group must have 4 houses
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        if (property_array[i].color_group == prop->color_group && 
            property_array[i].owner_id == player->player_id) {
            if (property_array[i].building_count != 4) {
                printf("  Even building rule: %s has %d houses, but %s has %d.\n",
                       prop->property_name, prop->building_count,
                       property_array[i].property_name,
                       property_array[i].building_count);
                return 0;
            }
        }
    }
    
    return 1;
}

// Build a hotel on a property (upgrade from 4 houses)
// Returns: 1 = success, 0 = failure
int build_hotel(Player* player, int property_index) {
    if (player == NULL) return 0;
    if (property_index < 0 || property_index >= MAX_PROPERTIES) return 0;
    
    // Check if can build hotel
    if (!can_build_hotel(player, property_index)) {
        return 0;
    }
    
    Property* prop = &property_array[property_index];
    int cost = prop->hotel_construction_cost;
    
    // Deduct cost
    player->cash -= cost;
    
    // Upgrade to hotel (building_count = 5)
    prop->building_count = 5;
    
    // Reset condition to 100%
    prop->condition_percentage = 100;
    prop->rounds_since_maintenance = 0;
    
    printf("  %s upgraded %s to a HOTEL! (Cost: LKR %d)\n",
           player->player_name, prop->property_name, cost);
    
    return 1;
}

// Calculate rent for a property including building multipliers
// Returns: Rent amount in LKR
int calculate_rent_with_buildings(Property* prop) {
    if (prop == NULL) return 0;
    
    int base_rent = prop->base_rent;
    int buildings = prop->building_count;
    
    // If property is mortgaged, no rent collected
    if (prop->is_mortgaged) {
        printf("  %s is mortgaged. No rent collected.\n", prop->property_name);
        return 0;
    }
    
    // If building is closed (condition < 25%), no rent
    if (prop->condition_percentage < 25 && buildings > 0) {
        printf("  %s building is closed. No rent collected.\n", prop->property_name);
        return 0;
    }
    
    // Apply rent multiplier based on buildings (from assignment)
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
    
    // Apply condition multiplier (from Table 3)
    int condition_multiplier = get_rent_multiplier(prop);
    if (condition_multiplier == 0) {
        return 0;  // Building closed
    }
    
    // Calculate rent: base_rent * building_multiplier * condition_multiplier / 100
    int rent = (base_rent * multiplier * condition_multiplier) / 100;
    
    // If no buildings, condition doesn't apply (use 100%)
    if (buildings == 0) {
        rent = base_rent;
    }
    
    return rent;
}

// Get rent multiplier based on building condition (Table 3)
int get_rent_multiplier(Property* prop) {
    if (prop == NULL) return 0;
    
    int condition = prop->condition_percentage;
    
    // If no buildings, condition doesn't matter
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

// Get current building multiplier based on building count
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

// Print building status for a property
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

// Print all buildings for a player
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

// ============================================
// BUILDING CONDITION SYSTEM
// ============================================

// Update building condition - called at end of each round
// Decreases condition by 2% per round for buildings
void update_building_condition(Property* prop) {
    if (prop == NULL) return;
    
    // Only applies to properties with buildings
    if (prop->building_count == 0) return;
    
    // Decrease condition by 2% each round
    prop->condition_percentage -= 2;
    
    // Cap at 0 (minimum)
    if (prop->condition_percentage < 0) {
        prop->condition_percentage = 0;
    }
    
    // Track rounds since maintenance
    prop->rounds_since_maintenance++;
    
    // Check if condition dropped below 25% (building closed)
    if (prop->condition_percentage < 25) {
        printf("  WARNING: %s building is CLOSED! (Condition: %d%%)\n",
               prop->property_name, prop->condition_percentage);
    }
    
    // Check for structural damage (20+ consecutive rounds without maintenance)
    if (prop->rounds_since_maintenance > 20 && !prop->has_structural_damage) {
        process_structural_damage(prop);
    }
}

// Perform maintenance on a property's building
// Restores condition to 100%
// Returns: 1 = success, 0 = failure
int perform_maintenance(Player* player, int property_index) {
    if (player == NULL) return 0;
    if (property_index < 0 || property_index >= MAX_PROPERTIES) return 0;
    
    Property* prop = &property_array[property_index];
    
    // Check if player owns the property
    if (prop->owner_id != player->player_id) {
        printf("  %s does not own %s!\n", player->player_name, prop->property_name);
        return 0;
    }
    
    // Check if property has buildings
    if (prop->building_count == 0) {
        printf("  %s has no buildings to maintain.\n", prop->property_name);
        return 0;
    }
    
    // Check if condition is already 100%
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
    
    // Check if player has enough cash
    if (player->cash < cost) {
        printf("  Insufficient funds! Maintenance cost: LKR %d, Available: LKR %d\n",
               cost, player->cash);
        return 0;
    }
    
    // Deduct cost
    player->cash -= cost;
    
    // Restore condition to 100%
    prop->condition_percentage = 100;
    prop->rounds_since_maintenance = 0;
    
    // If had structural damage, remove it (renovation required separately)
    // Maintenance alone doesn't fix structural damage
    
    printf("  %s performed maintenance on %s. (Cost: LKR %d)\n",
           player->player_name, prop->property_name, cost);
    printf("    Condition restored to 100%%.\n");
    
    return 1;
}

// Get maintenance cost for a property
// Returns: Cost in LKR
int get_maintenance_cost(Property* prop) {
    if (prop == NULL) return 0;
    if (prop->building_count == 0) return 0;
    
    int construction_cost;
    int percentage;
    
    if (prop->building_count == 5) {
        // Hotel: 8% of hotel construction cost
        construction_cost = prop->hotel_construction_cost;
        percentage = 8;
    } else {
        // House: 5% of house construction cost
        construction_cost = prop->house_construction_cost;
        percentage = 5;
    }
    
    int cost = (construction_cost * percentage) / 100;
    
    // If property has structural damage, maintenance costs 50% more
    if (prop->has_structural_damage) {
        cost = (cost * 150) / 100;
    }
    
    return cost;
}

// Process structural damage on a property
// Occurs when maintenance is ignored for more than 20 consecutive rounds
void process_structural_damage(Property* prop) {
    if (prop == NULL) return;
    if (prop->has_structural_damage) return;
    
    printf("\n  ⚠️ STRUCTURAL DAMAGE on %s! ⚠️\n", prop->property_name);
    printf("  Maintenance ignored for over 20 rounds.\n");
    
    // Mark as structurally damaged
    prop->has_structural_damage = 1;
    
    // Reduce property value by 15%
    prop->value_reduction += 15;
    if (prop->value_reduction > 30) {
        prop->value_reduction = 30;  // Max 30%
    }
    
    // Reduce maximum rent by 25%
    // We'll handle this in calculate_rent_with_buildings
    // For now, we'll store it as a flag
    
    printf("  Effects:\n");
    printf("    - Property value reduced by 15%% (Total reduction: %d%%)\n", 
           prop->value_reduction);
    printf("    - Maximum rent reduced by 25%%\n");
    printf("    - Future maintenance costs increased by 50%%\n");
    printf("  Renovation required to restore property.\n");
}

// Check if a building is closed (condition < 25%)
int is_building_closed(Property* prop) {
    if (prop == NULL) return 1;  // NULL considered closed
    if (prop->building_count == 0) return 0;  // No building = not closed
    
    return (prop->condition_percentage < 25);
}

// Check if a building needs maintenance (condition < 50%)
int needs_maintenance(Property* prop) {
    if (prop == NULL) return 0;
    if (prop->building_count == 0) return 0;
    
    return (prop->condition_percentage < 50);
}

// Get condition status as string
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

// Print condition of all buildings owned by a player
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

// Renovate a property (fixes structural damage and resets age)
// Note: This is different from maintenance
// Returns: 1 = success, 0 = failure
int renovate_property(Player* player, int property_index) {
    if (player == NULL) return 0;
    if (property_index < 0 || property_index >= MAX_PROPERTIES) return 0;
    
    Property* prop = &property_array[property_index];
    
    // Check if player owns the property
    if (prop->owner_id != player->player_id) {
        printf("  %s does not own %s!\n", player->player_name, prop->property_name);
        return 0;
    }
    
    // Check if property needs renovation
    if (!prop->has_structural_damage && prop->property_age <= 50) {
        printf("  %s does not need renovation.\n", prop->property_name);
        return 0;
    }
    
    // Calculate renovation cost: 25% of replacement value
    int property_value = get_property_value(prop);
    int renovation_cost = (property_value * 25) / 100;
    
    if (renovation_cost == 0) {
        renovation_cost = 1000;  // Minimum cost
    }
    
    // Check if player has enough cash
    if (player->cash < renovation_cost) {
        printf("  Insufficient funds! Renovation cost: LKR %d, Available: LKR %d\n",
               renovation_cost, player->cash);
        return 0;
    }
    
    // Deduct cost
    player->cash -= renovation_cost;
    
    // Restore property
    prop->has_structural_damage = 0;
    prop->property_age = 0;
    prop->value_reduction = 0;
    prop->condition_percentage = 100;
    prop->rounds_since_maintenance = 0;
    
    printf("  %s renovated %s. (Cost: LKR %d)\n",
           player->player_name, prop->property_name, renovation_cost);
    printf("    Property restored: age reset, damage repaired, value restored.\n");
    
    return 1;
}

// ============================================
// DEPRECIATION SYSTEM
// ============================================

// Update property age - called at end of each round
// Increments age for all owned properties
void update_property_age(Property* prop) {
    if (prop == NULL) return;
    
    // Only applies to owned properties
    if (prop->owner_id == -1) return;
    
    // Increment age by 1 each round
    prop->property_age++;
}

// Calculate depreciation for a property
// Properties older than 50 rounds lose 1% every 5 rounds (max 30%)
// Returns: Current depreciation percentage (0-30)
int calculate_depreciation(Property* prop) {
    if (prop == NULL) return 0;
    if (prop->owner_id == -1) return 0;
    
    int age = prop->property_age;
    
    // No depreciation until age > 50
    if (age <= PROPERTY_AGE_THRESHOLD) {
        return 0;
    }
    
    // Calculate rounds over threshold
    int over_threshold = age - PROPERTY_AGE_THRESHOLD;
    
    // 1% depreciation every 5 rounds
    int depreciation = over_threshold / 5;
    
    // Cap at 30%
    if (depreciation > 30) {
        depreciation = 30;
    }
    
    // Store in property struct
    prop->value_reduction = depreciation;
    
    return depreciation;
}

// Get current property value after depreciation
// This is an enhanced version of the helper function
// Returns: Current value in LKR
int get_depreciated_value(Property* prop) {
    if (prop == NULL) return 0;
    
    int value = prop->purchase_price;
    
    // Apply value reduction from depreciation
    if (prop->value_reduction > 0) {
        value = (value * (100 - prop->value_reduction)) / 100;
    }
    
    // Apply structural damage reduction (15% additional)
    if (prop->has_structural_damage) {
        value = (value * 85) / 100;
    }
    
    return value;
}

// Check if property needs renovation (age > 50 or structural damage)
int needs_renovation(Property* prop) {
    if (prop == NULL) return 0;
    if (prop->owner_id == -1) return 0;
    
    if (prop->has_structural_damage) return 1;
    if (prop->property_age > PROPERTY_AGE_THRESHOLD) return 1;
    
    return 0;
}

// Renovate property - resets age and fixes structural damage
// This is already defined in the condition system, but let's make sure it's complete
// Returns: 1 = success, 0 = failure
int renovate_property(Player* player, int property_index) {
    if (player == NULL) return 0;
    if (property_index < 0 || property_index >= MAX_PROPERTIES) return 0;
    
    Property* prop = &property_array[property_index];
    
    // Check if player owns the property
    if (prop->owner_id != player->player_id) {
        printf("  %s does not own %s!\n", player->player_name, prop->property_name);
        return 0;
    }
    
    // Check if property needs renovation
    if (!prop->has_structural_damage && prop->property_age <= PROPERTY_AGE_THRESHOLD) {
        printf("  %s does not need renovation. Age: %d, Threshold: %d\n",
               prop->property_name, prop->property_age, PROPERTY_AGE_THRESHOLD);
        return 0;
    }
    
    // Calculate renovation cost: 10% of current market value (from assignment)
    int property_value = get_property_value(prop);
    int renovation_cost = (property_value * 10) / 100;
    
    if (renovation_cost == 0) {
        renovation_cost = 500;  // Minimum cost
    }
    
    // Check if player has enough cash
    if (player->cash < renovation_cost) {
        printf("  Insufficient funds! Renovation cost: LKR %d, Available: LKR %d\n",
               renovation_cost, player->cash);
        return 0;
    }
    
    // Deduct cost
    player->cash -= renovation_cost;
    
    // Restore property
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

// Calculate net worth of a player including depreciated values
int calculate_net_worth(Player* player) {
    if (player == NULL) return 0;
    if (player->is_bankrupt) return 0;
    
    int net_worth = player->cash;
    
    // Add property values (using depreciated value)
    for (int i = 0; i < player->owned_property_count; i++) {
        int prop_idx = player->owned_property_indices[i];
        if (prop_idx < 0 || prop_idx >= MAX_PROPERTIES) continue;
        
        Property* prop = &property_array[prop_idx];
        
        // Add property value (with depreciation)
        net_worth += get_depreciated_value(prop);
        
        // Add building value
        if (prop->building_count > 0) {
            int building_value = 0;
            if (prop->building_count == 5) {
                // Hotel value = hotel construction cost
                building_value = prop->hotel_construction_cost;
            } else {
                // House value = house construction cost × number of houses
                building_value = prop->house_construction_cost * prop->building_count;
            }
            net_worth += building_value;
        }
        
        // Add railway/utility values (handled by property value already)
    }
    
    // Subtract loan amount
    if (player->player_loan.is_active) {
        net_worth -= player->player_loan.current_amount;
    }
    
    return net_worth;
}

// Print property depreciation status
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
            printf("    ⚠️ STRUCTURAL DAMAGE - Needs renovation!\n");
        }
        if (prop->property_age > PROPERTY_AGE_THRESHOLD + 20) {
            printf("    ⚠️ Property is significantly aged - consider renovation.\n");
        }
        
        has_properties = 1;
    }
    
    if (!has_properties) {
        printf("  No properties owned.\n");
    }
    printf("================================\n");
}