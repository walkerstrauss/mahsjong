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

// Lock the screen size to a fixed height regardless of aspect ratio
// PLEASE ADJUST AS SEEN FIT
#define SCENE_HEIGHT 720 // Change to 874 for resizing from iPhone 16 Pro aspect ratio
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
bool GameScene::init(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<NetworkController>& network, std::shared_ptr<InputController>& inputController) {
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

    //Initializing match scene
    _matchScene = _assets->get<scene2::SceneNode>("matchscene");
    _matchScene->setContentSize(getSize());
    _matchScene->getChild(0)->setContentSize(_matchScene->getContentSize());
    _matchScene->doLayout();
    
    cugl::Size screenSize = cugl::Application::get()->getDisplaySize();
    screenSize *= _matchScene->getContentSize().height/screenSize.height;
    float offset = (screenSize.width -_matchScene->getWidth())/2;
    
    _matchScene->setPosition(offset, _matchScene->getPosition().y);
    
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
    _discardUINode->doLayout();

    _discardUINode->_root->setPosition(offset, _discardUINode->getPosition().y);
    
    // Initializing scene nodes
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
    _tilesetUIBtnKey = _tilesetUIBtn->addListener([this](const std::string& name, bool down){
        if (!down){
            setActive(false);
            setGameActive(false);
            _backBtn->activate();
            _discardUINode->_root->setVisible(true);
            AnimationController::getInstance().pause();
            AudioController::getInstance().playSound("Select");
        }
    });
    
    // Initializing back button
    _backBtn = std::dynamic_pointer_cast<scene2::Button>(
        _discardUINode->_root->getChildByName("tilesetscene")->getChildByName("board")->getChildByName("buttonClose"));
    _backBtnKey = _backBtn->addListener([this](const std::string& name, bool down) {
        if (!down) {
            setActive(true);
            setGameActive(true);
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
    
    
    // Init the button for playing sets.
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
        
    addChild(_matchScene);
    addChild(_discardUINode->_root);
    addChild(_pileUINode->_root);
    
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
    
    //Initialization of shared objects
    _tileSet = _matchController->getTileSet();
    _pile = _matchController->getPile();
    _discardPile = _matchController->getDiscardPile();
    
    // Premature repositioning so it tiles don't render in the corner of the screen
    _player->getHand().updateTilePositions(_playerHandRegion, 0);
    
    _pile->pileBox = pileRegionNode->getBoundingBox();
    _pile->setTilePositions(false);

    // Setting texture location in hand
    for(auto& tile : _player->getHand()._tiles) {
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
    
    _input = inputController; //Initialize the input controller
    
    _quit = false;
    setActive(false);
    _matchScene->setVisible(true);
    _discardedTileImage->setVisible(false);
    
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

    //AudioController::getInstance().playMusic("match-music");
    initTurnIndicators();
    
    for (int i = 0; i < 14; i++){
        std::shared_ptr<TexturedNode> tile = std::dynamic_pointer_cast<TexturedNode>(_assets->get<SceneNode>("matchscene.gameplayscene.opponent-hand-tile.tile-back_" + std::to_string(i)));
        if (tile){
            _opponentHandTiles.push_back(tile);
            tile->setVisible(false);
        }
    }
    for (int i = 0; i < 14; i++){
        std::shared_ptr<TexturedNode> tile = std::dynamic_pointer_cast<TexturedNode>(_assets->get<SceneNode>("matchscene.gameplayscene.player-hand-tile.tile-back_" + std::to_string(i)));
        if (tile){
            _playerHandTiles.push_back(tile);
            tile->setVisible(false);
        }
    }
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
    
    _playSetBtn->setVisible(false);
    
    _playSetBtn->addListener([this](const std::string& name, bool down) {
        if (!down) {
            AudioController::getInstance().playSound("Select");
            if(!_matchController->playSet()) {
                _discardedTileImage->setVisible(true);
            };
        }
    });

    _turnSheet = SpriteNode::allocWithSheet(_assets->get<Texture>("turn-sheet"), 2, 3, 3);
    _turnSheet->setAnchor(Vec2::ANCHOR_CENTER);
    _turnSheet->setPosition(1085,screenSize.height/2);
    _turnSheet->setScale(0.12);
    _turnSheet->setFrame(0);
    _turnSheet->setVisible(true);
    
    _remainingLabel = std::dynamic_pointer_cast<Label>(_assets->get<SceneNode>("matchscene.gameplayscene.tile-left.tile-left-number"));
    _remainingTiles = _tileSet->deck.size();
    _remainingLabel->setText(std::to_string(_remainingTiles));
    
    _timer = std::dynamic_pointer_cast<Label>(_assets->get<SceneNode>("matchscene.gameplayscene.timer"));
    _timer->setText("00:30");
    initPlayerGuide();
    updateTurnIndicators();
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
    init(_assets, _network, _input);
    return;
}

/**
 * The method to update the game mode
 *
 * @param timestep The amount of time (in seconds) since the last frame
 */
void GameScene::update(float timestep) {
    if (_input->getKeyPressed() == cugl::KeyCode::A && _input->getPrevKeyPressed() != cugl::KeyCode::A) {

        // there are multiple _choice. One is from MatchController, and the other is from GameScene.
        _choice = Choice::WIN;

        _matchController->setChoice(MatchController::Choice::WIN);

        _network->broadcastEnd(_network->getLocalPid());
    }
    _matchController->update(timestep);
    
    // Fetching current mouse position
    cugl::Vec2 mousePos = cugl::Scene::screenToWorldCoords(cugl::Vec3(_input->getPosition()));
    
    // Constantly updating the position of tiles in hand
    _player->getHand().updateTilePositions(_playerHandRegion, timestep);
    if(!(_pile->choice == Pile::Choice::SHUFFLE)) {
        _pile->pileJump(timestep);
    }
    //Constantly update pile tile positions
    _pile->updateTilePositions(timestep);
    // Constantly update discard pile tile
    _discardPile->updateTilePositions(timestep);
    
    // Constantly updating turn indicators based on player turn
    updateTurnIndicators();
    displayPlayerSets();
    displayOpponentSets();
    updatePlayerGuide();
    
    if (_matchController->getChoice() == MatchController::WIN){
        _choice = WIN;
    }
    
    if (_matchController->getChoice() == MatchController::LOSE){
        _choice = LOSE;
    }
    
    // Update timer display based on remaining time in turn
//    updateTurnTimer(timestep);
    
    // Updating discardUINode if matchController has a discard update
    if(_matchController->getChoice() == MatchController::Choice::DISCARDUIUPDATE) {
        _discardUINode->updateLabels(_discardPile->getTiles());
        
        if(_discardPile->getTopTile()) {
            if (_discardPile->getTopTile()->debuffed) {
                _discardedTileImage->setTexture(_assets->get<Texture>("debuffed"));
            } else {
                _discardedTileImage->setTexture(_assets->get<Texture>(_discardPile->getTopTile()->toString()));
            }
        }
        _discardedTileImage->SceneNode::setContentSize(32.88, 45);
        _discardedTileImage->setVisible(true);
        
        _matchController->setChoice(MatchController::NONE);
    }
    
    if(_matchController->getChoice() == MatchController::PILEDRAW){
        _remainingTiles--;
        _remainingLabel->setText(std::to_string(_remainingTiles));
        
        _matchController->setChoice(MatchController::NONE);
    }
    
    // If matchController state is SUCCESS_SET, deactivate button
    if(_matchController->getChoice() == MatchController::SUCCESS_SET) {
        //AudioController::getInstance().playSound("PlayedSet");
        playerTabVisible = true;
        opponentTabVisible = true;
        _playSetBtn->setVisible(false);
        _playSetBtn->deactivate();
        _matchController->setChoice(MatchController::Choice::NONE);
    }
    
    // If matchController state is FAILED_SET, deactivate button and make discarded tile visible
    if(_matchController->getChoice() == MatchController::FAILED_SET) {
        //AudioController::getInstance().playSound("WrongAction");
        _discardedTileImage->setVisible(true);
        _playSetBtn->setVisible(false);
        _playSetBtn->deactivate();
        _matchController->setChoice(MatchController::Choice::NONE);
    }
    
    // UI Handling for Celestial Tiles
    if (_matchController->getChoice() == MatchController::Choice::MONKEYTILE) {
        AnimationController::getInstance().tryAddFade(_tradeArea, true, 0.2f, _wasTradeTileVisible);
    }
    
    if (_matchController->getChoice() == MatchController::Choice::RATTILE
        && _pileUINode->getState() == PileUINode::NONE) {
        _pileUINode->setState(PileUINode::RATSELECT);
    }
    
    if (_matchController->getChoice() == MatchController::Choice::PIGTILE) {
        _discardUINode->_root->setVisible(true);
        _backBtn->setVisible(false);
    }
    
    if (_matchController->getChoice() == MatchController::Choice::DRAGONTILE) {
        if (_pileUINode->getState() == PileUINode::NONE) {
            _pileUINode->setState(PileUINode::DRAGONROW);
        } else if (_pileUINode->getState() == PileUINode::FINISH) {
            _matchController->playDragon();
            _matchController->setChoice(MatchController::Choice::NONE);
            _pileUINode->setState(PileUINode::NONE);
        }
    }
        
    // If we are in drawn discard state, set discarded tile image visibility to false since player drew it
    if(_matchController->getChoice() == MatchController::DRAWNDISCARD || _network->getStatus() == NetworkController::DRAWNDISCARD) {
        _discardedTileImage->setVisible(false);
    }
    
    // If play set button is active and visible and match controller state is NONE, deactivate and set visible to false
    if((_playSetBtn->isVisible() || _playSetBtn->isActive()) && _matchController->getChoice() == MatchController::NONE) {
        _playSetBtn->deactivate();
        _playSetBtn->setVisible(false);
    }
    
    // Clicking/Tapping and Dragging logic
    if(_input->didRelease() && !_input->isDown()) {
        cugl::Vec2 initialMousePos = cugl::Scene::screenToWorldCoords(cugl::Vec3(_input->getInitialPosition()));
        if(-TAP_ACTIVE_LENGTH <= (initialMousePos - mousePos).length() <= TAP_ACTIVE_LENGTH) {
            if (_matchController->getChoice() == MatchController::Choice::PIGTILE) {
                int tileIndex = _discardUINode->getClickedTile(mousePos);
                if (tileIndex != -1) {
                    _matchController->playPig(_discardUINode->tileFromIndex(tileIndex));
                    _discardUINode->_root->setVisible(false);
                    _backBtn->setVisible(true);
                    _discardUINode->decrementLabel(tileIndex);
                    _matchController->setChoice(MatchController::Choice::NONE);
                }
            } else {
                clickedTile(mousePos);
            }
        }
    }
    updateDrag(mousePos, _input->isDown(), _input->didRelease(), timestep);
    
    // If scene is not active prevent any input from user that changes the state of the game
    if(!isActive()) {
        return;
    }
    
    // If it is your turn, allow turn-based actions
    if(_network->getCurrentTurn() == _network->getLocalPid()) {
        // Coords of initial click and ending release
        cugl::Vec2 initialMousePos = cugl::Scene::screenToWorldCoords(cugl::Vec3(_input->getInitialPosition()));
        
        bool releasedInPile = _input->didRelease() && _pileBox.contains(mousePos);
        // Drawing (from pile) logic
        
        if(_pileBox.contains(initialMousePos) && releasedInPile &&  _matchController->getChoice() != MatchController::Choice::RATTILE && _matchController->getChoice() != MatchController::MONKEYTILE) {
            //            if(_pileBox.contains(initialMousePos) && releasedInPile) {
            if (_matchController->hasDrawn){
                if (_matchController->hasPlayedCelestial){
                     AudioController::getInstance().playSound("WrongAction", false);
                     showPlayerGuide("discard-to-end");
                } else if (_matchController->getChoice() != MatchController::DRAGONTILE && _matchController->getChoice() != MatchController::PIGTILE){
                     AudioController::getInstance().playSound("WrongAction", false);
                     showPlayerGuide("discard-or-play-to-end");
                }
            }
            
            _matchController->drawTile();
        }
        
        updateSpriteNodes(timestep);
    } else {
        _tradeArea->setVisible(false);
        _wasTradeTileVisible = false;
        _dragToDiscardNode->setVisible(false);
        _wasDragToDiscardVisible = false;
        _dragToHandNode->setVisible(false);
        _wasDragToHandVisible = false;
        _playArea->setVisible(false);
        _wasPlayAreaVisible = false;
    }
    
    // Fade areas in or out depending on input
    updateAreaVisibility(mousePos, timestep);
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
    
    _pileUINode->_root->render(_batch);
    _pile->draw(_batch);
    _player->draw(_batch);

    for (auto key : playerGuideKeys){
        auto node = playerGuideNodeMap[key];
        if (framesOnScreen > 0 && framesOnScreen < maxFramesOnScreen && node->isVisible()){
            node->render(_batch);
        }
    }
    
    if (_dragToDiscardNode && _dragToDiscardNode->isVisible()) {
        _dragToDiscardNode->render(_batch);
    } else if (_playArea && _playArea->isVisible()){
        _playArea->render(_batch);
    } else if (_tradeArea && _tradeArea->isVisible()){
        _tradeArea->render(_batch);
    }
    if (_dragToHandNode && _dragToHandNode->isVisible()) {
        _dragToHandNode->render(_batch);
    }

    _discardUINode->_root->render(_batch);
    
    _batch->setColor(Color4(255, 0, 0, 200));
    _batch->setTexture(nullptr);


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
            _player->drawInfo(_draggingTile, _batch, _matchScene->getSize());
        }
    }

    _batch->end();
}

void GameScene::setActive(bool value){
    if (isActive() != value){
        Scene2::setActive(value);
    }
}
    
void GameScene::setGameActive(bool value){
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

void GameScene::clickedTile(cugl::Vec2 mousePos){
    cugl::Vec2 initialMousePos = cugl::Scene::screenToWorldCoords(cugl::Vec3(_input->getInitialPosition()));
    
    for(const auto& pair : _tileSet->tileMap){
        std::shared_ptr<TileSet::Tile> currTile = pair.second;
        // If you cannot select or deselect the tile return
        if(currTile->tileRect.contains(mousePos) && currTile->tileRect.contains(initialMousePos)){
            if((_network->getHostStatus() && currTile->inHostHand) || (!_network->getHostStatus() && currTile->inClientHand)) {
                if(currTile->selectable) {
                    if(currTile->selected) {
                        AudioController::getInstance().playSound("Unclick");
//                        AnimationController::getInstance().animateTileDeselect(currTile, 30);
                        auto it = std::find(_player->getHand()._selectedTiles.begin(), _player->getHand()._selectedTiles.end(), currTile);
                        if (it != _player->getHand()._selectedTiles.end()) {
                            _player->getHand()._selectedTiles.erase(it);
                            currTile->selected = false;
                        }
                    }
                    else {
                        AudioController::getInstance().playSound("Click");
//                        AnimationController::getInstance().animateTileSelect(currTile, 30);
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

void GameScene::dragTile(){
    if (!_draggingTile) return;
    
    cugl::Vec2 screenPos = _input->getPosition();
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
        
void GameScene::updateDrag(const cugl::Vec2& mousePos, bool mouseDown, bool mouseReleased, float timestep) {
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
                        showPlayerGuide("drew-try-play");
                    } else {
                        if (_matchController->getChoice() != MatchController::PIGTILE && _matchController->getChoice() != MatchController::RATTILE) {
                            AudioController::getInstance().playSound("WrongAction", false);
                            showPlayerGuide("drew-try-discard");
                        }
                    }
                } else {
                    if(_draggingTile->_suit == TileSet::Tile::Suit::CELESTIAL && !_draggingTile->debuffed) {
                        if (!_matchController->playCelestial(_draggingTile) && _matchController->getChoice() != MatchController::DRAGONTILE) {
                            AudioController::getInstance().playSound("WrongAction", false);
                            if (_discardPile->getSize() < 1){
                                showPlayerGuide("pig-fail");
                            } else {
                                showPlayerGuide("must-draw-play");
                            }
                        }
                  }
                  else {
                      // Monkey tile was played, regular tile chosen to trade
                      if (_matchController->getChoice() == MatchController::Choice::MONKEYTILE) {
                          
                          _matchController->playMonkey(_draggingTile);
                          // Play the swap sound when the monkey tile is activated.
                          //AudioController::getInstance().playSound("swap");
                          
                          AnimationController::getInstance().tryAddFade(_tradeArea, false, 0.2f, _wasTradeTileVisible);
                          // Rebind _player to prevent null ptr error
                          _player = _network->getHostStatus() ? _matchController->hostPlayer : _matchController->clientPlayer;
                          _matchController->setChoice(MatchController::Choice::NONE);
                      }
                      // Regular tile getting discarded
                      else if(_matchController->discardTile(_draggingTile)) {
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
                      } else if (_matchController->getChoice() != MatchController::DRAGONTILE){
                          AudioController::getInstance().playSound("WrongAction", false);
                          showPlayerGuide("must-draw-discard");;
                      }
                    }
                }
            } else {
                AudioController::getInstance().playSound("WrongAction", false);
                showPlayerGuide("not-your-turn");
            }
        }
        if (_dragInitiated && _draggingTile) {
            float distance = (mousePos - _dragStartPos).length();
            if(_draggingTile->discarded) {
                if(_playerHandRegion.contains(mousePos)) {
                    if(_matchController->drawDiscard()) {
                        _playSetBtn->activate();
                        _playSetBtn->setVisible(true);
                        _draggingTile->_scale = 0.325;
                        _matchController->hasDrawn = true;
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

void GameScene::updateAreaVisibility(Vec2 mousePos, float timestep){
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

void GameScene::endTurnFromTimeout(){
    if (_network->getCurrentTurn() != _network->getLocalPid()) return;
    
    _matchController->hasTimedOut = true;
    
    const auto& hand = _player->getHand();
    if (hand._tiles.size() > hand._size) {
        if (!hand._tiles.empty()){
            auto discardTile = hand._tiles.back();
            if (_matchController->discardTile(discardTile)){
                _discardedTileImage->setTexture(_assets->get<Texture>(discardTile->toString()));
                _discardedTileImage->setContentSize(32.88, 45);
                _discardedTileImage->setVisible(true);
                int index = _discardUINode->getLabelIndex(discardTile);
                _discardUINode->incrementLabel(index);
            } else {
                CULog("hand already at required size for ending turn");
            }
        }
    }
    
    _matchController->endTurn();
}
