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
    inHostHand = false;
    inClientHand = false;
    discarded = false;
    
    selected = false;
    selectedInSet = false;
    played = false;
}

#pragma mark -
#pragma mark Tileset Constructors

/**
 * Initializes deck to a **STARTING** representation of numbered tiles
 *
 * Only call if host
 */
void TileSet::initHostDeck(){
    for(int i = 1; i < 4; i++){
        Tile::Suit currSuit = static_cast<Tile::Suit>(i);
        for(int j = 1; j < 10; j++){
            Tile::Rank currRank = static_cast<Tile::Rank>(j);
            for(int k = 0; k < 4; k++){
                std::shared_ptr<Tile> newTile = std::make_shared<Tile>(currRank, currSuit);
                newTile->_id = k;
                deck.emplace_back(newTile);
                tileMap.insert({newTile->toString() + " " + std::to_string(newTile->_id), newTile});
                tileCount += 1;
            }
        }
    }
}

/**
 * Initializes deck to a **STARTING** representation of numbered tiles
 *
 * @returns  true if successful
 */
bool TileSet::initClientDeck(const std::shared_ptr<cugl::JsonValue>& deckJson){
    std::vector<std::shared_ptr<TileSet::Tile>> allTiles = processTileJson(deckJson);
    deck = allTiles;
    
    for (auto& tile : deck) {
        std::string key = tile->toString() + " " + std::to_string(tile->_id);
        tileMap.insert({key, tile});
    }
    return true;
}


#pragma mark -
#pragma mark Tileset Gameplay Handling
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
 * Sets the texture of a wild tile
 *
 * @param assets    the asset manager to get the texture from
 */
void TileSet::setBackTextures(const std::shared_ptr<cugl::AssetManager>& assets){
    for (const auto& it : deck) {
        if (it->inPile) {
            it->setTexture(assets->get<Texture>("facedown"));
        }
    }
}

/**
 * Draws the tiles in the tileset to the screen
 */
//void TileSet::draw(const std::shared_ptr<cugl::graphics::SpriteBatch>& batch, cugl::Size size, bool isHost){
//    for(const auto& it : deck){
//        Tile curr = (*it);
//        Vec2 pos = curr.pos;
//        
//        if (it->played) {
//            continue;
//        }
//        
//        if (isHost && (it->inHostHand || it->inPile || it->topTile)){
//            if(it->selected && it->inHostHand){
//                pos.y = curr.pos.y + 10;
//            }
//            if(it->selected && (it->inPile || it->discarded)){
//                it->_scale = 0.25;
//            }
//            else if (it->inPile || it->discarded){
//                it->_scale = 0.2;
//            }
//            Vec2 origin = Vec2(curr.getTileTexture()->getSize().width/2, curr.getTileTexture()->getSize().height/2);
//            
//            Affine2 trans;
//            trans.scale(curr._scale);
//            trans.translate(pos);
//            
//            Size textureSize(350.0, 415.0);
//            Vec2 rectOrigin(pos - (textureSize * curr._scale)/2);
//            it->tileRect = cugl::Rect(rectOrigin, textureSize * curr._scale);
//
//            batch->draw(curr.getTileTexture(), origin, trans);
//        } else if (!isHost && (it->inClientHand || it->inPile || it->topTile)){
//            if(it->selected && it->inClientHand){
//                pos.y = curr.pos.y + 10;
//            }
//            if(it->selected && (it->inPile || it->discarded)){
//                it->_scale = 0.25;
//            }
//            else{
//                it->_scale = 0.2;
//            }
//            Vec2 origin = Vec2(curr.getTileTexture()->getSize().width/2, curr.getTileTexture()->getSize().height/2);
//            
//            Affine2 trans;
//            trans.scale(curr._scale);
//            trans.translate(pos);
//            
//            Size textureSize(350.0, 415.0);
//            Vec2 rectOrigin(pos - (textureSize * curr._scale)/2);
//            it->tileRect = cugl::Rect(rectOrigin, textureSize * curr._scale);
//
//            batch->draw(curr.getTileTexture(), origin, trans);
//        }
//    }
//}

#pragma mark -
#pragma mark Tile Gameplay Handling
/**
 * Creates a cugl::JsonValue representation of the current state of the deck
 *
 * @Returns std::shared_ptr<cugl::JsonVaue>
 */
const std::shared_ptr<cugl::JsonValue> TileSet::toJson(std::vector<std::shared_ptr<Tile>> tiles){
    std::shared_ptr<cugl::JsonValue> root  = cugl::JsonValue::allocObject();
    for (auto& tile : tiles){
        std::string key = tile->toString() + " " + std::to_string(tile->_id);
        
        std::shared_ptr<cugl::JsonValue> currTile = cugl::JsonValue::allocObject();
        currTile->appendValue("suit", tile->toStringSuit());
        currTile->appendValue("rank", tile->toStringRank());
        currTile->appendValue("id", std::to_string(tile->_id));
        currTile->appendValue("pileCoord", tile->pileCoord.toString());
        currTile->appendValue("inPile", tile->inPile);
        currTile->appendValue("inHostHand", tile->inHostHand);
        currTile->appendValue("inClientHand", tile->inClientHand);
        currTile->appendValue("discarded", tile->discarded);
        currTile->appendValue("selected", tile->selected);
        currTile->appendValue("selectedInSet", tile->selectedInSet);
        currTile->appendValue("played", tile->played);
        currTile->appendValue("inDeck", tile->inDeck);
        currTile->appendValue("pos", tile->pos);
        currTile->appendValue("scale", tile->_scale);
        
        root->appendChild(key, currTile);
    }
    
    return root;
}

void TileSet::setNextTile(std::shared_ptr<cugl::JsonValue>& nextTileJson) {
    if(nextTileJson->children().size() > 1){
        CULog("Invalid set next tile size");
        return;
    }
    for(auto const& tileKey : nextTileJson->children()){
        const std::string suit = tileKey->getString("suit");
        const std::string rank = tileKey->getString("rank");
        const std::string id = tileKey->getString("id");
        
        std::string key = suit + " " + rank + " " + id;
        nextTile = tileMap[key];
    }
}

void TileSet::updateDeck(const std::shared_ptr<cugl::JsonValue>& deckJson) {
    for(const auto& tileKey : deckJson->children()) {
        const std::string rank = tileKey->getString("rank");
        const std::string suit = tileKey->getString("suit");
        const std::string id = tileKey->getString("id");
        
//        const cugl::Vec2 pileCoord = Tile::toVector(tileKey->getString("pileCoord"));
        const bool inPile = tileKey->getBool("inPile");
        const bool inHostHand = tileKey->getBool("inHostHand");
        const bool inClientHand = tileKey->getBool("inClientHand");
        const bool discarded = tileKey->getBool("discarded");
        const bool selected = tileKey->getBool("selected");
        const bool selectedInSet = tileKey->getBool("selectedInSet");
        const bool played = tileKey->getBool("played");
        const bool inDeck = tileKey->getBool("inDeck");
        const cugl::Vec2 pos = Tile::toVector(tileKey->getString("pos"));
        const float scale = tileKey->getFloat("scale");
        
        std::string key = rank + " of " + suit + " " + id;
        tileMap[key]->inPile = inPile;
        tileMap[key]->inHostHand = inHostHand;
        tileMap[key]->inClientHand = inClientHand;
        tileMap[key]->discarded = discarded;
        tileMap[key]->selected = selected;
        tileMap[key]->selectedInSet = selectedInSet;
        tileMap[key]->played = played;
        tileMap[key]->pos = pos;
        tileMap[key]->_scale = scale;
        if(tileMap[key]->inDeck == true && inDeck == false){
            deck.erase(std::remove(deck.begin(), deck.end(), tileMap[key]), deck.end());
        }
        tileMap[key]->inDeck = inDeck;
    }
}

std::vector<std::shared_ptr<TileSet::Tile>> TileSet::processTileJson(const std::shared_ptr<cugl::JsonValue>& tileJson) {
    std::vector<std::shared_ptr<TileSet::Tile>> tiles;
    
    for(const auto& tileKey : tileJson->children()) {
        const Tile::Suit suit = Tile::toSuit(tileKey->getString("suit"));
        const Tile::Rank rank = Tile::toRank(tileKey->getString("rank"));
        const int id = std::stoi(tileKey->getString("id"));
        const cugl::Vec2 pileCoord = Tile::toVector(tileKey->getString("pileCoord"));
        const bool inPile = tileKey->getBool("inPile");
        const bool inHostHand = tileKey->getBool("inHostHand");
        const bool inClientHand = tileKey->getBool("inClientHand");
        const bool discarded = tileKey->getBool("discarded");
        const bool selected = tileKey->getBool("selected");
        const bool selectedInSet = tileKey->getBool("selectedInSet");
        const bool played = tileKey->getBool("played");
        const bool inDeck = tileKey->getBool("inDeck");
        const cugl::Vec2 pos = Tile::toVector(tileKey->getString("pos"));
        const float scale = tileKey->getFloat("scale");
        
        std::shared_ptr<Tile> newTile = std::make_shared<Tile>(rank, suit);
        newTile->_id = id;
        newTile->pileCoord = pileCoord;
        newTile->inPile = inPile;
        newTile->discarded = discarded;
        newTile->selected = selected;
        newTile->selectedInSet = selectedInSet;
        newTile->played = played;
        newTile->pos = pos;
        newTile->_scale = scale;
        newTile->inHostHand = inHostHand;
        newTile->inClientHand = inClientHand;
        newTile->inDeck = inDeck;
        
        tiles.push_back(newTile);
    }
    
    return tiles;
}

