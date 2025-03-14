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
    _paused = false;
    _assets = assets;
    _network = network;
    
    Size dimen = getSize();
    _matchScene = _assets->get<scene2::SceneNode>("matchscene");
    _matchScene->setContentSize(dimen);
    _matchScene->doLayout();
    _pauseScene = _assets->get<scene2::SceneNode>("pause");
    _pauseScene->doLayout();
   
    std::shared_ptr<scene2::SceneNode> childNode = _matchScene->getChild(0);
    _discardBtn = std::dynamic_pointer_cast<scene2::Button>(childNode->getChild(3));
    _tilesetUIBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("matchscene.gameplayscene.button_tileset"));
    _pauseBtn = std::dynamic_pointer_cast<scene2::Button>(childNode->getChild(1));
    _continueBtn = std::dynamic_pointer_cast<scene2::Button>(_pauseScene->getChild(0)->getChild(2));
    
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
                        _discardPile->addTile(tile);
                        _tileSet->tilesToJson.push_back(tile);
                        _network->broadcastNewDiscard(_tileSet->toJson(_tileSet->tilesToJson));
                        _tileSet->clearTilesToJson();
                        //Add to discard UI scene
                        _discardUIScene->incrementLabel(tile);
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
            _discardUIScene->setActive(true);
            _discardUIScene->backBtn->activate();
        }
    });
    _pauseBtnKey = _pauseBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            _matchScene->setVisible(false);
            _pauseScene->setVisible(true);
            _paused = true;
            _continueBtn->activate();
        }
    });
    _continueBtnKey = _continueBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            _matchScene->setVisible(true);
            _pauseScene->setVisible(false);
            _paused = false;
        }
    });
    _discardBtn->activate();
    _pauseBtn->activate();
    _tilesetUIBtn->activate();
    
    
    _matchScene->setVisible(true);
    addChild(_matchScene);
    addChild(_pauseScene);
    _pauseScene->setVisible(false);
    setActive(true);
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
    
    // Initialize the discard tileset UI scene
    _discardUIScene = std::make_shared<DiscardUIScene>();
    _discardUIScene->init(_assets);

    _input.init(); //Initialize the input controller
    
    // Initialize grandma tile label
    _gmaLabelTexture = assets->get<Texture>("grandma text");
    if (!_gmaLabelTexture) {
        CULog("missing gma text");
        return false;
    } else {
        _tileSet->gmaTexture = _gmaLabelTexture;
    }
    _quit = false;
    
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
    
//    for(auto const& tile : _player->getHand()._tiles){
//        CULog("%s, %s", tile->toString().c_str(), tile->pos.toString().c_str());
//    }
    //Reading input
    _input.readInput();
    _input.update();
    
    if (_input.getKeyPressed() == KeyCode::R && _input.getKeyDown()) {
        reset();
    }
    
    if (_discardUIScene->back){
        _discardUIScene->setActive(false);
        _discardUIScene->back = false;
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
        _discardUIScene->decrementLabel(_discardPile->getTopTile());
        _discardPile->removeTopTile();
        _network->setStatus(NetworkController::Status::INGAME);
    }
    
    if(_network->getStatus() == NetworkController::Status::NEWDISCARD){
        std::string suit = _network->getDiscardTile()->getString("suit");
        std::string rank = _network->getDiscardTile()->getString("rank");
        std::string id = _network->getDiscardTile()->getString("id");

        if(_discardPile->getTopTile()->toString() != rank + " " + suit && std::to_string(_discardPile->getTopTile()->_id) != id){
            _discardPile->addTile(_tileSet->tileMap[rank + " " + suit + " " + id]);
            _discardUIScene->incrementLabel(_tileSet->tileMap[rank + " " + suit + " " + id]);
            _network->setStatus(NetworkController::Status::INGAME);
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
//            _network->broadcastUpdating();
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
            if(_player->getHand()._selectedTiles.size() != 1 || _player->getHand()._selectedTiles.size() != 2){
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
                            _discardUIScene->incrementLabel(tile);
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
                _discardUIScene->decrementLabel(currDiscardTile);
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
//    const std::shared_ptr<Texture> temp = Texture::getBlank();
    // Draw background and top section
//    _batch->draw(temp, Color4(141,235,207,100), Rect(Vec2::ZERO,getSize()));
//    _batch->draw(temp,Color4(37,41,88,255),Rect(Vec2(0.0f,620),Vec2(getSize())));
    if (_paused){
        _pauseScene->render(_batch);
        _batch->end();
        return;
    }
    if (_discardUIScene->isActive()){
        _discardUIScene->render(_batch);
        _batch->end();
        return;
    }
    if (_gameWin) {
        _batch->setColor(Color4::BLUE);
        Affine2 trans;
        trans.scale(4);
        trans.translate(getSize().width / 2 - 2 * _win->getBounds().size.width, getSize().height / 2 - _win->getBounds().size.height);
        _batch->drawText(_win, trans);
        _batch->end();
        return;
    }
    if (_gameLose) {
        _batch->setColor(Color4::RED);
        //_batch->drawText(_lose, Vec2((getSize().width) / 2 - _lose->getBounds().size.height, getSize().height / 2));
        Affine2 trans;
        trans.scale(4);
        trans.translate(getSize().width / 2 - 2 * _lose->getBounds().size.width, getSize().height / 2 - _lose->getBounds().size.height);
        _batch->drawText(_lose, trans);
        _batch->end();
        return;
    }
    
    // Not paused, in discard UI, won or lost. So, render match.
    _matchScene->render(_batch);
    
    // Draw all tiles in hand, pile and grandma tiles
    _tileSet->draw(_batch, getSize(), _network->getHostStatus());
    
    // Draw score
//    _batch->setColor(Color4::GREEN);
//    _batch->drawText(_text,Vec2(getSize().width - _text->getBounds().size.width - 10, getSize().height-_text->getBounds().size.height));
    
    _batch->end();
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
