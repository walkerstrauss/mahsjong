
//
//  MJPile.cpp
//  Mahsjong
//
//  Created by Patrick Choo on 2/18/25.
//

#include "MJPile.h"
#include "MJPlayer.h"

/**
 * This is the class intializing and handling the pile.
 */
#pragma mark -
#pragma mark Constructors
/**
 * Initializes a new layer of the pile with tiles drawn from the deck
 *
 * @param size             the size the pile should be
 * @param tileSet      the tileset to draw from to build the pile
 */
bool Pile::initPile(int size, std::shared_ptr<TileSet> tileSet) {
    _tileSet = tileSet;
    _pileSize = size;
    _pile.clear();
    _draw.clear();
    _pairs.clear();
    Pile::createPile();
    return true;
}

/**
 * Creates a new pile and positions them for drawing to the screen
 *
 * @return true if the pile was created successfully, and false otherwise
 */
bool Pile::createPile() {
    _pile.clear();
    cugl::Size screenSize = cugl::Application::get()->getDisplaySize();

    // Variables for positioning the pile tiles for drawing
    float spacingFactor = 0.85f;
    float spacingFactorX = 0.8f;
    float xShift = 125.0f;
    float yShift = 125.0f;
    
    // Iterate through the pile
    for (int i = 0; i < _pileSize; i++) {
        std::vector<std::shared_ptr<TileSet::Tile>> row;
        for (int j = 0; j < _pileSize; j++) {
            if (_tileSet->deck.size() <= index) { //If our deck is empty, set the rest of the _pile to be empty (deck.empty() instead?)
                row.push_back(nullptr);
                continue;
            }
            
            std::shared_ptr<TileSet::Tile> tile = _tileSet->deck[index];
            cugl::Size _size = tile->getTileTexture()->getSize();
            
            tile->_scale = 0.2;
            tile->inPile = true;
            tile->pileCoord = cugl::Vec2(i, j);

            float x = j * (_size.width * tile->_scale * spacingFactorX) + (_size.width * tile->_scale / 2);
            float y = i * (_size.height * tile->_scale) + (_size.height * tile->_scale / 2);
            
            float pileWidth = _pileSize * (_size.width * tile->_scale * spacingFactorX);
            float pileHeight = _pileSize * (_size.height * tile->_scale);
            cugl::Vec2 pileOffset((screenSize.width - pileWidth) / 2 + xShift, (screenSize.height - pileHeight) / 2 + yShift);
            
            tile->pos = cugl::Vec2(x * spacingFactor, y * spacingFactor) + pileOffset;
                        
            
            std::string key = tile->toString() + " " + std::to_string(tile->_id);
            _pileMap.insert({key, tile->pileCoord});
            row.push_back(_tileSet->deck[index]);
            index += 1;
        }
        _pile.push_back(row); //add tile from deck to pile
    }
    return true;
}

#pragma mark -
#pragma mark Gameplay Handling

/**
 * Method to get the tiles drawn from the pile for the player
 *
 * @param number_of_tiles   the number of tiles to draw from the pile
 * @return a vector of tiles to add to the player hand
 */
std::vector<std::shared_ptr<TileSet::Tile>> Pile::tilesDrawn(int number_of_tiles) {
    _draw.clear(); //We should not be re-drawing tiles from previous plays

    while(_draw.size() < number_of_tiles){
        if (_pile.empty() || getVisibleSize() == 0) { //If pile ran out of tiles
            if (_tileSet->deck.size() == 14) { //If we have nothing in our deck, return what we have
                return _draw;
            }
            Pile::createPile(); //Otherwise remake the pile
        } else {
            for (int i = 0; i < _pileSize; i++) { //Find the first available non null tile in pile and add to draw
                for (int j = 0; j < _pileSize; j++) {
                    if (_pile[i][j] != nullptr && _draw.size() < number_of_tiles) {
                        _pile[i][j]->inPile = false;
                        _pile[i][j]->selected = false;
                        
                        _tileSet->tilesToJson.push_back(_pile[i][j]);

                        _draw.push_back(_pile[i][j]);
                        _pile[i][j] = nullptr;
                    }
                }
            }
        }
    }
    return _draw;
}

void Pile::removePileTile(const std::shared_ptr<cugl::JsonValue> tileJson){
    for(auto const& tileKey : tileJson->children()){
        const std::string suit = tileKey->getString("suit");
        const std::string rank = tileKey->getString("rank");
        const std::string id = tileKey->getString("id");
        
        std::string key = rank + " of " + suit + " " + id;
        int x = _pileMap[key].x;
        int y = _pileMap[key].y;
        
        _pile[x][y]->inPile = false;
        _pile[x][y] = nullptr;
        _pileMap.erase(key);
    }
}

///**
// * Method to handle pair making for the pile, including removing from pile and returning removed tiles
// *
// * @param player    the player whose hand the tiles in the pair are being drawn to
// * @return a vector of tiles in the pair
// */
//std::vector<std::shared_ptr<TileSet::Tile>> Pile::pairTile(const std::shared_ptr<Player>& player) {
//    int x = _pairs[0]->pileCoord.x; //Get the pairs posistion in the pile
//    int y = _pairs[0]->pileCoord.y;
//    int X = _pairs[1]->pileCoord.x;
//    int Y = _pairs[1]->pileCoord.y;
//
//    std::shared_ptr<TileSet::Tile> _tile1 = _pile[x][y];
//    std::shared_ptr<TileSet::Tile> _tile2 = _pile[X][Y];
//    
//    if (_tile1->getRank() == _tile2->getRank() && _tile1->getSuit() == _tile2->getSuit()) { //Valid pair?
//        CULog("VALID!\n");
//        if (!player->discarding){
//            player->discarding = true;
//            for (auto& tile : player->getHand()._selectedTiles){
//                player->getHand().discard(tile);
//                tile->selected = false;
//                tile->inHand = false;
//                tile->inPile = false;
//            }
//            player->getHand()._selectedTiles.clear();
//            player->discarding = false;
//            
//            player->getHand()._tiles.push_back(_tile1);
//            player->getHand()._tiles.push_back(_tile2);
//            
//            _tile1->inHand = true;
//            _tile2->inHand = true;
//    
//            _tile1->inPile = false;
//            _tile2->inPile = false;
//            
//            _tile1->selected = false;
//            _tile2->selected = false;
//
//            //Remove tiles from pile
//            _pile[x][y] = nullptr;
//            _pile[X][Y] = nullptr;
//        }
//    }
//    else {
//        for (auto& tile : player->getHand()._selectedTiles){
//            tile->selected = false;
//        }
//        player->getHand()._selectedTiles.clear();
//        _tile1->selected = false;
//        _tile2->selected = false;
//        CULog("NAW!\n");
//    }
//    return _draw;
//}

///**
// * Method to check if the player has selected two tiles that form a pair and handle pairs
// *
// * @param player    the player for the game
// */
//void Pile::pairs(const cugl::Vec2 mousePos, const std::shared_ptr<Player>& player) {
//    for (int i = 0; i < getPileSize(); i++) {//Loop through our pile
//        for (int j = 0; j < getPileSize(); j++) {
//            if (_pile[i][j] == nullptr) { //If no longer in pile
//                continue;
//            }
//            std::shared_ptr<TileSet::Tile> _tile = _pile[i][j]; //Collect tile
//        
//            if (_tile->tileRect.contains(mousePos)) {
//                int index = 0;
//                for (const auto& it : _pairs) { //Checks whether the tile we selected is already selected. if it is deselect
//                    if (_tile->toString() == it->toString() && _tile->_id == it->_id) {
//                        _tile->pileCoord = cugl::Vec2();
//                        _pairs.erase(_pairs.begin() + index);
//                        return; //If it is already in the pairs, remove it
//                    }
//                    index += 1;
//                }
//                if (_pairs.size() < 2) { //Do we have a pair selected?
//                    _tile->pileCoord = cugl::Vec2(i, j);
//                    _pairs.push_back(_tile);
//                }
//            }
//        }
//    }
//    // Handle pair selection
//    if(_pairs.size() == 2){
//        if(player->getHand()._selectedTiles.size() == 2){
//            pairTile(player);
//            _pairs[0]->pileCoord = cugl::Vec2();
//            _pairs[1]->pileCoord = cugl::Vec2();
//            _pairs.clear();
//        }
//    }
//}
