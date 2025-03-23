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
bool GameScene::init(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<NetworkController> network) {
    // Initialize the scene to a locked height
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::initWithHint(Size(0,SCENE_HEIGHT))) {
        std::cerr << "Scene2 initialization failed!" << std::endl;
        return false;
    }

    _assets = assets;
    _network = network;
    _choice = Choice::NONE;
    
//    Size dimen = getSize();
    _matchScene = _assets->get<scene2::SceneNode>("matchscene");
    _matchScene->setContentSize(1280,720);
    cugl::Size screenSize = cugl::Application::get()->getDisplaySize();
    //cugl::Size screenSize = Size(0,SCENE_HEIGHT);
    
    screenSize *= _matchScene->getContentSize().height/screenSize.height;
    
    float offset = (screenSize.width -_matchScene->getWidth())/2;    
    _matchScene->setPosition(offset, _matchScene->getPosition().y);

    
    if (!Scene2::initWithHint(screenSize)) {
        std::cerr << "Scene2 initialization failed!" << std::endl;
        return false;
    }

    _matchScene->setContentSize(screenSize);
    _matchScene->doLayout();
   
    std::shared_ptr<scene2::SceneNode> childNode = _matchScene->getChild(0);
    _discardBtn = std::dynamic_pointer_cast<scene2::Button>(childNode->getChild(3));
    _tilesetUIBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("matchscene.gameplayscene.discardButton"));
    _pauseBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("matchscene.gameplayscene.pauseButton"));
  
    _discardBtnKey = _discardBtn->addListener([this](const std::string& name, bool down){
        if (!down){
//            _pile->reshufflePile();
            _network->broadcastDeckMap(_tileSet->mapToJson()); // Sends tile state
            _network->broadcastPileLayer();
            if(_player->getHand()._tiles.size() == _player->getHand()._size){
                CULog("Cannot discard tiles, already at required hand size");
                return;
            }
            if (0 < _player->getHand()._selectedTiles.size() && _player->getHand()._selectedTiles.size() < 2) {
                if (!_player->discarding){
                    _player->discarding = true;
                    for (auto& tile : _player->getHand()._selectedTiles) {
                        //Add to discard pile
                        tile->selected = false;
                        tile->inHostHand = false;
                        tile->inClientHand = false;
                        tile->discarded = true;
                        _discardPile->addTile(tile);
                        _discardPile->updateTilePositions();
                        _tileSet->tilesToJson.push_back(tile);
                        _network->broadcastNewDiscard(_tileSet->toJson(_tileSet->tilesToJson));
                        _tileSet->clearTilesToJson();
                        //Add to discard UI scene
                        // TODO: add to discard ui scene from app
                        _player->getHand().discard(tile, _network->getHostStatus());
                    }
                    _player->getHand()._selectedTiles.clear();
                    _player->discarding = false;
                    _network->broadcastDeck(_tileSet->toJson(_tileSet->deck));
                    _tileSet->clearTilesToJson();
//                    _network->endTurn();
                }
            }
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

    // Initialize tile set
    _tileSet = std::make_shared<TileSet>();
    _player = std::make_shared<Player>();
    _pile = std::make_shared<Pile>(); //Init our pile

    if(_network->getHostStatus()){
        //Setting up whole deck
        _tileSet->initHostDeck();
        _tileSet->setAllTileTexture(assets);
        _tileSet->shuffle();
        //Initializes the deck (ordered representation)
        _network->initGame(_tileSet->toJson(_tileSet->deck));

        //Setting up hand
        _player->getHand().initHand(_tileSet, _network->getHostStatus());
        //Updating tile positions in hand
        _player->getHand().updateTilePositions(getSize());
        
        //Creating pile as Host
        _pile->initPile(5, _tileSet, _network->getHostStatus());
        //Broadcasting all tiles with attributes (sets pile tiles as !inDeck)
        _network->broadcastStartingDeck(_tileSet->mapToJson());
    } else {
        //Initializing client pile (pile full of nullptrs)
        _pile->initPile(5, _tileSet, _network->getHostStatus());
        
        //Initialzing client deck
        _tileSet->initClientDeck(_network->getStartingDeck());
        _tileSet->setAllTileTexture(_assets);
        _tileSet->updateDeck(_network->getDeckJson());
        _player->getHand().initHand(_tileSet, _network->getHostStatus());
        _player->getHand().updateTilePositions(getSize());
        _tileSet->addActionAndCommandTiles(assets);
        _tileSet->shuffle();
        _pile->remakePile();
        _network->broadcastDeck(_tileSet->mapToJson());
    }

    std::string msg = strtool::format("Score: %d", _player->_totalScore);
    _text = TextLayout::allocWithText(msg, assets->get<Font>("pixel32"));
    _text->layout();
    _win = TextLayout::allocWithText("You Won!\nReset with R", assets->get<Font>("pixel32"));
    _win->layout();
    _lose = TextLayout::allocWithText("You Lost!\nReset with R", assets->get<Font>("pixel32"));
    _lose->layout();

    
//    _tileSet->setBackTextures(assets);
//    _winBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("matchscene.gameplayscene.winButton"));
//    _defeatBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("matchscene.gameplayscene.loseButton"));
//    
//    _winBtn->addListener([this](const std::string& name, bool down){
//        if (!down){
//            _choice = Choice::WIN;
//        }
//    });
//        // Initialize the discard pile
//        _discardPile = std::make_shared<DiscardPile>();
//        _discardPile->init(_assets);
//    });
//                         
//    _defeatBtn->addListener([this](const std::string& name, bool down){
//        if (!down){
//            _choice = Choice::LOSE;
//        }
//    });

    
    // Initialize the discard pile
    _discardPile = std::make_shared<DiscardPile>();
    _discardPile->init(_assets);

    _input.init(); //Initialize the input controller
    
    _quit = false;
    setActive(false);
    _matchScene->setVisible(true);
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
    init(_assets, _network);
    return;
}

/**
 * The method to update the game mode
 *
 * @param timestep The amount of time (in seconds) since the last frame
 */
void GameScene::update(float timestep) {
    
    if(_discardPile->getTopTile()){
        CULog("%d", _discardPile->getTopTile()->played);
    }
//    for(auto const& tile : _player->getHand()._tiles){
//        CULog("%s, %s", tile->toString().c_str(), tile->pos.toString().c_str());
//    }
    //Reading input
    _input.readInput();
    _input.update();
    

    cugl::Vec2 mousePos = cugl::Scene::screenToWorldCoords(cugl::Vec3(_input.getPosition()));
     
    bool isMouseDown = _input.isDown();
    bool isMouseReleased = _input.didRelease();
    
    // Update the player's drag state.
    _player->updateDrag(mousePos, isMouseDown, isMouseReleased);
    _player->getHand().updateTilePositions(_matchScene->getSize());

    
    if (_network->getStatus() == NetworkController::Status::DECK) {
        _tileSet->updateDeck(_network->getDeckJson());
        _network->setStatus(NetworkController::Status::INGAME);
    }
    
    if(_network->getStatus() == NetworkController::Status::PILETILEUPDATE) {
        _pile->removePileTile(_network->getPileTile(), _network->getIsHostDraw());
        _tileSet->clearTilesToJson();
        _network->setStatus(NetworkController::Status::INGAME);
    }
    
    if(_discardPile->getTopTile() && _network->getStatus() == NetworkController::Status::REMOVEDISCARD){
        // TODO: handle decrement discard ui
        _discardPile->getTopTile()->played = true;
        _discardPile->getTopTile()->discarded = false;
        _discardPile->removeTopTile();
        _network->setStatus(NetworkController::Status::INGAME);
    }
    
    if(_network->getStatus() == NetworkController::Status::NEWDISCARD){
        for(auto const& tileKey : _network->getDiscardTile()->children()){
            std::string suit = tileKey->getString("suit");
            std::string rank = tileKey->getString("rank");
            std::string id = tileKey->getString("id");
            
            if(!_discardPile->getTopTile() || (_discardPile->getTopTile()->toString() + std::to_string(_discardPile->getTopTile()->_id)) != rank + " of " + suit + id){
                _discardPile->addTile(_tileSet->tileMap[rank + " of " + suit + " " + id]);
                _discardPile->updateTilePositions();
                incrementLabel(_tileSet->tileMap[rank + " of " + suit + " " + id]);
                _network->setStatus(NetworkController::Status::INGAME);
            }
        }
    }
    
    std::shared_ptr<TileSet::Tile> lastTile = nullptr;
    if (!_player->getHand()._drawnPile.empty()) {
        lastTile = _player->getHand()._drawnPile.back();
    }
    
    if(_network->getStatus() == NetworkController::Status::LAYER) {
        _tileSet->updateDeck(_network->getTileMapJson());
        _pile->remakePile();
        _network->setStatus(NetworkController::Status::INGAME);
    }

    _player->getHand().updateTilePositions(_matchScene->getSize());
    
    if(_input.didRelease() && !_input.isDown()){
        cugl::Vec2 prev = _input.getPosition(); //Get our mouse position
        cugl::Vec2 mousePos = cugl::Scene::screenToWorldCoords(cugl::Vec3(prev));
        if (_network->getCurrentTurn() == _network->getLocalPid()) {
            _discardPile->isTileSelected(mousePos);
            clickedTile(mousePos);
        }
    }
    
    if (_network->getCurrentTurn() == _network->getLocalPid()) {
        //Start turn by drawing tile to hand
        if(_input.getKeyPressed() == KeyCode::D && _input.getKeyDown()){
            if(_player->getHand()._tiles.size() > _player->getHand()._size){
                CULog("Hand too big");
                return;
            }
            _player->getHand().drawFromPile(_pile, 1, _network->getHostStatus());
            _network->broadcastTileDrawn(_tileSet->toJson(_tileSet->tilesToJson));
            _tileSet->clearTilesToJson();
            if (_pile->getVisibleSize() == 0) {
                _pile->createPile();
                _network->broadcastDeckMap(_tileSet->mapToJson());
                _network->broadcastPileLayer();
            }
            else{
                _network->broadcastDeck(_tileSet->toJson(_tileSet->deck));
            }
            if (_player->getHand().isWinningHand()){
                _gameWin = true;
            }
            _player->canDraw = false;
        }
            
        for (auto& tile : _player->getHand()._drawnPile) {
            tile->setTexture(_assets->get<Texture>(tile->toString()));
        }
        
        if (_player->getHand().isWinningHand()){
            _gameWin = true;
        }
        if (_input.getKeyPressed() == KeyCode::G && _input.getKeyDown()){
            if(_player->getHand()._selectedTiles.size() != 1 && _player->getHand()._selectedTiles.size() != 2){
                CULog("Must have 1 or 2 tiles selected in hand");
                return;
            }
            else if (!_discardPile->getTopTile()){
                CULog("Must have a tile in the discard pile");
                return;
            }
            
            //Temporarily add the top tile from the discard pile into selected tiles
            std::shared_ptr<TileSet::Tile> currDiscardTile = _discardPile->getTopTile();
            if(_player->getHand()._size == 2 && _player->getHand()._selectedTiles.size() == 1){
                if(_player->getHand()._selectedTiles[0]->toString() == _discardPile->getTopTile()->toString()){
                    _gameWin = true;
                }
                else{
                    _player->getHand()._selectedTiles[0]->selected = false;
                }
            }
            _player->getHand()._selectedTiles.push_back(currDiscardTile);
            
            if (_player->getHand().isSetValid(_player->getHand()._selectedTiles)) {
                for(auto const& tile : _player->getHand()._selectedTiles){
                    if(tile == currDiscardTile){
                        continue;
                    }
                    for(auto it = _player->getHand()._tiles.begin(); it != _player->getHand()._tiles.end();)
                        if(tile == *it){
                            it = _player->getHand()._tiles.erase(it);
                            tile->selected = false;
                            if (_network->getHostStatus()) {
                                tile->inHostHand = false;
                            } else {
                                tile->inClientHand = false;
                            }
                            tile->played = true;
                            incrementLabel(tile);
                            break;
                        }
                        else{
                            it++;
                        }
                }
                _player->getHand()._size -= 2;
                _player->getHand()._selectedTiles.clear();
                
                currDiscardTile->discarded = false;
                currDiscardTile->played = true;
                _network->broadcastRemoveDiscard();
                // TODO: handle decrement discard UI label
                _discardPile->removeTopTile();
                _player->canExchange = false;
            }
            else {
                CULog("Not a valid set");
                for(auto const& tile : _player->getHand()._tiles){
                    if(tile->selected){
                        tile->selected = false;
                    }
                }
                _player->getHand()._selectedTiles.clear();
            }
        }
//        if (_input.getKeyPressed() == KeyCode::W && _input.getKeyDown()){
//            _choice = Choice::WIN;
//        }
//        if (_input.getKeyPressed() == KeyCode::L && _input.getKeyDown()){
//            _choice = Choice::LOSE;
//        }
//        if (_input.getKeyPressed() == KeyCode::E && _input.getKeyDown()) {
//            if(_player->canDraw || _player->canExchange){
//                CULog("Must draw from pile or discard first");
//                return;
//            }
//            if(_player->getHand()._tiles.size() - _player->getHand()._selectedTiles.size() != _player->getHand()._size){
//                CULog("Discard count must make hand equal to required size");
//                return;
//            }
//            for(auto& tile: _player->getHand()._selectedTiles){
//                _player->getHand().discard(tile, _network->getHostStatus());
//                _discardPile->addTile(tile);
//                _discardPile->updateTilePositions();
//            }
//        }
        
        
//
        if (_input.getKeyPressed() == KeyCode::N && _input.getKeyDown()) {
            if(_player->canDraw && _player->canExchange){
                CULog("Must perform a draw from pile or discard first");
                return;
            }
            if(_player->getHand()._tiles.size() != _player->getHand()._size){
                CULog("Must meet hand size requirement");
                return;
            }
            _network->endTurn();
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
    _player->draw(_batch);
    _pile->draw(_batch);
    _discardPile->draw(_batch);
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
        _discardBtn->activate();
        _tilesetUIBtn->activate();
//        _winBtn->activate();
//        _defeatBtn->activate();
    } else {
        _pauseBtn->deactivate();
        _discardBtn->deactivate();
        _tilesetUIBtn->deactivate();
//        _winBtn->deactivate();
//        _defeatBtn->deactivate();
    }
}

void GameScene::processData(std::vector<std::string> msg){
    std::string name = msg[0];
    std::string id = msg[1];
    std::string selected = msg[2];
    
    for(const auto& tile : _player->getHand()._tiles){
        if(tile->toString() == name && std::to_string(tile->_id) == id){
            if(selected == "true"){
                tile->selected = true;
            }
        }
    }
}

void GameScene::applyAction(std::shared_ptr<TileSet::ActionTile> actionTile) {
    switch (actionTile->type) {
        case TileSet::ActionTile::ActionType::CHAOS:
            CULog("CHAOS: Reshuffling the pile...");
            _pile->reshufflePile();
            // broadcast pile info
            _player->getHand().discard(actionTile, _network->getHostStatus());
            break;
        case TileSet::ActionTile::ActionType::ECHO:
            CULog("ECHO: Draw two tiles...");
            _player->getHand().drawFromPile(_pile, 2, _network->getHostStatus());
            break;
        default:
            break;
    }
}

void GameScene::applyCommand(std::shared_ptr<TileSet::CommandTile> commandTile) {
    switch (commandTile->type) {
        case TileSet::CommandTile::CommandType::OBLIVION:
            CULog("OBLIVION: Removing all action tiles from hand...");
            int actionCount = _player->getHand().loseActions(_network->getHostStatus());
            
            while (_player->getHand().getTileCount() < 13) {
                _player->getHand().drawFromPile(_pile, 1, _network->getHostStatus());
                _network->broadcastTileDrawn(_tileSet->toJson(_tileSet->tilesToJson));
                _tileSet->clearTilesToJson();
                if (_pile->getVisibleSize() == 0) {
                    _pile->createPile();
                    _network->broadcastDeckMap(_tileSet->mapToJson());
                    _network->broadcastPileLayer();
                }
                else {
                    _network->broadcastDeck(_tileSet->toJson(_tileSet->deck));
                }
                if (_player->getHand().isWinningHand()){
                    _gameWin = true;
                }
                _network->endTurn();
            }
            break;

    }
}

void GameScene::clickedTile(cugl::Vec2 mousePos){
    for(const auto& pair : _tileSet->tileMap){
        std::shared_ptr<TileSet::Tile> currTile = pair.second;
        if(currTile->tileRect.contains(mousePos)){
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
                if(currTile->selected) {
                    currTile->_scale = 0.2;
                }
                else{
                    currTile->_scale = 0.25;
                }
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



void GameScene::pressTile(){
    
    cugl::Vec2 screenPos = _input.getPosition();
    cugl::Vec2 mousePos = cugl::Scene::screenToWorldCoords(cugl::Vec3(screenPos));
    
    // if the player tapped on a tile in the hand.
    for (auto & tile : _player->getHand()._tiles) {
        if (tile->tileRect.contains(mousePos)) {
            // select this tile
            CULog("selected a tile");
            tile->selected = true;
        }
    }
    
    
    // if the player pressed on the pile
    if (_pileBox.contains(mousePos)) {
        
        if(_player->getHand()._tiles.size() > _player->getHand()._size){
            CULog("Hand too big");
            return;
        }
        _player->getHand().drawFromPile(_pile, 1, _network->getHostStatus());
        _network->broadcastTileDrawn(_tileSet->toJson(_tileSet->tilesToJson));
        _tileSet->clearTilesToJson();
        _network->broadcastDeck(_tileSet->toJson(_tileSet->deck));
        if (_player->getHand().isWinningHand()){
            _gameWin = true;
        }
        _player->canDraw = false;
    }
    
    // does it work for the pile? -- no?
    for(auto& tile : _tileSet->deck){
        
        if (tile->tileRect.contains(mousePos)){
            if (tile->tileRect.contains(mousePos)){
                
                _draggingTile = tile;
                _draggingTile->pressed = true;
                _dragOffset = _draggingTile->pos - mousePos;
                break;
            }else{ // tile is not pressed if the user don't touch the tile anymore.
                tile->pressed = false;
            }
            
        }
    }
}

void GameScene::dragTile(){
    
    cugl::Vec2 screenPos = _input.getPosition();
    cugl::Vec2 mousePos = cugl::Scene::screenToWorldCoords(cugl::Vec3(screenPos));
    
    cugl::Vec2 newPos = mousePos + _dragOffset;
    
    _draggingTile->pos = newPos;
    _draggingTile->tileRect.origin = newPos;
    

}

void GameScene::releaseTile() {
    // finalize tile if needed
    // e.g. snap to discard region, check if in the right place
    _draggingTile = nullptr;
}
