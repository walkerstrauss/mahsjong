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
#include <queue>
#include <string>
#include "MJInputController.h"
#include "MJTileSet.h"
#include "MJPlayer.h"
#include "MJPile.h"
#include "MJDiscardPile.h"
#include "MJDiscardUIScene.h"
#include "MJNetworkController.h"
#include "MJAudioController.h"
#include "MJAnimationController.h"
#include "MJMatchController.h"


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
        TILESET,
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
protected:
    /** Asset manager for this game mode */
    std::shared_ptr<cugl::AssetManager> _assets;
    /** The network connection (as made by this scene) */
    std::shared_ptr<NetworkController> _network;
    /** Input controller for player input*/
    InputController _input;
    /** Match controller for processing game logic */
    MatchController _matchController; 
    /** JSON with all of our constants*/
    std::shared_ptr<cugl::JsonValue> _constants;
    /** Scene2 object for match scene */
    std::shared_ptr<cugl::scene2::SceneNode> _matchScene;
    /** Scene2 object for the pause scene */
    std::shared_ptr<cugl::scene2::SceneNode> _pauseScene;
    /** Reference to the discard UI scene for the game */
    std::shared_ptr<DiscardUIScene> _discardUIScene;
    /** TileSet for the game */
    std::shared_ptr<TileSet> _tileSet;
    /** Reference to player */
    std::shared_ptr<Player> _player;
    /** Reference to tile pile */
    std::shared_ptr<Pile> _pile;
    /** Reference to the discard pile */
    std::shared_ptr<DiscardPile> _discardPile;
    /** Reference to texture for gma text*/
    std::shared_ptr<cugl::graphics::Texture> _gmaLabelTexture;
    /** Text layout */
    std::shared_ptr<cugl::graphics::TextLayout> _text;

    /** Temporary discard area b/c no asset created for it yet */
    cugl::Rect discardArea;
    /** Reference to the audio controller */
//    std::shared_ptr<AudioController> _audio;
    
    Hand* _hand; // pointer to the hand.
    
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
    /** Button for pausing */
    std::shared_ptr<cugl::scene2::Button> _pauseBtn;
    /** Button for continuing (in the pause scene) */
    std::shared_ptr<cugl::scene2::Button> _continueBtn;
    /** Button to set to win scene (for debugging) */
    std::shared_ptr<cugl::scene2::Button> _winBtn;
    /** Button to set to defeat scene (for debugging) */
    std::shared_ptr<cugl::scene2::Button> _defeatBtn;
    /** Button for ending turn */
    std::shared_ptr<cugl::scene2::Button> _endTurnBtn;
    
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
    /** Holds reference to chow sprite sheet */
    std::shared_ptr<cugl::graphics::SpriteSheet> _chowSheet;
    /** Holds reference to kong sprite sheet */
    std::shared_ptr<cugl::graphics::SpriteSheet> _kongSheet;
    /** Holds reference to pong sprite sheet */
    std::shared_ptr<cugl::graphics::SpriteSheet> _pongSheet;
    /** Holds reference to win sprite sheet */
    std::shared_ptr<cugl::graphics::SpriteSheet> _winSheet;
    /** Holds reference to back pick sprite sheet */
    std::shared_ptr<cugl::graphics::SpriteSheet> _backPickSheet;
    /** Holds reference to empty discard sprite sheet */
    std::shared_ptr<cugl::graphics::SpriteSheet> _emptyDiscardSheet;
    /** Holds reference to empty pick sprite sheet */
    std::shared_ptr<cugl::graphics::SpriteSheet> _emptyPickSheet;
    /** Holds reference to empty pick flip sprite sheet */
    std::shared_ptr<cugl::graphics::SpriteSheet> _emptyPickFlipSheet;
    
    /** The tile currently being dragged */
    cugl::Vec2 _dragOffset;
    
    /** The rectangle representing the pile's position used for selection handling */
    cugl::Rect _pileBox;
    
    /** The rectangle representing the discrad pile's position*/
    cugl::Rect _discardBox;

    /** The rectangle representing the active play/discard area for all tiles*/
    cugl::Rect _activeRegion;

    std::shared_ptr<TileSet::Tile> _draggingTile = nullptr;
    cugl::Vec2 _dragStartPos;
    bool _dragInitiated = false;
    const float DRAG_THRESHOLD = 0.0f;

    cugl::Vec2 _originalTilePos = cugl::Vec2::ZERO;
    bool shouldReturn = true;
    
    bool _waitingForTileSelection = false;
    std::shared_ptr<TileSet::Tile> discardedTileSaved;
    bool _selectedThree = false;


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
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<NetworkController> network, MatchController& matchController);
    
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
     bool incrementLabel(std::shared_ptr<TileSet::Tile> tile);
     
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

    void updateDrag(const cugl::Vec2& mousePos, bool mouseDown, bool mouseReleased);
    
    void discardTile(std::shared_ptr<TileSet::Tile> tile);
    
    
    void revertDiscardedTile();
    
};

#endif /* __MJ_GAME_SCENE_H__ */
