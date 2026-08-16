#include "types.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


Square board_array[BOARD_SIZE];
Property property_array[MAX_PROPERTIES];

// BOARD INITIALIZATION

void init_board_data() {
    int prop_index = 0;
    
    // ========================================
    // SQUARE 0: GO
    // ========================================
    board_array[0].square_index = 0;
    strcpy(board_array[0].square_name, "GO");
    board_array[0].square_type = SQUARE_START;
    board_array[0].property_index = -1;
    
    // ========================================
    // SQUARE 1: Pettah (Brown Group)
    // ========================================
    board_array[1].square_index = 1;
    strcpy(board_array[1].square_name, "Pettah");
    board_array[1].square_type = SQUARE_PROPERTY;
    board_array[1].property_index = prop_index;
    
    property_array[prop_index].color_group = GROUP_BROWN;
    strcpy(property_array[prop_index].property_name, "Pettah");
    property_array[prop_index].purchase_price = 1500;
    property_array[prop_index].mortgage_value = 750;
    property_array[prop_index].base_rent = 100;        
    property_array[prop_index].house_construction_cost = 500;
    property_array[prop_index].hotel_construction_cost = 2000;
    property_array[prop_index].owner_id = -1;
    property_array[prop_index].is_mortgaged = 0;
    property_array[prop_index].is_loan_locked = 0;
    property_array[prop_index].building_count = 0;
    property_array[prop_index].insurance_policy = INSURANCE_NONE;
    property_array[prop_index].insurance_rounds_remaining = 0;
    property_array[prop_index].property_age = 0;
    property_array[prop_index].condition_percentage = 100;
    property_array[prop_index].rounds_since_maintenance = 0;
    property_array[prop_index].has_structural_damage = 0;
    property_array[prop_index].value_reduction = 0;
    prop_index++;
    
    // ========================================
    // SQUARE 2: Community Development Fund (Event)
    // ========================================
    board_array[2].square_index = 2;
    strcpy(board_array[2].square_name, "Community Development Fund");
    board_array[2].square_type = SQUARE_EVENT;
    board_array[2].property_index = -1;
    
    // ========================================
    // SQUARE 3: Maradana (Brown Group)
    // ========================================
    board_array[3].square_index = 3;
    strcpy(board_array[3].square_name, "Maradana");
    board_array[3].square_type = SQUARE_PROPERTY;
    board_array[3].property_index = prop_index;
    
    property_array[prop_index].color_group = GROUP_BROWN;
    strcpy(property_array[prop_index].property_name, "Maradana");
    property_array[prop_index].purchase_price = 1800;
    property_array[prop_index].mortgage_value = 750;
    property_array[prop_index].base_rent = 120;
    property_array[prop_index].house_construction_cost = 500;
    property_array[prop_index].hotel_construction_cost = 2000;
    property_array[prop_index].owner_id = -1;
    property_array[prop_index].is_mortgaged = 0;
    property_array[prop_index].is_loan_locked = 0;
    property_array[prop_index].building_count = 0;
    property_array[prop_index].insurance_policy = INSURANCE_NONE;
    property_array[prop_index].insurance_rounds_remaining = 0;
    property_array[prop_index].property_age = 0;
    property_array[prop_index].condition_percentage = 100;
    property_array[prop_index].rounds_since_maintenance = 0;
    property_array[prop_index].has_structural_damage = 0;
    property_array[prop_index].value_reduction = 0;
    prop_index++;
    
    // ========================================
    // SQUARE 4: Income Tax
    // ========================================
    board_array[4].square_index = 4;
    strcpy(board_array[4].square_name, "Income Tax");
    board_array[4].square_type = SQUARE_TAX;
    board_array[4].property_index = -1;
    
    // ========================================
    // SQUARE 5: Colombo Fort Railway Station
    // ========================================
    board_array[5].square_index = 5;
    strcpy(board_array[5].square_name, "Colombo Fort Railway Station");
    board_array[5].square_type = SQUARE_RAILWAY;
    board_array[5].property_index = prop_index;
    
    property_array[prop_index].color_group = GROUP_RAILWAY;
    strcpy(property_array[prop_index].property_name, "Colombo Fort Railway Station");
    property_array[prop_index].purchase_price = 1500;
    property_array[prop_index].mortgage_value = 750;
    property_array[prop_index].base_rent = 250;
    property_array[prop_index].house_construction_cost = 0;
    property_array[prop_index].hotel_construction_cost = 0;
    property_array[prop_index].owner_id = -1;
    property_array[prop_index].is_mortgaged = 0;
    property_array[prop_index].is_loan_locked = 0;
    property_array[prop_index].building_count = 0;
    property_array[prop_index].insurance_policy = INSURANCE_NONE;
    property_array[prop_index].insurance_rounds_remaining = 0;
    property_array[prop_index].property_age = 0;
    property_array[prop_index].condition_percentage = 100;
    property_array[prop_index].rounds_since_maintenance = 0;
    property_array[prop_index].has_structural_damage = 0;
    property_array[prop_index].value_reduction = 0;
    prop_index++;
    
    // ========================================
    // SQUARE 6: Bambalapitiya (Light Blue)
    // ========================================
    board_array[6].square_index = 6;
    strcpy(board_array[6].square_name, "Bambalapitiya");
    board_array[6].square_type = SQUARE_PROPERTY;
    board_array[6].property_index = prop_index;
    
    property_array[prop_index].color_group = GROUP_LIGHT_BLUE;
    strcpy(property_array[prop_index].property_name, "Bambalapitiya");
    property_array[prop_index].purchase_price = 2500;
    property_array[prop_index].mortgage_value = 1250;
    property_array[prop_index].base_rent = 180;
    property_array[prop_index].house_construction_cost = 750;
    property_array[prop_index].hotel_construction_cost = 3000;
    property_array[prop_index].owner_id = -1;
    property_array[prop_index].is_mortgaged = 0;
    property_array[prop_index].is_loan_locked = 0;
    property_array[prop_index].building_count = 0;
    property_array[prop_index].insurance_policy = INSURANCE_NONE;
    property_array[prop_index].insurance_rounds_remaining = 0;
    property_array[prop_index].property_age = 0;
    property_array[prop_index].condition_percentage = 100;
    property_array[prop_index].rounds_since_maintenance = 0;
    property_array[prop_index].has_structural_damage = 0;
    property_array[prop_index].value_reduction = 0;
    prop_index++;
    
    // ========================================
    // SQUARE 7: National Event Card
    // ========================================
    board_array[7].square_index = 7;
    strcpy(board_array[7].square_name, "National Event Card");
    board_array[7].square_type = SQUARE_EVENT;
    board_array[7].property_index = -1;
    
    // ========================================
    // SQUARE 8: Wellawatte (Light Blue)
    // ========================================
    board_array[8].square_index = 8;
    strcpy(board_array[8].square_name, "Wellawatte");
    board_array[8].square_type = SQUARE_PROPERTY;
    board_array[8].property_index = prop_index;
    
    property_array[prop_index].color_group = GROUP_LIGHT_BLUE;
    strcpy(property_array[prop_index].property_name, "Wellawatte");
    property_array[prop_index].purchase_price = 2700;
    property_array[prop_index].mortgage_value = 1250;
    property_array[prop_index].base_rent = 200;
    property_array[prop_index].house_construction_cost = 750;
    property_array[prop_index].hotel_construction_cost = 3000;
    property_array[prop_index].owner_id = -1;
    property_array[prop_index].is_mortgaged = 0;
    property_array[prop_index].is_loan_locked = 0;
    property_array[prop_index].building_count = 0;
    property_array[prop_index].insurance_policy = INSURANCE_NONE;
    property_array[prop_index].insurance_rounds_remaining = 0;
    property_array[prop_index].property_age = 0;
    property_array[prop_index].condition_percentage = 100;
    property_array[prop_index].rounds_since_maintenance = 0;
    property_array[prop_index].has_structural_damage = 0;
    property_array[prop_index].value_reduction = 0;
    prop_index++;
    
    // ========================================
    // SQUARE 9: Mount Lavinia (Light Blue)
    // ========================================
    board_array[9].square_index = 9;
    strcpy(board_array[9].square_name, "Mount Lavinia");
    board_array[9].square_type = SQUARE_PROPERTY;
    board_array[9].property_index = prop_index;
    
    property_array[prop_index].color_group = GROUP_LIGHT_BLUE;
    strcpy(property_array[prop_index].property_name, "Mount Lavinia");
    property_array[prop_index].purchase_price = 3000;
    property_array[prop_index].mortgage_value = 1250;
    property_array[prop_index].base_rent = 220;
    property_array[prop_index].house_construction_cost = 750;
    property_array[prop_index].hotel_construction_cost = 3000;
    property_array[prop_index].owner_id = -1;
    property_array[prop_index].is_mortgaged = 0;
    property_array[prop_index].is_loan_locked = 0;
    property_array[prop_index].building_count = 0;
    property_array[prop_index].insurance_policy = INSURANCE_NONE;
    property_array[prop_index].insurance_rounds_remaining = 0;
    property_array[prop_index].property_age = 0;
    property_array[prop_index].condition_percentage = 100;
    property_array[prop_index].rounds_since_maintenance = 0;
    property_array[prop_index].has_structural_damage = 0;
    property_array[prop_index].value_reduction = 0;
    prop_index++;
    
    // ========================================
    // SQUARE 10: Jail / Just Visiting
    // ========================================
    board_array[10].square_index = 10;
    strcpy(board_array[10].square_name, "Jail / Just Visiting");
    board_array[10].square_type = SQUARE_JAIL;
    board_array[10].property_index = -1;
    
    // ========================================
    // SQUARE 11: Nugegoda (Pink Group)
    // ========================================
    board_array[11].square_index = 11;
    strcpy(board_array[11].square_name, "Nugegoda");
    board_array[11].square_type = SQUARE_PROPERTY;
    board_array[11].property_index = prop_index;
    
    property_array[prop_index].color_group = GROUP_PINK;
    strcpy(property_array[prop_index].property_name, "Nugegoda");
    property_array[prop_index].purchase_price = 3500;
    property_array[prop_index].mortgage_value = 1750;
    property_array[prop_index].base_rent = 260;
    property_array[prop_index].house_construction_cost = 1000;
    property_array[prop_index].hotel_construction_cost = 4000;
    property_array[prop_index].owner_id = -1;
    property_array[prop_index].is_mortgaged = 0;
    property_array[prop_index].is_loan_locked = 0;
    property_array[prop_index].building_count = 0;
    property_array[prop_index].insurance_policy = INSURANCE_NONE;
    property_array[prop_index].insurance_rounds_remaining = 0;
    property_array[prop_index].property_age = 0;
    property_array[prop_index].condition_percentage = 100;
    property_array[prop_index].rounds_since_maintenance = 0;
    property_array[prop_index].has_structural_damage = 0;
    property_array[prop_index].value_reduction = 0;
    prop_index++;
    
    // ========================================
    // SQUARE 12: Ceylon Electricity Board (Utility)
    // ========================================
    board_array[12].square_index = 12;
    strcpy(board_array[12].square_name, "Ceylon Electricity Board");
    board_array[12].square_type = SQUARE_UTILITY;
    board_array[12].property_index = prop_index;
    
    property_array[prop_index].color_group = GROUP_UTILITY;
    strcpy(property_array[prop_index].property_name, "Ceylon Electricity Board");
    property_array[prop_index].purchase_price = 1500;
    property_array[prop_index].mortgage_value = 750;
    property_array[prop_index].base_rent = 0;           // Calculated from dice
    property_array[prop_index].house_construction_cost = 0;
    property_array[prop_index].hotel_construction_cost = 0;
    property_array[prop_index].owner_id = -1;
    property_array[prop_index].is_mortgaged = 0;
    property_array[prop_index].is_loan_locked = 0;
    property_array[prop_index].building_count = 0;
    property_array[prop_index].insurance_policy = INSURANCE_NONE;
    property_array[prop_index].insurance_rounds_remaining = 0;
    property_array[prop_index].property_age = 0;
    property_array[prop_index].condition_percentage = 100;
    property_array[prop_index].rounds_since_maintenance = 0;
    property_array[prop_index].has_structural_damage = 0;
    property_array[prop_index].value_reduction = 0;
    prop_index++;
    
    // ========================================
    // SQUARE 13: Maharagama (Pink Group)
    // ========================================
    board_array[13].square_index = 13;
    strcpy(board_array[13].square_name, "Maharagama");
    board_array[13].square_type = SQUARE_PROPERTY;
    board_array[13].property_index = prop_index;
    
    property_array[prop_index].color_group = GROUP_PINK;
    strcpy(property_array[prop_index].property_name, "Maharagama");
    property_array[prop_index].purchase_price = 3800;
    property_array[prop_index].mortgage_value = 1750;
    property_array[prop_index].base_rent = 280;
    property_array[prop_index].house_construction_cost = 1000;
    property_array[prop_index].hotel_construction_cost = 4000;
    property_array[prop_index].owner_id = -1;
    property_array[prop_index].is_mortgaged = 0;
    property_array[prop_index].is_loan_locked = 0;
    property_array[prop_index].building_count = 0;
    property_array[prop_index].insurance_policy = INSURANCE_NONE;
    property_array[prop_index].insurance_rounds_remaining = 0;
    property_array[prop_index].property_age = 0;
    property_array[prop_index].condition_percentage = 100;
    property_array[prop_index].rounds_since_maintenance = 0;
    property_array[prop_index].has_structural_damage = 0;
    property_array[prop_index].value_reduction = 0;
    prop_index++;
    
    // ========================================
    // SQUARE 14: Kottawa (Pink Group)
    // ========================================
    board_array[14].square_index = 14;
    strcpy(board_array[14].square_name, "Kottawa");
    board_array[14].square_type = SQUARE_PROPERTY;
    board_array[14].property_index = prop_index;
    
    property_array[prop_index].color_group = GROUP_PINK;
    strcpy(property_array[prop_index].property_name, "Kottawa");
    property_array[prop_index].purchase_price = 4000;
    property_array[prop_index].mortgage_value = 1750;
    property_array[prop_index].base_rent = 300;
    property_array[prop_index].house_construction_cost = 1000;
    property_array[prop_index].hotel_construction_cost = 4000;
    property_array[prop_index].owner_id = -1;
    property_array[prop_index].is_mortgaged = 0;
    property_array[prop_index].is_loan_locked = 0;
    property_array[prop_index].building_count = 0;
    property_array[prop_index].insurance_policy = INSURANCE_NONE;
    property_array[prop_index].insurance_rounds_remaining = 0;
    property_array[prop_index].property_age = 0;
    property_array[prop_index].condition_percentage = 100;
    property_array[prop_index].rounds_since_maintenance = 0;
    property_array[prop_index].has_structural_damage = 0;
    property_array[prop_index].value_reduction = 0;
    prop_index++;
    
    // ========================================
    // SQUARE 15: Kandy Railway Station
    // ========================================
    board_array[15].square_index = 15;
    strcpy(board_array[15].square_name, "Kandy Railway Station");
    board_array[15].square_type = SQUARE_RAILWAY;
    board_array[15].property_index = prop_index;
    
    property_array[prop_index].color_group = GROUP_RAILWAY;
    strcpy(property_array[prop_index].property_name, "Kandy Railway Station");
    property_array[prop_index].purchase_price = 1500;
    property_array[prop_index].mortgage_value = 750;
    property_array[prop_index].base_rent = 250;        // Will multiply based on owned
    property_array[prop_index].house_construction_cost = 0;
    property_array[prop_index].hotel_construction_cost = 0;
    property_array[prop_index].owner_id = -1;
    property_array[prop_index].is_mortgaged = 0;
    property_array[prop_index].is_loan_locked = 0;
    property_array[prop_index].building_count = 0;
    property_array[prop_index].insurance_policy = INSURANCE_NONE;
    property_array[prop_index].insurance_rounds_remaining = 0;
    property_array[prop_index].property_age = 0;
    property_array[prop_index].condition_percentage = 100;
    property_array[prop_index].rounds_since_maintenance = 0;
    property_array[prop_index].has_structural_damage = 0;
    property_array[prop_index].value_reduction = 0;
    prop_index++;
    
    // ========================================
    // SQUARE 16: Negombo (Orange Group)
    // ========================================
    board_array[16].square_index = 16;
    strcpy(board_array[16].square_name, "Negombo");
    board_array[16].square_type = SQUARE_PROPERTY;
    board_array[16].property_index = prop_index;
    
    property_array[prop_index].color_group = GROUP_ORANGE;
    strcpy(property_array[prop_index].property_name, "Negombo");
    property_array[prop_index].purchase_price = 4500;
    property_array[prop_index].mortgage_value = 2250;
    property_array[prop_index].base_rent = 350;
    property_array[prop_index].house_construction_cost = 1250;
    property_array[prop_index].hotel_construction_cost = 5000;
    property_array[prop_index].owner_id = -1;
    property_array[prop_index].is_mortgaged = 0;
    property_array[prop_index].is_loan_locked = 0;
    property_array[prop_index].building_count = 0;
    property_array[prop_index].insurance_policy = INSURANCE_NONE;
    property_array[prop_index].insurance_rounds_remaining = 0;
    property_array[prop_index].property_age = 0;
    property_array[prop_index].condition_percentage = 100;
    property_array[prop_index].rounds_since_maintenance = 0;
    property_array[prop_index].has_structural_damage = 0;
    property_array[prop_index].value_reduction = 0;
    prop_index++;
    
    // ========================================
    // SQUARE 17: Sri Lanka Insurance
    // ========================================
    board_array[17].square_index = 17;
    strcpy(board_array[17].square_name, "Sri Lanka Insurance");
    board_array[17].square_type = SQUARE_INSURANCE;
    board_array[17].property_index = -1;
    
    // ========================================
    // SQUARE 18: Katunayake (Orange Group)
    // ========================================
    board_array[18].square_index = 18;
    strcpy(board_array[18].square_name, "Katunayake");
    board_array[18].square_type = SQUARE_PROPERTY;
    board_array[18].property_index = prop_index;
    
    property_array[prop_index].color_group = GROUP_ORANGE;
    strcpy(property_array[prop_index].property_name, "Katunayake");
    property_array[prop_index].purchase_price = 4700;
    property_array[prop_index].mortgage_value = 2250;
    property_array[prop_index].base_rent = 370;
    property_array[prop_index].house_construction_cost = 1250;
    property_array[prop_index].hotel_construction_cost = 5000;
    property_array[prop_index].owner_id = -1;
    property_array[prop_index].is_mortgaged = 0;
    property_array[prop_index].is_loan_locked = 0;
    property_array[prop_index].building_count = 0;
    property_array[prop_index].insurance_policy = INSURANCE_NONE;
    property_array[prop_index].insurance_rounds_remaining = 0;
    property_array[prop_index].property_age = 0;
    property_array[prop_index].condition_percentage = 100;
    property_array[prop_index].rounds_since_maintenance = 0;
    property_array[prop_index].has_structural_damage = 0;
    property_array[prop_index].value_reduction = 0;
    prop_index++;
    
    // ========================================
    // SQUARE 19: Ja-Ela (Orange Group)
    // ========================================
    board_array[19].square_index = 19;
    strcpy(board_array[19].square_name, "Ja-Ela");
    board_array[19].square_type = SQUARE_PROPERTY;
    board_array[19].property_index = prop_index;
    
    property_array[prop_index].color_group = GROUP_ORANGE;
    strcpy(property_array[prop_index].property_name, "Ja-Ela");
    property_array[prop_index].purchase_price = 5000;
    property_array[prop_index].mortgage_value = 2250;
    property_array[prop_index].base_rent = 400;
    property_array[prop_index].house_construction_cost = 1250;
    property_array[prop_index].hotel_construction_cost = 5000;
    property_array[prop_index].owner_id = -1;
    property_array[prop_index].is_mortgaged = 0;
    property_array[prop_index].is_loan_locked = 0;
    property_array[prop_index].building_count = 0;
    property_array[prop_index].insurance_policy = INSURANCE_NONE;
    property_array[prop_index].insurance_rounds_remaining = 0;
    property_array[prop_index].property_age = 0;
    property_array[prop_index].condition_percentage = 100;
    property_array[prop_index].rounds_since_maintenance = 0;
    property_array[prop_index].has_structural_damage = 0;
    property_array[prop_index].value_reduction = 0;
    prop_index++;
    
    // ========================================
    // SQUARE 20: Free Parking
    // ========================================
    board_array[20].square_index = 20;
    strcpy(board_array[20].square_name, "Free Parking");
    board_array[20].square_type = SQUARE_FREE_PARKING;
    board_array[20].property_index = -1;
    
    // ========================================
    // SQUARE 21: Kandy City (Red Group)
    // ========================================
    board_array[21].square_index = 21;
    strcpy(board_array[21].square_name, "Kandy City");
    board_array[21].square_type = SQUARE_PROPERTY;
    board_array[21].property_index = prop_index;
    
    property_array[prop_index].color_group = GROUP_RED;
    strcpy(property_array[prop_index].property_name, "Kandy City");
    property_array[prop_index].purchase_price = 5500;
    property_array[prop_index].mortgage_value = 2750;
    property_array[prop_index].base_rent = 450;
    property_array[prop_index].house_construction_cost = 1500;
    property_array[prop_index].hotel_construction_cost = 6000;
    property_array[prop_index].owner_id = -1;
    property_array[prop_index].is_mortgaged = 0;
    property_array[prop_index].is_loan_locked = 0;
    property_array[prop_index].building_count = 0;
    property_array[prop_index].insurance_policy = INSURANCE_NONE;
    property_array[prop_index].insurance_rounds_remaining = 0;
    property_array[prop_index].property_age = 0;
    property_array[prop_index].condition_percentage = 100;
    property_array[prop_index].rounds_since_maintenance = 0;
    property_array[prop_index].has_structural_damage = 0;
    property_array[prop_index].value_reduction = 0;
    prop_index++;
    
    // ========================================
    // SQUARE 22: National Event Card
    // ========================================
    board_array[22].square_index = 22;
    strcpy(board_array[22].square_name, "National Event Card");
    board_array[22].square_type = SQUARE_EVENT;
    board_array[22].property_index = -1;
    
    // ========================================
    // SQUARE 23: Peradeniya (Red Group)
    // ========================================
    board_array[23].square_index = 23;
    strcpy(board_array[23].square_name, "Peradeniya");
    board_array[23].square_type = SQUARE_PROPERTY;
    board_array[23].property_index = prop_index;
    
    property_array[prop_index].color_group = GROUP_RED;
    strcpy(property_array[prop_index].property_name, "Peradeniya");
    property_array[prop_index].purchase_price = 5800;
    property_array[prop_index].mortgage_value = 2750;
    property_array[prop_index].base_rent = 480;
    property_array[prop_index].house_construction_cost = 1500;
    property_array[prop_index].hotel_construction_cost = 6000;
    property_array[prop_index].owner_id = -1;
    property_array[prop_index].is_mortgaged = 0;
    property_array[prop_index].is_loan_locked = 0;
    property_array[prop_index].building_count = 0;
    property_array[prop_index].insurance_policy = INSURANCE_NONE;
    property_array[prop_index].insurance_rounds_remaining = 0;
    property_array[prop_index].property_age = 0;
    property_array[prop_index].condition_percentage = 100;
    property_array[prop_index].rounds_since_maintenance = 0;
    property_array[prop_index].has_structural_damage = 0;
    property_array[prop_index].value_reduction = 0;
    prop_index++;
    
    // ========================================
    // SQUARE 24: Katugastota (Red Group)
    // ========================================
    board_array[24].square_index = 24;
    strcpy(board_array[24].square_name, "Katugastota");
    board_array[24].square_type = SQUARE_PROPERTY;
    board_array[24].property_index = prop_index;
    
    property_array[prop_index].color_group = GROUP_RED;
    strcpy(property_array[prop_index].property_name, "Katugastota");
    property_array[prop_index].purchase_price = 6000;
    property_array[prop_index].mortgage_value = 2750;
    property_array[prop_index].base_rent = 500;
    property_array[prop_index].house_construction_cost = 1500;
    property_array[prop_index].hotel_construction_cost = 6000;
    property_array[prop_index].owner_id = -1;
    property_array[prop_index].is_mortgaged = 0;
    property_array[prop_index].is_loan_locked = 0;
    property_array[prop_index].building_count = 0;
    property_array[prop_index].insurance_policy = INSURANCE_NONE;
    property_array[prop_index].insurance_rounds_remaining = 0;
    property_array[prop_index].property_age = 0;
    property_array[prop_index].condition_percentage = 100;
    property_array[prop_index].rounds_since_maintenance = 0;
    property_array[prop_index].has_structural_damage = 0;
    property_array[prop_index].value_reduction = 0;
    prop_index++;
    
    // ========================================
    // SQUARE 25: Galle Railway Station
    // ========================================
    board_array[25].square_index = 25;
    strcpy(board_array[25].square_name, "Galle Railway Station");
    board_array[25].square_type = SQUARE_RAILWAY;
    board_array[25].property_index = prop_index;
    
    property_array[prop_index].color_group = GROUP_RAILWAY;
    strcpy(property_array[prop_index].property_name, "Galle Railway Station");
    property_array[prop_index].purchase_price = 1500;
    property_array[prop_index].mortgage_value = 750;
    property_array[prop_index].base_rent = 250;
    property_array[prop_index].house_construction_cost = 0;
    property_array[prop_index].hotel_construction_cost = 0;
    property_array[prop_index].owner_id = -1;
    property_array[prop_index].is_mortgaged = 0;
    property_array[prop_index].is_loan_locked = 0;
    property_array[prop_index].building_count = 0;
    property_array[prop_index].insurance_policy = INSURANCE_NONE;
    property_array[prop_index].insurance_rounds_remaining = 0;
    property_array[prop_index].property_age = 0;
    property_array[prop_index].condition_percentage = 100;
    property_array[prop_index].rounds_since_maintenance = 0;
    property_array[prop_index].has_structural_damage = 0;
    property_array[prop_index].value_reduction = 0;
    prop_index++;
    
    // ========================================
    // SQUARE 26: Galle Fort (Yellow Group)
    // ========================================
    board_array[26].square_index = 26;
    strcpy(board_array[26].square_name, "Galle Fort");
    board_array[26].square_type = SQUARE_PROPERTY;
    board_array[26].property_index = prop_index;
    
    property_array[prop_index].color_group = GROUP_YELLOW;
    strcpy(property_array[prop_index].property_name, "Galle Fort");
    property_array[prop_index].purchase_price = 6500;
    property_array[prop_index].mortgage_value = 3250;
    property_array[prop_index].base_rent = 600;
    property_array[prop_index].house_construction_cost = 2000;
    property_array[prop_index].hotel_construction_cost = 8000;
    property_array[prop_index].owner_id = -1;
    property_array[prop_index].is_mortgaged = 0;
    property_array[prop_index].is_loan_locked = 0;
    property_array[prop_index].building_count = 0;
    property_array[prop_index].insurance_policy = INSURANCE_NONE;
    property_array[prop_index].insurance_rounds_remaining = 0;
    property_array[prop_index].property_age = 0;
    property_array[prop_index].condition_percentage = 100;
    property_array[prop_index].rounds_since_maintenance = 0;
    property_array[prop_index].has_structural_damage = 0;
    property_array[prop_index].value_reduction = 0;
    prop_index++;
    
    // ========================================
    // SQUARE 27: Unawatuna (Yellow Group)
    // ========================================
    board_array[27].square_index = 27;
    strcpy(board_array[27].square_name, "Unawatuna");
    board_array[27].square_type = SQUARE_PROPERTY;
    board_array[27].property_index = prop_index;
    
    property_array[prop_index].color_group = GROUP_YELLOW;
    strcpy(property_array[prop_index].property_name, "Unawatuna");
    property_array[prop_index].purchase_price = 6800;
    property_array[prop_index].mortgage_value = 3250;
    property_array[prop_index].base_rent = 620;
    property_array[prop_index].house_construction_cost = 2000;
    property_array[prop_index].hotel_construction_cost = 8000;
    property_array[prop_index].owner_id = -1;
    property_array[prop_index].is_mortgaged = 0;
    property_array[prop_index].is_loan_locked = 0;
    property_array[prop_index].building_count = 0;
    property_array[prop_index].insurance_policy = INSURANCE_NONE;
    property_array[prop_index].insurance_rounds_remaining = 0;
    property_array[prop_index].property_age = 0;
    property_array[prop_index].condition_percentage = 100;
    property_array[prop_index].rounds_since_maintenance = 0;
    property_array[prop_index].has_structural_damage = 0;
    property_array[prop_index].value_reduction = 0;
    prop_index++;
    
    // ========================================
    // SQUARE 28: National Water Supply (Utility)
    // ========================================
    board_array[28].square_index = 28;
    strcpy(board_array[28].square_name, "National Water Supply and Drainage Board");
    board_array[28].square_type = SQUARE_UTILITY;
    board_array[28].property_index = prop_index;
    
    property_array[prop_index].color_group = GROUP_UTILITY;
    strcpy(property_array[prop_index].property_name, "National Water Supply and Drainage Board");
    property_array[prop_index].purchase_price = 1500;
    property_array[prop_index].mortgage_value = 750;
    property_array[prop_index].base_rent = 0;           // Calculated from dice
    property_array[prop_index].house_construction_cost = 0;
    property_array[prop_index].hotel_construction_cost = 0;
    property_array[prop_index].owner_id = -1;
    property_array[prop_index].is_mortgaged = 0;
    property_array[prop_index].is_loan_locked = 0;
    property_array[prop_index].building_count = 0;
    property_array[prop_index].insurance_policy = INSURANCE_NONE;
    property_array[prop_index].insurance_rounds_remaining = 0;
    property_array[prop_index].property_age = 0;
    property_array[prop_index].condition_percentage = 100;
    property_array[prop_index].rounds_since_maintenance = 0;
    property_array[prop_index].has_structural_damage = 0;
    property_array[prop_index].value_reduction = 0;
    prop_index++;
    
    // ========================================
    // SQUARE 29: Hikkaduwa (Yellow Group)
    // ========================================
    board_array[29].square_index = 29;
    strcpy(board_array[29].square_name, "Hikkaduwa");
    board_array[29].square_type = SQUARE_PROPERTY;
    board_array[29].property_index = prop_index;
    
    property_array[prop_index].color_group = GROUP_YELLOW;
    strcpy(property_array[prop_index].property_name, "Hikkaduwa");
    property_array[prop_index].purchase_price = 7000;
    property_array[prop_index].mortgage_value = 3250;
    property_array[prop_index].base_rent = 650;
    property_array[prop_index].house_construction_cost = 2000;
    property_array[prop_index].hotel_construction_cost = 8000;
    property_array[prop_index].owner_id = -1;
    property_array[prop_index].is_mortgaged = 0;
    property_array[prop_index].is_loan_locked = 0;
    property_array[prop_index].building_count = 0;
    property_array[prop_index].insurance_policy = INSURANCE_NONE;
    property_array[prop_index].insurance_rounds_remaining = 0;
    property_array[prop_index].property_age = 0;
    property_array[prop_index].condition_percentage = 100;
    property_array[prop_index].rounds_since_maintenance = 0;
    property_array[prop_index].has_structural_damage = 0;
    property_array[prop_index].value_reduction = 0;
    prop_index++;
    
    // ========================================
    // SQUARE 30: Go To Jail
    // ========================================
    board_array[30].square_index = 30;
    strcpy(board_array[30].square_name, "Go To Jail");
    board_array[30].square_type = SQUARE_GO_TO_JAIL;
    board_array[30].property_index = -1;
    
    // ========================================
    // SQUARE 31: Jaffna Town (Green Group)
    // ========================================
    board_array[31].square_index = 31;
    strcpy(board_array[31].square_name, "Jaffna Town");
    board_array[31].square_type = SQUARE_PROPERTY;
    board_array[31].property_index = prop_index;
    
    property_array[prop_index].color_group = GROUP_GREEN;
    strcpy(property_array[prop_index].property_name, "Jaffna Town");
    property_array[prop_index].purchase_price = 8000;
    property_array[prop_index].mortgage_value = 4000;
    property_array[prop_index].base_rent = 750;
    property_array[prop_index].house_construction_cost = 2500;
    property_array[prop_index].hotel_construction_cost = 10000;
    property_array[prop_index].owner_id = -1;
    property_array[prop_index].is_mortgaged = 0;
    property_array[prop_index].is_loan_locked = 0;
    property_array[prop_index].building_count = 0;
    property_array[prop_index].insurance_policy = INSURANCE_NONE;
    property_array[prop_index].insurance_rounds_remaining = 0;
    property_array[prop_index].property_age = 0;
    property_array[prop_index].condition_percentage = 100;
    property_array[prop_index].rounds_since_maintenance = 0;
    property_array[prop_index].has_structural_damage = 0;
    property_array[prop_index].value_reduction = 0;
    prop_index++;
    
    // ========================================
    // SQUARE 32: Nallur (Green Group)
    // ========================================
    board_array[32].square_index = 32;
    strcpy(board_array[32].square_name, "Nallur");
    board_array[32].square_type = SQUARE_PROPERTY;
    board_array[32].property_index = prop_index;
    
    property_array[prop_index].color_group = GROUP_GREEN;
    strcpy(property_array[prop_index].property_name, "Nallur");
    property_array[prop_index].purchase_price = 8300;
    property_array[prop_index].mortgage_value = 4000;
    property_array[prop_index].base_rent = 780;
    property_array[prop_index].house_construction_cost = 2500;
    property_array[prop_index].hotel_construction_cost = 10000;
    property_array[prop_index].owner_id = -1;
    property_array[prop_index].is_mortgaged = 0;
    property_array[prop_index].is_loan_locked = 0;
    property_array[prop_index].building_count = 0;
    property_array[prop_index].insurance_policy = INSURANCE_NONE;
    property_array[prop_index].insurance_rounds_remaining = 0;
    property_array[prop_index].property_age = 0;
    property_array[prop_index].condition_percentage = 100;
    property_array[prop_index].rounds_since_maintenance = 0;
    property_array[prop_index].has_structural_damage = 0;
    property_array[prop_index].value_reduction = 0;
    prop_index++;
    
    // ========================================
    // SQUARE 33: Ceylinco Insurance
    // ========================================
    board_array[33].square_index = 33;
    strcpy(board_array[33].square_name, "Ceylinco Insurance");
    board_array[33].square_type = SQUARE_INSURANCE;
    board_array[33].property_index = -1;
    
    // ========================================
    // SQUARE 34: Trincomalee (Green Group)
    // ========================================
    board_array[34].square_index = 34;
    strcpy(board_array[34].square_name, "Trincomalee");
    board_array[34].square_type = SQUARE_PROPERTY;
    board_array[34].property_index = prop_index;
    
    property_array[prop_index].color_group = GROUP_GREEN;
    strcpy(property_array[prop_index].property_name, "Trincomalee");
    property_array[prop_index].purchase_price = 8500;
    property_array[prop_index].mortgage_value = 4000;
    property_array[prop_index].base_rent = 800;
    property_array[prop_index].house_construction_cost = 2500;
    property_array[prop_index].hotel_construction_cost = 10000;
    property_array[prop_index].owner_id = -1;
    property_array[prop_index].is_mortgaged = 0;
    property_array[prop_index].is_loan_locked = 0;
    property_array[prop_index].building_count = 0;
    property_array[prop_index].insurance_policy = INSURANCE_NONE;
    property_array[prop_index].insurance_rounds_remaining = 0;
    property_array[prop_index].property_age = 0;
    property_array[prop_index].condition_percentage = 100;
    property_array[prop_index].rounds_since_maintenance = 0;
    property_array[prop_index].has_structural_damage = 0;
    property_array[prop_index].value_reduction = 0;
    prop_index++;
    
    // ========================================
    // SQUARE 35: Jaffna Railway Station
    // ========================================
    board_array[35].square_index = 35;
    strcpy(board_array[35].square_name, "Jaffna Railway Station");
    board_array[35].square_type = SQUARE_RAILWAY;
    board_array[35].property_index = prop_index;
    
    property_array[prop_index].color_group = GROUP_RAILWAY;
    strcpy(property_array[prop_index].property_name, "Jaffna Railway Station");
    property_array[prop_index].purchase_price = 1500;
    property_array[prop_index].mortgage_value = 750;
    property_array[prop_index].base_rent = 250;
    property_array[prop_index].house_construction_cost = 0;
    property_array[prop_index].hotel_construction_cost = 0;
    property_array[prop_index].owner_id = -1;
    property_array[prop_index].is_mortgaged = 0;
    property_array[prop_index].is_loan_locked = 0;
    property_array[prop_index].building_count = 0;
    property_array[prop_index].insurance_policy = INSURANCE_NONE;
    property_array[prop_index].insurance_rounds_remaining = 0;
    property_array[prop_index].property_age = 0;
    property_array[prop_index].condition_percentage = 100;
    property_array[prop_index].rounds_since_maintenance = 0;
    property_array[prop_index].has_structural_damage = 0;
    property_array[prop_index].value_reduction = 0;
    prop_index++;
    
    // ========================================
    // SQUARE 36: National Event Card
    // ========================================
    board_array[36].square_index = 36;
    strcpy(board_array[36].square_name, "National Event Card");
    board_array[36].square_type = SQUARE_EVENT;
    board_array[36].property_index = -1;
    
    // ========================================
    // SQUARE 37: Nuwara Eliya (Dark Blue)
    // ========================================
    board_array[37].square_index = 37;
    strcpy(board_array[37].square_name, "Nuwara Eliya");
    board_array[37].square_type = SQUARE_PROPERTY;
    board_array[37].property_index = prop_index;
    
    property_array[prop_index].color_group = GROUP_DARK_BLUE;
    strcpy(property_array[prop_index].property_name, "Nuwara Eliya");
    property_array[prop_index].purchase_price = 10000;
    property_array[prop_index].mortgage_value = 5000;
    property_array[prop_index].base_rent = 1000;
    property_array[prop_index].house_construction_cost = 3000;
    property_array[prop_index].hotel_construction_cost = 12000;
    property_array[prop_index].owner_id = -1;
    property_array[prop_index].is_mortgaged = 0;
    property_array[prop_index].is_loan_locked = 0;
    property_array[prop_index].building_count = 0;
    property_array[prop_index].insurance_policy = INSURANCE_NONE;
    property_array[prop_index].insurance_rounds_remaining = 0;
    property_array[prop_index].property_age = 0;
    property_array[prop_index].condition_percentage = 100;
    property_array[prop_index].rounds_since_maintenance = 0;
    property_array[prop_index].has_structural_damage = 0;
    property_array[prop_index].value_reduction = 0;
    prop_index++;
    
    // ========================================
    // SQUARE 38: Bank of Ceylon
    // ========================================
    board_array[38].square_index = 38;
    strcpy(board_array[38].square_name, "Bank of Ceylon");
    board_array[38].square_type = SQUARE_BANK;
    board_array[38].property_index = -1;
    
    // ========================================
    // SQUARE 39: Galle Face (Dark Blue)
    // ========================================
    board_array[39].square_index = 39;
    strcpy(board_array[39].square_name, "Galle Face");
    board_array[39].square_type = SQUARE_PROPERTY;
    board_array[39].property_index = prop_index;
    
    property_array[prop_index].color_group = GROUP_DARK_BLUE;
    strcpy(property_array[prop_index].property_name, "Galle Face");
    property_array[prop_index].purchase_price = 12000;
    property_array[prop_index].mortgage_value = 5000;
    property_array[prop_index].base_rent = 1200;
    property_array[prop_index].house_construction_cost = 3000;
    property_array[prop_index].hotel_construction_cost = 12000;
    property_array[prop_index].owner_id = -1;
    property_array[prop_index].is_mortgaged = 0;
    property_array[prop_index].is_loan_locked = 0;
    property_array[prop_index].building_count = 0;
    property_array[prop_index].insurance_policy = INSURANCE_NONE;
    property_array[prop_index].insurance_rounds_remaining = 0;
    property_array[prop_index].property_age = 0;
    property_array[prop_index].condition_percentage = 100;
    property_array[prop_index].rounds_since_maintenance = 0;
    property_array[prop_index].has_structural_damage = 0;
    property_array[prop_index].value_reduction = 0;
    prop_index++;


    
    // Initialize temporary fields shared by event and disaster systems.
    for (int i = 0; i < prop_index; i++) {
        property_array[i].event_closed_rounds = 0;
        property_array[i].event_closed_started_round = -1;
        property_array[i].insurance_started_round = -1;
        property_array[i].has_disaster_damage = 0;
        property_array[i].pending_repair_cost = 0;
    }

    // Verify we have exactly 28 properties
    if (prop_index != MAX_PROPERTIES) {
        printf("WARNING: Expected %d properties, but initialized %d\n", 
               MAX_PROPERTIES, prop_index);
    }

    
    
}

// DICE ROLLING FUNCTION

DiceRoll roll_dice(void) {
    DiceRoll dice_roll;

    dice_roll.first_die = rand() % 6 + 1;
    dice_roll.second_die = rand() % 6 + 1;
    dice_roll.total = dice_roll.first_die + dice_roll.second_die;
    dice_roll.is_double = dice_roll.first_die == dice_roll.second_die;

    return dice_roll;
}

// PLAYER MOVEMENT FUNCTION

int move_player(Player* player, int dice_total) {
    int old_position = player->board_position;
    int new_position = (old_position + dice_total) % BOARD_SIZE;
    
    // Update player position
    player->board_position = new_position;
    
    // Check if passed GO (wrapped around)
    if (new_position < old_position) {
        player->cash += GO_BONUS;
        return 1;  // Passed GO
    }
    
    // Check if exactly landed on GO (position 0 from old position > 0)
    if (new_position == 0 && old_position > 0) {
        player->cash += GO_BONUS;
        return 1;  // Landed on GO
    }
    
    return 0;  // Did not pass GO
}



// HELPER FUNCTIONS

Square* get_square(int position) {
    if (position >= 0 && position < BOARD_SIZE) {
        return &board_array[position];
    }
    return NULL;
}

Property* get_property(int property_index) {
    if (property_index >= 0 && property_index < MAX_PROPERTIES) {
        return &property_array[property_index];
    }
    return NULL;
}

Property* get_property_at_position(int position) {
    Square* square = get_square(position);
    if (square == NULL) {
        return NULL;
    }
    
    if (square->property_index >= 0) {
        return get_property(square->property_index);
    }
    
    return NULL;  // Not a property square
}

int is_property_square(int position) {
    Square* square = get_square(position);
    if (square == NULL) {
        return 0;
    }
    return (square->property_index >= 0);
}

SquareType get_square_type(int position) {
    Square* square = get_square(position);
    if (square == NULL) {
        return SQUARE_START;  // Default
    }
    return square->square_type;
}

int count_properties_in_group(Player* player, PropertyGroup group) {
    int count = 0;
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        if (property_array[i].color_group == group && 
            property_array[i].owner_id == player->player_id) {
            count++;
        }
    }
    return count;
}
