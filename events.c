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

typedef struct {
    const char* name;
    const char* description;
    int effect_percentage;
} RegionalCard;

static const RegionalCard regional_deck[12] = {
    {"Southern Tourism Boom", "Galle Fort, Unawatuna and Hikkaduwa rent +40%.", 40},
    {"Port City Expansion", "Pettah, Maradana and Colombo Fort Station values +25%.", 25},
    {"IT Industry Growth", "Maharagama, Nugegoda and Kottawa values +20%.", 20},
    {"Northern Development Programme", "Jaffna Town, Nallur and Trincomalee values +30%.", 30},
    {"Tea Export Boom", "Nuwara Eliya value +35%.", 35},
    {"Airport Expansion", "Negombo, Katunayake and Ja-Ela rent +30%.", 30},
    {"University City Growth", "Peradeniya and Kandy City values +20%.", 20},
    {"Beach Pollution", "Southern coastal rent -30%.", -30},
    {"Flood Damage", "Low-lying coastal property values -20%.", -20},
    {"Transport Strike", "Railway revenue -40%.", -40},
    {"Electricity Tariff Increase", "Utility rent +25%.", 25},
    {"Water Shortage", "Water utility revenue +20%; surrounding values -10%.", 20}
};

static int regional_deck_index = 0;

// ============================================
// FORWARD DECLARATIONS
// ============================================

void init_event_deck();
static void execute_event_card(GameState* game, EventCard* card,
                               ActiveEvent* active_event);

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
    regional_deck_index = 0;
}

static void execute_event_card(GameState* game, EventCard* card,
                               ActiveEvent* active_event) {
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
            property_array[property_index].event_closed_started_round =
                game->round_number + 1;
            printf("  %s is closed for 2 rounds.\n",
                   property_array[property_index].property_name);
        }
    }
    else if (strcmp(card->name, "Interest Rate Cut") == 0) {
        printf("  This player's borrowing rate is reduced by 2%%.\n");
    }
    else if (strcmp(card->name, "Interest Rate Increase") == 0) {
        printf("  This player's borrowing rate is increased by 2%%.\n");
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

        active_event->affected_group = groups[rand() % 8];
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

    int player_index = game->current_player_index;
    ActiveEvent* player_event = &game->player_events[player_index];
    
    // Draw the current card
    EventCard card = national_deck[deck_index];
    
    // Advance deck index (wrap around)
    deck_index = (deck_index + 1) % 20;
    
    // Print the card
    printf("  🃏 EVENT CARD: %s\n", card.name);
    printf("     %s\n", card.description);
    
    // Store this card only for the player who drew it.
    player_event->is_active = 1;
    player_event->rounds_remaining = card.duration_rounds;
    player_event->effect_percentage = card.effect_percentage;
    player_event->affected_group = GROUP_NONE;
    player_event->started_round = game->round_number + 1;
    strcpy(player_event->event_name, card.name);

    printf("  Card belongs to: %s\n", game->players[player_index].player_name);
    execute_event_card(game, &card, player_event);

    if (card.duration_rounds == 0) {
        player_event->is_active = 0;
        player_event->started_round = -1;
    }
    
    if (card.duration_rounds > 0) {
        printf("  Effect duration: %d rounds.\n", card.duration_rounds);
    } else {
        printf("  This card has an immediate effect.\n");
    }
}

void process_national_event(GameState* game) {
    if (game == NULL) return;

    static const EventCard economic_events[] = {
        {"Tourism Boom", "Hotels earn double rent and southern coastal values rise by 15%.", 15, 15, 0, "All"},
        {"Fuel Crisis", "Railway rent doubles and development costs rise by 20%.", 20, 15, 0, "All"},
        {"Heavy Monsoon", "Insurance premiums rise and coastal values fall by 10%.", -10, 15, 0, "All"},
        {"Economic Recession", "Values fall 15%, rent falls 10%, and loan interest rises 15%.", -15, 15, 0, "All"},
        {"Stock Market Boom", "Property values rise 10% and loan interest falls 10%.", 10, 15, 0, "All"},
        {"Government Housing Programme", "House construction costs fall by 25%.", -25, 15, 0, "All"},
        {"Foreign Investment", "Commercial property values rise by 20%.", 20, 15, 0, "All"},
        {"Political Unrest", "Hotel rent falls by 50% and riot risk increases.", -50, 15, 0, "All"}
    };
    int event_count = (int)(sizeof(economic_events) / sizeof(economic_events[0]));
    
    printf("\n🏛️ NATIONAL EVENT TRIGGERED! (Every 15 Rounds)\n");
    
    // Select from the separate periodic economic-event list.
    EventCard card = economic_events[rand() % event_count];
    
    // Print the event
    printf("  📜 %s\n", card.name);
    printf("     %s\n", card.description);
    
    // Store in game state
    game->national_event.is_active = 1;
    game->national_event.rounds_remaining = 15;
    game->national_event.effect_percentage = card.effect_percentage;
    game->national_event.affected_group = GROUP_NONE;
    game->national_event.started_round = game->round_number;
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
    update_average_rent_for_inflation(inflation_rate);
    
    // Apply inflation to all properties
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        Property* prop = &property_array[i];
        
        // Update property purchase price
        if (inflation_rate != 0) {
            prop->purchase_price = (prop->purchase_price * (100 + inflation_rate)) / 100;
            prop->base_rent = (prop->base_rent * (100 + inflation_rate)) / 100;
            prop->house_construction_cost = (prop->house_construction_cost * (100 + inflation_rate)) / 100;
            prop->hotel_construction_cost = (prop->hotel_construction_cost * (100 + inflation_rate)) / 100;
        }
    }

    // Railway and utility rents use tables and dice instead of base_rent.
    game->special_rent_inflation_basis_points =
        (game->special_rent_inflation_basis_points *
         (100 + inflation_rate)) / 100;
    
    // Update loan interest rate
    if (inflation_rate != 0) {
        game->current_interest_rate_basis_points =
            (game->current_interest_rate_basis_points *
             (100 + inflation_rate)) / 100;
        game->current_interest_rate =
            (game->current_interest_rate_basis_points + 50) / 100;
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
    printf("  Insurance premiums and repair costs use the recalculated values.\n");
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
    game->government_regulation.started_round = game->round_number;
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
            printf("  📜 %s\n", regulation_name);
            printf("     Loan interest decreases by 2%%.\n");
            break;
        }
        case 2: { // Housing Subsidy
            game->government_regulation.effect_percentage = -30;  // Construction -30%
            printf("  📜 %s\n", regulation_name);
            printf("     House construction costs reduce by 30%%.\n");
            break;
        }
        case 3: { // Luxury Property Tax
            game->government_regulation.effect_percentage = 25;  // Hotels tax 25%
            printf("  📜 %s\n", regulation_name);
            printf("     Hotels incur annual maintenance tax of 25%%.\n");

            // No calendar year is defined, so levy the tax when selected.
            for (int i = 0; i < MAX_PROPERTIES; i++) {
                Property* prop = &property_array[i];
                if (prop->owner_id == -1 || prop->building_count != 5) continue;

                Player* owner = &game->players[prop->owner_id];
                int developed_value = get_property_value(prop) +
                                      prop->hotel_construction_cost;
                int tax = (developed_value * 25) / 100;

                if (owner->cash < tax) {
                    declare_bankruptcy(owner,
                        "unable to pay the Luxury Property Tax");
                } else {
                    owner->cash -= tax;
                    printf("     %s paid LKR %d for %s.\n",
                           owner->player_name, tax, prop->property_name);
                }
            }
            break;
        }
        case 4: { // Railway Modernization
            game->government_regulation.effect_percentage = 25;  // Railway rent +25%
            printf("  📜 %s\n", regulation_name);
            printf("     Railway rents increase 25%%.\n");
            break;
        }
        case 5: { // Electricity Tariff Revision
            game->government_regulation.effect_percentage = 20;  // Utility rent +20%
            printf("  📜 %s\n", regulation_name);
            printf("     Utility rents increase 20%%.\n");
            break;
        }
        case 6: { // Insurance Regulation
            game->government_regulation.effect_percentage = -15;  // Insurance -15%
            printf("  📜 %s\n", regulation_name);
            printf("     Insurance premiums decrease 15%%.\n");
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
    
    RegionalCard card = regional_deck[regional_deck_index];
    int card_index = regional_deck_index;
    const char* region = card.name;
    regional_deck_index = (regional_deck_index + 1) % 12;
    
    // Store in game state
    game->regional_development.is_active = 1;
    game->regional_development.rounds_remaining = 15;
    game->regional_development.started_round = game->round_number;
    game->regional_development.effect_percentage = card.effect_percentage;
    game->regional_development.card_index = card_index;
    strcpy(game->regional_development.event_name, card.name);
    strcpy(game->regional_development.region_name, card.name);
    
    printf("  📍 %s Development Programme\n", region);
    printf("  %s\n", card.description);
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
    
    int eligible_indices[8];
    int eligible_count = 0;

    // Rule-LK 33: any previously affected group waits at least 30 rounds.
    for (int i = 0; i < num_groups; i++) {
        int rounds_since_selected =
            game->round_number - game->market_group_last_selected_round[i];

        if (rounds_since_selected >= 30) {
            eligible_indices[eligible_count] = i;
            eligible_count++;
        }
    }

    if (eligible_count < 2) {
        printf("  Market review skipped: fewer than two groups are eligible.\n");
        return;
    }

    int boom_choice = rand() % eligible_count;
    int boom_index = eligible_indices[boom_choice];
    PropertyGroup boom_group = groups[boom_index];
    
    // Remove the boom group before selecting the decline group.
    eligible_indices[boom_choice] = eligible_indices[eligible_count - 1];
    eligible_count--;

    int decline_index = eligible_indices[rand() % eligible_count];
    PropertyGroup decline_group = groups[decline_index];

    game->market_group_last_selected_round[boom_index] = game->round_number;
    game->market_group_last_selected_round[decline_index] = game->round_number;
    
    // Store boom in game state
    game->market_boom.is_active = 1;
    game->market_boom.rounds_remaining = 10;
    game->market_boom.started_round = game->round_number;
    game->market_boom.effect_percentage = 20;  // +20% for boom
    game->market_boom.group = boom_group;
    strcpy(game->market_boom.group_name, group_names[boom_index]);
    
    // Store decline in game state
    game->market_decline.is_active = 1;
    game->market_decline.rounds_remaining = 10;
    game->market_decline.started_round = game->round_number;
    game->market_decline.effect_percentage = -15;  // -15% for decline
    game->market_decline.group = decline_group;
    strcpy(game->market_decline.group_name, group_names[decline_index]);
    
    // Do not rewrite base values. Active conditions are applied when needed.
    
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
    
    Player* owner = &game->players[prop->owner_id];
    int compensation = process_disaster_claim(
        prop, damage, disaster, game);

    // Compensation is credited before the automatic repair attempt.
    if (compensation > 0) {
        owner->cash += compensation;
        printf("     Insurance compensation credited: LKR %d\n", compensation);
    }

    prop->has_disaster_damage = 1;
    prop->pending_repair_cost = damage;

    if (owner->cash >= prop->pending_repair_cost) {
        owner->cash -= prop->pending_repair_cost;
        printf("     Automatic repair completed for LKR %d.\n",
               prop->pending_repair_cost);
        prop->has_disaster_damage = 0;
        prop->pending_repair_cost = 0;
        prop->condition_percentage = 100;
    } else {
        printf("     Owner cannot yet afford repairs. Building is closed.\n");
    }
}

void process_pending_disaster_repairs(GameState* game) {
    if (game == NULL) return;

    for (int i = 0; i < MAX_PROPERTIES; i++) {
        Property* prop = &property_array[i];
        if (!prop->has_disaster_damage || prop->owner_id == -1) continue;

        Player* owner = &game->players[prop->owner_id];
        if (owner->is_bankrupt || owner->cash < prop->pending_repair_cost) continue;

        owner->cash -= prop->pending_repair_cost;
        printf("  Automatic disaster repair: %s paid LKR %d for %s.\n",
               owner->player_name, prop->pending_repair_cost,
               prop->property_name);
        prop->has_disaster_damage = 0;
        prop->pending_repair_cost = 0;
        prop->condition_percentage = 100;
    }
}

void update_event_durations(GameState* game) {
    if (game == NULL) return;

    // Reduce temporary property closures created by Political Rally.
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        if (property_array[i].event_closed_rounds > 0 &&
            property_array[i].event_closed_started_round != game->round_number) {
            property_array[i].event_closed_rounds--;

            if (property_array[i].event_closed_rounds == 0) {
                property_array[i].event_closed_started_round = -1;
                printf("  %s has reopened after the Political Rally.\n",
                       property_array[i].property_name);
            }
        }
    }

    // Each player keeps and expires their own Event-square card effect.
    for (int i = 0; i < MAX_PLAYERS; i++) {
        ActiveEvent* player_event = &game->player_events[i];

        if (!player_event->is_active ||
            player_event->started_round == game->round_number) continue;

        player_event->rounds_remaining--;

        if (player_event->rounds_remaining == 3) {
            printf("  %s: Event card '%s' expires in 3 rounds.\n",
                   game->players[i].player_name, player_event->event_name);
        }

        if (player_event->rounds_remaining <= 0) {
            printf("  %s: Event card '%s' has ended.\n",
                   game->players[i].player_name, player_event->event_name);
            player_event->is_active = 0;
            player_event->effect_percentage = 0;
            player_event->affected_group = GROUP_NONE;
            player_event->started_round = -1;
            strcpy(player_event->event_name, "None");
        }
    }
    
    // ============================================
    // 1. NATIONAL EVENT
    // ============================================
    if (game->national_event.is_active &&
        game->national_event.started_round != game->round_number) {
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
            game->national_event.started_round = -1;
            strcpy(game->national_event.event_name, "None");
        }
    }
    
    // ============================================
    // 2. REGIONAL DEVELOPMENT
    // ============================================
    if (game->regional_development.is_active &&
        game->regional_development.started_round != game->round_number) {
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
            game->regional_development.card_index = -1;
            game->regional_development.started_round = -1;
            strcpy(game->regional_development.event_name, "None");
            strcpy(game->regional_development.region_name, "None");
        }
    }
    
    // ============================================
    // 3. GOVERNMENT REGULATION
    // ============================================
    if (game->government_regulation.is_active &&
        game->government_regulation.started_round != game->round_number) {
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
            game->government_regulation.started_round = -1;
            strcpy(game->government_regulation.regulation_name, "None");
        }
    }
    
    // ============================================
    // 4. MARKET BOOM
    // ============================================
    if (game->market_boom.is_active &&
        game->market_boom.started_round != game->round_number) {
        game->market_boom.rounds_remaining--;
        
        if (game->market_boom.rounds_remaining == 0) {
            printf("  📢 Market Boom for %s Group has ended.\n",
                   game->market_boom.group_name);
            game->market_boom.is_active = 0;
            game->market_boom.effect_percentage = 0;
            game->market_boom.started_round = -1;
            strcpy(game->market_boom.group_name, "None");
        }
    }
    
    // ============================================
    // 5. MARKET DECLINE
    // ============================================
    if (game->market_decline.is_active &&
        game->market_decline.started_round != game->round_number) {
        game->market_decline.rounds_remaining--;
        
        if (game->market_decline.rounds_remaining == 0) {
            printf("  📢 Market Decline for %s Group has ended.\n",
                   game->market_decline.group_name);
            game->market_decline.is_active = 0;
            game->market_decline.effect_percentage = 0;
            game->market_decline.started_round = -1;
            strcpy(game->market_decline.group_name, "None");
        }
    }
}

void print_current_market_conditions(GameState* game) {
    if (game == NULL) return;

    printf("\n=========================================\n");
    printf("Current Market Conditions\n");
    printf("=========================================\n");

    printf("Market Boom\n");
    printf("-------------\n");
    if (game->market_boom.is_active) {
        printf("%s Group (+20%%)\n", game->market_boom.group_name);
        printf("Rounds Remaining : %d\n",
               game->market_boom.rounds_remaining);
    } else {
        printf("None\n");
    }

    printf("Market Decline\n");
    printf("----------------\n");
    if (game->market_decline.is_active) {
        printf("%s Group (-15%%)\n", game->market_decline.group_name);
        printf("Rounds Remaining : %d\n",
               game->market_decline.rounds_remaining);
    } else {
        printf("None\n");
    }

    printf("Regional Development\n");
    printf("-----------------------\n");
    if (game->regional_development.is_active) {
        printf("%s\n", game->regional_development.event_name);
        printf("(%+d%%)\n", game->regional_development.effect_percentage);
        printf("Rounds Remaining : %d\n",
               game->regional_development.rounds_remaining);
    } else {
        printf("None\n");
    }

    printf("Inflation\n");
    printf("------------\n");
    printf("%+d%%\n", game->current_inflation_rate);

    printf("Current Loan Interest\n");
    printf("-----------------------\n");
    int general_interest = apply_event_interest_modifiers(
        game->current_interest_rate, game, -1);
    printf("%d%%\n", general_interest);
    printf("=========================================\n");
}

// ============================================
// APPLY EVENT EFFECTS
// ============================================

static ActiveEvent* get_player_event(GameState* game, int player_id) {
    if (game == NULL) return NULL;
    if (player_id < 0 || player_id >= MAX_PLAYERS) return NULL;
    if (!game->player_events[player_id].is_active) return NULL;

    return &game->player_events[player_id];
}

static int property_has_name(Property* prop, const char* name) {
    return prop != NULL && strcmp(prop->property_name, name) == 0;
}

static int is_southern_tourism_property(Property* prop) {
    return property_has_name(prop, "Galle Fort") ||
           property_has_name(prop, "Unawatuna") ||
           property_has_name(prop, "Hikkaduwa");
}

static int apply_regional_rent_modifier(Property* prop, GameState* game,
                                         int rent) {
    if (prop == NULL || game == NULL ||
        !game->regional_development.is_active) return rent;

    switch (game->regional_development.card_index) {
        case 0: // Southern Tourism Boom
            if (is_southern_tourism_property(prop)) rent = (rent * 140) / 100;
            break;
        case 5: // Airport Expansion
            if (property_has_name(prop, "Negombo") ||
                property_has_name(prop, "Katunayake") ||
                property_has_name(prop, "Ja-Ela")) {
                rent = (rent * 130) / 100;
            }
            break;
        case 7: // Beach Pollution
            if (is_southern_tourism_property(prop)) rent = (rent * 70) / 100;
            break;
        case 9: // Transport Strike
            if (prop->color_group == GROUP_RAILWAY) rent = (rent * 60) / 100;
            break;
        case 10: // Electricity Tariff Increase
            if (prop->color_group == GROUP_UTILITY) rent = (rent * 125) / 100;
            break;
        case 11: // Water Shortage
            if (property_has_name(prop,
                "National Water Supply and Drainage Board")) {
                rent = (rent * 120) / 100;
            }
            break;
        default:
            break;
    }

    return rent;
}

static int apply_regional_value_modifier(Property* prop, GameState* game,
                                          int value) {
    if (prop == NULL || game == NULL ||
        !game->regional_development.is_active) return value;

    int card = game->regional_development.card_index;

    if (card == 1 &&
        (property_has_name(prop, "Pettah") ||
         property_has_name(prop, "Maradana") ||
         property_has_name(prop, "Colombo Fort Railway Station"))) {
        value = (value * 125) / 100;
    } else if (card == 2 &&
        (property_has_name(prop, "Maharagama") ||
         property_has_name(prop, "Nugegoda") ||
         property_has_name(prop, "Kottawa"))) {
        value = (value * 120) / 100;
    } else if (card == 3 &&
        (property_has_name(prop, "Jaffna Town") ||
         property_has_name(prop, "Nallur") ||
         property_has_name(prop, "Trincomalee"))) {
        value = (value * 130) / 100;
    } else if (card == 4 && property_has_name(prop, "Nuwara Eliya")) {
        value = (value * 135) / 100;
    } else if (card == 6 &&
        (property_has_name(prop, "Peradeniya") ||
         property_has_name(prop, "Kandy City"))) {
        value = (value * 120) / 100;
    } else if (card == 8 &&
        (property_has_name(prop, "Negombo") ||
         property_has_name(prop, "Katunayake") ||
         property_has_name(prop, "Ja-Ela"))) {
        value = (value * 80) / 100;
    } else if (card == 11 &&
        (property_has_name(prop, "Trincomalee") ||
         property_has_name(prop, "Jaffna Railway Station"))) {
        value = (value * 90) / 100;
    }

    return value;
}

// Apply effects based on active event
int apply_event_rent_modifiers(Property* prop, GameState* game,
                               int player_id, int current_rent) {
    if (game == NULL || prop == NULL) return current_rent;

    ActiveEvent* national = &game->national_event;
    ActiveEvent* card = get_player_event(game, player_id);

    if (prop->color_group == GROUP_RAILWAY ||
        prop->color_group == GROUP_UTILITY) {
        current_rent =
            (current_rent * game->special_rent_inflation_basis_points) / 10000;
    }

    if (national->is_active &&
        strcmp(national->event_name, "Tourism Boom") == 0 &&
        prop->building_count == 5) {
        current_rent *= 2;
    }

    if (national->is_active &&
        strcmp(national->event_name, "Fuel Crisis") == 0 &&
        prop->color_group == GROUP_RAILWAY) {
        current_rent *= 2;
    }

    if (national->is_active &&
        strcmp(national->event_name, "Economic Recession") == 0) {
        current_rent = (current_rent * 90) / 100;
    }

    if (national->is_active &&
        strcmp(national->event_name, "Political Unrest") == 0 &&
        prop->building_count == 5) {
        current_rent = (current_rent * 50) / 100;
    }

    if (card != NULL && strcmp(card->event_name, "Tourism Hype") == 0 &&
        prop->building_count == 5) {
        current_rent *= 2;
    }

    if (card != NULL && strcmp(card->event_name, "Fuel Shortage") == 0 &&
        prop->color_group == GROUP_RAILWAY) {
        current_rent *= 2;
    }

    if (card != NULL && strcmp(card->event_name, "Power Failure") == 0 &&
        prop->color_group == GROUP_UTILITY) {
        current_rent = (current_rent * 50) / 100;
    }

    if (card != NULL && strcmp(card->event_name, "Festival Season") == 0 &&
        prop->building_count == 5) {
        current_rent = (current_rent * 150) / 100;
    }

    if (game->government_regulation.is_active &&
        strcmp(game->government_regulation.regulation_name,
               "Railway Modernization") == 0 &&
        prop->color_group == GROUP_RAILWAY) {
        current_rent = (current_rent * 125) / 100;
    }

    if (game->government_regulation.is_active &&
        strcmp(game->government_regulation.regulation_name,
               "Electricity Tariff Revision") == 0 &&
        prop->color_group == GROUP_UTILITY) {
        current_rent = (current_rent * 120) / 100;
    }

    if (game->market_boom.is_active &&
        prop->color_group == game->market_boom.group) {
        current_rent = (current_rent * 125) / 100;
    }

    if (game->market_decline.is_active &&
        prop->color_group == game->market_decline.group) {
        current_rent = (current_rent * 80) / 100;
    }
    
    return apply_regional_rent_modifier(prop, game, current_rent);
}

// Apply event effects to property value
int apply_event_value_modifiers(Property* prop, GameState* game,
                                int player_id, int current_value) {
    if (game == NULL || prop == NULL) return current_value;

    ActiveEvent* national = &game->national_event;
    ActiveEvent* card = get_player_event(game, player_id);

    if (national->is_active &&
        strcmp(national->event_name, "Tourism Boom") == 0 &&
        is_coastal_property(prop)) {
        current_value = (current_value * 115) / 100;
    }

    if (national->is_active &&
        strcmp(national->event_name, "Heavy Monsoon") == 0 &&
        is_coastal_property(prop)) {
        current_value = (current_value * 90) / 100;
    }

    if (national->is_active &&
        strcmp(national->event_name, "Economic Recession") == 0) {
        current_value = (current_value * 85) / 100;
    }

    if (national->is_active &&
        strcmp(national->event_name, "Stock Market Boom") == 0) {
        current_value = (current_value * 110) / 100;
    }

    if (national->is_active &&
        strcmp(national->event_name, "Foreign Investment") == 0 &&
        is_standard_property(prop)) {
        current_value = (current_value * 120) / 100;
    }

    if (card != NULL && strcmp(card->event_name, "Economic Downturn") == 0) {
        current_value = (current_value * 85) / 100;
    }

    if (card != NULL && strcmp(card->event_name, "Stock Market Rise") == 0) {
        current_value = (current_value * 110) / 100;
    }

    if (card != NULL && strcmp(card->event_name, "Foreign Funding") == 0 &&
        is_standard_property(prop)) {
        current_value = (current_value * 115) / 100;
    }

    if (card != NULL && strcmp(card->event_name, "Port Expansion") == 0 &&
        prop->color_group == GROUP_RAILWAY) {
        current_value = (current_value * 120) / 100;
    }

    if (card != NULL && strcmp(card->event_name, "Property Revaluation") == 0 &&
        prop->color_group == card->affected_group) {
        current_value = (current_value * 115) / 100;
    }
    
    return apply_regional_value_modifier(prop, game, current_value);
}

// Apply event effects to construction costs
int apply_event_construction_modifiers(Property* prop, int cost,
                                       GameState* game, int player_id) {
    if (game == NULL) return cost;

    ActiveEvent* national = &game->national_event;
    ActiveEvent* card = get_player_event(game, player_id);

    if (national->is_active && strcmp(national->event_name, "Fuel Crisis") == 0) {
        cost = (cost * 120) / 100;
    }

    if (national->is_active &&
        strcmp(national->event_name, "Government Housing Programme") == 0) {
        cost = (cost * 75) / 100;
    }

    if (card != NULL && strcmp(card->event_name, "Housing Subsidy") == 0) {
        cost = (cost * 70) / 100;
    }

    if (card != NULL && strcmp(card->event_name, "Currency Depreciation") == 0) {
        cost = (cost * 110) / 100;
    }

    if (prop != NULL && prop->building_count < 4 &&
        game->government_regulation.is_active &&
        strcmp(game->government_regulation.regulation_name,
               "Housing Subsidy") == 0) {
        cost = (cost * 70) / 100;
    }

    if (prop != NULL && game->market_boom.is_active &&
        prop->color_group == game->market_boom.group) {
        cost = (cost * 110) / 100;
    }
    
    return cost;
}

// Apply event effects to insurance premiums
int apply_event_insurance_modifiers(int premium, GameState* game, int player_id) {
    if (game == NULL) return premium;

    ActiveEvent* national = &game->national_event;
    ActiveEvent* card = get_player_event(game, player_id);

    // Rule-LK 18 gives no exact Heavy Monsoon percentage, so use 10%.
    if (national->is_active && strcmp(national->event_name, "Heavy Monsoon") == 0) {
        premium = (premium * 110) / 100;
    }

    if (card != NULL && strcmp(card->event_name, "Insurance Discount") == 0) {
        premium = (premium * 80) / 100;
    }

    if (game->government_regulation.is_active &&
        strcmp(game->government_regulation.regulation_name,
               "Insurance Regulation") == 0) {
        premium = (premium * 85) / 100;
    }
    
    return premium;
}

int apply_event_interest_modifiers(int rate, GameState* game, int player_id) {
    if (game == NULL) return rate;

    ActiveEvent* national = &game->national_event;
    ActiveEvent* card = get_player_event(game, player_id);

    if (national->is_active &&
        strcmp(national->event_name, "Economic Recession") == 0) {
        rate = (rate * 115) / 100;
    }

    if (national->is_active &&
        strcmp(national->event_name, "Stock Market Boom") == 0) {
        rate = (rate * 90) / 100;
    }

    if (card != NULL && strcmp(card->event_name, "Interest Rate Cut") == 0) {
        rate -= 2;
    }

    if (card != NULL && strcmp(card->event_name, "Interest Rate Increase") == 0) {
        rate += 2;
    }

    if (game->government_regulation.is_active &&
        strcmp(game->government_regulation.regulation_name,
               "Reduce Loan Interest") == 0) {
        rate -= 2;
    }

    if (rate < 0) rate = 0;
    return rate;
}

int is_event_construction_suspended(GameState* game, int player_id) {
    ActiveEvent* card = get_player_event(game, player_id);

    return card != NULL && strcmp(card->event_name, "Labour Strike") == 0;
}

int can_purchase_under_regulation(Player* player, GameState* game) {
    if (player == NULL || game == NULL) return 0;

    if (!game->government_regulation.is_active ||
        strcmp(game->government_regulation.regulation_name,
               "Anti-Speculation Act") != 0) {
        return 1;
    }

    int undeveloped_count = 0;
    for (int i = 0; i < player->owned_property_count; i++) {
        int property_index = player->owned_property_indices[i];
        if (property_index < 0 || property_index >= MAX_PROPERTIES) continue;

        Property* prop = &property_array[property_index];
        if (is_standard_property(prop) && prop->building_count == 0) {
            undeveloped_count++;
        }
    }

    return undeveloped_count < 3;
}

int apply_government_tax_modifier(int tax, GameState* game) {
    if (game != NULL && game->government_regulation.is_active &&
        strcmp(game->government_regulation.regulation_name,
               "Increase Property Tax") == 0) {
        tax = (tax * 150) / 100;
    }

    return tax;
}

int apply_market_purchase_modifier(Property* prop, GameState* game, int value) {
    if (prop == NULL || game == NULL) return value;

    if (game->market_boom.is_active &&
        prop->color_group == game->market_boom.group) {
        value = (value * 115) / 100;
    }

    if (game->market_decline.is_active &&
        prop->color_group == game->market_decline.group) {
        value = (value * 85) / 100;
    }

    return value;
}

int apply_market_value_modifier(Property* prop, GameState* game, int value) {
    if (prop == NULL || game == NULL) return value;

    if (game->market_boom.is_active &&
        prop->color_group == game->market_boom.group) {
        value = (value * 120) / 100;
    }

    if (game->market_decline.is_active &&
        prop->color_group == game->market_decline.group) {
        value = (value * 85) / 100;
    }

    return value;
}

int apply_market_mortgage_modifier(Property* prop, GameState* game, int value) {
    if (prop == NULL || game == NULL) return value;

    if (game->market_boom.is_active &&
        prop->color_group == game->market_boom.group) {
        value = (value * 115) / 100;
    }

    if (game->market_decline.is_active &&
        prop->color_group == game->market_decline.group) {
        value = (value * 90) / 100;
    }

    return value;
}

int apply_market_auction_modifier(Property* prop, GameState* game, int value) {
    if (prop == NULL || game == NULL) return value;

    if (game->market_decline.is_active &&
        prop->color_group == game->market_decline.group) {
        value = (value * 75) / 100;
    }

    return value;
}

