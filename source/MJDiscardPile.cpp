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

bool DiscardPile::isTileSelected(const cugl::Vec2& mousePos){
    // Tile position (matches render)
    std::shared_ptr<TileSet::Tile> currTile = getTopTile();
    if (currTile == nullptr){
        return false;
    }
    else if(currTile->tileRect.contains(mousePos)){
        if(currTile->selected){
            currTile->_scale = 0.2;
            currTile->selected = false;
            _selectedTopTile -= 1;
        }
        else{
            currTile->_scale = 0.25;
            currTile->selected = true;
            _selectedTopTile += 1;
        }
        return true;
    }
    return false;
}

/**
 * Method to take a tile from the discard pile.
 * Returns a tile for the player to add to hand and updates the dicard pile
 *
 * @return a tile to add to the player hand
 */
std::shared_ptr<TileSet::Tile> DiscardPile::drawTopTile(){
    // TODO: call this method when player draws from discard pile -
    if (!_topTile) {
        return nullptr; // No tile to draw
    }
    std::shared_ptr<TileSet::Tile> topTile = _topTile;
    
    _discardMap.erase(std::to_string(_topTile->_id));
    _topTile = nullptr; // Set top tile to null

    _size = static_cast<int>(_discardPile.size()) + (_topTile ? 1 : 0);
    
    return topTile;
}

/*
 * Method to render the top card of the discard pile
 */
void DiscardPile::draw(const std::shared_ptr<cugl::graphics::SpriteBatch>& batch, const std::shared_ptr<TileSet::Tile> draggingTile){
    if(_topTile && _topTile == draggingTile) {
        Vec2 pos = _topTile->pos;
        Vec2 origin = Vec2(_topTile->getBackTextureNode()->getTexture()->getSize().width/2, _topTile->getBackTextureNode()->getTexture()->getSize().height/2);
        
        Size textureSize(_topTile->getBackTextureNode()->getTexture()->getSize());
        Vec2 rectOrigin(pos - (textureSize * _topTile->_scale)/2);
        _topTile->tileRect = cugl::Rect(rectOrigin, textureSize * _topTile->_scale);
        
        _topTile->getContainer()->setAnchor(Vec2::ANCHOR_CENTER);
        _topTile->getContainer()->setScale(_topTile->_scale);
        _topTile->getContainer()->setPosition(pos);
        _topTile->getContainer()->render(batch, Affine2::IDENTITY, Color4::WHITE);
    }
}
