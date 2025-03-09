//
//  MJPlayer.cpp
//  Mahsjong
//
//  Created by Patrick Choo on 2/18/25.
//

#include "MJPlayer.h"

using namespace cugl;

/**
 * This class represents a player's hand
 */
#pragma mark -
#pragma mark Constructors

/**
 * Creates a new hand for the player
 *
 * @param player    the player whose hand we are initilaizing
 */
Hand::Hand(Player* player) {
    _player = player;
}

/**
 * Initializes a new player hand by pulling 14 tiles from the game tileset
 *
 * @param tileSet   the tileset to draw from
 */
bool Hand::init(std::shared_ptr<TileSet>& tileSet){
    // draw from the deck
    for(int i = 0; i < 13; i++){
        std::shared_ptr<TileSet::Tile> drawnTile = tileSet->deck[i];
        drawnTile->inHand = true;
        drawnTile->_scale = 0.2;
        _tiles.push_back(drawnTile);
    }
    
    return true;
}

#pragma mark -
#pragma mark Gameplay Handling

/**
 * Draws how ever many cards we need from the pile
 *
 * @param pile      the pile to draw to our hand from
 */
void Hand::drawFromPile(std::shared_ptr<Pile>& pile, int number){

    std::vector<std::shared_ptr<TileSet::Tile>> drawnTiles = pile->tilesDrawn(number);
    
    for(auto& tile : drawnTiles){
        tile->inHand = true;
        _tiles.push_back(tile); // Add drawn tiles to hand
    }
 }

/**
 * Discards a single specified tile from our hand
 *
 * @param tile      the tile to discard from out hand
 */
void Hand::discard(std::shared_ptr<TileSet::Tile> tile){
    if(!tile || _tiles.size() - _selectedTiles.size() < 13){
        CULog("Discarding too many tiles or invalid tiles");
    }
    auto it = _tiles.begin();
    while(it != _tiles.end()){
        if (*it == tile) {
            // if we find the tile, discard it
            (*it)->discarded = true;
            (*it)->inHand = false;
            _tiles.erase(it);
            _discardCount++;
        } else {
            ++it;
        }
    }
}

/**
 * Counts the total number of selected tiles.
 */
int Hand::countSelectedTiles() {
    int totalTiles = 0;
    for (const auto& set : _selectedSets) {
        totalTiles += set.size();
    }
    return totalTiles;
}

/**
 * Method to make a set from your hand and add it to selected sets
 *
 * @returns true if successfully made VALID set, and false otherwise
 */
bool Hand::makeSet(){
    if(!isSetValid(_selectedTiles)){
        return false;
    }
    
    for (auto& tile : _selectedTiles) {
        tile->selected = true;
        tile->selectedInSet = true;
    }
    
    _selectedSets.push_back(_selectedTiles);
    _selectedTiles.clear();
    
    return true;
}

/**
 * Method to play a set from your hand (of 2 to 4 cards)
 *
 * @return true if a set was played sucessfully and false otherwise.
 */
bool Hand::playSet(const std::shared_ptr<TileSet>& tileSet){
    if (_selectedSets.empty()) {
           return false;
       }

    for (const auto& set : _selectedSets) {
        std::vector<std::shared_ptr<TileSet::Tile>> playedSet;
        auto it = _tiles.begin();
        
        while (it != _tiles.end()) {
            if (std::find(set.begin(), set.end(), *it) != set.end()) {
                (*it)->played = true;
                (*it)->inHand = false;
                (*it)->discarded = false; // because it was played, not discarded.
                
                for(const auto& gTile : tileSet->grandmaTiles){
                    if((*it)->toString() == (*gTile).toString()){
                        grandmaToAdd += 1;
                    }
                }
                playedSet.push_back(*it);
                it = _tiles.erase(it);
            } else {
                ++it;
            }
        }
        
        _playedSets.push_back(playedSet); // Move tiles to the playedSet
    }
    
    // Clear the selected tiles and unselect them.
    for (auto& set : _selectedSets) {
            for (auto& tile : set) {
                tile->selected = false;
                tile->selectedInSet = false;
            }
        }
    _selectedSets.clear();
    ScoreManager scoreManager(_playedSets);
    _score = scoreManager.calculateScore(); // count the score for the turn
    
    _player->_totalScore+=_score; // update level score
    _playedSets.clear();
    _player->endTurn();
    return true;
}

/**
 * Checks if the given set of tiles "selectedTiles" is valid under the game's set of rules.
 *
 * @param setTiles      the tiles to check.
 * @return true if the tiles form a VALID set, and false otherwise.
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
 * Confirms if a set isStraight.
 *
 * @param selectedTiles     the tiles to be checked for a straight
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
        for(int i = 0; i<sortedTiles.size() - 1; ++i){
            std::shared_ptr<TileSet::Tile> tileA = sortedTiles[i];
            std::shared_ptr<TileSet::Tile> tileC = sortedTiles[i+1];
            // should not count the wild tiles.
            if (tileC->getRank() == TileSet::Tile::Rank::WILD_RANK) {
                break;
                }
            int gap = static_cast<int>(tileC->getRank()) - static_cast<int>(tileA->getRank());
            if (gap != 1) {
                return false;
            }
        }

    return true;
}

/**
 * Confirms if a set if of a kind.
 *
 * @param selectedTiles     the tiles to be checked for a three/four of a kind
 */
bool Hand::isOfaKind(const std::vector<std::shared_ptr<TileSet::Tile>>& selectedTiles){
    std::shared_ptr<TileSet::Tile> tileA = selectedTiles[0];
    
    // iterate over all tiles besides the first tile.
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
 * Method to sort the tiles by Rank in ascending order.
 *
 * @param selectedTiles     a vector of selected tiles.
 * @return a vector of tiles sorted by Rank
 */
std::vector<std::shared_ptr<TileSet::Tile>> Hand::getSortedTiles(const std::vector<std::shared_ptr<TileSet::Tile>>& selectedTiles) {
    std::vector<std::shared_ptr<TileSet::Tile>> sortedTiles = selectedTiles; // creates a copy of the selectedTiles
    
    std::sort(sortedTiles.begin(), sortedTiles.end(),
              [](const std::shared_ptr<TileSet::Tile>& a,
                 const std::shared_ptr<TileSet::Tile>& b) { return a->getRank() < b->getRank();});
    return sortedTiles;
}

void Hand::updateTilePositions(){
  float startX = 140.0f; // Starting x position for hand tile positioning
  float endX = 936.0f; // Ending x position for hand tile positioning
  float tileSpacing = (endX-startX) / 13 + 15; // Spacing in x direction between tiles
  float yPos = 60.0f; // Height of hand tiles on the screen
    
  for (size_t i = 0; i < _tiles.size(); i++){
    cugl::Vec2 newPos(startX + i * tileSpacing, yPos);
      _tiles[i]->pos = newPos;
  }
}

/**
 * Method to check if selected tiles contain a wild card (jack)
 *
 * @param selectedTiles     the tiles to be checked for a wild card
 */
bool Hand::hasJack(std::vector<std::shared_ptr<TileSet::Tile>> selectedTiles){
    for(std::shared_ptr<TileSet::Tile>& tile : selectedTiles){
        
        if(tile->getRank()==TileSet::Tile::Rank::WILD_RANK){
            return true;
        }
    }
    return false;
}

/**
 * Handles selection of tiles using information from input event
 *
 * @param mousePos      the position of the mouse in this frame
 */
void Hand::clickedTile(Vec2 mousePos) {
    for (const auto& tile : _tiles) {
        if (tile) {
            if (tile->tileRect.contains(mousePos)) {
                if (tile->selected) {
                    tile->selected = false;
                    auto it = std::find(_selectedTiles.begin(), _selectedTiles.end(), tile);
                    if (it != _selectedTiles.end()) {
                        _selectedTiles.erase(it);
                    }
                } else {
                    tile->selected = true;
                    _selectedTiles.push_back(tile);
                }
            }
        }
    }
}

