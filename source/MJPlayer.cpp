//
//  MJPlayer.cpp
//  Mahsjong
//
//  Created by Patrick Choo on 2/18/25.

#include "MJPlayer.h"

using namespace cugl;

/**
 * This is the class intializing all player features and interactions
 *
 * TODO: Please implement all player interactions
 *
 */

Hand::Hand(Player* player) {
    _player = player;
}

/**
 initializes the hand with 14 cards
 */
bool Hand::init(std::shared_ptr<TileSet>& tileSet){
    // draw from the deck
    for(int i = 0; i<14; i++){
        std::shared_ptr<TileSet::Tile> drawnTile = tileSet->deck[i]; // gets the reference to the pointer.
        drawnTile->inHand = true;
        drawnTile->_scale = 0.2;
        _tiles.push_back(drawnTile);
    }
    
    return true;
}

/**
 This draws from the pile.
 It updates the global attributes inHand and _tiles.
 */
void Hand::drawFromPile(std::shared_ptr<Pile>& pile){
    
    // prevent from going over 14 tiles in a hand
    if(_tiles.size() >= 14){
         return;
     }

    // drawing from the pile.
    std::vector<std::shared_ptr<TileSet::Tile>>  drawnTiles = pile->tilesDrawn(static_cast<int>(14) - static_cast<int>(_tiles.size()));
    CULog("How many tiles I need %d", static_cast<int>(14) - static_cast<int>(_tiles.size()));
    CULog("How many tiles I draw %zu", drawnTiles.size());
    
    // if you draw more tiles than you need -- return.
    if(drawnTiles.size() != static_cast<int>(14) - static_cast<int>(_tiles.size())){
        return;
    }
    
    for(auto& tile : drawnTiles){
        
        tile->inHand = true;
        _tiles.push_back(tile);
    }
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
 Method which confirms that A set (singular set) is valid, and marks it as selected and selectedInSet.
 
 TODO: how am I gonna select multiple sets? This function selects a single set and adds to a set of selectedSets. do I unselect immediately?
 */
bool Hand::makeSet(){
    
    // check if each individual set is valid
    if(!isSetValid(_selectedTiles)){
        
        // if set is not valid -- empty the set of selected tiles.
        _selectedTiles.clear();
        return false;
    }
    
    // marks tiles as selected and selectedInSet
    for (auto& tile : _selectedTiles) {
        tile->selected = true;
        tile->selectedInSet = true;
    }
    
    // adds sets to the set of selected sets.
    _selectedSets.push_back(_selectedTiles);
    _selectedTiles.clear(); // ??? I'm not sure if I should clear it here.
    
    return true;
}


/**
 Method which confirms if the sets to play exist.
 Discards played tiles
 Moves them to _playedSet and marks as played.
 Unselects the tiles.
 Records the score in global variable _score. 
 */
bool Hand::playSet(){
    
    // check if there is at least one set.
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
    }
    
    // Clear the selected tiles and unselect them.
    for (auto& set : _selectedSets) {
            for (auto& tile : set) {
                tile->selected = false;
                tile->selectedInSet = false;
            }
        }
    
    _selectedSets.clear();
        
    //count the score for the turn.
    ScoreManager scoreManager(_playedSets);
    _score = scoreManager.calculateScore();
    
    //update the total score for the level. 
    _player->_totalScore+=_score;
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
    // NOT a straight and NOT of the same kind.
    if(!isStraight(selectedTiles) && !isOfaKind(selectedTiles)){
        return false;
    }
    
    return true;
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
        
        // Not the same SUIT.
        if(tileA->getSuit() != tileB->getSuit()){
            
            // Both of them is NOT wild SUIT.
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
    for(int i = 0; i<sortedTiles.size() - 1; ++i){
        std::shared_ptr<TileSet::Tile> tileA = sortedTiles[i];
        std::shared_ptr<TileSet::Tile> tileC = sortedTiles[i+1];
        
        // should not count the wild tiles.
        if (tileC->getRank() == TileSet::Tile::Rank::WILD_RANK) {
            
            break;
            }
        
        int gap = static_cast<int>(tileC->getRank()) - static_cast<int>(tileA->getRank()) - 1;
        numGaps = numGaps + gap;

    }
    
    // if the number of gaps in bigger than the number of wild tiles -- fail.
    if(numGaps > wildTiles){
        return false;
    }
    
    return true;
}


/**
 Checks if a set is "of a kind".
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
    
  float startX = 140.0f;
  float endX = 936.0f;
  float tileSpacing = (endX-startX) / 13 + 15;
  float yPos = 100.0f;

//  CULog("Updating tile positions...");
  for (size_t i = 0; i < _tiles.size(); i++){
    cugl::Vec2 newPos(startX + i * tileSpacing, yPos);
      _tiles[i]->pos = newPos;
//    CULog("Tile %zu set to position (%f, %f)", i, newPos.x, newPos.y);
  }
}

/**
 Checks if there is a Jack in the selected set.
 */
bool Hand::hasJack(std::vector<std::shared_ptr<TileSet::Tile>> selectedTiles){
    for(std::shared_ptr<TileSet::Tile>& tile : selectedTiles){
        
        if(tile->getRank()==TileSet::Tile::Rank::WILD_RANK){
            return true;
        }
    }
    return false;
}

void Hand::clickedTile(Vec2 mousePos) {
    for (const auto& tile : _tiles) {
        if (tile) {
            if (tile->tileRect.contains(mousePos)) {
                if (tile->selected) {
                    tile->selected = false;
                } else {
                    tile->selected = true;
                }
            }
        }
    }
}

