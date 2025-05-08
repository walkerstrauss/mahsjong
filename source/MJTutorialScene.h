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

/** Class representing the tutorial scene for the game */
class TutorialScene : public Scene2 {
public:
    enum Choice {
        NONE,
        INFO,
        SETTING,
        DISCARD_UI,
        DREW,
        DISCARDED,
        DRAWNDISCARD,
        DONE,
        BACK
    };
    
    std::vector<std::shared_ptr<TileSet::Tile>> discardedTiles;
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
    int _currentTurn = 0;
    std::shared_ptr<AssetManager> _assets;
    std::shared_ptr<SceneNode> _tutorialScene;
    std::shared_ptr<DiscardUINode> _discardUINode;
    std::shared_ptr<InputController> _input;
    Choice _choice;
    /** TileSet for the game */
    std::shared_ptr<TileSet> _tileSet;
    /** Reference to the player */
    std::shared_ptr<Player> _player1;
    /** Reference to the bot player*/
    std::shared_ptr<Player> _player2;
    /** Reference to tile pile */
    std::shared_ptr<Pile> _pile;
    /** Reference to the discard pile */
    std::shared_ptr<DiscardPile> _discardPile;
    /** Temporary discard area b/c no asset created for it yet */
    cugl::Rect discardArea;
    cugl::Rect _pileBox;
    /**Button to transition to the setting scene**/
    std::shared_ptr<Button> _settingBtn;
    /**Button to transition to the info scene **/
    std::shared_ptr<Button> _infoBtn;
    /** Button for transitioning to the tileset UI scene (discarded cards) */
    std::shared_ptr<cugl::scene2::Button> _tilesetUIBtn;
    
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
    
    /** Vector of scene nodes representing labels in the tileset UI table */
    std::vector<std::shared_ptr<cugl::scene2::Label>> _labels;
    /** Reference to scene node for UI scene */
    std::shared_ptr<scene2::SceneNode> _tilesetui;
    /** Button to exit the discard UI */
    std::shared_ptr<scene2::Button> _backBtn;
    
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
    
    float _botDelay = 2.0f;
public:
#pragma mark Constructors
    TutorialScene() : Scene2(){}
    
    bool init(const std::shared_ptr<AssetManager>& assets, std::shared_ptr<InputController>& inputController);

    Choice getChoice() const { return _choice; }
        
#pragma mark Gameplay Handling
    void update(float timestep) override;
    
    void updateDrag(const cugl::Vec2& mousePos, bool mouseDown, bool mouseReleased, float timestep);
    
    void clickedTile(Vec2 mousePos);
    
    void dragTile();
    
    void releaseTile();
    
    void endTurn();
    
    void resetTurn();
    
    virtual void setActive(bool value) override;
    
    void setTutorialActive(bool value);
    
    void dispose() override;
    
    void render() override;
    
    void initTurnIndicators();
    
    void updateTurnIndicators();
    
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
    
    bool discardTile(std::shared_ptr<TileSet::Tile>);
    
    bool drawDiscard();
    
    bool playCelestial(std::shared_ptr<TileSet::Tile>);
    
    bool drawTile();
    
    bool playSet();
    
    void initTileData(){
        _tileSet->deck.clear();
        
        // Init p1 tiles
        _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::ONE, TileSet::Tile::Suit::BAMBOO));
        _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::ONE, TileSet::Tile::Suit::BAMBOO));
         _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::TWO, TileSet::Tile::Suit::BAMBOO));
        _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::TWO, TileSet::Tile::Suit::BAMBOO));
        _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::FIVE, TileSet::Tile::Suit::CRAK));
        _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::FIVE, TileSet::Tile::Suit::CRAK));
        _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::SIX, TileSet::Tile::Suit::CRAK));
        _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::SIX, TileSet::Tile::Suit::CRAK));
        _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::EIGHT, TileSet::Tile::Suit::DOT));
         _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::EIGHT,TileSet::Tile::Suit::DOT));
        _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::NINE, TileSet::Tile::Suit::DOT));
        _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::NINE, TileSet::Tile::Suit::DOT));
        _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::ONE, TileSet::Tile::Suit::DOT));
        
        // Init p2 tiles
        _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::ONE, TileSet::Tile::Suit::BAMBOO));
        _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::TWO, TileSet::Tile::Suit::BAMBOO));
        _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::THREE, TileSet::Tile::Suit::BAMBOO));
        _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::THREE, TileSet::Tile::Suit::BAMBOO));
        _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::THREE, TileSet::Tile::Suit::BAMBOO));
        _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::FIVE, TileSet::Tile::Suit::CRAK));
        _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::SIX, TileSet::Tile::Suit::CRAK));
        _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::SEVEN, TileSet::Tile::Suit::CRAK));
        _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::SEVEN, TileSet::Tile::Suit::CRAK));
        _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::NINE, TileSet::Tile::Suit::DOT));
        _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::SEVEN, TileSet::Tile::Suit::DOT));
        _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::EIGHT, TileSet::Tile::Suit::DOT));
        _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::TWO, TileSet::Tile::Suit::DOT));
        
        // Init pile tiles
        _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::TWO, TileSet::Tile::Suit::DOT));
        _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::TWO, TileSet::Tile::Suit::DOT));
        _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::ROOSTER, TileSet::Tile::Suit::CELESTIAL));
        _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::TWO, TileSet::Tile::Suit::DOT));
        _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::TWO, TileSet::Tile::Suit::DOT));
        _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::TWO, TileSet::Tile::Suit::DOT));
        _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::TWO, TileSet::Tile::Suit::DOT));
         _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::TWO, TileSet::Tile::Suit::DOT));
        _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::TWO, TileSet::Tile::Suit::DOT));
        _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::TWO, TileSet::Tile::Suit::DOT));
        _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::TWO, TileSet::Tile::Suit::DOT));
        _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::TWO, TileSet::Tile::Suit::DOT));
        _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::TWO, TileSet::Tile::Suit::DOT));
        _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::TWO, TileSet::Tile::Suit::DOT));
        _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::TWO, TileSet::Tile::Suit::DOT));
        _tileSet->deck.push_back(std::make_shared<TileSet::Tile>(TileSet::Tile::Rank::TWO, TileSet::Tile::Suit::DOT));
        
        for (auto& tile : _tileSet->deck){
            _tileSet->tileMap[std::to_string(tile->_id)] = tile;
        }
        
        _tileSet->initTileNodes(_assets);
        _tileSet->setAllTileTexture(_assets);
        
        _player1->getHand().initHand(_tileSet, true);
        _player2->getHand().initHand(_tileSet, false);
        _pile->initPile(4, _tileSet, true);
    }
};
    
#endif
