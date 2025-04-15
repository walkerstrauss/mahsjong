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
using namespace cugl::scene2;
using namespace std;

#pragma mark -
#pragma mark Level Layout

// Lock the screen esize to a fixed heigh regardless of aspect ratio
// PLEASE ADJUST AS SEEN FIT
#define SCENE_HEIGHT 720 // Change to 874 for resizing from iPhone 16 Pro aspect ratio

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
    
    _matchScene = _assets->get<scene2::SceneNode>("matchscene");
    _matchScene->setContentSize(1280,720);
    
    _discardUINode = std::make_shared<DiscardUINode>();
    _discardUINode->init(_assets);
    _discardUINode->_root->setContentSize(1280,720);
    
    cugl::Size screenSize = cugl::Application::get()->getDisplaySize();
    
    screenSize *= _matchScene->getContentSize().height/screenSize.height;
    
    float offset = (screenSize.width -_matchScene->getWidth())/2;
    _matchScene->setPosition(offset, _matchScene->getPosition().y);

    _discardUINode->_root->setPosition(offset, _discardUINode->getPosition().y);
    
    if (!Scene2::initWithHint(screenSize)) {
        std::cerr << "Scene2 initialization failed!" << std::endl;
        return false;
    }
    
    _tilesetUIBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("matchscene.gameplayscene.discardButton"));
    _pauseBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("matchscene.gameplayscene.pauseButton"));
    _endTurnBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("matchscene.gameplayscene.endTurnButton"));
    _endTurnBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            _matchController->endTurn();
        }
    });
        
    _tilesetUIBtnKey = _tilesetUIBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            setActive(false);
            setGameActive(false);
            _backBtn->activate();
            _discardUINode->_root->setVisible(true);
            AnimationController::getInstance().pause();
        }
    });
    
    _pauseBtnKey = _pauseBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            _choice = Choice::PAUSE;
        }
    });
    
    _backBtn = std::dynamic_pointer_cast<scene2::Button>(
        _discardUINode->_root->getChildByName("tilesetscene")->getChildByName("board")->getChildByName("buttonClose"));
    
    _backBtnKey = _backBtn->addListener([this](const std::string& name, bool down) {
        if (!down) {
            setActive(true);
            setGameActive(true);
            _discardUINode->_root->setVisible(false);
        }
    });
        
    addChild(_matchScene);
    addChild(_discardUINode->_root);
    // Game Win and Lose bool
    _gameWin = false;
    _gameLose = false;
    
    // Init the match controller for the game
    _matchController = std::make_shared<MatchController>();
    _matchController->init(_assets, _network);
  
    // Host and Client specific initializations
    if(_network->getHostStatus()){
        _matchController->initHost();
        _player = _matchController->hostPlayer;
    } else {
        _matchController->initClient();
        _player = _matchController->clientPlayer;
    }
    
    // Premature repositioning so it tiles don't render in the corner of the screen
    _player->getHand().updateTilePositions(_matchScene->getSize());
    
    //Initialization of shared objects
    _tileSet = _matchController->getTileSet();
    _pile = _matchController->getPile();
    _discardPile = _matchController->getDiscardPile();
    
    // Init the Rect of the discard pile.
    if (_discardPile->getTopTile()) {
        std::shared_ptr<TileSet::Tile> tile = _discardPile->getTopTile();
        
        cugl::Size textureSize(350.0f, 415.0f);
        cugl::Vec2 pos(990, 520); // same pos as in discardPile
        cugl::Vec2 origin = (textureSize * tile->_scale) / 2;
        cugl::Vec2 rectOrigin = pos - origin;
        
        _discardBox = cugl::Rect(rectOrigin, textureSize * tile->_scale);
    }
    else{
        _discardBox = cugl::Rect(990 - 87.5, 520 - 103.75, 175, 207.5);
    }
    
    _input.init(); //Initialize the input controller
    
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
    
    // Initializing the active play/discard region
    std::shared_ptr<scene2::SceneNode> activeRegionNode = _assets->get<scene2::SceneNode>("matchscene.gameplayscene.activeRegion");
    cugl::Vec2 worldOrigin = activeRegionNode->nodeToWorldCoords(Vec2::ZERO);
    _activeRegion = cugl::Rect(worldOrigin, activeRegionNode->getContentSize());
    
    // Init the button for playing sets.
    _playSetBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("matchscene.gameplayscene.playSetButton"));

    cugl::Rect rect(0, 0, 150, 50);
    cugl::Poly2 poly(rect);

    std::shared_ptr<scene2::PolygonNode> upPlaceholder = scene2::PolygonNode::alloc();
    upPlaceholder->setPolygon(poly);
    upPlaceholder->setContentSize(cugl::Size(150, 50));
    upPlaceholder->setColor(cugl::Color4::GRAY);
    
    std::shared_ptr<scene2::PolygonNode> downPlaceholder = scene2::PolygonNode::alloc();
    downPlaceholder->setPolygon(poly);
    downPlaceholder->setContentSize(cugl::Size(150, 50));
    cugl::Color4 darkGray(64, 64, 64, 255);
    downPlaceholder->setColor(darkGray);
    
    _playSetBtn = scene2::Button::alloc(upPlaceholder, downPlaceholder);
    _playSetBtn->setContentSize(cugl::Size(150, 50));
    _playSetBtn->setAnchor(cugl::Vec2::ANCHOR_CENTER);
    _playSetBtn->setPosition(cugl::Vec2(850, 360));
    _playSetBtn->setColor(cugl::Color4::RED);
    
    _playSetBtn->deactivate();
    _playSetBtn->setVisible(false);
    _matchScene->addChild(_playSetBtn);
    
    _playSetBtn->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _matchController->playSet();
        }
    });
   // Init all animations
    _actionAnimNode = std::make_shared<AnimatedNode>();
    _actionAnimNode->setAnchor(Vec2::ANCHOR_CENTER);
    _actionAnimNode->setVisible(false);
    _actionAnimNode->setContentSize(Size(70, 70));
    _actionAnimNode->doLayout();
    _actionAnimNode->initWithData(_assets.get(), _assets->get<JsonValue>("animations"), "gameplay-action",12.0f);

    _pongSheet = SpriteNode::allocWithSheet(_assets->get<Texture>("pong-sheet"), 2, 3);
    _pongSheet->setAnchor(Vec2::ANCHOR_CENTER);
    _pongSheet->setPosition(screenSize.width/2,screenSize.height/2);
    _pongSheet->setScale(0.2);
    _pongSheet->setFrame(0);
    _pongSheet->setVisible(false);

    _chowSheet = SpriteNode::allocWithSheet(_assets->get<Texture>("chow-sheet"), 3, 3, 7);
    _chowSheet->setAnchor(Vec2::ANCHOR_CENTER);
    _chowSheet->setPosition(screenSize.width/2,screenSize.height/2);
    _chowSheet->setScale(0.2);
    _chowSheet->setFrame(0);
    _chowSheet->setVisible(false);

    _turnSheet = SpriteNode::allocWithSheet(_assets->get<Texture>("turn-sheet"), 2, 3, 3);
    _turnSheet->setAnchor(Vec2::ANCHOR_CENTER);
    _turnSheet->setPosition(1085,screenSize.height/2);
    _turnSheet->setScale(0.12);
    _turnSheet->setFrame(0);
    _turnSheet->setVisible(false);
    
    // Ideal method of adding animations below 
//    AnimationController::getInstance().addSpriteSheetAnimation(_pongSheet, 0, _pongSheet->getCount(), true);
//    AnimationController::getInstance().addSpriteSheetAnimation(_chowSheet, 0, _chowSheet->getCount(), true);
//    AnimationController::getInstance().addSpriteSheetAnimation(_turnSheet, 0, _turnSheet->getCount(), true);
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void GameScene::dispose() {
    if (_active) {
        _matchController->dispose();
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
    //Reading input
    _input.readInput();
    _input.update();
    
    _matchController->update(timestep);
    
    // Fetching current mouse position
    cugl::Vec2 mousePos = cugl::Scene::screenToWorldCoords(cugl::Vec3(_input.getPosition()));
    
    // Constantly updating the position of tiles in hand
    _player->getHand().updateTilePositions(_matchScene->getSize());
        
    // Updating discardUINode if matchController has a discard update
    if(_matchController->getChoice() == MatchController::Choice::DISCARDUIUPDATE) {
        _discardUINode->incrementLabel(_discardPile->getTopTile());
        _matchController->setChoice(MatchController::NONE);
    }
    
    // If play set button was active and matchController state is NONE, deactivate button
    if(_playSetBtn->isActive() && _matchController->getChoice() == MatchController::NONE) {
        _playSetBtn->setVisible(false);
        _playSetBtn->deactivate();
    }
        
    if(!isActive()) {
        return;
    }
    
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
        // If in drawn discard state, disallow any other action other then playing a set
        // Coords of initial click and ending release
        cugl::Vec2 initialMousePos = cugl::Scene::screenToWorldCoords(cugl::Vec3(_input.getInitialPosition()));
        
        if(_matchController->getChoice() == MatchController::DRAWNDISCARD) {
            return;
        }

        bool releasedInPile = _input.didRelease() && _pileBox.contains(mousePos);
        // Drawing (from pile) logic
        if(_pileBox.contains(initialMousePos) && releasedInPile) {
            _matchController->drawTile();
        }
        
        //Drawing (from discard) logic
        bool releasedInDiscard = _input.didRelease() && _discardBox.contains(mousePos);
        if(_discardBox.contains(initialMousePos) && releasedInDiscard) {
            // If drawing from discard is successful activate play set button
            if(_matchController->drawDiscard()) {
                _player->getHand().updateTilePositions(_matchScene->getSize());
                _playSetBtn->setVisible(true);
                _playSetBtn->activate();
            };
        }
    }

//    updateSpriteNodes(timestep);
//    
//    if (_input.getKeyPressed() == KeyCode::P && _input.getKeyDown()){
////        _actionAnimNode->setVisible(true);
////        _actionAnimNode->play("pong-sheet", AnimatedNode::AnimationType::INTERRUPT, _assets->get<Texture>("pong-sheet"));
//        _pongSheet->setVisible(true);
//        
//    } else if (_input.getKeyPressed() == KeyCode::C && _input.getKeyDown()){
////        _actionAnimNode->setVisible(true);
////        _actionAnimNode->play("chow-sheet", AnimatedNode::AnimationType::INTERRUPT, _assets->get<Texture>("chow-sheet"));
//        _chowSheet->setVisible(true);
//    } else if (_input.getKeyPressed() == KeyCode::T && _input.getKeyDown()){
//        _turnSheet->setVisible(true);
//    } else if (_input.getKeyPressed() == KeyCode::SPACE && _input.getKeyDown()){
//        _pongSheet->setVisible(false);
//        _chowSheet->setVisible(false);
//        _turnSheet->setVisible(false);
//    }
//    _actionAnimNode->update(timestep);
//    AnimationController::getInstance().update(timestep);
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
    // If there is an update w/ discard pile (drawing from discard) then stop rendering tile
    if(!(_network->getStatus() == NetworkController::DRAWNDISCARD)) {
        _discardPile->draw(_batch);
    }
    _player->draw(_batch);

//    if (_actionAnimNode->isVisible()) _batch->draw(_actionAnimNode->getTexture(), Rect(600,400,670,470));
    if (_turnSheet->isVisible()) _turnSheet->render(_batch);
    if (_pongSheet->isVisible()) _pongSheet->render(_batch);
    if (_chowSheet->isVisible()) _chowSheet->render(_batch);

    _discardUINode->_root->render(_batch);
    
    _batch->setColor(Color4(255, 0, 0, 200));
    _batch->setTexture(nullptr);
    
    
    
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
        _endTurnBtn->deactivate();
        _backBtn->deactivate();
    }
}

void GameScene::applyCelestial(TileSet::Tile::Rank type) {
    if (type == TileSet::Tile::Rank::OX) {
        _pile->reshufflePile();
        _network->broadcastDeckMap(_tileSet->mapToJson());
        _network->broadcastPileLayer();
    }
}

void GameScene::clickedTile(cugl::Vec2 mousePos){
    cugl::Vec2 initialMousePos = cugl::Scene::screenToWorldCoords(cugl::Vec3(_input.getInitialPosition()));
    
    for(const auto& pair : _tileSet->tileMap){
        std::shared_ptr<TileSet::Tile> currTile = pair.second;
        // If you cannot select or deselect the tile return
        if(currTile->tileRect.contains(mousePos) && currTile->tileRect.contains(initialMousePos)){
            if((_network->getHostStatus() && currTile->inHostHand) || (!_network->getHostStatus() && currTile->inClientHand)) {
                if(currTile->selected) {
                    if(currTile->unselectable) {
                        return;
                    }
                    // TODO: Play deselect sound effect
                    AnimationController::getInstance().animateTileDeselect(currTile, 30);
                    auto it = std::find(_player->getHand()._selectedTiles.begin(), _player->getHand()._selectedTiles.end(), currTile);
                    if (it != _player->getHand()._selectedTiles.end()) {
                        _player->getHand()._selectedTiles.erase(it);
                    currTile->selected = false;
                    }
                }
                else {
                    if(!currTile->selectable) {
                        return;
                    }
                    // TODO: Play select sound effect
                    AnimationController::getInstance().animateTileSelect(currTile, 30);
                    _player->getHand()._selectedTiles.push_back(currTile);
                    currTile->selected = true;
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

            //if(currTile->selected) {
            //    currTile->selected = false;
            //}
            //else {
            //   currTile->selected = true;
            //}
        }
    }
}


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
            _draggingTile = _player->getHand().getTileAtPosition(mousePos);
            if(_draggingTile && _draggingTile->unselectable) {
                return;
            }
            _dragInitiated = true;
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
        // Active play area logic. Ensure you only do these actions when it is your turn.
        if(_draggingTile && _activeRegion.contains(mousePos) && _network->getCurrentTurn() == _network->getLocalPid()) {
            if(_matchController->getChoice() != MatchController::DRAWNDISCARD) {
              if(_draggingTile->_suit == TileSet::Tile::Suit::CELESTIAL && !_draggingTile->debuffed) {
                  _matchController->playCelestial(_draggingTile);
              }
              else {
                  // Monkey tile was played, regular tile chosen to trade
                  if (_matchController->getChoice() == MatchController::Choice::MONKEYTILE) {
                      _matchController->playMonkey(_draggingTile);

                      // Rebind _player to prevent null ptr error
                      _player = _network->getHostStatus() ? _matchController->hostPlayer : _matchController->clientPlayer;
                      _matchController->setChoice(MatchController::Choice::NONE);
                  }
                  // Regular tile getting discarded
                  else if(_matchController->discardTile(_draggingTile)) {
                      _discardUINode->incrementLabel(_draggingTile);
                  };
              }
           }
        }
        if (_dragInitiated && _draggingTile) {
            float distance = (mousePos - _dragStartPos).length();
            if (distance > DRAG_THRESHOLD) {
                if (_draggingTile) {
                    if (shouldReturn) {
                        _draggingTile->selected = false;
                        
                        auto& selected = _player->getHand()._selectedTiles;
                        auto it = std::find(selected.begin(), selected.end(), _draggingTile);
                        if (it != selected.end()) {
                            selected.erase(it);
                        }
                        
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
            
        if (newIndex != -1 && _dragInitiated) {
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

void GameScene::playSetAnim(const std::vector<std::shared_ptr<TileSet::Tile>>& tiles){
    if (tiles.size() != 3 || !_actionAnimNode){
        std::string animKey;
        if (isPong(tiles)){
            animKey = "pong";
        } else if (isChow(tiles)){
            animKey = "chow";
        } else {
            return;
        }
//        _actionAnimNode->play(animKey, AnimatedNode::AnimationType::INTERRUPT);
    }
    
    
}

bool GameScene::isPong(const std::vector<std::shared_ptr<TileSet::Tile>>& tiles){
    for (auto& tile : tiles){
        if (tile->getSuit() == TileSet::Tile::Suit::CELESTIAL){
            return false;
        }
    }
    
    return (tiles[0]->toString() == tiles[1]->toString() &&
            tiles[1]->toString() == tiles[2]->toString());
}

bool GameScene::isChow(const std::vector<std::shared_ptr<TileSet::Tile>>& tiles){
    for (auto& tile : tiles){
        if (tile->getSuit() == TileSet::Tile::Suit::CELESTIAL){
            return false;
        }
    }
    auto sorted = _hand->getSortedTiles(tiles);
    return (sorted[0]->getSuit() == sorted[1]->getSuit() &&
            sorted[1]->getSuit() == sorted[2]->getSuit() &&
            TileSet::Tile::toIntRank(sorted[1]->getRank()) - 1 == TileSet::Tile::toIntRank(sorted[0]->getRank()) &&
            TileSet::Tile::toIntRank(sorted[2]->getRank()) - 1 == TileSet::Tile::toIntRank(sorted[1]->getRank()));
}
