#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

// ============================================
// GAME CONSTANTS
// ============================================

#define BOARD_SIZE 40
#define MAX_PLAYERS 4
#define MAX_PROPERTIES 28
#define MAX_COLLATERAL 10

#define STARTING_CASH 30000
#define GO_BONUS 2000
#define JAIL_BAIL 300
#define MAX_ROUNDS 500

#define AUCTION_INCREMENT 250
#define INSURANCE_DURATION 20
#define LOAN_DURATION 20
#define PROPERTY_AGE_THRESHOLD 50

// ============================================
// ENUMERATIONS
// ============================================

// Square types - what kind of space on the board
typedef enum {
    SQUARE_START,
    SQUARE_PROPERTY,
    SQUARE_EVENT,
    SQUARE_TAX,
    SQUARE_RAILWAY,
    SQUARE_UTILITY,
    SQUARE_JAIL,
    SQUARE_FREE_PARKING,
    SQUARE_GO_TO_JAIL,
    SQUARE_BANK,
    SQUARE_INSURANCE
} SquareType;

// Property color groups
typedef enum {
    GROUP_BROWN,
    GROUP_LIGHT_BLUE,
    GROUP_PINK,
    GROUP_ORANGE,
    GROUP_RED,
    GROUP_YELLOW,
    GROUP_GREEN,
    GROUP_DARK_BLUE,
    GROUP_RAILWAY,
    GROUP_UTILITY
} PropertyGroup;

// Player AI strategies
typedef enum {
    STRATEGY_AGGRESSIVE,
    STRATEGY_CONSERVATIVE,
    STRATEGY_RISK_TAKER,
    STRATEGY_OPPORTUNISTIC
} PlayerStrategy;

// Insurance policy types
typedef enum {
    INSURANCE_NONE ,
    INSURANCE_BASIC,
    INSURANCE_COMPREHENSIVE,
    INSURANCE_BUSINESS 
} InsurancePolicy;

// ============================================
// STRUCTS
// ============================================

// Property structure - represents a purchasable asset
typedef struct {
    char property_name[50];
    
    // Financial values (in LKR)
    int purchase_price;
    int mortgage_value;
    int base_rent;
    int house_construction_cost;
    int hotel_construction_cost;
    
    // Ownership and status
    int owner_id;              // -1 = unowned, otherwise player index (0-3)
    int is_mortgaged;          // 0 = no, 1 = yes
    int is_loan_locked;        // 0 = no, 1 = used as collateral
    
    // Buildings
    int building_count;        // 0-4 houses, 5 = hotel
    
    // Insurance
    int insurance_policy;      // 0 = none, 1-3 = policy type
    int insurance_rounds_remaining;
    
    // Depreciation and condition
    int property_age;          // Rounds since purchase/renovation
    int condition_percentage;  // 0-100, starts at 100
    int rounds_since_maintenance;
    int has_structural_damage; // 0 = no, 1 = yes
    int value_reduction;       // Percentage reduction (0-30)
    
    // Group
    PropertyGroup color_group;
    
} Property;

// Square structure - represents a position on the board
typedef struct {
    int square_index;          // 0-39
    char square_name[50];
    SquareType square_type;
    int property_index;        // -1 if not a property, else index into properties array
} Square;

// Loan structure
typedef struct {
    int is_active;             // 0 = no, 1 = yes
    int current_amount;
    int original_amount;
    int interest_rate;         // Percentage (e.g., 8 for 8%)
    int rounds_remaining;
    int initial_duration;      // Always 20
    
    int collateral_properties[MAX_COLLATERAL];  // Property indices
    int collateral_count;
} Loan;

// Player structure
typedef struct {
    int player_id;             // 0-3
    char player_name[30];
    PlayerStrategy strategy;
    
    // Finances
    int cash;
    
    // Board position
    int board_position;        // 0-39
    
    // Owned properties
    int owned_property_indices[MAX_PROPERTIES];  // List of property indices
    int owned_property_count;
    
    // Loan
    Loan player_loan;
    
    // Jail status
    int is_in_jail;
    int jail_turns_served;
    int consecutive_doubles_rolled;
    
    // Status
    int is_bankrupt;
    int rounds_survived;
    
    // Statistics (for AI decisions)
    int total_rent_collected;
    int total_expenses_paid;
    int auction_wins;
    
} Player;

// Active event tracking structure (nested inside GameState)
typedef struct {
    int is_active;
    char event_name[50];
    int rounds_remaining;
    int effect_percentage;
} ActiveEvent;

// Regional development tracking structure (NEW)
typedef struct {
    int is_active;
    char event_name[50];
    char region_name[50];      // Which region is affected
    int rounds_remaining;
    int effect_percentage;
} RegionalDevelopment;

// Active regulation tracking
typedef struct {
    int is_active;
    char regulation_name[50];
    int rounds_remaining;
    int effect_percentage;
} ActiveRegulation;

// Market boom/decline tracking
typedef struct {
    int is_active;
    char group_name[50];
    int rounds_remaining;
    int effect_percentage;
} MarketCondition;

// Game state structure - overall game data
typedef struct {
    int round_number;
    int current_player_index;
    int starting_player_index;
    // Stores player array indices in the order that players take their turns.
    int turn_order[MAX_PLAYERS];
    int is_game_over;
    int winner_player_id;
    Player players[MAX_PLAYERS];
    
    // Active economy events
    ActiveEvent national_event;
    RegionalDevelopment regional_development;
    ActiveRegulation government_regulation;
    MarketCondition market_boom;
    MarketCondition market_decline;
    
    // Current rates
    int current_inflation_rate;    // -3 to 12 (percesntage)
    int current_interest_rate;     // Base loan interest rate
    
} GameState;

// ============================================
// EXTERNAL GLOBAL ARRAYS
// (Defined in board.c, accessible everywhere)
// ============================================

extern Square board_array[BOARD_SIZE];
extern Property property_array[MAX_PROPERTIES];

#endif // TYPES_H
