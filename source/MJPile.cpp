
//
//  MJPile.cpp
//  Mahsjong
//
//  Created by Patrick Choo on 2/18/25.
//

#include "MJPile.h"
#include "MJPlayer.h"
#include "MJAnimationController.h"

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
bool Pile::initPile(int size, std::shared_ptr<TileSet> tileSet, bool isHost) {
    _tileSet = tileSet;
    _pileSize = size;
    _pile.clear();
    _draw.clear();
    _pairs.clear();
    if(isHost){
        createPile();
    }
    else{
        createEmptyPile();
    }
    return true;
}

/**
 * Creates a new pile according to size. Initializes all pile elements to 0
 *
 * @return true if pile was created successfully, and false otherwise
 */
bool Pile::createEmptyPile(){
    if (!(_pile.size() == 0)){
        return false;
    }
    for (int i = 0; i < _pileSize; i++) {
        std::vector<std::shared_ptr<TileSet::Tile>> row;
        for (int j = 0; j < _pileSize; j++) {
            row.push_back(nullptr);
        }
        _pile.push_back(row);
    }
    
    return true;
}

/**
 * Creates a new pile and positions them for drawing to the screen
 *
 * @return true if the pile was created successfully, and false otherwise
 */
bool Pile::createPile() {
    _pile.clear();
    
    // Variable to keep track of what index we are on for pile creation
    int index = 0;
    
    // Iterate through the pile
    for (int i = 0; i < _pileSize; i++) {
        std::vector<std::shared_ptr<TileSet::Tile>> row;
        for (int j = 0; j < _pileSize; j++) {
            if (_tileSet->deck.size() <= 0) { //If our deck is empty, set the rest of the _pile to be empty (deck.empty() instead?)
                row.push_back(nullptr);
                continue;
            }
            
            std::shared_ptr<TileSet::Tile> tile = _tileSet->deck[index];
            
            tile->_scale = 0.2;
            tile->inPile = true;
            tile->pileCoord = cugl::Vec2(i, j);
            tile->inDeck = false; 

            row.push_back(_tileSet->deck[index]);
            index += 1;
        }
        _pile.push_back(row); //add tile from deck to pile
    }
    
    updateTilePositions();
    
    // Erase tiles put into the pile from deck
    if(_tileSet->deck.size() <= 25){
        _tileSet->deck.clear();
    }
    else{
        _tileSet->deck.erase(_tileSet->deck.begin(), _tileSet->deck.begin() + index + 1);
    }
    
    return true;
}

void Pile::updateTilePositions() {
    cugl::Size screenSize(1280, 720); //Temporary pile placement fix

    float spacingY = 0.85f;
    float spacingX = 0.8f;
    
    _pileMap.clear();
    
    for (int i = 0; i < _pileSize; i++) {
        for (int j = 0; j < _pileSize; j++) {
            std::shared_ptr<TileSet::Tile> tile = _pile[i][j];
            if (tile == nullptr) continue;
            
            cugl::Size _size = tile->getTileTexture()->getSize();
            cugl::Vec2 tileSize(_size.width * tile->_scale, _size.height * tile->_scale);
            
            float pileWidth = (_pileSize - 1) * (tileSize.x * spacingX);
            float pileHeight = (_pileSize - 1)* (tileSize.y * spacingY);
            cugl::Vec2 pileOffset((screenSize.width - pileWidth) / 2, (screenSize.height - pileHeight) / 2);
            
            float x = j * tileSize.x * spacingX;
            float y = i * tileSize.y * spacingY;
            
            if (!AnimationController::getInstance().isTileAnimated(tile)) {
                AnimationController::getInstance().addSelectAnim(tile, tile->pos, (Vec2(x, y) + pileOffset), tile->_scale, tile->_scale, 20, false);
            }

            std::string key = tile->toString() + " " + std::to_string(tile->_id);
            _pileMap.insert({key, tile->pileCoord});
        }
    }
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
    return _draw;
}

void Pile::removePileTile(const std::shared_ptr<cugl::JsonValue> tileJson, bool isHostDraw){
    for(auto const& tileKey : tileJson->children()){
        const std::string suit = tileKey->getString("suit");
        const std::string rank = tileKey->getString("rank");
        const std::string id = tileKey->getString("id");
        
        const std::string actionType = tileKey->getString("actionType", "None");
        const std::string commandType = tileKey->getString("commandType", "None");
        
        std::string key = rank + " of " + suit + " " + id;
        
        if(actionType != "None"){
            key = actionType + " " + id;
        }
        else if (commandType != "None") {
            key = commandType + " " + id;
        }
        CULog("%s", key.c_str());
        
        int x = _pileMap[key].x;
        int y = _pileMap[key].y;
        
        if(_pile[x][y] == nullptr){
            continue;
        }
        _pile[x][y]->inPile = false;
        if(isHostDraw){
            _pile[x][y]->inHostHand = true;
            _pile[x][y]->inClientHand = false;
        }
        else{
            _pile[x][y]->inHostHand = false;
            _pile[x][y]->inClientHand = true;
        }
        _pile[x][y] = nullptr;
        _pileMap.erase(key);
    }
}


void Pile::remakePile(){
    if(_pileMap.size() != 0){
        CUAssert("Must have an empty pile before remaking pile");
    }
    
    for(auto const& pairs : _tileSet->tileMap) {
        std::shared_ptr<TileSet::Tile> currTile = pairs.second;
        if(currTile->inPile) {
            std::string key = currTile->toString() + " " + std::to_string(currTile->_id);
            int x = currTile->pileCoord.x;
            int y = currTile->pileCoord.y;
            _pile[x][y] = currTile;
            _pileMap.insert({key, currTile->pileCoord});
        }
    }
}

/**
 * Draws the pile to the screen
 *
 * @param batch     the SpriteBatch to render to the screen
 */
void Pile::draw(const std::shared_ptr<cugl::graphics::SpriteBatch>& batch) {
    for(const auto& row : _pile) {
        for (const auto& tile : row) {
            if(tile == nullptr){
                continue;
            }
            cugl::Vec2 origin = cugl::Vec2(tile->getTileTexture()->getSize().width/2, tile->getTileTexture()->getSize().height/2);
            
            cugl::Affine2 trans;
            trans.scale(tile->_scale);
            trans.translate(tile->pos);
            
            cugl::Size textureSize(350.0, 415.0);
            cugl::Vec2 rectOrigin(tile->pos - (textureSize * tile->_scale)/2);
            tile->tileRect = cugl::Rect(rectOrigin, textureSize * tile->_scale);

            batch->draw(tile->getTileTexture(), origin, trans);
        }
    }
}

void Pile::reshufflePile(){
    std::vector<std::shared_ptr<TileSet::Tile>> tiles;
    for (int i = 0; i < _pileSize; i++) {
        for (int j = 0; j < _pileSize; j++) {
            if (_pile[i][j] != nullptr) {
                tiles.push_back(_pile[i][j]);
            }
        }
    }
    
    rdPile.init();
    rdPile.shuffle(tiles);
    
    int index = 0;
    for (int i = 0; i < _pileSize; i++) {
        for (int j = 0; j < _pileSize; j++) {
            if (_pile[i][j] != nullptr) {
                _pile[i][j] = tiles[index];
                _pile[i][j]->pileCoord = cugl::Vec2(i, j);
                index++;
            }
        }
    }
    
    CULog("pile reshuffled");
}

void Pile::removeNumTiles(int nums) {
    for(int i = 0; i < _pileSize; i++) {
        for(int j = 0; j < _pileSize; j++) {
            if(nums == 0){
                break;
            }
            else if (_pile[i][j] == nullptr) {
                continue;
            }
            _pile[i][j] = nullptr;
            nums -= 1;
        }
    }
    CUAssertLog(nums == 0, "Did not delete correct amount of tiles");
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


