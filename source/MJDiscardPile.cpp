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

bool DiscardPile::init(const std::shared_ptr<cugl::AssetManager>& assets){
    // clear all vectors and set initial size to 0
    _discardPile.clear();
    _topTile = nullptr;
    _size = 0;
    if (assets == nullptr){
        return false;
    }
    _assets = assets;
    
    // TODO: add other initialization logic as necessary
    return true;
}

/**
 * Method to update the position of the discard pile tiles
 */
void DiscardPile::updateTilePositions(){
    // TODO: implement positioning logic for if the pile is size one or two (if it is one keep location if it is two then start x, end x, spacing x, -
    // TODO: replace this with better logic - should be based on the position specified in a Json file
    
    // Set top tile position for rendering to game scene
    _topTile->pos = cugl::Vec2(990,520);
    Size textureSize(350.0, 415.0);
    Vec2 rectOrigin(_topTile->pos - (textureSize * getTopTile()->_scale)/2);
    getTopTile()->tileRect = cugl::Rect(rectOrigin, textureSize * getTopTile()->_scale);
    return;
}

bool DiscardPile::isTileSelected(const cugl::Vec2& mousePos){
    // Tile position (matches render)
    if (getTopTile() == nullptr){
        CULog("nullptr");
        return false;
    }
    cugl::Vec2 pos(990, 520);
//    if(it->selected){
//        pos.y = curr.pos.y + 10;
//    }
//    Vec2 origin = Vec2(curr.getTileTexture()->getSize().width/2, curr.getTileTexture()->getSize().height/2);
//    
//    Affine2 trans;
//    trans.scale(curr._scale);
//    trans.translate(pos);
//    
//    Size textureSize(350.0, 415.0);
//    Vec2 rectOrigin(pos - (textureSize * curr._scale)/2);
//    it->tileRect = cugl::Rect(rectOrigin, textureSize * curr._scale);
//    
//    batch->draw(curr.getTileTexture(), origin, trans);
        // Get tile size with scaling applied
    return getTopTile()->tileRect.contains(mousePos);
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

/**
 * Method to render the top card of the discard pile
 */
void DiscardPile::render(const std::shared_ptr<cugl::graphics::SpriteBatch>& batch){
    if (_size > 0){
        std::shared_ptr<TileSet::Tile> tile = getTopTile();
        cugl::Vec2 pos(990,520);
        
        cugl::Vec2 origin = cugl::Vec2(tile->getTileTexture()->getSize().width/2, tile->getTileTexture()->getSize().height/2);
        cugl::Affine2 trans;
        trans.scale(tile->_scale);
        trans.translate(pos);
        
        batch->draw(tile->getTileTexture(), origin, trans);
    }
}

