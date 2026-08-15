#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ============================================
// CONSTANTS
// ============================================

// Inflation rates (from Rule-LK 12)
// Possible values: -3, 0, 2, 5, 8, 12
static const int INFLATION_RATES[] = {-3, 0, 2, 5, 8, 12};
static const int NUM_INFLATION_RATES = 6;

// Government regulation names (from Rule-LK 24)
static const char* REGULATION_NAMES[] = {
    "Increase Property Tax",
    "Reduce Loan Interest",
    "Housing Subsidy",
    "Luxury Property Tax",
    "Railway Modernization",
    "Electricity Tariff Revision",
    "Insurance Regulation",
    "Anti-Speculation Act"
};

static const int NUM_REGULATIONS = 8;

// Regional development regions (from Rule-LK 35)
static const char* REGIONS[] = {
    "Southern Province",
    "Western Province",
    "Central Province",
    "Northern Province",
    "Eastern Province",
    "North Western Province",
    "North Central Province",
    "Uva Province",
    "Sabaragamuwa Province"
};

static const int NUM_REGIONS = 9;

// Disaster types (from Rule-LK 10)
static const char* DISASTER_TYPES[] = {
    "Fire",
    "Flood",
    "Riot",
    "Building Collapse",
    "Electrical Failure"
};

static const int NUM_DISASTERS = 5;

// ============================================
// EVENT CARD DECK
// ============================================

// Event card structure
typedef struct {
    char name[50];
    char description[100];
    int effect_percentage;
    int duration_rounds; // 0 means the card takes effect immediately
    int target_type;  // 0 = all players, 1 = properties, 2 = regions
    char target_region[50];
} EventCard;

// National event deck (20 cards)
static EventCard national_deck[20];
static int deck_index = 0;

// ============================================
// FORWARD DECLARATIONS
// ============================================

void init_event_deck();
static void execute_event_card(GameState* game, EventCard* card);

static int is_standard_property(Property* prop) {
    return prop->color_group >= GROUP_BROWN &&
           prop->color_group <= GROUP_DARK_BLUE;
}

static int is_coastal_property(Property* prop) {
    const char* name = prop->property_name;

    return strstr(name, "Mount Lavinia") != NULL ||
           strstr(name, "Negombo") != NULL ||
           strstr(name, "Galle") != NULL ||
           strstr(name, "Unawatuna") != NULL ||
           strstr(name, "Hikkaduwa") != NULL ||
           strstr(name, "Trincomalee") != NULL;
}

static int choose_random_property(int developed_only, int coastal_only) {
    int choices[MAX_PROPERTIES];
    int choice_count = 0;

    for (int i = 0; i < MAX_PROPERTIES; i++) {
        Property* prop = &property_array[i];

        if (prop->owner_id == -1) continue;
        if (developed_only && prop->building_count == 0) continue;
        if (coastal_only && !is_coastal_property(prop)) continue;

        choices[choice_count] = i;
        choice_count++;
    }

    if (choice_count == 0) return -1;
    return choices[rand() % choice_count];
}

static void damage_property_from_card(int property_index) {
    if (property_index == -1) {
        printf("  No suitable owned property was available.\n");
        return;
    }

    Property* prop = &property_array[property_index];
    prop->has_structural_damage = 1;
    prop->condition_percentage -= 20;

    if (prop->condition_percentage < 0) {
        prop->condition_percentage = 0;
    }

    printf("  %s was damaged. Its condition is now %d%%.\n",
           prop->property_name, prop->condition_percentage);
}

// ============================================
// FUNCTIONS
// ============================================

void init_event_deck() {
    national_deck[0] = (EventCard){"Tourism Hype", "Hotels earn double rent for 5 rounds.", 100, 5, 1, "Hotels"};
    national_deck[1] = (EventCard){"Fuel Shortage", "Railway rent doubles for 5 rounds.", 100, 5, 1, "Railway"};
    national_deck[2] = (EventCard){"Heavy Floods", "A random coastal property is damaged.", 0, 0, 1, "Coastal"};
    national_deck[3] = (EventCard){"Political Rally", "One random property is closed for 2 rounds.", 0, 2, 1, "Random Property"};
    national_deck[4] = (EventCard){"Stock Market Rise", "All property values increase by 10%.", 10, 15, 1, "All"};
    national_deck[5] = (EventCard){"Economic Downturn", "All property values decrease by 15%.", -15, 15, 1, "All"};
    national_deck[6] = (EventCard){"Housing Subsidy", "House construction cost is reduced by 30%.", -30, 15, 1, "All"};
    national_deck[7] = (EventCard){"Interest Rate Cut", "The current interest rate decreases by 2%.", -2, 15, 0, "All"};
    national_deck[8] = (EventCard){"Interest Rate Increase", "The current interest rate increases by 2%.", 2, 15, 0, "All"};
    national_deck[9] = (EventCard){"Tax Amnesty", "Each player receives LKR 2,000.", 2000, 0, 0, "All Players"};
    national_deck[10] = (EventCard){"Power Failure", "Utility income is halved for 3 rounds.", -50, 3, 1, "Utility"};
    national_deck[11] = (EventCard){"Foreign Funding", "Commercial property values increase by 15%.", 15, 15, 1, "Commercial"};
    national_deck[12] = (EventCard){"Port Expansion", "Railway station values increase by 20%.", 20, 15, 1, "Railway"};
    national_deck[13] = (EventCard){"Festival Season", "Hotels receive 50% additional rent.", 50, 15, 1, "Hotels"};
    national_deck[14] = (EventCard){"Labour Strike", "Construction is suspended for 2 rounds.", 0, 2, 0, "All"};
    national_deck[15] = (EventCard){"Insurance Discount", "Insurance premiums are reduced by 20%.", -20, 15, 0, "All"};
    national_deck[16] = (EventCard){"Property Revaluation", "A random property group appreciates by 15%.", 15, 15, 2, "Random Group"};
    national_deck[17] = (EventCard){"Currency Depreciation", "Construction costs increase by 10%.", 10, 15, 0, "All"};
    national_deck[18] = (EventCard){"Government Grant", "A random player receives LKR 5,000.", 5000, 0, 0, "Random Player"};
    national_deck[19] = (EventCard){"National Disaster", "A random developed property is damaged.", 0, 0, 1, "Developed Property"};

    deck_index = 0;
}

static void execute_event_card(GameState* game, EventCard* card) {
    if (strcmp(card->name, "Heavy Floods") == 0) {
        int property_index = choose_random_property(0, 1);
        damage_property_from_card(property_index);
    }
    else if (strcmp(card->name, "Political Rally") == 0) {
        int property_index = choose_random_property(0, 0);

        if (property_index == -1) {
            printf("  No owned property was available to close.\n");
        } else {
            property_array[property_index].event_closed_rounds = 2;
            printf("  %s is closed for 2 rounds.\n",
                   property_array[property_index].property_name);
        }
    }
    else if (strcmp(card->name, "Interest Rate Cut") == 0) {
        game->current_interest_rate -= 2;
        if (game->current_interest_rate < 0) {
            game->current_interest_rate = 0;
        }
        printf("  Loan interest is now %d%%.\n", game->current_interest_rate);
    }
    else if (strcmp(card->name, "Interest Rate Increase") == 0) {
        game->current_interest_rate += 2;
        printf("  Loan interest is now %d%%.\n", game->current_interest_rate);
    }
    else if (strcmp(card->name, "Tax Amnesty") == 0) {
        for (int i = 0; i < MAX_PLAYERS; i++) {
            if (!game->players[i].is_bankrupt) {
                game->players[i].cash += 2000;
            }
        }
        printf("  Every solvent player received LKR 2,000.\n");
    }
    else if (strcmp(card->name, "Property Revaluation") == 0) {
        PropertyGroup groups[] = {
            GROUP_BROWN, GROUP_LIGHT_BLUE, GROUP_PINK, GROUP_ORANGE,
            GROUP_RED, GROUP_YELLOW, GROUP_GREEN, GROUP_DARK_BLUE
        };

        game->national_event.affected_group = groups[rand() % 8];
        printf("  One random property group will receive 15%% appreciation.\n");
    }
    else if (strcmp(card->name, "Government Grant") == 0) {
        int eligible_players[MAX_PLAYERS];
        int eligible_count = 0;

        for (int i = 0; i < MAX_PLAYERS; i++) {
            if (!game->players[i].is_bankrupt) {
                eligible_players[eligible_count] = i;
                eligible_count++;
            }
        }

        if (eligible_count > 0) {
            int winner = eligible_players[rand() % eligible_count];
            game->players[winner].cash += 5000;
            printf("  %s received LKR 5,000.\n",
                   game->players[winner].player_name);
        }
    }
    else if (strcmp(card->name, "National Disaster") == 0) {
        int property_index = choose_random_property(1, 0);
        damage_property_from_card(property_index);
    }
    else {
        printf("  The temporary modifier is now active.\n");
    }
}

void draw_event_card(GameState* game) {
    if (game == NULL) return;
    
    // Draw the current card
    EventCard card = national_deck[deck_index];
    
    // Advance deck index (wrap around)
    deck_index = (deck_index + 1) % 20;
    
    // Print the card
    printf("  🃏 EVENT CARD: %s\n", card.name);
    printf("     %s\n", card.description);
    
    // Apply the card effect to game state
    game->national_event.is_active = 1;
    game->national_event.rounds_remaining = card.duration_rounds;
    game->national_event.effect_percentage = card.effect_percentage;
    game->national_event.affected_group = GROUP_NONE;
    strcpy(game->national_event.event_name, card.name);

    execute_event_card(game, &card);

    if (card.duration_rounds == 0) {
        game->national_event.is_active = 0;
    }
    
    if (card.duration_rounds > 0) {
        printf("  Effect duration: %d rounds.\n", card.duration_rounds);
    } else {
        printf("  This card has an immediate effect.\n");
    }
}

void process_national_event(GameState* game) {
    if (game == NULL) return;
    
    printf("\n🏛️ NATIONAL EVENT TRIGGERED! (Every 15 Rounds)\n");
    
    // Draw a card from the deck
    EventCard card = national_deck[deck_index];
    deck_index = (deck_index + 1) % 20;
    
    // Print the event
    printf("  📜 %s\n", card.name);
    printf("     %s\n", card.description);
    
    // Store in game state
    game->national_event.is_active = 1;
    game->national_event.rounds_remaining = 15;
    game->national_event.effect_percentage = card.effect_percentage;
    strcpy(game->national_event.event_name, card.name);
    
    printf("  ✅ Effect active for 15 rounds.\n");
}

void process_inflation(GameState* game) {
    if (game == NULL) return;
    
    printf("\n📈 INFLATION TRIGGERED! (Every 10 Rounds)\n");
    
    // Select random inflation rate from possible values
    int index = rand() % NUM_INFLATION_RATES;
    int inflation_rate = INFLATION_RATES[index];
    
    // Store in game state
    game->current_inflation_rate = inflation_rate;
    
    // Apply inflation to all properties
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        Property* prop = &property_array[i];
        
        // Update property purchase price
        if (inflation_rate != 0) {
            prop->purchase_price = (prop->purchase_price * (100 + inflation_rate)) / 100;
            prop->mortgage_value = (prop->mortgage_value * (100 + inflation_rate)) / 100;
            prop->base_rent = (prop->base_rent * (100 + inflation_rate)) / 100;
            prop->house_construction_cost = (prop->house_construction_cost * (100 + inflation_rate)) / 100;
            prop->hotel_construction_cost = (prop->hotel_construction_cost * (100 + inflation_rate)) / 100;
        }
    }
    
    // Update loan interest rate
    if (inflation_rate != 0) {
        game->current_interest_rate = (game->current_interest_rate * (100 + inflation_rate)) / 100;
        if (game->current_interest_rate < 0) game->current_interest_rate = 0;
        if (game->current_interest_rate > 25) game->current_interest_rate = 25;
    }
    
    // Print results
    if (inflation_rate > 0) {
        printf("  📊 Inflation: +%d%%\n", inflation_rate);
    } else if (inflation_rate < 0) {
        printf("  📊 Deflation: %d%%\n", inflation_rate);
    } else {
        printf("  📊 No inflation this period.\n");
    }
    printf("  Current loan interest rate: %d%%\n", game->current_interest_rate);
}

void process_government_regulation(GameState* game) {
    if (game == NULL) return;
    
    printf("\n🏛️ GOVERNMENT REGULATION TRIGGERED! (Every 20 Rounds)\n");
    
    // Select random regulation
    int index = rand() % NUM_REGULATIONS;
    const char* regulation_name = REGULATION_NAMES[index];
    
    // Store in game state
    game->government_regulation.is_active = 1;
    game->government_regulation.rounds_remaining = 20;
    strcpy(game->government_regulation.regulation_name, regulation_name);
    
    // Apply regulation effects based on type
    switch (index) {
        case 0: { // Increase Property Tax
            game->government_regulation.effect_percentage = 50;  // Income Tax +50%
            printf("  📜 %s\n", regulation_name);
            printf("     Income Tax increases by 50%%.\n");
            break;
        }
        case 1: { // Reduce Loan Interest
            game->government_regulation.effect_percentage = -2;  // Interest -2%
            game->current_interest_rate -= 2;
            if (game->current_interest_rate < 0) {
                game->current_interest_rate = 0;
            }
            printf("  📜 %s\n", regulation_name);
            printf("     Loan interest decreases by 2%%.\n");
            break;
        }
        case 2: { // Housing Subsidy
            game->government_regulation.effect_percentage = -30;  // Construction -30%
            printf("  📜 %s\n", regulation_name);
            printf("     House construction costs reduce by 30%%.\n");
            // Apply to all properties
            for (int i = 0; i < MAX_PROPERTIES; i++) {
                property_array[i].house_construction_cost = 
                    (property_array[i].house_construction_cost * 70) / 100;
            }
            break;
        }
        case 3: { // Luxury Property Tax
            game->government_regulation.effect_percentage = 25;  // Hotels tax 25%
            printf("  📜 %s\n", regulation_name);
            printf("     Hotels incur annual maintenance tax of 25%%.\n");
            break;
        }
        case 4: { // Railway Modernization
            game->government_regulation.effect_percentage = 25;  // Railway rent +25%
            printf("  📜 %s\n", regulation_name);
            printf("     Railway rents increase 25%%.\n");
            // Apply to all railway properties
            for (int i = 0; i < MAX_PROPERTIES; i++) {
                if (property_array[i].color_group == GROUP_RAILWAY) {
                    property_array[i].base_rent = (property_array[i].base_rent * 125) / 100;
                }
            }
            break;
        }
        case 5: { // Electricity Tariff Revision
            game->government_regulation.effect_percentage = 20;  // Utility rent +20%
            printf("  📜 %s\n", regulation_name);
            printf("     Utility rents increase 20%%.\n");
            // Apply to all utility properties
            for (int i = 0; i < MAX_PROPERTIES; i++) {
                if (property_array[i].color_group == GROUP_UTILITY) {
                    property_array[i].base_rent = (property_array[i].base_rent * 120) / 100;
                }
            }
            break;
        }
        case 6: { // Insurance Regulation
            game->government_regulation.effect_percentage = -15;  // Insurance -15%
            printf("  📜 %s\n", regulation_name);
            printf("     Insurance premiums decrease 15%%.\n");
            // Note: Insurance premium calculation will need to consider this
            break;
        }
        case 7: { // Anti-Speculation Act
            game->government_regulation.effect_percentage = 3;  // Max 3 undeveloped
            printf("  📜 %s\n", regulation_name);
            printf("     Players may own at most 3 undeveloped properties.\n");
            printf("     Additional purchases require development within 5 rounds.\n");
            break;
        }
        default:
            printf("  Unknown regulation.\n");
            break;
    }
    
    printf("  ✅ Effect active for 20 rounds.\n");
}

void process_regional_development(GameState* game) {
    if (game == NULL) return;
    
    printf("\n🏗️ REGIONAL DEVELOPMENT TRIGGERED! (Every 15 Rounds)\n");
    
    // Select random region
    int index = rand() % NUM_REGIONS;
    const char* region = REGIONS[index];
    
    // Random effect value between 20-30%
    int effect = 20 + (rand() % 11);  // 20-30%
    
    // Store in game state
    game->regional_development.is_active = 1;
    game->regional_development.rounds_remaining = 15;
    game->regional_development.effect_percentage = effect;
    strcpy(game->regional_development.event_name, "Regional Development");
    strcpy(game->regional_development.region_name, region);
    
    printf("  📍 %s Development Programme\n", region);
    printf("     Property values in %s increase by %d%% for 15 rounds.\n", region, effect);
    printf("  ✅ Effect active for 15 rounds.\n");
}

void process_market_review(GameState* game) {
    if (game == NULL) return;
    
    printf("\n📊 MARKET REVIEW TRIGGERED! (Every 10 Rounds)\n");
    
    // Property groups (excluding RAILWAY and UTILITY)
    PropertyGroup groups[] = {
        GROUP_BROWN, GROUP_LIGHT_BLUE, GROUP_PINK, GROUP_ORANGE,
        GROUP_RED, GROUP_YELLOW, GROUP_GREEN, GROUP_DARK_BLUE
    };
    const char* group_names[] = {
        "Brown", "Light Blue", "Pink", "Orange",
        "Red", "Yellow", "Green", "Dark Blue"
    };
    int num_groups = 8;
    
    // Select random group for boom
    int boom_index = rand() % num_groups;
    PropertyGroup boom_group = groups[boom_index];
    
    // Select random group for decline (different from boom)
    int decline_index;
    do {
        decline_index = rand() % num_groups;
    } while (decline_index == boom_index);
    PropertyGroup decline_group = groups[decline_index];
    
    // Store boom in game state
    game->market_boom.is_active = 1;
    game->market_boom.rounds_remaining = 10;
    game->market_boom.effect_percentage = 20;  // +20% for boom
    strcpy(game->market_boom.group_name, group_names[boom_index]);
    
    // Store decline in game state
    game->market_decline.is_active = 1;
    game->market_decline.rounds_remaining = 10;
    game->market_decline.effect_percentage = -15;  // -15% for decline
    strcpy(game->market_decline.group_name, group_names[decline_index]);
    
    // Apply boom effects to properties in that group
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        if (property_array[i].color_group == boom_group) {
            property_array[i].purchase_price = (property_array[i].purchase_price * 120) / 100;
            property_array[i].mortgage_value = (property_array[i].mortgage_value * 120) / 100;
            property_array[i].base_rent = (property_array[i].base_rent * 125) / 100;
        }
        if (property_array[i].color_group == decline_group) {
            property_array[i].purchase_price = (property_array[i].purchase_price * 85) / 100;
            property_array[i].mortgage_value = (property_array[i].mortgage_value * 90) / 100;
            property_array[i].base_rent = (property_array[i].base_rent * 80) / 100;
        }
    }
    
    printf("  📈 BOOM: %s Group (+20%% value, +25%% rent)\n", group_names[boom_index]);
    printf("  📉 DECLINE: %s Group (-15%% value, -20%% rent)\n", group_names[decline_index]);
    printf("  ✅ Effects active for 10 rounds.\n");
}

void check_disaster(GameState* game) {
    if (game == NULL) return;
    
    printf("\n🌪️ DISASTER CHECK! (Every 10 Rounds)\n");
    
    // Find all developed properties (owned by players with buildings)
    int developed_properties[MAX_PROPERTIES];
    int num_developed = 0;
    
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        if (property_array[i].owner_id != -1 && property_array[i].building_count > 0) {
            developed_properties[num_developed++] = i;
        }
    }
    
    // If no developed properties, skip
    if (num_developed == 0) {
        printf("  No developed properties to damage.\n");
        return;
    }
    
    // Select random developed property
    int prop_index = developed_properties[rand() % num_developed];
    Property* prop = &property_array[prop_index];
    
    // Select random disaster type
    int disaster_index = rand() % NUM_DISASTERS;
    const char* disaster = DISASTER_TYPES[disaster_index];
    
    // Calculate damage: 10-30% of property value
    int property_value = get_property_value(prop);
    int damage = (property_value * (10 + (rand() % 21))) / 100;  // 10-30%
    
    printf("  🚨 %s strikes %s!\n", disaster, prop->property_name);
    printf("     Damage: LKR %d\n", damage);
    
    // Check if property has insurance
    if (prop->insurance_policy != INSURANCE_NONE) {
        // Process insurance claim
        int compensation = process_disaster_claim(prop, damage);
        if (compensation > 0) {
            // Add compensation to owner's cash
            Player* owner = &game->players[prop->owner_id];
            owner->cash += compensation;
            printf("     💰 Insurance compensation: LKR %d\n", compensation);
            printf("     Net loss: LKR %d\n", damage - compensation);
            
            // Deduct remaining damage from owner
            if (compensation < damage) {
                owner->cash -= (damage - compensation);
            }
        }
    } else {
        // No insurance - owner pays full damage
        Player* owner = &game->players[prop->owner_id];
        if (owner->cash >= damage) {
            owner->cash -= damage;
            printf("     💰 Owner pays full repair cost: LKR %d\n", damage);
        } else {
            // Can't afford repair - property becomes damaged
            printf("     ⚠️ Owner cannot afford repair!\n");
            // Mark property as damaged (condition drops)
            prop->condition_percentage -= 20;
            if (prop->condition_percentage < 0) {
                prop->condition_percentage = 0;
            }
        }
    }
}

void update_event_durations(GameState* game) {
    if (game == NULL) return;

    // Reduce temporary property closures created by Political Rally.
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        if (property_array[i].event_closed_rounds > 0) {
            property_array[i].event_closed_rounds--;

            if (property_array[i].event_closed_rounds == 0) {
                printf("  %s has reopened after the Political Rally.\n",
                       property_array[i].property_name);
            }
        }
    }
    
    // ============================================
    // 1. NATIONAL EVENT
    // ============================================
    if (game->national_event.is_active) {
        game->national_event.rounds_remaining--;
        
        // Check for renewal reminder (3 rounds before expiry)
        if (game->national_event.rounds_remaining == 3) {
            printf("  📢 REMINDER: National Event '%s' expires in 3 rounds!\n",
                   game->national_event.event_name);
        }
        
        // Check for expiry
        if (game->national_event.rounds_remaining <= 0) {
            printf("  📢 National Event '%s' has ended.\n",
                   game->national_event.event_name);
            game->national_event.is_active = 0;
            game->national_event.effect_percentage = 0;
            strcpy(game->national_event.event_name, "None");
        }
    }
    
    // ============================================
    // 2. REGIONAL DEVELOPMENT
    // ============================================
    if (game->regional_development.is_active) {
        game->regional_development.rounds_remaining--;
        
        if (game->regional_development.rounds_remaining == 3) {
            printf("  📢 REMINDER: Regional Development in %s expires in 3 rounds!\n",
                   game->regional_development.region_name);
        }
        
        if (game->regional_development.rounds_remaining <= 0) {
            printf("  📢 Regional Development in %s has ended.\n",
                   game->regional_development.region_name);
            game->regional_development.is_active = 0;
            game->regional_development.effect_percentage = 0;
            strcpy(game->regional_development.event_name, "None");
            strcpy(game->regional_development.region_name, "None");
        }
    }
    
    // ============================================
    // 3. GOVERNMENT REGULATION
    // ============================================
    if (game->government_regulation.is_active) {
        game->government_regulation.rounds_remaining--;
        
        if (game->government_regulation.rounds_remaining == 3) {
            printf("  📢 REMINDER: Government Regulation '%s' expires in 3 rounds!\n",
                   game->government_regulation.regulation_name);
        }
        
        if (game->government_regulation.rounds_remaining <= 0) {
            printf("  📢 Government Regulation '%s' has ended.\n",
                   game->government_regulation.regulation_name);
            game->government_regulation.is_active = 0;
            game->government_regulation.effect_percentage = 0;
            strcpy(game->government_regulation.regulation_name, "None");
        }
    }
    
    // ============================================
    // 4. MARKET BOOM
    // ============================================
    if (game->market_boom.is_active) {
        game->market_boom.rounds_remaining--;
        
        if (game->market_boom.rounds_remaining == 0) {
            printf("  📢 Market Boom for %s Group has ended.\n",
                   game->market_boom.group_name);
            game->market_boom.is_active = 0;
            game->market_boom.effect_percentage = 0;
            strcpy(game->market_boom.group_name, "None");
        }
    }
    
    // ============================================
    // 5. MARKET DECLINE
    // ============================================
    if (game->market_decline.is_active) {
        game->market_decline.rounds_remaining--;
        
        if (game->market_decline.rounds_remaining == 0) {
            printf("  📢 Market Decline for %s Group has ended.\n",
                   game->market_decline.group_name);
            game->market_decline.is_active = 0;
            game->market_decline.effect_percentage = 0;
            strcpy(game->market_decline.group_name, "None");
        }
    }
}

// ============================================
// APPLY EVENT EFFECTS
// ============================================

// Apply effects based on active event
int apply_event_rent_modifiers(Property* prop, GameState* game, int current_rent) {
    if (game == NULL || prop == NULL) return current_rent;
    
    // Tourism Hype: hotels get double rent
    if (game->national_event.is_active &&
        strcmp(game->national_event.event_name, "Tourism Hype") == 0) {
        if (prop->building_count == 5) {  // Hotel
            current_rent *= 2;
        }
    }
    
    // Fuel Shortage: railway rent doubles
    if (game->national_event.is_active &&
        strcmp(game->national_event.event_name, "Fuel Shortage") == 0) {
        if (prop->color_group == GROUP_RAILWAY) {
            current_rent *= 2;
        }
    }

    // Power Failure: utility income is halved
    if (game->national_event.is_active &&
        strcmp(game->national_event.event_name, "Power Failure") == 0) {
        if (prop->color_group == GROUP_UTILITY) {
            current_rent = (current_rent * 50) / 100;
        }
    }

    // Festival Season: hotels receive 50% additional rent
    if (game->national_event.is_active &&
        strcmp(game->national_event.event_name, "Festival Season") == 0) {
        if (prop->building_count == 5) {
            current_rent = (current_rent * 150) / 100;
        }
    }
    
    return current_rent;
}

// Apply event effects to property value
int apply_event_value_modifiers(Property* prop, GameState* game, int current_value) {
    if (game == NULL || prop == NULL) return current_value;
    
    // Economic Downturn: all property values decrease by 15%
    if (game->national_event.is_active &&
        strcmp(game->national_event.event_name, "Economic Downturn") == 0) {
        current_value = (current_value * 85) / 100;
    }
    
    // Stock Market Rise: all property values increase by 10%
    if (game->national_event.is_active &&
        strcmp(game->national_event.event_name, "Stock Market Rise") == 0) {
        current_value = (current_value * 110) / 100;
    }

    // Foreign Funding: standard commercial properties increase by 15%
    if (game->national_event.is_active &&
        strcmp(game->national_event.event_name, "Foreign Funding") == 0 &&
        is_standard_property(prop)) {
        current_value = (current_value * 115) / 100;
    }

    // Port Expansion: railway station values increase by 20%
    if (game->national_event.is_active &&
        strcmp(game->national_event.event_name, "Port Expansion") == 0 &&
        prop->color_group == GROUP_RAILWAY) {
        current_value = (current_value * 120) / 100;
    }

    // Property Revaluation: the selected property group appreciates by 15%
    if (game->national_event.is_active &&
        strcmp(game->national_event.event_name, "Property Revaluation") == 0 &&
        prop->color_group == game->national_event.affected_group) {
        current_value = (current_value * 115) / 100;
    }
    
    return current_value;
}

// Apply event effects to construction costs
int apply_event_construction_modifiers(int cost, GameState* game) {
    if (game == NULL) return cost;
    
    // Housing Subsidy: construction costs decrease by 30%
    if (game->national_event.is_active &&
        strcmp(game->national_event.event_name, "Housing Subsidy") == 0) {
        cost = (cost * 70) / 100;
    }
    
    // Currency Depreciation: construction costs increase by 10%
    if (game->national_event.is_active &&
        strcmp(game->national_event.event_name, "Currency Depreciation") == 0) {
        cost = (cost * 110) / 100;
    }
    
    return cost;
}

// Apply event effects to insurance premiums
int apply_event_insurance_modifiers(int premium, GameState* game) {
    if (game == NULL) return premium;
    
    // Insurance Discount: premiums decrease by 20%
    if (game->national_event.is_active &&
        strcmp(game->national_event.event_name, "Insurance Discount") == 0) {
        premium = (premium * 80) / 100;
    }
    
    return premium;
}

