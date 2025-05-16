//
//  MJTutorialScene.cpp
//  Mahsjong
//
//  Created by Walker Strauss on 4/22/25.
//

#include <cugl/cugl.h>
#include <iostream>
#include <sstream>
#include "MJTutorialScene.h"

using namespace cugl;
using namespace cugl::scene2;

#pragma mark -
#pragma mark Level Layout

// Lock the screen size to a fixed height regardless of aspect ratio
#define SCENE_HEIGHT 720
#define TAP_ACTIVE_LENGTH 10.0f

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
bool TutorialScene::init(const std::shared_ptr<cugl::AssetManager> &assets, std::shared_ptr<NetworkController> &network, std::shared_ptr<InputController> &inputController) {
    // Initialize the scene to a locked height
    if(assets == nullptr) {
        return false;
    } else if (!Scene2::initWithHint(Size(0, SCENE_HEIGHT))) {
        std::cerr << "Scene2 initialization failed!" << std::endl;
        return false;
    }
    
    Size screenSize = cugl::Application::get()->getDisplaySize();
    
#pragma mark Initializing app objects
    _assets = assets;
    _choice = NONE;
    _prevPhase = NOT;
    _phase = NOT;
    setPhase(START);
    _input = inputController;
    _network = network;
    
#pragma mark Initializing scene nodes
    // MatchScene Node
    _matchScene = _assets->get<SceneNode>("tutorialscene");
    _matchScene->setContentSize(getSize());
    _matchScene->getChild(0)->setContentSize(_matchScene->getContentSize());
    _matchScene->doLayout();
    screenSize *= _matchScene->getContentSize().height/screenSize.height;
    float offset = (screenSize.width - _matchScene->getWidth()) / 2;
    _matchScene->setPosition(offset, _matchScene->getPosition().y);
    
    // PlayArea node
    _playArea = _assets->get<SceneNode>("tutorialscene.gameplayscene.play-area");
    _playArea->setVisible(false);
    
    // TradeArea node
    _tradeArea = _assets->get<SceneNode>("tutorialscene.gameplayscene.drag-to-trade");
    _tradeArea->setVisible(false);
    
    _opponentHandRec = _assets->get<SceneNode>("tutorialscene.gameplayscene.opponent-hand-rec");
    
#pragma mark Initializing scene regions
    std::shared_ptr<scene2::SceneNode> activeRegionNode = _assets->get<scene2::SceneNode>("tutorialscene.gameplayscene.drag-to-discard-tile");
    std::shared_ptr<scene2::SceneNode> discardedTileRegionNode = _assets->get<scene2::SceneNode>("tutorialscene.gameplayscene.discarded-tile.discarded-rec");
    std::shared_ptr<scene2::SceneNode> playerHandRegionNode = _assets->get<scene2::SceneNode>("tutorialscene.gameplayscene.playerhand");
    std::shared_ptr<scene2::SceneNode> pileRegionNode = _assets->get<scene2::SceneNode>("tutorialscene.gameplayscene.pile");
    
    cugl::Vec2 activeRegionWorldOrigin = activeRegionNode->nodeToWorldCoords(Vec2::ZERO);
    cugl::Vec2 discardedTileRegionWorldOrigin = discardedTileRegionNode->nodeToWorldCoords(Vec2::ZERO);
    cugl::Vec2 playerHandRegionWorldOrigin = playerHandRegionNode->nodeToWorldCoords(Vec2::ZERO);
    cugl::Vec2 pileRegionNodeOrigin = pileRegionNode->nodeToWorldCoords(Vec2::ZERO);
    
    _activeRegion = cugl::Rect(activeRegionWorldOrigin, activeRegionNode->getContentSize());
    _discardedTileRegion = cugl::Rect(discardedTileRegionWorldOrigin, discardedTileRegionNode->getContentSize());
    _playerHandRegion = cugl::Rect(playerHandRegionWorldOrigin.x, playerHandRegionWorldOrigin.y - 300, playerHandRegionNode->getContentSize().width, playerHandRegionNode->getContentSize().height);
    _pileBox = cugl::Rect(pileRegionNodeOrigin, pileRegionNode->getContentSize());

#pragma mark Initializing textured nodes
    // Discarded tile image node
    _discardedTileImage = std::dynamic_pointer_cast<scene2::TexturedNode>(_assets->get<scene2::SceneNode>("tutorialscene.gameplayscene.discarded-tile.discarded-tile-recent.up.discarded-tile-recent"));
    
    // Drag to discard node
    _dragToDiscardNode = std::dynamic_pointer_cast<cugl::scene2::TexturedNode>(_assets->get<cugl::scene2::SceneNode>(
            "tutorialscene.gameplayscene.drag-to-discard-tile"));
    _dragToDiscardNode->setVisible(false);
    
    // Drag to hand node
    _dragToHandNode = std::dynamic_pointer_cast<cugl::scene2::TexturedNode>(_assets->get<cugl::scene2::SceneNode>(
            "tutorialscene.gameplayscene.drag-to-hand-area"));
    _dragToHandNode->setVisible(false);
    
#pragma mark Initializing custom scene nodes
    // Initialziing pile ui node
    _pileUINode = std::make_shared<PileUINode>();
    _pileUINode->init(_assets);
    _pileUINode->setContentSize(getSize());
    _pileUINode->_root->getChild(0)->setContentSize(_matchScene->getContentSize());
    _pileUINode->doLayout();
    
    //Initializing discard ui
    _discardUINode = std::make_shared<DiscardUINode>();
    _discardUINode->init(_assets);
    _discardUINode->setContentSize(getSize());
    _discardUINode->_root->getChild(0)->setContentSize(_matchScene->getContentSize());
    _discardUINode->_root->setPosition(offset, _discardUINode->getPosition().y);
    _discardUINode->doLayout();
    
#pragma mark Intializing buttons
    // Tileset UI Button
    _tilesetUIBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("tutorialscene.gameplayscene.discarded-tile.discard-can"));
    _tilesetUIBtnKey = _tilesetUIBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            setActive(false);
            setTutorialActive(false);
            _backBtn->activate();
            _discardUINode->_root->setVisible(true);
            AnimationController::getInstance().pause();
            AudioController::getInstance().playSound("Select");
        }
    });
    
    // Back Button
    _backBtn = std::dynamic_pointer_cast<scene2::Button>(
        _discardUINode->_root->getChildByName("tilesetscene")->getChildByName("board")->getChildByName("buttonClose"));
    _backBtnKey = _backBtn->addListener([this](const std::string& name, bool down) {
        if (!down) {
            setActive(true);
            setTutorialActive(true);
            _discardUINode->_root->setVisible(false);
            AudioController::getInstance().playSound("Done");
        }
    });

    // Settings button
    _settingBtn = std::dynamic_pointer_cast<Button>(_assets->get<SceneNode>("tutorialscene.gameplayscene.setting-icon"));
    _settingBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            _choice = SETTING;
            AudioController::getInstance().playSound("Select");
        }
    });
    
    // Info button
    _infoBtn = std::dynamic_pointer_cast<Button>(_assets->get<SceneNode>("tutorialscene.gameplayscene.information-icon"));
    _infoBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            _choice = INFO;
            AudioController::getInstance().playSound("Select",false);
        }
    });
    
    // Playset button
    _playSetBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("tutorialscene.gameplayscene.playSetButton"));
    _playSetBtn->setVisible(false);
    _playSetBtn->addListener([this](const std::string& name, bool down) {
        if (!down) {
            AudioController::getInstance().playSound("Select");
            if(!_matchController->playSet()) {
                _discardedTileImage->setVisible(true);
                setPhase(DRAW_DIS);
            } else {
                setPhase(SET_DISCARD);
            }
        }
    });
    
    // Opponent hand tab button
    _opponentHandBtn = std::dynamic_pointer_cast<Button>(_assets->get<SceneNode>("tutorialscene.gameplayscene.opponent-hand"));
    _opponentHandBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            AudioController::getInstance().playSound("Select");
            _opponentHandRec->setVisible(!_opponentHandRec->isVisible());
            for (int i = 0; i < _opponentHandTiles.size(); i++){
                _opponentHandTiles[i]->setVisible(!_opponentHandTiles[i]->isVisible());
            }
        }
    });
<<<<<<< HEAD
=======

    //_presentation = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<cugl::scene2::SceneNode>("tutorial.tutorialScene.tu1"));
    //_slides = {"tu1", "tu2", "tu3", "tu4", "tu5", "tu6", "tu7", "tu8", "tu9", "tu10", "tu11"};
    //_slide = 0;
    //_presentation->setTexture(_assets->get<cugl::graphics::Texture>(_slides[_slide]));
    //_presentation->setContentSize(_tutorialScene->getContentSize());
    //_presentation->setPosition(_tutorialScene->getWidth() / 2, _tutorialScene->getHeight() / 2);

>>>>>>> 127450907bf350d22bf8125e6b4018675c93b5a6
    
    //Initializing textures for tabs
    for (int i = 0; i < 14; i++){
        std::shared_ptr<TexturedNode> tile = std::dynamic_pointer_cast<TexturedNode>(_assets->get<SceneNode>("tutorialscene.gameplayscene.opponent-hand-tile.tile-back_" + std::to_string(i)));
        if (tile){
            _opponentHandTiles.push_back(tile);
            tile->setVisible(false);
        }
    }
    for (int i = 0; i < 14; i++){
        std::shared_ptr<TexturedNode> tile = std::dynamic_pointer_cast<TexturedNode>(_assets->get<SceneNode>("tutorialscene.gameplayscene.player-hand-tile.tile-back_" + std::to_string(i)));
        if (tile){
            _playerHandTiles.push_back(tile);
            tile->setVisible(false);
        }
    }
    
    _opponentHandBtn->activate();
    initTurnIndicators();
    
#pragma mark Initializing in game objects
    // Initializing match controller
    _matchController = std::make_shared<MatchController>();
    _matchController->init(_assets, network);
    _matchController->initTutorial();
    
    _player = _matchController->hostPlayer;
    _tileSet = _matchController->getTileSet();
    _pile = _matchController->getPile();
    _discardPile = _matchController->getDiscardPile();
    
    // Premature update to set player hand
    _player->getHand().updateTilePositions(_playerHandRegion, 0);
    for(auto& tile : _player->getHand()._tiles) {
        tile->getContainer()->setAnchor(Vec2::ANCHOR_CENTER);
        tile->getContainer()->setScale(tile->_scale);
        tile->getContainer()->setPosition(tile->pos);
    }
    
    //Setting pile location
    _pile->pileBox = pileRegionNode->getBoundingBox();
    _pile->setTilePositions(false);
    for(auto& row: _pile->_pile) {
        for(auto& tile: row) {
            if(tile == nullptr) {
                continue;
            }
            tile->getContainer()->setAnchor(Vec2::ANCHOR_CENTER);
            tile->getContainer()->setScale(tile->_scale);
            tile->getContainer()->setPosition(tile->pos);
        }
    }

    updateTurnIndicators();
    
#pragma mark Initializing UI Nodes
    // Start with shade visible
//    _shade = _assets->get<SceneNode>("tutorialscene.gameplayscene.shade");
//    _shade->setVisible(true);
//    _uiElements.push_back(_shade);
    initTutorialUINodes();
    
    addChild(_matchScene);
    addChild(_discardUINode->_root);
    addChild(_pileUINode->_root);
    
    setActive(false);
    _matchScene->setVisible(true);
    _discardedTileImage->setVisible(false);
    
    return true;
}

#pragma mark -
#pragma mark Gameplay Handling
void TutorialScene::update(float timestep) {
<<<<<<< HEAD
=======

//    _input.readInput();
//    _input.update();

    //if (_input->didPress() && !_back->isDown() && _slide < 11 && !_exit->isDown()) {
    //    if (_slide == 10){
    //        _slide = 0;
    //        _choice = BACK;

>>>>>>> 127450907bf350d22bf8125e6b4018675c93b5a6
    // Retrieve mouse position
    Vec2 mousePos = Scene::screenToWorldCoords(Vec3(_input->getPosition()));
    // Update player hand tile positions
    _player->getHand().updateTilePositions(_playerHandRegion, timestep);
    // Update pile tile positions
    _pile->updateTilePositions(timestep);
    // Updating match controller
    _matchController->update(timestep);
    // Updating discard pile positions
    _discardPile->updateTilePositions(timestep);
    
    updateAreaVisibility(mousePos, timestep);
    updateTurnIndicators();
    displayPlayerSets();
    displayOpponentSets();
    
    if(!isActive()) {
        return;
    }
    
    // Clicking/Tapping and Dragging logic
    if(_input->didRelease() && !_input->isDown()) {
        cugl::Vec2 initialMousePos = cugl::Scene::screenToWorldCoords(cugl::Vec3(_input->getInitialPosition()));
        if(-TAP_ACTIVE_LENGTH <= (initialMousePos - mousePos).length() <= TAP_ACTIVE_LENGTH) {
                clickedTile(mousePos);
        }
    }
    
    updateDrag(mousePos, _input->isDown(), _input->didRelease(), timestep);
    
    if(_phase == START) {
        if (_input->didRelease() && !_input->isDown()){
            setPhase(WELCOME);
        }
    }
    
    if(_phase == WELCOME){
        if (_welcomeTimer < _welcomeDelay){
            _welcomeTimer += timestep;
<<<<<<< HEAD
>>>>>>> origin/main-2.0
=======
>>>>>>> 127450907bf350d22bf8125e6b4018675c93b5a6
            return;
        }
        
        if (_input->didRelease() && !_input->isDown()){
            setPhase(ONE_DRAW);
            _welcomeTimer = 0.0f;
            return;
        }
    }
    
    if(_phase == ONE_DRAW) {
        Vec2 initialMousePos = cugl::Scene::screenToWorldCoords(cugl::Vec3(_input->getInitialPosition()));
        bool releasedInPile = _input->didRelease() && _pileBox.contains(mousePos);
        if(_pileBox.contains(initialMousePos) && releasedInPile) {
            if(_matchController->hasDrawn) {
                // Whatever you want to put here for tutorial like ("CAN'T DRAW AGAIN")
            }
            _matchController->drawTile();
            setPhase(DISCARD);
        }
    }
    
    if(_phase == DISCARD) {
        
    }
    
    if(_phase == ONE_OPP || _phase == TWO_OPP) {
        if (_turnTimer < _opponentDelay){
            _turnTimer += timestep;
            return;
        }
        
        _turnTimer = 0.0f;
        
        if (_phase == ONE_OPP){
            _opponentTile1 = _pile->tilesDrawn(1)[0];
            _opponentTile1->_scale = 0;
            _opponentTile1->selected = false;
            _opponentTile1->inHostHand = false;
            _opponentTile1->inClientHand = false;
            _opponentTile1->discarded = true;
        }
        
        if (_phase == TWO_OPP){
            _opponentTile2 = _pile->tilesDrawn(1)[0];
            _opponentTile2->_scale = 0;
            _opponentTile2->selected = false;
            _opponentTile2->inHostHand = false;
            _opponentTile2->inClientHand = false;
            _opponentTile2->discarded = true;
        }
        setPhase((_phase == ONE_OPP) ? ONE_END : TWO_END);
    }
    
    if (_phase == ONE_END || _phase == TWO_END){
        if (_discardTimer < _discardDelay){
            _discardTimer += timestep;
            return;
        }
        
        _discardTimer = 0.0f;
        
        if (_phase == ONE_END){
            _discardPile->addTile(_opponentTile1);
            _discardUINode->incrementLabel(_discardUINode->getLabelIndex(_opponentTile1));
            _discardedTileImage->setTexture(_assets->get<Texture>(_opponentTile1->toString()));
            _discardedTileImage->SceneNode::setContentSize(32.88, 45);
            _discardedTileImage->setVisible(true);
            _matchController->resetTurn();
        }
        
        if (_phase == TWO_END){
            _discardPile->addTile(_opponentTile2);
            _discardUINode->incrementLabel(_discardUINode->getLabelIndex(_opponentTile2));
            _discardedTileImage->setTexture(_assets->get<Texture>(_opponentTile2->toString()));
            _discardedTileImage->SceneNode::setContentSize(32.88, 45);
            _discardedTileImage->setVisible(true);
            _matchController->resetTurn();
        }
        
        setPhase((_phase == ONE_END) ? DRAW_DIS : TWO_DRAW);
    }
    
    if(_phase == DRAW_DIS) {
        
    }
    
    if (_phase == PRESS){
        std::vector<std::shared_ptr<TileSet::Tile>> tiles;
        for (auto& tile: _player->getHand()._selectedTiles){
            if (tile->getRank() == TileSet::Tile::Rank::TWO && tile->getSuit() == TileSet::Tile::Suit::BAMBOO){
                tiles.push_back(tile);
            } else {
                tile->selected = false;
            }
        }
        _player->getHand()._selectedTiles = tiles;

        
        for (auto& tile: _player->getHand()._tiles){
            if (tile->getRank() != TileSet::Tile::Rank::TWO || tile->getSuit() != TileSet::Tile::Suit::BAMBOO){
                tile->selectable = false;
            }
        }
        
        if (_player->getHand()._selectedTiles.size() == 3) {
            setPhase(PLAY_SET);
            return;
        }
        
        int i = 0;
        for (auto& tile : _player->getHand()._tiles){
            if (tile->getSuit() == TileSet::Tile::Suit::BAMBOO && tile->getRank() == TileSet::Tile::Rank::TWO && tile != _player->getHand()._tiles[_player->getHand()._tiles.size()-1]) {
                if (i < 2){
                    if (i == 0){
                        _pressArrow1->setPositionX(tile->pos.x);
                        i++;
                    } else {
                        _pressArrow2->setPositionX(tile->pos.x);
                        i++;
                    }
                }
            }
        }
    }
    
    if(_phase == PLAY_SET) {
        
    }
    
    if(_phase == TWO_DRAW) {
        Vec2 initialMousePos = cugl::Scene::screenToWorldCoords(cugl::Vec3(_input->getInitialPosition()));
        bool releasedInPile = _input->didRelease() && _pileBox.contains(mousePos);
        if(_pileBox.contains(initialMousePos) && releasedInPile) {
            if(_matchController->hasDrawn) {
                    // Whatever you want to put here for tutorial like ("CAN'T DRAW AGAIN")
            }
            _matchController->drawTile();
            setPhase(CELESTIAL);
        }
    }
    
    if(_phase == CELESTIAL) {
        for (auto& tile: _player->getHand()._tiles){
            if (tile->getSuit() == TileSet::Tile::Suit::CELESTIAL){
                _playCelestialArrow->setPositionX(tile->pos.x);
            }
        }
    }
    
    if(_phase == SET_DISCARD) {
        for (auto& tile: _player->getHand()._tiles){
            tile->selectable = true;
        }
        _playSetBtn->setVisible(false);
    }
    
    if (_phase == SHUFFLE){
        if (_shuffleTimer < _shuffleDelay){
            _shuffleTimer += timestep;
            return;
        }
        _shuffleTimer = 0.0f;
        setPhase(FINISHED);
    }
    
    if(_phase == FINISHED) {
        if (_finishedTimer < _finishedDelay){
            _finishedTimer += timestep;
            return;
        }
        
        if (_input->didRelease() && !_input->isDown()){
            // Transition back to main menu
            _choice = DONE;
            _finishedTimer = 0.0f;
            // Prepare game controllers for a match
            _matchController->inTutorial = false;
            _network->reset();
        }
    }
}

void TutorialScene::render() {
    _batch->begin(getCamera()->getCombined());
    
    _pileUINode->_root->render(_batch);
    _matchScene->render(_batch);
    _discardUINode->_root->render(_batch);
    
    _pile->draw(_batch);
    _player->draw(_batch);
    
    if (_dragToDiscardNode && _dragToDiscardNode->isVisible()) {
        _dragToDiscardNode->render(_batch);
    } else if (_playArea && _playArea->isVisible()) {
        _playArea->render(_batch);
    } else if (_tradeArea && _tradeArea->isVisible()) {
        _tradeArea->render(_batch);
    }
    if (_dragToHandNode && _dragToHandNode->isVisible()) {
        _dragToHandNode->render(_batch);
    }
    
    if (_draggingTile) {
        if (!_draggingTile->discarded){
            _draggingTile->getContainer()->setVisible(true);
            _draggingTile->getContainer()->render(_batch, Affine2::IDENTITY, Color4::WHITE);
        } else {
            _draggingTile->getContainer()->setVisible(true);
            _discardPile->draw(_batch);
        }
    }
    
    _batch->setColor(Color4(255, 0, 0, 200));
    _batch->setTexture(nullptr);
    
    if(_draggingTile && _draggingTile->_suit == TileSet::Tile::Suit::CELESTIAL) {
        if(_input->isDown() && _input->getInitialPosition() != _input->getPosition()) {
            _player->drawInfo(_draggingTile, _batch, _matchScene->getSize());
        }
    }
    
    for (auto& node : _uiMap[getPhaseUIMapKey(_phase)]){
        node->render(_batch);
    }
    
    _batch->end();
}

void TutorialScene::clickedTile(cugl::Vec2 mousePos){
    cugl::Vec2 initialMousePos = cugl::Scene::screenToWorldCoords(cugl::Vec3(_input->getInitialPosition()));
    
    for(const auto& pair : _tileSet->tileMap){
        std::shared_ptr<TileSet::Tile> currTile = pair.second;
        // If you cannot select or deselect the tile return
        if(currTile->tileRect.contains(mousePos) && currTile->tileRect.contains(initialMousePos)){
            if((_network->getHostStatus() && currTile->inHostHand) || (!_network->getHostStatus() && currTile->inClientHand)) {
                if(currTile->selectable) {
                    if(currTile->selected) {
                        AudioController::getInstance().playSound("Unclick");
                        auto it = std::find(_player->getHand()._selectedTiles.begin(), _player->getHand()._selectedTiles.end(), currTile);
                        if (it != _player->getHand()._selectedTiles.end()) {
                            _player->getHand()._selectedTiles.erase(it);
                            currTile->selected = false;
                        }
                    }
                    else {
                        AudioController::getInstance().playSound("Click");
                        _player->getHand()._selectedTiles.push_back(currTile);
                        currTile->selected = true;
                    }
                }
            }
            if(currTile->inPile && _pileUINode->getState() == PileUINode::RATSELECT) {
                _matchController->playRat(currTile);
                _matchController->setChoice(MatchController::Choice::NONE);
                _pileUINode->setState(PileUINode::NONE);
            }
            
            if(currTile->inPile && _pileUINode->getState() == PileUINode::DRAGONROW) {
                _dragonRow = _pile->selectedRow(currTile);
                _pileUINode->setState(PileUINode::DRAGONREARRANGE);
            }
        }
    }
}

void TutorialScene::updateDrag(const cugl::Vec2& mousePos, bool mouseDown, bool mouseReleased, float timestep) {
    auto& dragContainer = (_pileUINode->getState() == PileUINode::DRAGONREARRANGE)
                          ? _pile->_pile[_dragonRow]
                          : _player->getHand().getTiles();
    
    if (mouseDown) {
        if (!_dragInitiated) {
            _dragStartPos = mousePos;
            _draggingTile = getTileAtPosition(mousePos, dragContainer);
            _dragFromDiscard = false;
            if(_draggingTile && !_draggingTile->selectable) {
                return;
            }
            _dragInitiated = true;
            _player->_draggingTile = _draggingTile;
            if (_draggingTile) {
                _originalTilePos = _draggingTile->pos;
                _dragOffset = _draggingTile->pos - mousePos;
            }
            else if(_phase == DRAW_DIS && !_tilesetUIBtn->isDown() && _discardedTileRegion.contains(mousePos)) {
                if(_discardPile->getTopTile()) {
                    _dragFromDiscard = true;
                    _discardedTileImage->setVisible(false);
                    AudioController::getInstance().playSound("DrawDiscard");
                    _draggingTile = _discardPile->getTopTile();
                    _draggingTile->pos = mousePos;
                    _draggingTile->_scale = 0.325;
                }
                else {
                    AudioController::getInstance().playSound("WrongAction");
                }
            }
        }
        else {
            float distance = (mousePos - _dragStartPos).length();
            if (distance > DRAG_THRESHOLD && _draggingTile) {
                cugl::Vec2 newPos = mousePos + _dragOffset;
                _draggingTile->pos = newPos;
                _draggingTile->tileRect.origin = newPos;
                
                if (_draggingTile && !_dragFromDiscard) {
                    auto& tiles = dragContainer;

                     auto it = std::find(tiles.begin(), tiles.end(), _draggingTile);
                     if (it != tiles.end()) {
                         int oldIndex = static_cast<int>(std::distance(tiles.begin(), it));

                         if (oldIndex < tiles.size() - 1) {
                             // check right neighbor
                             auto rightTile = tiles[oldIndex + 1];
                             if (_draggingTile->pos.x > rightTile->pos.x) {
                                 std::swap(tiles[oldIndex], tiles[oldIndex + 1]);
                                 (_pileUINode->getState() == PileUINode::DRAGONREARRANGE)
                                 ? _pile->setTilePositions(false)
                                     : _player->getHand().updateTilePositions(_playerHandRegion, timestep);
                             }
                         }
                         if (oldIndex > 0) {
                             auto leftTile = tiles[oldIndex - 1];
                             if (leftTile && _draggingTile->pos.x < leftTile->pos.x) {
                                 std::swap(tiles[oldIndex], tiles[oldIndex - 1]);
                                 (_pileUINode->getState() == PileUINode::DRAGONREARRANGE)
                                 ? _pile->setTilePositions(false)
                                     : _player->getHand().updateTilePositions(_playerHandRegion, timestep);
                             }
                         }
                     }
                }
            }
        }
    }
    if (mouseReleased) {
        // Active play area logic. Ensure you only do these actions when it is your turn.
        if(_draggingTile && _activeRegion.contains(mousePos)) {
            if (_network->getCurrentTurn() == _network->getLocalPid()) {
                if(_matchController->getChoice() == MatchController::DRAWNDISCARD) {
                    if(_draggingTile->_suit == TileSet::Tile::Suit::CELESTIAL) {
                        AudioController::getInstance().playSound("WrongAction", false);
                    } else {
                        if (_matchController->getChoice() != MatchController::PIGTILE && _matchController->getChoice() != MatchController::RATTILE) {
                            AudioController::getInstance().playSound("WrongAction", false);
                        }
                    }
                } else {
                    if(_phase == CELESTIAL && _draggingTile->_suit == TileSet::Tile::Suit::CELESTIAL && !_draggingTile->debuffed) {
                        if (!_matchController->playCelestial(_draggingTile) && _matchController->getChoice() != MatchController::DRAGONTILE) {
                            AudioController::getInstance().playSound("WrongAction", false);
                        }
                        else {
                            setPhase(SHUFFLE);
                        }
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
                      else if((_phase == DISCARD || _phase == SET_DISCARD) && _matchController->discardTile(_draggingTile)) {
                          _draggingTile->pos = _discardedTileImage->getWorldPosition();
                          if(_draggingTile->debuffed) {
                              _discardedTileImage->setTexture(_assets->get<Texture>("debuffed"));
                          }
                          else {
                              _discardedTileImage->setTexture(_assets->get<Texture>(_draggingTile->toString()));
                          }
                          _discardedTileImage->SceneNode::setContentSize(32.88, 45);
                          _discardedTileImage->setVisible(true);
                          int index = _discardUINode->getLabelIndex(_draggingTile);
                          _discardUINode->incrementLabel(index);
                          _draggingTile->_scale = 0;
                          
                          setPhase((_phase == DISCARD) ? ONE_OPP : TWO_OPP);
                      } else if (_matchController->getChoice() != MatchController::DRAGONTILE){
                          AudioController::getInstance().playSound("WrongAction", false);
                      }
                    }
                }
            } else {
                AudioController::getInstance().playSound("WrongAction", false);
            }
        }
        if (_dragInitiated && _draggingTile) {
            float distance = (mousePos - _dragStartPos).length();
            if(_draggingTile->discarded) {
                if(_playerHandRegion.contains(mousePos) && _phase == DRAW_DIS) {
                    if(_matchController->drawDiscard()) {
                        _playSetBtn->activate();
                        _playSetBtn->setVisible(true);
                        _draggingTile->_scale = 0.325;
                        _matchController->hasDrawn = true;
                        setPhase(PRESS);
                    }
                    else {
                        _draggingTile->pos = _discardedTileImage->getWorldPosition();
                        _discardedTileImage->setVisible(true);
                        _draggingTile->_scale = 0;
                    }
                }
                else if (_matchController->getChoice() != MatchController::DRAWNDISCARD){
                    _discardedTileImage->setVisible(true);
                    _draggingTile->_scale = 0;
                }
            }
            else if (distance > DRAG_THRESHOLD) {
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

        int newIndex = getIndexAtPosition(mousePos, dragContainer);
            
        if (newIndex != -1 && _draggingTile) {

            auto& tiles = dragContainer;

            if((_draggingTile->discarded && _matchController->hasDrawn) || (_network->getLocalPid() != _network->getCurrentTurn())) {
                _player->_draggingTile = nullptr;
                releaseTile();
                return;
            }


            auto tile = std::find(tiles.begin(), tiles.end(), _draggingTile);
            if (tile != tiles.end()) {
                tiles.erase(tile);
            }
                newIndex = std::min(newIndex, (int)tiles.size());
                tiles.insert(tiles.begin() + newIndex, _draggingTile);
            }
            _player->_draggingTile = nullptr;
        
        (_pileUINode->getState() == PileUINode::DRAGONREARRANGE)
                    ? _pile->updateRow(_dragonRow, dragContainer, timestep)
                    :_player->getHand().updateTilePositions(_playerHandRegion, timestep);

            releaseTile();
     }
}


void TutorialScene::updateAreaVisibility(Vec2 mousePos, float timestep){
    bool isDragging = (_dragInitiated && _draggingTile != nullptr);
    int hand_length = static_cast<int>(_player->getHand()._tiles.size());
    
    bool shouldShowToHand = isDragging && _dragFromDiscard;
    bool shouldShow = isDragging && !_dragFromDiscard && _matchController->getChoice() != MatchController::DRAGONTILE && hand_length > _player->getHand()._size;
    bool shouldShowDiscard = shouldShow && !_playSetBtn->isVisible() && _dragStartPos != mousePos && _draggingTile->getSuit() != TileSet::Tile::Suit::CELESTIAL;
    bool shouldShowPlay = shouldShow && _draggingTile->getSuit() == TileSet::Tile::Suit::CELESTIAL && _dragStartPos != mousePos && !_playSetBtn->isVisible() && _matchController->getChoice() != MatchController::MONKEYTILE;
    
    AnimationController::getInstance().tryAddFade(_dragToHandNode, shouldShowToHand, 0.2f, _wasDragToHandVisible);
    AnimationController::getInstance().tryAddFade(_dragToDiscardNode, shouldShowDiscard, 0.2f, _wasDragToDiscardVisible);
    AnimationController::getInstance().tryAddFade(_playArea, shouldShowPlay, 0.2f, _wasPlayAreaVisible);
}

void TutorialScene::dispose() {
    if(_active) {
        _matchController->dispose();

        _tileSet = nullptr;
        _pile = nullptr;
        _discardPile = nullptr;
        _player = nullptr;
        
        if (_playSetBtn) {
            _playSetBtn->clearListeners();
            _playSetBtn->deactivate();
        }
        _playSetBtn = nullptr;
        if (_backBtn) {
            _backBtn->clearListeners();
            _backBtn->deactivate();
        }
        _backBtn = nullptr;
        if (_infoBtn) {
            _infoBtn->clearListeners();
            _infoBtn->deactivate();
        }
        _infoBtn = nullptr;
        if (_settingBtn) {
            _settingBtn->clearListeners();
            _settingBtn->deactivate();
        }
        _settingBtn = nullptr;
        if (_tilesetUIBtn) {
            _tilesetUIBtn->clearListeners();
            _tilesetUIBtn->deactivate();
        }
        _tilesetUIBtn = nullptr;
        if (_playerHandBtn) {
            _playerHandBtn->clearListeners();
            _playerHandBtn->deactivate();
        }
        _playerHandBtn = nullptr;
        if (_playerHandBtn2) {
            _playerHandBtn2->clearListeners();
            _playerHandBtn2->deactivate();
        }
        _playerHandBtn2 = nullptr;
        if (_opponentHandBtn) {
            _opponentHandBtn->clearListeners();
            _opponentHandBtn->deactivate();
        }
        _opponentHandBtn = nullptr;
        if (_opponentHandBtn2) {
            _opponentHandBtn2->clearListeners();
            _opponentHandBtn2->deactivate();
        }
        _opponentHandBtn2 = nullptr;
        
        removeAllChildren();
        
        _active = false;
    }
}

void TutorialScene::setActive(bool value) {
    if(isActive() != value) {
        Scene2::setActive(value);
    }
}

void TutorialScene::setTutorialActive(bool value) {
    if(value) {
        _choice = NONE;
        setPhase(START);
        _tilesetUIBtn->activate();
        _settingBtn->activate();
        _infoBtn->activate();
        updateTurnIndicators();
    }
    else {
        _backBtn->deactivate();
        _settingBtn->deactivate();
        _infoBtn->deactivate();
        updateTurnIndicators();
        dispose();
    }
}

