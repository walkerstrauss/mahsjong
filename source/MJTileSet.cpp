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
    
    inDeck = true;
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
                newTile->_id = tileCount;
                normalTiles.emplace_back(newTile);
                tileCount += 1;
            }
        }
    }
    
    initCelestialTiles();
    createDeck();
    
}

/**
 * Initializes deck to a **STARTING** representation of numbered tiles
 *
 * Only call if client
 */
void TileSet::initClientDeck(const std::shared_ptr<cugl::JsonValue>& deckJson){
    std::vector<std::shared_ptr<TileSet::Tile>> allTiles = processTileJson(deckJson);
    
    for (auto& tile : allTiles) {
        tileMap[std::to_string(tile->_id)] = tile;
        deck.push_back(tile);
    }
}

void TileSet::initTutorialDeck(){
    
}

/**
 * Sets the container scene node for this tile. Attatches a texture to the polygon node (back texture)
 * and a sprite sheet for the animated node (face texture). This allows us to animate the face of the
 * texture as well as abstract the face and the tile itself.
 *
 * @param assets    the asset manager to attach our textures
 */
void TileSet::initTileNodes(const std::shared_ptr<cugl::AssetManager>& assets){
    if(deck.size() == 0){
//        CULog("Deck is empty");
        return;
    }
    for(auto& it : deck){
        initTileNode(it,assets);
    }
}


void TileSet::initCelestialTiles() {
    // ROOSTER 3
    for (int i = 1; i < 4; ++i) {
        std::shared_ptr<Tile> rooster = std::make_shared<Tile>(Tile::Rank::ROOSTER, Tile::Suit::CELESTIAL);
        tileCount++;
        rooster->_id = tileCount;
        celestialTiles.push_back(rooster);
    }
    // OX 1
        std::shared_ptr<Tile> ox = std::make_shared<Tile>(Tile::Rank::OX, Tile::Suit::CELESTIAL);
        tileCount++;
        ox->_id = tileCount;
        celestialTiles.push_back(ox);
    // RABBIT 2
    for (int i = 1; i < 3; ++i) {
        std::shared_ptr<Tile> rabbit = std::make_shared<Tile>(Tile::Rank::RABBIT, Tile::Suit::CELESTIAL);
        tileCount++;
        rabbit->_id = tileCount;
        celestialTiles.push_back(rabbit);
    }
    // SNAKE 2
    for (int i = 1; i < 3; ++i) {
        std::shared_ptr<Tile> snake = std::make_shared<Tile>(Tile::Rank::SNAKE, Tile::Suit::CELESTIAL);
        tileCount++;
        snake->_id = tileCount;
        celestialTiles.push_back(snake);
    }
    // MONKEY 2
    for (int i = 1; i < 3; ++i) {
        std::shared_ptr<Tile> monkey = std::make_shared<Tile>(Tile::Rank::MONKEY, Tile::Suit::CELESTIAL);
        tileCount++;
        monkey->_id = tileCount;
        celestialTiles.push_back(monkey);
    }
    // RAT 7
    for (int i = 1; i < 8; ++i) {
        std::shared_ptr<Tile> rat = std::make_shared<Tile>(Tile::Rank::RAT, Tile::Suit::CELESTIAL);
        tileCount++;
        rat->_id = tileCount;
        celestialTiles.push_back(rat);
    }
    // DRAGON 3
    for (int i = 1; i < 4; ++i) {
        std::shared_ptr<Tile> dragon = std::make_shared<Tile>(Tile::Rank::DRAGON, Tile::Suit::CELESTIAL);
        tileCount++;
        dragon->_id = tileCount;
        celestialTiles.push_back(dragon);
    }
    // PIG 6
    for (int i = 1; i < 7; ++i) {
        std::shared_ptr<Tile> pig = std::make_shared<Tile>(Tile::Rank::PIG, Tile::Suit::CELESTIAL);
        tileCount++;
        pig->_id = tileCount;
        celestialTiles.push_back(pig);
    }
    
}

#pragma mark -
#pragma mark Tileset Gameplay Handling
/**
 * Combines normal and celestial tiles, ensuring even distribution
 */
void TileSet::createDeck() {
    rdTileSet.init();
    rdTileSet.shuffle(normalTiles);
    rdTileSet.shuffle(celestialTiles);

    std::size_t nIdx = 0, cIdx = 0;
    
    while (cIdx < celestialTiles.size()) {
        std::shared_ptr<TileSet::Tile> celestial = celestialTiles[cIdx];
        int gap = (std::rand() % 3) + 3;
        for (int i = 0; i < gap && nIdx < normalTiles.size(); ++i) {
            std::shared_ptr<TileSet::Tile> normal = normalTiles[nIdx];
            deck.push_back(normal);
            tileMap[std::to_string(normal->_id)] = normal;
            nIdx++;
        }
        
        deck.push_back(celestial);
        tileMap[std::to_string(celestial->_id)] = celestial;
        cIdx++;

    }
    
    while (nIdx < normalTiles.size()) {
        std::shared_ptr<TileSet::Tile> normal = normalTiles[nIdx];
        deck.push_back(normalTiles[nIdx]);
        tileMap[std::to_string(normal->_id)] = normal;
        nIdx++;
    }
}
/**
 * Sets the texture for all tiles in deck
 */
void TileSet::setAllTileTexture(const std::shared_ptr<cugl::AssetManager>& assets){
    if(deck.size() == 0){
//        CULog("Deck is empty");
    }
    for(const auto& it : deck){
        it->setTexture(assets->get<Texture>(it->toString()));
        if(it->getSuit() == TileSet::Tile::Suit::CELESTIAL) {
            it->setInfoTexture(assets->get<Texture>(it->toString() + " info"));
        }
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
        std::string key = std::to_string(tile->_id);

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
        currTile->appendValue("scale", tile->_scale);
        currTile->appendValue("debuffed", tile->debuffed);

        root->appendChild(key, currTile);
    }
    
    return root;
}

void TileSet::setNextTile(std::shared_ptr<cugl::JsonValue>& nextTileJson) {
    if(nextTileJson->children().size() > 1){
//        CULog("Invalid set next tile size");
        return;
    }
    for(auto const& tileKey : nextTileJson->children()){
        const std::string id = tileKey->getString("id");
        nextTile = tileMap[id];
    }
}

void TileSet::updateDeck(const std::shared_ptr<cugl::JsonValue>& deckJson) {
    for(const auto& tileKey : deckJson->children()) {
        const std::string suit = tileKey->getString("suit");
        const std::string rank = tileKey->getString("rank");
        const std::string id = tileKey->getString("id");
        const cugl::Vec2 pileCoord = Tile::toVector(tileKey->getString("pileCoord"));
        const bool inPile = tileKey->getBool("inPile");
        const bool inHostHand = tileKey->getBool("inHostHand");
        const bool inClientHand = tileKey->getBool("inClientHand");
        const bool discarded = tileKey->getBool("discarded");
        const bool selected = tileKey->getBool("selected");
        const bool selectedInSet = tileKey->getBool("selectedInSet");
        const bool played = tileKey->getBool("played");
        const bool inDeck = tileKey->getBool("inDeck");
        const float scale = tileKey->getFloat("scale");
        const bool debuffed = tileKey->getBool("debuffed");
        
        tileMap[id]->_suit = Tile::toSuit(suit);
        tileMap[id]->_rank = Tile::toRank(rank);
        tileMap[id]->inPile = inPile;
        tileMap[id]->inHostHand = inHostHand;
        tileMap[id]->inClientHand = inClientHand;
        tileMap[id]->discarded = discarded;
        tileMap[id]->selected = selected;
        tileMap[id]->selectedInSet = selectedInSet;
        tileMap[id]->played = played;
        tileMap[id]->_scale = scale;
        tileMap[id]->debuffed = debuffed;
        tileMap[id]->pileCoord = pileCoord;
        if(inDeck == false){
            deck.erase(std::remove(deck.begin(), deck.end(), tileMap[id]), deck.end());
        }
        tileMap[id]->inDeck = inDeck;
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
        const float scale = tileKey->getFloat("scale");
        const bool debuffed = tileKey->getBool("debuffed");
        
        std::shared_ptr<Tile> newTile = std::make_shared<Tile>(rank, suit);
        
        newTile->_id = id;
        newTile->pileCoord = pileCoord;
        newTile->inPile = inPile;
        newTile->discarded = discarded;
        newTile->selected = selected;
        newTile->selectedInSet = selectedInSet;
        newTile->played = played;
        newTile->_scale = scale;
        newTile->inHostHand = inHostHand;
        newTile->inClientHand = inClientHand;
        newTile->inDeck = inDeck;
        newTile->debuffed = debuffed;
        
        tiles.push_back(newTile);
    }
    
    return tiles;
}

std::vector<std::shared_ptr<TileSet::Tile>> TileSet::processDeckJson(const std::shared_ptr<cugl::JsonValue>& deckJson){
    std::vector<std::shared_ptr<TileSet::Tile>> deck;
    int id = 0;
    
    for(const auto& tile : deckJson->children()){
        const Tile::Rank rank = Tile::toRank(tile->getString("rank"));
        const Tile::Suit suit = Tile::toSuit(tile->getString("suit"));
        
        std::shared_ptr<Tile> newTile = std::make_shared<Tile>(rank, suit);
        newTile->_id = id++;
        
        deck.push_back(newTile);
    }
    
    return deck;
}



std::shared_ptr<cugl::JsonValue> TileSet::mapToJson() {
    for(auto const& pairs : tileMap) {
        tilesToJson.push_back(pairs.second);
    }
    
    std::shared_ptr<cugl::JsonValue> tileJson = toJson(tilesToJson);
    tilesToJson.clear();
    
    return tileJson;
}
