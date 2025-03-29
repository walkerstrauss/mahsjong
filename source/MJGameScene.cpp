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
    std::vector<std::shared_ptr<TileSet::Tile>> emptyDiscarded(2);
    discardedTiles = emptyDiscarded;
    
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
//
//    _matchScene->setContentSize(dimen);
//    _matchScene->doLayout();
//    _matchScene->setPosition(125, _matchScene->getPositionY());
   
    std::shared_ptr<scene2::SceneNode> childNode = _matchScene->getChild(0);
    
    _discardBtn = std::dynamic_pointer_cast<scene2::Button>(childNode->getChild(3));
    _tilesetUIBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("matchscene.gameplayscene.discardButton"));
    _pauseBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("matchscene.gameplayscene.pauseButton"));
    _endTurnBtn = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("matchscene.gameplayscene.endTurnButton"));
    _endTurnBtn->addListener([this](const std::string& name, bool down){
        if (!down && _network->getCurrentTurn() == _network->getLocalPid()){
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
    discardArea = cugl::Rect(Vec2(1000, 210), Size(273, 195));

    if(_network->getHostStatus()){
        //Setting up whole deck
        _tileSet->initHostDeck();
        _tileSet->setAllTileTexture(assets);
        _tileSet->shuffle();
        //Initializes the deck (ordered representation)
        _tileSet->addActionAndCommandTiles(assets);
        _network->initGame(_tileSet->toJson(_tileSet->deck));
        //Setting up hand
        _player->getHand().initHand(_tileSet, _network->getHostStatus());
        //Updating tile positions in hand
        _player->getHand().updateTilePositions(getSize());
        _tileSet->shuffle();
        //Creating pile as Host
        _pile->initPile(4, _tileSet, _network->getHostStatus());
        //Broadcasting all tiles with attributes (sets pile tiles as !inDeck)
        _network->broadcastStartingDeck(_tileSet->mapToJson());
    } else {
        _tileSet->initClientDeck(_network->getStartingDeck());
        //Initializing client pile (pile full of nullptrs)
        _pile->initPile(4, _tileSet, _network->getHostStatus());
        //Initializing client deck
        _tileSet->setAllTileTexture(_assets);
        _tileSet->setSpecialTextures(_assets);
        //Initializing client pile (pile full of nullptrs)
        _pile->initPile(4, _tileSet, _network->getHostStatus());
        _tileSet->updateDeck(_network->getDeckJson());
        _player->getHand().initHand(_tileSet, _network->getHostStatus());
        _player->getHand().updateTilePositions(getSize());
        _pile->remakePile();
        _pile->updateTilePositions();
        _network->broadcastDeck(_tileSet->mapToJson());
    }
    
    // Initialize the discard pile
    _discardPile = std::make_shared<DiscardPile>();
    _discardPile->init(_assets);
  
//    TileSet testDeck;
//    std::shared_ptr<cugl::JsonValue> constants = _assets->get<JsonValue>("constants");
//    std::shared_ptr<cugl::JsonValue> alphaDeck = constants->get("alpha_deck");
//    testDeck.deck = testDeck.processDeckJson(alphaDeck);
//    testDeck.printDeck();

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
//        CULog("%d", _discardPile->getTopTile()->played);
    }
//    for(auto const& tile : _player->getHand()._tiles){
//        CULog("%s, %s", tile->toString().c_str(), tile->pos.toString().c_str());
//    }
    //Reading input
    _input.readInput();
    _input.update();
    

    cugl::Vec2 mousePos = cugl::Scene::screenToWorldCoords(cugl::Vec3(_input.getPosition()));
    // Determine if the mouse is held down or was just released.
    bool isMouseDown = _input.isDown();
    bool isMouseReleased = _input.didRelease();
    
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
                discardedTiles.emplace_back(_tileSet->tileMap[rank + " of " + suit + " " + id]);
                _choice = DISCARDED;
            }
        }
        _network->setStatus(NetworkController::Status::INGAME);
    }
    
    std::shared_ptr<TileSet::Tile> lastTile = nullptr;
    if (!_player->getHand()._drawnPile.empty()) {
        lastTile = _player->getHand()._drawnPile.back();
    }
    
    if(_network->getStatus() == NetworkController::Status::LAYER) {
        _tileSet->updateDeck(_network->getTileMapJson());
        _pile->remakePile();
        _pile->updateTilePositions();
        _network->setStatus(NetworkController::Status::INGAME);
    }
    
    if(_network->getStatus() == NetworkController::Status::PREEMPTIVEDISCARD) {
        if(std::get<1>(_network->getNumDiscard()) != _network->getHostStatus()) {
            _pile->removeNumTiles(std::get<0>(_network->getNumDiscard()));
        }
        _network->setStatus(NetworkController::Status::INGAME);
    }

    _player->getHand().updateTilePositions(_matchScene->getSize());
    
    if(_input.didRelease() && !_input.isDown()){
        cugl::Vec2 prev = _input.getPosition(); //Get our mouse position
        cugl::Vec2 mousePos = cugl::Scene::screenToWorldCoords(cugl::Vec3(prev));
        if (_network->getCurrentTurn() == _network->getLocalPid()) {
            clickedTile(mousePos);
        }
    }
    
    updateDrag(mousePos, isMouseDown, isMouseReleased);
    
    if (_network->getCurrentTurn() == _network->getLocalPid()) {
        cugl::Vec2 mousePos = cugl::Scene::screenToWorldCoords(cugl::Vec3(_input.getPosition()));
        cugl::Vec2 initialMousePos = cugl::Scene::screenToWorldCoords(cugl::Vec3(_input.getInitialPosition()));
        bool tappedPile = _input.didRelease() && _pileBox.contains(mousePos);
        bool initialClickInPile = _pileBox.contains(initialMousePos);
        
        //Start turn by drawing tile to hand
        if((_input.getKeyPressed() == KeyCode::D && _input.getKeyDown() ) || (tappedPile && initialClickInPile)){
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
            
            if (!_player->getHand()._drawnPile.empty()) {
                auto drawnTile = _player->getHand()._drawnPile.back();
                CULog("drawn tile: %s", drawnTile->toString().c_str());
                if (drawnTile->getSuit() == TileSet::Tile::Suit::SPECIAL &&
                    drawnTile->getRank() == TileSet::Tile::Rank::COMMAND) {
                    auto cmdTile = std::dynamic_pointer_cast<TileSet::CommandTile>(drawnTile);
                    if (cmdTile) {
                        _player->getHand().discard(cmdTile, _network->getHostStatus());
                        applyCommand(cmdTile);
                    }
                }
            }
            
            _player->canDraw = false;
        }
            
        for (auto& tile : _player->getHand()._drawnPile) {
            tile->setTexture(_assets->get<Texture>(tile->toString()));
        }
        
        if (_player->getHand().isWinningHand()){
            _gameWin = true;
        }
        
        // ACTION TILE REGION
        std::shared_ptr<SceneNode> actionTileSection = _assets->get<scene2::SceneNode>("matchscene.gameplayscene.actionSection.up.actionTileSection");
        cugl::Rect actionTileSectionRect = actionTileSection->getBoundingBox();
        
        cugl::Vec2 worldOrigin = actionTileSection->nodeToWorldCoords(Vec2::ZERO);
        cugl::Rect worldRect(worldOrigin, actionTileSection->getContentSize());
        
        // if the screen is tapped and there is a selected tile
        if(_input.didRelease() && !_player->getHand()._selectedTiles.empty()){
            
            // for all selected tiles (must be one)
            for(auto const& tile : _player->getHand()._selectedTiles){
                
                CULog("Tile rect: (%f, %f, %f, %f), Discard area: (%f, %f, %f, %f)",
                      tile->tileRect.origin.x, tile->tileRect.origin.y,
                      tile->tileRect.size.width, tile->tileRect.size.height,
                      actionTileSectionRect.origin.x, actionTileSectionRect.origin.y,
                      actionTileSectionRect.size.width, actionTileSectionRect.size.height);

                
                // if this tile is in the discard area
                if(worldRect.contains(mousePos)){ // tile->tileRect)
                    
                    if(tile->getRank() != TileSet::Tile::Rank::ACTION){
                        CULog("You must use an action tile here");
                        continue;
                    }
                    
                    // activate the action tile
                    std::shared_ptr<TileSet::ActionTile> actionTile =
                        std::static_pointer_cast<TileSet::ActionTile>(
                            std::const_pointer_cast<TileSet::Tile>(tile));
                    applyAction(actionTile);
 
                    
                    // discard the action tile
                    _player->getHand().discard(tile, _network->getHostStatus());
                    _player->getHand()._selectedTiles.clear();
                    CULog("The tile was discarded");

                }
            }
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
                _player->canExchange = false;
                
                discardDrawTile = _discardPile->getTopTile();
                _discardPile->removeTopTile();
                _choice = DRAW_DISCARD;
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
        if (_input.getKeyPressed() == KeyCode::P && _input.getKeyDown()){
            if (_player->getHand()._selectedTiles.size() == 1) {
                std::shared_ptr<TileSet::Tile> selected = _player->getHand()._selectedTiles[0];
                if (selected->getSuit() == TileSet::Tile::Suit::SPECIAL &&
                    selected->getRank() == TileSet::Tile::Rank::ACTION) {
                    auto action = std::dynamic_pointer_cast<TileSet::ActionTile>(selected);
                    CULog("action tile: %s", action->toString().c_str());
                    applyAction(action);
                }
            }
        }
//        if (_input.getKeyPressed() == KeyCode::P && _input.getKeyDown()){
//            _pile->reshufflePile();
//            _network->broadcastDeckMap(_tileSet->mapToJson());
//            _network->broadcastPileLayer();
//        }
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
        if ((_input.getKeyPressed() == KeyCode::N && _input.getKeyDown())) {
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
        _discardBtn->activate();
        _tilesetUIBtn->activate();
        _endTurnBtn->activate();
    } else {
        _pauseBtn->deactivate();
        _discardBtn->deactivate();
        _tilesetUIBtn->deactivate();
        _endTurnBtn->deactivate();
    }
}

void GameScene::applyAction(std::shared_ptr<TileSet::ActionTile> actionTile) {
    _player->getHand().discard(actionTile, _network->getHostStatus());
    switch (actionTile->type) {
        case TileSet::ActionTile::ActionType::CHAOS:
            CULog("CHAOS: Reshuffling the pile...");
            _pile->reshufflePile();
            _network->broadcastDeckMap(_tileSet->mapToJson());
            _network->broadcastPileLayer();
            break;
        case TileSet::ActionTile::ActionType::ECHO:
            CULog("ECHO: Draw two tiles...");
            _player->getHand().drawFromPile(_pile, 2, _network->getHostStatus());
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
            break;
        case TileSet::ActionTile::ActionType::ORACLE:
            CULog("ORACLE: Draw any tile from pile...");
            
        default:
            break;
    }
    
}

void GameScene::applyCommand(std::shared_ptr<TileSet::CommandTile> commandTile) {
    switch (commandTile->type) {
        case TileSet::CommandTile::CommandType::OBLIVION:
            //            CULog("OBLIVION: Removing all action tiles from hand...");
            _player->getHand().loseActions(_network->getHostStatus());
            
            if(_player->getHand()._tiles.size() < _player->getHand()._size){
                int numTilesToDraw = _player->getHand()._size - static_cast<int>(_player->getHand()._tiles.size());
                if(_pile->getVisibleSize() <= numTilesToDraw){
                    int remainder = numTilesToDraw - _pile->getVisibleSize();
                    _network->broadcastPreDraw(_pile->getVisibleSize(), _network->getHostStatus());
                    _player->getHand().drawFromPile(_pile, _pile->getVisibleSize(), _network->getHostStatus());
                    
                    _pile->createPile();
                    _player->getHand().drawFromPile(_pile, remainder, _network->getHostStatus());
                }
                else{
                    _network->broadcastPreDraw(numTilesToDraw, _network->getHostStatus());
                    _player->getHand().drawFromPile(_pile, numTilesToDraw, _network->getHostStatus());
                }
                _tileSet->clearTilesToJson();
                _network->broadcastDeckMap(_tileSet->mapToJson());
                _network->broadcastDeck(_tileSet->toJson(_tileSet->deck));
                _network->broadcastPileLayer();
            }
            
            break;
        case TileSet::CommandTile::CommandType::DISCARD:
            //            CULog("VOID: Discarding random tile from hand...");
            if (_player->getHand()._tiles.size()) {
                cugl::Random rd;
                rd.init();
                int index = static_cast<int>(rd.getOpenUint64(0, _player->getHand()._size - 1));
                std::shared_ptr<TileSet::Tile> random = _player->getHand()._tiles[index];                
                _player->forcedDiscard = true;
                _player->getHand()._selectedTiles.clear();
                discardTile(random);
                _player->forcedDiscard = false;
                
                if(_player->getHand()._tiles.size() < _player->getHand()._size){
                    int numTilesToDraw = _player->getHand()._size - static_cast<int>(_player->getHand()._tiles.size());
                    if(_pile->getVisibleSize() <= numTilesToDraw){
                        int remainder = numTilesToDraw - _pile->getVisibleSize();
                        _network->broadcastPreDraw(_pile->getVisibleSize(), _network->getHostStatus());
                        _player->getHand().drawFromPile(_pile, _pile->getVisibleSize(), _network->getHostStatus());
                        
                        _pile->createPile();
                        _player->getHand().drawFromPile(_pile, remainder, _network->getHostStatus());
                    }
                    else{
                        _network->broadcastPreDraw(numTilesToDraw, _network->getHostStatus());
                        _player->getHand().drawFromPile(_pile, numTilesToDraw, _network->getHostStatus());
                    }
                    _tileSet->clearTilesToJson();
                    _network->broadcastDeckMap(_tileSet->mapToJson());
                    _network->broadcastDeck(_tileSet->toJson(_tileSet->deck));
                    _network->broadcastPileLayer();
                }
            }
            break;
    }
    _network->endTurn();
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





void GameScene::pressTile(){
    cugl::Vec2 screenPos = _input.getPosition();
    cugl::Vec2 mousePos = cugl::Scene::screenToWorldCoords(cugl::Vec3(screenPos));
    
    // if the player tapped on a tile in the hand.
    for (auto & tile : _player->getHand()._tiles) {
        if (tile->tileRect.contains(mousePos)) {
            // select this tile
            CULog("selected a tile");
            tile->selected = !tile->selected;
            _dragOffset = _draggingTile->pos - mousePos;
            _draggingTile->pressed = true;
            _draggingTile = tile;
            
            auto& selected = _player->getHand()._selectedTiles;
            auto it = std::find(selected.begin(), selected.end(), tile);
            if (tile->selected) {
                if (it == selected.end()) selected.push_back(tile);
            } else {
                if (it != selected.end()) selected.erase(it);
            }
            
        }
    }
    
    
    // if the player pressed on the pile
    if (_pileBox.contains(mousePos)) {
        
        if(_player->getHand()._tiles.size() > _player->getHand()._size){
            CULog("Hand too big");
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
    for (auto& row : _pile->_pile) {
        for (auto& tile : row) {
            if (tile && tile->tileRect.contains(mousePos)) {
                CULog("Selected a tile in pile");
                
                tile->selected = !tile->selected;
                _draggingTile = tile;
                _dragOffset = tile->pos - mousePos;
                tile->pressed = true;
                
                auto& selected = _player->getHand()._selectedTiles;
                auto it = std::find(selected.begin(), selected.end(), tile);
                if (tile->selected) {
                    if (it == selected.end()) selected.push_back(tile);
                } else {
                    if (it != selected.end()) selected.erase(it);
                }
                
            
            }
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
//            _player->getHand().updateTilePositions(_matchScene->getSize());
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
        if (_dragInitiated && _draggingTile) {
            if(discardArea.contains(_draggingTile->pos)){
                discardTile(_draggingTile);
            }
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
        if (!(tile->_rank == TileSet::Tile::Rank::ACTION || tile->_rank == TileSet::Tile::Rank::COMMAND)){
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
