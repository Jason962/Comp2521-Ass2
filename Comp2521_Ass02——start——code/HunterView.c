////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// HunterView.c: the HunterView ADT implementation
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

#include "Game.h"
#include "GameView.h"
#include "HunterView.h"
#include "Map.h"
#include "Places.h"
// add your own #includes here

// TODO: ADD YOUR OWN STRUCTS HERE
typedef struct Hunter {
    int Score;
    int Health;
    int Location;
    char *name; 
      
} Hunter;

struct hunterView {
	// TODO: ADD FIELDS HERE
	Hunter HunterInfo;
	int Round;
	PlaceId *shortestPath;
	GameView view;
};

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

HunterView HvNew(char *pastPlays, Message messages[])
{
	// TODO:
	// Create a New View using GV ADT:
	HunterView newView = malloc(sizeof(HunterView));
	assert(newView != NULL);
	newView->view = GvNew(pastPlays, messages);
	return newView;
	
}

void HvFree(HunterView hv)
{
	// TODO: 
	// Free the View:
	GvFree(hv->view);
	free(hv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round HvGetRound(HunterView hv)
{
	// TODO: 
	// Get the current round:
	int CurrentRound = GvGetRound(hv->view);
	hv->Round = CurrentRound;
	return CurrentRound;
}

Player HvGetPlayer(HunterView hv)
{
	// TODO: 
	// Determine whose turn it is:
	Player PlayerNumber = GvGetPlayer(hv->view);	
	return PlayerNumber;
}

int HvGetScore(HunterView hv)
{
	// TODO: 
	// To determine to score of the active player:
	int PlayerScore = GvGetScore(hv->view);
	return PlayerScore;
}

int HvGetHealth(HunterView hv, Player player)
{
	// TODO: 
	// Show the players HP:
	int PlayerHealth = GvGetHealth(hv->view, player);
	return PlayerHealth;
}

PlaceId HvGetPlayerLocation(HunterView hv, Player player)
{
	// TODO: 
	// Shows location of the Player:
	PlaceId LocationOfPlayer = GvGetPlayerLocation(hv->view, player);
	return LocationOfPlayer;
}

PlaceId HvGetVampireLocation(HunterView hv)
{
	// TODO: 
	// Show the location of the immature vampire:
	PlaceId LocationOfVampire = GvGetVampireLocation(hv->view);
	return LocationOfVampire;
}

////////////////////////////////////////////////////////////////////////
// Utility Functions

PlaceId HvGetLastKnownDraculaLocation(HunterView hv, Round *round)
{
	// TODO: 
	// Gets Dracula's last known location and 
	// how many rounds ago it was (*round):    
	bool *Free = false;
	int *VisitedNumber = 0;
	// Get the Location History of Dracula via GameView:
	PlaceId *LocationDracula = GvGetLocationHistory(hv->view, PLAYER_DRACULA, 
	                                         VisitedNumber , Free);
	// The function returns an array of PlaceId's of Dracula's visited locations
	int i = *VisitedNumber - 1;                    
	while (i > 0 && Free) {
	    // Scan the array backwards to see if any locations are real.
	    if (placeIsReal(LocationDracula[i])) {
	        // If there is a known location, return it:
	        *round = *VisitedNumber - 1 - i;
	        return LocationDracula[i];
	        
	    }
	    i--;
	}
	// If there was not a known location for Dracula.
    return NOWHERE;
}

PlaceId *HvGetShortestPathTo(HunterView hv, Player hunter, PlaceId dest,
                             int *pathLength)
{
	// TODO: 
	// To find the shortest path to the given destination, use Dijsktra's:
	int arrayDist[];
	int arrayPred[];
	
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *HvWhereCanIGo(HunterView hv, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

PlaceId *HvWhereCanIGoByType(HunterView hv, bool road, bool rail,
                             bool boat, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

PlaceId *HvWhereCanTheyGo(HunterView hv, Player player,
                          int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

PlaceId *HvWhereCanTheyGoByType(HunterView hv, Player player,
                                bool road, bool rail, bool boat,
                                int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// TODO
