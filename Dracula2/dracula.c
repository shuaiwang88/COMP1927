// dracula.c
// Implementation of your "Fury of Dracula" Dracula AI

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "Game.h"
#include "DracView.h"
#include "Places.h"
#include "Map.h"
#include "commonFunctions.h"

LocationID seaLoc[NUM_SEA] = {0, 4, 7, 10, 23, 32, 33, 43, 48, 64};
//char *furthestLocation(DracView gameState, LocationID *adjLocations,
//                                  int numLoc, PlayerID player);

// check if there is an isolated hunter
// a hunter is said to be isolated if he is in a location that is
//  unattainable by another hunter in 2 rounds
//int isIsolated(PlayerID player, DracView gameState);

//int islandEmpty(DracView gameState);

//int atLeastOneIsolated(char **location, DracView gameState); // sets location
//adjacent to most isolated player  in a positionn furthest from other players

//LocationID optimalStartLocation(DracView gameState);
//
//PlayerID mostIsolatedPlayer(DracView gameState);
void submitPlay(DracView gameState, LocationID *adjLoc, LocationID best, int isHide, PlayerMessage msg);
char *optimalLocation(DracView gameState, LocationID *adjLocations,
                      int numLoc, PlayerID player);




int locationScore(LocationID location, int length, int health);
int possibleHunters(DracView gameState, LocationID place);
LocationID furthestLocation(DracView gameState, LocationID *adjLocations,
                       int numLoc);
int islandEmpty(DracView gameState);
PlayerID mostIsolated(DracView gameState);
void locToDBOrHI(char *locAbbrev, LocationID trail[TRAIL_SIZE], LocationID hideTrail[TRAIL_SIZE]);
int isResearch(DracView gameState);
int *knownByHunter(DracView gameState, int *numLoc, LocationID trail[TRAIL_SIZE]);
LocationID predictHunterMove(DracView gameState, PlayerID hunter);
LocationID predictDracMove(DracView gameState, LocationID dracSrc, Round round);
int numOfNextAdj(DracView gameState, LocationID dest);
int numOfPossHunter(DracView gameState, LocationID dest);

int numOfHunter(DracView gameState, LocationID location);
int isBeingKilled(DracView gameState);
int isSurrounded(DracView gameState, LocationID *adjLoc, int numLoc);
int surroundedNextToSea(DracView gameState, LocationID *adjLoc, int numLoc);
int nextToHunter(DracView gameState, PlayerID player, LocationID *adjLoc, int numLoc);
LocationID lastStand(DracView gameState, LocationID *adjLoc, int numLoc);

int distanceFromSea(DracView gameState, LocationID location);
int distFromNearestHunter(DracView gameState, LocationID location);
LocationID stayNearSea(DracView gameState, LocationID *adjLoc, int numLoc);
int shouldStayNearSea(DracView gameState);
int canEndAtSea(DracView gameState);

int isCornered(DracView gameState, LocationID currLoc, LocationID trail[TRAIL_SIZE]);

void decideDraculaMove(DracView gameState)
{
//    char *location;
//    LocationID loc;
char *l0[11] = {"GA", "BC", "BD", "SO", "VR", "BS", "IO", "TS", "GO", "MR", "PA"};
char *l1[11] = {"NP", "FL", "ST", "BU", "PA", "BO", "MA", "AL", "BA", "BO", "ZA"};
char *l2[11] = {"SZ", "VE", "MU", "ZU", "FR", "ST", "PA", "BO", "NA", "CF", "PA"};
char *l3[11] = {"LO", "EC", "LE", "BO", "AL", "MA", "CA", "AO", "BB", "BO", "PA"};
char *d[11] = {"AL", "SR", "MA", "MA", "SN", "SN", "BB", "NA", "CF", "PA", "PA"};
int b;
printf("player0: ");
for (b=0;b<11;b++) {
  printf("%s-> ", idToName(abbrevToID(l0[b])));
}
printf("\n");
printf("player1: ");
for (b=0;b<11;b++) {

  printf("%s-> ", idToName(abbrevToID(l1[b])));

}
  printf("\n");
printf("player2: ");
for (b=0;b<11;b++) {

  printf("%s-> ", idToName(abbrevToID(l2[b])));

}
printf("\n");
printf("player3: ");
for (b=0;b<11;b++) {

  printf("%s-> ", idToName(abbrevToID(l3[b])));

}
printf("\n");
printf("dracula: ");
for (b=0;b<11;b++) {

  printf("%s-> ", idToName(abbrevToID(d[b])));

}
printf("\n");


    LocationID trail[TRAIL_SIZE];
LocationID trail2[TRAIL_SIZE];
    giveMeTheTrail(gameState, PLAYER_DRACULA, trail, FALSE);
    int y, i;
    for (y = 0 ; y < TRAIL_SIZE ; y++) {
        printf("trail[%d] = %d\n", y, trail[y]);
    }
giveMeTheTrail(gameState, PLAYER_DRACULA, trail2, TRUE);
    for (y = 0 ; y < TRAIL_SIZE ; y++) {
        printf("hideTrail[%d] = %d\n", y, trail2[y]);
    }

    int numLoc = 0;
    LocationID *adjLoc = NULL;
    LocationID *path = calloc(NUM_MAP_LOCATIONS, sizeof(LocationID));
    LocationID x, currLoc;
    Round round = giveMeTheRound(gameState);
    PlayerID hunter;
    findPathLength(13, 62, 0, 0, path);
    if (round == 0) {
        PlayerID mostIsolatedHunter = mostIsolated(gameState);
        if (islandEmpty(gameState)) {
            registerBestPlay("ED", "first move");
        } else if (mostIsolatedHunter != -1) {
            LocationID from = whereIs(gameState, mostIsolatedHunter);
            for (x = MIN_MAP_LOCATION; x < NUM_MAP_LOCATIONS; x++) {
                if (findPathLength(from, x, mostIsolatedHunter, 1, path) == 2) {
                    numLoc++;
                    if (adjLoc != NULL) {
                        adjLoc = realloc(adjLoc, numLoc * sizeof(LocationID));
                        assert(adjLoc != NULL);
                        adjLoc[numLoc - 1] = x;
                    } else {
                        adjLoc = malloc(numLoc * sizeof(LocationID));
                        assert(adjLoc != NULL);
                        adjLoc[numLoc - 1] = x;
                    }
                }
            }
            currLoc = furthestLocation(gameState, adjLoc, numLoc);
            submitPlay(gameState, adjLoc, currLoc, FALSE, "first move");
        } else {
            numLoc = NUM_MAP_LOCATIONS;
            adjLoc = malloc(numLoc * sizeof(LocationID));
            for (x = MIN_MAP_LOCATION; x < NUM_MAP_LOCATIONS; x++) adjLoc[x] = x;
            currLoc = furthestLocation(gameState, adjLoc, numLoc);
            submitPlay(gameState, adjLoc, currLoc, FALSE, "first move");
        }
        return;
    } else {
        adjLoc = whereCanIgo(gameState, &numLoc, TRUE, TRUE);
        printf("Where am i %s\n", idToName(whereIs(gameState, PLAYER_DRACULA)));

        for (y = 0 ; y < numLoc ; y++) {
            printf("adjLocations[%d] = %s\n", y, idToName(adjLoc[y]));
        }
        if (numLoc == 0) {
            registerBestPlay("TP", "come to CD and catch me!");
            return;
        }
        int health = howHealthyIs(gameState, PLAYER_DRACULA);
        int count = 0;
        if (health <= 2) {
            for (x = 0; x < numLoc; x++) {
                if (idToType(adjLoc[x]) != SEA) {
                    currLoc = adjLoc[x];
                    count++;
                }
            }
            if (count == 1) {
                submitPlay(gameState, adjLoc, currLoc, TRUE, "i hate water");
                return;
            } else if (count > 1) {
                for (x = 0; x < numLoc; x++) {
                    if (idToType(adjLoc[x]) == SEA) {
                        shiftLeft(adjLoc, x, numLoc);
                        numLoc--;
                    }
                }
            }
        }

        // if (shouldStayNearSea(gameState)) {
        //   currLoc = stayNearSea(gameState, adjLoc, numLoc);
        //   submitPlay(gameState, adjLoc, currLoc, TRUE, "camp near sea");
        // }

        for (i = 0 ; i < numLoc ; i++) {
            if (possibleHunters(gameState, adjLoc[i]) == 1) {
                for (hunter = PLAYER_LORD_GODALMING ; hunter <= PLAYER_MINA_HARKER; hunter++) {
                    if (whereIs(gameState, hunter) == adjLoc[i]) {
                         if ((howHealthyIs(gameState, hunter) < 2) && (round % 13 != 0)) {
                            submitPlay(gameState, adjLoc, adjLoc[i], TRUE, "kill a hunter");
                            return;
                         }
                    }
                }
            }
        }
        /*int notSurrounded = 0;
        for (i = 0 ; i < numLoc ; i++) {
            if (possibleHunters(gameState, adjLoc[i]) == 0) notSurrounded = 1;
        }
        if (notSurrounded != 1) {

        }
        if (isDangerous()) {
            for (i = 0; i < NUM_SEA; i++) {
                length = findPathLength(whereIs(gameState, PLAYER_DRACULA), seaLoc[i], hunter, round+1, path);
                if (length < minLength) {
                    minLength = length;
                    if (length == 0) currLoc = path[0];
                    else currLoc = path[1];
                }
            }
            submitPlay(gameState, adjLoc, currLoc, TRUE, "catch me in sea!");
        }*/

        //LocationID predictHunterMove(DracView gameState, PlayerID hunter);
        //for (hunter = PLAYER_LORD_GODALMING ; hunter <= PLAYER_MINA_HARKER; hunter++) {
            //predictLoc = predictHunterMove(DracView gameState, PlayerID hunter);

          /*  if (isBeingKilled(gameState)) {
                      adjLoc = whereCanIgo(gameState, &numLoc, TRUE, TRUE);
                      currLoc = lastStand(gameState, adjLoc, numLoc);
                      printf("last Stand %s\n", idToName(currLoc));
                      submitPlay(gameState, adjLoc, currLoc, TRUE, "other moves");
                      return;
                    }*/

        currLoc = furthestLocation(gameState, adjLoc, numLoc);
//int location = abbrevToID(locAbbrev);
printf("best play %s\n", idToName(currLoc));
        submitPlay(gameState, adjLoc, currLoc, TRUE, "other moves");
        return;
    }
}

// register the best play to the game engine
void submitPlay(DracView gameState, LocationID *adjLoc, LocationID best, int isHide, PlayerMessage msg) {
    LocationID trail[TRAIL_SIZE];
    LocationID hideTrail[TRAIL_SIZE];
    giveMeTheTrail(gameState, PLAYER_DRACULA, trail, FALSE);
    giveMeTheTrail(gameState, PLAYER_DRACULA, hideTrail, TRUE);
    char *locAbbrev = malloc(3 * sizeof(char));
    idToAbbrev(best, locAbbrev);
    if (isHide == TRUE) locToDBOrHI(locAbbrev, trail, hideTrail);
    registerBestPlay(locAbbrev, msg);
    free(locAbbrev);
    free(adjLoc);
    return;
}

int optimalLocScore(int numOfNextAdj, int pathLength, int possHunt) {
  int score = pathLength * 9 + numOfNextAdj * 3 - possHunt * 18;
  return score;
//    int otherHunters;
//    char *location;
//    int total = -1;
//    int tempTotal = 0;
//    PQueue pq = newPQueue();
//    int x;
//    for (x = 0 ; x < numLoc ; x++) {
//        tempTotal = 0;
//        for (otherHunters = 0; otherHunters < 4; otherHunters++) {
//            if (player != otherHunters){
//                PQueueJoin(pq, adjLocations[x], findPathLength(adjLocations[x],
//                                                               whereIs(gameState, otherHunters)));
//            }
//        }
//        int order = 0; //adjust weighting here
//        while(!PQueueIsEmpty(pq)) {
//            if (order == 0) tempTotal += PQueueLeave(pq) * 40; //weighing 1
//            if (order == 1) tempTotal += PQueueLeave(pq) * 30; //weighing 2
//            if (order == 2) tempTotal += PQueueLeave(pq) * 20; //weighing 3
//            if (order == 3) tempTotal += PQueueLeave(pq) * 10; //weighing 4
//            order++;
//        }
//        if (total > tempTotal || total == -1) {
//            total = tempTotal;
//            location = idToName(adjLocations[x]);
//            printf("tempTotal: %d\n", tempTotal);
//            printf("location: %s\n", location);
//        }
//    }
//    return location;
}

// higher score if the place suits dracula more
int locationScore(LocationID location, int length, int health) {
    Map map = newMap();
    int nRail = 0;        // count the number of rail connections of a place
    int nRoad = 0;        // count the number of road connections of a place
    int nBoat = 0;        // count the number of baot connections of a place
    int typeScore = 0;
    int locScore = 0;
    VList cur = map->connections[location];

    for (; cur != NULL; cur = cur->next) {
        if (cur->type == RAIL || cur->type == ANY)
            nRail++;
        if (cur->type == ROAD || cur->type == ANY)
            nRoad++;
        if (cur->type == BOAT || cur->type == ANY)
            nBoat++;
    }
    if (idToType(location) == SEA) {
        typeScore = health/2;
        if (health <= 12) typeScore = -40;
        if (health <= 2) typeScore = -100;
    } else if (idToType(location) == LAND) {
        typeScore = 62;
    }

    locScore = (length * 10) + typeScore + nRoad * 2 + nBoat - nRail;
    //printf("locScore = %d\n", locScore);
    disposeMap(map);
    return locScore;
}

// count the number of hunters that are able go to the given place next round
int possibleHunters(DracView gameState, LocationID place) {
    PlayerID hunter;
    int numLoc;
    int count = 0;
    Round round = giveMeTheRound(gameState) + 1;

    if (idToType(place) == SEA) return 0;
    for (hunter = PLAYER_LORD_GODALMING ; hunter <= PLAYER_MINA_HARKER; hunter++) {
        LocationID *futureMove = whereCanTheyGo(gameState, &numLoc,
                                                whereIs(gameState, hunter),
                                                hunter, round, TRUE, TRUE, TRUE);
        if (inArray(futureMove, place, numLoc) != -1)
            count++;
    }
    return count;
}

// find the adjacent location which is furthest away from all the hunters
LocationID furthestLocation(DracView gameState, LocationID *adjLocations,
                       int numLoc) {
    PlayerID hunter;
    LocationID location;
    //char *abbrev = malloc(3 * sizeof(char));
    Round round = giveMeTheRound(gameState);
    int length;
    int nearestLength = MAX_MAP_LOCATION;
    int maxLength = -1;
    int tmpScore;
    int locScore = 0;
    int possHunters;
    int minPossHunters = 5;
    LocationID *path = calloc(NUM_MAP_LOCATIONS, sizeof(LocationID));
    int x;

    for (x = 0 ; x < numLoc; x++) {
      printf ("fur adjLocations[%d] = %s\n", x, idToName(adjLocations[x]));
        possHunters = possibleHunters(gameState, adjLocations[x]);
        for (hunter = PLAYER_LORD_GODALMING; hunter <= PLAYER_MINA_HARKER; hunter++) {
            length = findPathLength(adjLocations[x], whereIs(gameState, hunter), hunter, round+1, path);
            //printf("src = %s, dest = %s, round = %d, player = %d, length = %d\n", idToName(adjLocations[x]), idToName(whereIs(gameState, hunter)), round, hunter, length);
            if (length < nearestLength) nearestLength = length;
        }
        if (possHunters < minPossHunters) {
            maxLength = nearestLength;
            location = adjLocations[x];
            minPossHunters = possHunters;
        } else if ((possHunters == minPossHunters) && (nearestLength > maxLength)) {
            if ((round % 13 == 0) && (idToType(adjLocations[x]) == SEA) &&
                (maxLength > 2)) {
            } else if ((idToType(adjLocations[x]) == SEA) && (maxLength > 3)) {
            } else {
                maxLength = nearestLength;
                location = adjLocations[x];
            }
        } else if ((possHunters == minPossHunters) && (nearestLength == maxLength)) {
            if ((round % 13 == 0) && (idToType(adjLocations[x]) == SEA) &&
                (maxLength > 2)) {
            } else {
                tmpScore = locationScore(adjLocations[x], nearestLength, howHealthyIs(gameState, PLAYER_DRACULA));
                if (tmpScore > locScore) {
                    location = adjLocations[x];
                    locScore = tmpScore;
                }
            }
        }
    }
    free(path);
    //idToAbbrev(location, abbrev);
    return location;
}

// check whether the largest island is occupied by a hunter
int islandEmpty(DracView gameState) {
    PlayerID player;
    LocationID location;

    for (player = PLAYER_LORD_GODALMING ; player <= PLAYER_MINA_HARKER; player++) {
        location = whereIs(gameState, player);
        if (location == EDINBURGH || location == MANCHESTER ||
            location == LIVERPOOL || location == SWANSEA	||
            location == LONDON || location == PLYMOUTH)
            return 0;
    }
    return 1;
}

// find the most isolated hunter, return -1 if no isolated hunter is found
PlayerID mostIsolated(DracView gameState) {
    int distance = 0;
    int length, nearest = MAX_MAP_LOCATION;
    PlayerID player, hunter;
    PlayerID mostIsolatedHunter = -1;
    LocationID *path = calloc(NUM_MAP_LOCATIONS, sizeof(LocationID));

    for (player = PLAYER_LORD_GODALMING; player <= PLAYER_MINA_HARKER; player++) {
        for (hunter = PLAYER_LORD_GODALMING; hunter <= PLAYER_MINA_HARKER; hunter++) {
            if (hunter != player) {
                length = findPathLength(whereIs(gameState, hunter),
                                        whereIs(gameState, player),
                                        hunter, giveMeTheRound(gameState) + 1, path);
                if (length < nearest)
                    nearest = length;
            }
        }
        if (nearest > distance) {
            distance = nearest;
            mostIsolatedHunter = player;
        }
    }
    free(path);
    if (distance <= 2) return -1;
    return mostIsolatedHunter;
}

void locToDBOrHI(char *locAbbrev, LocationID trail[TRAIL_SIZE], LocationID hideTrail[TRAIL_SIZE]) {
    shiftRight(trail, 0, TRAIL_SIZE - 1);
    shiftRight(hideTrail, 0, TRAIL_SIZE - 1);
    trail[0] = abbrevToID(locAbbrev);
    hideTrail[0] = abbrevToID(locAbbrev);
    int dbOrHi = hasDBOrHI(hideTrail);

    if (dbOrHi == BOTH_HIDE_AND_DB) return;
    if (trail[0] == trail[1] &&
        (dbOrHi != HAS_HIDE) &&
        idToType(trail[0]) != SEA) {
        strcpy(locAbbrev, "HI");
        hideTrail[0] = HIDE;
    } else if (dbOrHi != HAS_DOUBLE_BACK) {
        if (trail[0] == trail[1]) {
            strcpy(locAbbrev, "D1");
            hideTrail[0] = DOUBLE_BACK_1;
        } else if (trail[0] == trail[2]) {
            strcpy(locAbbrev, "D2");
            hideTrail[0] = DOUBLE_BACK_2;
        } else if (trail[0] == trail[3]) {
            strcpy(locAbbrev, "D3");
            hideTrail[0] = DOUBLE_BACK_3;
        } else if (trail[0] == trail[4]) {
            strcpy(locAbbrev, "D4");
            hideTrail[0] = DOUBLE_BACK_4;
        } else if (trail[0] == trail[5]) {
            strcpy(locAbbrev, "D5");
            hideTrail[0] = DOUBLE_BACK_5;
        }
    }
    return;
}

// check if the hunter is doing a research
int isResearch(DracView gameState) {
    PlayerID hunter;
    int count = 0;
    LocationID start, end;

    for (hunter = PLAYER_LORD_GODALMING; hunter <= PLAYER_MINA_HARKER; hunter++) {
        lastMove(gameState, hunter, &start, &end);
        if (start == end ) count++;
    }
    if (count == NUM_PLAYERS - 1) return 1;
    else return 0;
}

// check which location in dracula's trail is known by the hunters
// return array of trail indexes of that know locations
int *knownByHunter(DracView gameState, int *numLoc, LocationID trail[TRAIL_SIZE]) {
    PlayerID hunter;
    *numLoc = 0;
    int *knownLoc = malloc(TRAIL_SIZE * sizeof(LocationID));
    int i;
    for (i = 0; i < TRAIL_SIZE; i++) {
        for (hunter = PLAYER_LORD_GODALMING; hunter <= PLAYER_MINA_HARKER; hunter++) {
            if (whereIs(gameState, hunter) == trail[i]) {
                knownLoc[*numLoc] = i;
                (*numLoc)++;
                break;
            }
        }
    }
    for (i=0;i<*numLoc;i++) printf("known=trail[%d]", knownLoc[i]);
    return knownLoc;
}

// to predict the moves of hunter in next round when one of the dracula's location is known
LocationID predictHunterMove(DracView gameState, PlayerID hunter) {
    LocationID trail[TRAIL_SIZE];
    giveMeTheTrail(gameState, PLAYER_DRACULA, trail, FALSE);
    int numLoc, length;
    int *knownLoc = knownByHunter(gameState, &numLoc, trail);
    Round currRound = giveMeTheRound(gameState);
    LocationID dracMove = predictDracMove(gameState, trail[knownLoc[0]], currRound - knownLoc[0]);
    LocationID *predictPath = calloc(NUM_MAP_LOCATIONS, sizeof(LocationID));
    LocationID huntMove;
    int i;

    for (i = 1; i <= knownLoc[0]; i++) {
        dracMove = predictDracMove(gameState, dracMove, currRound - knownLoc[0] + i);
    }
    length = findPathLength(whereIs(gameState, hunter), dracMove, hunter, currRound + 1, predictPath);
    if (length > 0) huntMove = predictPath[1];
    else huntMove = predictPath[0];
    free(knownLoc);
    free(predictPath);
    return huntMove;
}

// Dracula version of perdict Dracula move
// to predict the moves of dracula in next few round when one of the dracula's location is known
// predict where dracula will go in round round
LocationID predictDracMove(DracView gameState, LocationID dracSrc, Round round) {
    PlayerID hunter;
    LocationID location;
    int length;
    int nearestLength = MAX_MAP_LOCATION;
    int maxLength = -1;
    LocationID *path = calloc(NUM_MAP_LOCATIONS, sizeof(LocationID));
    LocationID hunterTrail[TRAIL_SIZE];
    int numLoc;
    LocationID *adjLoc = whereCanTheyGo(gameState, &numLoc,
                               dracSrc, PLAYER_DRACULA, round,
                               TRUE, FALSE, TRUE);
    Round currRound = giveMeTheRound(gameState);
    int x;

    for (x = 0 ; x < numLoc ; x++) {
        for (hunter = PLAYER_LORD_GODALMING; hunter <= PLAYER_MINA_HARKER; hunter++) {
            giveMeTheTrail(gameState, hunter, hunterTrail, FALSE);
            length = findPathLength(adjLoc[x], hunterTrail[currRound - round], hunter, round+1, path);
            if (length < nearestLength) nearestLength = length;
        }
        if (nearestLength > maxLength) {
            maxLength = nearestLength;
            location = adjLoc[x];
        } else if (nearestLength == maxLength) {
            if (idToType(x) != SEA) {
                location = adjLoc[x];
            }
        }
    }
    free(path);
    return location;
}

int isDangerous(DracView gameState, LocationID currLoc, LocationID trail[TRAIL_SIZE]) {
    int knownNumLoc;
    int *knownLoc = knownByHunter(gameState, &knownNumLoc, trail);
    LocationID *path = calloc(NUM_MAP_LOCATIONS, sizeof(LocationID));
    Round round = giveMeTheRound(gameState);
    PlayerID hunter;

    if (knownLoc[0] < 3) {
        for (hunter = PLAYER_LORD_GODALMING; hunter <= PLAYER_MINA_HARKER; hunter++) {
            if (findPathLength(currLoc, whereIs(gameState, hunter), hunter, round+1, path) < 2) {
                free(knownLoc);
                free(path);
                return 1;
            }
        }
    }
    free(knownLoc);
    free(path);
    return 0;
}

int numOfHunter(DracView gameState, LocationID location) {
  int count = 0;
  PlayerID hunter;
  for (hunter = PLAYER_LORD_GODALMING; hunter <= PLAYER_MINA_HARKER; hunter++) {
    if (location == whereIs(gameState, hunter)) count++;
  }
  return count;
}

int isSurrounded(DracView gameState, LocationID *adjLoc, int numLoc) {
  //check if drac is surrounded with hunters in adjacent locations excluding his current location
  int x;
  int atLeastOne = 1;
  for (x = 0 ; x < numLoc ; x++) {
     if (numOfHunter(gameState, adjLoc[x]) == 0 && adjLoc[x] != whereIs(gameState,
          PLAYER_DRACULA)){
        atLeastOne = 0;
     }
  }
  return atLeastOne;
}

int surroundedNextToSea(DracView gameState, LocationID *adjLoc, int numLoc) {
  //check if drac is surrounded with hunters in adjacent locations excluding his current location
  int x;
  int atLeastOne = 1;
  int nextToSea = 0;
  for (x = 0 ; x < numLoc ; x++) {
     if (numOfHunter(gameState, adjLoc[x]) == 0 && idToType(adjLoc[x]) != SEA) {
       atLeastOne = 0;
     }
     if (idToType(adjLoc[x]) == SEA) {
       nextToSea = 1;
     }
  }
  if (atLeastOne && nextToSea) return 1;
  return 0;
}

int isBeingKilled(DracView gameState) {
  int numLoc;
  LocationID *adjLoc = whereCanIgo(gameState, &numLoc, TRUE, TRUE);
  int x;
  int hunter;
  int numNearby=0;
  for (hunter = PLAYER_LORD_GODALMING; hunter <= PLAYER_MINA_HARKER; hunter++) {
    for (x = 0; x < numLoc ; x++) {
      if (whereIs(gameState, hunter) == adjLoc[x]) numNearby++;
    }
  }
  if (isSurrounded(gameState, adjLoc, numLoc) && numNearby*10 >= howHealthyIs(gameState, PLAYER_DRACULA)) return 1;
  if (surroundedNextToSea(gameState, adjLoc, numLoc) && howHealthyIs(gameState,
      PLAYER_DRACULA) <= 2 ) {
        if (numNearby*10 >= howHealthyIs(gameState, PLAYER_DRACULA)) return 1;
  }
  return 0;
}

int nextToHunter(DracView gameState, PlayerID player, LocationID *adjLoc, int numLoc) {
  int x;
  for (x = 0 ; x < numLoc; x++) {
    if (whereIs(gameState, player) == adjLoc[x]) return 1;
  }
  return 0;
}

LocationID lastStand(DracView gameState, LocationID *adjLoc, int numLoc) {
  int hunter;
  for (hunter = PLAYER_LORD_GODALMING; hunter <= PLAYER_MINA_HARKER; hunter++) {
    if (nextToHunter(gameState, hunter, adjLoc, numLoc) && howHealthyIs(gameState, hunter) <= 6) {
      return whereIs(gameState, hunter);
    }
  }
  return furthestLocation(gameState, adjLoc, numLoc);
}

// count the number of possible move next round if Dracula choose dest as a location to go
int numOfNextAdj(DracView gameState, LocationID dest) {
  int numLoc;
  char *locAbbrev = malloc(3 * sizeof(char));
  idToAbbrev(dest, locAbbrev);
  LocationID hideTrail[TRAIL_SIZE];
  LocationID trail[TRAIL_SIZE];
  giveMeTheTrail(gameState, PLAYER_DRACULA,
                 hideTrail, TRUE);
  giveMeTheTrail(gameState, PLAYER_DRACULA,
                 trail, FALSE);
  locToDBOrHI(locAbbrev, trail, hideTrail);
  whereCanDracgo(gameState, &numLoc, trail,
                  hideTrail, dest, TRUE, TRUE);
  return numLoc;
}

// count the number of hunters that will possibly go to dest in the round
int numOfPossHunter(DracView gameState, LocationID dest) {
  PlayerID hunter;
  int count = 0;
  for (hunter = PLAYER_LORD_GODALMING; hunter <= PLAYER_MINA_HARKER; hunter++) {
    if (dest == predictHunterMove(gameState, hunter)) count++;
  }
  return count;
}

// check if dracula and hunters are in the same city
int meetHunter(DracView gameState) {
  LocationID trail[TRAIL_SIZE];
  giveMeTheTrail(gameState, PLAYER_DRACULA, trail, FALSE);
  int numLoc;
  int *knownLoc = knownByHunter(gameState, &numLoc, trail);
  if (knownLoc[0] == 0) return 1;
  return 0;
}

// count the number of adjLoc that would not result in HI or DB
int needHiding(DracView gameState, LocationID location) {
  char *locAbbrev = malloc(3 * sizeof(char));
  LocationID hideTrail[TRAIL_SIZE];
  LocationID trail[TRAIL_SIZE];
    idToAbbrev(location, locAbbrev);
    giveMeTheTrail(gameState, PLAYER_DRACULA,
                   hideTrail, TRUE);
    giveMeTheTrail(gameState, PLAYER_DRACULA,
                   trail, FALSE);
    locToDBOrHI(locAbbrev, trail, hideTrail);
    if (hideTrail[0] > MAX_MAP_LOCATION) return 1;
  return 0;
}

//
int ewfhwef(DracView gameState, int numLoc, LocationID *adjLoc) {
  int i, count = 0;
  if (meetHunter(gameState)) {


    for (i = 0; i < numLoc; i++) {
        if (!needHiding(gameState, adjLoc[i])) {
          if (numOfHunter(gameState, adjLoc[i]) > 0)
          count++;
        }
    }
    if (count > 0) {}
  }
  return 0;
}

// find the shortest path to escape to sea and return the next location in that path
LocationID escapeToSea(DracView gameState) {
  int i, length, minLength = NUM_MAP_LOCATIONS;
  LocationID *path = calloc(NUM_MAP_LOCATIONS, sizeof(LocationID));
  LocationID currLoc;
  for (i = 0; i < NUM_SEA; i++) {
      length = findPathLength(whereIs(gameState, PLAYER_DRACULA), seaLoc[i], PLAYER_DRACULA, 0, path);
      if (length < minLength) {
          minLength = length;
          if (length == 0) currLoc = path[0];
          else currLoc = path[1];
      }
  }
  free(path);
  return currLoc;
}

int distFromSea(DracView gameState, LocationID location) {
  int i, length, minLength = NUM_MAP_LOCATIONS;
  LocationID *path = calloc(NUM_MAP_LOCATIONS, sizeof(LocationID));
  Round round = giveMeTheRound(gameState);
  for (i = 0; i < NUM_SEA; i++) {
      length = findPathLength(location, seaLoc[i], PLAYER_DRACULA, round + 1, path);
      if (length < minLength) minLength = length;
  }
  free(path);
  return minLength;
}

int distFromNearestHunter(DracView gameState, LocationID location) {
  int hunter, length, minLength = NUM_MAP_LOCATIONS;
  LocationID *path = calloc(NUM_MAP_LOCATIONS, sizeof(LocationID));
  Round round = giveMeTheRound(gameState);
  for (hunter = PLAYER_LORD_GODALMING; hunter <= PLAYER_MINA_HARKER ; hunter++) {
      length = findPathLength(location, whereIs(gameState, hunter), hunter, round + 1, path);
      if (length < minLength) minLength = length;
  }
  free(path);
  return minLength;
}

LocationID stayNearSea(DracView gameState, LocationID *adjLoc, int numLoc) {
  LocationID bestMove = -1;
  int x;
  for (x = 0; x < numLoc; x++) {
    if (idToType(adjLoc[x]) != SEA) {
      if (bestMove == -1 || distFromSea(gameState, adjLoc[x]) < distFromSea(gameState, bestMove)) {
        bestMove = adjLoc[x];
      } else if (distFromSea(gameState, adjLoc[x]) == distFromSea(gameState, bestMove)) {
        if (distFromNearestHunter(gameState, adjLoc[x]) > distFromNearestHunter(gameState, bestMove)) bestMove = adjLoc[x];
      }
    }
  }
  if (bestMove == -1) bestMove = furthestLocation(gameState, adjLoc, numLoc);
  return bestMove;
}

int shouldStayNearSea(DracView gameState) {
  if (distFromNearestHunter(gameState, whereIs(gameState, PLAYER_DRACULA)) >= 4) return 1;
  return 0;
}

int canEndAtSea(DracView gameState) {
  if (howHealthyIs(gameState, PLAYER_DRACULA) % 2 == 0) {
    if (howHealthyIs(gameState, PLAYER_DRACULA)/2  >= giveMeTheScore(gameState) - 1) return 1;
  } else {
    if (howHealthyIs(gameState, PLAYER_DRACULA)/2 + 1  >= giveMeTheScore(gameState) - 1) return 1;
  }
  return 0;
}

int isCornered(DracView gameState, LocationID currLoc, LocationID trail[TRAIL_SIZE]) {
  int knownNumLoc;
  int *knownLoc = knownByHunter(gameState, &knownNumLoc, trail);
  LocationID *path = calloc(NUM_MAP_LOCATIONS, sizeof(LocationID));
  Round round = giveMeTheRound(gameState);
  PlayerID hunter;

  if (knownLoc[0] < 3) {
      for (hunter = PLAYER_LORD_GODALMING; hunter <= PLAYER_MINA_HARKER; hunter++) {
          if (findPathLength(currLoc, whereIs(gameState, hunter), hunter, round+1, path) < 2) {
              free(knownLoc);
              free(path);
              return 1;
          }
      }

  }

  free(knownLoc);
  free(path);
  return 0;
}
