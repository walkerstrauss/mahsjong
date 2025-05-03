//
//  MJGameScene.h
//  Mahsjong
//
//  Created by Patrick Choo on 2/18/25.
//

#ifndef __MJ_GAME_SCENE_H__
#define __MJ_GAME_SCENE_H__

#include <cugl/cugl.h>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <string>
#include "MJInputController.h"
#include "MJTileSet.h"
#include "MJPlayer.h"
#include "MJPile.h"
#include "MJDiscardPile.h"
#include "MJDiscardUINode.h"
#include "MJPileUINode.h"
#include "MJNetworkController.h"
#include "MJAudioController.h"
#include "MJAnimationController.h"
#include "MJMatchController.h"
#include "MJAnimatedNode.h"


using namespace cugl;
using namespace cugl::scene2;
using namespace cugl::netcode;
using namespace std;

/**
 * This class is the primary gameplay controller for our game
 *
 * This world should contain all objects, assets, and input controller. Please start
 * including any and all objects and classes that will build our game */
class GameScene: public cugl::scene2::Scene2{
public:
    /**
     * Enum representing the player's choice when in the
     * game scene for app transitioning scenes logic
     */
    enum Choice {
        NONE,
        PAUSE,
        SETTING,
        INFO,
        SETS,
        DISCARDED,
        DRAW_DISCARD,
        WIN,
        LOSE
    };
    /** Vector of tiles to add to discardUI*/
    std::vector<std::shared_ptr<TileSet::Tile>> discardedTiles;
    Choice _choice;
    std::shared_ptr<TileSet::Tile> discardDrawTile;
    std::vector<std::shared_ptr<TexturedNode>> _opponentHandTiles;
    std::vector<std::shared_ptr<TexturedNode>> _playerHandTiles;
    std::shared_ptr<SceneNode> _opponentHandRec;
    std::shared_ptr<SceneNode> _playerHandRec;
    int _remainingTiles;
    std::shared_ptr<Label> _remainingLabel;
    int opponentSetIndex = 0;
    int playerSetIndex = 0;
protected:
    /** Asset manager for this game mode */
    std::shared_ptr<cugl::AssetManager> _assets;
    /** The network connection (as made by this scene) */
    std::shared_ptr<NetworkController> _network;
    /** Input controller for player input*/
    std::shared_ptr<InputController> _input;
    /** Match controller for processing game logic */
    std::shared_ptr<MatchController> _matchController;
    /** JSON with all of our constants*/
    std::shared_ptr<cugl::JsonValue> _constants;
    /** Scene2 object for match scene */
    std::shared_ptr<cugl::scene2::SceneNode> _matchScene;
    /** Scene2 object for the pause scene */
    std::shared_ptr<cugl::scene2::SceneNode> _pauseScene;
    /** Reference to the discard UI node for the game */
    std::shared_ptr<DiscardUINode> _discardUINode;
    /** Reference to the pile UI node for the game */
    std::shared_ptr<PileUINode> _pileUINode;
    /** TileSet for the game */
    std::shared_ptr<TileSet> _tileSet;
    /** Reference to player */
    std::shared_ptr<Player> _player;
    /** Reference to tile pile */
    std::shared_ptr<Pile> _pile;
    /** Reference to the discard pile */
    std::shared_ptr<DiscardPile> _discardPile;
    /** Temporary discard area b/c no asset created for it yet */
    cugl::Rect discardArea;
    cugl::Rect _pileBox;
    /** Rect for pile */
    /** Pointer to the hand */
    Hand* _hand;
    std::shared_ptr<cugl::graphics::TextLayout> _win;
    std::shared_ptr<cugl::graphics::TextLayout> _lose;
    bool _gameWin;
    bool _gameLose;
    /** Whether this player is the host */
    bool _ishost;
    /** Whether we paused the game **/
    bool _paused;
    /** Whether we quit the game */
    bool _quit;
    /** Whether or not UI is active*/
    bool _uiopen = false;
    /** Button for discarding */
    std::shared_ptr<cugl::scene2::Button> _discardBtn;
    /** Button for transitioning to the tileset UI scene (discarded cards) */
    std::shared_ptr<cugl::scene2::Button> _tilesetUIBtn;
    /** Button for ending turn */
    std::shared_ptr<cugl::scene2::Button> _endTurnBtn;
    /**Button to transition to the setting scene**/
    std::shared_ptr<Button> _settingBtn;
    /**Button to transition to the info scene **/
    std::shared_ptr<Button> _infoBtn;
    
    /** Textured node to set the discarded tile image*/
    std::shared_ptr<cugl::scene2::TexturedNode> _discardedTileImage;
    
    std::shared_ptr<cugl::scene2::TexturedNode> _dragToDiscardNode;
    
    std::shared_ptr<cugl::scene2::TexturedNode> _dragToHandNode;
    std::shared_ptr<SceneNode> _playArea;
    std::shared_ptr<SceneNode> _tradeArea;
    bool _dragFromDiscard = false;
    bool _dragToHandVisible = false;
    
    /** Button for playing a set */
    std::shared_ptr<cugl::scene2::Button> _playSetBtn;
    /** Key for discard button listener */
    Uint32 _discardBtnKey;
    /** Key for tileset UI button listener */
    Uint32 _tilesetUIBtnKey;
    /** Key for pause button listener */
    Uint32 _pauseBtnKey;
    /** Key for continue button */
    Uint32 _continueBtnKey;
    /** Vector of scene nodes representing labels in the tileset UI table */
    std::vector<std::shared_ptr<cugl::scene2::Label>> _labels;
    /** Reference to scene node for UI scene */
    std::shared_ptr<scene2::SceneNode> _tilesetui;
    /** Button to exit the discard UI */
    std::shared_ptr<scene2::Button> _backBtn;
    /** Key for the listener for the back button for discard UI */
    Uint32 _backBtnKey;
    /** Holds reference to chow sprite node */
    std::shared_ptr<cugl::scene2::SpriteNode> _chowSheet;
    /** Holds reference to pong sprite node */
    std::shared_ptr<cugl::scene2::SpriteNode> _pongSheet;
    /** Holds reference to turn sprite node */
    std::shared_ptr<cugl::scene2::SpriteNode> _turnSheet;
    float _frameTimer = 0.0f;
    float _frameDelay = 0.2f;
    
    /** The tile currently being dragged */
    cugl::Vec2 _dragOffset;
    
    /** The rectangle representing the discrad pile's position*/
    cugl::Rect _discardBox;
    
    /** The rectangle representing the active play/discard area for all tiles*/
    cugl::Rect _activeRegion;
    
    /** The rectangle representing the discarded tile widget */
    cugl::Rect _discardedTileRegion;
    
    /** The rectangle reprsenting the player hand region */
    cugl::Rect _playerHandRegion;
    
    std::shared_ptr<TileSet::Tile> _draggingTile = nullptr;
    int _dragonRow = -1;
    
    cugl::Vec2 _dragStartPos;
    bool _dragInitiated = false;
    const float DRAG_THRESHOLD = 0.0f;
    
    cugl::Vec2 _originalTilePos = cugl::Vec2::ZERO;
    bool shouldReturn = true;
    
    std::shared_ptr<AnimatedNode> _actionAnimNode;
    
    bool _waitingForTileSelection = false;
    std::shared_ptr<TileSet::Tile> discardedTileSaved;
    bool _selectedThree = false;
    
    std::shared_ptr<Button> _opponentHandBtn;
    std::shared_ptr<Button> _playerHandBtn;
    std::shared_ptr<Button> _opponentHandBtn2;
    std::shared_ptr<Button> _playerHandBtn2;
    
    bool opponentTabVisible = false;
    bool playerTabVisible = false;
    
    std::vector<std::string> playerGuideKeys;
    std::unordered_map<std::string, std::shared_ptr<SceneNode>> playerGuideNodeMap;
    int framesOnScreen = 0;
    int maxFramesOnScreen = 180;
    
    // Field to track the time left in the active turn
    float _turnTimeRemaining;
    const float TURN_DURATION = 45.0f;
    bool turnTimerActive = false;
    int prevTurnId = -99;
    std::shared_ptr<Label> _timer;
    
    bool _wasPlayAreaVisible = false;
    bool _wasDragToHandVisible = false;
    bool _wasDragToDiscardVisible = false;
    bool _wasTradeTileVisible = false;
public:
#pragma mark -
#pragma mark Constructors
    /**
     * Temp queue for incoming deserialized data packets
     */
    std::queue<std::vector<std::string>> dataQueue;
    
    /**
     * Creates a new game mode with the default values
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer
     */
    GameScene() : cugl::scene2::Scene2() {}
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose() override;
    
    /**
     * Initializes the controller contents, and starts the game
     *
     * The constructor does not allocate any objects or memory. This allows
     * us to have a non-pointer reference to this controller, reducing our memory
     * allocation. Instead, allocation happens in this method
     *
     * @param assets    the asset manager for the game
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<NetworkController>& network, std::shared_ptr<InputController>& inputController);
    
    /**
     * Sets whether the player is host.
     *
     * We may need to have gameplay specific code for host.
     *
     * @param host  Whether the player is host.
     */
    void setHost(bool host)  { _ishost = host; }
    
    /**
     * Returns true if the player quits the game.
     * @return true if the player quits the game.
     */
    bool didQuit() const { return _quit; }
    
    /**
     * Disconnects this scene from the network controller.
     *
     * Technically, this method does not actually disconnect the network controller.
     * Since the network controller is a smart pointer, it is only fully disconnected
     * when ALL scenes have been disconnected.
     */
    void disconnect() { _network = nullptr; }
    
    
#pragma mark -
#pragma mark Gameplay Handling
    /**
     * Returns choice of this game scene
     */
    Choice getChoice(){
        return _choice;
    }
    /**
     * Rests the status of the game so we can play again.
     */
    void reset() override;
    
    /**
     * The method called to update the game mode
     *
     * @param timestep The amount of time (in seconds) since the last frame
     */
    void update(float timestep) override;
    
    /**
     * Draws all this scene to the scene's SpriteBatch.
     */
    void render() override;
    
    virtual void setActive(bool value) override;
    
    void setGameActive(bool value);
    
    void render(std::shared_ptr<cugl::graphics::SpriteBatch>& batch);
    
    void processData(std::vector<std::string> msg);
    /**
     * Applies the effects of the given type of celestial tile.
     */
    void applyCelestial(TileSet::Tile::Rank type);
    
    /**
     * Checks whether or not a tile has been clicked and sets selected status accordingly
     */
    void clickedTile(cugl::Vec2 mousePos);
    
    /**
     * Method to init the sprite sheets for the game scene
     *
     * @return true if initialization of sprite sheets was successful, and false otherwise
     */
    bool initSheets();
    
    /**
     * Method to get the index of this tile's associated label in the discard UI vector of labels
     *
     * @param tile  the tile whose label we need in _labels
     * @return an int representing the index of this tile's discard UI label
     */
    int getLabelIndex(std::shared_ptr<TileSet::Tile> tile);
    
    /**
     * Method to increment discard UI label corresponding to tile passed as argument
     *
     * @param tile  the tile to increment in the discard UI
     * @return true if update was successful, and false otherwise
     */
    void incrementLabel(std::shared_ptr<TileSet::Tile> tile);
    
    /**
     * Method to decrement discard UI label corresponding to tile passed as argument
     *
     * @param tile  the tile to increment in the discard UI
     * @return true if update was successful, and false otherwise
     */
    bool decrementLabel(std::shared_ptr<TileSet::Tile> tile);
    
    /**
     * Method to detect the tiles which are being pressed by a user in a mobile version of the game.
     */
    void pressTile();
    
    void dragTile();
    
    void releaseTile();
    
    void updateDrag(const cugl::Vec2& mousePos, bool mouseDown, bool mouseReleased, float timestep);
    
    void discardTile(std::shared_ptr<TileSet::Tile> tile);
    
    
    void playSetAnim(const std::vector<std::shared_ptr<TileSet::Tile>>& tiles);
    
    bool isPong(const std::vector<std::shared_ptr<TileSet::Tile>>& tiles);
    
    bool isChow(const std::vector<std::shared_ptr<TileSet::Tile>>& tiles);
    
    void updateSpriteNode(std::shared_ptr<SpriteNode>& sheetNode){
        if (sheetNode->getFrame() >= sheetNode->getCount() - 1){
            sheetNode->setFrame(0);
        } else {
            sheetNode->setFrame(sheetNode->getFrame() + 1);
        }
        return;
    }
    
    void updateSpriteNodes(float timestep){
        _frameTimer += timestep;  // Accumulate time
        if (_frameTimer >= _frameDelay) {
            _frameTimer = 0; // Reset timer
            //            updateSpriteNode(_pongSheet);
            //            updateSpriteNode(_chowSheet);
            updateSpriteNode(_turnSheet);
        }
    }
    
    void revertDiscardedTile();
    
    void initTurnIndicators(){
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
    void updateTurnIndicators(){
        if (_network->getCurrentTurn() == _network->getLocalPid()){
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
    
    void initPlayerGuide(){
        playerGuideKeys = {
            "discard-or-play-to-end",
            "discard-to-end",
            "invalid",
            "valid",
            "must-discard-play",
            "must-draw-discard",
            "must-draw-play",
            "not-your-turn",
            "start-your-turn1",
            "start-your-turn2",
            "drew-try-play",
            "drew-try-discard",
            "pig-fail"
        };
        
        for (auto key : playerGuideKeys){
            playerGuideNodeMap[key] = _assets->get<SceneNode>("matchscene.gameplayscene." + key);
        }
    }
    
    void updatePlayerGuide(){
        for (auto key : playerGuideKeys){
            auto node = playerGuideNodeMap[key];
            if (node->isVisible()){
                framesOnScreen++;
                if (framesOnScreen > maxFramesOnScreen){
                    node->setVisible(false);
                }
            }
        }
    }
    
    void showPlayerGuide(std::string key){
        for (auto key : playerGuideKeys){
            auto node = playerGuideNodeMap[key];
            if (node->isVisible()){
                node->setVisible(false);
            }
        }
        auto node = playerGuideNodeMap[key];
        node->setVisible(true);
        framesOnScreen = 0;
    }
    
    void displayOpponentSets(){
        int i = 0;
        for (auto set : _player->getHand().opponentPlayedSets){
            set = _player->getHand().getSortedTiles(set);
            for (auto tile : set){
                auto node = _opponentHandTiles[i];
                node->setTexture(tile->getTileTexture());
                node->setContentSize(30, 38.46f);
                node->doLayout();
                i++;
            }
        }
    }
    
    void displayPlayerSets(){
        int i = 0;
        for (auto set : _player->getHand()._playedSets){
            set = _player->getHand().getSortedTiles(set);
            for (auto tile : set){
                auto node = _playerHandTiles[i];
                node->setTexture(tile->getTileTexture());
                node->setContentSize(30, 38.46f);
                node->doLayout();
                i++;
            }
        }
    }
    
    std::shared_ptr<TileSet::Tile> getTileAtPosition(const cugl::Vec2& mousePos, std::vector<std::shared_ptr<TileSet::Tile>> tiles) {
        for (const auto& tile : tiles) {
            if (tile && tile->tileRect.contains(mousePos)) {
                return tile;
            }
        }
        return nullptr;
    }
    
    int getIndexAtPosition(const Vec2& mousePos, const std::vector<std::shared_ptr<TileSet::Tile>>& tiles) {
        for (int i = 0; i < (int)tiles.size(); ++i) {
            if (tiles[i] && tiles[i]->tileRect.contains(mousePos)) {
                return i;
            }
        }
        return -1;
    }
    
    void endTurnFromTimeout();
    
    void updateAreaVisibility(Vec2 mousePos, float timestep);
    
    void updateTurnTimer(float timestep){
        int currTurn = _network->getCurrentTurn();
        if (currTurn != prevTurnId){
            prevTurnId = currTurn;
            
            if (currTurn == _network->getLocalPid()){
                _turnTimeRemaining = TURN_DURATION;
                turnTimerActive = true;
            } else {
                turnTimerActive = false;
                _turnTimeRemaining = 0.0f;
                _timer->setText("00:00");
            }
            
        }
        
        if (turnTimerActive) {
            _turnTimeRemaining -= timestep;
            if (_turnTimeRemaining <= 0.0f){
                _turnTimeRemaining = 0.0f;
                turnTimerActive = false;
                endTurnFromTimeout();
            }
            
            int sec = static_cast<int>(_turnTimeRemaining);
            
            std::string timeAsStr = "00:";
            if (sec < 10){
                timeAsStr += "0";
            }
            
            timeAsStr += std::to_string(sec);
            _timer->setText(timeAsStr);
        }
    }
    
};

#endif /* __MJ_GAME_SCENE_H__ */
