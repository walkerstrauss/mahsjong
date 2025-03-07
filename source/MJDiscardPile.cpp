//
//  MJDiscardPile.cpp
//  Mahsjong
//
//  Created by Walker Strauss on 3/5/25.
//

#include "MJDiscardPile.h"
#include "MJPlayer.h"
#include "MJTileSet.h"

using namespace cugl;
using namespace cugl::scene2;
using namespace cugl::graphics;

/**
 * This is the class initializing and handling the discard pile
 */
#pragma mark -
#pragma mark Constructors

bool DiscardPile::init(const std::shared_ptr<Player>& player){
    // clear all vectors and set initial size to 0
    _discardPile.clear();
    _topTile = nullptr;
    _size = 0;
    // set player
    if (!player){
        CULog("No player for discarding!");
        return false;
    } else {
        _player = player;
    }
    // TODO: add other initialization logic as necessary -
    return true;
}

/**
 * Method to update the position of the discard pile tiles
 */
void DiscardPile::updateTilePositions(){
    // TODO: implement positioning logic for if the pile is size one or two (if it is one keep location if it is two then start x, end x, spacing x, -
    // TODO: replace this with better logic - should be based on the position specified in a Json file -
    
    // Set top tile position for rendering to game scene
    _topTile->pos = cugl::Vec2(1100,600);
    return;
}

/**
 * Method to take a tile from the discard pile.
 * Returns a tile for the player to add to hand and updates the dicard pile
 *
 * @return a tile to add to the player hand
 */
std::shared_ptr<TileSet::Tile> DiscardPile::drawTopTile(){
    // TODO: call this method when player draws from discard pile -
    CULog("drawing from discard pile");
    return getTopTile();
}


