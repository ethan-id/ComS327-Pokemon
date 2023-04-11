#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <ncurses.h>
#include <iostream>
#include "parse.h"
extern "C" {
    #include "heap.h"
}
using namespace std;

#define GRASS_COLOR 123
#define WATER_COLOR 234
#define BOULDER_COLOR 456
#define ROAD_COLOR 678
#define TREE_COLOR 890

const int INFINITY_T = 2147483640;
const int NUM_VERTICES = 1680; // 21 * 80

class position {
    public:
        int rowPos;
        int colPos;
};

typedef enum {
    None = 0, Up = 1, Down = 2, Left = 3, Right = 4
} direction_t;

class character: public position {
    public: 
        char npc;
        char value[20];
        direction_t direction;
        int defeated;
        long int nextMoveTime;
        char spawn;
};

class squares {
    public:
        int rowPos;
        int colPos;
        int cost;
        char terrain;
};

class player: public position {
    public:
        int preset;
};

class terrainMap {
    public:
        int quit;
        int wantToFly;
        int flyRow;
        int flyCol;
        int generated;
        player pc;
        int northSouthExit;
        int westEastExit;
        int worldRow;
        int worldCol;
        char terrain[21][80];
        position roadPositions[101]; // 80 + 21
        character *trainers[];
};

terrainMap *world[401][401];
direction_t lastMove;

int decorateTerrain(char map[21][80]) {
    int i, j, k;
    char decorations[2] = {'^', '%'};

    for (k = 0; k < (rand() % (20 - 10)) + 10; k++) {
        j = (rand() % (79 - 1)) + 1;
        i = (rand() % (20 - 1)) + 1;

        if (map[i][j] != '#' && map[i][j] != 'M' && map[i][j] != 'C' && map[i][j] != '~') {
            map[i][j] = decorations[rand() % 2];
        }
    }

    return 0;
}

int generateBuildings(terrainMap *terrainMap, int row, int col) {
    int pC = (rand() % (70 - 10)) + 10;
    int pM = (rand() % (16 - 3)) + 3;

    int i, j;

    while (terrainMap->terrain[terrainMap->westEastExit - 1][pC - 1] == '#' || terrainMap->terrain[terrainMap->westEastExit - 1][pC] == '#') {
        pC = (rand() % (70 - 10)) + 10;
    }

    while (terrainMap->terrain[pM - 1][terrainMap->northSouthExit - 1] == '#' || terrainMap->terrain[pM][terrainMap->northSouthExit - 1] == '#') {
        pM = (rand() % (16 - 3)) + 3;
    }

    for (i = 1; i < 3; i++) {
        for (j = 0; j < 2; j++) {
            terrainMap->terrain[terrainMap->westEastExit - i][pC - j] = 'C';
        }
    }

    for (i = 0; i < 2; i++) {
        for (j = 1; j < 3; j++) {
            terrainMap->terrain[pM - i][terrainMap->northSouthExit - j] = 'M';
        }
    }

    return 0;  
}

int generatePaths(terrainMap *terrainMap, int currWorldRow, int currWorldCol) {
    // Will need to update to check for existence of exits
    int i, j, k = 0;
    int rowStart = 0, colStart = 0;
    int rowEnd = 80, colEnd = 21;

    if (currWorldRow == 0) { // If at the bottom of the world
        colEnd = 20;
    }
    if (currWorldRow == 401) { // If at the top of the world
        colStart = 1;
    }
    if (currWorldCol == 0) {
        rowStart = 1;
    }
    if (currWorldCol == 401) {
        rowEnd = 79;
    }

    for (i = rowStart; i < rowEnd; i++) {
        terrainMap->terrain[terrainMap->westEastExit][i] = '#';
        terrainMap->roadPositions[k].colPos = i;
        terrainMap->roadPositions[k].rowPos = terrainMap->westEastExit;
        k++;
    }

    for (j = colStart; j < colEnd; j++) {
        terrainMap->terrain[j][terrainMap->northSouthExit] = '#';
        terrainMap->roadPositions[k].colPos = terrainMap->northSouthExit;
        terrainMap->roadPositions[k].rowPos = j;
        k++;
    }

    return 0;  
}

int checkSurroundingsForChar(int x, int y, char map[21][80], char checkChar) {
    int i, j;
    
    for (i = -7; i < 8; i++) {
        for (j = -7; j < 8; j++) {
            if (map[y+i][x+j] == checkChar || map[y+i][x+j] == '%' || map[y+i][x+j] == '#') {
                return 1;
            }
        }
    }

    return 0;
}

void generateWater(char map[21][80]) {  
    int x = rand() % 79 + 1;
    int y = rand() % 20 + 1;

    // Ensure that the random point to be used as the center of a monument is at least 5 squares away from any tall grass and
    // the borders of the map
    while(checkSurroundingsForChar(x, y, map, ':') == 1) {
        x = rand() % 79 + 1;
        y = rand() % 20 + 1;
    }

    int i, j;

    int height = (rand() % 7) + 3;
    int width = (rand() % 13) + 5;

    for (i = -height; i < height; i++) {
        for (j = -width; j < width; j++) {
            if (y+i >= 0 && y+i <= 20 && x+j >= 0 && x+j <= 79) {
                if (map[y + i][x + j] != '%' && map[y + i][x + j] != '#') {
                    map[y + i][x + j] = '~';
                }
            }
        }
    }
}

void generateTallGrass(char map[21][80]) {
    int i, j;
    int spotX = (rand() % 29) + 10;
    int spotY = (rand() % 10) + 5;

    while(checkSurroundingsForChar(spotX, spotY, map, '%') == 1) {
        spotX = (rand() % 29) + 10;
        spotY = (rand() % 10) + 5;
    }

    int height = (rand() % (5 - 3)) + 3;
    int width = (rand() % (12 - 7)) + 7;

    for (i = -height; i < height; i++) {
        for (j = -width; j < width; j++) {
            if (spotY+i >= 0 && spotY+i <= 20 && spotX+j >= 0 && spotX+j <= 79) {
                if (map[spotY + i][spotX + j] != '%' && map[spotY + i][spotX + j] != '#') {
                    map[spotY + i][spotX + j] = ':';
                }
            }
        }
    }

    spotX += ((rand() % 20) + 20);
    spotY += ((rand() % 5) - 7);
    
    height = (rand() % 4) + 3;
    width = (rand() % 8) + 4;

    for (i = -height; i < height; i++) {
        for (j = -width; j < width; j++) {
            if (spotY+i >= 0 && spotY+i <= 20 && spotX+j >= 0 && spotX+j <= 79) {
                if (map[spotY + i][spotX + j] != '%' && map[spotY + i][spotX + j] != '#') {
                    map[spotY + i][spotX + j] = ':';
                }
            }
        }
    }
}

void generateExits(terrainMap *terrainMap, int row, int col) {
    int northSouthExit = (rand() % (69 - 10)) + 10;
    int westEastExit = (rand() % (16 - 3)) + 3;

    terrainMap->northSouthExit = northSouthExit;
    terrainMap->westEastExit = westEastExit;

    if ((row - 1) >= 0 && (row + 1) < 401 && (col - 1) >= 0 && (col + 1) < 401) {
        if (world[row - 1][col]->northSouthExit) {
            terrainMap->northSouthExit = world[row - 1][col]->northSouthExit;
        }
        if (world[row + 1][col]->northSouthExit) {
            terrainMap->northSouthExit = world[row + 1][col]->northSouthExit;    
        }
        if (world[row][col - 1]->westEastExit) {
            terrainMap->westEastExit = world[row][col - 1]->westEastExit;
        }
        if (world[row][col + 1]->westEastExit) {
            terrainMap->westEastExit = world[row][col + 1]->westEastExit;
        }
    }
}

void placeCharacter(terrainMap *terrainMap) {
    // Pick a random road
    int selectedRoad = (rand() % (101 - 0)) + 0;
    int selected = 0;
    // 'Place character' 
    while(!selected) {
        if (terrainMap->roadPositions[selectedRoad].rowPos == 0 
        || terrainMap->roadPositions[selectedRoad].rowPos == 20 
        || terrainMap->roadPositions[selectedRoad].colPos == 0 
        || terrainMap->roadPositions[selectedRoad].colPos == 79) {
            selectedRoad = (rand() % (101 - 0)) + 0;
        } else {
            selected = 1;
        }
    }
    terrainMap->pc.rowPos = terrainMap->roadPositions[selectedRoad].rowPos;
    terrainMap->pc.colPos = terrainMap->roadPositions[selectedRoad].colPos;
}

void populateHikerCosts(char terrain[21][80], squares squares[21][80]) {
    int i, j;

    for (i = 0; i < 21; i++) {
        for (j = 0; j < 80; j++) {
            if(terrain[i][j] == '^' // Tree
            || terrain[i][j] == '~' // Water
            || (terrain[i][j] == '#' && ((i == 0 || i == 20) || (j == 0 || j == 79)))) { // Gate
                squares[i][j].cost = INFINITY_T;
            }
            if(terrain[i][j] == '%' || terrain[i][j] == '^') { // Boulder
                if(i != 0 && i != 20 && j != 0 && j != 79) {
                    squares[i][j].cost = 15;
                } else {
                    squares[i][j].cost = INFINITY_T;
                }
            }
            if(terrain[i][j] == '#' && i != 0 && i != 20 && j != 0 && j != 79) { // Road, not gate
                squares[i][j].cost = 10;
            }
            if(terrain[i][j] == 'M' || terrain[i][j] == 'C') {
                squares[i][j].cost = 50;
            }
            if(terrain[i][j] == ':') {
                squares[i][j].cost = 15;
            }
            if(terrain[i][j] == '.') {
                squares[i][j].cost = 10;
            }
        }
    }
}

void populateRivalCosts(char terrain[21][80], squares squares[21][80]) {
    int i, j;

    for (i = 0; i < 21; i++) {
        for (j = 0; j < 80; j++) {
            if(terrain[i][j] == '%' // Boulder
            || terrain[i][j] == '^' // Tree
            || terrain[i][j] == '~' // Water
            || (terrain[i][j] == '#' && ((i == 0 || i == 20) || (j == 0 || j == 79)))) { // Gate
                squares[i][j].cost = INFINITY_T;
            }
            if(terrain[i][j] == '#' && i != 0 && i != 20 && j != 0 && j != 79) { // Road, not gate
                squares[i][j].cost = 10;
            }
            if(terrain[i][j] == 'M' || terrain[i][j] == 'C') {
                squares[i][j].cost = 50;
            }
            if(terrain[i][j] == ':') {
                squares[i][j].cost = 20;
            }
            if(terrain[i][j] == '.') {
                squares[i][j].cost = 10;
            }
            
        }
    }
}

void dijkstra(char map[21][80], squares squares[21][80], player source) {
    static int altCount = 0;
    int i, j;
    int dist[21][80];
    position positions[21][80];
    position *prev[21][80];
    heap h;
    
    // Using costs of positions as keys, NULL compare function will treat keys as signed integers
    heap_create(&h, 21, NULL);

    // For each 'vertex'
    for(i = 0; i < 21; i++) {
        for(j = 0; j < 80; j++) {
            dist[i][j] = INFINITY_T; // set distance from source to INFINITY_T
            prev[i][j] = (position*) malloc(sizeof(position));
            positions[i][j].rowPos = i;
            positions[i][j].colPos = j;
            if(i == source.rowPos && j == source.colPos) { // set source dist[] to 0
                dist[i][j] = 0;
            }
            heap_insert(&h, &dist[i][j], &positions[i][j]); // add to queue
        }
    }

    // dist[source.rowPos][source.colPos] = 0;

    heap_entry u;
    // While the queue is not empty
    while(heap_size(&h) > 0) {
        // Remove 'vertex' u from the queue
        heap_delmin(&h, &u.key, &u.value);

        // Get the position of the 'vertex' off of the element stored in the queue
        //  AKA its row and column indices on the map.
        position *value = static_cast<position*>(u.value);

        // For each neighbor, v or [i][j], of u
        for (i = value->rowPos - 1; i < value->rowPos + 2; i++) {
            for (j = value->colPos - 1; j < value->colPos + 2; j++) {
                // Make sure neighbors are within bounds of map
                if (i > -1 && i < 21 && j > -1 && j < 80) {
                    // alt = distance to u from source + cost of edge from u to neighbor v
                    int alt = squares[value->rowPos][value->colPos].cost + squares[i][j].cost;
                    if (alt < INFINITY_T && alt < dist[i][j]) { // If alternate path is of lower cost
                        // printf("%d, %d\t", alt, altCount);
                        altCount++;
                        dist[i][j] = alt; // set cost to alt
                        prev[i][j]->rowPos = i;
                        prev[i][j]->colPos = j;
                    }
                }
            }
        }
    }

    // Reassign squares.cost to dist to display the map
    for(i = 0; i < 21; i++) {
        for(j = 0; j < 80; j++) {
            free(prev[i][j]);
            squares[i][j].cost = dist[i][j];
        }
    }
    
    heap_destroy(&h);
}

position findPath(terrainMap *terrainMap, int row, int col, character *trainer) {
    int i, j;
    // squares arrays hold the dijkstra generated cost map to the player
    squares rivalSquares[21][80], hikerSquares[21][80];

    // position variable to return
    position moveHere;
    // Stand still by default
    moveHere.rowPos = row;
    moveHere.colPos = col;

    if(trainer->npc == 'r') {
        for(i = 0; i < 21; i++) {
            for(j = 0; j < 80; j++) {
                rivalSquares[i][j].rowPos = i;
                rivalSquares[i][j].colPos = j;
                rivalSquares[i][j].terrain = terrainMap->terrain[i][j];
            }
        }

        populateRivalCosts(terrainMap->terrain, rivalSquares);

        dijkstra(terrainMap->terrain, rivalSquares, terrainMap->pc);

        for (i = row - 1; i < row + 2; i++) {
            for (j = col - 1; j < col + 2; j++) {
                if (rivalSquares[i][j].cost < rivalSquares[row][col].cost) {
                    moveHere.rowPos = i;
                    moveHere.colPos = j;
                }
            }
        }
    }
    
    if(trainer->npc == 'h') {
        for(i = 0; i < 21; i++) {
            for(j = 0; j < 80; j++) {
                hikerSquares[i][j].rowPos = i;
                hikerSquares[i][j].colPos = j;
                hikerSquares[i][j].terrain = terrainMap->terrain[i][j];
            }
        }

        populateHikerCosts(terrainMap->terrain, hikerSquares);

        dijkstra(terrainMap->terrain, hikerSquares, terrainMap->pc);

        for (i = row - 1; i < row + 2; i++) {
            for (j = col - 1; j < col + 2; j++) {
                if (hikerSquares[i][j].cost < hikerSquares[row][col].cost) {
                    moveHere.rowPos = i;
                    moveHere.colPos = j;
                }
            }
        }
    }

    return moveHere;
}

int positionOccupied(int arrSize, position arr[], position pos) {
    int i;

    for (i = 0; i < arrSize; i++) {
        if (arr[i].rowPos == pos.rowPos && arr[i].colPos == pos.colPos) {
            return 1;
        }
    }
    return 0;
}

void displayMap(terrainMap *terrainMap, int numTrainers, character *trainers[]) {
    int i, j, k;
    char charToPrint;

    for (i = 0; i < 21; i++) {
        for (j = 0; j < 80; j++) {
            charToPrint = terrainMap->terrain[i][j];
            for (k = 0; k < numTrainers; k++) {
                if (i == trainers[k]->rowPos && j == trainers[k]->colPos && trainers[k]->npc != '@') {
                    charToPrint = trainers[k]->npc;
                }
            }
            if(i == terrainMap->pc.rowPos && j == terrainMap->pc.colPos) {
                charToPrint = '@';
            }
            switch(charToPrint) {
                case '~' :
                    attron(COLOR_PAIR(WATER_COLOR));
                    break;
                case '^' :
                    attron(COLOR_PAIR(TREE_COLOR));
                    break;
                case '.' :
                    attron(COLOR_PAIR(GRASS_COLOR));
                    break;
                case ':' :
                    attron(COLOR_PAIR(GRASS_COLOR));
                    break;
                case '%' :
                    attron(COLOR_PAIR(BOULDER_COLOR));
                    break;
                case '#' :
                    attron(COLOR_PAIR(ROAD_COLOR));
                    break;
                default :
                    break;
            }
            mvprintw(i + 1, j, "%c", charToPrint);
            switch(charToPrint) {
                case '~' :
                    attroff(COLOR_PAIR(WATER_COLOR));
                    break;
                case '^' :
                    attroff(COLOR_PAIR(TREE_COLOR));
                    break;
                case '.' :
                    attroff(COLOR_PAIR(GRASS_COLOR));
                    break;
                case ':' :
                    attroff(COLOR_PAIR(GRASS_COLOR));
                    break;
                case '%' :
                    attroff(COLOR_PAIR(BOULDER_COLOR));
                    break;
                case '#' :
                    attroff(COLOR_PAIR(ROAD_COLOR));
                    break;
                default :
                    break;
            }
        }
    }
}

void findPosition(character *trainer, terrainMap *terrainMap, int numTrainers) {
    static int positionsMarked = 0;
    int col = (rand() % (70 - 10)) + 10;
    int row = (rand() % (16 - 3)) + 3;

    switch(trainer->npc) {
            case '@' :
                break;
            case 'm' : // is a swimmer
                // pick random position that has not been chosen before and is water
                while(terrainMap->terrain[row][col] != '~') {
                    col = (rand() % (70 - 10)) + 10;
                    row = (rand() % (16 - 3)) + 3;
                }
                // mark trainer position
                trainer->rowPos = row;
                trainer->colPos = col;
                // printf("Placed %c at [%d, %d]\n", trainer->npc, row, col);
                
                positionsMarked++;
                break;
            default : // is any other type of npc
                // pick random position that has not been chosen before and is not a boulder, tree, water, or building
                while(terrainMap->terrain[row][col] == '%'
                || terrainMap->terrain[row][col] == '^'
                || terrainMap->terrain[row][col] == '~'
                || terrainMap->terrain[row][col] == '#'
                || terrainMap->terrain[row][col] == 'M'
                || terrainMap->terrain[row][col] == 'C') {
                    col = (rand() % (70 - 10)) + 10;
                    row = (rand() % (16 - 3)) + 3;
                }
                // mark trainer position
                trainer->rowPos = row;
                trainer->colPos = col;
                // printf("Placed %c at [%d, %d]\n", trainer->npc, row, col);
                // for wanderers to know what terrain they spawned in
                trainer->spawn = terrainMap->terrain[row][col];
                
                positionsMarked++;
                break;
        }
}

int getMoveCost(terrainMap *terrainMap, int row, int col, character *trainer) {
    switch(trainer->npc) {
        case '@' :
            switch(terrainMap->terrain[row][col]) {
                case '#' :
                    return 10;
                case 'M' :
                    return 10;
                case 'C' :
                    return 10;
                case ':' :
                    return 20;
                case '.' :
                    return 10;
                default :
                    return INFINITY_T;
            }
            break;
        case 'h' :
            switch(terrainMap->terrain[row][col]) {
                case '#' :
                    return 10;
                case 'M' :
                    return 50;
                case 'C' :
                    return 50;
                case ':' :
                    return 15;
                case '.' :
                    return 10;
                default :
                    return INFINITY_T;
            }
            break;
        case 'm' :
            switch(terrainMap->terrain[row][col]) {
                case '~' :
                    return 7;
                case '#' :
                    return 7;
                default :
                    return INFINITY_T;
            }
            break;
        case 'w' :
            if (terrainMap->terrain[row][col] == trainer->spawn) {
                return 10;
            } else {
                return INFINITY_T;
            }
            break;
        default :
            switch(terrainMap->terrain[row][col]) {
                case '#' :
                    return 10;
                case 'M' :
                    return 50;
                case 'C' :
                    return 50;
                case ':' :
                    return 20;
                case '.' :
                    return 10;
                case '~' :
                    return INFINITY_T;
                default :
                    return INFINITY_T;
            }
            break;
    }
    return INFINITY_T;
}

int positionNotOccupied(int row, int col, int numTrainers, character *trainers[]) {
    int i;

    for (i = 0; i < numTrainers; i++) {
        if (trainers[i]->rowPos == row && trainers[i]->colPos == col) {
            return i + 10;
        }
    }

    return 1;
}

int notGate(terrainMap *terrainMap, int row, int col) {
    if ((row == 0 && col == terrainMap->northSouthExit)
    || (row == 20 && col == terrainMap->northSouthExit)
    || (col == 0 && row == terrainMap->westEastExit)
    || (col == 79 && row == terrainMap->westEastExit)) {
        return 0;
    }

    return 1; 
}

void generateTrainers(terrainMap *terrainMap, int numTrainers) {
    // pick random assortment of trainers, including at least one hiker and one rival
    // unless numTrainers < 2
    int i;

    character *trainers[numTrainers];
    char trainerOptions[7] = {'r', 'h', 'p', 'w', 's', 'e', 'm'};

    for (i = 0; i < numTrainers; i++) {
        trainers[i] = static_cast<character*>(malloc(sizeof(*trainers[i])));
    }

    for (i = 0; i < numTrainers; i++) {
        trainers[i]->defeated = 0;
    }

    // Fill up trainers[] with random npcs, guaranteeing the first to be a hiker and the second to be a rival, rest are random
    for (i = 0; i < numTrainers; i++) {
        if (i == 0) {
            trainers[i]->npc = 'h';
            trainers[i]->nextMoveTime = 0;
        } else if (i == 1) {
            trainers[i]->npc = 'r';
            trainers[i]->nextMoveTime = 0;
        } else {
            trainers[i]->npc = trainerOptions[rand() % 7];
            trainers[i]->nextMoveTime = 0;
        }
    }
    
    // Player for the queue to allow the user a turn to move and to redraw the map
    trainers[numTrainers - 1]->npc = '@';
    trainers[numTrainers - 1]->rowPos = terrainMap->pc.rowPos;
    trainers[numTrainers - 1]->colPos = terrainMap->pc.colPos;
    trainers[numTrainers - 1]->nextMoveTime = 0;

    // If the player has a preset position, AKA just came from another map, place it next to the corresponding exit.
    if (terrainMap->pc.preset) {
        switch(lastMove) {
            case Up :
                terrainMap->pc.rowPos = 19;
                terrainMap->pc.colPos = terrainMap->northSouthExit;
                break;
            case Down :
                terrainMap->pc.rowPos = 1;
                terrainMap->pc.colPos = terrainMap->northSouthExit;
                break;
            case Left :
                terrainMap->pc.rowPos = terrainMap->westEastExit;
                terrainMap->pc.colPos = 78;
                break;
            case Right :
                terrainMap->pc.rowPos = terrainMap->westEastExit;
                terrainMap->pc.colPos = 1;
                break;
            case None :
                break;
        }
        trainers[numTrainers - 1]->rowPos = terrainMap->pc.rowPos;
        trainers[numTrainers - 1]->colPos = terrainMap->pc.colPos;
    }

    // Place all trainers and give pacers, wanderers, and explorers, a random direction to start with
    direction_t directionOptions[4] = {Up, Down, Left, Right};
    
    for (i = 0; i < numTrainers; i++) {
        if (trainers[i]->npc != '@') {
            findPosition(trainers[i], terrainMap, numTrainers);
        }

        // Build value string to use in heap
        snprintf(trainers[i]->value, sizeof(trainers[i]->value), "%c %d", trainers[i]->npc, i);

        if (trainers[i]->npc == 'w' || trainers[i]->npc == 'p' || trainers[i]->npc == 'e' || trainers[i]->npc == 'm') {
            trainers[i]->direction = directionOptions[rand() % 4];
        }
    }

    heap characterHeap;
    heap_create(&characterHeap, 9999, NULL);

    if (terrainMap->generated == 1) {
        // AKA overwrite trainers[i] with terrainMap->trainers[i] or trainers stored from previous generation.
        for (i = 0; i < numTrainers; i++) {
            trainers[i] = terrainMap->trainers[i];
        }
    } else {
        // Mark as generated.
        terrainMap->generated = 1;
    }

    // Insert trainers into queue.
    for (i = 0; i < numTrainers; i++) {
        heap_insert(&characterHeap, &trainers[i]->nextMoveTime, &trainers[i]->value);
    }

    // While the queue of trainers isn't empty, dequeue the trainer with the cheapest next move, make the move, 
    //  then reinsert with old cost + next move cost
    heap_entry u;
    int dontQuit = 1;
    while(heap_delmin(&characterHeap, &u.key, &u.value) && dontQuit) {
        // move the things
        char value[20];
        char *npc;
        char *index;
        long int moveCost = 0;
        int inBattle = 0;

        // Deconstruct Value String
        strcpy(value, (char*)u.value);
        npc = strtok(value, " ");
        index = strtok(NULL, " ");
        int i = atoi(index);

        switch(*npc) {
            case '@' : {
                usleep(250000);
                displayMap(terrainMap, numTrainers, trainers);
                int c = getch();
                // mvprintw(0, 60, "You pressed %d    ", c);
                switch(c) {
                    case(102) : {
                        int flying = 1;
                        char str[10] = "";
                        mvprintw(0, 0, "You have pressed 'f', activating the fly command.");
                        mvprintw(22, 0, "Press any key to continue, 'esc' to cancel.");
                        int ch = getch();
                        if (ch == 27) {
                            flying = 0;
                            mvprintw(0, 0, "%-80s", "");
                            mvprintw(22, 0, "%-80s", "");
                        }
                        refresh();
                        while(flying) {
                            mvprintw(22, 0, "Please enter two integers from -200 to 200 separated by a space: ");
                            echo();
                            getstr(str);
                            if (strcmp(str, "") != 0) {
                                terrainMap->flyRow = atoi(strtok(str, " "));
                                terrainMap->flyCol = atoi(strtok(NULL, " "));
                                terrainMap->wantToFly = 1;
                                mvprintw(0, 0, "%-80s", "");
                                mvprintw(22, 0, "%-80s", "");
                                flying = 0;
                                dontQuit = 0;
                            }
                            noecho();
                        }
                        break;
                    }
                    case (55) :
                    case (121) : {
                        mvprintw(0, 0, "%-50s", " ");
                        int posNotOcc = positionNotOccupied(trainers[i]->rowPos - 1, trainers[i]->colPos - 1, numTrainers, trainers);
                        if (terrainMap->terrain[trainers[i]->rowPos - 1][trainers[i]->colPos - 1] != '%'
                        && terrainMap->terrain[trainers[i]->rowPos - 1][trainers[i]->colPos - 1] != '^'
                        && terrainMap->terrain[trainers[i]->rowPos - 1][trainers[i]->colPos - 1] != '~'
                        && notGate(terrainMap, trainers[i]->rowPos - 1, trainers[i]->colPos - 1)
                        && posNotOcc == 1) {
                            trainers[i]->colPos--;
                            trainers[i]->rowPos--;
                            terrainMap->pc.rowPos--;
                            terrainMap->pc.colPos--;                
                            moveCost = getMoveCost(terrainMap, trainers[i]->rowPos, trainers[i]->colPos - 1, trainers[i]);
                        }
                        if (posNotOcc != 1) {
                            if(trainers[posNotOcc - 10]->defeated != 0) {
                                mvprintw(0, 0, "%-50s", "You've already defeated this trainer in battle...");
                            } else {
                                inBattle = 1;
                                while(inBattle) {
                                    mvprintw(0, 0, "You entered a battle with %c! Press 'esc' to leave\t\t", trainers[posNotOcc - 10]->npc);                                    int ch = getch();
                                    if (ch == 27) {
                                        inBattle = 0;
                                        trainers[posNotOcc - 10]->defeated = 1;
                                    }
                                    mvprintw(0, 0, "%-50s", "You left the battle.");
                                }
                            }
                        }
                        break;
                    }
                    case (56) :
                    case (107) : {
                        mvprintw(0, 0, "%-50s", " ");
                        int posNotOcc = positionNotOccupied(trainers[i]->rowPos - 1, trainers[i]->colPos, numTrainers, trainers);
                        int gateCheck = notGate(terrainMap, trainers[i]->rowPos - 1, trainers[i]->colPos);

                        if (terrainMap->terrain[trainers[i]->rowPos - 1][trainers[i]->colPos] != '%'
                        && terrainMap->terrain[trainers[i]->rowPos - 1][trainers[i]->colPos] != '^'
                        && terrainMap->terrain[trainers[i]->rowPos - 1][trainers[i]->colPos] != '~'
                        && gateCheck
                        && posNotOcc == 1) {
                            trainers[i]->rowPos--;
                            terrainMap->pc.rowPos--;
                            moveCost = getMoveCost(terrainMap, trainers[i]->rowPos - 1, trainers[i]->colPos, trainers[i]);
                        }

                        if (posNotOcc != 1) {
                            if(trainers[posNotOcc - 10]->defeated != 0) {
                                mvprintw(0, 0, "%-50s", "You've already defeated this trainer in battle...");
                            } else {
                                inBattle = 1;
                                while(inBattle) {
                                    mvprintw(0, 0, "You entered a battle with %c! Press 'esc' to leave\t\t", trainers[posNotOcc - 10]->npc);                                    mvprintw(0, 0, "You entered a battle with %c! Press 'esc' to leave\t\t", trainers[posNotOcc - 10]->npc);
                                    int ch = getch();
                                    if (ch == 27) {
                                        inBattle = 0;
                                        trainers[posNotOcc - 10]->defeated = 1;
                                    }
                                    mvprintw(0, 0, "%-50s", "You left the battle.");
                                }
                            }
                        }

                        if (gateCheck == 0) { // If player wants to move into gate
                            // Save trainer array to terrainMap
                            for(int k = 0; k < numTrainers; k++) {
                                terrainMap->trainers[k] = trainers[k];
                            }
                            lastMove = Up;
                            dontQuit = 0;
                        }

                        break;
                    }
                    case (57) :
                    case (117) : {
                        mvprintw(0, 0, "%-50s", " ");
                        int posNotOcc = positionNotOccupied(trainers[i]->rowPos - 1, trainers[i]->colPos + 1, numTrainers, trainers);
                        if (terrainMap->terrain[trainers[i]->rowPos - 1][trainers[i]->colPos + 1] != '%'
                        && terrainMap->terrain[trainers[i]->rowPos - 1][trainers[i]->colPos + 1] != '^'
                        && terrainMap->terrain[trainers[i]->rowPos - 1][trainers[i]->colPos + 1] != '~'
                        && notGate(terrainMap, trainers[i]->rowPos - 1, trainers[i]->colPos + 1)
                        && posNotOcc == 1) {
                            trainers[i]->rowPos--;
                            trainers[i]->colPos++;
                            terrainMap->pc.rowPos--;
                            terrainMap->pc.colPos++;
                            moveCost = getMoveCost(terrainMap, trainers[i]->rowPos - 1, trainers[i]->colPos + 1, trainers[i]);
                        }
                        if (posNotOcc != 1) {
                            if(trainers[posNotOcc - 10]->defeated != 0) {
                                mvprintw(0, 0, "%-50s", "You've already defeated this trainer in battle...");
                            } else {
                                inBattle = 1;
                                while(inBattle) {
                                    mvprintw(0, 0, "You entered a battle with %c! Press 'esc' to leave\t\t", trainers[posNotOcc - 10]->npc);                                    int ch = getch();
                                    if (ch == 27) {
                                        inBattle = 0;
                                        trainers[posNotOcc - 10]->defeated = 1;
                                    }
                                    mvprintw(0, 0, "%-50s", "You left the battle.");
                                }
                            }
                        }
                        break;
                    }
                    case (54) :
                    case (108) : {
                        mvprintw(0, 0, "%-50s", " ");
                        int posNotOcc = positionNotOccupied(trainers[i]->rowPos, trainers[i]->colPos + 1, numTrainers, trainers);
                        int gateCheck = notGate(terrainMap, trainers[i]->rowPos, trainers[i]->colPos + 1);

                        if (terrainMap->terrain[trainers[i]->rowPos][trainers[i]->colPos + 1] != '%'
                        && terrainMap->terrain[trainers[i]->rowPos][trainers[i]->colPos + 1] != '^'
                        && terrainMap->terrain[trainers[i]->rowPos][trainers[i]->colPos + 1] != '~'
                        && gateCheck
                        && posNotOcc == 1) {
                            trainers[i]->colPos++;
                            terrainMap->pc.colPos++;
                            moveCost = getMoveCost(terrainMap, trainers[i]->rowPos, trainers[i]->colPos + 1, trainers[i]);
                        }

                        if (posNotOcc != 1) {
                            if(trainers[posNotOcc - 10]->defeated != 0) {
                                mvprintw(0, 0, "%-50s", "You've already defeated this trainer in battle...");
                            } else {
                                inBattle = 1;
                                while(inBattle) {
                                    mvprintw(0, 0, "You entered a battle with %c! Press 'esc' to leave\t\t", trainers[posNotOcc - 10]->npc);                                    int ch = getch();
                                    if (ch == 27) {
                                        inBattle = 0;
                                        trainers[posNotOcc - 10]->defeated = 1;
                                    }
                                    mvprintw(0, 0, "%-50s", "You left the battle.");
                                }
                            }
                        }

                        if (gateCheck == 0) { // If player wants to move into gate
                            // Save trainer array to terrainMap
                            for(int k = 0; k < numTrainers; k++) {
                                terrainMap->trainers[k] = trainers[k];
                                terrainMap->trainers[k]->rowPos = trainers[k]->rowPos;
                                terrainMap->trainers[k]->colPos = trainers[k]->colPos;
                            }
                            lastMove = Right;
                            dontQuit = 0;
                        }
                        break;
                    }
                    case (51) :
                    case (110) : {
                        mvprintw(0, 0, "%-50s", " ");
                        int posNotOcc = positionNotOccupied(trainers[i]->rowPos + 1, trainers[i]->colPos + 1, numTrainers, trainers);
                        if (terrainMap->terrain[trainers[i]->rowPos + 1][trainers[i]->colPos + 1] != '%'
                        && terrainMap->terrain[trainers[i]->rowPos + 1][trainers[i]->colPos + 1] != '^'
                        && terrainMap->terrain[trainers[i]->rowPos + 1][trainers[i]->colPos + 1] != '~'
                        && notGate(terrainMap, trainers[i]->rowPos + 1, trainers[i]->colPos + 1)
                        && posNotOcc == 1) {
                            trainers[i]->colPos++;
                            trainers[i]->rowPos++;
                            terrainMap->pc.colPos++;
                            terrainMap->pc.rowPos++;
                            moveCost = getMoveCost(terrainMap, trainers[i]->rowPos + 1, trainers[i]->colPos + 1, trainers[i]);
                        }
                        if (posNotOcc != 1) {
                            if(trainers[posNotOcc - 10]->defeated != 0) {
                                mvprintw(0, 0, "%-50s", "You've already defeated this trainer in battle...");
                            } else {
                                inBattle = 1;
                                while(inBattle) {
                                    mvprintw(0, 0, "You entered a battle with %c! Press 'esc' to leave\t\t", trainers[posNotOcc - 10]->npc);                                    int ch = getch();
                                    if (ch == 27) {
                                        inBattle = 0;
                                        trainers[posNotOcc - 10]->defeated = 1;
                                    }
                                    mvprintw(0, 0, "%-50s", "You left the battle.");
                                }
                            }
                        }
                        break;
                    }
                    case (50) :
                    case (106) : {
                        mvprintw(0, 0, "%-50s", " ");
                        int posNotOcc = positionNotOccupied(trainers[i]->rowPos + 1, trainers[i]->colPos, numTrainers, trainers);
                        int gateCheck = notGate(terrainMap, trainers[i]->rowPos + 1, trainers[i]->colPos);

                        if (terrainMap->terrain[trainers[i]->rowPos + 1][trainers[i]->colPos] != '%'
                        && terrainMap->terrain[trainers[i]->rowPos + 1][trainers[i]->colPos] != '^'
                        && terrainMap->terrain[trainers[i]->rowPos + 1][trainers[i]->colPos] != '~'
                        && gateCheck
                        && posNotOcc == 1) {
                            trainers[i]->rowPos++;
                            terrainMap->pc.rowPos++;
                            moveCost = getMoveCost(terrainMap, trainers[i]->rowPos + 1, trainers[i]->colPos, trainers[i]);
                        }

                        if (posNotOcc != 1) {
                            if(trainers[posNotOcc - 10]->defeated != 0) {
                                mvprintw(0, 0, "%-50s", "You've already defeated this trainer in battle...");
                            } else {
                                inBattle = 1;
                                while(inBattle) {
                                    mvprintw(0, 0, "You entered a battle with %c! Press 'esc' to leave\t\t", trainers[posNotOcc - 10]->npc);                                    int ch = getch();
                                    if (ch == 27) {
                                        inBattle = 0;
                                        trainers[posNotOcc - 10]->defeated = 1;
                                    }
                                    mvprintw(0, 0, "%-50s", "You left the battle.");
                                }
                            }
                        }

                        if (gateCheck == 0) { // If player wants to move into gate
                            // Save trainer array to terrainMap
                            for(int k = 0; k < numTrainers; k++) {
                                terrainMap->trainers[k] = trainers[k];
                            }
                            lastMove = Down;
                            dontQuit = 0;
                        }
                        break;
                    }
                    case (49) :
                    case (98) : {
                        mvprintw(0, 0, "%-50s", " ");
                        int posNotOcc = positionNotOccupied(trainers[i]->rowPos + 1, trainers[i]->colPos - 1, numTrainers, trainers);
                        if (terrainMap->terrain[trainers[i]->rowPos + 1][trainers[i]->colPos - 1] != '%'
                        && terrainMap->terrain[trainers[i]->rowPos + 1][trainers[i]->colPos - 1] != '^'
                        && terrainMap->terrain[trainers[i]->rowPos + 1][trainers[i]->colPos - 1] != '~'
                        && notGate(terrainMap, trainers[i]->rowPos + 1, trainers[i]->colPos - 1)
                        && posNotOcc == 1) {
                            trainers[i]->colPos--;
                            trainers[i]->rowPos++;
                            terrainMap->pc.colPos--;
                            terrainMap->pc.rowPos++;
                            moveCost = getMoveCost(terrainMap, trainers[i]->rowPos + 1, trainers[i]->colPos - 1, trainers[i]);
                        }
                        if (posNotOcc != 1) {
                            if(trainers[posNotOcc - 10]->defeated != 0) {
                                mvprintw(0, 0, "%-50s", "You've already defeated this trainer in battle...");
                            } else {
                                inBattle = 1;
                                while(inBattle) {
                                    mvprintw(0, 0, "You entered a battle with %c! Press 'esc' to leave\t\t", trainers[posNotOcc - 10]->npc);                                    int ch = getch();
                                    if (ch == 27) {
                                        inBattle = 0;
                                        trainers[posNotOcc - 10]->defeated = 1;
                                    }
                                    mvprintw(0, 0, "%-50s", "You left the battle.");
                                }
                            }
                        }
                        break;
                    }
                    case (52) :
                    case (104) : {
                        mvprintw(0, 0, "%-50s", " ");
                        int posNotOcc = positionNotOccupied(trainers[i]->rowPos, trainers[i]->colPos - 1, numTrainers, trainers);
                        int gateCheck = notGate(terrainMap, trainers[i]->rowPos, trainers[i]->colPos - 1);
                        
                        if (terrainMap->terrain[trainers[i]->rowPos][trainers[i]->colPos - 1] != '%'
                        && terrainMap->terrain[trainers[i]->rowPos][trainers[i]->colPos - 1] != '^'
                        && terrainMap->terrain[trainers[i]->rowPos][trainers[i]->colPos - 1] != '~'
                        && gateCheck
                        && posNotOcc == 1) {
                            trainers[i]->colPos--;
                            terrainMap->pc.colPos--;
                            moveCost = getMoveCost(terrainMap, trainers[i]->rowPos, trainers[i]->colPos - 1, trainers[i]);
                        }

                        if (posNotOcc != 1) {
                            if(trainers[posNotOcc - 10]->defeated != 0) {
                                mvprintw(0, 0, "%-50s", "You've already defeated this trainer in battle...");
                            } else {
                                inBattle = 1;
                                while(inBattle) {
                                    mvprintw(0, 0, "You entered a battle with %c! Press 'esc' to leave\t\t", trainers[posNotOcc - 10]->npc);                                    int ch = getch();
                                    if (ch == 27) {
                                        inBattle = 0;
                                        trainers[posNotOcc - 10]->defeated = 1;
                                    }
                                    mvprintw(0, 0, "%-50s", "You left the battle.");
                                }
                            }
                        }

                        if (gateCheck == 0) { // If player wants to move into gate
                            // Save trainer array to terrainMap
                            for(int k = 0; k < numTrainers; k++) {
                                terrainMap->trainers[k] = trainers[k];
                            }
                            lastMove = Left;
                            dontQuit = 0;
                        }
                        break;
                    }
                    case (53) :
                    case (32) :
                    case (46) : {
                        mvprintw(0, 0, "%-50s", "Resting...");
                        moveCost = getMoveCost(terrainMap, trainers[i]->rowPos, trainers[i]->colPos, trainers[i]);
                        break;
                    }
                    case (62) : {
                        mvprintw(0, 0, "Attempt to enter building    ");
                        if(terrainMap->terrain[trainers[i]->rowPos][trainers[i]->colPos] == 'M' || terrainMap->terrain[trainers[i]->rowPos][trainers[i]->colPos] == 'C') {
                            mvprintw(0, 0, "%-50s", "Entered Building");
                            int inBuilding = 1;
                            while(inBuilding) {                                                    
                                mvprintw(0, 30, "%50s", "You are in the building, press '<' to exit");
                                int ch = getch();
                                if (ch == 60) {
                                    mvprintw(0, 0, "%-50s", "Exited building");
                                    mvprintw(0, 30, "%50s", " ");
                                    inBuilding = 0;
                                }
                                refresh();
                            }
                        } else {
                            mvprintw(0, 0, "%-50s", "Not standing on Building");
                        }
                        break;
                    }
                    case (116) : {
                        mvprintw(0, 0, "%-50s", "List of Nearby Trainers");
                        int displayingList = 1;
                        int scroll = 0;
                        while(displayingList) {
                            int northSouth = trainers[numTrainers - 1]->rowPos - trainers[scroll]->rowPos;

                            int westEast = trainers[numTrainers - 1]->colPos - trainers[scroll]->colPos;
                            mvprintw(0, 0, "Trainer: %c\t Distance to Player: %d %s %d %s \t\t", trainers[scroll]->npc, (northSouth > 0 ? northSouth : northSouth * -1), (northSouth > 0 ? "North" : "South"), (westEast > 0 ? westEast : westEast * -1), ((westEast > 0 ? "West" : "East")));
                            mvprintw(22, 0, "Displaying Trainers: Press UP or DOWN to scroll, 'esc' to Close List");
                            refresh();
                            int c = getch();
                            if (c == 259 && (scroll - 1) >= 0 && (scroll - 1) < numTrainers - 2) { // up
                                scroll--;
                            }
                            if (c == 258 && (scroll + 1) >= 0 && (scroll + 1) < numTrainers - 2) { // down
                                scroll++;
                            }
                            if (c == 27) {
                                mvprintw(0, 0, "%80s", " ");
                                mvprintw(22, 0, "%80s", " ");
                                displayingList = 0;
                            }
                        }
                        break;
                    }
                    case (113) : {
                        mvprintw(0, 0, "%-50s", "Quitting...");
                        refresh();
                        usleep(500000);
                        terrainMap->quit = 1;
                        dontQuit = 0;
                        break;
                    }
                }
                if (moveCost < INFINITY_T) {
                    trainers[i]->nextMoveTime += moveCost;
                }
                heap_insert(&characterHeap, &trainers[i]->nextMoveTime, &trainers[i]->npc);
                break;
            }
            case 'r' : {
                position rivalMove = findPath(terrainMap, trainers[i]->rowPos, trainers[i]->colPos, trainers[i]);
                if (rivalMove.rowPos != terrainMap->pc.rowPos && rivalMove.colPos != terrainMap->pc.colPos) {
                    trainers[i]->rowPos = rivalMove.rowPos;
                    trainers[i]->colPos = rivalMove.colPos;
                }
                moveCost = getMoveCost(terrainMap, rivalMove.rowPos, rivalMove.colPos, trainers[i]);
                if (moveCost < INFINITY_T) {
                    trainers[i]->nextMoveTime += moveCost;
                }
                heap_insert(&characterHeap, &trainers[i]->nextMoveTime, &trainers[i]->npc);
                break;
            }
            case 'h' : {
                position hikerMove = findPath(terrainMap, trainers[i]->rowPos, trainers[i]->colPos, trainers[i]);
                if (hikerMove.rowPos != terrainMap->pc.rowPos && hikerMove.colPos != terrainMap->pc.colPos) {
                    trainers[i]->rowPos = hikerMove.rowPos;
                    trainers[i]->colPos = hikerMove.colPos;
                }
                moveCost = getMoveCost(terrainMap, hikerMove.rowPos, hikerMove.colPos, trainers[i]);
                if (moveCost < INFINITY_T) {
                    trainers[i]->nextMoveTime += moveCost;
                }
                heap_insert(&characterHeap, &trainers[i]->nextMoveTime, &trainers[i]->npc);
                break;
            }
            case 'p' : {
                if (trainers[i]->direction == Left) {
                    if (terrainMap->terrain[trainers[i]->rowPos][trainers[i]->colPos - 1] != '%'
                    && terrainMap->terrain[trainers[i]->rowPos][trainers[i]->colPos - 1] != '^'
                    && terrainMap->terrain[trainers[i]->rowPos][trainers[i]->colPos - 1] != '~'
                    && terrainMap->terrain[trainers[i]->rowPos][trainers[i]->colPos - 1] != 'M'
                    && terrainMap->terrain[trainers[i]->rowPos][trainers[i]->colPos - 1] != 'C'
                    && notGate(terrainMap, trainers[i]->rowPos, trainers[i]->colPos - 1)
                    && positionNotOccupied(trainers[i]->rowPos, trainers[i]->colPos - 1, numTrainers, trainers)) {
                        trainers[i]->colPos--;
                        moveCost = getMoveCost(terrainMap, trainers[i]->rowPos, trainers[i]->colPos - 1, trainers[i]);
                        if (moveCost < INFINITY_T) {
                            trainers[i]->nextMoveTime += moveCost;
                        }
                    } else {
                        trainers[i]->direction = Right;
                    }
                }
                if (trainers[i]->direction == Right) {
                    if (terrainMap->terrain[trainers[i]->rowPos][trainers[i]->colPos + 1] != '%'
                    && terrainMap->terrain[trainers[i]->rowPos][trainers[i]->colPos + 1] != '^'
                    && terrainMap->terrain[trainers[i]->rowPos][trainers[i]->colPos + 1] != '~'
                    && terrainMap->terrain[trainers[i]->rowPos][trainers[i]->colPos + 1] != 'M'
                    && terrainMap->terrain[trainers[i]->rowPos][trainers[i]->colPos + 1] != 'C'
                    && notGate(terrainMap, trainers[i]->rowPos, trainers[i]->colPos + 1)
                    && positionNotOccupied(trainers[i]->rowPos, trainers[i]->colPos + 1, numTrainers, trainers)) {
                        trainers[i]->colPos++;
                        moveCost = getMoveCost(terrainMap, trainers[i]->rowPos, trainers[i]->colPos + 1, trainers[i]);
                        if (moveCost < INFINITY_T) {
                            trainers[i]->nextMoveTime += moveCost;
                        }
                    } else {
                        trainers[i]->direction = Left;
                    }
                }
                if (trainers[i]->direction == Down) {
                    if (terrainMap->terrain[trainers[i]->rowPos + 1][trainers[i]->colPos] != '%'
                    && terrainMap->terrain[trainers[i]->rowPos + 1][trainers[i]->colPos] != '^'
                    && terrainMap->terrain[trainers[i]->rowPos + 1][trainers[i]->colPos] != '~'
                    && terrainMap->terrain[trainers[i]->rowPos + 1][trainers[i]->colPos] != 'M'
                    && terrainMap->terrain[trainers[i]->rowPos + 1][trainers[i]->colPos] != 'C'
                    && notGate(terrainMap, trainers[i]->rowPos + 1, trainers[i]->colPos)
                    && positionNotOccupied(trainers[i]->rowPos + 1, trainers[i]->colPos, numTrainers, trainers)) {
                        trainers[i]->rowPos++;
                        moveCost = getMoveCost(terrainMap, trainers[i]->rowPos + 1, trainers[i]->colPos, trainers[i]);
                        if (moveCost < INFINITY_T) {
                            trainers[i]->nextMoveTime += moveCost;
                        }
                    } else {
                        trainers[i]->direction = Up;
                    }
                }
                if (trainers[i]->direction == Up) {
                    if (terrainMap->terrain[trainers[i]->rowPos - 1][trainers[i]->colPos] != '%'
                    && terrainMap->terrain[trainers[i]->rowPos - 1][trainers[i]->colPos] != '^'
                    && terrainMap->terrain[trainers[i]->rowPos - 1][trainers[i]->colPos] != '~'
                    && terrainMap->terrain[trainers[i]->rowPos - 1][trainers[i]->colPos] != 'M'
                    && terrainMap->terrain[trainers[i]->rowPos - 1][trainers[i]->colPos] != 'C'
                    && notGate(terrainMap, trainers[i]->rowPos - 1, trainers[i]->colPos)
                    && positionNotOccupied(trainers[i]->rowPos - 1, trainers[i]->colPos, numTrainers, trainers)) {
                        trainers[i]->rowPos--;
                        moveCost = getMoveCost(terrainMap, trainers[i]->rowPos - 1, trainers[i]->colPos, trainers[i]);
                        if (moveCost < INFINITY_T) {
                            trainers[i]->nextMoveTime += moveCost;
                        }
                    } else {
                        trainers[i]->direction = Down;
                    }
                }
                // printf("Moved Pacer\n");
                heap_insert(&characterHeap, &trainers[i]->nextMoveTime, &trainers[i]->npc);
                // printf("Inserted Pacer with nextMoveTime: %ld\n", trainers[i]->nextMoveTime);
                break;
            }
            case 'w' : {
                // move in direction until reach edge of spawn terrain then walk in random new direction
                if (trainers[i]->direction == Left) {
                    if (positionNotOccupied(trainers[i]->rowPos, trainers[i]->colPos - 1, numTrainers, trainers)
                    && notGate(terrainMap, trainers[i]->rowPos, trainers[i]->colPos - 1)
                    && terrainMap->terrain[trainers[i]->rowPos][trainers[i]->colPos - 1] == trainers[i]->spawn) {
                        trainers[i]->colPos--;
                        moveCost = getMoveCost(terrainMap, trainers[i]->rowPos, trainers[i]->colPos - 1, trainers[i]);
                        if (moveCost < INFINITY_T) {
                            trainers[i]->nextMoveTime += moveCost;
                        }
                    } else {
                        trainers[i]->direction = directionOptions[rand() % 4];
                    }
                }
                if (trainers[i]->direction == Right) {
                    if (positionNotOccupied(trainers[i]->rowPos, trainers[i]->colPos + 1, numTrainers, trainers)
                    && notGate(terrainMap, trainers[i]->rowPos, trainers[i]->colPos + 1)
                    && terrainMap->terrain[trainers[i]->rowPos][trainers[i]->colPos + 1] == trainers[i]->spawn) {
                        trainers[i]->colPos++;
                        moveCost = getMoveCost(terrainMap, trainers[i]->rowPos, trainers[i]->colPos + 1, trainers[i]);
                        if (moveCost < INFINITY_T) {
                            trainers[i]->nextMoveTime += moveCost;
                        }
                    } else {
                        trainers[i]->direction = directionOptions[rand() % 4];
                    }
                }
                if (trainers[i]->direction == Down) {
                    if (positionNotOccupied(trainers[i]->rowPos + 1, trainers[i]->colPos, numTrainers, trainers)
                    && notGate(terrainMap, trainers[i]->rowPos + 1, trainers[i]->colPos)
                    && terrainMap->terrain[trainers[i]->rowPos + 1][trainers[i]->colPos] == trainers[i]->spawn) {
                        trainers[i]->rowPos++;
                        moveCost = getMoveCost(terrainMap, trainers[i]->rowPos + 1, trainers[i]->colPos, trainers[i]);
                        if (moveCost < INFINITY_T) {
                            trainers[i]->nextMoveTime += moveCost;
                        }
                    } else {
                        trainers[i]->direction = directionOptions[rand() % 4];
                    }
                }
                if (trainers[i]->direction == Up) {
                    if (positionNotOccupied(trainers[i]->rowPos - 1, trainers[i]->colPos, numTrainers, trainers)
                    && notGate(terrainMap, trainers[i]->rowPos - 1, trainers[i]->colPos)
                    && terrainMap->terrain[trainers[i]->rowPos - 1][trainers[i]->colPos] == trainers[i]->spawn) {
                        trainers[i]->rowPos--;
                        moveCost = getMoveCost(terrainMap, trainers[i]->rowPos - 1, trainers[i]->colPos, trainers[i]);
                        if (moveCost < INFINITY_T) {
                            trainers[i]->nextMoveTime += moveCost;
                        }
                    } else {
                        trainers[i]->direction = directionOptions[rand() % 4];
                    }
                }
                // printf("Moved Wanderer\n");
                // usleep(250000);
                // displayMap(terrainMap, numTrainers, trainers);
                heap_insert(&characterHeap, &trainers[i]->nextMoveTime, &trainers[i]->npc);
                // printf("Inserted Wanderer with nextMoveTime: %ld\n", trainers[i]->nextMoveTime);
                break;
            }
            case 's' :
                // Sentries don't move
                break;
            case 'e' : {
                // move in direction until reach impassable terrain (boulder, tree, building, or water) then walk in random new direction
                if (trainers[i]->direction == Left) {
                    if (terrainMap->terrain[trainers[i]->rowPos][trainers[i]->colPos - 1] != '%'
                    && terrainMap->terrain[trainers[i]->rowPos][trainers[i]->colPos - 1] != '^'
                    && terrainMap->terrain[trainers[i]->rowPos][trainers[i]->colPos - 1] != '~'
                    && terrainMap->terrain[trainers[i]->rowPos][trainers[i]->colPos - 1] != 'M'
                    && terrainMap->terrain[trainers[i]->rowPos][trainers[i]->colPos - 1] != 'C'
                    && notGate(terrainMap, trainers[i]->rowPos, trainers[i]->colPos - 1)
                    && positionNotOccupied(trainers[i]->rowPos, trainers[i]->colPos - 1, numTrainers, trainers)) {
                        trainers[i]->colPos--;
                        moveCost = getMoveCost(terrainMap, trainers[i]->rowPos, trainers[i]->colPos - 1, trainers[i]);
                        if (moveCost < INFINITY_T) {
                            trainers[i]->nextMoveTime += moveCost;
                        }
                    } else {
                        trainers[i]->direction = directionOptions[rand() % 4];
                    }
                }
                if (trainers[i]->direction == Right) {
                    if (terrainMap->terrain[trainers[i]->rowPos][trainers[i]->colPos + 1] != '%'
                    && terrainMap->terrain[trainers[i]->rowPos][trainers[i]->colPos + 1] != '^'
                    && terrainMap->terrain[trainers[i]->rowPos][trainers[i]->colPos + 1] != '~'
                    && terrainMap->terrain[trainers[i]->rowPos][trainers[i]->colPos + 1] != 'M'
                    && terrainMap->terrain[trainers[i]->rowPos][trainers[i]->colPos + 1] != 'C'
                    && notGate(terrainMap, trainers[i]->rowPos, trainers[i]->colPos + 1)
                    && positionNotOccupied(trainers[i]->rowPos, trainers[i]->colPos + 1, numTrainers, trainers)) {
                        trainers[i]->colPos++;
                        moveCost = getMoveCost(terrainMap, trainers[i]->rowPos, trainers[i]->colPos + 1, trainers[i]);
                        if (moveCost < INFINITY_T) {
                            trainers[i]->nextMoveTime += moveCost;
                        }
                    } else {
                        trainers[i]->direction = directionOptions[rand() % 4];
                    }
                }
                if (trainers[i]->direction == Down) {
                    if (terrainMap->terrain[trainers[i]->rowPos + 1][trainers[i]->colPos] != '%'
                    && terrainMap->terrain[trainers[i]->rowPos + 1][trainers[i]->colPos] != '^'
                    && terrainMap->terrain[trainers[i]->rowPos + 1][trainers[i]->colPos] != '~'
                    && terrainMap->terrain[trainers[i]->rowPos + 1][trainers[i]->colPos] != 'M'
                    && terrainMap->terrain[trainers[i]->rowPos + 1][trainers[i]->colPos] != 'C'
                    && notGate(terrainMap, trainers[i]->rowPos + 1, trainers[i]->colPos)
                    && positionNotOccupied(trainers[i]->rowPos + 1, trainers[i]->colPos, numTrainers, trainers)) {
                        trainers[i]->rowPos++;
                        moveCost = getMoveCost(terrainMap, trainers[i]->rowPos + 1, trainers[i]->colPos, trainers[i]);
                        if (moveCost < INFINITY_T) {
                            trainers[i]->nextMoveTime += moveCost;
                        }
                    } else {
                        trainers[i]->direction = directionOptions[rand() % 4];
                    }
                }
                if (trainers[i]->direction == Up) {
                    if (terrainMap->terrain[trainers[i]->rowPos - 1][trainers[i]->colPos] != '%'
                    && terrainMap->terrain[trainers[i]->rowPos - 1][trainers[i]->colPos] != '^'
                    && terrainMap->terrain[trainers[i]->rowPos - 1][trainers[i]->colPos] != '~'
                    && terrainMap->terrain[trainers[i]->rowPos - 1][trainers[i]->colPos] != 'M'
                    && terrainMap->terrain[trainers[i]->rowPos - 1][trainers[i]->colPos] != 'C'
                    && notGate(terrainMap, trainers[i]->rowPos - 1, trainers[i]->colPos)
                    && positionNotOccupied(trainers[i]->rowPos - 1, trainers[i]->colPos, numTrainers, trainers)) {
                        trainers[i]->rowPos--;
                        moveCost = getMoveCost(terrainMap, trainers[i]->rowPos - 1, trainers[i]->colPos, trainers[i]);
                        if (moveCost < INFINITY_T) {
                            trainers[i]->nextMoveTime += moveCost;
                        }
                    } else {
                        trainers[i]->direction = directionOptions[rand() % 4];
                    }
                }
                // printf("Moved Explorer\n");
                heap_insert(&characterHeap, &trainers[i]->nextMoveTime, &trainers[i]->npc);
                // printf("Inserted Explorer with nextMoveTime: %ld\n", trainers[i]->nextMoveTime);
                break;
            }
            case 'm' : {
                // move in direction until reach edge of spawn terrain then walk in random new direction
                // if player is cardinally adjacent/on edge of water directly north, south, west, or east, move towards player
                if (trainers[i]->direction == Left) {
                    if (positionNotOccupied(trainers[i]->rowPos, trainers[i]->colPos - 1, numTrainers, trainers)
                    && (terrainMap->terrain[trainers[i]->rowPos][trainers[i]->colPos - 1] == '~'
                    || (terrainMap->terrain[trainers[i]->rowPos][trainers[i]->colPos - 1] == '#' && terrainMap->terrain[trainers[i]->rowPos][trainers[i]->colPos - 2] == '#'))) {
                        trainers[i]->colPos--;
                        moveCost = getMoveCost(terrainMap, trainers[i]->rowPos, trainers[i]->colPos - 1, trainers[i]);
                        if (moveCost < INFINITY_T) {
                            trainers[i]->nextMoveTime += moveCost;
                        }
                    } else {
                        trainers[i]->direction = directionOptions[rand() % 4];
                    }
                }
                if (trainers[i]->direction == Right) {
                    if (positionNotOccupied(trainers[i]->rowPos, trainers[i]->colPos + 1, numTrainers, trainers)
                    && (terrainMap->terrain[trainers[i]->rowPos][trainers[i]->colPos + 1] == '~'
                    || (terrainMap->terrain[trainers[i]->rowPos][trainers[i]->colPos + 1] == '#' && terrainMap->terrain[trainers[i]->rowPos][trainers[i]->colPos + 2] == '~'))) {
                        trainers[i]->colPos++;
                        moveCost = getMoveCost(terrainMap, trainers[i]->rowPos, trainers[i]->colPos + 1, trainers[i]);
                        if (moveCost < INFINITY_T) {
                            trainers[i]->nextMoveTime += moveCost;
                        }
                    } else {
                        trainers[i]->direction = directionOptions[rand() % 4];
                    }
                }
                if (trainers[i]->direction == Down) {
                    if (positionNotOccupied(trainers[i]->rowPos + 1, trainers[i]->colPos, numTrainers, trainers)
                    && (terrainMap->terrain[trainers[i]->rowPos + 1][trainers[i]->colPos] == '~'
                    || (terrainMap->terrain[trainers[i]->rowPos + 1][trainers[i]->colPos] == '#' && terrainMap->terrain[trainers[i]->rowPos + 2][trainers[i]->colPos] == '#'))) {
                        trainers[i]->rowPos++;
                        moveCost = getMoveCost(terrainMap, trainers[i]->rowPos + 1, trainers[i]->colPos, trainers[i]);
                        if (moveCost < INFINITY_T) {
                            trainers[i]->nextMoveTime += moveCost;
                        }
                    } else {
                        trainers[i]->direction = directionOptions[rand() % 4];
                    }
                }
                if (trainers[i]->direction == Up) {
                    if (positionNotOccupied(trainers[i]->rowPos - 1, trainers[i]->colPos, numTrainers, trainers)
                    &&(terrainMap->terrain[trainers[i]->rowPos - 1][trainers[i]->colPos] == '~'
                    || (terrainMap->terrain[trainers[i]->rowPos - 1][trainers[i]->colPos] == '#' && terrainMap->terrain[trainers[i]->rowPos - 2][trainers[i]->colPos] == '#'))) {
                        trainers[i]->rowPos--;
                        moveCost = getMoveCost(terrainMap, trainers[i]->rowPos - 1, trainers[i]->colPos, trainers[i]);
                        if (moveCost < INFINITY_T) {
                            trainers[i]->nextMoveTime += moveCost;
                        }
                    } else {
                        trainers[i]->direction = directionOptions[rand() % 4];
                    }
                }
                heap_insert(&characterHeap, &trainers[i]->nextMoveTime, &trainers[i]->npc);
                break;
            }
            default :
                break;
        }
    }

    for (i = 0; i < numTrainers; i++) {
        free(trainers[i]);
    }

    heap_destroy(&characterHeap);
}

void generateTerrain(terrainMap *tM, int a, int b, int firstGeneration, int numTrainers) {
    int i, j;

    // Add border to map fill rest with short grass
    for (i = 0; i < 21; i++) {
        for (j = 0; j < 80; j++) {
            if (i == 0 || i == 20 || j == 0 || j == 79) {
                tM->terrain[i][j] = '%';
            } else {
                tM->terrain[i][j] = '.';
            }
        }
    }

    // Calculate building spawn chance
    double chance = (rand() / (RAND_MAX / 1.00));
    double bldngSpawnChance = abs(a - 200) + abs(b - 200);
    bldngSpawnChance *= -45.00;
    bldngSpawnChance /= 400.00;
    bldngSpawnChance += 50.00;
    bldngSpawnChance /= 100.00;

    generateExits(tM, a, b);
    generateTallGrass(tM->terrain);
    generateWater(tM->terrain);
    generatePaths(tM, a, b);
    if ((chance < bldngSpawnChance && chance > 0.00) || firstGeneration) {
        generateBuildings(tM, a, b);
    }
    decorateTerrain(tM->terrain);
    placeCharacter(tM);
    generateTrainers(tM, numTrainers);
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    int i, j;
    int quit = 0;
    int currWorldRow = 200;
    int currWorldCol = 200;
    int numTrainers = 8; // Default number of trainers = 8

    // If the user passed a parameter
    if(argv[1]) {
        if (strcmp(argv[1], "--numtrainers") == 0) {
            // Generate terrain with the number they passed
            numTrainers = atoi(argv[2]) + 1;
        }
        // if (verifyFileName(argv[1])) {
        //     parseFile(argv[1]);
        // } else {
        //     printf("Invalid file name\n");
        // }
    }

    initscr();
    keypad(stdscr, TRUE);		/* We get F1, F2 etc..		*/
	noecho();			/* Don't echo() while we do getch */
    start_color();
    init_pair(GRASS_COLOR, COLOR_GREEN, COLOR_BLACK);
    init_pair(WATER_COLOR, COLOR_BLUE, COLOR_BLACK);
    init_pair(TREE_COLOR, COLOR_BLACK, COLOR_GREEN);
    init_pair(BOULDER_COLOR, COLOR_CYAN, COLOR_BLACK);
    init_pair(ROAD_COLOR, COLOR_YELLOW, COLOR_BLACK);

    for (i = 0; i < 401; i++) {
        for (j = 0; j < 401; j++) {
            world[i][j] = static_cast<terrainMap*>(malloc(sizeof(*world[i][j])));
        }
    }

    for (i = 0; i < 401; i++) {
        for (j = 0; j < 401; j++) {
            world[i][j]->generated = 0;
        }
    }
    
    generateTerrain(world[currWorldRow][currWorldCol], currWorldRow, currWorldCol, 1, numTrainers);

    while(!quit) {
        if (world[currWorldRow][currWorldCol]->quit) {
            quit = 1;
        }
        if (world[currWorldRow][currWorldCol]->wantToFly) {
            int newRow = world[currWorldRow][currWorldCol]->flyRow;
            int newCol = world[currWorldRow][currWorldCol]->flyCol;
            world[currWorldRow][currWorldCol]->wantToFly = 0;
            currWorldRow = newRow;
            currWorldCol = newCol;
            if (!world[currWorldRow][currWorldCol]->generated) {
                generateTerrain(world[currWorldRow][currWorldCol], currWorldRow, currWorldCol, 0, numTrainers);
            } else {
                generateTrainers(world[currWorldRow][currWorldCol], numTrainers);
            }
        }
        switch(lastMove) {
            case Up :
                currWorldRow--;
                world[currWorldRow][currWorldCol]->worldRow = currWorldRow;
                world[currWorldRow][currWorldCol]->worldCol = currWorldCol;
                if (!world[currWorldRow][currWorldCol]->generated) {
                    lastMove = Up;
                    world[currWorldRow][currWorldCol]->pc.preset = 1;
                    generateTerrain(world[currWorldRow][currWorldCol], currWorldRow, currWorldCol, 0, numTrainers);
                } else {
                    generateTrainers(world[currWorldRow][currWorldCol], numTrainers);
                }
                break;
            case Down :
                currWorldRow++;
                world[currWorldRow][currWorldCol]->worldRow = currWorldRow;
                world[currWorldRow][currWorldCol]->worldCol = currWorldCol;
                if (!world[currWorldRow][currWorldCol]->generated) {
                    lastMove = Down;
                    world[currWorldRow][currWorldCol]->pc.preset = 1;
                    generateTerrain(world[currWorldRow][currWorldCol], currWorldRow, currWorldCol, 0, numTrainers);
                } else {
                    generateTrainers(world[currWorldRow][currWorldCol], numTrainers);
                }
                break;
            case Left :
                currWorldCol--;
                world[currWorldRow][currWorldCol]->worldRow = currWorldRow;
                world[currWorldRow][currWorldCol]->worldCol = currWorldCol;
                if (!world[currWorldRow][currWorldCol]->generated) {
                    lastMove = Left;
                    world[currWorldRow][currWorldCol]->pc.preset = 1;
                    generateTerrain(world[currWorldRow][currWorldCol], currWorldRow, currWorldCol, 0, numTrainers);
                } else {
                    generateTrainers(world[currWorldRow][currWorldCol], numTrainers);
                }
                break;
            case Right :
                currWorldCol++;
                world[currWorldRow][currWorldCol]->worldRow = currWorldRow;
                world[currWorldRow][currWorldCol]->worldCol = currWorldCol;
                if (!world[currWorldRow][currWorldCol]->generated) {
                    lastMove = Right;
                    world[currWorldRow][currWorldCol]->pc.preset = 1;
                    generateTerrain(world[currWorldRow][currWorldCol], currWorldRow, currWorldCol, 0, numTrainers);
                } else {
                    generateTrainers(world[currWorldRow][currWorldCol], numTrainers);
                }
                break;
            case None:
                break;
        }
    }

    for (i = 0; i < 401; i++) {
        for (j = 0; j < 401; j++) {
            free(world[i][j]);
        }
    }

    endwin();

    return 0;
}