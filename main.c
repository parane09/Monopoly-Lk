#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void init_game(GameState* game);
void run_game(GameState* game);
void print_game_start(GameState* game);


int main() {

    srand((unsigned int)time(NULL));
    
    // Print the simulation header
    printf("\n========================================\n");
    printf("   MONOPOLY-LK SIMULATION\n");
    printf("========================================\n\n");
    
    // Create the game state variable
    GameState game;

    // Sets up players, board, properties, and game state
    init_game(&game);
    
    // Print the starting information
    print_game_start(&game);
    
    // Run the main game simulation
    run_game(&game);
    
    // Print completion message
    printf("\n========================================\n");
    printf("   SIMULATION COMPLETE\n");
    printf("========================================\n");
    
    return 0;
}