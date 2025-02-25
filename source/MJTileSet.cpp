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

#pragma mark Tile

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
    
//    pos = Vec2(200, 200); Setting up drawing example, set when in hand and pile sets
    _scale = 0.4;
}

/**
 * Sets the texture for all tiles in deck
 */
void TileSet::setTexture(const std::shared_ptr<cugl::graphics::Texture>& value){
    Size size = value->getSize();
    _center = Vec2(size.width/2, size.height/2);
    
    for(const auto& it : deck){
        it->setTileTexture(value);
    }
}

/**
 * Initializes the **STARTING** representation of the deck.
 *
 * When creating a new level, call shuffle to reshuffle the tileSet and create
 * the pile and hand by iterating through the tileSet
 */
TileSet::TileSet(){
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
    
    wildCount = 0;
}

/**
 * Generates 3 random unique grandma tiles
 */
void TileSet::generateGrandmaTiles() {
    cugl::Random rd;
    rd.init();
    
    std::vector<int> ranks;
    std::vector<int> suits;

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
            
            grandmaTiles.push_back(newTile);
        }
    }
}

void TileSet::draw(const std::shared_ptr<cugl::graphics::SpriteBatch>& batch, cugl::Size size){
    for(const auto& it : deck){
        Tile curr = (*it);
        if(curr.discarded){
            continue;
        }
        Vec2 pos = curr.pos;
        Vec2 origin = _center;
        
        Affine2 trans;
        trans.translate(pos);
        trans.scale(curr._scale);
        
        batch->draw(curr.getTileTexture(), origin, trans);
    }
}





