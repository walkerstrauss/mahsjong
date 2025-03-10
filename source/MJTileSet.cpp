//
//  MJTileSet.cpp
//  Mahsjong
//
//  Created by Patrick Choo on 2/21/25.
//

#include "MJTileSet.h"

using namespace cugl;
using namespace cugl::graphics;
using namespace cugl::audio;

#pragma mark -
#pragma mark Tile Constructors

/**
 * Allocates a tile by setting its number and suit
 *
 * A wild tile can either have a wild rank and/or a wild suit
 *
 * @param r     The rank
 * @param s     The suit
 */
TileSet::Tile::Tile(const TileSet::Tile::Rank r, const TileSet::Tile::Suit s){
    _rank = r;
    _suit = s;
    
    inPile = false;
    inHand = false;
    discarded = false;
    
    selected = false;
    selectedInSet = false;
    played = false;
}

#pragma mark -
#pragma mark Tileset Constructors
/**
 * Initializes the **STARTING** representation of the deck.
 *
 * When creating a new level, call shuffle to reshuffle the tileSet and create
 * the pile and hand by iterating through the tileSet
 */
TileSet::TileSet(){
    rdTileSet.init();
    for(int i = 1; i < 4; i++){
        TileSet::Tile::Suit currSuit = static_cast<TileSet::Tile::Suit>(i);
        for(int j = 1; j < 10; j++){
            TileSet::Tile::Rank currRank = static_cast<TileSet::Tile::Rank>(j);
            for(int k = 0; k < 4; k++){
                std::shared_ptr<Tile> newTile = std::make_shared<Tile>(currRank, currSuit);
                newTile->_id = k;
                deck.emplace_back(newTile);
                tileCount += 1;
            }
        }
    }
    generateGrandmaTiles();
    wildCount = 0;
}

#pragma mark -
#pragma mark Tileset Gameplay Handling

/**
 * Generates 3 random unique grandma tiles
 */
void TileSet::generateGrandmaTiles() {
    cugl::Random rd;
    rd.init();
    std::vector<int> ranks;
    std::vector<int> suits;
    float startX = 108.0f;
    float startY = 675.0f;
    float spacing = 60.0f;

    while (ranks.size() < 3) {
        int rank = static_cast<int>(rd.getOpenUint64(1, 9));
        int suit = static_cast<int>(rd.getOpenUint64(1, 3));

        bool exists = false;
        for (size_t i = 0; i < ranks.size(); i++) {
            if (ranks[i] == rank && suits[i] == suit) {
                exists = true;
                break;
            }
        }
        if (!exists) {
            ranks.push_back(rank);
            suits.push_back(suit);

            std::shared_ptr<TileSet::Tile> newTile = std::make_shared<TileSet::Tile>(
                static_cast<TileSet::Tile::Rank>(rank),
                static_cast<TileSet::Tile::Suit>(suit)
            );
            
            newTile->pos = Vec2(startX + ranks.size() * spacing, startY);
            newTile->_scale = 0.2f;
            grandmaTiles.push_back(newTile);
        }
    }
}

/**
 * Sets the texture for all tiles in deck
 */
void TileSet::setAllTileTexture(const std::shared_ptr<cugl::AssetManager>& assets){
    if(deck.size() == 0){
        CULog("Deck is empty");
    }
    for(const auto& it : deck){
        std::string currTileTexture = it->toString();
        it->setTexture(assets->get<Texture>(currTileTexture));
    }
    for(const auto& it: grandmaTiles){
        std::string currTileTexture = it->toString();
        it->setTexture(assets->get<Texture>(currTileTexture));
    }
}

/**
 * Draws the tiles in the tileset to the screen
 */
void TileSet::draw(const std::shared_ptr<cugl::graphics::SpriteBatch>& batch, cugl::Size size){
    for(const auto& it : deck){
        Tile curr = (*it);
        Vec2 pos = curr.pos;
        if(it->played){
            continue;
        }
        if(it->selected){
            pos.y = curr.pos.y + 10;
        }
        Vec2 origin = Vec2(curr.getTileTexture()->getSize().width/2, curr.getTileTexture()->getSize().height/2);
        
        Affine2 trans;
        trans.scale(curr._scale);
        trans.translate(pos);
        
        Size textureSize(350.0, 415.0);
        Vec2 rectOrigin(pos - (textureSize * curr._scale)/2);
        it->tileRect = cugl::Rect(rectOrigin, textureSize * curr._scale);
        
        batch->draw(curr.getTileTexture(), origin, trans);
    }
//    for (const auto& it : grandmaTiles){
//        Tile curr = (*it);
//        Vec2 pos = curr.pos;
//        Vec2 origin = Vec2(curr.getTileTexture()->getSize().width/2, curr.getTileTexture()->getSize().height/2);
//        Affine2 trans;
//        trans.scale(curr._scale);
//        trans.translate(pos);
//        
//        batch->draw(curr.getTileTexture(), origin, trans);
//    }
//    Vec2 pos = Vec2(70.0f,675.0f);
//    Vec2 origin = Vec2(gmaTexture->getSize().width/2,gmaTexture->getSize().height/2);
//    Affine2 trans;
//    trans.scale(0.7);
//    trans.translate(pos);
//    
//    batch->draw(gmaTexture, origin, trans);
}

#pragma mark -
#pragma mark Tile Gameplay Handling

/**
 * Sets the texture of a wild tile
 *
 * @param assets    the asset manager to get the texture from
 */
void TileSet::Tile::setWildTexture(const std::shared_ptr<cugl::AssetManager>& assets){
    std::string currTileTexture = (this)->toString();
    this->setTexture(assets->get<Texture>(currTileTexture));
}


