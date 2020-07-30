////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// GameView.c: GameView ADT implementation
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10   v3.0    Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "Game.h"
#include "GameView.h"
#include "Map.h"
#include "Places.h"

#include "Queue.h"
#include "ctype.h"
// add your own #includes here

#define MAX_LENGTH 14680
#define PLAY_LENGTH 7
// TODO: ADD YOUR OWN STRUCTS HERE
static int getHunterHealth(GameView gv, int player);
static int getDraculaHealth(GameView gv);
static void initialiseHealth (GameView gv);
void initialiseLocation(GameView gv);
static PlaceId GvGetPlayerLocationcall(GameView gv, Player player);
static PlaceId GvGetVampireLocationcall(GameView gv);

typedef struct _playerData{
    int health;
    Queue trail; //created first in.
}playerData;

struct gameView {
    char play[MAX_LENGTH];
    int turn;
    int score;
    int flag;
    playerData players[NUM_PLAYERS];
};

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

GameView GvNew(char *pastPlays, Message messages[])
{
	int player;
    // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	GameView new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate GameView!\n");
		exit(EXIT_FAILURE);
	}
    new->turn = (strlen(pastPlays) + 1)/PLAY_LENGTH;
    // the flag is used to prevent the program from re-calculating the HP of
    // the player again, as it will mess up the values
    // this is included to pass the automatic test
    new->flag = 0;
    initialiseHealth(new);
    for (player = PLAYER_LORD_GODALMING; player < NUM_PLAYERS; player++) {
        new->players[player].health = GvGetHealth(new, player);
        new->players[player].trail = newQueue();
    }
    initialiseLocation(new);
    // get the game score by calling the function getScore
    new->score = GvGetScore(new);
    // since the hp is now updated for every player
    // set the flag so that the program does not recalculate the hp again
    // this is included to pass the automatic test
    new->flag = 1;
    return new;
}

void GvFree(GameView gv)
{
	int player;
    
    for (player = PLAYER_LORD_GODALMING; player < NUM_PLAYERS; player++) {
        disposeQueue(gv->players[player].trail);
    }
	free(gv);

}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round GvGetRound(GameView gv)
{
	int round;
    // divide by 5 to see the current round
    // note: round =/= turn
    round = gv->turn / NUM_PLAYERS;
	return round;
}

Player GvGetPlayer(GameView gv)
{
	int player, playerID;
    
    // mod 5 to see the current player
    player = gv->turn % NUM_PLAYERS;
    
    switch (player) {
        case 1: playerID = PLAYER_DR_SEWARD; 	break;
        case 2: playerID = PLAYER_VAN_HELSING;	break;
        case 3:	playerID = PLAYER_MINA_HARKER;	break;
        case 4: playerID = PLAYER_DRACULA; 		break;
        default: playerID = PLAYER_LORD_GODALMING;
    }
    
    return playerID;
}

int GvGetScore(GameView gv)
{
	int score, turn;
    int placeID, player;
    int vampire = 0;
    int round = 0;
    int i, j;
    char play[PLAY_LENGTH + 1], location[3];
    
    // if flag = 1, don't do any calculation
    // just return the value
    if (gv->flag) return gv->score;
    
    turn = gv->turn;
    
    // go through the entire pastPlays from turn 0 to current turn
    // if score == 0, break the loop
    // if it's end of Dracula turn, score loss Dracula turn
    // if player != Dracula && current location = hospital, score loss hospital
    // if round % 13 == 0 & move[4] is 'V' & player = Dracula, set vampire = 1
    // if turn != 0, round = roundDown(turn/5)
    // if player != Dracula, check for their encounter
    //		if encounter contains 'V', set vampire = 0
    // if round = 6, 19, 32, etc.. & player = vampire & vampire = 1
    // 		score loss vampire matures
    //		set vampire = 0
    for (i = 0; i <= turn; i++) {
        memset (play, '\0', sizeof(play));
        strncpy (play, gv->play + i*PLAY_LENGTH, 8);
        memset (location, '\0', sizeof(location));
        strncpy (location, play + 1, 2);
        placeID = placeAbbrevToId(location);
        player = i%NUM_PLAYERS;
        
        if (score == 0) break;
        
        if (i <= 5) {
            score = GAME_START_SCORE;
        }
        if (turn != 0 && player == 0) {
            score -= SCORE_LOSS_DRACULA_TURN;
        }
        /////Little problem
        if (player != PLAYER_DRACULA && placeID == HOSPITAL_PLACE) {
            score -= SCORE_LOSS_HUNTER_HOSPITAL;
        }
        if (round%13 == 0 && play[4] == 'V' && player == PLAYER_DRACULA) {
            vampire = 1;
        }
        if (turn != 0) {
            round = i/NUM_PLAYERS;
        }
        if (i != PLAYER_DRACULA) {
            for (j = 3; j < PLAY_LENGTH; j++) {
                if (strlen(play) == 0) break;
                if (play[j] == '.') break;
                else if (play[j] == 'V') vampire = 0;
            }
        }
        
        if (((round-6)%13) == 0 && player == PLAYER_DRACULA && vampire == 1) {
            score -= SCORE_LOSS_VAMPIRE_MATURES;
            vampire = 0;
        }
    }
    
    return score;
}

int GvGetHealth(GameView gv, Player player)
{
	int health;
    
    // if flag variable is set, do not go through the function again
    // instead just return the value from the struct
    // this is included to pass automatic test
    if (gv->flag) return gv->players[player].health;
    
    // else call the functions to calculate the current health
    // of the desired player
    if (player == PLAYER_DRACULA) {
        health = getDraculaHealth(gv);
    } else {
        health = getHunterHealth(gv, player);
    }
    
    return health;
}

PlaceId GvGetPlayerLocation(GameView gv, Player player)
{
	int PlaceId;
    PlaceId = GvGetPlayerLocationcall(gv, player);
	return PlaceId;
}

PlaceId GvGetVampireLocation(GameView gv)
{
	int placeID;
    placeID = GvGetVampireLocationcall(gv);
    return placeID;
}

PlaceId *GvGetTrapLocations(GameView gv, int *numTraps)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numTraps = 0;
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Game History

PlaceId *GvGetMoveHistory(GameView gv, Player player,
                          int *numReturnedMoves, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedMoves = 0;
	*canFree = false;
	return NULL;
}

PlaceId *GvGetLastMoves(GameView gv, Player player, int numMoves,
                        int *numReturnedMoves, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedMoves = 0;
	*canFree = false;
	return NULL;
}

PlaceId *GvGetLocationHistory(GameView gv, Player player,
                              int *numReturnedLocs, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	*canFree = false;
	return NULL;
}

PlaceId *GvGetLastLocations(GameView gv, Player player, int numLocs,
                            int *numReturnedLocs, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	*canFree = false;
	return 0;
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *GvGetReachable(GameView gv, Player player, Round round,
                        PlaceId from, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

PlaceId *GvGetReachableByType(GameView gv, Player player, Round round,
                              PlaceId from, bool road, bool rail,
                              bool boat, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions
static int getHunterHealth(GameView gv, int player)
{
    int health, turn, round;
    int indexEncounters, indexLocation;
    int i, j;
    char encounters[5], location[3];
    Queue trail = newQueue();
    char play[PLAY_LENGTH];
    Play temp;
    
    turn = gv->turn;
    round = GvGetRound(gv);
    
    // traverse through the entire pastPlays from round 0 to current round
    // if it is round 0, health = initial health
    // else use maths algos to calculate health
    for (i = 0; i <= round; i++) {
        indexLocation = player*PLAY_LENGTH + i*NUM_PLAYERS*PLAY_LENGTH + 1;
        memset (location, '\0', sizeof(location));
        strncpy (location, gv->play + indexLocation, 2);
        strncpy (play, gv->play + player*PLAY_LENGTH + PLAY_LENGTH*NUM_PLAYERS*i, PLAY_LENGTH);
        
        if (numItems(trail) == 6) leaveQueue(trail);
        if (strlen(gv->play) != 0) addToQueue(trail, play);
        
        if (i == 0) {
            health = GAME_START_HUNTER_LIFE_POINTS;
        }
        else {
            indexEncounters = player*PLAY_LENGTH + i*NUM_PLAYERS*PLAY_LENGTH + 3;
            if (indexEncounters > turn*PLAY_LENGTH) break;
            memset (encounters, '\0', sizeof(encounters));
            strncpy (encounters, gv->play + indexEncounters, 4);
            
            for (j = 0; j < numItems(trail) - 2; j++);
            temp = getElementAt(trail, j);
            if (location[0] == temp[1] && location[1] == temp[2]) {
                health += LIFE_GAIN_REST;
                if (health >= GAME_START_HUNTER_LIFE_POINTS) {
                    health = GAME_START_HUNTER_LIFE_POINTS;
                }
            } else if (placeAbbrevToId(location) == HOSPITAL_PLACE) {
                health = GAME_START_HUNTER_LIFE_POINTS;
            }
            
            for (j = 0; j < 4; j++) {
                if (encounters[j] == '.') break;
                else if (encounters[j] == 'T') {
                    health -= LIFE_LOSS_TRAP_ENCOUNTER;
                }
                else if (encounters[j] == 'D') {
                    health -= LIFE_LOSS_DRACULA_ENCOUNTER;
                    gv->players[PLAYER_DRACULA].health -= LIFE_LOSS_HUNTER_ENCOUNTER;
                }
            }
            if (health <= 0) {
                health = 0;
            }
        }
    }
    return health;
}
// TODO

static int getDraculaHealth(GameView gv)
{
    int health, round;
    int indexLocation, placeID;
    int i, j;
    char location[3];
    Queue trail = newQueue();
    char play[PLAY_LENGTH];
    Play temp;
    
    round = GvGetRound(gv);
    
    // traverse through the entire pastPlays from round 0 to current round
    // if it is round 0, health = initial health
    // else use maths algos to calculate health
    for (i = 0; i <= round; i++) {
        if(gv -> turn == 15) printf("i = %d and health = %d\n", i, health);
        
        indexLocation = PLAYER_DRACULA*PLAY_LENGTH + i*NUM_PLAYERS*PLAY_LENGTH + 1;
        memset (location, '\0', sizeof(location));
        strncpy (location, gv->play + indexLocation, 2);
        placeID = placeAbbrevToId(location);
        strncpy (play, gv->play + PLAYER_DRACULA*PLAY_LENGTH + PLAY_LENGTH*NUM_PLAYERS*i, PLAY_LENGTH);
        
        if (numItems(trail) == 6) leaveQueue(trail);
        if (strlen(gv->play) != 0) addToQueue(trail, play);
        
        if (i == 0) {
            health = GAME_START_BLOOD_POINTS;
            if (location[0] == 'S' && location[1] == '?') {
                health -= LIFE_LOSS_SEA;
                gv->players[PLAYER_DRACULA].health = health;
            } else if (placeID <= 70 && placeID >= 0 && placeIsSea(placeID)) {
                health -= LIFE_LOSS_SEA;
                gv->players[PLAYER_DRACULA].health = health;
            }
        } else {
            health = gv->players[PLAYER_DRACULA].health;
            
            if (location[0] == 'S' && location[1] == '?') {
                health -= LIFE_LOSS_SEA;
                gv->players[PLAYER_DRACULA].health = health;
            } else if (placeID <= 70 && placeID >= 0 && placeIsSea(placeID)) {
                health -= LIFE_LOSS_SEA;
                gv->players[PLAYER_DRACULA].health = health;
            } else if (location[0] == 'D' && isdigit(location[1])) {
                for (j = 0; j < numItems(trail) - location[1]; j++);
                temp = getElementAt(trail, j);
                strncpy (location, temp + 1, 2);
                placeID = placeAbbrevToId(location);
                if (temp[1] == 'S' && temp[2] == '?') {
                    health -= LIFE_LOSS_SEA;
                    gv->players[PLAYER_DRACULA].health = health;
                } else if (placeID <= 70 && placeID >= 0&& placeIsSea(placeID)) {		
                    health -= LIFE_LOSS_SEA;			
                    gv->players[PLAYER_DRACULA].health = health;					
                }				
            } else if (placeID <= 70 && placeID >= 0 && placeAbbrevToId(location) == CASTLE_DRACULA) {
                health += LIFE_GAIN_CASTLE_DRACULA;
                gv->players[PLAYER_DRACULA].health = health;
            }			
        }
        if (health <= 0) {
            health = 0;		
            break;
        }
    }
    
    return health;
}

static void initialiseHealth (GameView gv)
{
    int player;
    
    for (player = PLAYER_DR_SEWARD; player < NUM_PLAYERS; player++) {
        if (player == PLAYER_DRACULA) {
            gv->players[PLAYER_DRACULA].health = GAME_START_BLOOD_POINTS;
        } else {
            gv->players[player].health = GAME_START_HUNTER_LIFE_POINTS;
        }	
    }
}

void initialiseLocation(GameView gv)
{
    char location[3];
    
    // find the index of the location abbreviation from pastPlays
    // if the current turn is 'player', he/she should not have made a new move,
    // then we need to subtract the index by 40 (8 bytes * NUM_PLAYERS)
    // only if it is not round 0 (to avoid negative index)
    char * pastPlayS = malloc(sizeof(char) * strlen(gv->play));
    strncpy(pastPlayS, gv->play, MAX_LENGTH);
    char * currPlay = strtok(gv->play, " ");
    int i = 0;
    while(currPlay != NULL){
        addToQueue(gv->players[i%NUM_PLAYERS].trail, currPlay);
        i++;
        currPlay = strtok(NULL, " ");
    }
    free(pastPlayS);
}

static PlaceId GvGetPlayerLocationcall(GameView gv, Player player) {
    char location[3];
    int turn, round;
    int index;
    int placeId;
    
    turn = gv->turn;
    round = GvGetRound(gv);
    
    // find the index of the location abbreviation from pastPlays
    // if the current turn is 'player', heshould not have made a new move,
    // then we need to subtract the index by 40 (7 bytes * NUM_PLAYERS)
    // only if it is not round 0 (to avoid negative index)
    index = player*PLAY_LENGTH + round*NUM_PLAYERS*PLAY_LENGTH + 1;
    if (index > turn*PLAY_LENGTH) index -= NUM_PLAYERS*PLAY_LENGTH;
    
    // copy the abbreaviated name to the array 'location'
    
    memset (location, '\0', sizeof(location));
    strncpy (location, gv->play + index, 2);
    
    // use the abbrevToID function in places.c to convert to a locationID
    // an exception is Dracula, where the there are many possible locations
    // switch does not support char compare, so have to do it case by case
    if (player != PLAYER_DRACULA) {
        placeId = placeAbbrevToId(location);
    }
    return placeId;
}

static PlaceId GvGetVampireLocationcall(GameView gv)
{
    char location[3];
    int turn, round;
    int index;
    int placeId;
    Player playerV;
    turn = gv->turn;
    round = GvGetRound(gv);
    
    // find the index of the location abbreviation from pastPlays
    // if the current turn is 'player', heshould not have made a new move,
    // then we need to subtract the index by 40 (7 bytes * NUM_PLAYERS)
    // only if it is not round 0 (to avoid negative index)
    index = playerV*PLAY_LENGTH + round*NUM_PLAYERS*PLAY_LENGTH + 1;
    if (index > turn*PLAY_LENGTH) index -= NUM_PLAYERS*PLAY_LENGTH;
    
    // copy the abbreaviated name to the array 'location'
    
    memset (location, '\0', sizeof(location));
    strncpy (location, gv->play + index, 2);
    
    // use the abbrevToID function in places.c to convert to a locationID
    // an exception is Dracula, where the there are many possible locations
    // switch does not support char compare, so have to do it case by case
    if (playerV == PLAYER_DRACULA) {
        if (location[1] == '?') {
            if (location[0] == 'C') {
                placeId = CITY_UNKNOWN;
            } else {
                placeId = SEA_UNKNOWN;
            }
        } else if (location[0] == 'H' && location[1] == 'I') {
            placeId = HIDE;
        } else if (location[0] == 'D' && isdigit(location[1])) {
            switch (location[1]) {
                case '1': placeId = DOUBLE_BACK_1; break;
                case '2': placeId = DOUBLE_BACK_2; break;
                case '3': placeId = DOUBLE_BACK_3; break;
                case '4': placeId = DOUBLE_BACK_4; break;
                case '5': placeId = DOUBLE_BACK_5; break;
            }
        } else if (location[0] == 'T' && location[1] == 'P') {
            placeId = TELEPORT;
        } else {
            placeId = placeAbbrevToId(location);
        }		
    }
    
    return placeId;
}
