//
//  MJDiscardPile.cpp
//  Mahsjong
//
//  Created by Walker Strauss on 3/5/25.
//

#include "MJDiscardPile.h"
#include "MJPlayer.h"
#include "MJTileSet.h"

#define VELOCITY_THRESHOLD 2.0f
#define ROTATE_MAX 0.3f

#define SPRING 0.05f
#define DAMP 0.05f

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

void DiscardPile::updateTilePositions(float dt) {
    if(_topTile) {
        Vec2 pos = _topTile->pos;
        Vec2 origin = Vec2(_topTile->getTileTexture()->getSize().width/2, _topTile->getTileTexture()->getSize().height/2);
        
        Size textureSize(_topTile->getBackTextureNode()->getTexture()->getSize());
        Vec2 rectOrigin(pos - (textureSize * _topTile->_scale)/2);
        _topTile->tileRect = cugl::Rect(rectOrigin, textureSize * _topTile->_scale);
       
        float velocity = _topTile->getContainer()->getPosition().x - _topTile->pos.x;
        float displacement = _topTile->getContainer()->getAngle();
        float force = -SPRING * displacement - DAMP * velocity;
        
        Vec2 lerpPos = _topTile->getContainer()->getPosition();
        lerpPos.lerp(pos, 0.5);
        
        velocity += force * dt;
        displacement = std::clamp(velocity * dt, -ROTATE_MAX, ROTATE_MAX);
        
        _topTile->getContainer()->setAnchor(Vec2::ANCHOR_CENTER);
        _topTile->getContainer()->setAngle(displacement);
        _topTile->getContainer()->setScale(_topTile->_scale);
        _topTile->getContainer()->setPosition(lerpPos);
    }
}

/*
 * Method to render the top card of the discard pile
 */
void DiscardPile::draw(const std::shared_ptr<cugl::graphics::SpriteBatch>& batch){
    if(_topTile) {
        _topTile->getContainer()->render(batch, Affine2::IDENTITY, Color4::WHITE);
    }
}
