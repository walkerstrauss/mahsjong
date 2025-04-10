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
bool Hand::initHand(std::shared_ptr<TileSet>& tileSet, bool isHost){
    _size = 13;
    // draw from the deck
    for(int i = 0; i < _size; i++){
        std::shared_ptr<TileSet::Tile> drawnTile = tileSet->deck[i];
        if(isHost) {
            drawnTile->inHostHand = true;
            drawnTile->inClientHand = false;
        }
        else{
            drawnTile->inHostHand = false;
            drawnTile->inClientHand = true;
        }
        drawnTile->_scale = 0.2;
        drawnTile->inDeck = false; 
        _tiles.push_back(drawnTile);
    }
    
    tileSet->deck.erase(tileSet->deck.begin(), tileSet->deck.begin() + 13);
    return true;
}

#pragma mark -
#pragma mark Gameplay Handling

/**
 * Draws how ever many cards we need from the pile
 *
 * @param pile      the pile to draw to our hand from
 */
void Hand::drawFromPile(std::shared_ptr<Pile>& pile, int number, bool isHost){
    _drawnPile = pile->tilesDrawn(number);
    for(auto& tile : _drawnPile){
        if (isHost) {
            tile->inHostHand = true;
            tile->inClientHand = false;
        } else {
            tile->inClientHand = true;
            tile->inHostHand = false; 
        }
        tile->inPile = false;
        tile->selected = false;
        tile->discarded = false;
        _tiles.push_back(tile); // Add drawn tiles to hand
    }
 }

void Hand::drawFromDiscard(std::shared_ptr<TileSet::Tile> tile, bool isHost) {
    if (!tile) {
        return;
    }
    
    if (isHost) {
        tile->inHostHand = true;
    } else {
        tile->inClientHand = true;

    }
    tile->discarded = false;
    tile->inPile = false;
    tile->selected = false; 
    _tiles.push_back(tile);
}

/**
 * Discards a single specified tile from our hand
 *
 * @param tile      the tile to discard from out hand
 */
bool Hand::discard(std::shared_ptr<TileSet::Tile> tile, bool isHost){
    auto it = _tiles.begin();
    while(it != _tiles.end()){
        if (*it == tile) {
            // if we find the tile, discard it
            (*it)->discarded = true;
            if (isHost) {
                (*it)->inHostHand = false;
            } else {
                (*it)->inClientHand = false;
            }
            (*it)->inPile = false;
            (*it)->selected = false;

            _tiles.erase(it);
        } else {
            ++it;
        }
    }
    return true;
}

void Hand::loseActions(bool isHost) {
    auto it = _tiles.begin();
    while(it != _tiles.end()){
        if ((*it)->getRank() == TileSet::Tile::Rank::ACTION) {
            discard(*it, isHost);
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
bool Hand::playSet(const std::shared_ptr<TileSet>& tileSet, bool isHost){
    if (_selectedSets.empty()) {
           return false;
       }

    for (const auto& set : _selectedSets) {
        std::vector<std::shared_ptr<TileSet::Tile>> playedSet;
        auto it = _tiles.begin();
        
        while (it != _tiles.end()) {
            if (std::find(set.begin(), set.end(), *it) != set.end()) {
                (*it)->played = true;
                if (isHost) {
                    (*it)->inHostHand = false;
                } else {
                    (*it)->inClientHand = false;

                }
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
//    ScoreManager scoreManager(_playedSets);
//    _score = scoreManager.calculateScore(); // count the score for the turn
    
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
//    if(!isStraight(selectedTiles) && !isOfaKind(selectedTiles)){
//        return false;
//    }
    
    return true;
}

/**
 * Confirms if a set isStraight.
 *
 * @param selectedTiles     the tiles to be checked for a straight
 */
//bool Hand::isStraight(const std::vector<std::shared_ptr<TileSet::Tile>>& selectedTiles){
//    
//    int wildTiles = 0; // only jacks.
//    
//    std::vector<std::shared_ptr<TileSet::Tile>> sortedTiles = getSortedTiles(selectedTiles);
//    
//    // the base tile.
//    std::shared_ptr<TileSet::Tile> tileA = sortedTiles[0];
//    
////    // check if the base tile wild RANK. (jacks)
////    if(tileA->getRank()==TileSet::Tile::Rank::WILD_RANK){
////        wildTiles++;
////    }
//    
//    // check if the sorted selectedTiles are of the same SUIT.
//    for(int i = 1; i<sortedTiles.size(); ++i){
//        
//        std::shared_ptr<TileSet::Tile> tileB = sortedTiles[i];
//        
//        // Not the same SUIT.
//        if(tileA->getSuit() != tileB->getSuit()){
//            
//            // Both of them is NOT wild SUIT.
//            if(tileA->getSuit() != TileSet::Tile::Suit::WILD_SUIT &&
//               tileB->getSuit() != TileSet::Tile::Suit::WILD_SUIT){
//                
//                return false;
//            }
//            
//        }
//        
//        // check if there any wild tiles.
//        if(tileB->getRank()==TileSet::Tile::Rank::WILD_RANK){
//            wildTiles++;
//        }
//    }
//    
//    
//    // check if the selectedTiles are consequitive.
//        for(int i = 0; i<sortedTiles.size() - 1; ++i){
//            std::shared_ptr<TileSet::Tile> tileA = sortedTiles[i];
//            std::shared_ptr<TileSet::Tile> tileC = sortedTiles[i+1];
//            // should not count the wild tiles.
//            if (tileC->getRank() == TileSet::Tile::Rank::WILD_RANK) {
//                break;
//                }
//            int gap = static_cast<int>(tileC->getRank()) - static_cast<int>(tileA->getRank());
//            if (gap != 1) {
//                return false;
//            }
//        }
//
//    return true;
//}

///**
// * Confirms if a set if of a kind.
// *
// * @param selectedTiles     the tiles to be checked for a three/four of a kind
// */
//bool Hand::isOfaKind(const std::vector<std::shared_ptr<TileSet::Tile>>& selectedTiles){
//    std::shared_ptr<TileSet::Tile> tileA = selectedTiles[0];
//    
//    // iterate over all tiles besides the first tile.
//    for(int j=1; j<selectedTiles.size(); ++j){
//        
//        std::shared_ptr<TileSet::Tile> tileB = selectedTiles[j];
//        
//        // Not the same SUIT.
//        if(tileA->getSuit() != tileB->getSuit()){
//            // Neither is wild SUIT.
//            if(tileA->getSuit() != TileSet::Tile::Suit::WILD_SUIT &&
//               tileB->getSuit() != TileSet::Tile::Suit::WILD_SUIT){
//                return false;
//                    
//            // One is wild SUIT.
//            }else if(tileA->getRank() != tileB->getRank() &&
//                     tileA->getRank() != TileSet::Tile::Rank::WILD_RANK &&
//                     tileB->getRank() != TileSet::Tile::Rank::WILD_RANK){
//                    return false;
//                }
//                
//        }
//        // Not the same RANK.
//        if(tileA->getRank() != tileB->getRank()){
//            // Neither is wild RANK.
//            if(tileA->getRank() != TileSet::Tile::Rank::WILD_RANK &&
//               tileB->getRank() != TileSet::Tile::Rank::WILD_RANK){
//                return false;
//                
//            // One is wild RANK.
//            }else if(tileA->getSuit() != tileB->getSuit() &&
//                     tileA->getSuit() != TileSet::Tile::Suit::WILD_SUIT &&
//                     tileB->getSuit() != TileSet::Tile::Suit::WILD_SUIT){
//                return false;
//            }
//        }
//    }
//    return true;
//}

bool Hand::isWinningHand() {
    
    if (_tiles.size() == _size + 1) {
        std::vector<std::shared_ptr<TileSet::Tile>> sortedHand = getSortedTiles(_tiles);
        
        std::map<std::pair<TileSet::Tile::Rank, TileSet::Tile::Suit>, int> tileCounts;
        for (const auto& tile : sortedHand) {
            tileCounts[{tile->getRank(), tile->getSuit()}]++;
        }
        
        return onePairFourSets(tileCounts, 0, 0);
    }
    return false;
}

bool Hand::onePairFourSets(std::map<std::pair<TileSet::Tile::Rank, TileSet::Tile::Suit>, int>& tileCounts, int pair, int sets) {
    
    if (pair == 1 && sets == _size / 3) {
        return true;
    }
    
    for (auto& [tile, count] : tileCounts) {
        
        if (count == 0) continue;
        
        // Make Pong
        if (count >= 3) {
            tileCounts[tile] -= 3;
            if (onePairFourSets(tileCounts, pair, sets + 1)) {
                return true;
            }
            tileCounts[tile] += 3;
        }
        
        // Make Pair
        if (count >= 2) {
            tileCounts[tile] -= 2;
            if (onePairFourSets(tileCounts, pair + 1, sets)) {
                return true;
            }
            tileCounts[tile] += 2;
        }
        
        // Make Chow
        TileSet::Tile::Rank rank = tile.first;
        TileSet::Tile::Suit suit = tile.second;
        
        if (rank <= TileSet::Tile::Rank::SEVEN) {
            auto nextTile1 = std::make_pair(static_cast<TileSet::Tile::Rank>(static_cast<int>(rank) + 1), suit);
            auto nextTile2 = std::make_pair(static_cast<TileSet::Tile::Rank>(static_cast<int>(rank) + 2), suit);
            
            if (tileCounts[nextTile1] > 0 && tileCounts[nextTile2] > 0) {
                tileCounts[tile]--;
                tileCounts[nextTile1]--;
                tileCounts[nextTile2]--;
                
                if (onePairFourSets(tileCounts, pair, sets + 1)) {
                    return true;
                }
                
                tileCounts[tile]++;
                tileCounts[nextTile1]++;
                tileCounts[nextTile2]++;
            }
        }
    }
    return false;
}

/**
 * Method to sort the tiles by Rank in ascending order.
 *
 * @param selectedTiles     a vector of selected tiles.
 * @return a vector of tiles sorted by Rank and Suit
 */
std::vector<std::shared_ptr<TileSet::Tile>> Hand::getSortedTiles(const std::vector<std::shared_ptr<TileSet::Tile>>& selectedTiles) {
    std::vector<std::shared_ptr<TileSet::Tile>> sortedTiles = selectedTiles; // creates a copy of the selectedTiles
    
    std::sort(sortedTiles.begin(), sortedTiles.end(),
        [](const std::shared_ptr<TileSet::Tile>& a, const std::shared_ptr<TileSet::Tile>& b) {
            // Added to prevent nullptr error
            if (a == nullptr || b == nullptr) {
                return false;
            }
            if (a->getSuit() == b->getSuit()) {
                return a->getRank() < b->getRank(); // Sort by rank if suit is the same
            }
            return a->getSuit() < b->getSuit(); // Otherwise, sort by suit
        }
    );
    return sortedTiles;
}


void Hand::updateTilePositions(cugl::Size sceneSize){
    cugl::Size screenSize = cugl::Application::get()->getDisplaySize();
    
    screenSize *= sceneSize.height/screenSize.height;
    
    
    float offsetWidth = (screenSize.width - sceneSize.width)/2.0f;
    float startX = offsetWidth + 75; // Starting x position for hand tile positioning
    float endX = screenSize.width - offsetWidth - 100; // Ending x position for hand tile positioning
    float tileSpacing = (endX-startX) / 14 ; // Spacing in x direction between tiles
    float yPos = 80.0f; // Height of hand tiles on the screen


    for (size_t i = 0; i < _tiles.size(); i++){
        if (_tiles[i] == _player->getDraggingTile()) {
          continue;
        }
      
    cugl::Vec2 newPos(startX + i * tileSpacing + (_tiles[i]->getTileTexture()->getWidth()/2 * _tiles[i]->_scale), yPos);
    _tiles[i]->pos = newPos;
      
      
  }
}

///**
// * Method to check if selected tiles contain a wild card (jack)
// *
// * @param selectedTiles     the tiles to be checked for a wild card
// */
//bool Hand::hasJack(std::vector<std::shared_ptr<TileSet::Tile>> selectedTiles){
//    for(std::shared_ptr<TileSet::Tile>& tile : selectedTiles){
//        
//        if(tile->getRank()==TileSet::Tile::Rank::WILD_RANK){
//            return true;
//        }
//    }
//    return false;
//}

/**
 * Function to draw the player's currently held tiles
 *
 * @param batch     the SpriteBatch to draw the tiles to screen
 */
void Player::draw(const std::shared_ptr<cugl::graphics::SpriteBatch>& batch) {
    for(const auto& tile : _hand._tiles){
        Vec2 pos = tile->pos;
        Vec2 origin = Vec2(tile->getTileTexture()->getSize().width/2, tile->getTileTexture()->getSize().height/2);
        
        if(tile->selected){
            pos.y += 10;
        }
        Affine2 trans;
        trans.scale(tile->_scale);
        trans.translate(pos);
        
        Size textureSize(350.0, 415.0);
        Vec2 rectOrigin(tile->pos - (textureSize * tile->_scale)/2);
        tile->tileRect = cugl::Rect(rectOrigin, textureSize * tile->_scale);

        batch->draw(tile->getTileTexture(), origin, trans);
    }
}

std::shared_ptr<TileSet::Tile> Hand::getTileAtPosition(const cugl::Vec2& mousePos) {
    for (const auto& tile : _tiles) {
        if (tile) {
            if (tile->tileRect.contains(mousePos)) {
                return tile;
            }
        }
    }
    return nullptr;
}
