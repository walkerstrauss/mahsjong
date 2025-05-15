//
//  MJTutorialScene.h
//  Mahsjong
//
//  Created by Walker Strauss on 4/22/25.
//

#ifndef __MJ_TUTORIAL_SCENE_H__
#define __MJ_TUTORIAL_SCENE_H__
#include <cugl/cugl.h>
#include "MJInputController.h"
#include "MJAudioController.h"
#include "MJTileSet.h"
#include "MJPlayer.h"
#include "MJPile.h"
#include "MJDiscardPile.h"
#include "MJDiscardUINode.h"
#include "MJAnimationController.h"
#include "unordered_map"

using namespace cugl;
using namespace cugl::scene2;
using namespace cugl::graphics;

/** Class representing the tutorial scene */
class TutorialScene : public cugl::scene2::Scene2 {
public:
    /**
     * Enum representing the player's choice when in the tutorial scene
     * for app transition scenes logic
     */
    enum Choice {
        NONE,
        PAUSE,
        SETTING,
        INFO,
        SETS,
        DISCARDED,
        DRAW_DISCARD,
        DONE,
        BACK,
        DISCARD_UI
    };
    
    /**
     * Enum representing the current choice the player must make
     */
    enum TutorialPhase {
        NOT,
        START,
        WELCOME,
        ONE_DRAW,
        DISCARD,
        ONE_OPP,
        ONE_END,
        TWO_DRAW,
        CELESTIAL,
        TWO_OPP,
        TWO_END,
        DRAW_DIS,
        PLAY_SET,
        SET_DISCARD,
        SHUFFLE,
        FINISHED,
        PRESS
    };
    
    /** Current scene choice */
    Choice _choice;
    
    /** The current tutorial phase */
    TutorialPhase _phase;
    
    /** The prev phase*/
    TutorialPhase _prevPhase;
    
    /** Returns choice of this game scene */
    Choice getChoice() { return _choice; }
    
private:
#pragma mark App objects
    /** Asset manager for this game mode */
    std::shared_ptr<AssetManager> _assets;
    /** Input controller for player input */
    std::shared_ptr<InputController> _input;
    /** Network controller */
    std::shared_ptr<NetworkController> _network; 
#pragma mark Scene Objects
    /** JSON with all of our constants */
    std::shared_ptr<JsonValue> _constants;
    /** Reference to the discardUI node for the game */
    std::shared_ptr<DiscardUINode> _discardUINode;
    /** Reference to the pileUI node for the game */
    std::shared_ptr<PileUINode> _pileUINode;
    /** Scene2 object for the match scene */
    std::shared_ptr<SceneNode> _matchScene;
    /** Scene2 object for the pause scene */
    std::shared_ptr<SceneNode> _pauseScene;
    /** Reference to the play area */
    std::shared_ptr<SceneNode> _playArea;
    /** Reference to the trade area */
    std::shared_ptr<SceneNode> _tradeArea;
    /** Reference to opponent hand tab node */
    std::shared_ptr<SceneNode> _opponentHandRec;
    /** Reference to the player hand tab node */
    std::shared_ptr<SceneNode> _playerHandRec;
    /** TexturedNodes for gameplay options */
    std::shared_ptr<TexturedNode> _discardedTileImage;
    std::shared_ptr<TexturedNode> _dragToDiscardNode;
    std::shared_ptr<TexturedNode> _dragToHandNode;
    
    
#pragma mark SceneNode Objects
    /** Buttons */
    std::shared_ptr<Button> _backBtn;
    std::shared_ptr<Button> _playSetBtn; 
    std::shared_ptr<Button> _tilesetUIBtn;
    std::shared_ptr<Button> _settingBtn;
    std::shared_ptr<Button> _infoBtn;
    std::shared_ptr<Button> _opponentHandBtn;
    std::shared_ptr<Button> _playerHandBtn;
    std::shared_ptr<Button> _opponentHandBtn2;
    std::shared_ptr<Button> _playerHandBtn2;
    bool opponentTabVisible = false;
    bool playerTabVisible = false;
    
    /** Rect areas */
    Rect _activeRegion;
    Rect _discardedTileRegion;
    Rect _playerHandRegion;
    Rect _pileBox; 
    
    /** Tile vectors */
    std::vector<std::shared_ptr<TexturedNode>> _opponentHandTiles;
    std::vector<std::shared_ptr<TexturedNode>> _playerHandTiles;
    
    /** UI elements */
    std::vector<std::shared_ptr<SceneNode>> _uiElements;
    std::shared_ptr<SceneNode> _shade;
    
#pragma mark SceneNode button keys
    Uint32 _tilesetUIBtnKey;
    Uint32 _pauseBtnKey;
    Uint32 _backBtnKey;
    
#pragma mark Gameplay Objects
    /** TileSet for the game */
    std::shared_ptr<TileSet> _tileSet;
    /** Reference to the player */
    std::shared_ptr<Player> _player;
    /** Reference to the pile */
    std::shared_ptr<Pile> _pile;
    /** Reference to the discard pile */
    std::shared_ptr<DiscardPile> _discardPile;
    /** Reference to the match controller */
    std::shared_ptr<MatchController> _matchController;
    
    /** Drag objects */
    std::shared_ptr<TileSet::Tile> _draggingTile = nullptr;
    
    Vec2 _dragOffset;
    Vec2 _dragStartPos;
    Vec2 _originalTilePos = Vec2::ZERO;
    
    bool _dragInitiated = false;
    bool _dragFromDiscard = false;
    bool _wasDragToHandVisible = false;
    bool _wasDragToDiscardVisible = false;
    bool _wasPlayAreaVisible = false;
    bool shouldReturn = false;
    
    std::shared_ptr<TileSet::Tile> _opponentTile1;
    std::shared_ptr<TileSet::Tile> _opponentTile2;
    
    const float DRAG_THRESHOLD = 0.0f;
    
#pragma mark Delay Timers
    float _opponentDelay = 2.0f;
    float _turnTimer = 0.0f;
    float _discardDelay = 2.0f;
    float _discardTimer = 0.0f;
    float _shuffleDelay = 4.0f;
    float _shuffleTimer = 0.0f;
    float _welcomeTimer = 0.0f;
    float _welcomeDelay = 1.0f;
    float _finishedTimer = 0.0f;
    float _finishedDelay = 1.0f;
    
#pragma mark Celestial Tiles
    int _dragonRow = -1;
    
#pragma mark Tutorial UI Nodes
    // Map to hold lists of scene nodes for each phase of the tutorial
    std::unordered_map<std::string, std::vector<std::shared_ptr<SceneNode>>> _uiMap;
    
    // Vectors to hold nodes for each phase
    std::vector<std::shared_ptr<SceneNode>> _pileDrawPhaseNodes;
    std::vector<std::shared_ptr<SceneNode>> _discardPhaseNodes;
    std::vector<std::shared_ptr<SceneNode>> _waitPhaseNodes;
    std::vector<std::shared_ptr<SceneNode>> _drawDiscardPhaseNodes;
    std::vector<std::shared_ptr<SceneNode>> _pressPhaseNodes;
    std::vector<std::shared_ptr<SceneNode>> _playSetPhaseNodes;
    std::vector<std::shared_ptr<SceneNode>> _playCelestialNodes;
    
    // Nodes for pile draw phase
    std::shared_ptr<SceneNode> _box;
    std::shared_ptr<SceneNode> _drawPileArrow;
    std::shared_ptr<SceneNode> _drawPileText;
    
    // Nodes for discard phase
    std::shared_ptr<SceneNode> _discardArrow;
    std::shared_ptr<SceneNode> _discardText;
    
    // Nodes for wait for opponent phase
    std::shared_ptr<SceneNode> _waitArrow;
    std::shared_ptr<SceneNode> _waitText;
    
    // Nodes for draw discard phase
    std::shared_ptr<SceneNode> _drawDiscardArrow;
    std::shared_ptr<SceneNode> _drawDiscardText;
    
    // Nodes for select two tiles for playing set phase
    std::shared_ptr<SceneNode> _pressArrow1;
    std::shared_ptr<SceneNode> _pressArrow2;
    std::shared_ptr<SceneNode> _pressText;
    
    // Nodes for play set phase
    std::shared_ptr<SceneNode> _playSetArrow;
    std::shared_ptr<SceneNode> _playSetText;
    
    // Nodes for play celestial phase
    std::shared_ptr<SceneNode> _playCelestialArrow;
    std::shared_ptr<SceneNode> _playCelestialText;
        
public:
#pragma mark -
#pragma mark Gameplay Handling
    /**
     * Creates a new game mode with the default values
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer.
     */
    TutorialScene() : cugl::scene2::Scene2() {}
    
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
     * Disposes of all (non-static) resources allocated to this node.
     */
    void dispose() override;
    
    /**
     * Rests the status of the game so we can play again.
     */
    void reset() override { return; }
    
    /**
      * The method called to update the game mode
      *
      * @param timestep The amount of time (in seconds) since the last frame
      */
     void update(float timestep) override;
    
    /**
     * Draws all this scene to the scene's SpriteBatch
     */
    void render() override;
    
    /**
     * Sets the current active scene as active or not active
     */
    virtual void setActive(bool value) override;
    
    /**
     * Sets if the current gameplay is active or not.
     */
    void setTutorialActive(bool value);
    
    /**
     * The main dragging function for this scene. Checks if the dragging tile
     * if a celestial or normal and, on release, executes the appropriate action. Also handles
     * rearranging hand and pile tiles.
     */
    void updateDrag(const cugl::Vec2& mousePos, bool mouseDown, bool mouseReleased, float timestep);
    
    /**
     * Updates the visibility of regions
     */
    void updateAreaVisibility(Vec2 mousePos, float timestep);
    
    /** Releases the current dragging tile */
    void releaseTile() {
        if (_draggingTile) {
            _draggingTile->pressed = false;
            _draggingTile = nullptr;
        }
    }
    
    /**
     * Fetches the inde of the tile at the current mouse position 
     */
    int getIndexAtPosition(const Vec2& mousePos, const std::vector<std::shared_ptr<TileSet::Tile>>& tiles) {
        for (int i = 0; i < (int)tiles.size(); ++i) {
            if (tiles[i] && tiles[i]->tileRect.contains(mousePos)) {
                return i;
            }
        }
        return -1;
    }
    
    /**
     * Fetches tile at the current mouse position within the player hand
     */
    std::shared_ptr<TileSet::Tile> getTileAtPosition(const cugl::Vec2& mousePos, std::vector<std::shared_ptr<TileSet::Tile>> tiles) {
        for (const auto& tile : tiles) {
            if (tile && tile->tileRect.contains(mousePos)) {
                return tile;
            }
        }
        return nullptr;
    }
    
    /** Shows the opponent sets */
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
    
    /** Shows player sets */
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
    
    /**
     * Checks whether or not a tile has been clicked and sets selected status accordingly
     */
    void clickedTile(cugl::Vec2 mousePos);
    
    void initTurnIndicators(){
        _opponentHandRec = _assets->get<SceneNode>("tutorialscene.gameplayscene.opponent-hand-rec");
        _opponentHandBtn = std::dynamic_pointer_cast<Button>(_assets->get<SceneNode>("tutorialscene.gameplayscene.opponent-hand"));
        _opponentHandBtn->addListener([this](const std::string& name, bool down){
            if (!down){
                opponentTabVisible = !opponentTabVisible;
            }
        });
        _opponentHandBtn2 = std::dynamic_pointer_cast<Button>(_assets->get<SceneNode>("tutorialscene.gameplayscene.opponent-hand2"));
        _opponentHandBtn2->addListener([this](const std::string& name, bool down){
            if (!down){
                opponentTabVisible = !opponentTabVisible;
            }
        });
        
        _playerHandRec = _assets->get<SceneNode>("tutorialscene.gameplayscene.player-hand-rec");
        _playerHandBtn = std::dynamic_pointer_cast<Button>(_assets->get<SceneNode>("tutorialscene.gameplayscene.playerhand-button"));
        _playerHandBtn->addListener([this](const std::string& name, bool down){
            if (!down){
                AudioController::getInstance().playSound("Select");
                playerTabVisible = !playerTabVisible;
            }
        });
        _playerHandBtn2 = std::dynamic_pointer_cast<Button>(_assets->get<SceneNode>("tutorialscene.gameplayscene.playerhand-button2"));
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
    
    void initTutorialUINodes(){
        std::shared_ptr<SceneNode> shade = _assets->get<SceneNode>("tutorialscene.gameplayscene.welcome-shade");
        std::shared_ptr<SceneNode> section = _assets->get<SceneNode>("tutorialscene.gameplayscene.welcome-section");
//        section->setContentSize(1008,400);
//        section->setPosition(185,56);
//        std::shared_ptr<Label> label = std::dynamic_pointer_cast<Label>(_assets->get<SceneNode>("tutorialscene.gameplayscene.welcome-text"));
        std::vector<std::shared_ptr<SceneNode>> nodes = {shade, section /**, label*/};
        _uiMap["welcome"] = nodes;
        
        _box = _assets->get<SceneNode>("tutorialscene.gameplayscene.box");
        _drawPileArrow = _assets->get<SceneNode>("tutorialscene.gameplayscene.drawFromPileArrow");
        _drawPileText = _assets->get<SceneNode>("tutorialscene.gameplayscene.drawFromPileText");
        _pileDrawPhaseNodes.push_back(_box);
        _pileDrawPhaseNodes.push_back(_drawPileArrow);
        _pileDrawPhaseNodes.push_back(_drawPileText);
        _uiMap["pile"] = _pileDrawPhaseNodes;
        
        _discardArrow = _assets->get<SceneNode>("tutorialscene.gameplayscene.dragToDiscardArrow");
        _discardText = _assets->get<SceneNode>("tutorialscene.gameplayscene.dragToDiscardText");
        _discardPhaseNodes.push_back(_discardArrow);
        _discardPhaseNodes.push_back(_discardText);
        _uiMap["discard"] = _discardPhaseNodes;
        
        _waitArrow = _assets->get<SceneNode>("tutorialscene.gameplayscene.waitOpponentArrow");
        _waitText = _assets->get<SceneNode>("tutorialscene.gameplayscene.waitOpponentText");
        _waitPhaseNodes.push_back(_waitArrow);
        _waitPhaseNodes.push_back(_waitText);
        _uiMap["wait"] = _waitPhaseNodes;
        
        _drawDiscardArrow = _assets->get<SceneNode>("tutorialscene.gameplayscene.drawDiscardedArrow");
        _drawDiscardText = _assets->get<SceneNode>("tutorialscene.gameplayscene.drawDiscardedText");
        _drawDiscardPhaseNodes.push_back(_drawDiscardArrow);
        _drawDiscardPhaseNodes.push_back(_drawDiscardText);
        _uiMap["draw discard"] = _drawDiscardPhaseNodes;
        
        _pressArrow1 = _assets->get<SceneNode>("tutorialscene.gameplayscene.pressArrow");
        _pressArrow2 = _assets->get<SceneNode>("tutorialscene.gameplayscene.pressArrow2");
        _pressText = _assets->get<SceneNode>("tutorialscene.gameplayscene.pressText");
        _pressPhaseNodes.push_back(_pressArrow1);
        _pressPhaseNodes.push_back(_pressArrow2);
        _pressPhaseNodes.push_back(_pressText);
        _uiMap["press"] = _pressPhaseNodes;
        
        _playSetArrow = _assets->get<SceneNode>("tutorialscene.gameplayscene.playsetArrow");
        _playSetText = _assets->get<SceneNode>("tutorialscene.gameplayscene.playsetText");
        _playSetPhaseNodes.push_back(_playSetArrow);
        _playSetPhaseNodes.push_back(_playSetText);
        _uiMap["play set"] = _playSetPhaseNodes;
        
        _playCelestialArrow = _assets->get<SceneNode>("tutorialscene.gameplayscene.playCelestialTileArrow");
        _playCelestialText = _assets->get<SceneNode>("tutorialscene.gameplayscene.playCelestialTileText");
        _playCelestialNodes.push_back(_playCelestialArrow);
        _playCelestialNodes.push_back(_playCelestialText);
        _uiMap["celestial"] = _playCelestialNodes;
        
        std::shared_ptr<SceneNode> shade2 = _assets->get<SceneNode>("tutorialscene.gameplayscene.finished-shade");
        std::shared_ptr<SceneNode> section2 = _assets->get<SceneNode>("tutorialscene.gameplayscene.finished-section");
//        section2->setContentSize(1008,400);
//        section2->setPosition(185,56);
        
        std::vector<std::shared_ptr<SceneNode>> nodes2 = {shade2,section2};
        _uiMap["finished"] = nodes2;
        
        std::vector<std::shared_ptr<SceneNode>> none;
        _uiMap["none"] = none;
    }
    
    const std::string getPhaseUIMapKey(TutorialPhase phase){
        std::string s;
        switch (phase){
            case WELCOME:
                s = "welcome";
                break;
            case ONE_DRAW:
            case TWO_DRAW:
                s = "pile";
                break;
            case SET_DISCARD:
            case DISCARD:
                s = "discard";
                break;
            case ONE_OPP:
            case TWO_OPP:
            case ONE_END:
            case TWO_END:
                s = "wait";
                break;
            case DRAW_DIS:
                s = "draw discard";
                break;
            case PLAY_SET:
                s = "play set";
                break;
            case CELESTIAL:
                s = "celestial";
                break;
            case PRESS:
                s = "press";
                break;
            case FINISHED:
                s = "finished";
                break;
            default:
                s = "none";
                break;
        }
        return s;
    }
    
    void setPhase(TutorialPhase phase){
        if (_phase == phase) return;
        
        for (auto& node : _uiMap[getPhaseUIMapKey(_phase)]){
            node->setVisible(false);
        }
        
        _prevPhase = _phase;
        _phase = phase;
        
        for (auto& node : _uiMap[getPhaseUIMapKey(phase)]){
            node->setVisible(true);
        }
    }
    
};
    
#endif
