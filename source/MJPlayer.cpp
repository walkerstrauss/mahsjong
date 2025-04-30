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

#define VELOCITY_THRESHOLD 2.0f
#define ROTATE_MAX 0.3f

#define SPRING 0.05f
#define DAMP 0.05f

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
        drawnTile->_scale = 0.325;
        drawnTile->inDeck = false;
        _tiles.push_back(drawnTile);
    }
    
    _tiles = getSortedTiles(_tiles);
    
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
        tile->_scale = 0.325;
        _tiles.push_back(tile); // Add drawn tiles to hand
    }
 }

/**
 * Discards a single specified tile from our hand
 *
 * @param tile      the tile to discard from out hand
 */
bool Hand::discard(std::shared_ptr<TileSet::Tile> tile, bool isHost){
    if (removeTile(tile, isHost)) {
        tile->discarded = true;
        return true;
    }
    return false;
}

/**
 *  Finds and removes the given tile from hand.
 */
bool Hand::removeTile(std::shared_ptr<TileSet::Tile> tile, bool isHost) {
    auto it = _tiles.begin();
    while(it != _tiles.end()){
        if ((*it)->_id == tile->_id) {
            if (isHost) {
                (*it)->inHostHand = false;
            } else {
                (*it)->inClientHand = false;
            }
            (*it)->inPile = false;
            (*it)->selected = false;
            
            _tiles.erase(it);
            
            return true;
        } else {
            it++;
        }
    }
    return false;
}

/**
 * Method to play a set from your hand (of 2 to 4 cards)
 *
 * @return true if a set was played sucessfully and false otherwise.
 */

bool Hand::playSet(bool isHost) {
    if (_selectedTiles.empty()) {
        return false;
    }
    
    // Prepare a container for the played set
    std::vector<std::shared_ptr<TileSet::Tile>> playedSet;
    
    // Iterate over the hand and remove any tile that is in _selectedTiles
    auto it = _tiles.begin();
    while (it != _tiles.end()) {
        // Check if the current tile is in the selected tiles list
        if (std::find(_selectedTiles.begin(), _selectedTiles.end(), *it) != _selectedTiles.end()) {
            (*it)->played = true;
            if (isHost) {
                (*it)->inHostHand = false;
            } else {
                (*it)->inClientHand = false;
            }
            // Since the tile is played (and not being discarded), update its status.
//            (*it)->discarded = false;
            
            // Add tile to the played set
            playedSet.push_back(*it);
            // Remove the tile from the hand
            it = _tiles.erase(it);
        } else {
            ++it;
        }
    }
    
    _playedSets.push_back(playedSet);
    
    // Unselect all tiles now that they've been played.
    for (auto& tile : _selectedTiles) {
        tile->selected = false;
        tile->selectedInSet = false;
        
        tile->_scale = 0;
        tile->pos = Vec2::ZERO;
    }
    _selectedTiles.clear();
    
    _size-=3;
    
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
    if(selectedTiles.size() <= 2 || selectedTiles.size() >= 4){
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

///**
// * Confirms if a set if of a kind.
// * Cann't take in a celestial tile.
// * @param selectedTiles
// */
bool Hand::isOfaKind(const std::vector<std::shared_ptr<TileSet::Tile>>& selectedTiles){
    std::shared_ptr<TileSet::Tile> tileA = selectedTiles[0];

    // iterate over all tiles besides the first tile.
    for(int j=1; j<selectedTiles.size(); ++j){

        std::shared_ptr<TileSet::Tile> tileB = selectedTiles[j];

        // Not the same SUIT.
        if(tileA->getSuit() != tileB->getSuit()){
            return false;
        }
        // Not the same RANK.
        if(tileA->getRank() != tileB->getRank()){
            return false;
        }
    }
    
    return true;
}


/**
 * Confirms if a set isStraight.
 *
 * @param selectedTiles     the tiles to be checked for a straight
 */
bool Hand::isStraight(const std::vector<std::shared_ptr<TileSet::Tile>>& selectedTiles){

    std::vector<std::shared_ptr<TileSet::Tile>> sortedTiles = getSortedTiles(selectedTiles);

    // the base tile.
    std::shared_ptr<TileSet::Tile> tileA = sortedTiles[0];

    // check if the sorted selectedTiles are of the same SUIT.
    for(int i = 1; i<sortedTiles.size(); ++i){

        std::shared_ptr<TileSet::Tile> tileB = sortedTiles[i];

        // Not the same SUIT.
        if(tileA->getSuit() != tileB->getSuit()){
            return false;
        }
    }

    // check if the selectedTiles are consequitive.
        for(int i = 0; i<sortedTiles.size() - 1; ++i){
            std::shared_ptr<TileSet::Tile> tileA = sortedTiles[i];
            std::shared_ptr<TileSet::Tile> tileC = sortedTiles[i+1];

            int gap = static_cast<int>(tileC->getRank()) - static_cast<int>(tileA->getRank());
            if (gap != 1) {
                return false;
            }
        }

    return true;
}

bool Hand::isWinningHand() {
    if (_tiles.size() == _size + 1) {
        std::vector<std::shared_ptr<TileSet::Tile>> sortedHand = getSortedTiles(_tiles);
        std::map<std::pair<TileSet::Tile::Rank, TileSet::Tile::Suit>, int> tileCounts;
        for (const auto& tile : sortedHand) {
            if (tile->getSuit() == TileSet::Tile::Suit::CELESTIAL || tile->debuffed) {
                return false;
            }
            tileCounts[{tile->getRank(), tile->getSuit()}]++;
        }
        
    return onePairFourSets(tileCounts, 0, (int)_playedSets.size());
    }
    return false;
}

bool Hand::onePairFourSets(std::map<std::pair<TileSet::Tile::Rank, TileSet::Tile::Suit>, int>& tileCounts, int pair, int sets) {
    if (pair == 1 && sets == 4) {
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
        
        if (rank <= TileSet::Tile::Rank::SEVEN && suit != TileSet::Tile::Suit::CELESTIAL) {
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


void Hand::updateTilePositions(cugl::Rect rect, float dt){
    float startX = rect.getMinX(); // Starting x position for hand tile positioning
    float endX = rect.getMaxX(); // Ending x position for hand tile positioning
    float tileSpacing = (endX-startX) / getTileCount() ; // Spacing in x direction between tiles
    float yPos = (rect.getMaxY() + rect.getMinY()) / 2.0f; // Height of hand tiles on the screen


    for (size_t i = 0; i < _tiles.size(); i++){
        if (_tiles[i] == _player->getDraggingTile()) {
            continue;
        }
        
        cugl::Vec2 newPos(startX + i * tileSpacing + (_tiles[i]->getBackTextureNode()->getTexture()->getWidth()/2 * _tiles[i]->_scale), yPos);
        
        if(_tiles[i]->selected) {
            newPos.y += 10.0f;
        }
        
        _tiles[i]->pos = newPos;
    }
    
    for(const auto& tile : _tiles){
        Vec2 pos = tile->pos;
        Vec2 origin = Vec2(tile->getTileTexture()->getSize().width/2, tile->getTileTexture()->getSize().height/2);
        
        Size textureSize(tile->getBackTextureNode()->getTexture()->getSize());
        Vec2 rectOrigin(pos - (textureSize * tile->_scale)/2);
        tile->tileRect = cugl::Rect(rectOrigin, textureSize * tile->_scale);
       
        float velocity = tile->getContainer()->getPosition().x - tile->pos.x;
        float displacement = tile->getContainer()->getAngle();
        float force = -SPRING * displacement - DAMP * velocity;
        
        Vec2 lerpPos = tile->getContainer()->getPosition();
        lerpPos.lerp(pos, 0.5);
        
        velocity += force * dt;
        displacement = std::clamp(velocity * dt, -ROTATE_MAX, ROTATE_MAX);
        
        tile->getContainer()->setAnchor(Vec2::ANCHOR_CENTER);
        tile->getContainer()->setAngle(displacement);
        tile->getContainer()->setScale(tile->_scale);
        tile->getContainer()->setPosition(lerpPos);
        tile->getContainer()->setVisible(tile != _player->_draggingTile);
    }
}

/**
 * Function to draw the player's currently held tiles
 *
 * @param batch     the SpriteBatch to draw the tiles to screen
 */
void Player::draw(const std::shared_ptr<cugl::graphics::SpriteBatch>& batch) {
    for(const auto& tile : _hand._tiles){
        tile->getContainer()->render(batch, Affine2::IDENTITY, Color4::WHITE);
    }
}

void Player::drawInfo(const std::shared_ptr<TileSet::Tile> tile, const std::shared_ptr<cugl::graphics::SpriteBatch>& batch, Size screenSize) {
    if(tile->debuffed) {
        return;
    }
    std::shared_ptr<Texture> infoTexture = tile->getInfoTexture();
    Vec2 origin(infoTexture->getSize().width/2.0f, infoTexture->getSize().height/2.0f);
    
    Size textureSize = infoTexture->getSize() * 0.9f;
    float textureWidth = textureSize.getIWidth() * 0.5f;
    float textureHeight = textureSize.getIHeight() * 0.5f;
    
    float tileHeight = tile->getBackTextureNode()->getSize().height * tile->_scale;
    float tileWidth = tile->getBackTextureNode()->getSize().width * tile->_scale;
    
    float totalHeight = tile->getContainer()->getPosition().y + tileHeight * 0.5f + textureHeight + 2.2f;
    float totalMinWidth = tile->getContainer()->getPosition().x - textureWidth * 0.5f + 2.0f;
    float totalMaxWidth = tile->getContainer()->getPosition().x + textureWidth * 0.5f + 2.0f;

    float x;
    float y;
    
    if(totalHeight > screenSize.getIHeight()) {
        y = tile->getContainer()->getPosition().y;
        if(totalMinWidth < 0) {
            x = tile->getContainer()->getPosition().x + tileWidth * 0.5f + textureWidth + 2.2f;
        }
        else {
            x = tile->getContainer()->getPosition().x - tileWidth * 0.5f - textureWidth - 2.2f;
        }
    }
    else{
        if(totalMinWidth < 0) {
            x = tile->getContainer()->getPosition().x + tileWidth * 0.5f + textureWidth + 2.2f;
            y = tile->getContainer()->getPosition().y;
        }
        else if (totalMaxWidth > screenSize.width){
            x = tile->getContainer()->getPosition().x - tileWidth * 0.5f - textureWidth - 2.2f;
            y = tile->getContainer()->getPosition().y;
        }
        else {
            x = tile->getContainer()->getPosition().x;
            y = tile->getContainer()->getPosition().y + tileHeight * 0.5f + textureHeight + 2.2f;
        }
    }
    
    Affine2 trans;
    Vec2 pos(x, y);
    trans.scale(0.9f);
    trans.translate(pos);
    
    batch->draw(infoTexture, origin, trans);
}
