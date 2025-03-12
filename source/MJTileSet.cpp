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
                tileCount += 1;
            }
        }
    }
}

/**
 * Initializes deck to a **STARTING** representation of numbered tiles
 *
 * Only call if clientx
 */
void TileSet::initClientDeck(const std::shared_ptr<cugl::JsonValue>& deckJson){
    
    for(auto const& tileKey : deckJson->children()){
            const Tile::Suit suit = Tile::toSuit(tileKey->getString("suit"));
            const Tile::Rank rank = Tile::toRank(tileKey->getString("rank"));
            const int id = std::stoi(tileKey->getString("id"));
//            const cugl::Vec2 pileCoord = Tile::toVector(tileIdNum->getString("pileCoord"));
//            const bool inPile = tileIdNum->getBool("inPile");
//            const bool inHand = tileIdNum->getBool("inhand");
//            const bool discarded = tileIdNum->getBool("discarded");
//            const bool selected = tileIdNum->getBool("selected");
//            const bool selectedInSet = tileIdNum->getBool("selectedInSet");
//            const bool played = tileIdNum->getBool("played");
//            const cugl::Vec2 pos = Tile::toVector(tileIdNum->getString("pos"));
//            const float scale = tileIdNum->getFloat("scale");
            
            std::shared_ptr<Tile> newTile = std::make_shared<Tile>(rank, suit);
            newTile->_id = id;
//            newTile->pileCoord = pileCoord;
//            newTile->inPile = inPile;
//            newTile->inHand = inHand;
//            newTile->discarded = discarded;
//            newTile->selected = selected;
//            newTile->selectedInSet = selectedInSet;
//            newTile->played = played;
//            newTile->pos = pos;
//            newTile->_scale = scale;
        
            deck.emplace_back(newTile);
        }
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
void TileSet::draw(const std::shared_ptr<cugl::graphics::SpriteBatch>& batch, cugl::Size size){
    for(const auto& it : deck){
        Tile curr = (*it);
        Vec2 pos = curr.pos;
        
        if ((pos.x == 0.0f && pos.y == 0.0f) || it->played) {
            continue;
        }
        
        if(it->selected && it->inHand){
            pos.y = curr.pos.y + 10;
        }
        if(it->selected && (it->inPile || it->discarded)){
            it->_scale = 0.25;
        }
        else{
            it->_scale = 0.2;
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
        currTile->appendValue("inHand", tile->inHand);
        currTile->appendValue("discarded", tile->discarded);
        currTile->appendValue("selected", tile->selected);
        currTile->appendValue("selectedInSet", tile->selectedInSet);
        currTile->appendValue("played", tile->played);
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
