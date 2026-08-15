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
#define MAX_COLLATERAL MAX_PROPERTIES

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
    GROUP_UTILITY,
    GROUP_NONE
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

// Stores the complete result of rolling two dice. Keeping the individual
// values makes doubles available for jail and consecutive-doubles rules.
typedef struct {
    int first_die;
    int second_die;
    int total;
    int is_double;
} DiceRoll;

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
    int event_closed_rounds;   // Temporary closure caused by an event card
    
    // Group
    PropertyGroup color_group;

    // Geographic/category flags used to target economic and regional events.
    unsigned int classification_flags;
    
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
    int last_dice_total;       // Most recent dice roll; 0 before the first turn
    
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
    PropertyGroup affected_group; // Used by Property Revaluation
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
    int market_effects_applied;
    
} GameState;

// ============================================
// EXTERNAL GLOBAL ARRAYS
// (Defined in board.c, accessible everywhere)
// ============================================

extern Square board_array[BOARD_SIZE];
extern Property property_array[MAX_PROPERTIES];

// ============================================
// FUNCTION PROTOTYPES
// ============================================

// Prototypes from game.c
void init_game(GameState* game);
void print_game_start(GameState* game);
void run_game(GameState* game);
Player* get_player_by_id(GameState* game, int player_id);
int get_round(const GameState* game);
int check_game_over(GameState* game);
Player* determine_winner(GameState* game);
void print_winner_details(const Player* winner);
int determine_first_player(GameState* game);
void process_turn(GameState* game, Player* player);
void resolve_landing(GameState* game, Player* player);

// Prototypes from board.c
void init_board_data(void);
DiceRoll roll_dice(void);
int move_player(Player* player, int dice_total);
Square* get_square(int position);
Property* get_property(int property_index);
Property* get_property_at_position(int position);
int is_property_square(int position);
SquareType get_square_type(int position);
int count_properties_in_group(Player* player, PropertyGroup group);


// Prototypes from finance.c
int get_property_value(Property* prop);
int get_total_mortgage_value(Player* player);
int has_monopoly(Player* player, PropertyGroup group);
int get_min_buildings_in_group(Player* player, PropertyGroup group);
void print_player_finance(Player* player);

// loan system
int get_max_loan_amount(Player* player);
int take_loan(Player* player, int amount);
int repay_loan(Player* player, int amount);
void apply_loan_interest(Player* player);
void process_loan_default(Player* player);
void declare_bankruptcy(Player* player, const char* reason);
void lock_collateral(Player* player, int loan_amount);
void unlock_collateral(Player* player);

//  Insurance System 
int calculate_insurance_premium(Property* prop, int policy_type);
int buy_insurance(Player* player, int property_index, int policy_type);
void process_insurance_expiry(Property* prop);
int process_disaster_claim(Property* prop, int damage_cost);
int has_active_insurance(Property* prop);
int get_insurance_reminder(Property* prop);
const char* get_insurance_policy_name(int policy_type);
void print_player_insurance(Player* player);


// finance.c - Building System Prototypes
int can_build_house(Player* player, int property_index);
int build_house(Player* player, int property_index);
int can_build_hotel(Player* player, int property_index);
int build_hotel(Player* player, int property_index);
int calculate_rent_with_buildings(Property* prop);
int get_rent_multiplier(Property* prop);
int get_building_multiplier(int building_count);
void print_building_status(Property* prop);
void print_player_buildings(Player* player);
int get_building_cost(Property* prop, GameState* game);


// finance.c - Building Condition Prototypes
void update_building_condition(Property* prop);
int perform_maintenance(Player* player, int property_index);
int get_maintenance_cost(Property* prop);
void process_structural_damage(Property* prop);
int is_building_closed(Property* prop);
int needs_maintenance(Property* prop);
const char* get_condition_status(Property* prop);
void print_player_conditions(Player* player);
int renovate_property(Player* player, int property_index);

// finance.c - Property Depreciation Prototypes
void update_property_age(Property* prop);
int calculate_depreciation(Property* prop);
int get_depreciated_value(Property* prop);
int needs_renovation(Property* prop);
int calculate_total_property_value(Player* player);
int calculate_net_worth(Player* player);
void print_property_depreciation(Player* player);


// game.c prototypes
void end_of_round_processing(GameState* game);
void process_inflation(GameState* game);
void check_disaster(GameState* game);
void process_market_review(GameState* game);
void process_national_event(GameState* game);
void process_regional_development(GameState* game);
void process_government_regulation(GameState* game);
void buy_property(Player* player, Property* prop);
void start_auction(GameState* game, Property* prop);


// players.c - AI Strategy Prototypes
int should_buy_property(Player* player, Property* prop);
int get_auction_bid(Player* player, Property* prop, int current_bid);
int should_take_loan(Player* player);
int get_loan_amount(Player* player);
int should_repay_loan(Player* player);
int should_build(Player* player);
int choose_build_property(Player* player);
int should_build_hotel(Player* player);
int choose_hotel_property(Player* player);
int should_buy_insurance(Player* player, int property_index);
int get_insurance_type(Player* player, int property_index);
int should_renovate(Player* player, int property_index);

// players.c - Aggressive Investor strategy
int aggressive_should_buy(Player* player, Property* prop);
int aggressive_auction_bid(Player* player, Property* prop, int current_bid);
int aggressive_should_loan(Player* player);
int aggressive_loan_amount(Player* player);
int aggressive_should_repay(Player* player);
int aggressive_should_build(Player* player);
int aggressive_choose_build(Player* player);
int aggressive_should_hotel(Player* player);
int aggressive_choose_hotel(Player* player);
int aggressive_insurance_type(Player* player, int property_index);
int aggressive_should_renovate(Player* player, int property_index);

// players.c - Conservative Banker strategy
int conservative_should_buy(Player* player, Property* prop);
int conservative_auction_bid(Player* player, Property* prop, int current_bid);
int conservative_should_loan(Player* player);
int conservative_loan_amount(Player* player);
int conservative_should_repay(Player* player);
int conservative_should_build(Player* player);
int conservative_choose_build(Player* player);
int conservative_should_hotel(Player* player);
int conservative_choose_hotel(Player* player);
int conservative_should_insure(Player* player, int property_index);
int conservative_should_renovate(Player* player, int property_index);

// players.c - Risk Taker strategy
int risk_taker_should_buy(Player* player, Property* prop);
int risk_taker_auction_bid(Player* player, Property* prop, int current_bid);
int risk_taker_should_loan(Player* player);
int risk_taker_loan_amount(Player* player);
int risk_taker_should_repay(Player* player);
int risk_taker_should_build(Player* player);
int risk_taker_choose_build(Player* player);
int risk_taker_should_hotel(Player* player);
int risk_taker_choose_hotel(Player* player);
int risk_taker_should_insure(Player* player, int property_index);
int risk_taker_should_renovate(Player* player, int property_index);

// players.c - Opportunistic Trader strategy
int opportunistic_should_buy(Player* player, Property* prop);
int opportunistic_auction_bid(Player* player, Property* prop, int current_bid);
int opportunistic_should_loan(Player* player);
int opportunistic_loan_amount(Player* player);
int opportunistic_should_repay(Player* player);
int opportunistic_should_build(Player* player);
int opportunistic_choose_build(Player* player);
int opportunistic_should_hotel(Player* player);
int opportunistic_choose_hotel(Player* player);
int opportunistic_should_insure(Player* player, int property_index);
int opportunistic_should_renovate(Player* player, int property_index);


// events.c prototypes
void init_event_deck();
void draw_event_card(GameState* game);
void process_national_event(GameState* game);
void process_inflation(GameState* game);
void process_government_regulation(GameState* game);
void process_regional_development(GameState* game);
void process_market_review(GameState* game);
void check_disaster(GameState* game);
void update_event_durations(GameState* game);
// events.c - Event effect prototypes
int apply_event_rent_modifiers(Property* prop, GameState* game, int current_rent);
int apply_event_value_modifiers(Property* prop, GameState* game, int current_value);
int apply_event_construction_modifiers(int cost, GameState* game);
int apply_event_insurance_modifiers(int premium, GameState* game);
void remove_event_effects(GameState* game);

#endif // TYPES_H

