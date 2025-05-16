
//
//  MJPile.cpp
//  Mahsjong
//
//  Created by Patrick Choo on 2/18/25.
//

#include "MJPile.h"
#include "MJPlayer.h"
#include "MJAnimationController.h"

#define VELOCITY_THRESHOLD 2.0f
#define SHUFFLE_VELOCITY 800.f
#define ROTATE_MAX 0.3f

#define SPRING 0.05f
#define DAMP 0.05f

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
bool Pile::initPile(int size, std::shared_ptr<TileSet> tileSet, bool isHost, std::shared_ptr<AssetManager>& assets) {
    _assets = assets;
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
        
    // Iterate through the pile
    for (int i = 0; i < _pileSize; i++) {
        std::vector<std::shared_ptr<TileSet::Tile>> row;
        for (int j = 0; j < _pileSize; j++) {
            if (_tileSet->deck.size() <= 0) { //If our deck is empty, set the rest of the _pile to be empty (deck.empty() instead?)
                row.push_back(nullptr);
                continue;
            }
            
            std::shared_ptr<TileSet::Tile> tile = _tileSet->deck.back();
            
            tile->_scale = 0.275;
            tile->inPile = true;
            tile->pileCoord = cugl::Vec2(i, j);
            tile->inDeck = false; 

            row.push_back(tile);
            _tileSet->deck.pop_back();
        }
        _pile.push_back(row); //add tile from deck to pile
    }
    
    setTilePositions(false);
    
    return true;
}

void Pile::setTilePositions(bool shuffling) {
    float spacingY = 1.0f;
    float spacingX = 1.0f;
    
    float height = pileBox.getMaxY() - pileBox.getMinY();
    float width = pileBox.getMaxX() - pileBox.getMinX();
    
    std::unordered_map<std::shared_ptr<TileSet::Tile>, Vec2> tilePos;
    
    _pileMap.clear();
    
    for (int i = 0; i < _pileSize; i++) {
        for (int j = 0; j < _pileSize; j++) {
            std::shared_ptr<TileSet::Tile> tile = _pile[i][j];
            if (tile == nullptr) continue;
            
            cugl::Size _size = tile->getBackTextureNode()->getSize();
            cugl::Vec2 tileSize(_size.width * tile->_scale, _size.height * tile->_scale);
            
            float pileWidth = (_pileSize - 1) * (tileSize.x * spacingX);
            float pileHeight = (_pileSize - 1)* (tileSize.y * spacingY);
            Vec2 pileOffset((pileBox.origin.x + (width - pileWidth) * 0.5f), (pileBox.origin.y + (height - pileHeight) * 0.5f));

            float x = j * tileSize.x * spacingX;
            float y = i * tileSize.y * spacingY;
            
            if(shuffling) tilePos.insert({tile, Vec2(x, y) + pileOffset});
            else tile->pos = cugl::Vec2(x, y) + pileOffset;

            std::string key = std::to_string(tile->_id);
            _pileMap.insert({key, tile->pileCoord});
        }
    }
    
    if(shuffling) {
        pileFlipMoveAway(tilePos);
    }
}

void Pile::updateTilePositions(float dt) {
    for (int i = 0; i < _pileSize; i++) {
        for (int j = 0; j < _pileSize; j++) {
            std::shared_ptr<TileSet::Tile> tile = _pile[i][j];
            if(tile == nullptr){
                continue;
            }
            Vec2 pos = tile->pos;
            Vec2 origin = Vec2(tile->getTileTexture()->getSize().width/2, tile->getTileTexture()->getSize().height/2);
            
            Size textureSize(tile->getBackTextureNode()->getTexture()->getSize());
            Vec2 rectOrigin(pos - (textureSize * tile->_scale)/2);
            tile->tileRect = cugl::Rect(rectOrigin, textureSize * tile->_scale);
            
            float velocity = tile->getContainer()->getPosition().x - tile->pos.x;
            float displacement = tile->getContainer()->getAngle();
            float force = -SPRING * displacement - DAMP * velocity;
            
            velocity += force * dt;
            displacement = std::clamp(velocity * dt, -ROTATE_MAX, ROTATE_MAX);
            
            Vec2 movePos;
            if(tile->moveToCenter) {
                Vec2 delta = tile->pos - tile->getContainer()->getPosition();
                float dist = delta.length();
                if(dist < 1e-3f) {
                    movePos = tile->pos;
                    tile->moveToCenter = false;
                }
                else {
                    Vec2 dir = delta/ dist;
                    float step = SHUFFLE_VELOCITY * dt;
                    movePos = tile->getContainer()->getPosition() + dir * std::min(step, dist);
                }
            }
            else {
                Vec2 lerpPos = tile->getContainer()->getPosition();
                lerpPos.lerp(pos, 0.5);
                movePos = lerpPos;
            }
            
            tile->getContainer()->setAnchor(Vec2::ANCHOR_CENTER);
            tile->getContainer()->setAngle(displacement);
            if(!tile->animating) {
                tile->getContainer()->setScale(tile->_scale);
            }
            tile->getContainer()->setPosition(movePos);
            tile->getContainer()->setVisible(true);
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
        const std::string id = tileKey->getString("id");
        
        int x = _pileMap[id].x;
        int y = _pileMap[id].y;
        
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
        _pileMap.erase(id);
    }
}

void Pile::removeTile(std::shared_ptr<TileSet::Tile> tile) {
    std::vector<std::shared_ptr<TileSet::Tile>> tiles;
    for (int i = _pileSize - 1; i >= 0; i--) {
        for (int j = _pileSize - 1; j >= 0; j--) {
            if (_pile[i][j] && _pile[i][j] != tile) {
                tiles.push_back(_pile[i][j]);
            }
        }
    }
    
    tile->inPile = false;
    
    clearPile();
    
    int index = 0;
    for (int i = _pileSize - 1; i >= 0; i--) {
        for (int j = _pileSize - 1; j >= 0; j--) {
            if (index < tiles.size()) {
                _pile[i][j] = tiles[index];
                _pile[i][j]->pileCoord = cugl::Vec2(i, j);
                index++;
            }
        }
    }
}

void Pile::remakePile(bool shuffling){
    if(shuffling) {
        pileFlipMoveCenter();
    }
    
    _pileMap.clear();
    for(auto const& pairs : _tileSet->tileMap) {
        std::shared_ptr<TileSet::Tile> currTile = pairs.second;
        if(currTile->inPile) {
            std::string key = std::to_string(currTile->_id);
            int x = currTile->pileCoord.x;
            int y = currTile->pileCoord.y;
            _pile[x][y] = currTile;
            _pileMap.insert({key, currTile->pileCoord});
        }
    }
    
    setTilePositions(shuffling);
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
            tile->getContainer()->render(batch, Affine2::IDENTITY, Color4::WHITE);
        }
    }
}

void Pile::reshufflePile(){
    pileFlipMoveCenter();
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
                _pile[i][j]->animating = true; 
                index++;
            }
        }
    }
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


void Pile::updateRow(int row, const std::vector<std::shared_ptr<TileSet::Tile>>& tiles, float dt) {
    for (int j = 0; j < _pileSize; j++) {
        if (_pile[row][j] != nullptr) {
            _pile[row][j] = tiles[j];
            _pile[row][j]->pileCoord = cugl::Vec2(row, j);
        }
    }
    setTilePositions(false);
}

int Pile::selectedRow(std::shared_ptr<TileSet::Tile> tile) {
    for (int i = 0; i < _pileSize; i++) {
        for (int j = 0; j < _pileSize; j++) {
            if (_pile[i][j] == tile) {
                return i;
                break;
            }
        }
    }
    return -1;
}

void Pile::pileJump(float dt) {
    time += dt;
    if (time < 5.0f) return;
    time = 0.0f;
    
    int stagger = 0;
    
    Application* app = Application::get();
    for (int i = _pileSize - 1; i >= 0; --i) {
      for (int j = 0; j < _pileSize; ++j) {
          std::shared_ptr<TileSet::Tile> tile = _pile[i][j];
            if(!tile) {
                continue;
            }
            app->schedule([tile](){
              tile->pos.y += 3.0f;
              return false;
            }, stagger + 0.05);
            
            app->schedule([tile](){
              tile->pos.y -= 3.0f;
              return false;
            }, stagger += 100);
        }
    }
}

/** Pile flip effect */
void Pile::pileFlipMoveCenter() {
    choice = SHUFFLE;
    int counter = 0;
    float flipDelay = 50.0f;
    
    Vec2 pileCenter = pileBox.origin + Vec2(pileBox.size.width / 2.0f, pileBox.size.height / 2.0f);
    
    Application* app = Application::get();
    for(int i = _pileSize - 1; i >= 0; --i) {
        for(int j = 0; j < _pileSize; ++j) {
            std::shared_ptr<TileSet::Tile> tile = _pile[i][j];
            if(!tile) {
                continue;
            }
            
            counter += 1;
            std::shared_ptr<Texture> frontTexture = tile->getSuit() == TileSet::Tile::Suit::CELESTIAL ?
            _assets->get<Texture>("blank celestial hand") : _assets->get<Texture>("blank normal hand");
            std::shared_ptr<Texture> backTexture = tile->getSuit() == TileSet::Tile::Suit::CELESTIAL ?
            _assets->get<Texture>("back celestial") : _assets->get<Texture>("back normal");
            
            app->schedule([tile, backTexture, frontTexture, pileCenter] () mutable {
                AnimationController::getInstance().animateTileFlip(tile, frontTexture, backTexture, tile->_scale, 12.0f, false);
                return false;
            }, flipDelay * counter);
        }
    }
    
    counter = 0;
    
    for(int i = _pileSize - 1; i >= 0; --i) {
        for(int j = 0; j < _pileSize; ++j) {
            std::shared_ptr<TileSet::Tile> tile = _pile[i][j];
            if(!tile) {
                continue;
            }
            
            counter += 1;
            app->schedule([tile, pileCenter] () mutable {
                tile->pos = pileCenter;
                tile->moveToCenter = true; 
                return false;
            }, flipDelay * counter + 100.0f);
        }
    }
}

void Pile::pileFlipMoveAway(std::unordered_map<std::shared_ptr<TileSet::Tile>, Vec2> tilePos) {
    float flipDelay = 50.0f;
    float flipMoveCenterEnd = flipDelay * getVisibleSize() + 1000.0f;
    float moveAway = 50.0f;
    float moveAwayEnd = flipMoveCenterEnd + moveAway * getVisibleSize();
    
    int counter = 0;
        
    Application* app = Application::get();
    for(int i = _pileSize - 1; i >= 0; --i) {
        for(int j = 0; j < _pileSize; ++j) {
            std::shared_ptr<TileSet::Tile> tile = _pile[i][j];
            if(!tile) {
                continue;
            }
            
            counter += 1;
            
            app->schedule([tile, tilePos] () mutable {
                tile->moveToCenter = true;
                tile->pos = tilePos[tile];
                return false;
            }, flipMoveCenterEnd + counter * moveAway);
        }
    }
    
    counter = 0;
    
    for(int i = _pileSize - 1; i >= 0; --i) {
        for(int j = 0; j < _pileSize; ++j) {
            std::shared_ptr<TileSet::Tile> tile = _pile[i][j];
            if(!tile) {
                continue;
            }
            
            counter += 1;
            std::shared_ptr<Texture> frontTexture = tile->getSuit() == TileSet::Tile::Suit::CELESTIAL ?
            _assets->get<Texture>("blank celestial hand") : _assets->get<Texture>("blank normal hand");
            std::shared_ptr<Texture> backTexture = tile->getSuit() == TileSet::Tile::Suit::CELESTIAL ?
            _assets->get<Texture>("back celestial") : _assets->get<Texture>("back normal");
            
            app->schedule([tile, backTexture, frontTexture] () mutable {
                AnimationController::getInstance().animateTileFlip(tile, backTexture, frontTexture, tile->_scale, 12.0f, true);
                return false;
            }, flipMoveCenterEnd + counter * flipDelay);
        }
    }
    choice = NONE; 
}
