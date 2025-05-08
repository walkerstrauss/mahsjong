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

bool TutorialScene::init(const std::shared_ptr<AssetManager>& assets, std::shared_ptr<InputController>& input){
    if (assets == nullptr){
        return false;
    } else if (input == nullptr){
        return false;
    } else if (!Scene2::initWithHint(0, SCENE_HEIGHT)){
        return false;
    }
    
    _assets = assets;
    _input = input;
    _choice = Choice::NONE;
    
    _tutorialScene = assets->get<SceneNode>("matchscene");
    _tutorialScene->setContentSize(getSize());
    _tutorialScene->getChild(0)->setContentSize(_tutorialScene->getContentSize());
    _tutorialScene->doLayout();
    
    _discardUINode = std::make_shared<DiscardUINode>();
    _discardUINode->init(_assets);
    _discardUINode->setContentSize(getSize());
    _discardUINode->_root->getChild(0)->setContentSize(_tutorialScene->getContentSize());
    _discardUINode->doLayout();
    
    _playArea = _assets->get<SceneNode>("matchscene.gameplayscene.play-area");
    _playArea->setVisible(false);
    
    _tradeArea = _assets->get<SceneNode>("matchscene.gameplayscene.drag-to-trade");
    _tradeArea->setVisible(false);

    std::shared_ptr<scene2::SceneNode> activeRegionNode = _assets->get<scene2::SceneNode>("matchscene.gameplayscene.drag-to-discard-tile");
    std::shared_ptr<scene2::SceneNode> discardedTileRegionNode = _assets->get<scene2::SceneNode>("matchscene.gameplayscene.discarded-tile.discarded-rec");
    std::shared_ptr<scene2::SceneNode> playerHandRegionNode = _assets->get<scene2::SceneNode>("matchscene.gameplayscene.playerhand");
    std::shared_ptr<scene2::SceneNode> pileRegionNode = _assets->get<scene2::SceneNode>("matchscene.gameplayscene.pile");
    
    cugl::Vec2 activeRegionWorldOrigin = activeRegionNode->nodeToWorldCoords(Vec2::ZERO);
    cugl::Vec2 discardedTileRegionWorldOrigin = discardedTileRegionNode->nodeToWorldCoords(Vec2::ZERO);
    cugl::Vec2 playerHandRegionWorldOrigin = playerHandRegionNode->nodeToWorldCoords(Vec2::ZERO);
    cugl::Vec2 pileRegionNodeOrigin = pileRegionNode->nodeToWorldCoords(Vec2::ZERO);
    
    _activeRegion = cugl::Rect(activeRegionWorldOrigin, activeRegionNode->getContentSize());
    _discardedTileRegion = cugl::Rect(discardedTileRegionWorldOrigin, discardedTileRegionNode->getContentSize());
    _playerHandRegion = cugl::Rect(playerHandRegionWorldOrigin.x, playerHandRegionWorldOrigin.y - 300, playerHandRegionNode->getContentSize().width, playerHandRegionNode->getContentSize().height);
    _pileBox = cugl::Rect(pileRegionNodeOrigin, pileRegionNode->getContentSize());
    
    // Initializing tileset UI button
    _tilesetUIBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("matchscene.gameplayscene.discarded-tile.discard-can"));
    _tilesetUIBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            setActive(false);
//            setTutorialActive(false);
            _backBtn->activate();
            _discardUINode->_root->setVisible(true);
            AnimationController::getInstance().pause();
            AudioController::getInstance().playSound("Select");
        }
    });
    
    
    // Initializing back button
    _backBtn = std::dynamic_pointer_cast<scene2::Button>(
        _discardUINode->_root->getChildByName("tilesetscene")->getChildByName("board")->getChildByName("buttonClose"));
    _backBtn->addListener([this](const std::string& name, bool down) {
        if (!down) {
            setActive(true);
//            setTutorialActive(true);
            _discardUINode->_root->setVisible(false);
            AudioController::getInstance().playSound("Done");
        }
    });
    
    _settingBtn = std::dynamic_pointer_cast<Button>(_assets->get<SceneNode>("matchscene.gameplayscene.setting-icon"));
    _settingBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            _choice = SETTING;
            AudioController::getInstance().playSound("Select");
        }
    });
    
    // Initializing settings button
    _infoBtn = std::dynamic_pointer_cast<Button>(_assets->get<SceneNode>("matchscene.gameplayscene.information-icon"));
    _infoBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            _choice = INFO;
            AudioController::getInstance().playSound("Select",false);
        }
    });
    
    //     Initializing opponent hand
    _opponentHandRec = _assets->get<SceneNode>("matchscene.gameplayscene.opponent-hand-rec");
    _opponentHandBtn = std::dynamic_pointer_cast<Button>(_assets->get<SceneNode>("matchscene.gameplayscene.opponent-hand"));
    _opponentHandBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            AudioController::getInstance().playSound("Select");
            _opponentHandRec->setVisible(!_opponentHandRec->isVisible());
            for (int i = 0; i < _opponentHandTiles.size(); i++){
                _opponentHandTiles[i]->setVisible(!_opponentHandTiles[i]->isVisible());
            }
        }
    });
    for (int i = 0; i < 14; i++){
        std::shared_ptr<TexturedNode> tile = std::dynamic_pointer_cast<TexturedNode>(_assets->get<SceneNode>("matchscene.gameplayscene.opponent-hand-tile.tile-back_" + std::to_string(i)));
        if (tile != nullptr){
            _opponentHandTiles.push_back(tile);
        }
    }
    _opponentHandBtn->activate();
    
    _discardedTileImage = std::dynamic_pointer_cast<scene2::TexturedNode>(_assets->get<scene2::SceneNode>("matchscene.gameplayscene.discarded-tile.discarded-tile-recent.up.discarded-tile-recent"));
    
    _dragToDiscardNode = std::dynamic_pointer_cast<cugl::scene2::TexturedNode>(
        _assets->get<cugl::scene2::SceneNode>(
            "matchscene.gameplayscene.drag-to-discard-tile"
        )
    );
    _dragToDiscardNode->setVisible(false);
    
    _dragToHandNode = std::dynamic_pointer_cast<cugl::scene2::TexturedNode>(
        _assets->get<cugl::scene2::SceneNode>(
            "matchscene.gameplayscene.drag-to-hand-area"
        )
    );
    _dragToHandNode->setVisible(false);
    _discardedTileImage->setVisible(false);
    addChild(_tutorialScene);
    addChild(_discardUINode->_root);
    
    _player1 = std::make_shared<Player>();
    _player2 = std::make_shared<Player>();
    
    _tileSet = std::make_shared<TileSet>();
    _tileSet->initTutorialDeck();
    
    _pile = std::make_shared<Pile>();
    _discardPile = std::make_shared<DiscardPile>();
    _discardPile->init(_assets);
    
    initTileData();
    
    _pile->pileBox = pileRegionNode->getBoundingBox();
    _pile->setTilePositions();
    
    for(auto& tile : _player1->getHand()._tiles) {
        tile->getContainer()->setAnchor(Vec2::ANCHOR_CENTER);
        tile->getContainer()->setScale(tile->_scale);
        tile->getContainer()->setPosition(tile->pos);
    }
    
    // Setting texture location in pile
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
    
    initTurnIndicators();
    
    _dragToDiscardNode = std::dynamic_pointer_cast<cugl::scene2::TexturedNode>(
        _assets->get<cugl::scene2::SceneNode>(
            "matchscene.gameplayscene.drag-to-discard-tile"
        )
    );

    _dragToDiscardNode->setVisible(false);

    _dragToHandNode = std::dynamic_pointer_cast<cugl::scene2::TexturedNode>(
        _assets->get<cugl::scene2::SceneNode>(
            "matchscene.gameplayscene.drag-to-hand-area"
        )
    );
    
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
    
    _playArea = _assets->get<SceneNode>("matchscene.gameplayscene.play-area");
    _playArea->setVisible(false);
    
    _activeRegion = cugl::Rect(activeRegionWorldOrigin, activeRegionNode->getContentSize());
    _discardedTileRegion = cugl::Rect(discardedTileRegionWorldOrigin, discardedTileRegionNode->getContentSize());
    
    // Init the button for playing sets.
    _playSetBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("matchscene.gameplayscene.playSetButton"));
    _playSetBtn->setVisible(false);
    
    _playSetBtn->addListener([this](const std::string& name, bool down) {
        if (!down && _currentTurn == 0) {
            AudioController::getInstance().playSound("Select");
            playSet();
        }
    });
    setActive(false);
    return true;
}

void TutorialScene::update(float timestep) {
    cugl::Vec2 mousePos = Scene::screenToWorldCoords(cugl::Vec3(_input->getPosition()));
    
    // Update tile animations and positions
    _player1->getHand().updateTilePositions(_playerHandRegion, timestep);
    _pile->pileJump(timestep);
    _pile->updateTilePositions(timestep);
    _discardPile->updateTilePositions(timestep);
    updateTurnIndicators();
    
    if (_input->didRelease() && !_input->isDown()){
        cugl::Vec2 initialMousePos =
            cugl::Scene::screenToWorldCoords(cugl::Vec3(_input->getInitialPosition()));
        if (initialMousePos - mousePos == Vec2(0,0)){
            clickedTile(mousePos);
        }
    }
    
    updateDrag(mousePos, _input->isDown(), _input->didRelease(), timestep);
    
    // Check for pile click - add this to handle drawing from pile
    if (_currentTurn == 0 && _input->didRelease()) {
        cugl::Vec2 start = Scene::screenToWorldCoords(cugl::Vec3(_input->getInitialPosition()));
        if (start == mousePos) {  // This checks if it was a click and not a drag
            // Check if player clicked the pile and hasn't drawn yet
            if (_pileBox.contains(mousePos)) {
                drawTile();  // Draw from pile
            }
        }
    }

    // Handle bot turn
    if (_currentTurn == 1) {
        _botDelay -= timestep;
        if (_botDelay <= 0.0f) {
            if (getChoice() == NONE){
                _player2->getHand().drawFromPile(_pile, 1, false);
                AudioController::getInstance().playSound("Pile");
            }
        }
    }
}

void TutorialScene::updateDrag(const cugl::Vec2& mousePos, bool mouseDown, bool mouseReleased, float timestep){
    auto& dragContainer = _player1->getHand().getTiles();
    if (mouseDown) {
        if (!_dragInitiated) {
            _dragStartPos = mousePos;
            _draggingTile = getTileAtPosition(mousePos, dragContainer);
            std::cout << "Drag started for tile: " << (_draggingTile ? "Tile exists" : "No tile found") << std::endl;
            _dragFromDiscard = false;
            if(_draggingTile && !_draggingTile->selectable) {
                return;
            }
            _dragInitiated = true;
            _player1->_draggingTile = _draggingTile;
            if (_draggingTile) {
                _originalTilePos = _draggingTile->pos;
                _dragOffset = _draggingTile->pos - mousePos;
            }
            else if(!_tilesetUIBtn->isDown() && _discardedTileRegion.contains(mousePos)) {
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
                                     _player1->getHand().updateTilePositions(_playerHandRegion, timestep);
                             }
                         }
                         if (oldIndex > 0) {
                             auto leftTile = tiles[oldIndex - 1];
                             if (leftTile && _draggingTile->pos.x < leftTile->pos.x) {
                                 std::swap(tiles[oldIndex], tiles[oldIndex - 1]);
                                _player1->getHand().updateTilePositions(_playerHandRegion, timestep);
                             }
                         }
                     }
                }
            }
        }
    }
    if (mouseReleased) {
        // Active play area logic. Ensure you only do these actions when it is your turn.
        if(_draggingTile && _activeRegion.contains(mousePos) && getChoice() == DISCARDED) {
        
                if(_draggingTile->_suit == TileSet::Tile::Suit::CELESTIAL && !_draggingTile->debuffed) {
                    if (playCelestial(_draggingTile)) {
                        AudioController::getInstance().playSound("WrongAction", false);
                        if (_discardPile->getSize() < 1){
                        } else {
                        }
                    }
                }
                else {
                  // Regular tile getting discarded
                if(discardTile(_draggingTile)) {
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
                  }
                }
            } else {
                AudioController::getInstance().playSound("WrongAction", false);
            }
        }
        if (_dragInitiated && _draggingTile) {
            float distance = (mousePos - _dragStartPos).length();
            if(_draggingTile->discarded) {
                if(_playerHandRegion.contains(mousePos)) {
                    if(drawDiscard()) {
                        _playSetBtn->activate();
                        _playSetBtn->setVisible(true);
                        _draggingTile->_scale = 0.325;
                    }
                    else {
                        _draggingTile->pos = _discardedTileImage->getWorldPosition();
                        _discardedTileImage->setVisible(true);
                        _draggingTile->_scale = 0;
                    }
                }
                else if (getChoice() != DRAWNDISCARD){
                    _discardedTileImage->setVisible(true);
                    _draggingTile->_scale = 0;
                }
            }
            else if (distance > DRAG_THRESHOLD) {
                if (_draggingTile) {
                    if (shouldReturn) {
                        _draggingTile->selected = false;
                        
                        auto& selected = _player1->getHand()._selectedTiles;
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

            if(_draggingTile->discarded || _currentTurn == 1) {
                _player1->_draggingTile = nullptr;
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
        _player1->_draggingTile = nullptr;
        _player1->getHand().updateTilePositions(_playerHandRegion, timestep);

        releaseTile();
     }


void TutorialScene::clickedTile(Vec2 mousePos){
    cugl::Vec2 initialMousePos = cugl::Scene::screenToWorldCoords(cugl::Vec3(_input->getInitialPosition()));
    
    for(const auto& pair : _tileSet->tileMap){
        std::shared_ptr<TileSet::Tile> currTile = pair.second;
        // If you cannot select or deselect the tile return
        if(currTile->tileRect.contains(mousePos) && currTile->tileRect.contains(initialMousePos)){
            if(_currentTurn == 0 && currTile->inHostHand) {
                if(currTile->selectable) {
                    if(currTile->selected) {
                        AudioController::getInstance().playSound("Unclick");
                        auto it = std::find(_player1->getHand()._selectedTiles.begin(), _player1->getHand()._selectedTiles.end(), currTile);
                        if (it != _player1->getHand()._selectedTiles.end()) {
                            _player1->getHand()._selectedTiles.erase(it);
                            currTile->selected = false;
                        }
                    }
                    else {
                        AudioController::getInstance().playSound("Click");
                        _player1->getHand()._selectedTiles.push_back(currTile);
                        currTile->selected = true;
                    }
                }
            }
        }
    }
}

void TutorialScene::dragTile(){
    if (!_draggingTile) return;
    
    cugl::Vec2 screenPos = _input->getPosition();
    cugl::Vec2 mousePos = cugl::Scene::screenToWorldCoords(cugl::Vec3(screenPos));
    
    cugl::Vec2 newPos = mousePos + _dragOffset;
    
    _draggingTile->pos = newPos;
    _draggingTile->tileRect.origin = newPos;
}

void TutorialScene::releaseTile(){
    if (_draggingTile) {
        _draggingTile->pressed = false;
        _draggingTile = nullptr;
    }
}

void TutorialScene::setActive(bool value){
    if (isActive() != value){
        Scene2::setActive(value);
    }
}

void TutorialScene::setTutorialActive(bool value){
    if (value){
        _choice = NONE;
        _tilesetUIBtn->activate();
        _settingBtn->activate();
        _infoBtn->activate();
        updateTurnIndicators();
    } else {
        _backBtn->deactivate();
        _settingBtn->deactivate();
        _infoBtn->deactivate();
        updateTurnIndicators();
    }
}

void TutorialScene::render(){
    _batch->begin(getCamera()->getCombined());
    
    _tutorialScene->render(_batch);
    _pile->draw(_batch);
    _player1->draw(_batch);
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
    _discardUINode->_root->render(_batch);
    if (_draggingTile) {
        if (!_draggingTile->discarded){
            _draggingTile->getContainer()->setVisible(true);
            _draggingTile->getContainer()->render(_batch, Affine2::IDENTITY, Color4::WHITE);
        } else {
            _draggingTile->getContainer()->setVisible(true);
            _discardPile->draw(_batch);
        }
    }
    
    if(_draggingTile && _draggingTile->_suit == TileSet::Tile::Suit::CELESTIAL) {
        if(_input->isDown() && _input->getInitialPosition() != _input->getPosition()) {
            _player1->drawInfo(_draggingTile, _batch, _tutorialScene->getSize());
        }
    }

    _batch->end();
}

bool TutorialScene::drawTile(){
    if (_pile->getVisibleSize() == 0 || getChoice() != NONE) return false;
    
    int currPlayerId = _currentTurn; // 0 = player1 (human), 1 = player2 (bot)
    std::shared_ptr<Player> currPlayer = (currPlayerId == 0) ? _player1 : _player2;

    if (currPlayer->getHand()._tiles.size() > currPlayer->getHand()._tiles.size()) return false;
    
    auto tile = _pile->tilesDrawn(1)[0];
    if (!tile) return false;
    
    tile->inDeck = false;
    tile->inHostHand = (currPlayerId == 0);
    tile->inClientHand =(currPlayerId == 1);
    tile->selectable = (currPlayerId == 0);
    
    currPlayer->getHand()._tiles.push_back(tile);
    if (currPlayerId == 0){
        currPlayer->getHand().updateTilePositions(_playerHandRegion, 0.0f);
    }
    
    AudioController::getInstance().playSound("Pile");
    _choice = DREW;
    return true;
}

bool TutorialScene::drawDiscard(){
    if (!_discardPile->getTopTile()) return false;

    int currPlayerId = _currentTurn;
    std::shared_ptr<Player> currPlayer = (currPlayerId == 0) ? _player1 : _player2;

    if (currPlayer->getHand()._tiles.size() > currPlayer->getHand()._size) return false;

    auto tile = _discardPile->drawTopTile();
    tile->inDeck = false;
    tile->inHostHand = (currPlayerId == 0);
    tile->inClientHand = (currPlayerId == 1);
    tile->selected = (currPlayerId == 0);
    tile->selectable = false;

    currPlayer->getHand()._tiles.push_back(tile);
    if (currPlayerId == 0) {
        currPlayer->getHand()._selectedTiles.push_back(tile);
        currPlayer->getHand().updateTilePositions(_playerHandRegion, 0.0f);
        _discardedTileImage->setVisible(false);
    }

    AudioController::getInstance().playSound("Pile");
    _choice = DRAWNDISCARD;

    return true;
}

bool TutorialScene::discardTile(std::shared_ptr<TileSet::Tile> tile){
    if (!tile) return false;

    int currPlayerId = _currentTurn;
    std::shared_ptr<Player> currPlayer = (currPlayerId == 0) ? _player1 : _player2;

    if (currPlayer->getHand()._tiles.size() <= currPlayer->getHand()._size) return false;

    tile->selected = false;
    tile->inHostHand = false;
    tile->inClientHand = false;
    tile->discarded = true;

    currPlayer->getHand().discard(tile, currPlayerId == 0);
    _discardPile->addTile(tile);

    if (currPlayerId == 0 && tile->_suit != TileSet::Tile::Suit::CELESTIAL) {
        _discardedTileImage->setTexture(_assets->get<Texture>(tile->toString()));
        _discardedTileImage->SceneNode::setContentSize(32.88f, 45);
        _discardedTileImage->setVisible(true);
    }

    _choice = DISCARDED;

    AudioController::getInstance().playSound("Discard");
    endTurn();
    return true;
    }

bool TutorialScene::playCelestial(std::shared_ptr<TileSet::Tile> tile){
    if (!tile) return false;
    
    int currPlayerId = _currentTurn;
    std::shared_ptr<Player> currPlayer = (currPlayerId == 0) ? _player1 : _player2;
    
    if (tile->getSuit() != TileSet::Tile::Suit::CELESTIAL || tile->getRank() != TileSet::Tile::Rank::ROOSTER) return false;
    
    tile->selected = false;
    tile->inHostHand = false;
    tile->inClientHand = false;
    tile->discarded = true;
    currPlayer->getHand().discard(tile, currPlayerId == 0);
    _pile->reshufflePile();
    _pile->setTilePositions();
    
    endTurn();
    
    AudioController::getInstance().playSound("Rooster");
    
    return true;
}

bool TutorialScene::playSet() {
    int currPlayerId = _currentTurn;
    std::shared_ptr<Player> currPlayer = (currPlayerId == 0) ? _player1 : _player2;

    if (currPlayer->getHand().isSetValid(currPlayer->getHand()._selectedTiles)) {
        AudioController::getInstance().playSound("PlayedSet");

        currPlayer->getHand().playSet(currPlayerId == 0);
        currPlayer->getHand()._selectedTiles.clear();

        _choice = NONE;
        return true;
    } else {
        AudioController::getInstance().playSound("WrongAction");

        for (auto it = currPlayer->getHand()._tiles.begin(); it != currPlayer->getHand()._tiles.end();) {
            std::shared_ptr<TileSet::Tile> tile = *it;
            tile->selected = false;

            if (tile->discarded) {
                tile->inHostHand = false;
                tile->inClientHand = false;
                tile->selectable = false;

                tile->_scale = 0;
                tile->pos = Vec2::ZERO;

                _discardPile->addTile(tile);
                currPlayer->getHand().removeTile(tile, currPlayerId == 0);
            } else {
                ++it;
            }
        }

        currPlayer->getHand()._selectedTiles.clear();
        _choice = DONE;
        return false;
    }
}

void TutorialScene::endTurn(){
        if (_currentTurn == 0){
            _currentTurn = 1;
        } else {
            _currentTurn = 0;
            _botDelay = 2.0f;
        }
}

void TutorialScene::initTurnIndicators(){
    _opponentHandRec = _assets->get<SceneNode>("matchscene.gameplayscene.opponent-hand-rec");
    _opponentHandBtn = std::dynamic_pointer_cast<Button>(_assets->get<SceneNode>("matchscene.gameplayscene.opponent-hand"));
    _opponentHandBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            opponentTabVisible = !opponentTabVisible;
        }
    });
    _opponentHandBtn2 = std::dynamic_pointer_cast<Button>(_assets->get<SceneNode>("matchscene.gameplayscene.opponent-hand2"));
    _opponentHandBtn2->addListener([this](const std::string& name, bool down){
        if (!down){
            opponentTabVisible = !opponentTabVisible;
        }
    });
    
    _playerHandRec = _assets->get<SceneNode>("matchscene.gameplayscene.player-hand-rec");
    _playerHandBtn = std::dynamic_pointer_cast<Button>(_assets->get<SceneNode>("matchscene.gameplayscene.playerhand-button"));
    _playerHandBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            AudioController::getInstance().playSound("Select");
            playerTabVisible = !playerTabVisible;
        }
    });
    _playerHandBtn2 = std::dynamic_pointer_cast<Button>(_assets->get<SceneNode>("matchscene.gameplayscene.playerhand-button2"));
    _playerHandBtn2->addListener([this](const std::string& name, bool down){
        if (!down){
            AudioController::getInstance().playSound("Select");
            playerTabVisible = !playerTabVisible;
        }
    });
    
    updateTurnIndicators();
}

void TutorialScene::updateTurnIndicators(){
    if (_currentTurn == 0){
        if (!_opponentHandBtn->isActive()){
            _opponentHandBtn->activate();
        }
        _opponentHandBtn->setVisible(true);
        if (_opponentHandBtn2->isActive()){
            _opponentHandBtn2->deactivate();
        }
        _opponentHandBtn2->setVisible(false);
        if (_playerHandBtn->isActive()){
            _playerHandBtn->deactivate();
        }
        _playerHandBtn->setVisible(false);
        if (!_playerHandBtn2->isActive()){
            _playerHandBtn2->activate();
        }
        _playerHandBtn2->setVisible(true);
    } else {
        if (_opponentHandBtn->isActive()){
            _opponentHandBtn->deactivate();
        }
        _opponentHandBtn->setVisible(false);
        if (!_opponentHandBtn2->isActive()){
            _opponentHandBtn2->activate();
        }
        _opponentHandBtn2->setVisible(true);
        if (!_playerHandBtn->isActive()){
            _playerHandBtn->activate();
        }
        _playerHandBtn->setVisible(true);
        if (_playerHandBtn2->isActive()){
            _playerHandBtn2->deactivate();
        }
        _playerHandBtn2->setVisible(false);
    }
    
    _opponentHandRec->setVisible(opponentTabVisible);
    for (int i = 0; i < _opponentHandTiles.size(); i++){
        _opponentHandTiles[i]->setVisible(opponentTabVisible);
    }
    
    _playerHandRec->setVisible(playerTabVisible);
    for (int i = 0; i < _playerHandTiles.size(); i++){
        _playerHandTiles[i]->setVisible(playerTabVisible);
    }
}

void TutorialScene::dispose(){
    if (_active){
        removeAllChildren();
        _active = false;
    }
}
