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
    _discardBtn = std::dynamic_pointer_cast<scene2::Button>(childNode->getChild(6));
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
                        //Add to discard UI scene
                        _discardUIScene->incrementLabel(tile);
                        
                        _player->getHand().discard(tile);
                        tile->selected = false;
                        tile->inHand = false;
                        tile->inPile = false;
                        tile->discarded = true; 
                    }
                    _player->getHand()._selectedTiles.clear();
                    _player->discarding = false;
                    
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
    _tileSet->shuffle();
    
    // Initialize the player
    _player = std::make_shared<Player>();
    _player->getHand().init(_tileSet);
    _player->getHand().updateTilePositions();
    
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
    _tileSet->setBackTextures(assets);
    
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
    init(_assets, _network);
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
    
    _player->getHand().updateTilePositions();
    CULog("%lu", _player->getHand()._selectedTiles.size());
    
    //if (_network->getCurrentTurn() == _network->getLocalPid()) {
        if(_input.didRelease() && !_input.isDown()){
            cugl::Vec2 prev = _input.getPosition(); //Get our mouse position
            cugl::Vec2 mousePos = cugl::Scene::screenToWorldCoords(cugl::Vec3(prev));
            _player->getHand().clickedTile(mousePos);
            _discardPile->isTileSelected(mousePos);
        }
  
        //Start turn by drawing tile to hand
        if(_input.getKeyPressed() == KeyCode::D && _input.getKeyDown()){
            if(_player->getHand()._tiles.size() >= 14 || _player->getHand()._tiles.size() <= 12){
                CULog("Incorrect hand size for draw action");
                return;
            }
            _player->getHand().drawFromPile(_pile, 1);
//            _player->canDraw = false;
            
            for (auto& tile : _player->getHand()._drawnPile) {
                tile->setTexture(_assets->get<Texture>(tile->toString()));
            }
            
            if (_player->getHand().isWinningHand()){
                _gameWin = true;
        }
        if (_input.getKeyPressed() == KeyCode::G && _input.getKeyDown()){
            if(_player->getHand()._selectedTiles.size() != 2){
                CULog("Must have 2 tiles selected in hand");
                return;
            }
            else if (!_discardPile->getTopTile()){
                CULog("Must have a tile in the discard pile");
                return;
            }
            //Temporarily add the top tile from the discard pile into selected tiles
            std::shared_ptr<TileSet::Tile> currDiscardTile = _discardPile->getTopTile();
            _player->getHand()._selectedTiles.push_back(currDiscardTile);
            
            if (_player->getHand().isSetValid(_player->getHand()._selectedTiles)) { // Selected tiles include ones in the piles...
                for(auto const& tile : _player->getHand()._selectedTiles){
                    if(tile == currDiscardTile){
                        continue;
                    }
                    for(auto it = _player->getHand()._tiles.begin(); it != _player->getHand()._tiles.end();)
                        if(tile == *it){
                            it = _player->getHand()._tiles.erase(it);
                            tile->selected = false;
                            tile->inHand = false;
                            tile->played = true;
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
                _discardPile->removeTopTile();
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
        
        if (_input.getKeyPressed() == KeyCode::E && _input.getKeyDown()) {
            if(_player->canDraw || _player->canExchange){
                CULog("Must draw from pile or discard first");
                return;
            }
            if(_player->getHand()._tiles.size() - _player->getHand()._selectedTiles.size() != _player->getHand()._size){
                CULog("Discard count must make hand equal to required size");
                return;
            }
            for(auto& tile: _player->getHand()._selectedTiles){
                _player->getHand().discard(tile);
                _discardPile->addTile(tile);
                _discardPile->updateTilePositions();
            }
        }

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

//        else if(_input.getKeyPressed() == KeyCode::E && _input.getKeyDown()){
//            //Error exchange cases
//            if(!(_player->getHand()._selectedTiles.size() == 1)){
//                CULog("Trying to exchange w/o selecting 1 tile in hand");
//                if(_player->getHand()._selectedTiles.size() > 1){
//                    for(auto& tile: _player->getHand()._selectedTiles){
//                        tile->selected = false;
//                    }
//                }
//                else if((_player->getHand()._selectedTiles.size() == 0)){
//                    CULog("Trying to exchange w/o selecting tiles in hand");
//                }
//            }
//            if(!(_discardPile->getTopTile()->selected == true)){
//                CULog("Trying to exchange w/o selecting a tile in discard");
//            }
//            //Start Exchanging
//            else{
//                _player->exchanging = true;
//                currDiscardTile = _discardPile->getTopTile();
//                currHandTile = _player->getHand()._selectedTiles[0];
//                
//                currHandTile->inHand = false;
//                currHandTile->discarded = true;
//                
//                for(auto it = _player->getHand()._tiles.begin(); it!= _player->getHand()._tiles.end();){
//                    if(*it == currHandTile){
//                        it = _player->getHand()._tiles.erase(it);
//                        break;
//                    } else{
//                        it++;
//                    }
//                }
//                _player->getHand()._selectedTiles.clear();
//                
//                currDiscardTile->discarded = false;
//                currDiscardTile->inHand = true;
//                _discardPile->removeTopTile();
//                
//                _player->getHand()._tiles.push_back(currDiscardTile);
//                _discardPile->addTile(currHandTile);
//            }
//        }
//        //Handling playing set in exchange and play phase
//        else if(_player->exchanging){
//            if(_input.getKeyPressed() == KeyCode::P && _input.getKeyDown()){
//                if(_player->getHand().isSetValid(_player->getHand()._selectedTiles)){
//                    _player->getHand()._size -= _player->getHand()._selectedTiles.size();
//                    for(auto& inSet : _player->getHand()._selectedTiles){
//                        for(auto it = _player->getHand()._tiles.begin(); it!= _player->getHand()._tiles.end();){
//                            if(*it == inSet){
//                                it = _player->getHand()._tiles.erase(it);
//                                break;
//                            } else{
//                                it++;
//                            }
//                        }
//                    }
//                    //Clear everything once played set
//                    _player->getHand()._selectedTiles.clear();
//                    currHandTile = nullptr;
//                    currDiscardTile = nullptr;
//                }
//                else{
//                    CULog("Invalid set during exchange and play");
//                    for(auto& tile : _player->getHand()._selectedTiles){
//                        tile->selected = false;
//                    }
//                    for(auto it = _player->getHand()._tiles.begin(); it != _player->getHand()._tiles.end();){
//                        if(*it == currDiscardTile){
//                            currDiscardTile->inHand = false;
//                            currDiscardTile->discarded = true;
//                            it = _player->getHand()._tiles.erase(it);
//                        }
//                    }
//                    _player->getHand()._tiles.push_back(currHandTile);
//                    _discardPile->removeTopTile();
//                    _discardPile->addTile(currDiscardTile);
//                    _player->getHand()._selectedTiles.clear();
//                    _player->exchanging = false;
//                }
//            }
//        }
    }
    
    
//    //If there was a click we check if it was on a tile in the pile
//    if((_input.didRelease() && !_input.isDown())){
//        cugl::Vec2 prev = _input.getPosition(); //Get our mouse posistion
//        cugl::Vec2 mousePos = cugl::Scene::screenToWorldCoords(cugl::Vec3(prev));
//        _player->getHand().clickedTile(mousePos);
//        _pile->pairs(mousePos, _player);
//    }
//    
//    // See if we selected a pile tile and/or made a pair
//    if(_pile->_pairs.size() == 2 && _player->getHand()._selectedTiles.size() == 2){
//        cugl::Vec2 prev = _input.getPosition(); //Get our mouse posistion
//        cugl::Vec2 mousePos = cugl::Scene::screenToWorldCoords(cugl::Vec3(prev));
//        _pile->pairs(mousePos, _player);
//    }
//    if(_input.getKeyPressed() == KeyCode::P && _input.getKeyDown()){
//        _player->getHand().playSet(_tileSet);
//
//        if(_player->getHand()._tiles.size() < 14){
//            while(_player->getHand().grandmaToAdd > 0){
//                std::shared_ptr<TileSet::Tile> wildTile = _tileSet->generateWildTile();
//                wildTile->setWildTexture(_assets);
//                wildTile->inHand = true;
//                wildTile->_scale = 0.2;
//                
//                _tileSet->deck.push_back(wildTile);
//                
//                _player->getHand()._tiles.push_back(wildTile);
//                _player->getHand().grandmaToAdd -= 1;
//            }
//            _player->getHand().drawFromPile(_pile, 1);
//        }
//    } else if (_input.getKeyPressed() == KeyCode::D && _input.getKeyDown()){
//        // Discard selected cards (up to 4)
//        if (_player->getHand()._selectedTiles.size() > 0 && _player->getHand()._selectedTiles.size() <= 2) {
//            if (!_player->discarding){
//                _player->discarding = true;
//                for (auto& tile : _player->getHand()._selectedTiles) {
//                    
//                    // Add it to the discard pile
//                    _discardPile->addTile(tile);
//                    // Add it to discard UI
//                    _discardUIScene->incrementLabel(tile);
//                    
//                    _player->getHand().discard(tile);
//                    tile->selected = false;
//                    tile->inHand = false;
//                    tile->inPile = false;
//                    tile->discarded = true;
//                }
//                _player->getHand()._selectedTiles.clear();
//                _player->getHand().drawFromPile(_pile, 1);
//                _player->discarding = false;
//            }
//        } else {
//            for (auto& tile : _player->getHand()._selectedTiles) {
//                tile->selected = false;
//            }
//            _player->getHand()._selectedTiles.clear();
//        }
//        
//    } else if (_input.getKeyPressed() == KeyCode::S && _input.getKeyDown()){
//            if(!_player->getHand().makeSet()){
//                for(const auto& it : _player->getHand()._selectedTiles){
//                    it->selected = false;
//                }
//                _player->getHand()._selectedTiles.clear();
//                };
//    }
//        _text->setText(strtool::format("Score: %d", _player->_totalScore));
//        _text->layout();
//
//        //Win or Lose
//        if (_player->_totalScore >= 20) {
//            _gameWin = true;
//        }
//        else if (_player->_turnsLeft <= 0 && _gameWin != true) {
//            _gameLose = true;
//        }
//    if (_discardPile->isTileSelected(_input.getPosition())){
//        CULog("selected");
//        // TODO: add code to handle checking if we can make a set with top discard and adding to hand if we can
//        // TODO: once is added to hand, we have to make it so that they cannot do anything until they have made and shown a set with the discard tile
//    }
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
    _tileSet->draw(_batch, getSize());
    
    // Draw score
//    _batch->setColor(Color4::GREEN);
//    _batch->drawText(_text,Vec2(getSize().width - _text->getBounds().size.width - 10, getSize().height-_text->getBounds().size.height));
    
    // Check if we need to flip over next layer of the pile
    if (_pile->getVisibleSize() == 0 && _tileSet->deck.size() != 14) { //Only update pile if we still have tiles from deck
        _pile->createPile();
    }
    
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


