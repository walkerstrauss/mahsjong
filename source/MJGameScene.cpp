//
//  MJGameScene.cpp
//  Mahsjong
//
//  Created by Patrick Choo on 2/18/25.
//

#include <cugl/cugl.h>
#include <iostream>
#include <sstream>
#include "MJGameScene.h"

using namespace cugl;
using namespace cugl::graphics;
using namespace cugl::audio;
using namespace std;

#pragma mark -
#pragma mark Level Layout

// Lock the screen esize to a fixed heigh regardless of aspect ratio
// PLEASE ADJUST AS SEEN FIT
#define SCENE_HEIGHT 720

#pragma mark -
#pragma mark Constructors
/**
 * Initializes the controller contents, and starts the game
 *
 * The constructor does not allocate any objects or memory. This allows
 * us to have a non-pointer reference to this controller, reducing our memory
 * allocation. Instead, allocation happens in this method
 *
 * @param assets    the asset manager for the game
 */
bool GameScene::init(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<NetworkController> network, MatchController& matchController) {
    // Initialize the scene to a locked height
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::initWithHint(Size(0,SCENE_HEIGHT))) {
        std::cerr << "Scene2 initialization failed!" << std::endl;
        return false;
    }

    _assets = assets;
    _network = network;
    _matchController = matchController;
    _choice = Choice::NONE;
    
    _matchScene = _assets->get<scene2::SceneNode>("matchscene");
    _matchScene->setContentSize(1280,720);
    cugl::Size screenSize = cugl::Application::get()->getDisplaySize();
    
    screenSize *= _matchScene->getContentSize().height/screenSize.height;
    
    float offset = (screenSize.width -_matchScene->getWidth())/2;    
    _matchScene->setPosition(offset, _matchScene->getPosition().y);

    
    if (!Scene2::initWithHint(screenSize)) {
        std::cerr << "Scene2 initialization failed!" << std::endl;
        return false;
    }
   
    _tilesetUIBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("matchscene.gameplayscene.discardButton"));
    _pauseBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("matchscene.gameplayscene.pauseButton"));
    _endTurnBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("matchscene.gameplayscene.endTurnButton"));
    _endTurnBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            _matchController.endTurn();
        }
    });
    
    _tilesetUIBtnKey = _tilesetUIBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            _choice = Choice::TILESET;
        }
    });
    
    _pauseBtnKey = _pauseBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            _choice = Choice::PAUSE;
        }
    });
    
    addChild(_matchScene);
    // Game Win and Lose bool
    _gameWin = false;
    _gameLose = false;
    
    discardArea = cugl::Rect(Vec2(1000, 210), Size(273, 195));

    // Host and Client specific initializations
    if(_network->getHostStatus()){
        _matchController.initHost();
        _player = _matchController.hostPlayer;
    } else {
        _matchController.initClient();
        _player = _matchController.clientPlayer;
    }
    
    //Initialization of shared objects
    _tileSet = _matchController.getTileSet();
    _pile = _matchController.getPile();
    _discardPile = _matchController.getDiscardPile();
    
    _input.init(); //Initialize the input controller
    
    // TODO: initialize audio controller and init with asset manager
    // TODO: initialize animations for game scene 
    
    _quit = false;
    setActive(false);
    _matchScene->setVisible(true);
    
    
    // init the pile for the pile rect
    float minX = std::numeric_limits<float>::max();
    float minY = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float maxY = std::numeric_limits<float>::lowest();

    for (const auto& row : _pile->_pile) {
        for (const auto& tile : row) {
            if (tile == nullptr) continue;
            
            cugl::Size textureSize(350.0f, 415.0f);
            cugl::Vec2 rectOrigin = tile->pos - (textureSize * tile->_scale) / 2;
            cugl::Vec2 rectEnd = rectOrigin + (textureSize * tile->_scale);

            minX = std::min(minX, rectOrigin.x);
            minY = std::min(minY, rectOrigin.y);
            maxX = std::max(maxX, rectEnd.x);
            maxY = std::max(maxY, rectEnd.y);
            }
    }

    _pileBox = cugl::Rect(minX, minY, maxX - minX, maxY - minY);
    
    std::shared_ptr<SceneNode> actionTileSection = _assets->get<scene2::SceneNode>("matchscene.gameplayscene.actionSection.up.actionTileSection");
    cugl::Vec2 worldOrigin = actionTileSection->nodeToWorldCoords(Vec2::ZERO);
    _celestialBox = cugl::Rect(worldOrigin, actionTileSection->getContentSize());
    
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void GameScene::dispose() {
    if (_active) {
        removeAllChildren();
        _active = false;
    }
}

#pragma mark -
#pragma mark Gameplay Handling
/**
 * Resets the status of the game so that we can play again
 */
void GameScene::reset() {
    _gameLose = false;
    _gameWin = false;
    dispose();
    init(_assets, _network, _matchController);
    return;
}

/**
 * The method to update the game mode
 *
 * @param timestep The amount of time (in seconds) since the last frame
 */
void GameScene::update(float timestep) {
    //Reading input
    _input.readInput();
    _input.update();
    
    // Fetching current mouse position
    cugl::Vec2 mousePos = cugl::Scene::screenToWorldCoords(cugl::Vec3(_input.getPosition()));
    
    // Constantly updating the position of tiles in hand
    _player->getHand().updateTilePositions(_matchScene->getSize());
    
    // Clicking/Tapping and Dragging logic
    if(_input.didRelease() && !_input.isDown()) {
        cugl::Vec2 initialMousePos = cugl::Scene::screenToWorldCoords(cugl::Vec3(_input.getInitialPosition()));
        if(initialMousePos - mousePos == Vec2(0, 0)) {
            clickedTile(mousePos);
        }
    }
    updateDrag(mousePos, _input.isDown(), _input.didRelease());
    
    // If it is your turn, allow turn-based actions
    if(_network->getCurrentTurn() == _network->getLocalPid()) {
        // Coords of initial click and ending release
        cugl::Vec2 initialMousePos = cugl::Scene::screenToWorldCoords(cugl::Vec3(_input.getInitialPosition()));
        bool releasedInPile = _input.didRelease() && _pileBox.contains(mousePos);
        // Drawing (from pile) logic
        if(_pileBox.contains(initialMousePos) && releasedInPile) {
            _matchController.drawTile();
        }
    }
}


/**
 * Draws all this to the scene's SpriteBatch.
 */
void GameScene::render() {
    /**
    * This is the temporary view of our camera. Not supposed to do this.
    * TODO: Please edit camera view appropriately
    */
    _batch->begin(getCamera()->getCombined());
    const std::shared_ptr<Texture> temp = Texture::getBlank();
    
    _batch->draw(temp, Color4(0,0,0,255), Rect(Vec2::ZERO, cugl::Application().get()->getDisplaySize()));

    _matchScene->render(_batch);
    _pile->draw(_batch);
    _discardPile->draw(_batch);
    _player->draw(_batch);
    
    _batch->setColor(Color4(255, 0, 0, 200));
    _batch->setTexture(nullptr);
    
    _batch->fill(discardArea);
    
    _batch->end();
}

void GameScene::setActive(bool value){
    CULog("set active");
    if (isActive() != value){
        Scene2::setActive(value);
    }
}
    
void GameScene::setGameActive(bool value){
    if (value){
        _choice = NONE;
        _pauseBtn->activate();
        _tilesetUIBtn->activate();
        _endTurnBtn->activate();
    } else {
        _pauseBtn->deactivate();
        _tilesetUIBtn->deactivate();
        _endTurnBtn->deactivate();
    }
}

void GameScene::applyCelestial(TileSet::Tile::Rank type) {
    if (type == TileSet::Tile::Rank::CHAOS) {
        _pile->reshufflePile();
        _network->broadcastDeckMap(_tileSet->mapToJson());
        _network->broadcastPileLayer();
    }
    
}
//void GameScene::applyAction(std::shared_ptr<TileSet::ActionTile> actionTile) {
//    _player->getHand().discard(actionTile, _network->getHostStatus());
//    switch (actionTile->type) {
//        case TileSet::ActionTile::ActionType::CHAOS:
//            CULog("CHAOS: Reshuffling the pile...");
//            _pile->reshufflePile();
//            _network->broadcastDeckMap(_tileSet->mapToJson());
//            _network->broadcastPileLayer();
//            break;
//        case TileSet::ActionTile::ActionType::ECHO:
//            CULog("ECHO: Draw two tiles...");
//            _player->getHand().drawFromPile(_pile, 2, _network->getHostStatus());
//            _network->broadcastTileDrawn(_tileSet->toJson(_tileSet->tilesToJson));
//            _tileSet->clearTilesToJson();
//            if (_pile->getVisibleSize() == 0) {
//                _pile->createPile();
//                _network->broadcastDeckMap(_tileSet->mapToJson());
//                _network->broadcastPileLayer();
//            }
//            else{
//                _network->broadcastDeck(_tileSet->toJson(_tileSet->deck));
//            }
//            break;
//        case TileSet::ActionTile::ActionType::ORACLE:
//            CULog("ORACLE: Draw any tile from pile...");
//            
//        default:
//            break;
//    }
//    
//}

void GameScene::clickedTile(cugl::Vec2 mousePos){
    cugl::Vec2 initialMousePos = cugl::Scene::screenToWorldCoords(cugl::Vec3(_input.getInitialPosition()));
    
    for(const auto& pair : _tileSet->tileMap){
        std::shared_ptr<TileSet::Tile> currTile = pair.second;
        if(currTile->tileRect.contains(mousePos) && currTile->tileRect.contains(initialMousePos)){
            if((_network->getHostStatus() && currTile->inHostHand) || (!_network->getHostStatus() && currTile->inClientHand)) {
                if(currTile->selected) {
                    auto it = std::find(_player->getHand()._selectedTiles.begin(), _player->getHand()._selectedTiles.end(), currTile);
                    if (it != _player->getHand()._selectedTiles.end()) {
                        _player->getHand()._selectedTiles.erase(it);
                    }
                }
                else{
                    _player->getHand()._selectedTiles.push_back(currTile);
                }
            }
            if(currTile->inPile) {
                continue;
//                if(currTile->selected) {
//                    currTile->_scale = 0.2;
//                }
//                else{
//                    currTile->_scale = 0.25;
//                }
            }
            if(currTile->selected) {
                currTile->selected = false;
            }
            else {
                currTile->selected = true;
            }
        }
    }
}

/**
 * Method to get the index of this tile's associated label in the discard UI vector of labels
 *
 * @param tile  the tile whose label we need in _labels
 * @return an int representing the index of this tile's discard UI label
 */
int GameScene::getLabelIndex(std::shared_ptr<TileSet::Tile> tile){
    int rowIndex = 0;
    switch (tile->getSuit()){
        case TileSet::Tile::Suit::BAMBOO:
            break;
        case TileSet::Tile::Suit::CRAK:
            rowIndex = 9;
            break;
        case TileSet::Tile::Suit::DOT:
            rowIndex = 18;
            break;
        default:
            break;
    }
    return rowIndex + (int)tile->getRank() - 1;
}


/**
 * Method to update discard UI label corresponding to tile passed as argument
 *
 * @param tile  the tile to increment in the discard UI
 * @return true if update was successful, and false otherwise
 */
bool GameScene::incrementLabel(std::shared_ptr<TileSet::Tile> tile){
    // Get index of label in _labels
    int i = getLabelIndex(tile);
    
    // Check if we already discarded 4 (or more) of this tile
    if (std::stoi(_labels[i]->getText()) > 3){
        CULog("already discarded all copies of this tile");
        return false;
    }
    
    // Increment discard UI number and update label text
    std::string text = std::to_string(std::stoi(_labels[i]->getText()) + 1);
    _labels[i]->setText(text);
    return true;
}

bool GameScene::decrementLabel(std::shared_ptr<TileSet::Tile> tile){
    // Get index of label in _labels
    int i = getLabelIndex(tile);
    
    // Check if we already discarded 4 (or more) of this tile
    if (std::stoi(_labels[i]->getText()) < 1){
        CULog("none of this tile discarded - cannot decrement");
        return false;
    }
    
    // Increment discard UI number and update label text
    std::string text = std::to_string(std::stoi(_labels[i]->getText()) - 1);
    _labels[i]->setText(text);
    return true;
}

//void GameScene::pressTile(){
//    cugl::Vec2 screenPos = _input.getPosition();
//    cugl::Vec2 mousePos = cugl::Scene::screenToWorldCoords(cugl::Vec3(screenPos));
//    
//    // if the player tapped on a tile in the hand.
//    for (auto & tile : _player->getHand()._tiles) {
//        if (tile->tileRect.contains(mousePos)) {
//            // select this tile
//            CULog("selected a tile");
//            tile->selected = !tile->selected;
//            _dragOffset = _draggingTile->pos - mousePos;
//            _draggingTile->pressed = true;
//            _draggingTile = tile;
//            
//            auto& selected = _player->getHand()._selectedTiles;
//            auto it = std::find(selected.begin(), selected.end(), tile);
//            if (tile->selected) {
//                if (it == selected.end()) selected.push_back(tile);
//            } else {
//                if (it != selected.end()) selected.erase(it);
//            }
//            
//        }
//    }
//    
//    // if the player pressed on the pile
//    if (_pileBox.contains(mousePos)) {
//        
//        if(_player->getHand()._tiles.size() > _player->getHand()._size){
//            CULog("Hand too big");
//        }
//        _player->getHand().drawFromPile(_pile, 1, _network->getHostStatus());
//        _network->broadcastTileDrawn(_tileSet->toJson(_tileSet->tilesToJson));
//        _tileSet->clearTilesToJson();
//        _network->broadcastDeck(_tileSet->toJson(_tileSet->deck));
//        if (_player->getHand().isWinningHand()){
//            _gameWin = true;
//        }
//        _player->canDraw = false;
//    }
//    
//    // does it work for the pile? -- no?
//    for (auto& row : _pile->_pile) {
//        for (auto& tile : row) {
//            if (tile && tile->tileRect.contains(mousePos)) {
//                CULog("Selected a tile in pile");
//                
//                tile->selected = !tile->selected;
//                _draggingTile = tile;
//                _dragOffset = tile->pos - mousePos;
//                tile->pressed = true;
//                
//                auto& selected = _player->getHand()._selectedTiles;
//                auto it = std::find(selected.begin(), selected.end(), tile);
//                if (tile->selected) {
//                    if (it == selected.end()) selected.push_back(tile);
//                } else {
//                    if (it != selected.end()) selected.erase(it);
//                }
//                
//            
//            }
//        }
//    }
//}

void GameScene::dragTile(){
    
    if (!_draggingTile) return;
    
    cugl::Vec2 screenPos = _input.getPosition();
    cugl::Vec2 mousePos = cugl::Scene::screenToWorldCoords(cugl::Vec3(screenPos));
    
    cugl::Vec2 newPos = mousePos + _dragOffset;
    
    _draggingTile->pos = newPos;
    _draggingTile->tileRect.origin = newPos;
    

}

void GameScene::releaseTile() {
    if (_draggingTile) {
        _draggingTile->pressed = false;
        _draggingTile = nullptr;
    }
}
        
void GameScene::updateDrag(const cugl::Vec2& mousePos, bool mouseDown, bool mouseReleased) {
    if (mouseDown) {
        if (!_dragInitiated) {
            _dragStartPos = mousePos;
            _dragInitiated = true;
            _draggingTile = _player->getHand().getTileAtPosition(mousePos);
            _player->_draggingTile = _draggingTile;
            if (_draggingTile) {
                _originalTilePos = _draggingTile->pos;
                _dragOffset = _draggingTile->pos - mousePos;
            }
        }
        else {
            float distance = (mousePos - _dragStartPos).length();
            if (distance > DRAG_THRESHOLD && _draggingTile) {
                cugl::Vec2 newPos = mousePos + _dragOffset;
                _draggingTile->pos = newPos;
                _draggingTile->tileRect.origin = newPos;
            }
        }
    }

    if (mouseReleased) {
        // Discarding logic
        if(_draggingTile && discardArea.contains(mousePos)) {
            _matchController.discardTile(_draggingTile);
        }
        else if(_draggingTile && _celestialBox.contains(mousePos)) {
            _matchController.playCelestial(_draggingTile);
        }
        if (_dragInitiated && _draggingTile) {
            float distance = (mousePos - _dragStartPos).length();
            if (distance > DRAG_THRESHOLD) {
                if (_draggingTile) {
                    if (shouldReturn) {
                        _draggingTile->selected = false;
                        _draggingTile->pos = _originalTilePos;
                        _draggingTile->tileRect.origin = _originalTilePos;
                    }
                }
            }
        }
        _dragInitiated = false;
        _originalTilePos = cugl::Vec2::ZERO;
        
        // Player hand rearranging (dragging)
        int newIndex = _player->getHand().getTileIndexAtPosition(mousePos);

        if (newIndex != -1 && _draggingTile) {
            auto& tiles = _player->getHand().getTiles();
            auto tile = std::find(tiles.begin(), tiles.end(), _draggingTile);
            if (tile != tiles.end()) {
            tiles.erase(tile);
            }
            newIndex = std::min(newIndex, (int)tiles.size());
            tiles.insert(tiles.begin() + newIndex, _draggingTile);
        }
        _player->_draggingTile = nullptr;
        _player->getHand().updateTilePositions(_matchScene->getSize());
        releaseTile();
    }
}

void GameScene::discardTile(std::shared_ptr<TileSet::Tile> tile) {
    if(!(_network->getCurrentTurn() == _network->getLocalPid()) || (_player->getHand()._size >= _player->getHand()._tiles.size() && !_player->forcedDiscard)){
        return;
    }
    if (!_player->discarding){
        _player->discarding = true;
        //Add to discard pile
        tile->selected = false;
        tile->inHostHand = false;
        tile->inClientHand = false;
        tile->discarded = true;
        if (!(tile->_suit == TileSet::Tile::Suit::CELESTIAL)){
            _discardPile->addTile(tile);
            _discardPile->updateTilePositions();
            _tileSet->tilesToJson.push_back(tile);
            _network->broadcastNewDiscard(_tileSet->toJson(_tileSet->tilesToJson));
            _tileSet->clearTilesToJson();
            discardedTiles.emplace_back(tile);
        }
        _player->getHand().discard(tile, _network->getHostStatus());
    }
    _player->discarding = false;
}

