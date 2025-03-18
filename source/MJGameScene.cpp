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
    
    Size dimen = getSize();
    _matchScene = _assets->get<scene2::SceneNode>("matchscene");
    _matchScene->setContentSize(dimen);
    _matchScene->doLayout();
   
    _discardBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("matchscene.gameplayscene.discard"));
    _tilesetUIBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("matchscene.gameplayscene.button_tileset"));
    _pauseBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("matchscene.gameplayscene.button_pause"));
    
    _discardBtnKey = _discardBtn->addListener([this](const std::string& name, bool down){
        if (!down){
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
                        _discardPile->addTile(tile);
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
    
    if(_network->getHostStatus()){
        _tileSet->initHostDeck();
        _tileSet->shuffle();
        _player->getHand().initHost(_tileSet);
    } else {
        _tileSet->initClientDeck(_network->getDeckJson());
        _player->getHand().initClient(_tileSet);
        }
    

    std::string msg = strtool::format("Score: %d", _player->_totalScore);
    _text = TextLayout::allocWithText(msg, assets->get<Font>("pixel32"));
    _text->layout();
    _win = TextLayout::allocWithText("You Won!\nReset with R", assets->get<Font>("pixel32"));
    _win->layout();
    _lose = TextLayout::allocWithText("You Lost!\nReset with R", assets->get<Font>("pixel32"));
    _lose->layout();

    _tileSet->setAllTileTexture(assets);
    _pile = std::make_shared<Pile>(); //Init our pile
    _pile->initPile(5, _tileSet);

    
//    _tileSet->setBackTextures(assets);
    
    if(_network->getHostStatus()){
        _network->initGame(_tileSet->toJson(_tileSet->deck));
    } else {
        _network->broadcastDeck(_tileSet->toJson(_tileSet->deck));
    }

    
    // Initialize the discard pile
    _discardPile = std::make_shared<DiscardPile>();
    _discardPile->init(_assets);

    _input.init(); //Initialize the input controller
    
    _quit = false;
    setActive(false);
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
    
    if (_input.getKeyPressed() == KeyCode::R && _input.getKeyDown()) {
        reset();
    }
    
    if (_gameLose || _gameWin) {
        return;
    }
    
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
                // TODO: send to app to update UI
                _network->setStatus(NetworkController::Status::INGAME);
            }
        }
    }
    
    std::shared_ptr<TileSet::Tile> lastTile = nullptr;
    if (!_player->getHand()._drawnPile.empty()) {
        lastTile = _player->getHand()._drawnPile.back();
    }
    
    if(_network->getStatus() == NetworkController::Status::LAYER) {
        _pile->createPile();
        
//        _network->broadcastDeck(_tileSet->toJson(_tileSet->deck));
        _network->setStatus(NetworkController::Status::INGAME);
    }
    
    _player->getHand().updateTilePositions();
    
    //if (_network->getCurrentTurn() == _network->getLocalPid()) {
    if(_input.didRelease() && !_input.isDown()){
        cugl::Vec2 prev = _input.getPosition(); //Get our mouse position
        cugl::Vec2 mousePos = cugl::Scene::screenToWorldCoords(cugl::Vec3(prev));
        _player->getHand().clickedTile(mousePos);
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
            _network->broadcastDeck(_tileSet->toJson(_tileSet->deck));
            if (_player->getHand().isWinningHand()){
                _gameWin = true;
            }
            _player->canDraw = false;
        }
        
        if (_pile->getVisibleSize() == 0) {
            _network->broadcastPileLayer();
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
                            // TODO: send to app to update UI
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
        if (_input.getKeyPressed() == KeyCode::W && _input.getKeyDown()){
            _choice = Choice::WIN;
        }
        if (_input.getKeyPressed() == KeyCode::L && _input.getKeyDown()){
            _choice = Choice::LOSE;
        }
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
    _matchScene->render(_batch);
    _tileSet->draw(_batch, getSize(), _network->getHostStatus());
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
        _matchScene->setVisible(true);
        _pauseBtn->activate();
        _discardBtn->activate();
        _tilesetUIBtn->activate();
    } else {
        _matchScene->setVisible(false);
        _pauseBtn->deactivate();
        _discardBtn->deactivate();
        _tilesetUIBtn->deactivate();
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
            else{
                tile->selected = false;
            }
        }
    }
}

void GameScene::clickedTile(cugl::Vec2 mousePos){
    for(auto& tile : _tileSet->deck){
        bool inHand;
        if (_network->getHostStatus()) {
            inHand = tile->inHostHand;
        } else {
            inHand = tile->inClientHand;
        }
        if (inHand || tile->discarded || tile->played){
            continue;
        }
        else if (tile->inPile){
            if (tile->tileRect.contains(mousePos)){
                if(tile->selected){
                    tile->selected = false;
                }
                else{
                    tile->selected = true;
                }
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
