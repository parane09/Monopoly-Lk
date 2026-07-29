#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// ============================================
// FUNCTION PROTOTYPES
// (Functions defined in other files)
// ============================================

void init_game(GameState* game);
void run_game(GameState* game);
void print_game_start(GameState* game);

// ============================================
// MAIN - PROGRAM ENTRY POINT
// ============================================

int main() {
    // Seed the random number generator with current time
    // This ensures different random numbers each run
    srand((unsigned int)time(NULL));
    
    // Print the simulation header
    printf("\n========================================\n");
    printf("   MONOPOLY-LK SIMULATION\n");
    printf("========================================\n\n");
    
    // Create the game state variable
    // This holds ALL game data (players, board, events, etc.)
    GameState game;
    
    // Initialize the game
    // Sets up players, board, properties, and game state
    init_game(&game);
    
    // Print the starting information
    // Shows player names, strategies, and starting cash
    print_game_start(&game);
    
    // Run the main game simulation
    // This contains the main loop that runs for up to 500 rounds
    run_game(&game);
    
    // Print completion message
    printf("\n========================================\n");
    printf("   SIMULATION COMPLETE\n");
    printf("========================================\n");
    
    return 0;  // Program finished successfully
}