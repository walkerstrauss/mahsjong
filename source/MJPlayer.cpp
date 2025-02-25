//
//  MJPlayer.cpp
//  Mahsjong
//
//  Created by Patrick Choo on 2/18/25.
//  Modified by Mariia Tiulchenko on 2/22/25

#include "MJPlayer.h"

using namespace cugl;

/**
 * This is the class intializing all player features and interactions
 *
 * TODO: Please implement all player interactions
 *
 */

/**
 initializes the hand with 14 cards
 */
bool Hand::init(std::shared_ptr<TileSet>& tileSet){
    
    tileSet->shuffle();
    
    // draw from the deck
    for(int i = 0; i<14; i++){
        std::shared_ptr<TileSet::Tile> drawnTile = tileSet->deck[i]; // gets the reference to the pointer.
        drawnTile->inHand = true;
        drawnTile->_scale = 0.2;
        _tiles.push_back(drawnTile);
    }
    
    updateTilePositions();
    
    return true;
}


/**
 This is a function which just adds tiles from undefined sources.
 It updates the attribute inHand and _tiles.
 
 
 TODO: should it be drawing selected sets of tiles, instead of individual tiles?
 
 TODO: is this enough for the drawing method?
 TODO: do I need to "pop" from the pile and from the deck?
 TODO: And the mecahincs of drawing from the pile
 */
void Hand::draw(std::shared_ptr<TileSet::Tile> tile){
    
    // prevent from going over 14 tiles in a hand
    if(!tile || _tiles.size() >= 14){
         return;
     }
    // TODO: draw from the pile
    
     // A tile is now in the hand.
     tile->inHand = true;
     
     // A tile is added to the current hand.
     _tiles.push_back(tile);
    
    // TODO: pop from the pile
 }

/**
 A discard method which allows to discard up to 5 tiles per turn.
 
 TODO: does not track the number of discard per level.
 TODO: is not responible for discarding cards when drawing from pile.
 
 TODO: should I rewrite it to discard selected sets of tiles, instead of individual tiles?
 */
bool Hand::discard(std::shared_ptr<TileSet::Tile> tile){
    
    // can discard up to 5 tiles per turn
    if(!tile || _discardCount >= 5){
        return false;
    }
    
    // iterate over the set of tiles, and erase one.
    auto it = _tiles.begin();
    while(it != _tiles.end()){
        if (*it == tile) {
            
            (*it)->discarded = true;
            (*it)->inHand = false;
            
            _tiles.erase(it);
            _discardCount++;
            
            return true;
            
        } else {
            
            ++it;
        }
    }
    
    return false;
}

/**
 Method which confirms if a selected set of tiles is valid and marks the tiles as selected.
 */
bool Hand::makeSet(const std::vector<std::shared_ptr<TileSet::Tile>>& selectedTiles){
    
    // check if each individual set is valid
    if(!isSetValid(selectedTiles)){
        return false;
    }
    
    // doesn't allow more than 14 tiles to be selected
    if (countSelectedTiles() + selectedTiles.size() > 14) {
        return false;
    }
    
    // marks tiles as selected and selectedInSet
    for (auto& tile : selectedTiles) {
        tile->selected = true;
        tile->selectedInSet = true;
    }
    
    // adds sets to the set of selected sets.
    _selectedSets.push_back(selectedTiles);
    
    return true;
}


/**
 Method which confirms if the sets to play exist.
 Discards played tiles -- moves them to _playedSet and marks as played.
 TODO: record the score here?
 TODO: "move" the tiles somewhere?
 */
bool Hand::playSet(){
    
    // check if the set of sets is not empty
    if (_selectedSets.empty()) {
           return false;
       }
    
    // remove tiles from the hand using a while loop
    for (const auto& set : _selectedSets) {
        
        std::vector<std::shared_ptr<TileSet::Tile>> playedSet;
        
        auto it = _tiles.begin();
        while (it != _tiles.end()) {
            if (std::find(set.begin(), set.end(), *it) != set.end()) {
                (*it)->played = true;
                (*it)->inHand = false;
                (*it)->discarded = false; // because it was played, not discarded.
                
                playedSet.push_back(*it);
                it = _tiles.erase(it);
            } else {
                ++it;
            }
        }
        
        // Move tiles to the playedSet
        _playedSets.push_back(playedSet);
        
        // TODO: record a score here?
        
    }
    
    //Clear the selected tiles and unselected them.
    for (auto& set : _selectedSets) {
            for (auto& tile : set) {
                tile->selected = false;
                tile->selectedInSet = false;
            }
        }
    
    _selectedSets.clear();
    
    return true;
}

/**
 Confirms if each individual set is valid.
 TODO: should selectedTiles be a global attribute defined in header?
 */
bool Hand::isSetValid(const std::vector<std::shared_ptr<TileSet::Tile>>& selectedTiles){
    
    // check if the size of the set is valid.
    // only pairs, 3 or 4-tile sets.
    if(selectedTiles.size() < 2 || selectedTiles.size() > 4){
        
        return false;
    }
    
    // check that all tiles are unique references.
    for(size_t it = 0; it < selectedTiles.size(); it++){
        for(size_t jt = it+1; jt < selectedTiles.size(); jt++){
            if(selectedTiles[it]==selectedTiles[jt]){
                
                return false;
            }
        }
    }

    return true;
}

/**
 Counts the number of selected tiles.
 Is a helper function.
 */
int Hand::countSelectedTiles() {
    int totalTiles = 0;
    for (const auto& set : _selectedSets) {
        totalTiles += set.size();
    }
    return totalTiles;
}



/**
 Checks if a set if straight.
 */
bool Hand::isStraight(const std::vector<std::shared_ptr<TileSet::Tile>>& selectedTiles){
    
    int wildTiles = 0; // only jacks.
    
    std::vector<std::shared_ptr<TileSet::Tile>> sortedTiles = getSortedTiles(selectedTiles);
    
    // the base tile.
    std::shared_ptr<TileSet::Tile> tileA = sortedTiles[0];
    
    // check if the base tile wild RANK. (jacks)
    if(tileA->getRank()==TileSet::Tile::Rank::WILD_RANK){
        wildTiles++;
    }
    
    // check if the sorted selectedTiles are of the same SUIT.
    for(int i = 1; i<sortedTiles.size(); ++i){
        
        std::shared_ptr<TileSet::Tile> tileB = sortedTiles[i];
        
        if(tileA->getSuit() != tileB->getSuit()){
            
            // Not the same SUIT.
            if(tileA->getSuit() != TileSet::Tile::Suit::WILD_SUIT &&
               tileB->getSuit() != TileSet::Tile::Suit::WILD_SUIT){
                
                return false;
            }
            
        }
        // check if there any wild tiles.
        if(tileB->getRank()==TileSet::Tile::Rank::WILD_RANK){
            wildTiles++;
        }
    }
    
    
    // check if the selectedTiles are consequitive.
    
    int numGaps = 0;
    bool usedWild = false;
    
    for(int i = 0; i<sortedTiles.size(); ++i){
        
        std::shared_ptr<TileSet::Tile> tileA = sortedTiles[i];
        std::shared_ptr<TileSet::Tile> tileC = sortedTiles[i+1];
        
        // should not count the wild tiles.
        if (tileC->getRank() == TileSet::Tile::Rank::WILD_RANK) {
            
            break;
            }
        
        int gap = static_cast<int>(tileC->getRank()) - static_cast<int>(tileA->getRank()) - 1;
        numGaps = numGaps + gap;

    }
    
    // if the number of gaps in smaller than the number of wild tiles -- fail.
    if(numGaps < wildTiles){
        return false;
    }
    
    return true;
}



/**
 Checks if a set is "of a kind".
 Returns true, if so.
 Returns false, if it is not "3/4 of a kind".
 */
bool Hand::isOfaKind(const std::vector<std::shared_ptr<TileSet::Tile>>& selectedTiles){
    
    // the base tile.
    std::shared_ptr<TileSet::Tile> tileA = selectedTiles[0];
    
    // iterate over all tiles minus the first tile.
    for(int j=1; j<selectedTiles.size(); ++j){
        
        std::shared_ptr<TileSet::Tile> tileB = selectedTiles[j];
            
        // Not the same SUIT.
        if(tileA->getSuit() != tileB->getSuit()){
            // Neither is wild SUIT.
            if(tileA->getSuit() != TileSet::Tile::Suit::WILD_SUIT &&
               tileB->getSuit() != TileSet::Tile::Suit::WILD_SUIT){
                return false;
                    
            // One is wild SUIT.
            }else if(tileA->getRank() != tileB->getRank() &&
                     tileA->getRank() != TileSet::Tile::Rank::WILD_RANK &&
                     tileB->getRank() != TileSet::Tile::Rank::WILD_RANK){
                    return false;
                }
                
        }
        // Not the same RANK.
        if(tileA->getRank() != tileB->getRank()){
            // Neither is wild RANK.
            if(tileA->getRank() != TileSet::Tile::Rank::WILD_RANK &&
               tileB->getRank() != TileSet::Tile::Rank::WILD_RANK){
                return false;
                
            // One is wild RANK.
            }else if(tileA->getSuit() != tileB->getSuit() &&
                     tileA->getSuit() != TileSet::Tile::Suit::WILD_SUIT &&
                     tileB->getSuit() != TileSet::Tile::Suit::WILD_SUIT){
                return false;
            }
        }
    }
        
    return true;

}


/**
 Creates a copy of selectedTiles and sorts it.
 */
std::vector<std::shared_ptr<TileSet::Tile>> Hand::getSortedTiles(const std::vector<std::shared_ptr<TileSet::Tile>>& selectedTiles) {
    
    // creates a copy of the selectedTiles.
    std::vector<std::shared_ptr<TileSet::Tile>> sortedTiles = selectedTiles;
    
    std::sort(sortedTiles.begin(), sortedTiles.end(),
              [](const std::shared_ptr<TileSet::Tile>& a,
                 const std::shared_ptr<TileSet::Tile>& b) { return a->getRank() < b->getRank();});
    
    return sortedTiles;
}

void Hand::updateTilePositions(){
    float startX = 128.0f;
    float endX = 896.0f;
    float tileSpacing = (endX-startX) / 13;
    
    float yPos = 100.0f;
    
    CULog("Updating tile positions...");
    
    for (size_t i = 0; i < _tiles.size(); i++){
        Vec2 newPos(startX + i * tileSpacing, yPos);
        _tiles[i]->pos = newPos;
        
        CULog("Tile %zu set to position (%f, %f)", i, newPos.x, newPos.y);
    }
}

void Hand::draw(const std::shared_ptr<cugl::graphics::SpriteBatch>& batch){
    for (const auto& tile : _tiles){
        Size size = tile->getTileTexture()->getSize();
        Vec2 pos = tile->pos;
        Vec2 origin(size.width/2, size.height/2);
        
        Affine2 transform;
        transform.scale(tile->_scale);
        transform.translate(pos);
        if (tile->getTileTexture()){
            batch->draw(tile->getTileTexture(), origin, transform);
        }
    }
}
