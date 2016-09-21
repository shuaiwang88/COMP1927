// commonFunctions.h
// Interface for commonFuntions.c

#ifndef commonFunctions_h
#define commonFunctions_h

#include <stdio.h>
#include "Map.h"
#include "Globals.h"
#include "GameView.h"


// struct definition of Map
typedef struct vNode *VList;

struct vNode {
    LocationID  v;    // ALICANTE, etc
    TransportID type; // ROAD, RAIL, BOAT
    VList       next; // link to next node
};

struct MapRep {
    int   nV;         // #vertices
    int   nE;         // #edges
    VList connections[NUM_MAP_LOCATIONS]; // array of lists
};


// struct definition of gameView
typedef struct _playerInfo {
    int playerHealth;                      // health of each player
    LocationID playerCurrLocation;         // current loaction of each player
    LocationID playerTrail[TRAIL_SIZE];    // trail location of each player
} playerInfo;

struct gameView {
    char *pastPlays;                       // string of all plays in the past
    int gameScore;                         // current game score
    Round roundNumber;                     // current round number
    PlayerID currentPlayer;                // ID of current player
    PlayerMessage *messages;               // messages provided by players
    playerInfo *players[NUM_PLAYERS];      // array of player info
    
};


//max number of event encountered each play
#define NUM_EVENT_ENCOUNTER 3
//number of chars per round in pastPlays
#define CHARS_PER_ROUND     40
//number of chars per turn in pastPlays
#define CHARS_PER_TURN      8


// traverse the map and return an array of nearby cities of "from" with type "type"
// *nearby and *size is the original array and size of the array
// the function updates the *nearby array and size of the array
LocationID *NearbyCities(Map map, LocationID from, LocationID *nearby, int *size, int type);


// check an object is in the array
int inArray(int *array, int object, int size);


// given a Place abbreviation, return its ID number (with other "locations" included)
LocationID otherToID(char *abbrev);


// count the number of chars in a given string
int countChar(char* string);


// return the health of a hunter in a particula play
// pastPlays    : the string of that particula play
// health       : health from last round
// prevLocation : previous location of the hunter
// currLocation : current location of the hunter
int hunterTurnHealth(char *pastPlays, int health, LocationID prevLocation, LocationID currLocation);


// Update the trail with newLocation inserted
// trail : trail of a player in last 6 truns
// newLocation : the newest location to be the part of trail
void updatePlayerTrail(LocationID trail[TRAIL_SIZE], LocationID newLocation);


// return the actual location ID of Dracula if his move is a hide or double back
// currID : ID of current location
// trail  : tral of Dracula
LocationID dracSpecialLocation(LocationID currID, LocationID trail[TRAIL_SIZE]);


// initialise a given trail to UNKNOWN_LOCATION
void initialiseTrail(LocationID trail[TRAIL_SIZE]);


// check and update the number of traps and number of immature vampires in a given location
// trail : trail of a player
// c     : the char of an event encountered in a play
// where : location that you are interested in knowing the number of traps and immature vampires
void numEncounter(LocationID trail[TRAIL_SIZE], char c, LocationID where, int *numTraps, int *numVamps);


#endif /* commonFunctions_h */