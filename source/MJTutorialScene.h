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
        START,
        ONE_DRAW,
        DISCARD,
        ONE_OPP,
        TWO_DRAW,
        CELESTIAL,
        TWO_OPP,
        DRAW_DIS,
        PLAY_SET,
        SET_DISCARD,
        FINISHED
    };
    
    /** Current scene choice */
    Choice _choice;
    
    /** The current tutorial phase */
    TutorialPhase _phase;
    
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
    
    const float DRAG_THRESHOLD = 0.0f;
    
#pragma mark Celestial Tiles
    int _dragonRow = -1;
    
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
    
};
    
#endif
