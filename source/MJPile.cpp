
//
//  MJPile.cpp
//  Mahsjong
//
//  Created by Patrick Choo on 2/18/25.
//

#include "MJPile.h"

/**
 * This is the class intializing and handling the pile.
 *
 * TODO:
 * 1. Making pair support
 * 2. Draw pile
 */

 //shuffle deck every time you draw cards.

#pragma mark -
#pragma mark Initialize our Pile

/*
*   Returns the dimension of our pile (set as a square)
*/
int Pile::getPileSize() {
    return _pileSize;
}

/*
*   Returns the number of visible (non null) tiles on the pile
*/
int Pile::getVisibleSize() {
    if (_pile.empty()) {
        return 0;
    }

    int visible = 0;
    for (int i = 0; i < _pileSize; i++) {
        for (int j = 0; j < _pileSize; j++) {
            if (_pile[i][j] != nullptr) {
                visible += 1;
            }
        }
    }
    return visible;
}

/*
*   Creates a new pile given the size, with tiles drawn from deck
*/
bool Pile::initPile(int size, std::shared_ptr<TileSet> tileSet) {
    _tileSet = tileSet;

    _pileSize = size; //initiate our pile sizes
    _pile.clear(); //Make sure pile is empty
    _draw.clear();
    _pairs.clear();

    Pile::createPile();
    return true;
}

bool Pile::createPile() {
    _pile.clear();
    //if (!_tileSet->deck.empty()) {
    //    _tileSet->shuffle(); //Shuffle deck
    //}
    
    cugl::Size screenSize = cugl::Application::get()->getDisplaySize();

    // to bring tiles closer together and shifting up.
    float spacingFactor = 0.9;
    float spacingFactorX = 0.7f;
    float yShift = 100.0f;
    
    
    for (int i = 0; i < _pileSize; i++) { //collect from the deck size^2 tiles and add to the pile
        std::vector<std::shared_ptr<TileSet::Tile>> row; //Row to collect tiles
        for (int j = 0; j < _pileSize; j++) {
            
            if (_tileSet->deck.size() <= index) { //If our deck is empty, set the rest of the _pile to be empty (deck.empty() instead?)
                row.push_back(nullptr);
                continue;
            }
            
            std::shared_ptr<TileSet::Tile> tile = _tileSet->deck[index];
            cugl::Size _size = tile->getTileTexture()->getSize();
            
            tile->_scale = 0.2;
            tile->inPile = true;

            float x = j * (_size.width * tile->_scale * spacingFactorX) + (_size.width * tile->_scale / 2);
            float y = i * (_size.height * tile->_scale) + (_size.height * tile->_scale / 2);
            
            float pileWidth = _pileSize * (_size.width * tile->_scale * spacingFactorX);
            float pileHeight = _pileSize * (_size.height * tile->_scale);
            cugl::Vec2 pileOffset((screenSize.width - pileWidth) / 2, (screenSize.height - pileHeight) / 2 + yShift);
            
            tile->pos = cugl::Vec2(x * spacingFactor, y * spacingFactor) + pileOffset;
            
            row.push_back(_tileSet->deck[index]);
            index += 1;
        }
        _pile.push_back(row); //add tile from deck to pile
    }
    return true;
}

/*
*   Returns (for the hand) the requested number of tiles from the pile if it is capable
*/
std::vector<std::shared_ptr<TileSet::Tile>> Pile::tilesDrawn(int number_of_tiles) {

    _draw.clear(); //We should not be re-drawing tiles from previous plays

    for (int x = 0; x < number_of_tiles; x++) { //Collect number_of_tiles from pile, remove from pile and add to draw

        if (_pile.empty() || getVisibleSize() == 0) { //If pile ran out of tiles

            if (_tileSet->deck.size() == 14) { //If we have nothing in our deck, return what we have
                return _draw;
            }
            Pile::createPile(); //Otherwise remake the pile
        }

        for (int i = 0; i < _pileSize; i++) { //Find the first available non null tile in pile and add to draw
            for (int j = 0; j < _pileSize; j++) {

                if (_pile[i][j] != nullptr) {
                    _draw.push_back(_pile[i][j]);
                    _pile[i][j] = nullptr;
                }
            }
        }
    }
    return _draw;
}

/*
* Handling pair making. If a valid pair selected, remove from pile and return the tiles as a vector
*/
std::vector<std::shared_ptr<TileSet::Tile>> Pile::pairTile() {
    _draw.clear(); //Clear the vector we are returning
    int x = _pairs[0]->pileCoord.x; //Get the pairs posistion in the pile
    int y = _pairs[0]->pileCoord.y;
    int X = _pairs[1]->pileCoord.x;
    int Y = _pairs[1]->pileCoord.y;

    std::shared_ptr<TileSet::Tile> _tile1 = _pile[x][y];
    std::shared_ptr<TileSet::Tile> _tile2 = _pile[X][Y];
    
    if (_tile1->getRank() == _tile2->getRank() && _tile1->getSuit() == _tile2->getSuit()) { //Valid pair?
        CULog("VALID!\n");
        _draw.push_back(_tile1);
        _draw.push_back(_tile2);
        
        _tile1->inHand = true;
        _tile2->inHand = true;
        _tile1->inPile = false;
        _tile2->inPile = false;
        
        _tile1->_scale = 0;
        _tile2->_scale = 0;

        //Remove tiles from pile
        _pile[x][y] = nullptr;
        _pile[X][Y] = nullptr;
    }
    else {
        _tile1->_scale = 0.2;
        _tile2->_scale = 0.2;
        CULog("NAW!\n");
        
    }
    return _draw;
}

/*
* Used in update, determines when Player selects pairs
*/

void Pile::pairs(const cugl::Vec2 mousePos) {
    for (int i = 0; i < getPileSize(); i++) {//Loop through our pile
        for (int j = 0; j < getPileSize(); j++) {
            if (_pile[i][j] == nullptr) { //If no longer in pile
                continue;
            }
            std::shared_ptr<TileSet::Tile> _tile = _pile[i][j]; //Collect tile

            cugl::Size _size = _tile->getTileTexture()->getSize(); //Get tile posistion on pile UPDATE IF WE CHANGE HOW IT IS DRAWN
            float scale = _tile->_scale;
            float x = _tile->pos.x;
            float y = _tile->pos.y;
            float halfWidth = (_size.width * scale) / 2;
            float halfHeight = (_size.height * scale) / 2;

            if (mousePos.x >= x - halfWidth && mousePos.x <= x + halfWidth && mousePos.y >= y - halfHeight && mousePos.y <= y + halfHeight) {
                int index = 0;
                for (const auto& it : _pairs) { //Checks whether the tile we selected is already selected. if it is deselect
                    if (_tile->toString() == it->toString() && _tile->_id == it->_id) {
                        _tile->_scale = 0.2;
                        _tile->pileCoord = cugl::Vec2();
                        _pairs.erase(_pairs.begin() + index);

                        return; //If it is already in the pairs, remove it from pairs
                    }
                    index += 1;
                }

                if (_pairs.size() < 2) { //Do we have a pair selected?
                    _tile->_scale = 0.25;
                    _tile->pileCoord = cugl::Vec2(i, j);
                    _pairs.push_back(_tile);
                    
                    if(_pairs.size() == 2){
                        pairTile();
                        _pairs[0]->pileCoord = cugl::Vec2();
                        _pairs[1]->pileCoord = cugl::Vec2();
                        _pairs.clear();
                    }
                    
                }
            }
        }
    }
}







